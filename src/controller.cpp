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

static char *rcsId = "$Id$";

#include "stdafx.h"
#include "megatune.h"
#include "msDatabase.h"
#include "controller.h"
#include "parser.h"
#include "repository.h" // For VERSION_STRING, move that to version.h or whatever.

#include "miniDOM.h"
#include "units.h"

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------

static byteString xlate(const char *s) {
   return byteString(s, strlen(s)).xlate(); // Translate \nnn into chars and all that.
}

//------------------------------------------------------------------------------

static void bigEndIt(BYTE *dw, int nBytes)
{
   BYTE  sb;
   switch (nBytes) {
      case 1:
         // Do nothing, first byte already holds value.
         break;
      case 2:
         // Swap first two bytes.
         sb = dw[0]; dw[0] = dw[1]; dw[1] = sb;
         break;
      case 4:
         // Reverse all four bytes.
         sb = dw[0]; dw[0] = dw[3]; dw[3] = sb;
         sb = dw[1]; dw[1] = dw[2]; dw[2] = sb;
         break;
   }
}

//------------------------------------------------------------------------------

static void cpDWord(DWORD d, int nBytes, byteString &bs, int ofs)
{
   BYTE *dw = reinterpret_cast<BYTE *>(&d);
   for (int i = 0; i < nBytes; i++) bs[i+ofs] = dw[i];
}

//==============================================================================
//  x %nc = count,       binary, n-bytes long
//  - %nC = count,       ascii,  n-character field width
//  x %no = offset,      binary
//  - %nO = offset,      ascii
//  x %np = page number, binary
//  - %nP = page number, ascii
//  x %nv = value,       binary, found at _const[offset]
//  - %nV = value,       ascii
//  x %ni = page id,     binary
//
//  If n is not present, it defaults to 1.
//
//  Intel x86 processors are little endian, _const is stored in machine
//  endian, so we need to translate back and forth when using words longer
//  than 8 bits.
//------------------------------------------------------------------------------

void commandFormat::parse(const char *rawCmd)
{
   _raw = xlate(rawCmd);
   _dSiz = 0; // Keep track of data characters.
   _oOfs = _cOfs = _pOfs = _vOfs = _iOfs = -1; // In case we are re-parsing.
   int n;
   for (unsigned int i = 0, iBlt = 0; i < _raw.len(); i++) {
      if (_raw[i] != '%') {
         _blt[iBlt++] = _raw[i];
         _dSiz++;
      }
      else {
         // Parse out optional "n" value.
         i++;
         n = 0;
         while (isdigit(_raw[i])) {
            n = 10*n + (_raw[i++]-'0');
         }
         if (n == 0) n = 1;

         switch (_raw[i]) {
            case 'o': _oOfs = iBlt; _oSiz = n; iBlt += n; break;
            case 'p': _pOfs = iBlt; _pSiz = n; iBlt += n; break;
            case 'v': _vOfs = iBlt; _vSiz = n; iBlt += n; break;
            case 'c': _cOfs = iBlt; _cSiz = n; iBlt += n; break;
            case 'i': _iOfs = iBlt; _iSiz = n; iBlt += n; break;

            default:
               throw "Invalid format '%c' in '%s'";
         }
      }
   }
   _empty = _dSiz+_oSiz+_pSiz+_vSiz+_cSiz == 0;
}

//------------------------------------------------------------------------------

byteString &commandFormat::buildCmd(pageParameters &pp, DWORD ofs, const BYTE *val, int cnt)
{
   int n = _vSiz * cnt;
   _blt.resize(_dSiz+_oSiz+_pSiz+_cSiz+n);
   if (_pOfs >= 0) {
      if (pp._bigEnd) bigEndIt(reinterpret_cast<BYTE *>(&pp._pageNo), _pSiz);
      cpDWord(pp._pageNo, _pSiz, _blt, _pOfs);
   }
   if (_iOfs >= 0) { // Copy verbatim, assume user knows endian issues.
      for (int i = 0; i < _iSiz; i++) {
         _blt[_iOfs+i] = pp._pageIdentifier[i];
      }
   }
   if (_oOfs >= 0) {
      if (pp._bigEnd) bigEndIt(reinterpret_cast<BYTE *>(&ofs), _oSiz);
      cpDWord(ofs, _oSiz, _blt, _oOfs);
   }
   if (_cOfs >= 0) {
      if (pp._bigEnd) bigEndIt(reinterpret_cast<BYTE *>(&cnt), _cSiz);
      cpDWord(cnt, _cSiz, _blt, _cOfs);
   }
   if (_vOfs >= 0) {
      for (int i = 0; i < n; i++) {
         _blt[_vOfs+i] = val[i];
      }
   }
   return _blt;
}

//==============================================================================
//  Only propagate commands from the previous one if the all have distinct
//  page activation.

void pageInfo::fillFrom(pageInfo &rhs)
{
   if (_activate.empty() && _pp._pageIdentifier.empty()) return;

   if (_burnCommand.empty(false)) _burnCommand = rhs._burnCommand;
   if (_readWhole  .empty(false)) _readWhole   = rhs._readWhole;
   if (_readChunk  .empty(false)) _readChunk   = rhs._readChunk;
   if (_readValue  .empty(false)) _readValue   = rhs._readValue;
   if (_writeWhole .empty(false)) _writeWhole  = rhs._writeWhole;
   if (_writeChunk .empty(false)) _writeChunk  = rhs._writeChunk;
   if (_writeValue .empty(false)) _writeValue  = rhs._writeValue;
}

