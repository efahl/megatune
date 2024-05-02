//------------------------------------------------------------------------------
//--  Copyright (c) 2004,2005,2006 by Eric Fahlgren                           --
//--  All Rights Reserved.                                                    --
//--                                                                          --
//--  This program is free software; you can redistribute it and/or           --
//--  modify it under the terms of the GNU General Public License             --
//--  as published by the Free Software Foundation; either version            --
//--  2 of the License, or (at your option) any later version.                --
//--  See http://www.gnu.org/licenses/gpl.txt                                 --
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "msDatabase.h"
#include "megatune.h"

extern msDatabase mdb;

#include "parser.h"

//------------------------------------------------------------------------------

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>

#include <map>
#include <vector>

//------------------------------------------------------------------------------

static FILE *Outfile   = NULL;
static bool  Debugging = false;

static void _DiagPrint(const char *Format, ...)
{
   va_list Args;
#define FILE_OUT
#ifdef FILE_OUT
   static char Output[512] = { '\0' };
   if (Outfile == NULL) Outfile = fopen("expr.dmp", "w");
#endif

   va_start(Args, Format);
#ifdef FILE_OUT
      vfprintf(Outfile, Format, Args);
#else
      vsprintf(&Output[strlen(Output)], Format, Args);
      if (Output[strlen(Output)-1] == '\n') {
         Output[strlen(Output)-1] = '\0';
         command_insert(Output);
         strcpy(Output, "");
      }
#endif
   va_end(Args);
}

// Use this as it avoids unneeded evaluation of arguments and runs lots faster.
#define DiagPrint if(Debugging)_DiagPrint

//------------------------------------------------------------------------------
//--  Error Handler                                                           --

void exprError::setFile (const char *f) { file = f;         }
void exprError::setTitle(const char *t) { strcpy(title, t); }
void exprError::setLine (int         l) { lineNumber = l;   }

void exprError::reset(char  c) { tok[0] = c; tok[1] = '\0'; } 
void exprError::reset(char *s) { strcpy(tok, s); }

const char *exprError::errorString() { return str; }

char exprError::append(char c)
{
   int len    = strlen(tok);
   tok[len]   = c;
   tok[len+1] = '\0';
   return c;
}

int exprError::backup()
{
   int len = strlen(tok);
   if (len) tok[len-1] = '\0';
   return len;
}

void exprError::Error(const char *Msg)
{
   found = true;
   if (lineNumber == 0)
      sprintf(str, "Error: %s '%s'", Msg, tok);
   else
      sprintf(str, "%s:%d: %s '%s'", file?file:"???", lineNumber, Msg, tok);
   throw *this;
}

void exprError::display()  {
#ifdef STANDALONE
#     define outStr str
#else
   char outStr[512];
   sprintf(outStr, "%s\n\nTerminate MegaTune?", str);
#endif
   if (MessageBoxA(NULL, outStr, title, MB_YESNO | MB_APPLMODAL) == IDYES) {
      exit(1);
   }
}

//------------------------------------------------------------------------------
//--  Evaluation stack, needed here by function evaluator.                    --

struct stackEntry {
   typedef enum { intV, dblV, strV } entType;
   entType t;
   double  d;
   int     i;
   char   *s;
};

struct stack {
   exprError &e;
   int           SP;
   stackEntry    s[50];

   stack(exprError &ec) : e(ec), SP(0) { }

   inline int    topI() { return s[SP-1].i; }
   inline double topD() { return s[SP-1].d; }
   inline char  *topS() { return s[SP-1].s; }

   inline void toDouble(int idx) {
      switch (s[idx].t) {
         case stackEntry::dblV:
            break;
         case stackEntry::intV:
            s[idx].d = s[idx].i;
            s[idx].t = stackEntry::dblV;
            break;
         default:
            e.Error("Can't convert string to double");
            break;
      }
   }

   inline void push(double d) {
      s[SP].d = d;
      s[SP].t = stackEntry::dblV;
      SP++;
   }
   inline void push(int i) {
      s[SP].i = i;
      s[SP].t = stackEntry::intV;
      SP++;
   }
   void push(char *t) {
      s[SP].s = t;
      s[SP].t = stackEntry::strV;
      SP++;
   }
   void pop(char **t) {
      SP--;
      if (s[SP].t != stackEntry::strV) e.Error("Can't pop a string");
      *t = s[SP].s;
   }
   inline void pop(int &i) {
      SP--;
      switch (s[SP].t) {
         case stackEntry::dblV: i = int(s[SP].d); break;
         case stackEntry::intV: i =     s[SP].i;  break;
         default: e.Error("Can't pop an integer");
      }
   }
   inline void pop(double &d) {
      SP--;
      switch (s[SP].t) {
         case stackEntry::dblV: d = s[SP].d; break;
         case stackEntry::intV: d = s[SP].i; break;
         default: e.Error("Can't pop a double");
      }
   }
   typedef double (*unaryFunc)(double);
   typedef double (*binaryFunc)(double,double);
   inline void apply(unaryFunc  f) {
      toDouble(SP-1);
      s[SP-1].d = f(s[SP-1].d);
   }
   inline void apply(binaryFunc f) {
      SP--;
      toDouble(SP-1);
      toDouble(SP  );
      s[SP-1].d = f(s[SP-1].d, s[SP].d);
   }

   inline bool topIs(stackEntry::entType t) { return s[SP-1].t  == t; }
};

//------------------------------------------------------------------------------
//--  To add a new function, write an ev_function and then register it in     --
//--  the functions constructor.  Search for "acos" and you'll see the two    --
//--  places that you need to add code.                                       --

struct funcInfo {
   typedef void (*funcEv)(stack &s);

   enum { notaFunc = -1 };
   static int nextId;

   typedef enum {
      nonArg,
      strArg,
      intArg,
      dblArg
   } argType;

   char     name[128];
   funcEv   fn;
   int      id;
   int     nArgs;
   argType *args;

