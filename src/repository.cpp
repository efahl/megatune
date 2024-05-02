//------------------------------------------------------------------------------
//--  Copyright (c) 2003,2004,2005,2006 by Eric Fahlgren                      --
//--  All Rights Reserved.                                                    --
//--                                                                          --
//--  This program is free software; you can redistribute it and/or           --
//--  modify it under the terms of the GNU General Public License             --
//--  as published by the Free Software Foundation; either version            --
//--  2 of the License, or (at your option) any later version.                --
//--  See http://www.gnu.org/licenses/gpl.txt                                 --
//------------------------------------------------------------------------------

#include "stdafx.h"

#pragma warning(disable:4018 4100 4245 4663 4786)

#include <stack>
#include <math.h>

#include "megatune.h"
#include "resource.h"
#include "repository.h"
#include "msDatabase.h"
#include "datalog.h"
#include "burstLog.h"
#include "tuning3D.h"
#include "userDefined.h"
#include "userMenu.h"
#include "userHelp.h"
#include "curveEditor.h"

extern msDatabase        mdb;
extern datalogOptions    lop;
extern userDialogList    ud;
extern userHelpList      uhl;
extern tuningDialog      ut[tuningDialog::nUT];
extern curveDialog       uc[curveDialog::nUC];
extern userMenuList      uml;
extern userIndicatorList uil;

#define _DEBUG 1
#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------

FILE *cfgOpen (const char *fileName, const char *mode);
void  cfgLog  (const char *Format, ...);
int   cfgClose(FILE *file);

class msgInfo {
   public:
      enum {
         mInfo    = 0x01, // Send to cfgLog
         mAlert   = 0x02, // Alert box + cfgLog
         mWarning = 0x04, // WARNING alert + cfgLog
         mError   = 0x08, // ERROR alert box + cfgLog
         mExit    = 0x10  // Terminate MT
      };

      const char *fileName;
      int         lineNo;
      int         level;
      int         ssize;

      msgInfo(const char *n, int l) : fileName(n), level(l), lineNo(0), ssize(0) { }

      void send(UINT type, const char *fmt, ...)
      {
         va_list args;
         va_start(args, fmt);
            _send(type, fmt, args);
         va_end(args);
      }

      void send1(UINT type, const char *fmt, ...)
      {
         va_list args;
         va_start(args, fmt);
            ssize--;
            _send(type, fmt, args);
            ssize++;
         va_end(args);
      }

      void send0(UINT type, const char *fmt, ...)
      {
         va_list args;
         va_start(args, fmt);
            int svsize = ssize;
            ssize = 0;
            _send(type, fmt, args);
            ssize = svsize;
         va_end(args);
      }

   private:
      void _send(UINT type, const char *fmt, va_list args)
      {
         const char *msgPrefix = "";
         if (type &  mWarning) msgPrefix = "WARNING:";
         if (type &  mError  ) msgPrefix = "ERROR:";

         char fileInfo[128];
         if (*fileName == 0 || lineNo <= 0)
            strcpy(fileInfo, "");
         else
            sprintf(fileInfo, "%s:%04d", fileName, lineNo);

         char msgRaw[2048];
         vsprintf(msgRaw, fmt, args);

         if (type & (mAlert | mWarning | mError)) {
            msgOk("MegaTune Startup", "%s%s - %s", msgPrefix, fileInfo, msgRaw);
         }

         CString msg(msgRaw);
                msg.Replace("\t",   " ");
         while (msg.Replace(" \n",  "\n"));
         while (msg.Replace("\n\n", "\n"));
                msg.Replace("\n",   " ");

         cfgLog("%*s%s %*s%s%s", 3*level,"", fileInfo, 3*ssize,"", msgPrefix, msg);

         if (type &  mExit ) exit(1);
      }
};

static msgInfo globalMsg("", 0);

//------------------------------------------------------------------------------

int   
   Uundefined,
   UegoVoltage,
   UveTuneLodIdx,   // Zero-based index of load bin tuning point.
   UveTuneRpmIdx,   // Index of rpm bin.
   UveTuneValue     // Value contained in VE[veTuneLodIdx, veTuneRpmIdx].
;

//------------------------------------------------------------------------------

repository::repository()
 : currentUD ( -1)
 , currentUF (  0)
 , currentUT ( -1)
 , currentUC ( -1)
 , currentCP ( -1)
 , currentOP ( -1)
 , MTversion (0.0)

 , _fieldColoring(true)
 , _barHysteresis(2.5)
{
   // NO default settings so users do need megatune.ini
   loaTPD =     0.0; hiaTPD =    30.0;
   loaMAD =     0.0; hiaMAD =   300.0;

   lorEGO =     0.0; hirEGO =     1.0;
   lorCT  =   -40.0; hirCT  =   215.0;
   lorBAT =     6.0; hirBAT =    15.0;
   lorTR  =     0.0; hirTR  =   100.0;
   lorGE  =     0.0; hirGE  =   100.0;
   lorMAP =     0.0; hirMAP =   250.0;
   lorMAT =   -40.0; hirMAT =   215.0;
   lorRPM =     0.0; hirRPM = 10000.0;
   lorPW  =     0.0; hirPW  =    25.5;
   lorDC  =     0.0; hirDC  =   100.0;
   lorEGC =     0.0; hirEGC =   200.0;
   lorBC  =     0.0; hirBC  =   200.0;
   lorWC  =     0.0; hirWC  =   200.0;
   lorADC =     0.0; hirADC =   200.0;
   lorVE  =     0.0; hirVE  =   200.0;
   lorACC =     0.0; hirACC =   200.0;

   lofEGO =     0.0; hifEGO =     1.0; rdfEGO =     0.5;

   lotEGO =     0.0; hitEGO =     1.0; rdtEGO =     0.5;
   lotVEB =     0.0; hitVEB =   120.0;
   
   g_alertColor      = RGB(255,   0,   0); // "Redline" part of gauge scale
   g_backgroundColor = ::GetSysColor(COLOR_3DFACE);
   g_faceColor       = RGB(255, 255, 255);
   g_ledColor        = RGB(  0, 255,   0); // Normal LEDs on bar meter
   g_ledAlertColor   = RGB(255,   0,   0); // "Redline" part of bar meter
   g_needleColor     = RGB(155,   0,   0);
   g_rangeColor      = RGB(  0,   0,   0); // Numbers at either end of scale
   g_scaleColor      = RGB( 96,  96,  96); // Normal part of the scale
   g_titleColor      = RGB( 80,  80,  80); // Text title at top of gauge
   g_valueColor      = RGB(  0,   0,   0); // Current value text and units

   t_backgroundColor = RGB(  0,   0,   0);
   t_cursorColor     = RGB(255,   0,   0);
   t_cursorText      = RGB(  0,   0,   0);
   t_gridColor       = RGB(200, 200, 200);
   t_spotColor       = RGB(  0, 255,   0);
   t_spotText        = RGB(  0,   0,   0);
   t_textColor       = RGB(255, 255, 255);

   setColor("SYSTEM_BG", ::GetSysColor(COLOR_3DFACE       ));
   setColor("SYSTEM_WT", ::GetSysColor(COLOR_WINDOWTEXT   ));
   setColor("SYSTEM_AC", ::GetSysColor(COLOR_ACTIVECAPTION));
   setColor("SYSTEM_CT", ::GetSysColor(COLOR_CAPTIONTEXT  ));

   setColor("black",   RGB(  0,   0,   0));
   setColor("gray50",  RGB(127, 127, 127));
   setColor("white",   RGB(255, 255, 255));
   setColor("red",     RGB(255,   0,   0));
   setColor("green",   RGB(  0, 255,   0));
   setColor("blue",    RGB(  0,   0, 255));
   setColor("yellow",  RGB(255, 255,   0));
   setColor("magenta", RGB(255,   0, 255));
   setColor("cyan",    RGB(  0, 255, 255));

#ifdef _DEBUG
// writeInit("default.ini");
#endif
}

void repository::init()
{
   readInit();
#ifdef _DEBUG
// writeInit("postread.ini");
#endif
}