//==============================================================================

controllerDescriptor::controllerDescriptor(msComm &IO)
 : _const              (NULL)
 , _io                 (IO)
 , _changed            (false)
 , _interWriteDelay    (0)
 , _writeBlocks        (true)
 , _pageActivationDelay(0)
 , _blockReadTimeout   (25)
 , _userVarSize        (0)
 , _sigFile            (NULL)
 , _verifying          (false)
{
   // B&G MS-I V 2.xx and 3.00
   setEndianness     ("big");
   setVersionInfo    ("");
   setQueryCommand   ("Q");
   setSignature      (BYTE(20), NULL);

   setNOPages        (1);
   setOchBlockSize   (22,  0);
   setOchGetCommand  ("A", 0);
   setOchBurstCommand("A", 0);
   setOchPollPattern ("[1]", 0);

   setNPages         (1);
   setBurnCommand    ("B", 0);
   setPageSize       (125, 0);
   setPageActivate   ("",  0);
   setPageIdentifier ("",  0);
   setPageReadWhole  ("V", 0);
   setPageWriteValue ("W%1o%1v", 0);

   memset(_wholePage, 0, sizeof(_wholePage));
}

controllerDescriptor::~controllerDescriptor()
{
   if (_const    ) delete [] _const;
   if (_exprs    ) delete    _exprs;
   if (_ochBuffer) delete [] _ochBuffer;
   if (_userVar  ) delete [] _userVar;
   if (_sigFile  ) free(_sigFile);

   for (symMapIter iSym = symbolTable.begin(); iSym != symbolTable.end(); iSym++) {
      delete (*iSym).second;
   }

   for (int i = 0; i < _nPages; i++) {
      delete _wholePage[i];
   }
}

void cfgLog(const char *Format, ...);

void controllerDescriptor::init()
{
   cfgLog("   interWriteDelay     = %6d\n   pageActivationDelay = %6d\n   blockReadTimeout    = %6d\n   writeBlocks         = %s", _interWriteDelay, _pageActivationDelay, _blockReadTimeout, _writeBlocks?"on":"off");

   _io.setChunking    (_writeBlocks, _interWriteDelay);
   _io.setReadTimeouts(_blockReadTimeout);

   int n = totalSpace() < 257 ? 257 : totalSpace();
   _const     = new BYTE[n];
   memset(_const, 0, n);

   n = totalOSpace() < 22 ? 22 : totalOSpace();
   _ochBuffer = new BYTE[n];

   int i;
   for (i = 0; i < _nOPages; i++) {
      _oPage[i].reset();
   }

   for (i = 0; i < _nPages; i++) {
      _page[i]._pp._pageNo   = i;
      _page[i]._pp._bigEnd   = _bigEnd;
      _page[i]._pp._modified = false;
   }

   for (i = 1; i < _nPages; i++) {
      _page[i].fillFrom(_page[i-1]); // ??? this is wack
   }

   for (i = 0; i < _nPages; i++) {
      _wholePage[i] = new symbol();
      char shape[16];
      sprintf(shape, "[%d]", _page[i].siz());
      _wholePage[i]->setCArray ("__page__", "U08", i, 0, "", shape, 1.0, 0.0, 0.0, 1.0, 0);
   }

   int varIndex = 0;
   for (symMapIter iSym = symbolTable.begin(); iSym != symbolTable.end(); iSym++) {
      symbol *s = (*iSym).second;
      if (!s->isConst()) {
         // Allocate indexes for the variable data now.
         s->varIndex(varIndex);
         varIndex++;
      }
   }
   assert(_userVarSize == 0);
   _userVarSize = varIndex+1;
   _userVar     = new double[_userVarSize];
   memset(_userVar, 0, _userVarSize*sizeof(double));

   // Parse the expressions now that we know where they all go.

   // this needs to parse the expression in the same order in which they are entered...

   _exprs = new expression();
   for (int index = 0; index < symbolTable.size(); index++) {
      for (symMapIter iSym = symbolTable.begin(); iSym != symbolTable.end(); iSym++) {
         symbol *s = (*iSym).second;
         if (s->_sequence == index) {
            if (s->isExpr()) {
               if (s->exprText() != "---") {
                  _exprs->addExpr(s->varIndex(), s->exprText(), s->exprFile(), s->exprLine());
               }
            }
         }
      }
   }
   _exprs->setOutputBuffer(_userVar);
}

//------------------------------------------------------------------------------

int controllerDescriptor::addExpr(const char *expr)
{
   _exprs->addExpr(_userVarSize, expr, "", 0);

   delete [] _userVar;
   _userVarSize++;
   _userVar = new double[_userVarSize];
   memset(_userVar, 0, _userVarSize*sizeof(double));
   _exprs->setOutputBuffer(_userVar);
   return _userVarSize-1;
}

//------------------------------------------------------------------------------

void controllerDescriptor::recalc()
{
   _exprs->recalc();
}

//------------------------------------------------------------------------------

void controllerDescriptor::pageModified(int pageNo, bool state)
{
   _page[mxi(pageNo,0)].modified(state);
}

bool controllerDescriptor::pageModified(int pageNo)
{
   return _page[mxi(pageNo,0)].modified();
}

//------------------------------------------------------------------------------

void controllerDescriptor::setNOPages(int n)
{
   _nOPages = n;
}