   funcInfo() : fn(NULL), id(notaFunc), nArgs(0) { memset(name, 0, sizeof(name)); }

   funcInfo(const char *pname, funcEv pfn, int pnArgs)
    : fn   (pfn),
      id   (nextId++),
      nArgs(pnArgs)
   {
      strcpy(name, pname);
      args = new argType[nArgs];
   }

   void addArg(int iArg, argType at) { args[iArg] = at; }

   void eval(stack &s) { if (fn) fn(s); else throw name; }
};

int funcInfo::nextId = funcInfo::notaFunc+1;

void ev_PI   (stack &s) { s.push(3.14159265358979323846264338328E+0); }
void ev_acos (stack &s) { s.apply(acos ); }
void ev_asin (stack &s) { s.apply(asin ); }
void ev_atan (stack &s) { s.apply(atan ); }
void ev_ceil (stack &s) { s.apply(ceil ); }
void ev_cos  (stack &s) { s.apply(cos  ); }
void ev_exp  (stack &s) { s.apply(exp  ); }
void ev_fabs (stack &s) { s.apply(fabs ); }
void ev_floor(stack &s) { s.apply(floor); }
void ev_log  (stack &s) { s.apply(log  ); }
void ev_log10(stack &s) { s.apply(log10); }
void ev_sin  (stack &s) { s.apply(sin  ); }
void ev_sqrt (stack &s) { s.apply(sqrt ); }
void ev_tan  (stack &s) { s.apply(tan  ); }

void ev_atan2(stack &s) { s.apply(atan2); }
void ev_fmod (stack &s) { s.apply(fmod ); }
void ev_pow  (stack &s) { s.apply(pow  ); }

struct xferFunc { // MS Tables
   const char *fileName;
   WORD       *values;
   int        nValues;

   int operator [](int idx) {
      if (values == NULL || nValues <= 0) return 0;
      if (idx <  0                      ) idx = 0;
      if (idx >= nValues                ) idx = nValues-1;
      return values[idx];
   }

   int idx(int value) {
      for (int i = 0; i < nValues-1; i++) {
         if (values[i] <= value && value <= values[i+1] && values[i] != values[i+1]) return i;
      }
      return nValues-1;
   }

   xferFunc() : fileName(NULL), values(NULL), nValues(0) { }
   xferFunc(const char *fn) {
      fileName = fn;
      mdb.readTable(fileName, &values, nValues);
   }

  ~xferFunc() {
     //if (nValues) delete [] values;
  }
};

#include <map>

struct ltstr {
   bool operator()(const char* s1, const char* s2) const {
      return strcmp(s1, s2) < 0;
   }
};

typedef std::map<const char *, xferFunc, ltstr> tableMap;
typedef tableMap::iterator                      tableMapIter;

tableMap tables;

void ev_table(stack &s) // Table lookup.
{
   int   i1;
   char *s2;
   s.pop(&s2);
   s.pop( i1);
   tableMapIter tbl = tables.find(s2);
   if (tbl != tables.end()) {
      s.push((*tbl).second[i1]);
   }
   else {
      tables[s2] = xferFunc(s2);
      s.push(tables[s2][i1]);
   }
}

void ev_itable(stack &s) // Inverse table lookup, returns index of value.
{
   int   i1;
   char *s2;
   s.pop(&s2);
   s.pop( i1);
   tableMapIter tbl = tables.find(s2);
   if (tbl != tables.end()) {
      s.push((*tbl).second.idx(i1));
   }
   else {
      tables[s2] = xferFunc(s2);
      s.push(tables[s2].idx(i1));
   }
}

//------------------------------------------------------------------------------

#include "Vex.h"

struct vexFunc { // VEX Tables
   const char *fileName;
   vex         v;

   int interpolate(int rpm, int lod, int pag)
   {
      if (pag < 0 || pag >= v.pageCount()) return 0;
      return v.page(pag)->interpolate(rpm, lod);
   }

   vexFunc() : fileName(NULL), v(vex()) { }
   vexFunc(const char *fn) : v(vex()) {
      fileName = fn;
      vex::errorStatus status = v.read(fn);
      if (status != vex::noError)
         throw CString("Could not read ")+fn+"\n"+v.errorMsg(status);
   }

  ~vexFunc() { }
};

typedef std::map<const char *, vexFunc*, ltstr> vexTableMap;
typedef vexTableMap::iterator                   vexTableMapIter;

vexTableMap vexTables;

void ev_vexInterp(stack &s)
{
   // double = vexInterp(rpm, load, page, "file.vex")
   int i1, i2, i3;
   char *s4;
   s.pop(&s4);
   s.pop( i3);
   s.pop( i2);
   s.pop( i1);
   vexTableMapIter tbl = vexTables.find(s4);
   if (tbl != vexTables.end()) {
      s.push((*tbl).second->interpolate(i1, i2, i3));
   }
   else {
      vexTables[s4] = new vexFunc(s4);
      s.push(vexTables[s4]->interpolate(i1, i2, i3));
   }
}

void ev_tempCvt(stack &s) {
   double t;
   s.pop(t);
#ifdef STANDALONE
   s.push(2);
#else
   s.push(mdb.tempFromDb(t));
#endif
}

bool   fakingTime = false;
double fakeTime = 0.0;
double timeNow();
void ev_timeNow(stack &s) { s.push(fakingTime ? fakeTime : timeNow()); }

#if 0
void ev_const(stack &s) { // Fetch constant data.
   int addr;
   s.pop(addr);
#ifdef STANDALONE
   s.push(1);
#else
   s.push(mdb.ConstRaw(addr));
#endif
}
#endif

//------------------------------------------------------------------------------

class functions {
   typedef std::map<const char*, funcInfo, ltstr> funcMap;
   typedef funcMap::iterator                      funcMapIter;

   funcMap f;
   std::vector<funcInfo *> idList;

