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

#ifndef CONTROLLER_H
#define CONTROLLER_H 1

#if _MSC_VER > 1000
#  pragma once
#endif // _MSC_VER > 1000

#include <assert.h>

//------------------------------------------------------------------------------

#include "byteString.h"
#include "symbol.h"
#include "msComm.h"

#include <map>

//------------------------------------------------------------------------------

struct pageParameters {
   int            _pageNo;
   int            _size;
   int            _offset;
   byteString     _pageIdentifier;
   bool           _bigEnd;
   bool           _modified;
};

class commandFormat {
   bool _empty;

   int _dSiz; // Number of bytes in raw data part.

   int _pOfs; // Location of bytes in command.
   int _pSiz; // Size of data for this part of command.
   int _iOfs; // Page ID, might be different than page number.
   int _iSiz;
   int _oOfs; // Offset part
   int _oSiz;
   int _cOfs; // Byte count, used in "chunk" write commands.
   int _cSiz;
   int _vOfs; // Address part
   int _vSiz; // Size of one value in bytes.
   int _vCnt; // Number of values (not bytes).

   byteString _raw;
   byteString _blt;

   // What about computed checksums?
   // What about suffix data in variable length writes?

public:

   commandFormat()
    : _empty(true)
    , _pOfs(-1), _pSiz( 0)
    , _iOfs(-1), _iSiz( 0)
    , _cOfs(-1), _cSiz( 0)
    , _oOfs(-1), _oSiz( 0)
    , _vOfs(-1), _vSiz( 0)
   {
   }

   void        parse   (const char *rawCmd);
   byteString &buildCmd(pageParameters &pp, DWORD ofs, const BYTE *val, int cnt=1);

   bool empty    (bool chkDash=true) { return _empty || (chkDash && _raw == "-"); }
   int  valueSize()                  { return _vSiz;  }
};

//------------------------------------------------------------------------------

struct _pageInfo {
   pageParameters _pp;

   int siz(int s=0) { if (s) _pp._size   = s; return _pp._size;   }
   int ofs(int o=0) { if (o) _pp._offset = o; return _pp._offset; }

   bool modified()       { return _pp._modified; }
   void modified(bool m) { _pp._modified = m;    }
};

struct pageInfo : _pageInfo {
   commandFormat  _activate;
   commandFormat  _burnCommand;
   commandFormat  _readWhole;
   commandFormat  _readChunk;
   commandFormat  _readValue;
   commandFormat  _writeWhole;
   commandFormat  _writeChunk;
   commandFormat  _writeValue;

   void fillFrom(pageInfo &rhs);
};

struct oPageInfo : _pageInfo {
   commandFormat  _ochGetCommand;
   commandFormat  _ochBurstCommand;

   void fillFrom(_pageInfo &rhs);

   void increment(bool ok) { _polls++; if (!ok) _failures++; }
   void reset    ()        { _polls = _failures = 0; }
   int  polls    ()        { return _polls;    }
   int  failures ()        { return _failures; }
   int _polls;
   int _failures;
};

//------------------------------------------------------------------------------

class pollSequence {
   public:
      pollSequence() { }
     ~pollSequence() { }

      void      add (int i);
      unsigned  size()               { return _seq.size(); }
      int       operator [](int idx) { return idx < size() ? _seq[idx] : 0; }

   private:
      std::vector<int> _seq;
};

//------------------------------------------------------------------------------

typedef std::map<const CString, symbol *> symMap;
typedef symMap::iterator                  symMapIter;

class expression;

class controllerDescriptor {
   expression *_exprs;
   static int lastPage;

   msComm  &_io;

public:
   enum { maxPages = 50 };
private:

   bool       _changed;       // MT and MS memory are different.
   bool       _verifying;     // We should read and compare, if possible.

   bool       _bigEnd;
   byteString _versionInfo;    // Command to get title string.
   byteString _queryCommand;   // Command to return signature.
   byteString _signature;
   char      *_sigFile;

   int        _nPages;
   pageInfo   _page[maxPages];
   symbol    *_wholePage[maxPages]; // Internal symbols pointing to whole page.
   int        _interWriteDelay;     // Milliseconds between writes.
   bool       _writeBlocks;         // Use a single write or multi?
   int        _pageActivationDelay; // Milliseconds after page activate command or burn command is sent.
   int        _blockReadTimeout;    // Total comm block timeout for a single read.

   int        _nOPages;
   oPageInfo  _oPage[maxPages];
   BYTE      *_ochBuffer;
   std::vector<pollSequence> _ochPoll;

   int        _userVarSize;
// double    *_userVar;      // All runtime vars and calculated ones.

   int mxi(int pageNo, int db) {
      if (db == 0) { return pageNo < _nPages  ? pageNo : _nPages -1; }
      else         { return pageNo < _nOPages ? pageNo : _nOPages-1; }
   }

   symMap symbolTable;

   const byteString &pageActivate   (int pageNo);
   const byteString &pageReadWhole  (int pageNo);
   const byteString &pageReadChunk  (int pageNo, int offset, int nBytes);
   const byteString &pageReadValue  (int pageNo, int offset);
   const byteString &burnCommand    (int pageNo);
   const byteString &ochGetCommand  (int pageNo);
   const byteString &ochBurstCommand(int pageNo);

   void              pageModified   (int pageNo, bool state);

public:
   controllerDescriptor(msComm &IO);
  ~controllerDescriptor();