repository::~repository()
{
   for (pageMapIter iP = _gauge.begin(); iP != _gauge.end(); iP++) {
      for (gaugeMapIter iG = (*iP).second.begin(); iG != (*iP).second.end(); iG++) {
         delete (*iG).second;
      }
//    delete (*iP).second; // Crashes for some reason...
   }
}

//------------------------------------------------------------------------------
//--  Looks at every gauge and copies the contents from any reference that    --
//--  is found.                                                               --

void repository::resolveGaugeReferences()
{
   gaugeMapIter noConfig = _gauge[S_GaugeConfigurations].end();

   for (pageMapIter iP = _gauge.begin(); iP != _gauge.end(); iP++) {
      char sss[123]; strcpy(sss, (*iP).first);
      for (gaugeMapIter iG = (*iP).second.begin(); iG != (*iP).second.end(); iG++) {
         gaugeConfiguration *g = (*iG).second;
         if (*g->ref) { // It references some config, so update it.
            gaugeMapIter iC = _gauge[S_GaugeConfigurations].find(g->ref);
            if (iC != noConfig)
               *g = *((*iC).second);
            else {
               globalMsg.send(msgInfo::mWarning,
                  "Gauge labeled '%s' references gauge configuration\n'%s,' which cannot be found.\t",
                  g->label, g->ref);
               strcpy(g->title, " NO GAUGE: ");
               strcat(g->title, g->ref);
            }
         }
      }
   }
}

std::vector<CString> repository::gaugeConfigs()
{
   std::vector<CString> names;
   gaugeMap &cfg = _gauge[S_GaugeConfigurations];

   for (gaugeMapIter iG = cfg.begin(); iG != cfg.end(); iG++) {
      names.push_back((*iG).first);
   }
   return names;
}

//------------------------------------------------------------------------------
//--  Iterates over all gauges, including configurations, and resolves the    --
//--  output channel name into its machine address.                           --

void repository::resolveVarReferences()
{
// intMapIter noOCH = _ochIdx.end(); // Keep it around for convenience.

   for (pageMapIter iP = _gauge.begin(); iP != _gauge.end(); iP++) {
      for (gaugeMapIter iG = (*iP).second.begin(); iG != (*iP).second.end(); iG++) {
         gaugeConfiguration *g = (*iG).second;
         if (!g->cName[0]) continue;
         int varIdx = mdb.cDesc.varIndex(g->cName);
         if (varIdx >= 0)
            g->och = varIdx;
         else {
            g->och = Uundefined;
            globalMsg.send(msgInfo::mWarning,
               "Gauge configuration titled '%s' references\n"
               "variable '%s,' which cannot be found.\t",
               g->title, g->cName);
            strcpy(g->title, " NO VAR: ");
            strcat(g->title, g->cName);
         }
      }
   }

   for (int i = 0; i < tuningDialog::nUT; i++) {
      if (ut[i].title.IsEmpty()) continue;

      ut[i].xOch = mdb.cDesc.lookup(ut[i].xOchName);
      if (ut[i].xOch == NULL) {
         globalMsg.send(msgInfo::mWarning,
            "Table Editor titled '%s' references\n"
            "X variable '%s,' which cannot be found.\t",
            ut[i].title, ut[i].xOchName);
      }
      ut[i].yOch = mdb.cDesc.lookup(ut[i].yOchName);
      if (ut[i].yOch == NULL) {
         globalMsg.send(msgInfo::mWarning, 
            "Table Editor titled '%s' references\n"
            "Y variable '%s,' which cannot be found.\t",
            ut[i].title, ut[i].yOchName);
      }

      ut[i].x = mdb.cDesc.lookup(ut[i].xName);
      if (ut[i].x == NULL) {
         globalMsg.send(msgInfo::mWarning, 
            "Table Editor titled '%s' references\n"
            "xBin constant '%s,' which cannot be found.\t",
            ut[i].title, ut[i].xName);
      }
      ut[i].y = mdb.cDesc.lookup(ut[i].yName);
      if (ut[i].y == NULL) {
         globalMsg.send(msgInfo::mWarning, 
            "Table Editor titled '%s' references\n"
            "yBin constant '%s,' which cannot be found.\t",
            ut[i].title, ut[i].yName);
      }
      ut[i].z = mdb.cDesc.lookup(ut[i].zName);
      if (ut[i].z == NULL) {
         globalMsg.send(msgInfo::mWarning, 
            "Table Editor titled '%s' references\n"
            "zBin constant '%s,' which cannot be found.\t",
            ut[i].title, ut[i].zName);
      }
      if (ut[i].x && ut[i].y && ut[i].z) {
         int rows, cols;
         ut[i].z->shape(rows, cols);
         if (ut[i].x->nValues() != cols || ut[i].y->nValues() != rows) {
            globalMsg.send(msgInfo::mWarning, 
               "Table Editor titled '%s' has zBin\n"
               "with a different shape (%dx%d) than described by x and y (%dx%d).\t",
               ut[i].title, cols, rows, ut[i].x->nValues(), ut[i].y->nValues());
         }
      }

      if (ut[i].atp.autoTune()) { // Verify that certain parameters are valid.
         if (ut[i].atp.correctorName.IsEmpty()) {
            globalMsg.send(msgInfo::mWarning, 
            "Table Editor titled '%s' enables auto-tune,\n"
            "but has no 'corrector' variable defined in the [AutoTune] block.",
            ut[i].title, ut[i].atp.correctorName);
         }
         else {
            ut[i].atp.corrector = mdb.cDesc.lookup(ut[i].atp.correctorName);
            if (ut[i].atp.corrector == NULL) {
            globalMsg.send(msgInfo::mWarning, 
               "Table Editor titled '%s' references\n"
               "auto-tune corrector variable '%s,' which cannot be found.\t",
               ut[i].title, ut[i].atp.correctorName);
            }
         }
      }
   }

   for (int iUC = 0; iUC < curveDialog::nUC; iUC++) {
      if (!uc[iUC].title().IsEmpty()) {
         uc[iUC].resolve();
      }
   }

   for (int iUD = 0; iUD < ud.nDialogs(); iUD++) {
      userDialog &d = ud[iUD];
      for (int iUF = 0; iUF < d.nFields(); iUF++) {
         userEditField &f = d[iUF];
         if (!f.name().IsEmpty()) {
            symbol *s = f.sym(mdb.cDesc.lookup(f.name()));
            if (s == NULL) {
               globalMsg.send(msgInfo::mWarning,
                  "User-defined dialog titled '%s' references\n"
                  "constant '%s,' which cannot be found.\t",
                  d.title(), f.name());
            }
            else {
               if (d.pageNo() > 0 && d.pageNo() != s->page()) {
                  globalMsg.send(msgInfo::mWarning, 
                     "%s: Symbol \"%s\" was found on page %d, but should be on page %d.",
                     d.title(), s->name(), s->page()+1, d.pageNo()+1);
               }
            }
         }
      }
   }
}

//------------------------------------------------------------------------------

void repository::resolveMenuReferences()
{
   for (int iMenu = 0; iMenu < uml.nMenus(); iMenu++) {
      userMenu &um = uml[iMenu];
      for (int iSubMenu = 0; iSubMenu < um.nSubMenus(); iSubMenu++) {
         userSubMenu &sm = um[iSubMenu];
         if (!sm.isSeparator() && !sm.isPlugIn()) { // Separators and plug-ins already have valid ids.
            UINT id = ud.id(sm.dbox());
            if (id == 0) id = uhl.id(sm.dbox());
            if (id > 0) {
               if (sm.isStd() && sm.pageNo() > 0) id += sm.pageNo();
               sm.dbid(id);
            }
            else {
               globalMsg.send(msgInfo::mWarning, 
                  "Submenu entry references dialog id '%s,' which cannot be found.\t",
                  sm.dbox());
            }
         }
      }
   }
}

//------------------------------------------------------------------------------

void repository::setLimits(const char *page, const char *key, tokenizer &t)
{
   gaugeMapIter iG = _gauge[page].find(key);
   if (iG != _gauge[page].end())
      (*iG).second->set(t);
   else {
      gaugeConfiguration *g = new gaugeConfiguration();
      g->set(t);
      _gauge[page][_strdup(key)] = g;
   }
}

//------------------------------------------------------------------------------

