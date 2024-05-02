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

#ifndef REPOSITORY_H
#define REPOSITORY_H 1

//------------------------------------------------------------------------------

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef VERSION_STRING
#define VERSION_STRING "3.00"
#endif
#define VERSION         3.00

//------------------------------------------------------------------------------

#pragma warning(disable:4018 4100 4245 4663 4786)

#include <vector>
#include <map>
#include "gaugeConfig.h"
#include "datalog.h"
#include "dataDialog.h"

//------------------------------------------------------------------------------

struct ltstr {
   bool operator()(const char* s1, const char* s2) const {
      return strcmp(s1, s2) < 0;
   }
};

typedef std::map<const char*, gaugeConfiguration*, ltstr> gaugeMap;
typedef gaugeMap::iterator                                gaugeMapIter;

typedef std::map<const char *, gaugeMap, ltstr>           pageMap;
typedef pageMap::iterator                                 pageMapIter;

#define UNDEFINED_COLOR (RGB(123,231,213))
typedef std::map<const char *, COLORREF, ltstr>           colorMap;
typedef colorMap::iterator                                colorMapIter;

//------------------------------------------------------------------------------

class repository {
   pageMap  _gauge;   // All the gauges.
   char     _pageLbl[4][10]; // Tuning page button labels.

   void setLimits  (const char *page, const char *label, tokenizer &t);
   void setGaugeRef(const char *page, const char *gaugeId, const char *ref);
   void setGaugeRef(const char *page, tokenizer &t, int maxGaugeNo);
   void setGaugeFam(const char *page, tokenizer &t, int maxGaugeNo);

   void resolveGaugeReferences();
   void resolveVarReferences  ();
   void resolveMenuReferences ();

   double   _barHysteresis;
   bool     _fieldColoring;
   colorMap _colors;

   void     setColor(const char *colorName, COLORREF color);
   COLORREF getColor(tokenizer &t);

   void aeTpsDot(tokenizer &t) { loaTPD = t.v(1); hiaTPD = t.v(2); }
   void aeMapDot(tokenizer &t) { loaMAD = t.v(1); hiaMAD = t.v(2); }

   void runEGO(tokenizer &t) { lorEGO = t.v(1); hirEGO = t.v(2); }
   void runCT (tokenizer &t) { lorCT  = t.v(1); hirCT  = t.v(2); }
   void runBAT(tokenizer &t) { lorBAT = t.v(1); hirBAT = t.v(2); }
   void runTR (tokenizer &t) { lorTR  = t.v(1); hirTR  = t.v(2); }
   void runGE (tokenizer &t) { lorGE  = t.v(1); hirGE  = t.v(2); }
   void runMAP(tokenizer &t) { lorMAP = t.v(1); hirMAP = t.v(2); }
   void runMAT(tokenizer &t) { lorMAT = t.v(1); hirMAT = t.v(2); }
   void runRPM(tokenizer &t) { lorRPM = t.v(1); hirRPM = t.v(2); }
   void runPW (tokenizer &t) { lorPW  = t.v(1); hirPW  = t.v(2); }
   void runDC (tokenizer &t) { lorDC  = t.v(1); hirDC  = t.v(2); }
   void runEGC(tokenizer &t) { lorEGC = t.v(1); hirEGC = t.v(2); }
   void runBC (tokenizer &t) { lorBC  = t.v(1); hirBC  = t.v(2); }
   void runWC (tokenizer &t) { lorWC  = t.v(1); hirWC  = t.v(2); }
   void runADC(tokenizer &t) { lorADC = t.v(1); hirADC = t.v(2); }
   void runVE (tokenizer &t) { lorVE  = t.v(1); hirVE  = t.v(2); }
   void runACC(tokenizer &t) { lorACC = t.v(1); hirACC = t.v(2); }

   void frontEGO(tokenizer &t) { lofEGO = t.v(1); hifEGO = t.v(2); rdfEGO = t.v(3); } // LED meter

   void tuneVEB(tokenizer &t) { lotVEB = t.v(1); hitVEB = t.v(2);                }
   void tuneCD (tokenizer &t) { vatCD  = int(t.v(1)); }
   void tuneSD (tokenizer &t) { vatSD  = int(t.v(1)); }
   void tuneEGO(tokenizer &t) { lotEGO = t.v(1); hitEGO = t.v(2); rdtEGO = t.v(3); } // LED meter

   void doRead(FILE *, const char *, int); // Parses and processes file, possibly recursing on #include. 
   void readInit();     // Finds megatune.ini and opens it.

   int currentUC;
   int currentUD;
   int currentUF;
   int currentUT;
   int currentCP;
   int currentOP;

public:
   repository();
   virtual ~repository();

   void init();

   void writeInit(const char *fName); // Spews out the current ini file.

   const gaugeConfiguration *gauge(const char *page, const char *name) {
      return _gauge[page][name];
   }
   std::vector<CString> repository::gaugeConfigs();

   double   barHysteresis();
   bool     fieldColoring();
   COLORREF color(const char *colorName);

   datalogList logFormat; // Move this to datalog stuff...

   char *pageLabel(int idx) {
      if (idx >= 0 && idx <= 3)
         return _pageLbl[idx];
      else
         return "";
   }

   double MTversion;         // Version of megatune.ini file.  Should match the
                             // version of the executable.

   // Controller versioning.
   double lofEGO, hifEGO, rdfEGO;

   double loaTPD, hiaTPD;
   double loaMAD, hiaMAD;

   double lorEGO, hirEGO;
   double lorCT,  hirCT; 
   double lorBAT, hirBAT; 
   double lorTR,  hirTR; 
   double lorGE,  hirGE; 
   double lorMAP, hirMAP; 
   double lorMAT, hirMAT; 
   double lorRPM, hirRPM; 
   double lorPW,  hirPW; 
   double lorDC,  hirDC; 
   double lorEGC, hirEGC; 
   double lorBC,  hirBC; 
   double lorWC,  hirWC; 
   double lorADC, hirADC; 
   double lorVE,  hirVE; 
   double lorACC, hirACC;

   double lotEGO, hitEGO, rdtEGO; // The LED bar.

   double lotVEB, hitVEB;

   int      vatSD;  // spotDepth
   int      vatCD;  // cursorDepth
   int      gaugeColumns;

   COLORREF g_alertColor;
   COLORREF g_backgroundColor;
   COLORREF g_faceColor;
   COLORREF g_bgWarningColor;
   COLORREF g_bgDangerColor;
   COLORREF g_ledColor;
   COLORREF g_ledAlertColor;
   COLORREF g_needleColor;
   COLORREF g_rangeColor;
   COLORREF g_scaleColor;
   COLORREF g_titleColor;
   COLORREF g_valueColor;

   COLORREF t_backgroundColor;
   COLORREF t_cursorColor;
   COLORREF t_cursorText;
   COLORREF t_gridColor;
   COLORREF t_spotColor;
   COLORREF t_spotText;
   COLORREF t_textColor;
   CString  t_fontFace;
   int      t_fontSize;
};

//------------------------------------------------------------------------------
#endif