void controllerDescriptor::setOchBlockSize(int nBytes, int pageNo)
{
   pageNo = mxi(pageNo,1);
   _oPage[pageNo].siz(nBytes);
   _oPage[pageNo].ofs(pageNo <= 0 ? 0 : _oPage[pageNo-1].ofs()+_oPage[pageNo-1].siz());
}

void controllerDescriptor::setOchGetCommand(const char *cmd, int pageNo)
{
   pageNo = mxi(pageNo,1);
   _oPage[pageNo]._ochGetCommand.parse(cmd);
}

void controllerDescriptor::setOchBurstCommand(const char *cmd, int pageNo)
{
   pageNo = mxi(pageNo,1);
   _oPage[pageNo]._ochBurstCommand.parse(cmd);
}

//------------------------------------------------------------------------------

#define notDigit(c)  (strchr("0123456789", c) == NULL)
#define isSign(c)    (strchr("+-",c) != NULL)
#define isChar(c,x)  (toupper(c) == x)
#define notChar(c,x) (toupper(c) != x)

#define skipSp {while (*s == ' ') s++;}
#define next   {s++; skipSp}

void pollSequence::add(int i)
{
   // Don't make them unique, user might have a good reason to re-poll...
   // for (unsigned iS = 0; iS < _seq.size(); iS++) if (_seq[iS] == i) return;
   _seq.push_back(i);
}

int controllerDescriptor::ochSeqCount ()             { return int(_ochPoll.size()); }
int controllerDescriptor::ochSeqSize  (int i)        { return int(_ochPoll[i].size()); }
int controllerDescriptor::ochSeqValue (int i, int s) { return _ochPoll[i][s]; }

void controllerDescriptor::setOchPollPattern(const char *ss, int i)
{
   if (i == 0) _ochPoll.clear();

   const char *s = ss; skipSp;
   const char *o = s;

   _ochPoll.push_back(pollSequence());
   pollSequence &sq = _ochPoll.back();

   if (notChar(*s, '[')) throw symErr("Missing opening bracket '['", o, s-o);
   next; // Past opening [.

   while (*s != ']') {
      int loPg;
      int hiPg;
      if (*s == '*') {
         loPg = 1;
         hiPg = _nOPages;
      }
      else {
         if (notDigit(*s)) throw symErr("Missing number", o, s-o);
         loPg = strtol(s, const_cast<char **>(&s), 10);
         hiPg = loPg;
         if (loPg < 1 || loPg > _nOPages) throw symErr("Page number out of range", o, s-o-1);
         skipSp; // Past any trailing blanks.

         if (*s == '-') {
            next;
            skipSp;
            hiPg = strtol(s, const_cast<char **>(&s), 10);
            if (hiPg < loPg || hiPg > _nOPages) throw symErr("Page number out of range", o, s-o-1);
            skipSp; // Past any trailing blanks.
         }
      }

      if (*s == ',') next;

      for (int iP = loPg-1; iP < hiPg; iP++) {
         sq.add(iP);
      }
   }

   if (notChar(*s, ']')) throw symErr("Missing closing bracket ']' after polling sequence value", o, s-o);
   next; // Past the closing ].

   if (*s) throw symErr("Useless junk in polling sequence specification", o, s-o);
}

//------------------------------------------------------------------------------

void controllerDescriptor::setOchPolled(int pageNo, bool ok)
{
   _oPage[mxi(pageNo,1)].increment(ok);
}

void controllerDescriptor::setEndianness(const char *cmd)
{
   _bigEnd = strcmpi(cmd, "big") == 0;
}

void controllerDescriptor::setVersionInfo(const char *cmd)
{
   _versionInfo = xlate(cmd);
}

void controllerDescriptor::setQueryCommand(const char *cmd)
{
   _queryCommand = xlate(cmd);
}

void controllerDescriptor::setSignature(const byteString sig, const char *sigFile)
{
   _signature = sig;
   _signature.xlate();
   if (_sigFile) free(_sigFile);
   _sigFile = sigFile ? _strdup(sigFile) : NULL;
}

//------------------------------------------------------------------------------
// These should check bounds...

void controllerDescriptor::setNPages(int n)
{
   _nPages = n;
}

void controllerDescriptor::setVerify(bool setting)
{
   _verifying = setting;
}

void controllerDescriptor::setPageSize(int size, int i)
{
   _page[i].siz(size);
   _page[i].ofs(i <= 0 ? 0 : _page[i-1].ofs()+_page[i-1].siz());
}

void controllerDescriptor::setBurnCommand(const char *cmd, int i)
{
   _page[i]._burnCommand.parse(cmd);
}

void controllerDescriptor::setPageActivate   (const char *cmd, int i)
{
   _page[i]._activate.parse(cmd);
}
void controllerDescriptor::setPageIdentifier (const char *cmd, int i)
{
   _page[i]._pp._pageIdentifier = xlate(cmd);
}
void controllerDescriptor::setPageReadWhole(const char *cmd, int i)
{
// assert(_nPages > i);
   _page[i]._readWhole.parse(cmd);
}
void controllerDescriptor::setPageReadChunk(const char *cmd, int i)
{
   _page[i]._readChunk.parse(cmd);
}
void controllerDescriptor::setPageReadValue(const char *cmd, int i)
{
   _page[i]._readValue.parse(cmd);
}