void repository::setGaugeRef(const char *page, const char *gaugeId, const char *ref)
{
   gaugeMapIter iG = _gauge[page].find(gaugeId);
   if (iG != _gauge[page].end())
      strcpy((*iG).second->ref, ref);
   else {
      // Not there yet, make one... 
      gaugeConfiguration *g = new gaugeConfiguration(ref);
      _gauge[page][_strdup(gaugeId)] = g;
   }
}

void repository::setGaugeRef(const char *page, tokenizer &t, int maxGaugeNo)
{
   // Error check: t[0] is gauge1 through gauge6 or gauge8...
   setGaugeRef(page, t[0], t[1]);
}

void repository::setGaugeFam(const char *page, tokenizer &t, int maxGaugeNo)
{
   // Error check: t[0] is gauge1 through gauge6 or gauge8...
   if (t.size() == 5 && strlen(t[0]) == 6) {
      char gaugeId[8];
      strcpy(gaugeId, t[0]);
      strcat(gaugeId, "a");
      for (int i = 0; i < 4; i++) {
         gaugeId[6] = 'a' + i;
         setGaugeRef(page, gaugeId, t[i+1]);
      }
   }
}

//------------------------------------------------------------------------------

double repository::barHysteresis() { return _barHysteresis; }
bool   repository::fieldColoring() { return _fieldColoring; }

COLORREF repository::color(const char *colorName)
{
   if (_colors.find(colorName) == _colors.end()) return UNDEFINED_COLOR;
   return _colors[colorName];
}

void repository::setColor(const char *colorName, COLORREF color)
{
   colorMapIter iC = _colors.find(colorName);
   if (iC != _colors.end())
      (*iC).second = color;
   else
      _colors[_strdup(colorName)] = color;
}

COLORREF repository::getColor(tokenizer &t)
{
   if (t.size() == 4 && t.t(0) == tokenizer::Tnum)
      return RGB(t.v(1),t.v(2),t.v(3));
   else
      return color(t[1]);
}

//------------------------------------------------------------------------------

typedef enum {
   blockNone,
   blockAccelerationWizard,
   blockAutoTune,
   blockBurstMode,
   blockColorMap,
   blockConstants,
   blockCurveEditor,
   blockDatalog,
   blockDefaults,
   blockFrontPage,
   blockGaugeColors,
   blockGaugeConfigurations,
   blockMegaTune,
   blockMenu,
   blockOutputChannels,
   blockRunTime,
   blockTableEditor,
   blockTuning,
   blockUserDefined,
   blockUnits
} blockType;

//#define strEq(s) (strncmp(s,t[0],strlen(s)) == 0)
#define strEq(s) (t.eq(s,0))

//------------------------------------------------------------------------------

static std::map<const char*, bool, ltstr> setList;

static void varSet(const char *setName, bool value)
{
   std::map<const char*, bool, ltstr>::iterator _set = setList.find(setName);
   if (_set == setList.end())
      setList[_strdup(setName)] = value;
   else
     (*_set).second = value;
}

static bool isVarSet(const char *setName)
{
   if (setList.find(setName) == setList.end()) return false;
   return setList[setName];
}

static bool isVarDef(const char *setName, msgInfo &msg)
{
   if (setList.find(setName) == setList.end()) {
      msg.send(msgInfo::mWarning, 
         "Conditional check references undefined value '%s'.     \n\n"
         "Only first reference will be reported.",
         setName);
      varSet(setName, false);
      return false;
   }
   return true;
}

//------------------------------------------------------------------------------

struct ifBlock {
   char     *fileName;
   int       lineNo;
   enum      readState { yetToRead, nowReading, doneReading };
   readState reading;
   readState restoreState;
   ifBlock(const char *fn, int ln, readState curRd)
    : lineNo      (ln),
      reading     (yetToRead),
      restoreState(curRd),
      fileName    (_strdup(fn))
   { }
  ~ifBlock() { /*free(fileName);*/ }

   static const char *sReading(readState s) { return s == nowReading ? "true" : "false"; }
};

static std::stack<ifBlock> ifStack;

//------------------------------------------------------------------------------

static void checkPageCount(const char *item, int n, msgInfo &msg)
{
   if (n != mdb.cDesc.nPages()) {
      msg.send(msgInfo::mError | msgInfo::mExit, 
         "The number of %s values (%d) isn't consistent with     \n"
         "\t\"nPages = %d\" in the [Constants] definitions.\n\n"
         "MegaTune is terminating.",
         item, n, mdb.cDesc.nPages());
   }
}

static void checkOPageCount(const char *item, int n, msgInfo &msg)
{
   if (n != mdb.cDesc.nOPages()) {
      msg.send(msgInfo::mError | msgInfo::mExit, 
         "The number of %s values (%d) isn't consistent with     \n"
         "\t\"nPages = %d\" in the [OutputChannels] definitions.\n\n"
         "MegaTune is terminating.",
         item, n, mdb.cDesc.nOPages());
   }
}

//------------------------------------------------------------------------------

static ifBlock::readState reading = ifBlock::nowReading;

#define checkFor2nd(ms) \
    if (nT < 3) { \
       msg.send(msgInfo::mWarning, ms); \
       continue; \
    }

#define INDENT 3*msg.level,""
extern bool writeXML;

