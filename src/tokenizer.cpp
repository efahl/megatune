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
#include "tokenizer.h"

int tokenizer::parse(const char *line)
{
   clear();

   char tok[1024];
   const char *s = line;
   int i;
   while (*s) {
      while (isspace(*s)) s++;
      if (!*s || *s == ';') break;

      if (*s == ',' || *s == '=') { // Syntactic  sugar, ignore it.
         s++;
         continue;
      }

      if (*s == '#') { // The following string is a directive.
         token.push_back(T("#", Tdir));
         s++;
         continue;
      }

      if (*s == ':') { // Syntax
         token.push_back(T(":", Tcol));
         s++;
         continue;
      }

      if (isalpha(*s)) { // Parse alpha-numeric id.
         for (i = 0; isalnum(*s) || *s == '_'; i++, s++) tok[i] = *s;
         tok[i] = 0;
         token.push_back(T(tok, Tid));
         continue;
      }

      if (isdigit(*s) || *s == '-') { // Floats with no exponent.
         tok[0] = *(s++);
         for (i = 1; isdigit(*s) || *s == '.'; i++, s++) tok[i] = *s;
         tok[i] = 0;
         token.push_back(T(tok, Tnum, atof(tok)));
         continue;
      }

      if (*s == '"') { // Quoted string, quotes removed.
         i = 0;
         s++; // tok[i++] = *(s++); // or quotes left in...
         for ( ; *s && *s != '"'; i++, s++) tok[i] = *s;
         if (*s != '"') throw CString("Closing quote not found.");
         if (*s) s++; // tok[i++] = *(s++);
         tok[i] = 0;
         token.push_back(T(tok, Tstr));
         continue;
      }

      if (*s == '[') { // Block identifier
         for (i = 0; *s && *s != ']'; i++, s++) {
            tok[i] = *s;
         }
         if (*s) tok[i++] = *(s++);
         tok[i] = 0;
         token.push_back(T(tok, Tblk));
         continue;
      }

      if (*s == '{') { // Expression
         for (i = 0; *s && *s != '}'; i++, s++) {
            tok[i] = *s;
         }
         if (*s) tok[i++] = *(s++);
         tok[i] = 0;
         token.push_back(T(tok, Texp));
         continue;
      }

   // printf("Ignoring %c\n", *s);
      s++;
   }
   return token.size();
}

//------------------------------------------------------------------------------

const char *tokenizer::stripped(int idx)
{
   char *str = idx<size() ? token[idx].str : "";
   if (*str) {
      memmove(str, str+1, strlen(str)+1); // Eat the {
      if (strchr(str, '}'))
      *strrchr(str, '}') = '\0';          // Then the }
   }
   return str;
}

//------------------------------------------------------------------------------

void cfgLog(const char *Format, ...);
void tokenizer::dump()
{
   char *typeName[] = { "Tdir", "Tid ", "Tnum", "Tstr", "Tblk", "Texp", "Tcol" };
   cfgLog("-- %3d ---------------------------", token.size());
   for (size_t i = 0; i < token.size(); i++) {
      cfgLog("  %03d = %s >%s<", i, typeName[token[i].typ], token[i].str);
   }
}

//------------------------------------------------------------------------------
