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

#ifndef MSDATABASE_H
#define MSDATABASE_H 1

#if _MSC_VER > 1000
#  pragma once
#endif // _MSC_VER > 1000

#include <assert.h>

//------------------------------------------------------------------------------

#include "msComm.h"
#include "datalog.h"
#include "controller.h"

double timeNow();

//------------------------------------------------------------------------------

extern int   
   Uundefined,
   UegoVoltage,
   UveTuneLodIdx,   // Zero-based index of load bin tuning point.
   UveTuneRpmIdx,   // Index of rpm bin.
   UveTuneValue     // Value contained in VE[veTuneLodIdx, veTuneRpmIdx].
;

//------------------------------------------------------------------------------

void dPrintV(FILE *, const char *, va_list);
void dPrintN(FILE *, const char *, ...);
void dPrintL(FILE *, const char *, ...);

struct msqInfo {
   char  *path;
   char  *sig;
   int    major;
   int    minor;
   msqInfo(const char *p, const char *s, int mj, int mn)
   : path (_strdup(p))
   , sig  (_strdup(s))
   , major(mj)
   , minor(mn)
   {
   }
  ~msqInfo()
   {
      free(path);
      free(sig);
   }
};

class msDatabase {
private:
   msComm   io;
   datalog *log;

public:
   controllerDescriptor cDesc;
   symbol *lookupByPage(const char *symName, int pageNo);

   double getByName (const char *symName, int pageNo);
   bool   setByName (const char *symName, int pageNo, double v);
   bool   sendByName(const char *symName, int pageNo);

   static BYTE *pBytes;

private:
   int     _pageNo;
   void    getVersion();

   bool    charIn(char c, char *set);

   bool _loaded;  // Memory image is valid from either file or MS.
   bool _burned;  // MS RAM and flash are different.

public:
   void dAll   (FILE *, bool=false);

public:
   msDatabase();
  ~msDatabase();

   bool init();

   static bool number(char *s, BYTE &n);
   static bool number(char *s, WORD &n);
   static bool number(char *s, LONG &n);

   void readTable  (const char *fileName, WORD **values, int  &nValues);
   void read2DTable(const char *fileName, WORD **values, int  &nValues);
   bool readTable  (const char *fileName, WORD  *values, bool   msg);

   char                  signature[128];
   char                  title    [128];
   static CString        settingsFile;
   void                  readSettings (bool fullLoad=true);
   void                  writeSettings();
   static std::vector<msqInfo*> infoSettings();

private:
   bool                  bursting;
   int                   commPortNumber;
   int                   commPortRate;
public:
   int                   burstCommPort;
   int                   burstCommRate;
   int                   userTimerInt;
   int                   timerInterval;

   double                wwuX[10];
   double                egoLEDvalue();

   bool                  controllerReset; // Reset detected.
   int                   controllerResetCount;

   double tempFromDb(double t);
   double tempScl() { return therm == Celsius ? 5.0/9.0 : 1.0; }
   double tempOfs() { return therm == Celsius ? -72.0 : -40.0; }
   void   fixThermoLabel(CStatic &lbl, char *fmt=NULL, double value=0.0);
   char *thermLabel();

   typedef enum { Celsius, Fahrenheit } thermType;
   static thermType therm;

   static bool      rawTPS;

   // Configuration storage/retrieval.
   bool    readConfig (); // PCC config files.
   void    writeConfig(int port=-1, int rate=-1);
   CRect   mainWin;
   CRect   tuneWin;

   void save();    // Controller configuration file.
   void saveAs();
   void open();
   void veExport(int tableNo=-1);
   void veImport(int tableNo=-1);

   bool getRuntime();
   void putConst(int pageNo=-1); // -1 sends and burns whole table.
   void putConstByteP(int pageNo, int offset);
   bool getConst(int pageNo=-1); // -1 means "current page."
   bool getPage (int pageNo=-1); // -1 does getConst(allpages)
   bool load();                  // Get all data from controller.
   void burnConst(int thisPage=-1);
   bool getMemory(BYTE pageNumber, BYTE buffer[256]); // pageNumber is MSB of full 16-bit address.

   bool toggleCommLogging() { return io.toggleCommLogging(); }
   int  port() { return io.port(); }
   int  rate() { return io.rate(); }
   bool commOpened() { return io.isOpen(); }
   void logEvent(CString s) { io.logEvent(s); }

   int  setPageNo(int pageNo, bool activate=false);

   void dump();

   void setRecording(bool state, bool burstMode=false);
   CString m_logFileName;
   bool stillBursting() { return bursting; }
   bool recording();
   void markLog(const char *msg="manual") {
      if (log) log->mark(msg);
   }

   bool changed();
   bool loaded ();
   bool burned ();
};

//------------------------------------------------------------------------------
#endif