void controllerDescriptor::setPageWriteWhole(const char *cmd, int i)
{
   _page[i]._writeWhole.parse(cmd);
}
void controllerDescriptor::setPageWriteChunk(const char *cmd, int i)
{
   _page[i]._writeChunk.parse(cmd);
}
void controllerDescriptor::setPageWriteValue(const char *cmd, int i)
{
   _page[i]._writeValue.parse(cmd);
}

//------------------------------------------------------------------------------

const byteString &controllerDescriptor::ochGetCommand  (int page) {
   page = mxi(page,1);
   return _oPage[page]._ochGetCommand  .buildCmd(_oPage[page]._pp, 0, NULL, _oPage[page].siz());
}
const byteString &controllerDescriptor::ochBurstCommand(int page) {
   page = mxi(page,1);
   return _oPage[page]._ochBurstCommand.buildCmd(_oPage[page]._pp, 0, NULL, _oPage[page].siz());
}

int controllerDescriptor::totalOSpace() {
   int space = 0;
   for (int i = 0; i < _nPages; i++) space += _oPage[i].siz();
   return space;
}


const byteString &controllerDescriptor::versionInfo   () const { return _versionInfo;   }
const byteString &controllerDescriptor::queryCommand  () const { return _queryCommand;  }
const byteString &controllerDescriptor::signature     () const { return _signature;     }
const char       *controllerDescriptor::signatureFile () const { return _sigFile;       }

//------------------------------------------------------------------------------

int controllerDescriptor::nPages() { return _nPages; }

int controllerDescriptor::pageSize  (int pageNo, int db) { return db == 0 ? _page[mxi(pageNo,0)].siz() : _oPage[mxi(pageNo,1)].siz(); } 
int controllerDescriptor::pageOffset(int pageNo, int db) { return db == 0 ? _page[mxi(pageNo,0)].ofs() : _oPage[mxi(pageNo,1)].ofs(); }

int controllerDescriptor::totalSpace() {
   int space = 0;
   for (int i = 0; i < _nPages; i++) space += _page[i].siz();
   return space;
}

//------------------------------------------------------------------------------

const byteString &controllerDescriptor::burnCommand(int pageNo)
{
   pageNo = mxi(pageNo,0);
   return _page[pageNo]._burnCommand.buildCmd(_page[pageNo]._pp, 0, NULL, _page[pageNo].siz());
}

const byteString &controllerDescriptor::pageActivate(int pageNo)
{
   pageNo = mxi(pageNo,0);
   return _page[pageNo]._activate.buildCmd(_page[pageNo]._pp, 0, NULL, _page[pageNo].siz());
}

//------------------------------------------------------------------------------

const byteString &controllerDescriptor::pageReadWhole(int pageNo)
{
   pageNo = mxi(pageNo,0);
   return _page[pageNo]._readWhole.buildCmd(_page[pageNo]._pp, 0, NULL, _page[pageNo].siz());
}

const byteString &controllerDescriptor::pageReadChunk(int pageNo, int offset, int nBytes)
{
   pageNo = mxi(pageNo,0);
   return _page[pageNo]._readChunk.buildCmd(_page[pageNo]._pp, offset, NULL, nBytes);
}

const byteString &controllerDescriptor::pageReadValue(int pageNo, int offset)
{
   pageNo = mxi(pageNo,0);
   return _page[pageNo]._readValue.buildCmd(_page[pageNo]._pp, offset, NULL, 1);
}

//------------------------------------------------------------------------------

const byteString &controllerDescriptor::pageWriteWhole(int pageNo)
{
   pageNo = mxi(pageNo,0);
   return _page[pageNo]._writeWhole.buildCmd(_page[pageNo]._pp, 0, _const+_page[pageNo].ofs(), _page[pageNo].siz());
}

const byteString &controllerDescriptor::pageWriteChunk(int pageNo, int offset, int nBytes)
{
   pageNo = mxi(pageNo,0);
   return _page[pageNo]._writeChunk.buildCmd(_page[pageNo]._pp, offset, _const+(offset+_page[pageNo].ofs()), nBytes);
}

const byteString &controllerDescriptor::pageWriteValue(int pageNo, int offset)
{
   pageNo = mxi(pageNo,0);
   return _page[pageNo]._writeValue.buildCmd(_page[pageNo]._pp, offset, _const+(offset+_page[pageNo].ofs()), 1);
}

//------------------------------------------------------------------------------

BYTE  controllerDescriptor::getB(int pNo, int ofs, int db)
{
   BYTE *d = db == 0 ? _const : _ochBuffer;
   return d[ofs+pageOffset(mxi(pNo,db),db)];
}

WORD  controllerDescriptor::getW(int pNo, int ofs, int db)
{
   BYTE *d = db == 0 ? _const : _ochBuffer;
   WORD  v;
   int   po = pageOffset(mxi(pNo,db),db);
   BYTE *b  = reinterpret_cast<BYTE *>(&v);
   b[0]     = d[po+ofs+0];
   b[1]     = d[po+ofs+1];
   if (_bigEnd) bigEndIt(b, 2);
   return v;
}

DWORD controllerDescriptor::getD(int pNo, int ofs, int db)
{
   BYTE *d = db == 0 ? _const : _ochBuffer;
   DWORD v;
   int   po = pageOffset(mxi(pNo,db),db);
   BYTE *b  = reinterpret_cast<BYTE *>(&v);
   b[0]     = d[po+ofs+0];
   b[1]     = d[po+ofs+1];
   b[2]     = d[po+ofs+2];
   b[3]     = d[po+ofs+3];
   if (_bigEnd) bigEndIt(b, 4);
   return v;
}

