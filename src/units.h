//------------------------------------------------------------------------------
//--  Copyright (c) 2006 by Eric Fahlgren, All Rights Reserved.               --
//--                                                                          --
//--  This program is free software; you can redistribute it and/or           --
//--  modify it under the terms of the GNU General Public License             --
//--  as published by the Free Software Foundation; either version            --
//--  2 of the License, or (at your option) any later version.                --
//--  See http://www.gnu.org/licenses/gpl.txt                                 --
//------------------------------------------------------------------------------

#ifndef UNITS_H
#define UNITS_H 1

#if _MSC_VER > 1000
#  pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------

#pragma warning(disable:4786)
#include <map>

class conversion;

class units {
   public:
      units();
     ~units();

      double convert(double x, CString from, CString to);
      bool   exists (CString name);

   private:
      std::map<CString, conversion*> _cvtbl;
};

//------------------------------------------------------------------------------
#endif