   void addOne(const char *name, funcInfo::funcEv fn, int nArgs, ...)
   {
      f[name] = funcInfo(name, fn, nArgs);
      idList.push_back(&f[name]);
      funcMapIter fi = f.find(name);
      va_list args;
      va_start(args, nArgs);
         for (int iArg = 0; iArg < nArgs; iArg++) {
            (*fi).second.addArg(iArg, static_cast<funcInfo::argType>(va_arg(args, int)));
         }
      va_end(args);
   }

public:
   functions() {
      addOne("PI",        ev_PI,        0);
      addOne("acos",      ev_acos,      1, funcInfo::dblArg);
      addOne("asin",      ev_asin,      1, funcInfo::dblArg);
      addOne("atan",      ev_atan,      1, funcInfo::dblArg);
      addOne("atan2",     ev_atan2,     2, funcInfo::dblArg, funcInfo::dblArg);
      addOne("ceil",      ev_ceil,      1, funcInfo::dblArg);
//    addOne("const",     ev_const,     1, funcInfo::intArg);
      addOne("cos",       ev_cos,       1, funcInfo::dblArg);
      addOne("exp",       ev_exp,       1, funcInfo::dblArg);
      addOne("fabs",      ev_fabs,      1, funcInfo::dblArg);
      addOne("floor",     ev_floor,     1, funcInfo::dblArg);
      addOne("fmod",      ev_fmod,      2, funcInfo::dblArg, funcInfo::dblArg);
      addOne("itable",    ev_itable,    2, funcInfo::intArg, funcInfo::strArg);
      addOne("log",       ev_log,       1, funcInfo::dblArg);
      addOne("log10",     ev_log10,     1, funcInfo::dblArg);
      addOne("pow",       ev_pow,       2, funcInfo::dblArg, funcInfo::dblArg);
      addOne("sin",       ev_sin,       1, funcInfo::dblArg);
      addOne("sqrt",      ev_sqrt,      1, funcInfo::dblArg);
      addOne("table",     ev_table,     2, funcInfo::intArg, funcInfo::strArg);
      addOne("tan",       ev_tan,       1, funcInfo::dblArg);
      addOne("tempCvt",   ev_tempCvt,   1, funcInfo::dblArg);
      addOne("timeNow",   ev_timeNow,   0);
      addOne("vexInterp", ev_vexInterp, 4, funcInfo::intArg, funcInfo::intArg, funcInfo::intArg, funcInfo::strArg);

      (*f.end()).second = funcInfo();
   }

   // The following assume that (*f.end()).second returns a valid funcInfo
   // object, which I'll bet is not always the case...  It is now, see that
   // line just above?

// funcInfo::funcEv fn    (const char *name) { return (*f.find(name)).second.fn;    }
   int              id    (const char *name) { return (*f.find(name)).second.id;    }
   int              argCnt(const char *name) { return (*f.find(name)).second.nArgs; }

   funcInfo &fromId(int id) { return *idList[id]; }
};

static functions *func = NULL;

//------------------------------------------------------------------------------

static char *DecimalNumeric = "0123456789";
static char *BinaryNumeric  = "01";
static char *HexNumeric     = "0123456789abcdefABCDEF";
static char *AlphaNumeric   = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
static char *Alpha          = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";

#define AndChar       '&'
#define OrChar        '|'
#define PlusChar      '+'
#define QuoteChar     '"'
#define MinusChar     '-'
#define MultChar      '*'
#define ModChar       '%'
#define DivChar       '/'
#define LParenChar    '('
#define RParenChar    ')'
#define CommaChar     ','
#define EqualChar     '='
#define LtChar        '<'
#define GtChar        '>'
#define NotChar       '!'
#define QChar         '?'
#define ColonChar     ':'
#define CaretChar     '^'
#define TildeChar     '~'
#define TermChar      '\0'

class lexer {
public:
   typedef enum {
      StrTok,
      IntTok,
      DblTok,
      VariableTok,
      ConstantTok,
      FunctionTok,

      BAndTok,
      BOrTok,
      BXorTok,
      BNotTok,
      LAndTok,
      LOrTok,
      LNotTok,
      CondTok,
      ColonTok,
      PlusTok,
      MinusTok,
      MultTok,
      ModTok,
      DivTok,
      LParenTok,
      RParenTok,
      CommaTok,
      EqualTok,
      LShiftTok,
      RShiftTok,
      EqTok,
      NeTok,
      LtTok,
      LeTok,
      GtTok,
      GeTok,

      TermTok,

      ErrorTok
   } Token;

private:
   exprError &e;
   char          inputLine[128];
   int           nextChar;

public:
   Token   CurrentTok;
   int     CurrentFnc, fnArgCount;
   int     CurrentVar;
   symbol *CurrentSym;
   char    CurrentStr[128];
   int     CurrentInt;
   double  CurrentDbl;

private:

#  define CharIsIn(Char,Set) ((Char)?(char *)strchr((Set),(Char)):NULL)
#  define GETN_SKIP          (e.append(inputLine[nextChar++]))
#  define PREV_CHAR          (e.backup(),nextChar--)

   void GetSequence(char firstChar, const char *validChars, char *destStr)
   {
      char  inputChar = firstChar;
      int   inputLoc  = 0;
      char *itsThere  = CharIsIn(inputChar, validChars);
      while (itsThere) {
         destStr[inputLoc] = inputChar;
         inputLoc++;

         inputChar = inputLine[nextChar];
         if ((itsThere = CharIsIn(inputChar, validChars)) != NULL) GETN_SKIP;
      }

      destStr[inputLoc] = 0;
   }

   void GetUntil(char untilChar, char *destStr)
   {
      int   inputLoc  = 0;
      while (inputLine[nextChar] != untilChar) {
         destStr[inputLoc++] = GETN_SKIP;
      }
      destStr[inputLoc] = '\0';
      GETN_SKIP; // Past the closing one.
   }

