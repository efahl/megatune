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

#ifndef GAUGECONFIG_H
#define GAUGECONFIG_H 1

//------------------------------------------------------------------------------

#include "tokenizer.h"

//------------------------------------------------------------------------------

class gaugeConfiguration {
public:
   char   label[128];  // My name.
   char   ref  [128];  // The gauge from which I was (or will be) derived, if any.
   char   title[128];
   char   units[128];
   char   cName[128];
   char   faceFile[128];

   int    och;         // Output channel index (I liken it to a machine address).
   double lo,  hi;
   double loD, hiD;
   double loW, hiW;
   int    vDecimals;
   int    rDecimals;

private:
   void doCopy(const gaugeConfiguration &from);

public:
   gaugeConfiguration(
    const char *rf  = "",
    const char *l   = "",
    const char *t   = "",
    const char *u   = "",
    double      lv  =  0.0,
    double      hv  =  1.0,
    double      lvD = -0.1,
    double      lvW = -0.1,
    double      hvW =  1.1,
    double      hvD =  1.1,
    int         vd  =  0,
    int         rd  =  0,
    char       *rtN = "");

   gaugeConfiguration(const gaugeConfiguration &from);
   gaugeConfiguration &operator= (const gaugeConfiguration &from);

   void set(double loL, double hiL, double redline);
   void set(tokenizer &t);
   void print(FILE*);
};

//------------------------------------------------------------------------------
#endif