//------------------------------------------------------------------------------

#define SET(i) { if (_const[po+ofs+i] != b[i]) { _const[po+ofs+i] = b[i]; _changed = true; } }

void controllerDescriptor::putB(DWORD v, int pNo, int ofs, int db)
{
   if (db != 0) return;
   int   po = pageOffset(mxi(pNo,db),db);
   BYTE *b  = reinterpret_cast<BYTE *>(&v);
   SET(0);
}

void controllerDescriptor::putW(DWORD v, int pNo, int ofs, int db)
{
   if (db != 0) return;
   int   po = pageOffset(mxi(pNo,db),db);
   BYTE *b  = reinterpret_cast<BYTE *>(&v);
   if (_bigEnd) bigEndIt(b, 2);
   SET(0);
   SET(1);
}

void controllerDescriptor::putD(DWORD v, int pNo, int ofs, int db)
{
   if (db != 0) return;
   int   po = pageOffset(mxi(pNo,db),db);
   BYTE *b  = reinterpret_cast<BYTE *>(&v);
   if (_bigEnd) bigEndIt(b, 4);
   SET(0);
   SET(1);
   SET(2);
   SET(3);
}

//------------------------------------------------------------------------------

bool controllerDescriptor::write(const BYTE *bytes, DWORD nBytes)
{
   return _io.write(bytes, nBytes);
}

//------------------------------------------------------------------------------

int controllerDescriptor::lastPage = -99;

bool controllerDescriptor::read(BYTE *bytes, DWORD nBytes)
{
   bool success = _io.read(bytes, nBytes);
   if (!success) lastPage = -99;
   return success;
}

bool controllerDescriptor::read(BYTE &byte)
{
   return read(&byte, 1);
}

//------------------------------------------------------------------------------

void controllerDescriptor::flush()
{
   _io.flush();
}

//------------------------------------------------------------------------------

bool controllerDescriptor::sendPageActivate(int thisPage, bool force)
{
   if (pageActivate(thisPage).empty()) return true;

   if (thisPage != lastPage || force) {
      lastPage = thisPage;
      bool success = _io.write(pageActivate(thisPage));
      if (success && _pageActivationDelay) ::Sleep(_pageActivationDelay);
      return success;
   }
   return true;
}

bool controllerDescriptor::sendPageActivate(const symbol *s)
{
   return sendPageActivate(s->page());
}

bool controllerDescriptor::sendPageReadWhole(int page)
{
   pageModified    (page, false);
   sendPageActivate(page, true);
   return _io.write(pageReadWhole(page));
}

bool controllerDescriptor::sendPageReadChunk(int page, int offset, int nBytes)
{
   sendPageActivate(page, true);
   return _io.write(pageReadChunk(page, offset, nBytes));
}

bool controllerDescriptor::sendPageReadValue(int page, int offset)
{
   sendPageActivate(page, true);
   return _io.write(pageReadValue(page, offset));
}

bool controllerDescriptor::verify(int pageNo, int offset, int nBytes)
{
   static BYTE *readBuffer     = NULL;
   static int   readBufferSize = 0;

   const BYTE *ram = _const+(offset+_page[pageNo].ofs());
   // 
   return true;
}

bool controllerDescriptor::sendBurnCommand(int thisPage)
{
   sendPageActivate(thisPage);
   bool success = _io.write(burnCommand(thisPage));
   if (success) {
      if (_pageActivationDelay) ::Sleep(_pageActivationDelay);
      if (_verifying          ) verify(thisPage, 0, _page[thisPage].siz());
      pageModified(thisPage, false);
   }
   return success;
}

bool controllerDescriptor::sendOchGetCommand(int thisPage)
{
   return _io.write(ochGetCommand(thisPage));
}

bool controllerDescriptor::sendOchBurstCommand(int thisPage)
{
   return _io.write(ochBurstCommand(thisPage));
}

//------------------------------------------------------------------------------

void controllerDescriptor::addSymbol(symbol *s)
{
   symMapIter i = symbolTable.find(s->name());
   if (i != symbolTable.end()) {
      throw CString("You have defined \"")+s->name()+"\" more than once, change one of them.";
   }
   symbolTable[s->name()] = s;
}

bool controllerDescriptor::send(const symbol *s, int index)
{
   if (s == NULL           ) return false;
   if (!sendPageActivate(s)) return false;

   int page     = s->page();
   int ofs      = s->offset(index);
   int nBytes;
   int valBytes = _page[page]._writeValue.valueSize();
   if (s->size() <= valBytes || index >= 0 && s->sizeOf() <= valBytes) {
      nBytes = valBytes;
      if (index < 0) index = 0;
      if (_writeBlocks) ::Sleep(_interWriteDelay);
      _io.write(pageWriteValue(page, s->offset(index)));
   }
   else {
      nBytes = index == -1 ? s->size() : s->sizeOf();
      if (index < 0) index = 0;
      if (!_page[page]._writeChunk.empty()) { // We got a chunk write, so use it.
         if (_writeBlocks) ::Sleep(_interWriteDelay);
         _io.write(pageWriteChunk(page, ofs, nBytes));
      }
      else if (!_page[page]._writeValue.empty()) { // Just do it value at a time.
         byteString chunk;
         for (int i = 0; i < nBytes; i+=valBytes) {
            chunk.append(pageWriteValue(page, ofs+i));
         }
         if (_writeBlocks) ::Sleep(_interWriteDelay);
         _io.write(chunk);
      }
      else if (!_page[page]._writeWhole.empty()) {
         if (_writeBlocks) ::Sleep(_interWriteDelay);
bool ramWrite = true;
         if (ramWrite) {
            _io.write(pageWriteWhole(page));
         }
         else {
            byteString chunk = pageWriteWhole(page);
            _io.write(chunk.ptr(),   2);
            if (_pageActivationDelay) ::Sleep(_pageActivationDelay);
            for (int i = 2; i < chunk.len(); i+=2) {
               _io.write(chunk.ptr()+i, 2);
            }
         }
      }
   }

   pageModified(page, true);
   return _verifying ? verify(page, ofs, nBytes) : true;
}