   void GetNumber(char firstChar)
   {
      char  inputChar;
      char  numStr[100];
      char  newStr[100];
      char *end;
      int   base = 10;
      char *Numeric = DecimalNumeric;

      if (firstChar == '0') { // Allow for binary numbers with a variant on C's hex: 0b10101
         switch (inputLine[nextChar]) {
            case 'b': base =  2; GETN_SKIP; Numeric = BinaryNumeric; break;
            case 'x': base = 16; GETN_SKIP; Numeric = HexNumeric;    break;
         }
      }

      GetSequence(firstChar, Numeric, numStr);

      inputChar = inputLine[nextChar];
      if (!CharIsIn(inputChar, ".eE")) {
         //DiagPrint("   Integer value: >%s<\n", numStr);
         CurrentTok = IntTok;
         CurrentInt = strtol(numStr, &end, base);
         if (*end) e.Error("Numeric conversion error, value may have overflowed");
      }
      else {
         if (base != 10) e.Error("Floating point values may only be base 10");

         if (inputChar == '.') {
            strcat(numStr, ".");
            GETN_SKIP;
            inputChar = GETN_SKIP;

            GetSequence(inputChar, DecimalNumeric, newStr);
            if (*newStr)
               strcat(numStr, newStr);
            else
               PREV_CHAR;
         }
         if (strlen(numStr) == 1 && numStr[0] == '.') e.Error("Expected numeric digits after decimal");

         inputChar = GETN_SKIP;
         if (CharIsIn(inputChar, "Ee")) {
            strcat(numStr, "e");
            inputChar = GETN_SKIP;

            if (inputChar == '+' || inputChar == '-') {
               if (inputChar == '-') strcat(numStr, "-");
               inputChar = GETN_SKIP;
            }

            GetSequence(inputChar, DecimalNumeric, newStr);
            strcat(numStr, newStr);
            if (strlen(newStr) == 0) e.Error("Expected number for exponent");
            inputChar = GETN_SKIP;
         }

         PREV_CHAR;

         //DiagPrint("   Real value: >%s<\n", numStr);
         CurrentTok = DblTok;
         CurrentDbl = strtod(numStr, &end);
         if (*end) e.Error("Numeric conversion error, value may have overflowed");
      }
   }

public:

   lexer(exprError &ec) : e(ec) { }
   void Init(const char *Line)
   {
      strcpy(inputLine, Line);
      nextChar  = 0;
   }

   void GetToken()
   {
      while (isspace(inputLine[nextChar])) nextChar++;

      char InputChar = GETN_SKIP;
      e.reset(InputChar);

      if (CharIsIn(InputChar, Alpha)) {
         GetSequence(InputChar, AlphaNumeric, CurrentStr);
         // Search the constants table.  These first two have unique entries.
         // Search the function table.
         CurrentFnc = func->id(CurrentStr);
         if (CurrentFnc != funcInfo::notaFunc) {
            CurrentTok = FunctionTok;
            fnArgCount = func->argCnt(CurrentStr);
         }
         else {
            // Search the variables table in controller symbol table; if not there, error.
            CurrentVar = mdb.cDesc.varIndex(CurrentStr);
            if (CurrentVar != -1)
               CurrentTok = VariableTok;
            else {
               CurrentSym = mdb.cDesc.lookup(CurrentStr);
               if (CurrentSym == NULL) 
                  e.Error("Unknown identifier");
               CurrentTok = ConstantTok;
            }
         }
      }
      else if (CharIsIn(InputChar, DecimalNumeric)) {
         GetNumber(InputChar);
      }
      else {
         switch (InputChar) {
            case QuoteChar : GetUntil(QuoteChar, CurrentStr); CurrentTok = StrTok; break;
            case PlusChar  : CurrentTok = PlusTok;   break;
            case MinusChar : CurrentTok = MinusTok;  break;
            case MultChar  : CurrentTok = MultTok;   break;
            case ModChar   : CurrentTok = ModTok;    break;
            case DivChar   : CurrentTok = DivTok;    break;
            case LParenChar: CurrentTok = LParenTok; break;
            case RParenChar: CurrentTok = RParenTok; break;
            case CommaChar : CurrentTok = CommaTok;  break;
            case TermChar  : CurrentTok = TermTok;   break;
            case CaretChar : CurrentTok = BXorTok;   break;
            case TildeChar : CurrentTok = BNotTok;   break;
            case QChar     : CurrentTok = CondTok;   break;
            case ColonChar : CurrentTok = ColonTok;  break;
            case AndChar   :
               switch (inputLine[nextChar]) {
                  case AndChar   : CurrentTok = LAndTok;   GETN_SKIP; break;
                  default        : CurrentTok = BAndTok;              break;
               }
               break;
            case OrChar    :
               switch (inputLine[nextChar]) {
                  case OrChar    : CurrentTok = LOrTok;    GETN_SKIP; break;
                  default        : CurrentTok = BOrTok;               break;
               }
               break;
            case NotChar :
               switch (inputLine[nextChar]) {
                  case EqualChar : CurrentTok = NeTok;     GETN_SKIP; break;
                  default        : CurrentTok = LNotTok;              break;
               }
               break;
            case EqualChar :
               switch (inputLine[nextChar]) {
                  case EqualChar : CurrentTok = EqTok;     GETN_SKIP; break;
                  default        : CurrentTok = EqualTok;             break;
               }
               break;
            case LtChar    :
               switch (inputLine[nextChar]) {
                  case LtChar    : CurrentTok = LShiftTok; GETN_SKIP; break;
                  case EqualChar : CurrentTok = LeTok;     GETN_SKIP; break;
                  default        : CurrentTok = LtTok;                break;
               }
               break;
            case GtChar    :
               switch (inputLine[nextChar]) {
                  case GtChar    : CurrentTok = RShiftTok; GETN_SKIP; break;
                  case EqualChar : CurrentTok = GeTok;     GETN_SKIP; break;
                  default        : CurrentTok = GtTok;                break;
               }
               break;
            default        : e.Error("Invalid input token"); break;
         }
      }
   }
};

