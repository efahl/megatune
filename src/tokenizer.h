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

#ifndef TOKENIZER_H
#define TOKENIZER_H 1

//------------------------------------------------------------------------------

#include <vector>

class tokenizer {
public:
   typedef enum { Tdir, Tid, Tnum, Tstr, Tblk, Texp, Tcol, Tgarbage } type;

private:
   struct T {
      char  *str;
      type   typ;
      double num;
      T(char *s, type t, double d=0.0) : str(_strdup(s)), typ(t), num(d) { }
      void clear() { free(str); }
   };

   std::vector<T> token;
   void clear() {
     for (size_t i = 0; i < token.size(); i++) token[i].clear();
     token.clear();
   }

public:
   tokenizer() : token(std::vector<T>()) { } 
  ~tokenizer() { clear(); }

   char *operator []   (int idx) { return idx<size() ? token[idx].str : "";       }
   double v            (int idx) { return idx<size() ? token[idx].num : 0.0;      }
   int    i            (int idx) { return idx<size() ? int(token[idx].num) : 0;   }
   type   t            (int idx) { return idx<size() ? token[idx].typ : Tgarbage; }
   const char *stripped(int idx);
   int   size          ()        { return token.size();   }

   bool  eq            (const char *s, int idx) { return strcmp (operator[](idx), s) == 0; }
   bool  match         (const char *s, int idx) { return strncmp(operator[](idx), s, strlen(s)) == 0; }
// bool  eq            (const char *s, int idx) { return strcmp (token[idx].str, s) == 0; }
// bool  match         (const char *s, int idx) { return strncmp(token[idx].str, s, strlen(s)) == 0; }

   int  parse(const char *line);
   void dump ();
};

//------------------------------------------------------------------------------
#endif