bool controllerDescriptor::sendPage(int thisPage)
{
   return send(_wholePage[thisPage]);
}

//------------------------------------------------------------------------------

symbol *controllerDescriptor::lookup(const CString &name)
{
   symMapIter i = symbolTable.find(name);
   return i == symbolTable.end() ? NULL : (*i).second;
}

int controllerDescriptor::varIndex(const CString &name)
{
   if (symbolTable.find(name) == symbolTable.end()) {
//    msgOk("Symbol Lookup", CString("Couldn't find "+name));
      return -1;
   }
//   assert(!symbolTable[name]->isConst()); // This is often false, since this function is called from expression parsing to find out if indeed the symbol is a variable or not.
   return symbolTable[name]->varIndex();
}

void controllerDescriptor::populateUserVars()
{
   // Speed this up with a simple "symbol *vars" list built in "init"...
   for (symMapIter iSym = symbolTable.begin(); iSym != symbolTable.end(); iSym++) {
      symbol *s = (*iSym).second;
      if (s->isVar() && !s->isExpr()) {
         _userVar[s->varIndex()] = s->valueUser(0);
      }
   }
}

//------------------------------------------------------------------------------

void controllerDescriptor::serialize(CFile &msqFile)
{
   int sigSize = _signature.len();
   msqFile.Write(&sigSize,         sizeof(int));
   msqFile.Write(_signature.ptr(), sigSize);
   msqFile.Write(&_bigEnd,         sizeof(_bigEnd));
   msqFile.Write(&_nPages,         sizeof(_nPages));
   for (int i = 0; i < _nPages; i++) {
      int s = _page[i].siz();
      msqFile.Write(&s, sizeof(int));
   }
   msqFile.Write(_const, totalSpace());
}

bool controllerDescriptor::checkFile(CFile &msqFile, CString &error)
{
   bool ok = true;
   error   = "";

   int  errNo = 1;
   char msg[128];

   int   sigSize;
   msqFile.Read(&sigSize, sizeof(int));
   BYTE *sig = new BYTE[sigSize];
   msqFile.Read(sig, sigSize);
   byteString signature(sig, sigSize);

   if (signature != _signature) {
      sprintf(msg, "   %d) Signatures don't match, found \"%s\", expected \"%s\".", errNo++, signature.str(), _signature.str());
      if (!error.IsEmpty()) error += "\n\n";
      error += CString(msg);
      ok     = false;
   }
   delete [] sig;

   bool bigEnd;
   msqFile.Read(&bigEnd, sizeof(bool));
   if (bigEnd != _bigEnd) {
      sprintf(msg, "   %d) Endianess of multi-byte data doesn't match.", errNo++);
      if (!error.IsEmpty()) error += "\n\n";
      error += CString(msg);
      ok     = false;
   }

   int nPages;
   msqFile.Read(&nPages, sizeof(int));
   if (nPages != _nPages) {
      sprintf(msg, "   %d) Number of pages doesn't match, found %d, expected %d.", errNo++, nPages, _nPages);
      if (!error.IsEmpty()) error += "\n\n";
      error += CString(msg);
      ok     = false;
   }

   int pageSize;
   for (int i = 0; i < _nPages; i++) {
      msqFile.Read(&pageSize, sizeof(int));
      if (pageSize != _page[i].siz()) {
         sprintf(msg, "   %d) Page %d size doesn't match, found %d, expected %d.", errNo++, i+1, pageSize, _page[i].siz());
         if (!error.IsEmpty()) error += "\n\n";
         error += CString(msg);
         ok     = false;
      }
   }

   return ok;
}

void controllerDescriptor::deserialize(CFile &msqFile)
{
   int   sigSize;
   msqFile.Read(&sigSize, sizeof(int));
   BYTE *sig = new BYTE[sigSize];
   msqFile.Read(sig, sigSize);
   byteString signature(sig, sigSize);

   bool bigEnd;
   msqFile.Read(&bigEnd, sizeof(bool));

   int nPages;
   msqFile.Read(&nPages, sizeof(int));

   int *pageSize = new int[nPages];
   int totalBytes = 0;
   int i;
   for (i = 0; i < nPages; i++) {
      msqFile.Read(pageSize+i, sizeof(int));
      totalBytes += pageSize[i];
   }

   BYTE *constBuf = new BYTE[totalBytes];
   msqFile.Read(constBuf, totalBytes);

#define MIN(x,y) ((x)<(y)?(x):(y))
   int dskOfs = 0;
   int memOfs = 0;
   for (i = 0; i < MIN(_nPages, nPages); i++) {
      memcpy(_const+memOfs, constBuf+dskOfs, MIN(_page[i].siz(), pageSize[i]));
      memOfs += _page[i].siz();
      dskOfs += pageSize[i];
   }

   delete [] constBuf;
   delete [] pageSize;
}