   BYTE   *_const;        // Make this private and clean up msDatabase.
   double *_userVar;      // private: All runtime vars and calculated ones.

   void init();  // Call after all adds and sets are done.

   bool changed()             { return _changed;    }
   void changed(bool changed) { _changed = changed; }

   BYTE &ConstB(int ofs, int pNo) { return _const[ofs+pageOffset(mxi(pNo,0))]; }
   BYTE &ConstR(int ofs)          { return _const[ofs]; }

   BYTE  getB(int pNo, int ofs, int db=0);
   WORD  getW(int pNo, int ofs, int db=0);
   DWORD getD(int pNo, int ofs, int db=0);

   void  putB(DWORD v, int pNo, int ofs, int db=0);
   void  putW(DWORD v, int pNo, int ofs, int db=0);
   void  putD(DWORD v, int pNo, int ofs, int db=0);

   void setNOPages        (int n);
   void setOchBlockSize   (int        nBytes, int pageNo);
   void setOchGetCommand  (const char *cmd,   int pageNo);
   void setOchBurstCommand(const char *cmd,   int pageNo);
   void setOchPollPattern (const char *seq,   int i);
   void setDelay          (int delay) { _interWriteDelay = delay; }
   void setWriteBlocks    (bool blocks) { _writeBlocks = blocks; }

   void setPageActivationDelay(int delay) { _pageActivationDelay = delay; }
   void setBlockReadTimeout   (int delay) { _blockReadTimeout    = delay; }

   void setNPages         (int n);
   void setVerify         (bool setting);
   void setEndianness     (const char *cmd);
   void setVersionInfo    (const char *cmd);
   void setQueryCommand   (const char *cmd);
   void setSignature      (const byteString sig, const char *sigFile);

   void setPageSize       (int        size, int pageNo);
   void setBurnCommand    (const char *cmd, int pageNo);
   void setPageActivate   (const char *cmd, int pageNo);
   void setPageIdentifier (const char *cmd, int pageNo);

   void setPageReadWhole  (const char *cmd, int pageNo);
   void setPageReadChunk  (const char *cmd, int pageNo);
   void setPageReadValue  (const char *cmd, int pageNo);
   void setPageWriteWhole (const char *cmd, int pageNo);
   void setPageWriteChunk (const char *cmd, int pageNo);
   void setPageWriteValue (const char *cmd, int pageNo);

   const byteString &versionInfo    () const;
   const byteString &queryCommand   () const;
   const byteString &signature      () const;
   const char       *signatureFile  () const;

   int               nPages         ();
   int               pageSize       (int pageNo, int db=0);
   int               pageOffset     (int pageNo, int db=0);
   bool              pageModified   (int pageNo);

   const byteString &pageWriteWhole (int pageNo);
   const byteString &pageWriteChunk (int pageNo, int offset, int nBytes);
   const byteString &pageWriteValue (int pageNo, int offset);

   int   totalSpace();

   int   nOPages     ()              { return _nOPages; }
   int   totalOSpace ();
   int   ochBlockSize(int pg)                { return _oPage[mxi(pg,1)].siz(); }
   void  setOch      (int pg, BYTE v, int i) { _ochBuffer[pageOffset(pg,1)+i] = v; }
   int   ochSeqCount ();
   int   ochSeqSize  (int i);
   int   ochSeqValue (int i, int s);

   //---------------------------------------------------------------------------

   bool    sendPageActivate   (const symbol *s);
   bool    sendPageActivate   (int page, bool force=false);
   bool    sendPageReadWhole  (int page);
   bool    sendPageReadChunk  (int page, int offset, int nBytes);
   bool    sendPageReadValue  (int page, int offset);
   bool    sendBurnCommand    (int page);
   bool    sendOchGetCommand  (int page);
   bool    sendOchBurstCommand(int page);
   void    setOchPolled       (int pageNo, bool ok);

   bool    verify             (int page, int offset, int nBytes);

   bool    write(const BYTE *bytes, DWORD nBytes); // For made-up commands...
   bool    read (      BYTE *bytes, DWORD nBytes);
   bool    read (      BYTE &byte);
   void    flush();

   //--  Symbol management  ----------------------------------------------------

   symMap &symbolTab() { return symbolTable; }

   void    addSymbol(symbol *s);
   bool    send     (const symbol *s, int index=-1);
   bool    sendPage (int thisPage);

   symbol *lookup   (const CString &name);
   int     varIndex (const CString &name); // Return user variable index.

   int     addExpr(const char *expr);
   void    recalc();
   void    populateUserVars();

   //---------------------------------------------------------------------------

   void   serialize(CFile &msqFile);
   bool checkFile  (CFile &msqFile, CString &error); // Validates before deserialize.
   void deserialize(CFile &msqFile);

   void saveXML(const char *fileName);
   int  readXML(const char *fileName);

   void dump(FILE *dmp);

   //---------------------------------------------------------------------------
   //--  COM support  ----------------------------------------------------------

   int symbolCount() { return symbolTable.size(); }

   symbol *symbols(int index) 
   {
      symMapIter iter = symbolTable.begin ();
      for (int i = 0; i < index; i++) {
         if (iter == symbolTable.end ()) return NULL;
         iter++;
      }
      return (*iter).second;
   }
};

//------------------------------------------------------------------------------
#endif
