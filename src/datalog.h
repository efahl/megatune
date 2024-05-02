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

#ifndef DATALOG_H
#define DATALOG_H 1

//------------------------------------------------------------------------------

#pragma warning(disable:4786)

#include <vector>

//------------------------------------------------------------------------------

const char *timeStamp(bool longForm=true);

//------------------------------------------------------------------------------

class datalogEntry {
public:
   enum entryType { eFloat, eInt };

private:
   CString      _ochName;   // time
   int          _och;       // 0
   CString      _label;     // "Time"
   entryType    _type;      // Above
   CString      _format;    // "%.3f"

public:
   datalogEntry(const char *nm, const char *lb, const char *tp, const char *fm);
  ~datalogEntry() { }
   
   const char *name  () { return _ochName; }
   const char *label () { return _label;   }
   int         ochIdx() { return _och;     }
   const char *format() { return _format;  }
   entryType   type  () { return _type;    }
   void resolve();
};

//------------------------------------------------------------------------------

class datalogList {
   std::vector<datalogEntry> ll;

public:
   datalogList() { }
//~dataloglist() { }

   void add(datalogEntry &de) { ll.push_back(de); }
   void resolve();

   void header(FILE *of);
   void write (FILE *of);

   int  columns() { return ll.size(); }
};

//------------------------------------------------------------------------------

struct datalogOptions {
   char        _commentText[1024];
   bool        _dumpBefore;
   bool        _dumpAfter;

   int         _enableWrite;
   CString     _enableVar;
   int         _markOnTrue;
   CString     _markerVar;

   CString     _delimiter; // "\t"
   CString     _defaultLogExtension;

   datalogOptions();
   
   void resolve();
};

//------------------------------------------------------------------------------

class datalog {
   bool        _recording;
   FILE       *_recordFile;
   int         _markerNumber;
   bool        _lastMarkState;

public:
   datalog();
  ~datalog();

   bool open (const char *fileName);
   void close();
   void write();
   void mark (const char *label);
};

//------------------------------------------------------------------------------
#endif