//------------------------------------------------------------------------------

void controllerDescriptor::dump(FILE *dmp)
{
   int iPage;
   for (iPage = 0; iPage < _nPages; iPage++) {
      dPrintL(dmp, "Page %d =================================================================", iPage);
      dPrintL(dmp, "");
      fflush(dmp);
      for (symMapIter iSym = symbolTable.begin(); iSym != symbolTable.end(); iSym++) {
         symbol *s = (*iSym).second;
         if (s->isConst() && s->page() == iPage) {
            s->dump(dmp);
         }
      }
      dPrintL(dmp, "");
   }
   fflush(dmp);

   for (iPage = 0; iPage < _nOPages; iPage++) {
      dPrintL(dmp, "Page %d (runtime) =======================================================", iPage);
      dPrintL(dmp, "   Polls   : %3d", _oPage[iPage].polls());
      dPrintL(dmp, "   Failures: %3d (%4.1f%%)", _oPage[iPage].failures(), 100.0*_oPage[iPage].failures()/_oPage[iPage].polls());
      dPrintL(dmp, "");
      fflush(dmp);
      for (symMapIter iSym = symbolTable.begin(); iSym != symbolTable.end(); iSym++) {
         symbol *s = (*iSym).second;
         if (s->isVar() && s->page() == iPage) {
            s->dump(dmp);
            fflush(dmp);
         }
      }
      dPrintL(dmp, "");
   }
}

//------------------------------------------------------------------------------

static CString intStr(int i)
{
   CString s;
   s.Format("%d", i);
   return s;
}

void controllerDescriptor::saveXML(const char *fileName)
{
   xml::DOM d;
   xml::domElement *msq = d.topElement("msq");
   msq->addAttribute("xmlns", "http://www.msefi.com/:msq");

   xml::domElement *bib = msq->addChild("bibliography");
   bib->addAttribute("writeDate", timeStamp());
   bib->addAttribute("author", "MegaTune "VERSION_STRING);

   xml::domElement *vin = msq->addChild("versionInfo");
   vin->addAttribute("fileFormat", "4.0");
   vin->addAttribute("signature",  _signature.ptr());
   vin->addAttribute("nPages",     intStr(_nPages));

   for (int i = 0; i < _nPages; i++) {
      xml::domElement *page = msq->addChild("page");
      page->addAttribute("number", intStr(i));
      page->addAttribute("size",   intStr(pageSize(i)));
      for (symMapIter iSym = symbolTable.begin(); iSym != symbolTable.end(); iSym++) {
         symbol *s = (*iSym).second;
         if (s->isConst() && s->page() == i) {
            xml::domElement *sym = page->addChild("constant");
            sym->addAttribute("name", s->name());

            CString u = s->units();
            if (!u.IsEmpty()) sym->addAttribute("units", u);

            int rows, cols;
            s->shape(rows, cols);
            if (rows > 1) sym->addAttribute("rows", intStr(rows));
            if (cols > 1) sym->addAttribute("cols", intStr(cols));

            sym->addContents(s->valueFormatted());
         }
      }
   }

   d.write(fileName);
}

//------------------------------------------------------------------------------