void repository::doRead(FILE *iniFile, const char *fileName, int depth)
{
   msgInfo msg(fileName, depth);
#ifdef _DEBUG
   if (_heapchk() != _HEAPOK) {
      msg.send(msgInfo::mError | msgInfo::mExit, "MT Heap Error"__FILE__" %d", __LINE__);
   }
#endif

   char      lineBuffer[512];
   blockType block = blockNone;

   tokenizer t;
   try {
   while (fgets(lineBuffer, sizeof(lineBuffer)-1, iniFile)) {
      msg.lineNo++;
      if (t.parse(lineBuffer) == 0) continue;
      int nT = t.size();

#ifdef _DEBUG
      if (_heapchk() != _HEAPOK) {
         msg.send(msgInfo::mError | msgInfo::mExit, "MT Heap Error"__FILE__" %d", __LINE__);
      }
#endif

      //------------------------------------------------------------------------
      //--  Read control directives.  ------------------------------------------

      if (t.t(0) == tokenizer::Tdir) {
         if (nT < 2) {
            msg.send(msgInfo::mWarning, "'#' has no directive, ignored.\t");
            continue;
         }

         // Just ignore the group directives used for the configuration editor.
         msg.ssize = ifStack.size();
         if (strcmp("group", t[1]) == 0) {
            msg.send(msgInfo::mInfo, "#group %s", t[2]);
            msg.level++;
            continue;
         }
         if (strcmp("endgroup", t[1]) == 0) {
            msg.level--;
            continue;
         }

         if (strcmp("if", t[1]) == 0) { //--------------------------------------
            checkFor2nd("'#if' has no setting value, ignored.\t");
            isVarDef(t[2], msg);

            ifStack.push(ifBlock(msg.fileName, msg.lineNo, reading));
            if (reading == ifBlock::nowReading) {
               if (isVarSet(t[2]))
                  reading = ifBlock::nowReading;
               else
                  reading = ifBlock::yetToRead;
            }
            else {
               reading = ifBlock::doneReading;
            }
            ifStack.top().reading = reading;
            msg.send(msgInfo::mInfo, "#if %s, including=%s", t[2], ifBlock::sReading(reading));
            continue;
         }

         else if (strcmp("ifnot", t[1]) == 0) { //------------------------------
            checkFor2nd("'#ifnot' has no setting value, ignored.\t");
            isVarDef(t[2], msg);

            ifStack.push(ifBlock(msg.fileName, msg.lineNo, reading));
            if (reading == ifBlock::nowReading) {
               if (!isVarSet(t[2]))
                  reading = ifBlock::nowReading;
               else
                  reading = ifBlock::yetToRead;
            }
            else {
               reading = ifBlock::doneReading;
            }
            ifStack.top().reading = reading;
            msg.send(msgInfo::mInfo, "#ifnot %s, including=%s", t[2], ifBlock::sReading(reading));
            continue;
         }

         else if (strcmp("elif", t[1]) == 0) { //-------------------------------
            checkFor2nd("'#elif' has no setting value, ignored.\t");
            isVarDef(t[2], msg);

            if (msg.ssize == 0) {
               msg.send(msgInfo::mError | msgInfo::mExit,
                  "'#elif' without '#if'.  Look for an extra '#endif' somewhere.\n\n"
                  "MegaTune is terminating.");
            }

            if (reading == ifBlock::yetToRead) {
               if (isVarSet(t[2]))
                  reading = ifBlock::nowReading;
               else
                  reading = ifBlock::yetToRead;
            }
            else {
               reading = ifBlock::doneReading;
            }
            ifStack.top().reading = reading;
            msg.send1(msgInfo::mInfo, "#elif %s, including=%s", t[2], ifBlock::sReading(reading));
            continue;
         }

         else if (strcmp("else", t[1]) == 0) { //-------------------------------
            if (reading == ifBlock::yetToRead)
               reading = ifBlock::nowReading;
            else
               reading = ifBlock::doneReading;
            ifStack.top().reading  = reading;
            msg.send1(msgInfo::mInfo, "#else, including=%s", ifBlock::sReading(reading));
            continue;
         }
         
         else if (strcmp("endif", t[1]) == 0) { //------------------------------
            if (msg.ssize == 0) {
               msg.send(msgInfo::mWarning, "Extra '#endif'.\n\nThings are going to be goofed up.\t");
            }
            else {
               reading = ifStack.top().restoreState;
               ifStack.pop();
               msg.send1(msgInfo::mInfo, "#endif, including=%s", ifBlock::sReading(reading?ifBlock::nowReading:ifBlock::yetToRead));
            }
            continue;
         }
      }

      //------------------------------------------------------------------------
      if (reading != ifBlock::nowReading) continue;
      //------------------------------------------------------------------------

      if (t.t(0) == tokenizer::Tdir) {
         if (strcmp("include", t[1]) == 0) {
            checkFor2nd("'#include' has no file specified, ignored.");
            FILE *incFile = cfgOpen(t[2], "r");
            if (incFile) {
               doRead(incFile, t[2], msg.level+1);
               cfgClose(incFile);
            }
            else {
               msg.send(msgInfo::mError | msgInfo::mExit,
                  "The included file '%s' could not be opened.\n\n"
                  "MegaTune is terminating.", t[2]);
            }
         }
         else if (strcmp("alert", t[1]) == 0) {
            checkFor2nd("'#alert' has no message specified, ignored.");
            msg.send0(msgInfo::mAlert, "%s", byteString(t[2]).xlate().ptr());
         }
         else if (strcmp("log", t[1]) == 0) {
            checkFor2nd("'#log' has no message specified, ignored.");
            msg.send0(msgInfo::mInfo, "%s", byteString(t[2]).xlate().ptr());
         }
         else if (strcmp("set", t[1]) == 0) {
            checkFor2nd("'#set' has no setting value, ignored.");
            varSet(t[2], true);
            msg.send0(msgInfo::mInfo, "  set %-20s (true)", t[2]);
         }
         else if (strcmp("unset", t[1]) == 0) {
            checkFor2nd("'#unset' has no setting value, ignored.");
            varSet(t[2], false);
            msg.send0(msgInfo::mInfo, "unset %-20s (false)", t[2]);
         }
         else if (strcmp("error", t[1]) == 0) {
            checkFor2nd("'#error' has no message specified, ignored.");
            msg.send0(msgInfo::mError, "%s", byteString(t[2]).xlate().ptr());
         }
         else if (strcmp("exit", t[1]) == 0) {
            msg.send(msgInfo::mExit, "#exit");
         }
         else {
            msg.send(msgInfo::mWarning, "Unrecognized directive '%s', ignored.", t[1]);
         }
         continue;
      }

      if (strEq(S__AccelerationWizard_ )) { block = blockAccelerationWizard;  continue; }
      if (strEq(S__AutoTune_           )) { block = blockAutoTune;            continue; }
      if (strEq(S__BurstMode_          )) { block = blockBurstMode;           continue; }
      if (strEq(S__ColorMap_           )) { block = blockColorMap;            continue; }
      if (strEq(S__Constants_          )) { block = blockConstants;           continue; }
      if (strEq(S__CurveEditor_        )) { block = blockCurveEditor;         continue; }
      if (strEq(S__Datalog_            )) { block = blockDatalog;             continue; }
      if (strEq(S__Defaults_           )) { block = blockDefaults;            continue; }
      if (strEq(S__FrontPage_          )) { block = blockFrontPage;           continue; }
      if (strEq(S__GaugeColors_        )) { block = blockGaugeColors;         continue; }
      if (strEq(S__GaugeConfigurations_)) { block = blockGaugeConfigurations; continue; }
      if (strEq(S__MegaTune_           )) { block = blockMegaTune;            continue; }
      if (strEq(S__Menu_               )) { block = blockMenu;                continue; }
      if (strEq(S__RunTime_            )) { block = blockRunTime;             continue; }
      if (strEq(S__OutputChannels_     )) { block = blockOutputChannels;      continue; }
      if (strEq(S__TableEditor_        )) { block = blockTableEditor;         continue; }
      if (strEq(S__Tuning_             )) { block = blockTuning;              continue; }
      if (strEq(S__Units_              )) { block = blockUnits;               continue; }
      if (strEq(S__UserDefined_        )) { block = blockUserDefined;         continue; }
      if (t[0][0] == ']')                 { block = blockNone;                continue; }

#define UNEXPECTED msg.send(msgInfo::mWarning, "Unexpected or unsupported token '%s'", t[0]);

      switch (block) {
         //---------------------------------------------------------------------
         case blockNone: //-----------------------------------------------------
            // Nothing yet...
            UNEXPECTED
            break;

         //---------------------------------------------------------------------
         case blockMegaTune: //-------------------------------------------------
            if (strEq(S_writeXML)) { writeXML = t.eq(S_true, 1); break; }

            if (strEq(S_MTversion)     ) {
               MTversion = t.v(1);
               if (MTversion != VERSION) {
                  msg.send(msgInfo::mWarning,
                     "The file '%s' is version %.2f, but was expected to be %.2f.\n\n"
                     "Strange behavior may result from this, update '%s' as soon as possible.",
                     msg.fileName, MTversion, VERSION, msg.fileName);
               }
               break;
            }

            if (strEq(S_queryCommand  )) { mdb.cDesc.setQueryCommand(t[1]);   break; }
            if (strEq(S_versionInfo   )) { mdb.cDesc.setVersionInfo (t[1]);   break; }
            if (strEq(S_signature     )) {
               switch (t.t(1)) {
                  case tokenizer::Tstr: mdb.cDesc.setSignature(t[1],                     msg.fileName); break;
                  case tokenizer::Tnum: mdb.cDesc.setSignature(byteString(BYTE(t.v(1))), msg.fileName); break;
               }
               break;
            }
            UNEXPECTED
            break;

         //---------------------------------------------------------------------
         case blockBurstMode: //------------------------------------------------
            if (strEq(S_commRate)     ) { mdb.burstCommRate = t.v(1); break; }
            if (strEq(S_commPort)     ) { mdb.burstCommPort = t.v(1); break; }
            if (strEq(S_getCommand)   ) { mdb.cDesc.setOchBurstCommand(t[1], 0); break; }
            if (strEq(S_initialDeltaT)) { burstLog::suggestedDeltaT(t.v(1)); break; }
            UNEXPECTED
            break;

         //---------------------------------------------------------------------
         case blockColorMap: //-------------------------------------------------
            if (strEq(S_fieldColoring)) { _fieldColoring = t.eq(S_on, 1); break; }
            setColor(t[0], RGB(t.i(1), t.i(2), t.i(3)));
            break;

         //---------------------------------------------------------------------
         case blockConstants: //------------------------------------------------
            if (strEq(S_nPages     )) { mdb.cDesc.setNPages(int(t.v(1))); break; }
            if (strEq(S_endianness )) { mdb.cDesc.setEndianness (t[1]);   break; }
            if (strEq(S_pageSize)) {
               checkPageCount(S_pageSize, nT-1, msg);
               // Array of ints, one per page.
               for (int i = 0; i < nT-1; i++) {
                  mdb.cDesc.setPageSize(int(t.v(i+1)), i);
               }
               break;
            }
            if (strEq(S_verify)) {
               mdb.cDesc.setVerify(t.eq(S_on, 1));
               break;
            }
            if (strEq(S_burnCommand)) {
               //checkPageCount(S_burnCommand, nT-1, msg);
               for (int i = 0; i < nT-1; i++) mdb.cDesc.setBurnCommand(t[i+1], i);
               break;
            }
            if (strEq(S_pageActivate)) {
               checkPageCount(S_pageActivate, nT-1, msg);
               for (int i = 0; i < nT-1; i++) mdb.cDesc.setPageActivate(t[i+1], i);
               break;
            }
            if (strEq(S_pageIdentifier)) {
               checkPageCount(S_pageIdentifier, nT-1, msg);
               for (int i = 0; i < nT-1; i++) mdb.cDesc.setPageIdentifier(t[i+1], i);
               break;
            }

            if (strEq(S_pageReadCommand)) { //== Read commands ==
               checkPageCount(S_pageReadCommand, nT-1, msg);
               for (int i = 0; i < nT-1; i++) mdb.cDesc.setPageReadWhole(t[i+1], i);
               break;
            }
            if (strEq(S_pageChunkRead)) {
               checkPageCount(S_pageChunkRead, nT-1, msg);
               for (int i = 0; i < nT-1; i++) mdb.cDesc.setPageReadChunk(t[i+1], i);
               break;
            }
            if (strEq(S_pageValueRead)) {
               checkPageCount(S_pageValueRead, nT-1, msg);
               for (int i = 0; i < nT-1; i++) mdb.cDesc.setPageReadValue(t[i+1], i);
               break;
            }

            if (strEq(S_pageWriteCommand)) { //== Write commands ==
               checkPageCount(S_pageWriteCommand, nT-1, msg);
               for (int i = 0; i < nT-1; i++) mdb.cDesc.setPageWriteWhole(t[i+1], i);
               break;
            }
            if (strEq(S_pageChunkWrite)) {
               checkPageCount(S_pageChunkWrite, nT-1, msg);
               for (int i = 0; i < nT-1; i++) mdb.cDesc.setPageWriteChunk(t[i+1], i);
               break;
            }
            if (strEq(S_pageValueWrite)) {
               checkPageCount(S_pageValueWrite, nT-1, msg);
               for (int i = 0; i < nT-1; i++) mdb.cDesc.setPageWriteValue(t[i+1], i);
               break;
            }

            if (strEq(S_interWriteDelay)) {
               mdb.cDesc.setDelay(t.i(1));
               break;
            }
            if (strEq(S_writeBlocks)) {
               mdb.cDesc.setWriteBlocks(t.eq(S_on, 1));
               break;
            }
            if (strEq(S_pageActivationDelay)) {
               mdb.cDesc.setPageActivationDelay(t.i(1));
               break;
            }
            if (strEq(S_blockReadTimeout)) {
               mdb.cDesc.setBlockReadTimeout(t.i(1));
               break;
            }

            if (strEq(S_page)) {
               currentCP = t.i(1)-1;

               // Validate it is 0...nPages-1
               if (currentCP < 0 || currentCP >= mdb.cDesc.nPages()) {
                  msg.send(msgInfo::mError | msgInfo::mExit,
                     "The \"page = %d\" setting is outside the range of \"nPages = %d\"     \n"
                     "\tin the [Constants] definitions.\n\n"
                     "MegaTune is terminating.",
                     currentCP+1, mdb.cDesc.nPages());
               }
               break;
            }

            //------------------------------------------------------------------
            //-- We've got a new symbol, check page number and then store it. --
            {
               if (currentCP < 0 || currentCP > mdb.cDesc.nPages()) {
                  throw "current page out of range";
               }

               // ??? check number of values...
               symbol *s = new symbol();
               if (t.eq(S_scalar, 1)) {
                  if (nT > 10)
                     msg.send(msgInfo::mWarning,
                        "Constant definition for \"%s\" has too many parameters %d, 10 expected.\t\n\n",
                        t[0], nT);
                  s->setCScalar(t[0], t[2], currentCP, int(t.v(3)),        t[4], t.v(5), t.v(6), t.v(7), t.v(8), t.v(9));
               }
               else if (t.eq(S_array, 1)) {
                  if (nT > 11)
                     msg.send(msgInfo::mWarning,
                        "Constant definition for \"%s\" has too many parameters %d, 11 expected.\t\n\n",
                        t[0], nT);
                  s->setCArray (t[0], t[2], currentCP, int(t.v(3)), t[5],  t[4], t.v(6), t.v(7), t.v(8), t.v(9), t.v(10));
               }
               else if (t.eq(S_bits, 1)) {
                  s->setCBits  (t[0], t[2], currentCP, int(t.v(3)), t[4]);
                  int i;
                  for (i = 5; i < nT; i++) {
                     s->userStrings.push_back(t[i]);
                  }
                  int ofs = s->bitOfs();
                  for (i = nT-5; i < s->nValues(); i++) {
                     char autoLbl[10];
                     sprintf(autoLbl, "%d", i+ofs); // Should be "0", "1"... with zero offset.
                     s->userStrings.push_back(autoLbl);
                  }
               }
               else {
                  msg.send(msgInfo::mWarning,
                     "Constant definition for \"%s\" is corrupted, starts with \"%s\"     \n\n"
                     "This entry is ignored.", t[0], t[1]);
                  break;
               }
               mdb.cDesc.addSymbol(s);

               // Validate page boundaries.
               if (s->offset()+s->size() > mdb.cDesc.pageSize(currentCP)) {
                  msg.send(msgInfo::mError | msgInfo::mExit,
                     "The byte range of %s values (%d-%d) overwrite the     \n"
                     "\tpage boundaries of 0-%d in the [Constants] definitions.\n\n"
                     "MegaTune is terminating.",
                     t[0],
                     s->offset(), s->offset()+s->size()-1,
                     mdb.cDesc.pageSize(currentCP)-1);
               }
               break;
            }
            UNEXPECTED
            break;

         //---------------------------------------------------------------------
         case blockOutputChannels: //-------------------------------------------
            // ??? grab the expressions and store them in order.
            if (nT < 2) {
               msg.send(msgInfo::mWarning, "Missing argument to '%s'", t[0]);
               break;
            }

            if (strEq(S_nPages      )) { mdb.cDesc.setNOPages(int(t.v(1))); break; }
            if (strEq(S_ochBlockSize)) {
               checkOPageCount(S_ochBlockSize, nT-1, msg);
               for (int i = 0; i < nT-1; i++) mdb.cDesc.setOchBlockSize(int(t.v(i+1)), i);
               break;
            }
            if (strEq(S_ochGetCommand)) {
               checkOPageCount(S_ochGetCommand, nT-1, msg);
               for (int i = 0; i < nT-1; i++) mdb.cDesc.setOchGetCommand(t[i+1], i);
               break;
            }
            if (strEq(S_pollingPattern)) {
               for (int i = 0; i < nT-1; i++) mdb.cDesc.setOchPollPattern(t[i+1], i);
               break;
            }

            if (strEq(S_page)) {
               currentOP = t.i(1)-1;

               // Validate it is 0...nPages-1
               if (currentOP < 0 || currentOP >= mdb.cDesc.nOPages()) {
                  msg.send(msgInfo::mError | msgInfo::mExit,
                     "The \"page = %d\" setting is outside the range of \"nPages = %d\"     \n"
                     "\tin the [OutputChannels] definitions.\n\n"
                     "MegaTune is terminating.",
                     currentOP+1, mdb.cDesc.nOPages());
               }
               break;
            }

            {
               if (currentOP < 0 && mdb.cDesc.nOPages() == 1) currentOP = 0;
               symbol *s = new symbol();
               if (t.eq(S_scalar, 1)) {
                  // name  hunk    type  ofs  units  scale  trans
                  // rpm = scalar, S16,    7, "RPM", 1.000, 0.000
                  s->setOScalar(t[0], t[2], currentOP, int(t.v(3)), t[4], t.v(5), t.v(6));
               }
               else if (t.eq(S_bits, 1)) {
                  // name  hunk    type  ofs  bitSpec
                  // sch = bits,   U08,   11, [2:2]
                  s->setOBits  (t[0], t[2], currentOP, int(t.v(3)), t[4]);
               }
               else if (t.t(1) == tokenizer::Texp) {
                  // var = { expr } [, "units"]
                  s->setExpr   (t[0], t.stripped(1), t[2], msg.fileName, msg.lineNo);
               }
               else {
                  msg.send(msgInfo::mWarning,
                     "OutputChannel definition for \"%s\" is corrupted, starts with \"%s\"     \n\n"
                     "This entry is ignored.", t[0], t[1]);
                  break;
               }
               mdb.cDesc.addSymbol(s);

               // Validate page boundaries.
               if (s->offset()+s->size() > mdb.cDesc.ochBlockSize(currentOP)) {
                  msg.send(msgInfo::mError | msgInfo::mExit,
                     "The byte range of %s values (%d-%d) overwrite the     \n"
                     "\tmemory boundaries of 0-%d in page %d of the [OutputChannels] definitions.\n\n"
                     "MegaTune is terminating.",
                     t[0],
                     s->offset(), s->offset()+s->size()-1,
                     mdb.cDesc.ochBlockSize(currentOP)-1, currentOP);
               }
               break;
            }
            UNEXPECTED
            break;

         //---------------------------------------------------------------------
         case blockUnits: //----------------------------------------------------
            if (strEq(S_temperature)) {
               if (nT == 2) {
                  if (toupper(*t[1]) == 'C') mdb.therm = msDatabase::Celsius;
                  if (toupper(*t[1]) == 'F') mdb.therm = msDatabase::Fahrenheit;
               }
               break;
            }
            if (strEq(S_TPS)) {
               if (nT == 2) {
                  if (toupper(*t[1]) == 'R') mdb.rawTPS = true;
                  if (toupper(*t[1]) == '%') mdb.rawTPS = false;
               }
               break;
            }
            UNEXPECTED
            break;

         //---------------------------------------------------------------------
         case blockGaugeConfigurations: //--------------------------------------
            setLimits(S_GaugeConfigurations, t[0], t);
            break;

         //---------------------------------------------------------------------
         case blockFrontPage: //------------------------------------------------
            // New style from gauge1 through gauge8.
            if (t.match(S_gauge, 0)     ) { setGaugeRef(S_FrontPage, t, 8); break; }
            if (strEq(S_egoLEDs        )) { frontEGO(t);                    break; } //=0,1.0,0.5

            // Built-in indicators                  name         -bg   -fg   +bg   +fg
            if (strEq(S_saved          )) { uil.set(S_saved,     t[1], t[2], t[3], t[4]);  break; } 
            if (strEq(S_logging        )) { uil.set(S_logging,   t[1], t[2], t[3], t[4]);  break; } 
            if (strEq(S_connected      )) { uil.set(S_connected, t[1], t[2], t[3], t[4]);  break; } 

            // User-defined indicators
            if (strEq(S_indicator      )) {
               if (t.eq(S_restart,1)) { uil.reset(); break; }
               //              expression     -Txt  +Txt  -bg   -fg   +bg   +fg
               uil.add("main", t.stripped(1), t[2], t[3], t[4], t[5], t[6], t[7]);
               break;
            }

            UNEXPECTED
            break;

         //---------------------------------------------------------------------
         case blockAccelerationWizard: //---------------------------------------
            if (strEq(S_mapDotBar      )) { aeMapDot(t); break; } //=0,1.0
            if (strEq(S_tpsDotBar      )) { aeTpsDot(t); break; } //=0,1.0
            UNEXPECTED
            break;

         //---------------------------------------------------------------------
         case blockRunTime: //--------------------------------------------------
            if (strEq(S_barHysteresis  )) { _barHysteresis = t.v(1); break; }
            if (strEq(S_egoBar         )) { runEGO(t); break; } //=0,1.0
            if (strEq(S_coolantBar     )) { runCT (t); break; } //=-40,215
            if (strEq(S_batteryBar     )) { runBAT(t); break; } //=6,15
            if (strEq(S_throttleBar    )) { runTR (t); break; } //=0,100
            if (strEq(S_gammaEBar      )) { runGE (t); break; } //=0,100
            if (strEq(S_mapBar         )) { runMAP(t); break; } //=0,255
            if (strEq(S_matBar         )) { runMAT(t); break; } //=-40,215
            if (strEq(S_rpmBar         )) { runRPM(t); break; } //=0,8000
            if (strEq(S_pulseWidthBar  )) { runPW (t); break; } //=0,25.5
            if (strEq(S_dutyCycleBar   )) { runDC (t); break; } //=0,100
            if (strEq(S_egoCorrBar     )) { runEGC(t); break; } //=0,200
            if (strEq(S_baroCorrBar    )) { runBC (t); break; } //=0,200
            if (strEq(S_warmupCorrBar  )) { runWC (t); break; } //=0,200
            if (strEq(S_airdenCorrBar  )) { runADC(t); break; } //=0,200
            if (strEq(S_veCorrBar      )) { runVE (t); break; } //=0,200
            if (strEq(S_accCorrBar     )) { runACC(t); break; } //=0,200
            UNEXPECTED
            break;

         //---------------------------------------------------------------------
         case blockTuning: //---------------------------------------------------
            // New style from gauge1 through gauge6.
            if (strEq(S_gaugeColumns   )) {
               gaugeColumns = t.v(1);
               if (gaugeColumns < 1) gaugeColumns = 1;
               if (gaugeColumns > 2) gaugeColumns = 2;
               break; // Skip over next line which would match.
            }
            if (t.match(S_gauge, 0)) { setGaugeFam(S_Tuning, t, 6); break; }

            if (strEq(S_pageButtons    )) {
               if (nT == 5) {
                  for (int i = 1; i < 5; i++) {
                     strncpy(_pageLbl[i-1], t[i], sizeof(_pageLbl[0])-1);
                  }
               }
               break;
            }

            if (strEq(S_egoLEDs        )) { tuneEGO(t); break; } //=0,1.0,0.5
            if (strEq(S_veBar          )) { tuneVEB(t); break; } //=0,120
            if (strEq(S_spotDepth      )) { tuneSD (t); break; }
            if (strEq(S_cursorDepth    )) { tuneCD (t); break; }
            if (strEq(S_font           )) { t_fontFace = t[1]; t_fontSize = t.i(2); break; }
            if (strEq(S_backgroundColor)) { t_backgroundColor = getColor(t); break; }
            if (strEq(S_cursorColor    )) { t_cursorColor     = getColor(t); break; }
            if (strEq(S_cursorText     )) { t_cursorText      = getColor(t); break; }
            if (strEq(S_gridColor      )) { t_gridColor       = getColor(t); break; }
            if (strEq(S_spotColor      )) { t_spotColor       = getColor(t); break; }
            if (strEq(S_spotText       )) { t_spotText        = getColor(t); break; }
            if (strEq(S_textColor      )) { t_textColor       = getColor(t); break; }
            UNEXPECTED
            break;

         //---------------------------------------------------------------------
         case blockGaugeColors: //----------------------------------------------
            if (strEq(S_alertColor     )) { g_alertColor      = getColor(t); break; }
            if (strEq(S_ledColor       )) { g_ledColor        = getColor(t); break; }
            if (strEq(S_ledAlertColor  )) { g_ledAlertColor   = getColor(t); break; }
            if (strEq(S_needleColor    )) { g_needleColor     = getColor(t); break; }
            if (strEq(S_rangeColor     )) { g_rangeColor      = getColor(t); break; }
            if (strEq(S_scaleColor     )) { g_scaleColor      = getColor(t); break; }
            if (strEq(S_titleColor     )) { g_titleColor      = getColor(t); break; }
            if (strEq(S_valueColor     )) { g_valueColor      = getColor(t); break; }
            if (strEq(S_backgroundColor)) { g_backgroundColor = getColor(t); break; }
            if (strEq(S_faceColor      )) { g_faceColor       = getColor(t); break; }
            if (strEq(S_bgWarningColor )) { g_bgWarningColor  = getColor(t); break; }
            if (strEq(S_bgDangerColor  )) { g_bgDangerColor   = getColor(t); break; }
            UNEXPECTED
            break;

         //---------------------------------------------------------------------
         case blockMenu: { //---------------------------------------------------
            static CString currentMenu = "main";
            if (strEq(S_menuDialog)) { currentMenu =  t[1];                                       break; }
            if (strEq(S_menu      )) { uml.addMenu   (currentMenu, t[1], t.stripped(2));          break; }
            if (strEq(S_subMenu   )) { uml.addSubMenu(t[1], t[2], t.i(3)-1, t.stripped(4)      ); break; }
            if (strEq(S_plugIn    )) {
               userSubMenu *sm = uml.addSubMenu(t[1], t[2], 0, t.stripped(4), true);
               userPlugIn::verbType     action = userPlugIn::none;
               if (t.eq("minimize", 3)) action = userPlugIn::minimize;
               if (t.eq("exit",     3)) action = userPlugIn::exit;
               sm->action(action);
               break;
            }
            }
            UNEXPECTED
            break;

         //---------------------------------------------------------------------
         case blockUserDefined: //----------------------------------------------
            if (strEq(S_dialog)) {
               static int nUDs = 0;
               currentUD = ud.id(t[1]) - ID_USER_DEFINED_00;
               if (currentUD < 0 || currentUD > nUDs) {
                  currentUD = nUDs++;
                  ud.addDialog(nT > 1 ? t[1]     : "Missing menu name",
                               nT > 2 ? t[2]     : "Missing menu title",
                               nT > 3 ? t.i(3)-1 : -1);
               }
               break;
            }
            if (strEq(S_array1D)) {
               ud[currentUD].add(t[1], t[2], t[3], t.stripped(4));
               break;
            }
            if (nT > 1 && strEq(S_field)) {
               if (nT > 4)  {
                  msg.send(msgInfo::mWarning,
                     "Version 2.20-style field definitions are no longer valid.\n\n"
                     "Ignoring line containing field=\"%s\"", t[1]);
                  break;
               }
               ud[currentUD].add(t[1], t[2], t.stripped(3));
               break;
            }
            if (strEq(S_topicHelp)) {
               ud[currentUD].setHelp(t[1]);
               break;
            }

            if (strEq(S_help   )) { uhl.addHelp(t[1], t[2]); break; }
            if (strEq(S_webHelp)) { uhl.addUri (t[1]);       break; }
            if (strEq(S_text   )) { uhl.addText(t[1]);       break; }

            UNEXPECTED
            break;

         //---------------------------------------------------------------------
         case blockCurveEditor: //----------------------------------------------
            if (strEq(S_curve)) {
               static int nUCs = 0;
               currentUC = ud.id(t[1]) - ID_USER_CURVE_00;
               if (currentUC < 0 || currentUC >= curveDialog::nUC) {
                  currentUC = nUCs++;
                  if (currentUC >= curveDialog::nUC) {
                     msg.send(msgInfo::mError | msgInfo::mExit,
                        "The number of curve editors exceeds %d.\n\n"
                        "MegaTune is terminating.", curveDialog::nUC);
                  }
                  uc[currentUC].setId    (t[1]);
                  uc[currentUC].setTitle (nT > 2 && *t[2] ? t[2] : "No Table Title");
                  uc[currentUC].setPageNo(nT > 3 ? int(t.v(3))-1 : -1);

                  ud.registerDialog(t[1], ID_USER_CURVE_00+currentUC);
               }
               break;
            }

            if (currentUC == -1) {
               msg.send(msgInfo::mError | msgInfo::mExit,
                  "'%s' specified before 'curve'.\n\n"
                  "MegaTune is terminating.", t[0]);
            }

            if (strEq(S_topicHelp)) {
               uc[currentUC].setHelp(t[1]);
               break;
            }
            if (strEq(S_columnLabel)) {
               uc[currentUC].setLabels(t[1], t[2]);
               break;
            }
            if (strEq(S_xAxis)) {
               uc[currentUC].setXaxis(t.v(1), t.v(2), t.i(3));
               break;
            }
            if (strEq(S_yAxis)) {
               uc[currentUC].setYaxis(t.v(1), t.v(2), t.i(3));
               break;
            }
            if (strEq(S_xBins)) { // array-constant, variable
               uc[currentUC].setX(t[1], t[2], !t.eq(S_readonly, 3));
               break;
            }
            if (strEq(S_yBins)) { // array-constant
               uc[currentUC].setY(t[1],   "", !t.eq(S_readonly, 2));
               break;
            }
            if (strEq(S_gauge)) {
               uc[currentUC].setGauge(t[1]);
               break;
            }

            UNEXPECTED
            break;

         //---------------------------------------------------------------------
         case blockTableEditor: //----------------------------------------------
            if (strEq(S_table)) {
               static int nUTs = 0;
               currentUT = ud.id(t[1]) - ID_USER_TABLE_00;
               if (currentUT < 0 || currentUT >= tuningDialog::nUT) {
                  currentUT = nUTs++;

                  if (currentUT >= tuningDialog::nUT) {
                     msg.send(msgInfo::mError | msgInfo::mExit,
                        "The number of table editors exceeds %d.\n\n"
                        "MegaTune is terminating.",
                        tuningDialog::nUT);
                  }
                  // table = table_id, map_id, "title", page
                  ut[currentUT].setIds(t[1], t[2]);
                  ut[currentUT].title    = nT > 3 && *t[3] ? t[3] : "No Table Title";
                  ut[currentUT].pageNo   = nT > 4 ? int(t.v(4))-1 : -1;

                  ud.registerDialog(t[1], ID_USER_TABLE_00 +currentUT);
                  ud.registerDialog(t[2], ID_USER_TUNING_00+currentUT);
               }
               break;
            }

            if (currentUT == -1) {
               msg.send(msgInfo::mError | msgInfo::mExit,
                  "'%s' specified before 'table'.\n\n"
                  "MegaTune is terminating.", t[0]);
            }

            if (strEq(S_topicHelp)) { // topicHelp = tableHelp, tuningHelp
               ut[currentUT].setHelp(t[1], t[2]);
               break;
            }

            if (strEq(S_xBins)) { // xBins = constant, variable
               ut[currentUT].xName     = t[1];
               ut[currentUT].xOchName  = t[2];
               ut[currentUT].xEditable = !t.eq(S_readonly, 3);
               break;
            }
            if (strEq(S_yBins)) {
               ut[currentUT].yName     = t[1];
               ut[currentUT].yOchName  = t[2];
               ut[currentUT].yEditable = !t.eq(S_readonly, 3);
               break;
            }
            if (strEq(S_zBins)) {
               ut[currentUT].zName     = t[1];
               ut[currentUT].zEditable = !t.eq(S_readonly, 2);
               // No variable, just the axes bins.
               break;
            }
            if (strEq(S_gridHeight)) {
               ut[currentUT].gridScale = t.v(1);
               break;
            }
            if (strEq(S_gridOrient)) {
               ut[currentUT].xRot = t.v(1);
               ut[currentUT].yRot = t.v(2);
               ut[currentUT].zRot = t.v(3);
               break;
            }
            if (strEq(S_upDownLabel)) {
               ut[currentUT].upLabel = t[1];
               ut[currentUT].dnLabel = t[2];
               break;
            }
            UNEXPECTED
            break;

         //---------------------------------------------------------------------
         case blockAutoTune: //-------------------------------------------------
            if (strEq(S_table)) {
               currentUT = ud.id(t[1]) - ID_USER_TUNING_00;
               if (currentUT < 0 || currentUT >= tuningDialog::nUT) {
                  msg.send(msgInfo::mWarning, "Unknown table id '%s', ignored.", t[1]);
                  currentUT = 0;
               }
               break;
            }
            if (strEq(S_xLimits               )) { ut[currentUT].atp.xMin = t.v(1); ut[currentUT].atp.xMax = t.v(2); break; }
            if (strEq(S_yLimits               )) { ut[currentUT].atp.yMin = t.v(1); ut[currentUT].atp.yMax = t.v(2); break; }
            if (strEq(S_zLimits               )) { ut[currentUT].atp.zMin = t.v(1); ut[currentUT].atp.zMax = t.v(2); break; }
            if (strEq(S_allowAutoTune         )) { ut[currentUT].atp.setAutoTune(t.eq(S_on, 1)); break; }
            if (strEq(S_corrector             )) { ut[currentUT].atp.correctorName = t[1];   break; }
            if (strEq(S_xRadius               )) { ut[currentUT].atp.xRadius       = t.v(1); break; }
            if (strEq(S_yRadius               )) { ut[currentUT].atp.yRadius       = t.v(1); break; }
            if (strEq(S_initialStartupInterval)) { ut[currentUT].atp.t0            = t.v(1); break; }
            if (strEq(S_updateInterval        )) { ut[currentUT].atp.dT            = t.v(1); break; }
            if (strEq(S_proportionalGain      )) { ut[currentUT].atp.pGain         = t.v(1); break; }
            if (strEq(S_lumpiness             )) { ut[currentUT].atp.lumpiness     = t.v(1); break; }
            UNEXPECTED
            break;

         //---------------------------------------------------------------------
         case blockDatalog: //--------------------------------------------------
            if (strEq(S_enableWrite     )) { lop._enableVar           = t[1]; break; }
            if (strEq(S_markOnTrue      )) { lop._markerVar           = t[1]; break; }
            if (strEq(S_delimiter       )) { lop._delimiter           = (char *)byteString(t[1]).xlate().ptr(); break; }
            if (strEq(S_defaultExtension)) { lop._defaultLogExtension = t[1]; break; }
            if (strEq(S_entry)) {
               // entry = time, "Time", float, "%.3f"
               logFormat.add(datalogEntry(t[1], t[2], t[3], t[4]));
               break;
            }
            UNEXPECTED
            break;

         //---------------------------------------------------------------------
         case blockDefaults: //-------------------------------------------------
            if (strEq("engineDisplacement")) { /*Dreqfuel::cid          = t.v(1);*/ break; }
            if (strEq("injectorFlow"      )) { /*Dreqfuel::injectorFlow = t.v(1);*/ break; }
            UNEXPECTED
            break;
      }
   }

#ifdef _DEBUG
   if (_heapchk() != _HEAPOK) {
      msg.send(msgInfo::mError | msgInfo::mExit, "MT Heap Error"__FILE__" %d", __LINE__);
   }
#endif

   }
   catch (symErr e) {
      msg.send(msgInfo::mError,
         "Error processing constant or variable.\n\n"
         "%s in '%s'", e.msg, e.txt);
   }
   catch (CString s)
   {
      msg.send(msgInfo::mError,
         "Error encountered reading startup file.\n\n"
         "%s\t\n\n"
         "File processing has terminated, expect lots of errors.\t", s);
   }
   catch (const char *s) {
      msg.send(msgInfo::mError,
         "Stray error, tell Eric.\n\n"
         "%s", s);
   }
}

