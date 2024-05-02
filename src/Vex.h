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

#if !defined(VEX_H)
#define VEX_H 1

#if _MSC_VER > 1000
#  pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------

#include <stdio.h> // Primitive but effective.

//------------------------------------------------------------------------------

class veTable {
   double  *loadBins;
   double  *rpmBins;
   double  *veBins;
   int     nLoad;
   int     nRPM;
   char     loadtype[10];
   double  _scale;
   double  _trans;

   void cleanUp();

   double *interpRow(double *Y, int oldN, int newN, double roundTo);

public:
   veTable(int nRPM=0, int nLoad=0, double rpmBins[]=NULL, double loadBins[]=NULL, double veBins[]=NULL);
  ~veTable();


   bool   setSize(int nRPM, int nLoad, double rpmBins[]=NULL, double loadBins[]=NULL, double veBins[]=NULL);
   void   setScale(double s) { _scale = s; }
   double scale() { return _scale; }
   void   setTranslate(double t) { _trans = t; }
   double translate() { return _trans; }

   void   loadType(char *type);
   char  *loadType();

   double load(int index);
   void   load(double value, int index);
   double rpm (int index);
   void   rpm (double value, int index);
   double ve  (int r, int c);
   void   ve  (double value, int r, int c);

   int nLoads() { return nLoad; }
   int nRPMs () { return nRPM;  }

   double interpolate(double rpm, double load); // Returns the interpolated value.

   void resize(int newNRPMs, int newNLODs, double roundRPM, double roundLOD, double roundVE);
};

//------------------------------------------------------------------------------

class vex {
public:
   enum errorStatus {
      noError,
      openError,
      readError,
      writeError,
      closeError,
      nonVexError,         // EVEME header wrong
      missingBracketError,
      missingLeftParenError,
      missingRightParenError,
      tableSizeError,
      rowIndexError,
      missingEqError,
   };
   static const char *errorMsg(errorStatus);

private:
   char lineBuffer[512];
   FILE *vexFile;

   double UserRev;
   char   UserComment[256];
   char   Date[20];
   char   Time[20];

   bool        getLine  ();
   errorStatus readTable(double *table, int rows, int cols);
   errorStatus write    (veTable &p);
   errorStatus closeFile(errorStatus);

   veTable **pages;
   int      nPages;

public:
   vex();
  ~vex();

   veTable    *page   (int pageNo);
   veTable    *newPage(int pageNo);
   int         pageCount() { return nPages; }

   errorStatus read (const char *fileName);
   errorStatus write(const char *fileName);

   double      userRev()                   { return UserRev; }
   void        userRev(double ur)          { UserRev = ur;   }
   char       *userComment()               { return UserComment; }
   void        userComment(const char *uc) { if (strlen(uc) < sizeof(UserComment)-1) strcpy(UserComment, uc); }
   char       *date()                      { return Date; }
   void        date(const char *d)         { if (strlen(d) < sizeof(Date)-1) strcpy(Date, d); }
   char       *time()                      { return Time; }
   void        time(const char *t)         { if (strlen(t) < sizeof(Time)-1) strcpy(Time, t); }
};

//------------------------------------------------------------------------------
#endif // !defined(VEX_H)