//------------------------------------------------------------------------------
//--  The Code Generator

class codeGen {
   exprError e;
   int         *Memory;
   int          sizeMem;
   int          nextMemLoc;
   int          checkPointLoc;

   void alloc(int nWords);
   void copy(const codeGen &from);

public:
   typedef enum {
      HALTop = 0, // Make sure this is zero, so we can init memory to all "HALTops" easily.
      DBGSop,
      LSHFop,
      RSHFop,
      PLUSop,
      MINUSop,
      MULTop,
      DIVop,
      MODop,
      NEGop,
      PUSHAop,
      PUSHIop,
      PUSHDop,
      PUSHSop,
      PUSHYop,
      POPAop,
      FUNCop,
      LTop,
      LEop,
      GTop,
      GEop,
      EQop,
      NEop,
      BANDop,
      BXORop,
      BORop,
      BNOTop,
      LNOTop,
      LANDop,
      LORop,
      BNEop,
      BRAop
   } op;

   codeGen() : Memory(NULL), sizeMem(0), nextMemLoc (0) {
      Gen1Op(HALTop);
      checkPointLoc = nextMemLoc;
   }
  ~codeGen() { if (Memory) delete [] Memory; }

   codeGen(const codeGen &from)
   {
      copy(from);
   }
   codeGen &operator= (const codeGen &from)
   {
      if (this == &from) return *this;
      copy(from);
      return *this;
   }

   //--------------------------------------------------------------------------
   //--  These two members allow you to append a new block of code, and if   --
   //--  errors are found during code generation, to back off any new code   --
   //--  that made its way into memory.                                      --
   void checkPoint() { checkPointLoc = nextMemLoc; }
   void recover   () { if (checkPointLoc > 0) nextMemLoc = checkPointLoc-1; Gen1Op(HALTop); }

   void append() {
      if (nextMemLoc > 0) {
         if (Memory[nextMemLoc-1] == HALTop) nextMemLoc--;
      }
   }

   void  Gen1Op(op theOperator);
   void  Gen2Op(op theOperator, int    theOperand);
   void  Gen2Op(op theOperator, double theOperand);
   void  Gen2Op(op theOperator, void  *theOperand);

   void  GenStr(op theOperator, const char *s) {
      char *ss = strdup(s);
      Gen1Op(theOperator);
      alloc(1);
      Memory[nextMemLoc++] = reinterpret_cast<int>(ss);
   }

   int   GenCond() {
      Gen1Op(BNEop);
      alloc(1);
      return nextMemLoc++;
   }
   int   GenBranch() {
      Gen1Op(BRAop);
      alloc(1);
      return nextMemLoc++;
   }
   void  Patch(int addr) {
      Memory[addr] = nextMemLoc;
   }

   void  Evaluate(double *);
};

//------------------------------------------------------------------------------

void codeGen::copy(const codeGen &from)
{
   checkPointLoc = from.checkPointLoc;
   sizeMem       = from.nextMemLoc; // Minimize wasted space.
   nextMemLoc    = from.nextMemLoc;
   Memory        = new int[nextMemLoc];
   memcpy(Memory, from.Memory, sizeMem*sizeof(int));
}

//------------------------------------------------------------------------------

void codeGen::alloc(int nWords)
{
   if (nextMemLoc+nWords >= sizeMem) {
      int *oldMem = Memory;
      sizeMem = nextMemLoc + nWords + 20;
      Memory = new int[sizeMem];
      memset(Memory, 0, sizeof(int)*sizeMem); // Fill it with "HALTop"
      memcpy(Memory, oldMem, nextMemLoc*sizeof(int));
      delete [] oldMem;
   }
}

//------------------------------------------------------------------------------

void codeGen::Gen1Op(op theOperator)
{
   alloc(1);
   Memory[nextMemLoc] = theOperator;
   nextMemLoc++;
}

//------------------------------------------------------------------------------

void codeGen::Gen2Op(op theOperator, int theOperand)
{
   alloc(2);
   Memory[nextMemLoc] = theOperator;
   nextMemLoc++;
   Memory[nextMemLoc] = theOperand;
   nextMemLoc++;
}

void codeGen::Gen2Op(op theOperator, double theOperand)
{
   alloc(3);
   Memory[nextMemLoc] = theOperator;
   nextMemLoc++;
   memcpy(Memory+nextMemLoc, &theOperand, sizeof(double));
   nextMemLoc+=2;
}

void codeGen::Gen2Op(op theOperator, void *theOperand)
{
   alloc(2);
   Memory[nextMemLoc] = theOperator;
   nextMemLoc++;
   Memory[nextMemLoc] = reinterpret_cast<int>(theOperand);
   nextMemLoc++;
}

//------------------------------------------------------------------------------

typedef int (*RelOp)(double, double);

int le(double d1, double d2) { return d1 <= d2; }
int lt(double d1, double d2) { return d1 <  d2; }
int ge(double d1, double d2) { return d1 >= d2; }
int gt(double d1, double d2) { return d1 >  d2; }
int eq(double d1, double d2) { return d1 == d2; }
int ne(double d1, double d2) { return d1 != d2; }

