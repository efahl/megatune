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

#ifndef MSCOMM_H
#define MSCOMM_H 1

//------------------------------------------------------------------------------

#include "byteString.h"

class msComm {
   int                   commPortNumber;
   int                   commPortRate;
   HANDLE                hPortHandle;

   bool _logging;
   bool _isOpen;
   bool _singleOperation;
   int  _interWriteDelay;
   int  _totalReadTimeout;

   enum logType { rd, wr, ro, pu, cl };
   void logOpen();
   void log(logType lt, bool ok, const BYTE *buf, int len);
   void beginLoggedOperation();

   bool open();
   void close();
   void reOpen();

public:
   msComm(int port, int rate);
  ~msComm();

   void logEvent(CString s);

   void reset(int port, int rate);

   bool toggleCommLogging();

   bool read(BYTE *pBuf, DWORD dwExpected);
   bool read(BYTE &pBYTE);

   bool write(const BYTE *pBuf, DWORD dwCount);
   bool write(const char *pBuf);
   bool write(const byteString &bString);

   bool flush(); // Purges input and output buffer.

   bool isOpen() { return _isOpen; }
   int  port() { return commPortNumber; }
   int  rate() { return commPortRate;   }

   void setChunking    (bool singleOperation, int interWriteDelay);
   void setReadTimeouts(int blockReadTimeout);
};

//------------------------------------------------------------------------------

#endif
