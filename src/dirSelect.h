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

//{{AFX_INCLUDES()
//}}AFX_INCLUDES

#ifndef DIRSELECT_H
#define DIRSELECT_H

#if _MSC_VER >= 1000
#  pragma once
#endif

//------------------------------------------------------------------------------

typedef char _path[_MAX_PATH];

struct dirEntry {
   _path  name;
   _path  path;
   dirEntry(const char *n, const char *p) { strcpy(name, n); strcpy(path, p); }
};

//------------------------------------------------------------------------------

#include <vector>

class dirSelect {
   std::vector<dirEntry *> entry;

public:
   dirSelect() { }
  ~dirSelect();

   void init();

   int nDirs() { return entry.size(); }
   
   void add(const char *path, const char *fullPath="") {
      entry.push_back(new dirEntry(path, fullPath));
   }

   dirEntry &operator [](int idx) {
      if (idx <  0      ) idx = 0;
      if (idx >= nDirs()) idx = nDirs()-1;
      return *entry[idx];
   }

   CString selected;
};

//------------------------------------------------------------------------------
#endif