void codeGen::Evaluate(double *Variables)
{
   static int zeroDivs = 1; // Should be zero, fix it. ???
   RelOp  relOp;
   stack  Stack(e);
   int    IP    = 0;
   bool   Done  = false;
   double d1, d2, d3;
   int    i1, i2;
   char  *s1;

   e.setTitle("MegaTune Expression Evaluator");

   DiagPrint(" IP  SP\n");

   do {
      DiagPrint("%3d %3d  ", IP, Stack.SP);
      switch (Memory[IP]) {
         case DBGSop:
            IP++;
            DiagPrint("DBGS  {%s}\n", reinterpret_cast<char *>(Memory[IP]));
            e.reset(reinterpret_cast<char *>(Memory[IP]));
            IP++;
            break;

         case POPAop:
         {
            DiagPrint("POP   @%03d", Memory[IP+1]);
            IP++;
            double v;
            Stack.pop(v);
            Variables[Memory[IP]] = v;
            DiagPrint("  (r=%f)\n", v);
            if (_isnan(Variables[Memory[IP]])) e.Error("Operation produced not-a-number");
            IP++;
         }
         break;

         case LNOTop:
            DiagPrint("LNOT\n");
            IP++;
            Stack.pop ( i1);
            Stack.push(!i1);
            break;

         case BNOTop:
            DiagPrint("BNOT\n");
            IP++;
            Stack.pop ( i1);
            Stack.push(~i1);
            break;

         case NEGop:
            DiagPrint("NEG\n");
            IP++;
            Stack.pop ( d1);
            Stack.push(-d1);
            break;

         case LSHFop:
            DiagPrint("LSHF\n");
            IP++;
            Stack.pop(i2);
            Stack.pop(i1);
            Stack.push(i1 << i2);
            break;

         case RSHFop:
            DiagPrint("RSHF\n");
            IP++;
            Stack.pop(i2);
            Stack.pop(i1);
            Stack.push(i1 >> i2);
            break;

         case PLUSop:
            DiagPrint("PLUS\n");
            IP++;
            Stack.pop(d2);
            Stack.pop(d1);
            Stack.push(d1+d2);
            break;

         case MINUSop:
            DiagPrint("MINUS\n");
            IP++;
            Stack.pop(d2);
            Stack.pop(d1);
            Stack.push(d1-d2);
            break;

         case MULTop:
            DiagPrint("MULT\n");
            IP++;
            Stack.pop(d2);
            Stack.pop(d1);
            Stack.push(d1*d2);
            break;

         case DIVop:
            DiagPrint("DIV\n");
            IP++;
            Stack.pop(d2);
            Stack.pop(d1);
            if (fabs(d2) > 1e-10)
              d3 = d1/d2;
            else {
               zeroDivs++;
               if (zeroDivs == 1) e.Error("Divide by zero.");
               d3 = 1e6;
            }
            Stack.push(d3);
            break;

         case MODop:
            DiagPrint("MOD\n");
            IP++;
            Stack.pop(i2);
            Stack.pop(i1);
            Stack.push(i1%i2);
            break;

         case LTop: relOp = lt; DiagPrint("LT\n"); goto relCmp;
         case LEop: relOp = le; DiagPrint("LE\n"); goto relCmp;
         case GTop: relOp = gt; DiagPrint("GT\n"); goto relCmp;
         case GEop: relOp = ge; DiagPrint("GE\n"); goto relCmp;
         case EQop: relOp = eq; DiagPrint("EQ\n"); goto relCmp;
         case NEop: relOp = ne; DiagPrint("NE\n"); goto relCmp;
         relCmp:
            IP++;
            Stack.pop(d2);
            Stack.pop(d1);
            Stack.push(relOp(d1, d2));
            break;

         case BANDop:
            DiagPrint("BAND\n");
            IP++;
            Stack.pop(i2);
            Stack.pop(i1);
            Stack.push(i1 & i2);
            break;

         case BXORop:
            DiagPrint("BXOR\n");
            IP++;
            Stack.pop(i2);
            Stack.pop(i1);
            Stack.push(i1 ^ i2);
            break;

         case BORop:
            DiagPrint("BOR\n");
            IP++;
            Stack.pop(i2);
            Stack.pop(i1);
            Stack.push(i1 | i2);
            break;

         case LANDop:
            DiagPrint("LAND\n");
            IP++;
            Stack.pop(i2);
            Stack.pop(i1);
            Stack.push(i1 && i2);
            break;

         case LORop:
            DiagPrint("LOR\n");
            IP++;
            Stack.pop(i2);
            Stack.pop(i1);
            Stack.push(i1 || i2);
            break;

         case HALTop:
            DiagPrint("HALT\n");
            Done = true;
            break;

         case PUSHAop:
            DiagPrint("PUSH  @%03d", Memory[IP+1]);
            IP++;
            Stack.push(Variables[Memory[IP]]);
            DiagPrint("  (r=%f)\n", Stack.topD());
            IP++;
            break;

         case PUSHIop:
            DiagPrint("PUSHI %1d\n", Memory[IP+1]);
            IP++;
            Stack.push(Memory[IP]);
            IP++;
            break;

         case PUSHDop:
            IP++;
            Stack.push(*reinterpret_cast<double *>(Memory+IP));
            DiagPrint("PUSHD %f\n", Stack.topD());
            IP+=2;
            break;

         case PUSHSop:
            IP++;
            Stack.push(reinterpret_cast<char *>(Memory[IP]));
            DiagPrint("PUSHS >%s<\n", Stack.topS());
            IP++;
            break;

         case PUSHYop: // Symbol reference
            IP++;
            Stack.push(reinterpret_cast<symbol *>(Memory[IP])->valueUser());
            DiagPrint("PUSHY %f (%s)\n", Stack.topD(), reinterpret_cast<symbol *>(Memory[IP])->name());
            IP++;
            break;

         case BNEop:
            DiagPrint("BNE\n");
            IP += 2;
            if (Stack.topIs(stackEntry::strV)) {
               Stack.pop(&s1);
               if (s1[0] == 0) IP =  Memory[IP-1];
            }
            else {
            Stack.pop(d1);
               if (d1 == 0.0) IP = Memory[IP-1];
            }
            break;

         case BRAop:
            DiagPrint("BRA\n");
            IP = Memory[IP+1];
            break;

         case FUNCop:
         {
            DiagPrint("FUNC  %3d  (%s)\n", Memory[IP+1], func->fromId(Memory[IP+1]).name);
            IP++;
            try {
               func->fromId(Memory[IP]).eval(Stack);
            }
            catch (char *name) {
               e.reset(name);
               e.Error("Internal error: invalid function");
            }
            catch (CString msg) {
               e.Error(msg);
            }
            IP++;
         }
         break;

         default:
            e.reset(char(Memory[IP])); // ???
            e.Error("Internal error: invalid op code");
            break;
      }
   } while (!Done);

   if (Stack.SP != 0) {
      e.reset('?');
      e.Error("Internal error: stack screwed up");
   }
}

