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

#include "stdafx.h"
#include <direct.h>
#include <time.h>

#include "megatune.h"
#include "repository.h"
#include "msDatabase.h"
#include "datalog.h"


extern repository     rep;
extern msDatabase     mdb;
extern datalogOptions lop;

//------------------------------------------------------------------------------

const char *timeStamp(bool longForm)
{
   const time_t timer = time(NULL);
   static char  formattedTime[26];

   struct tm *tm_ptr = gmtime(&timer);
   int        gmt_dy = tm_ptr->tm_yday;
   int        gmt_hr = tm_ptr->tm_hour;
   int        gmt_mn = tm_ptr->tm_min;

   tm_ptr = localtime(&timer);
   if (gmt_dy > tm_ptr->tm_yday) gmt_hr += 24;

   if (longForm) {
      int n = strftime(formattedTime, sizeof(formattedTime)-1, "%Y-%m-%dT%H:%M:%S", tm_ptr);
      sprintf(formattedTime+n, "%+03d:%02d", tm_ptr->tm_hour-gmt_hr, tm_ptr->tm_min-gmt_mn);
   }
   else {
      strftime(formattedTime, sizeof(formattedTime)-1, "%Y%m%d%H%M", tm_ptr);
   }

   return formattedTime;
}

//------------------------------------------------------------------------------

datalogEntry::datalogEntry(const char *nm, const char *lb, const char *tp, const char *fm)
 : _ochName (nm),
   _och     ( 0),
   _label   (lb),
   _format  (fm)
{
   if (strcmp(tp, "float") == 0)
      _type = eFloat;
   else if (strcmp(tp, "int") == 0)
      _type = eInt;
   else {
      msgOk("Custom Datalog", "Datatype '%s' should be either 'float' or 'int'", tp);
      _type = eInt;
   }
}

//------------------------------------------------------------------------------

void datalogEntry::resolve()
{
   _och = mdb.cDesc.varIndex(name());
   if (_och == -1) {
      msgOk("Custom Datalog", "Output channel '%s' cannot be found for datalog entry labeled '%s'", name(), label());
      _och = Uundefined;
   }
}

//------------------------------------------------------------------------------

void datalogList::resolve()
{
   for (int i = 0; i < columns(); i++) {
      ll[i].resolve();
   }
}

void datalogList::header(FILE *of)
{
   for (int i = 0; i < columns(); i++) {
      if (i > 0) fprintf(of, "%s", lop._delimiter); // fputc('\t', of);
      fprintf(of, "%s", ll[i].label());
   }
   fputc('\n', of);
}

void datalogList::write (FILE *of)
{
   for (int i = 0; i < columns(); i++) {
      if (i > 0) fprintf(of, "%s", lop._delimiter); // fputc('\t', of);
      switch (ll[i].type()) {
         case datalogEntry::eInt:
            fprintf(of, ll[i].format(), int(mdb.cDesc._userVar[ll[i].ochIdx()]));
            break;
         case datalogEntry::eFloat:
            fprintf(of, ll[i].format(),     mdb.cDesc._userVar[ll[i].ochIdx()]);
            break;
      }
   }
   fputc('\n', of);
}

//------------------------------------------------------------------------------

void datalogOptions::resolve()
{
   if (!_markerVar.IsEmpty()) {
      _markOnTrue  = mdb.cDesc.varIndex(_markerVar);
      if (_markOnTrue == -1) {
         msgOk("Custom Datalog", "Could not find \"markOnTrue\" variable named \"%s\"", _markerVar);
      }
   }
   if (!_enableVar.IsEmpty()) {
      _enableWrite = mdb.cDesc.varIndex(_enableVar);
      if (_enableWrite == -1) {
         msgOk("Custom Datalog", "Could not find \"enableWrite\" variable named \"%s\"", _enableVar);
      }
   }
}

//------------------------------------------------------------------------------

datalogOptions::datalogOptions()
 : _dumpBefore    (false)
 , _dumpAfter     (false)
 , _enableWrite   (-1)
 , _markOnTrue    (-1)
 , _delimiter     ("\t")
 , _defaultLogExtension("xls")
{
   memset(_commentText, 0, sizeof(_commentText));
   _enableVar = "";
   _markerVar = "";
}

//------------------------------------------------------------------------------

datalog::datalog()
 : _recording     (false)
 , _recordFile    (NULL)
 , _markerNumber  (0)
 , _lastMarkState (false)
{
}

datalog::~datalog()
{
   close();
}

//------------------------------------------------------------------------------

bool datalog::open(const char *fileName)
{
   close();
   _recordFile = fopen(fileName, "w");
   if (_recordFile == NULL) return false;

   fprintf(_recordFile, "\"%s\"\n", mdb.cDesc.signature());

   if (lop._commentText && lop._commentText[0]) { // Then we have some user-defined output for the log file.
      char *next = NULL;
      for (const char *s = lop._commentText; s && *s; s = next) {
         next = (char *)strchr(s, '\r');
         if (next) { *next = '\0'; }
         fprintf(_recordFile, "\"%s\"\n", s);
         if (next) { *next = '\r'; next += 2; }
      }
   }

   if (lop._dumpBefore) mdb.dAll(_recordFile, true);
   _recording = true;
   rep.logFormat.header(_recordFile);

   return true;
}

//------------------------------------------------------------------------------

void datalog::close()
{
   if (_recording) {
      _recording = false; // Do this before closing the file in case the timer fires.
      if (lop._dumpAfter) mdb.dAll(_recordFile, true);
      fclose(_recordFile);
   }
}

//------------------------------------------------------------------------------

void datalog::write()
{
   if (_recording) {
      if (lop._enableWrite >= 0 && mdb.cDesc._userVar[lop._enableWrite] == 0) return;
      if (lop._markOnTrue  >= 0) {
         if (mdb.cDesc._userVar[lop._markOnTrue] && !_lastMarkState) {
            mark(lop._markerVar);
         }
         _lastMarkState = mdb.cDesc._userVar[lop._markOnTrue] != 0;
         // Need some hysteresis on this to keep it from dithering
         // in the log file...
      }

      rep.logFormat.write(_recordFile);
      fflush(_recordFile);
   }
}

//------------------------------------------------------------------------------

void datalog::mark(const char *label)
{
   _markerNumber++;
   fprintf(_recordFile, "\"MARK %03d %s %s\"\n", _markerNumber, label, timeStamp());
   Beep(1000,100);
}

//------------------------------------------------------------------------------
