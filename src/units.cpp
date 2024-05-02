//------------------------------------------------------------------------------
//--  Copyright (c) 2006 by Eric Fahlgren                                     --
//--  All Rights Reserved.                                                    --
//--                                                                          --
//--  This program is free software; you can redistribute it and/or           --
//--  modify it under the terms of the GNU General Public License             --
//--  as published by the Free Software Foundation; either version            --
//--  2 of the License, or (at your option) any later version.                --
//--  See http://www.gnu.org/licenses/gpl.txt                                 --
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "units.h"
#include "miniDOM.h"

//------------------------------------------------------------------------------

class conversion {
   public:
      conversion(CString n, double s, double t) : _name(n), _scale(s), _trans(t) { }

      double int2usr(double intValue) { return (intValue + _trans) * _scale; }
      double usr2int(double usrValue) { return (usrValue / _scale) - _trans; }

   private:
      CString _name;
      double  _scale;
      double  _trans;
};

//------------------------------------------------------------------------------

FILE *cfgOpen(const char *fileName, const char *mode);
void  cfgLog (const char *Format, ...);

units::units()
{
   xml::DOM d;
   d.read("units.xml", cfgOpen("units.xml", "r"));
   xml::domElementList &elements = d.topElement()->children();

   for (xml::domElementIterator i = elements.begin(); i != elements.end(); i++) {
      xml::domElement *e = *i;

      if (e->tag() == "Unit") {
         CString name = e->attribute("name");
         double scale = strtod(e->attribute("scale",     "1"), NULL);
         double trans = strtod(e->attribute("translate", "0"), NULL);
         _cvtbl[name] = new conversion(name, scale, trans);
      }
   }
}

//------------------------------------------------------------------------------

units::~units()
{
   for (std::map<CString, conversion*>::iterator i = _cvtbl.begin(); i != _cvtbl.end(); i++) {
      delete (*i).second;
   }
}

//------------------------------------------------------------------------------

bool units::exists (CString name)
{
   return _cvtbl.find(name) != _cvtbl.end();
}

//------------------------------------------------------------------------------

double units::convert(double x, CString from, CString to)
{
   if (from != to && exists(from) && exists(to)) {
      double oldX = x;
      x = _cvtbl[to]->int2usr(_cvtbl[from]->usr2int(x));
      cfgLog("   INFO: Units conversion %.1f %s to %.1f %s", oldX, from, x, to);
   }
   return x;
}

//------------------------------------------------------------------------------