//==============================================================================

class parser {
   exprError e;
   lexer        l;
   codeGen     &c;

   void Primitive       ();
   void ArithFactor     ();
   void ArithTerm       ();
   void ArithExpression ();
   void Shift           ();
   void Relation        ();
   void BitFactor       ();
   void BitTerm         ();
   void LogFactor       ();
   void LogTerm         ();
   void Expression      ();
   void Assignment      ();

public:
   parser(const char *expr, codeGen &cg, const char *file, int lineNo)
    : l(e),
      c(cg)
   {
      e.setFile (file);
      e.setLine (lineNo);
      e.setTitle("MegaTune 'ini-file' Parsing Error");
      l.Init(expr);
      c.append();
      c.GenStr(codeGen::DBGSop, expr);
   }

   void parse();
   void parse(int destAddr);
};

//------------------------------------------------------------------------------

void parser::Primitive()
{
   int TheFunction;
   int nArgs;

   lexer::Token Operator = lexer::ErrorTok;
   if (l.CurrentTok == lexer::LNotTok || l.CurrentTok == lexer::MinusTok || l.CurrentTok == lexer::BNotTok) {
      Operator = l.CurrentTok;
      l.GetToken();
   }

   switch (l.CurrentTok) {
      case lexer::VariableTok:
         c.Gen2Op(codeGen::PUSHAop, l.CurrentVar);
         l.GetToken();
         break;

      case lexer::IntTok:
         c.Gen2Op(codeGen::PUSHIop, l.CurrentInt);
         l.GetToken();
         break;

      case lexer::DblTok:
         c.Gen2Op(codeGen::PUSHDop, l.CurrentDbl);
         l.GetToken();
         break;

      case lexer::StrTok:
         c.GenStr(codeGen::PUSHSop, l.CurrentStr);
         l.GetToken();
         break;

      case lexer::ConstantTok:
         c.Gen2Op(codeGen::PUSHYop, l.CurrentSym);
         l.GetToken();
         break;

      case lexer::FunctionTok:
      {
         TheFunction = l.CurrentFnc;
         nArgs       = l.fnArgCount;
         l.GetToken();                         // Scan past the function name

         for (int iArg = 0; iArg < nArgs; iArg++) {
            if (iArg == 0      && l.CurrentTok != lexer::LParenTok) e.Error("Expected left parenthesis after function");
            l.GetToken();                       // Scan past left paren or comma
            Expression();                       // Parse the expression
            if (iArg < nArgs-1 && l.CurrentTok != lexer::CommaTok ) e.Error("Expected comma after function argument");
         }

         if (nArgs > 0) {
            if (l.CurrentTok != lexer::RParenTok) e.Error("Expected right parenthesis");
            l.GetToken();                          // Eat the right paren
         }

         c.Gen2Op(codeGen::FUNCop, TheFunction);
      }
      break;

      default:
         e.Error("Invalid primitive");
         break;
   }

   if (Operator != lexer::ErrorTok) {
      if (Operator == lexer::MinusTok) c.Gen1Op(codeGen::NEGop);
      if (Operator == lexer::LNotTok ) c.Gen1Op(codeGen::LNOTop);
      if (Operator == lexer::BNotTok ) c.Gen1Op(codeGen::BNOTop);
   }
}

//------------------------------------------------------------------------------

void parser::ArithFactor()
{
   if (l.CurrentTok != lexer::LParenTok) {
      Primitive();
   }
   else {
      l.GetToken();                                   // Scan past left paren
      Expression();                                   // Parse the expression
      if (l.CurrentTok != lexer::RParenTok) {
         e.Error("Expected right parenthesis");
      }
      else {
         l.GetToken();                                 // Eat the right paren
      }
   }
}

//------------------------------------------------------------------------------

void parser::ArithTerm()
{
   ArithFactor();
   while (l.CurrentTok == lexer::MultTok || l.CurrentTok == lexer::DivTok || l.CurrentTok == lexer::ModTok) {
      lexer::Token Operator = l.CurrentTok;
      l.GetToken();                      // Scan past multiplicative operator
      ArithFactor();
      switch (Operator) {
         case lexer::MultTok: c.Gen1Op(codeGen::MULTop); break;
         case lexer::DivTok : c.Gen1Op(codeGen::DIVop);  break;
         case lexer::ModTok : c.Gen1Op(codeGen::MODop);  break;
         default: e.Error("Invalid arithmetic multiplicative operator");
      }
   }
}

//------------------------------------------------------------------------------

void parser::ArithExpression()
{
   ArithTerm();
   while (l.CurrentTok == lexer::PlusTok || l.CurrentTok == lexer::MinusTok) {
      lexer::Token Operator = l.CurrentTok;
      l.GetToken();                               // Scan past operator token
      ArithTerm();
      if (Operator == lexer::PlusTok) c.Gen1Op(codeGen::PLUSop);
      else                            c.Gen1Op(codeGen::MINUSop);
   }
}

//------------------------------------------------------------------------------

void parser::Shift()
{
   ArithExpression();
   while (l.CurrentTok == lexer::LShiftTok || l.CurrentTok == lexer::RShiftTok) {
      lexer::Token Operator = l.CurrentTok;
      l.GetToken();
      ArithExpression();
      if (Operator == lexer::LShiftTok) c.Gen1Op(codeGen::LSHFop);
      else                              c.Gen1Op(codeGen::RSHFop);
   }
}

//------------------------------------------------------------------------------