//------------------------------------------------------------------------------

void repository::readInit()
{
#define ochInit(n,v) \
   { symbol *s = new symbol(); \
   s->setExpr(n, "---", "", "auto-generated", 0); \
   mdb.cDesc.addSymbol(s); }

   // Get rid of all this and add it to init file.
   ochInit(S_UNDEFINED,      Uundefined);      // Put a guard in place to make sure this isn't changed.
   ochInit(S_veTuneLodIdx,   UveTuneLodIdx);
   ochInit(S_veTuneRpmIdx,   UveTuneRpmIdx);
   ochInit(S_veTuneValue,    UveTuneValue);
#undef ochInit

   const char *defaultFilename = "megatune.ini";

   FILE *iniFile = cfgOpen(defaultFilename, "r");
   if (!iniFile) {
      globalMsg.send(msgInfo::mError | msgInfo::mExit,
         "The file '%s' could not be opened.\n\n"
         "MegaTune is terminating.",
         defaultFilename);
   }
   else {
      doRead(iniFile, defaultFilename, 0);
      cfgClose(iniFile);
   }

   if (ifStack.size() > 0) {
      globalMsg.fileName = ifStack.top().fileName;
      globalMsg.lineNo   = ifStack.top().lineNo;
      globalMsg.send(msgInfo::mWarning, "Unterminated '#if'.\n\nThings are going to be goofed up.");
      globalMsg.fileName = "";
      globalMsg.lineNo   = 0;
   }

#ifdef _DEBUG
   if (_heapchk() != _HEAPOK) {
      globalMsg.send(msgInfo::mError | msgInfo::mExit,  "MT Heap Error"__FILE__" %d", __LINE__);
   }
#endif

   mdb.cDesc.init(); // Generates indexes, so must be done before the resolution passes.

   ud.init();
   resolveVarReferences  ();
   resolveGaugeReferences();
   resolveMenuReferences();

   logFormat.resolve();
   lop.resolve();

#ifdef _DEBUG
   if (_heapchk() != _HEAPOK) {
      globalMsg.send(msgInfo::mError | msgInfo::mExit,  "MT Heap Error"__FILE__" %d", __LINE__);
   }
#endif

#if 0 // _DEBUG
   FILE *xxx=fopen("ref.dmp", "w");
   for (pageMapIter iP = _gauge.begin(); iP != _gauge.end(); iP++) {
      fprintf(xxx, "%s\n", (*iP).first);
      for (gaugeMapIter iG = (*iP).second.begin(); iG != (*iP).second.end(); iG++) {
         fprintf(xxx, "   %s  (%s)\n", (*iG).first, (*iG).second->ref);
         gaugeConfiguration *g = (*iG).second;
         if (*g->ref) { // It references some config, so update it.
            gaugeMapIter iC = _gauge[S_GaugeConfigurations].find(g->ref);
//          fprintf(xxx, "      iC(%s)\n", (*iC).first);
            if (iC == _gauge[S_GaugeConfigurations].end())
               fprintf(xxx, "      no such ref\n");
         }
      }
   }
   fclose(xxx);
#endif
}

