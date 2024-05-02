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

#pragma warning(disable:4786)

#include "gaugeConfig.h"
#include "msDatabase.h"

//------------------------------------------------------------------------------

void gaugeConfiguration::doCopy(const gaugeConfiguration &from)
{
   // Move everything except ref!
 //strcpy(label,   from.label);
   strcpy(cName,    from.cName);
   strcpy(title,    from.title);
   strcpy(units,    from.units);
   strcpy(faceFile, from.faceFile);
   och       = from.och;
   lo        = from.lo;
   hi        = from.hi;
   loD       = from.loD;
   loW       = from.loW;
   hiW       = from.hiW;
   hiD       = from.hiD;
   vDecimals = from.vDecimals;
   rDecimals = from.rDecimals;
}

//------------------------------------------------------------------------------

gaugeConfiguration::gaugeConfiguration(
   const char *rf,
   const char *l,
   const char *t,
   const char *u,
   double      lv,  double      hv,
   double      lvD, double      lvW,
   double      hvW, double      hvD,
   int         vd,
   int         rd,
   char       *rtN)
 : och      (Uundefined),
   lo       (lv),
   hi       (hv),
   loD      (lvD),
   loW      (lvW),
   hiW      (hvW),
   hiD      (hvD),
   vDecimals(vd),
   rDecimals(rd)
{
   strcpy(cName, rtN);
   strcpy(ref,   rf);
   strcpy(label, l);
   strcpy(title, t);
   strcpy(units, u);
   strcpy(faceFile, "");
}

//------------------------------------------------------------------------------

gaugeConfiguration::gaugeConfiguration(const gaugeConfiguration &from)
{
   strcpy(ref, from.ref);
   doCopy(from);
}

//------------------------------------------------------------------------------

gaugeConfiguration &gaugeConfiguration::operator= (const gaugeConfiguration &from)
{
   if (this == &from) return *this;
   doCopy(from);
   return *this;
}

//------------------------------------------------------------------------------

void gaugeConfiguration::set(double loL, double hiL, double redline)
{
   lo  = loL;
   hi  = hiL;
   loD = lo - 1.0;
   loW = lo - 1.0;
   hiW = hi + 1.0;
   hiD = redline;
}

//------------------------------------------------------------------------------

void gaugeConfiguration::set(tokenizer &t)
{
   if (t.size() == 2)
      // Reference: gauge1 = rpmGauge
      // Note that "rpmGauge" may or may not exist at this time.
      strcpy(ref, t[1]);
   else if (t.size() == 4)
      // Old style : rpmGauge = 0, 8000, 6700
      set(t.v(1), t.v(2), t.v(3));
   else if (t.size() == 7) {
      // 2.16 style: rpmGauge = 0, 8000, 300, 500, 6500, 6700
      lo        = t.v(1);
      hi        = t.v(2);
      loD       = t.v(3);
      loW       = t.v(4);
      hiW       = t.v(5);
      hiD       = t.v(6);
   }
   else {
      // This is a true gauge configuration, not a displayed gauge.
      // accelEnrichGauge=accelEnrich,"Acceleration Enrichment","%",100,150,-1,-1,999,999,0,0
      strcpy(ref,   ""   );
      strcpy(label,     t  [ 0]);
      strcpy(cName,     t  [ 1]);
      // och - Resolved later by owner of och list.
      strcpy(title,     t  [ 2]);
      strcpy(units,     t  [ 3]);
      lo          =     t.v( 4);
      hi          =     t.v( 5);
      loD         =     t.v( 6);
      loW         =     t.v( 7);
      hiW         =     t.v( 8);
      hiD         =     t.v( 9);
      vDecimals   = int(t.v(10));
      rDecimals   = int(t.v(11));
      strcpy(faceFile,  t  [12]);
   }
}

//------------------------------------------------------------------------------

void gaugeConfiguration::print(FILE *iniFile)
{
   if (*ref)
      fprintf(iniFile, "   %-18s = %s\n", label, ref);
   else
      fprintf(iniFile,
         "   %-18s = %-15s, \"%s\"%*s, \"%s\"%*s, %5.1f, %7.1f, %5.1f, %5.1f, %7.1f, %7.1f, %1d, %1d\n", 
         label, cName,
         title, 25-strlen(title), "",
         units, 5-strlen(units), "",
         lo, hi, loD, loW, hiW, hiD, vDecimals, rDecimals);
}

//------------------------------------------------------------------------------