void parser::Relation()
{
   Shift();
   while (l.CurrentTok == lexer::LtTok
       || l.CurrentTok == lexer::LeTok
       || l.CurrentTok == lexer::GtTok
       || l.CurrentTok == lexer::GeTok
       || l.CurrentTok == lexer::EqTok
       || l.CurrentTok == lexer::NeTok) {
      lexer::Token Operator = l.CurrentTok;
      l.GetToken();
      Shift();
      switch(Operator) {
         case lexer::LtTok: c.Gen1Op(codeGen::LTop); break;
         case lexer::LeTok: c.Gen1Op(codeGen::LEop); break;
         case lexer::GtTok: c.Gen1Op(codeGen::GTop); break;
         case lexer::GeTok: c.Gen1Op(codeGen::GEop); break;
         case lexer::EqTok: c.Gen1Op(codeGen::EQop); break;
         case lexer::NeTok: c.Gen1Op(codeGen::NEop); break;
         default: e.Error("Invalid relational operator");
      }
   }
}

//------------------------------------------------------------------------------

void parser::BitFactor()
{
   Relation();
   while (l.CurrentTok == lexer::BAndTok || l.CurrentTok == lexer::BXorTok) {
      lexer::Token Operator = l.CurrentTok;
      l.GetToken();
      Relation();
      if (Operator == lexer::BAndTok) c.Gen1Op(codeGen::BANDop);
      else                            c.Gen1Op(codeGen::BXORop);
   }
}

//------------------------------------------------------------------------------

void parser::BitTerm()
{
   BitFactor();
   while (l.CurrentTok == lexer::BOrTok) {
      l.GetToken();
      BitFactor();
      c.Gen1Op(codeGen::BORop);
   }
}

//------------------------------------------------------------------------------

void parser::LogFactor()
{
   BitTerm();
   while (l.CurrentTok == lexer::LAndTok) {
      l.GetToken();
      BitTerm();
      c.Gen1Op(codeGen::LANDop); // Should be a short-circuit operator ???
   }
}

//------------------------------------------------------------------------------

void parser::LogTerm()
{
   LogFactor();
   while (l.CurrentTok == lexer::LOrTok) {
      l.GetToken();
      LogFactor();
      c.Gen1Op(codeGen::LORop); // Should be a short-circuit operator ???
   }
}

//------------------------------------------------------------------------------

void parser::Expression()
{
  // ?: conditional
  LogTerm();
  if (l.CurrentTok == lexer::CondTok) {
     l.GetToken(); // Past the '?'
     int patchC = c.GenCond();
     Expression();
     if (l.CurrentTok != lexer::ColonTok) e.Error("Expected : in conditional operator");
     l.GetToken(); // Past the ':'
     int patchB = c.GenBranch();
     c.Patch(patchC);
     Expression();
     c.Patch(patchB);
  }
}

//------------------------------------------------------------------------------

void parser::Assignment()
{
   if (l.CurrentTok != lexer::VariableTok) e.Error("Expected variable on left of assigment");
   int variable = l.CurrentVar;
   l.GetToken();
   if (l.CurrentTok != lexer::EqualTok   ) e.Error("Expected assignment operator");
   l.GetToken();
   Expression();
   c.Gen2Op(codeGen::POPAop, variable);
}

//------------------------------------------------------------------------------

void parser::parse()
{
   l.GetToken();
   Assignment();
   if (l.CurrentTok != lexer::TermTok) e.Error("Extra stuff after expression");
   c.Gen1Op(codeGen::HALTop);
}

//------------------------------------------------------------------------------

void parser::parse(int destAddr)
{
   l.GetToken();
   Expression();
   if (l.CurrentTok != lexer::TermTok) e.Error("Extra stuff after expression");
   c.Gen2Op(codeGen::POPAop, destAddr);
   c.Gen1Op(codeGen::HALTop);
}

//------------------------------------------------------------------------------

#ifdef STANDALONE
static codeGen *code = NULL;
void Compute(const char *ex)
{
   static int var    = 0;
   static int lineNo = 1;
   try {
      code->checkPoint();
      parser p(ex, *code, lineNo++);
      p.parse(var++);
   }
   catch (exprError e) {
      var--;
      code->recover();
      e.display();
   }
}

int main(int argc, char **argv)
{
   int nIters = 1;
   if (argc > 1) { nIters = strtol(argv[1], NULL, 10); printf("%d\n", nIters); }
   if (code == NULL) code = new codeGen();
   if (func == NULL) func = new functions();
   char inputLine[128];
   while (true) {
      printf("Expression:\n");
      fgets(inputLine, sizeof(inputLine), stdin);
      if (inputLine[0] == '\n') break;
      *strchr(inputLine, '\n') = '\0';
      Compute(inputLine);
   }

   double Variables[200];
   memset(Variables, 0, sizeof(Variables));
   for (int i = 0; i < nIters; i++) {
      try   { code->Evaluate(Variables); }
      catch (exprError e) { e.display(); }
   }

   return 0;
}
#else

//------------------------------------------------------------------------------

expression::expression()
 : _och(NULL)
{
   if (func == NULL) func = new functions();
   _code = new codeGen();
}

expression::~expression()
{
   delete func;
   delete _code;
}

//------------------------------------------------------------------------------

void expression::setOutputBuffer(double *och)
{
   _och = och;
}

//------------------------------------------------------------------------------

void expression::addExpr(int destVar, const char *expr, const char *file, int lineNo)
{
   try {
      DiagPrint("Adding %3d %s\n", destVar, expr);
      _code->checkPoint();
      parser p(expr, *_code, file, lineNo);
      p.parse(destVar);
   }
   catch (exprError e) {
      _code->recover();
      e.display();
   }
}

//------------------------------------------------------------------------------

void expression::recalc()
{
   if (_och == NULL) return;
   static bool inEval = false;
   if (!inEval) {
      inEval = true;
      try   { _code->Evaluate(_och); }
      catch (exprError e) { e.display(); }
      inEval = false;
   }
}

//------------------------------------------------------------------------------
#endif