int controllerDescriptor::readXML(const char *fileName)
{
   cfgLog("");
   units u;
   cfgLog("%s:Open", fileName);

   int nAnomalies = 0;

   xml::DOM d;
   d.read(fileName);
   xml::domElementList &elements = d.topElement()->children();

   const int nWarnings = 6;
   bool warningsEmitted[nWarnings+1];
   memset(warningsEmitted, 0, sizeof(bool)*(nWarnings+1));

   std::map<CString, bool> symSet;
   for (symMapIter iSym = symbolTable.begin(); iSym != symbolTable.end(); iSym++) {
      if ((*iSym).second->isConst()) {
         symSet[(*iSym).second->name()] = false;
      }
   }

   for (xml::domElementIterator i = elements.begin(); i != elements.end(); i++) {
      xml::domElement *e = *i;

      if (e->tag() == "versionInfo") {
         CString msqSignature = e->attribute("signature", "-not found-");
         CString fileFormat   = e->attribute("fileFormat", "-not found-");

         if (msqSignature == "-not found-" || fileFormat == "-not found-") {
            CString s;
            s.Format("File %s is missing attributes and appears not to be an msq file.", fileName);
            throw s;
         }

         if (msqSignature != signature().ptr()) {
            nAnomalies++;
            warningsEmitted[1] = true;
            cfgLog("   WARNING(01): Signatures don't match.\n"
                   "            Found in msq      \"%s\"\n"
                   "            Expected from ini \"%s\"\n",
                   msqSignature,
                   signature().ptr());
         }

         if (fileFormat != "4.0") {
            CString s;
            s.Format("Attribute incorrect in %s    \n\n"
                     "   Expected\tfileFormat=\"%s\"\n"
                     "   Found     \tfileFormat=\"%s\""
                     , fileName, "4.0", fileFormat);
            throw s;
         }
      }

      // Add "directives" to the xml file:
      //    <warnings disable="6" />
      //    <message text="Hello, world!" />

      if (e->tag() == "page") {
         xml::domElementList &ec = e->children();
         for (xml::domElementIterator c = ec.begin(); c != ec.end(); c++) {
            xml::domElement *cc = *c;
            if (cc->tag() == "constant") {
               CString name = cc->attribute("name");
               symbol *sym  = lookup(name);

               if (sym == NULL) {
                  nAnomalies++;
                  warningsEmitted[2] = true;
                  cfgLog("   WARNING(02): Constant named \"%s\" ignored, not found in configuration.", name);
               }
               else {
//if (sym->page() != 3) continue; // for some -extra testing
                  symSet[name] = true;
                  cc->cleanContents();
                  CString pValue = cc->contents();
                  if (pValue[0] == '"') {
                     pValue.Delete(0);
                     pValue.Delete(pValue.GetLength()-1);
                     if (! sym->storeValue(pValue)) {
                        nAnomalies++;
                        warningsEmitted[3] = true;
                        cfgLog("   WARNING(03): Value from option list \"%s\" = \"%s\" not found.", name, pValue);
                     }
                  }
                  else { // Numbers...
                     CString msqUnits = cc->attribute("units");
                     CString iniUnits = sym->units();
                     if (msqUnits != iniUnits && (!u.exists(msqUnits) || !u.exists(iniUnits))) {
                        nAnomalies++;
                        warningsEmitted[4] = true;
                        cfgLog("   WARNING(04): Constant \"%s\" units mismatch, \"%s\" in msq, expected \"%s\" from ini.", name, msqUnits, iniUnits);
                     }

                     int     msqRows = strtol(cc->attribute("rows", "1"), NULL, 10);
                     int     msqCols = strtol(cc->attribute("cols", "1"), NULL, 10);
                     int     iniRows, iniCols;
                     sym->shape(iniRows, iniCols);
                     if (msqRows != iniRows || msqCols != iniCols) {
                        nAnomalies++;
                        warningsEmitted[5] = true;
                        cfgLog("   WARNING(05): Constant \"%s\" shape mismatch, %dx%d in msq, expected %dx%d from ini.", name, msqRows, msqCols, iniRows, iniCols);
                     }

                     char *s = const_cast<char *>(LPCTSTR(pValue));
                     int idx = 0;
                     while (*s && idx < iniRows*iniCols) {
                        double x = strtod(s, &s);
                        x = u.convert(x, msqUnits, iniUnits);
                        sym->storeValue(x, idx);
                        idx++;
                     }
                  }
               }
            }
         }
      }
   }

   for (std::map<CString, bool>::iterator iSet = symSet.begin(); iSet != symSet.end(); iSet++) {
      if (! (*iSet).second) {
         nAnomalies++;
         warningsEmitted[6] = true;
         cfgLog("   WARNING(06): Constant named \"%s\" remains unaltered.", (*iSet).first);
      }
   }

   bool first = true;
   for (int iW = 1; iW <= nWarnings; iW++) {
      if (warningsEmitted[iW]) {
         if (first) {
            cfgLog("\nWarning Details\n---------------\n");
            first = false;
         }
         switch (iW) {
            case 1:
               cfgLog("   WARNING(01) means that the file you read was not written from the\n"
                      "               configuration that you are currently using.  If the\n"
                      "               two configurations were similar, you should see few\n"
                      "               warnings and most settings will carry over successfully.\n"
                      "               If they were quite different, you may have lots of\n"
                      "               problems getting things to work, but you need to make\n"
                      "               the judgement call on this by examining the other\n"
                      "               warnings above.\n");
               break;
            case 2:
               cfgLog("   WARNING(02) means that a constant was found in the msq file, but it\n"
                      "               does not appear in the current configuration.  MegaTune\n"
                      "               ignored this value and just threw it away.  You can\n"
                      "               safely ignore this message it is merely informative.\n");
               break;
            case 3:
               cfgLog("   WARNING(03) means that the value expressed in the file for the\n"
                      "               constant was not found one of the options listed in the\n"
                      "               current configuration.  The constant was not altered, so\n"
                      "               you probably want to look more closely at this one and\n"
                      "               see what caused the mismatch.\n");
               break;
            case 4:
               cfgLog("   WARNING(04) means that you have different units set in the msq and\n"
                      "               your current configuration.  This almost always means\n"
                      "               that the value of the constant is incorrect and you must\n"
                      "               manually correct the situation.  The easiest way might\n"
                      "               be to reset your configuration's settings, say from\n"
                      "               Celsius to Fahrenheit, then re-reading the msq.\n");
               break;
            case 5:
               cfgLog("   WARNING(05) means that the constant read from the file was a\n"
                      "               different shape and/or size than the configuration\n"
                      "               expected.  If the file contained more values than would\n"
                      "               fit in the current configuration, some values were thrown\n"
                      "               away.  This warning cannot be ignored, you must look at\n"
                      "               the resulting constants in memory and they usually will be\n"
                      "               all screwed up.\n");
               break;
            case 6:
               cfgLog("   WARNING(06) means that the constant found in the current configuration\n"
                      "               was not found in the file, and thus has not been altered by\n"
                      "               the file read.  Make sure that the value of the constant\n"
                      "               makes sense in the context of the other changes.\n");
               break;
         }
      }
   }

   cfgLog("%s:Close\n", fileName);
   return nAnomalies;
}

//------------------------------------------------------------------------------
