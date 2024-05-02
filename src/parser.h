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

#ifndef PARSER_H
#define PARSER_H 1
//------------------------------------------------------------------------------

class codeGen;

class expression {
   public:
      expression();
     ~expression();

      void setOutputBuffer(double *och);
      void addExpr(int destVar, const char *expr, const char *file, int lineNo);
      void recalc ();

   private:
      double  *_och;
      codeGen *_code;
};

//------------------------------------------------------------------------------

class exprError {
   const char *file;
   int         lineNumber;
   char        tok[128];
   char        str[128];
   char        title[128];
   bool        found;

public:
   exprError() : file(NULL), lineNumber(0), found(false) { tok[0] = title[0] = str[0] = '\0'; }

   void setFile (const char *f);
   void setTitle(const char *t);
   void setLine (int   l);

   void reset(char  c);
   void reset(char *s);

   const char *errorString();

   char append(char c);
   int  backup();
   void display();
   void Error(const char *Msg);
};

//------------------------------------------------------------------------------
#endif