//------------------------------------------------------------------------------

void repository::writeInit(const char *fName)
{
#define NL "\n"
   FILE *iniFile = cfgOpen(fName, "w");
   if (iniFile) {
      fprintf(iniFile, S__MegaTune_ NL);

      fprintf(iniFile, S__Units_ NL);
      fprintf(iniFile, "   " S_temperature " = \"%c\"\n", mdb.therm == msDatabase::Celsius ? 'C' : 'F');
      fprintf(iniFile, "   " S_TPS "         = \"%s\"\n", mdb.rawTPS ? S_raw : S_pct);

      fprintf(iniFile, S__GaugeColors_ NL);

      fprintf(iniFile, S__GaugeConfigurations_ NL);
      gaugeMapIter iG;
      for (iG = _gauge[S_GaugeConfigurations].begin(); iG != _gauge[S_GaugeConfigurations].end(); iG++) {
         (*iG).second->print(iniFile);
      }

      fprintf(iniFile, S__FrontPage_ NL);
      for (iG = _gauge[S_FrontPage].begin(); iG != _gauge[S_FrontPage].end(); iG++) {
         (*iG).second->print(iniFile);
      }

      fprintf(iniFile, S__Tuning_ NL);
      for (iG = _gauge[S_Tuning].begin(); iG != _gauge[S_Tuning].end(); iG++) {
         (*iG).second->print(iniFile);
      }

      fprintf(iniFile, S__RunTime_ NL);

      // Spit the output channels out in numerical order, different than other
      // blocks which are all in lexical order...
      fprintf(iniFile, S__OutputChannels_ NL);
//    std::multimap<int, const char*> ochByNum;
//    for (intMapIter iO = _ochIdx.begin(); iO != _ochIdx.end(); iO++) {
//       ochByNum.insert(std::pair<int, const char*>((*iO).second, (*iO).first));
//    }
//    for (std::multimap<int, const char*>::iterator iR = ochByNum.begin(); iR != ochByNum.end(); iR++) {
//       fprintf(iniFile, "   %-20s = %3d\n", (*iR).second, (*iR).first);
//    }

      fprintf(iniFile, S__UserDefined_ NL);
      cfgClose(iniFile);
   }
}

//------------------------------------------------------------------------------
