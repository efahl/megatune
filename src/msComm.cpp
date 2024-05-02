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
#include <Mmsystem.h>
#include "megatune.h"
#include "msComm.h"

//------------------------------------------------------------------------------

msComm::msComm(int port, int rate)
 : commPortNumber   (port)
 , commPortRate     (rate)
 , _logging         (false)
 , _singleOperation (false)
 , _interWriteDelay (0)
 , _totalReadTimeout(25)
{
   if (getenv("AUTOLOG")) _logging = true;
   _isOpen = open();
}

msComm::~msComm()
{
   beginLoggedOperation();
   log(cl, true, 0, -1);
   close();
}

//------------------------------------------------------------------------------

void msComm::setChunking(bool singleOperation, int interWriteDelay)
{
   _singleOperation = singleOperation;
   _interWriteDelay = interWriteDelay;
}

void msComm::setReadTimeouts(int blockReadTimeout)
{
   _totalReadTimeout = blockReadTimeout;
}

//------------------------------------------------------------------------------

void msComm::reset(int port, int rate)
{
   close();
   commPortNumber = port;
   commPortRate   = rate;
   _isOpen = open();
}

void msComm::reOpen()
{
   close();
   _isOpen = open();
}

//------------------------------------------------------------------------------

bool msComm::toggleCommLogging()
{
   return _logging = !_logging;
}

//------------------------------------------------------------------------------

static long timeNow    = 0;
static long totalWrit  = 0;
static long totalRead  = 0;

void msComm::beginLoggedOperation()
{
   timeNow = timeGetTime();
}

static FILE *logFile = NULL;

void msComm::logOpen()
{
   logFile = fopen("comm.log", "w");
   if (logFile) {
      fprintf(logFile, "Comm port:         %6d\nComm rate:         %6d\n", commPortNumber, commPortRate);
      COMMPROP commProp;
      GetCommProperties(hPortHandle, &commProp);
      fprintf(logFile, "txBuffer:          %6d bytes\nrxBuffer:          %6d bytes\n",
            commProp.dwCurrentTxQueue, commProp.dwCurrentRxQueue);
      fprintf(logFile, "Read timeout:      %6d ms\nBlock writes: %11s\nInter-write delay: %6d ms\n",
            _totalReadTimeout, _singleOperation?"on":"off", _interWriteDelay);
      fprintf(logFile, "\nSee winbase.h values of constants CE_xxx for error codes.\n\n");
      fprintf(logFile, " Millisec   Opr Data");
   }
}

static CString msg = "";

void msComm::logEvent(CString s)
{
   if (logFile == NULL) logOpen();
   msg += s + "\n";
}

void msComm::log(logType op, bool ok, const BYTE *buf, int len)
{
   static long timeStart = timeGetTime();

   if (_logging) {
      static logType  lastOp = rd;
      static char    *type[5] = { "RCV", "SND", "REOPEN", "PURGE", "CLOSE\n" };
      static int      pos = 0;

      static int   nWrit   = 0;
      static int   nRead   = 0;
      if (logFile == NULL) logOpen();

      if (logFile != NULL) {
         if (op != lastOp) {
            switch (lastOp) {
               case rd:
                  if (nRead) fprintf(logFile, "\n                %04d bytes read\n", nRead);
                  totalRead += nRead;
                  nRead = 0;
                  break;
               case wr:
                  if (nWrit) fprintf(logFile, "\n                %04d bytes written\n", nWrit);
                  totalWrit += nWrit;
                  nWrit = 0;
                  break;
            }
            if (!msg.IsEmpty()) {
               fprintf(logFile, "%s", msg);
               msg = "";
            }
         }

         if (op != lastOp || op != rd || !ok) {
            pos = 0;
            fprintf(logFile, "\n%9d %c %s", timeNow-timeStart, ok?' ':'X', type[op]);
            if (op == ro || op == pu) return;
         }

         if (true || ok) {
            if (op == rd) nRead += len;
            else          nWrit += len;
         }
         lastOp = op;

         for (int i = 0; i < len; i++) {
//          if (pos > 0 && pos % 16 == 0) fprintf(logFile, "\n%9d      ", timeNow-timeStart);
            if (pos > 0 && pos % 16 == 0) fprintf(logFile, "\n%9s      ", "");
            if (pos == 0 && op == wr && buf[0] >= 'A' && buf[0] <= 'z')
               fprintf(logFile, " %c ", buf[0]);
            else
               fprintf(logFile, " %02x", buf[i]);
            pos++;
         }

         if (op == cl) {
            fprintf(logFile, "Totals: %d written, %d read\n", totalWrit, totalRead);
         }

         if (!ok) {
            DWORD errors = 0;
            ClearCommError(hPortHandle, &errors, NULL);
            fprintf(logFile, "\n%9d   ERROR 0x%08x", timeNow-timeStart, errors);
         }
         fflush(logFile);
      }
   }
}

//------------------------------------------------------------------------------
//--  Lifted substantially from Tom Dolsky's hw2sys routines.                 --

bool msComm::open()
{
   char pPortName[6];
   sprintf(pPortName, "com%d", commPortNumber);
   hPortHandle = CreateFile(pPortName,
                    GENERIC_READ | GENERIC_WRITE,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);
   if (hPortHandle == INVALID_HANDLE_VALUE) return false;

   DWORD baudRate = commPortRate;
#if 0
   switch (commPortRate) {
      case   9600: baudRate = CBR_9600;   break;
      case  19200: baudRate = CBR_19200;  break;
      case  38400: baudRate = CBR_38400;  break;
      case  56000: baudRate = CBR_56000;  break;
      case  57600: baudRate = CBR_57600;  break;
      case 115200: baudRate = CBR_115200; break;
      case 128000: baudRate = CBR_128000; break;
      default    : baudRate = CBR_9600;   break;
   }
#endif

   DCB dcb;
   if (!GetCommState(hPortHandle, &dcb)) return false;
   dcb.BaudRate          = baudRate;
   dcb.ByteSize          = 8;
   dcb.Parity            = NOPARITY;
   dcb.StopBits          = ONESTOPBIT;
#if 0
   dcb.fBinary           = true;
   dcb.fOutX             = false;
   dcb.fInX              = false;
   dcb.fOutxCtsFlow      = false;
   dcb.fOutxDsrFlow      = false;
   dcb.fDtrControl       = DTR_CONTROL_DISABLE;
   dcb.fDsrSensitivity   = false;
   dcb.fErrorChar        = false;
   dcb.fNull             = false;
   dcb.fRtsControl       = RTS_CONTROL_DISABLE;
   dcb.fAbortOnError     = false;
#endif
   if (!SetCommState(hPortHandle, &dcb)) return false;

   COMMTIMEOUTS timeout;
   if (!GetCommTimeouts(hPortHandle, &timeout)) return false;
#define MS_II_SEEMS_TO_WORK 0
#if MS_II_SEEMS_TO_WORK
   timeout.ReadIntervalTimeout         =   2; // ms between chars on read.
   timeout.ReadTotalTimeoutMultiplier  =   0;
   timeout.WriteTotalTimeoutMultiplier =   0;
   timeout.WriteTotalTimeoutConstant   =  25;
#else
   timeout.ReadIntervalTimeout         =   0; // ms between chars on read.
   timeout.ReadTotalTimeoutMultiplier  =   0;
   timeout.ReadTotalTimeoutConstant    = _totalReadTimeout; // 25 for MS-I, 250 for MS-II

   timeout.WriteTotalTimeoutMultiplier =   2;
   timeout.WriteTotalTimeoutConstant   =  25;
#endif
   if (!SetCommTimeouts(hPortHandle, &timeout)) return false;

#if 0
   if (!SetupComm(hPortHandle, 8192, 4096)) return false;
#endif

   return true;
}

//------------------------------------------------------------------------------

void msComm::close()
{
   if (hPortHandle != NULL) {
      CloseHandle(hPortHandle);
      hPortHandle = NULL;
   }
}

//------------------------------------------------------------------------------
//--  Non-blocking read of byte stream from from the comm port.  ---------------

bool msComm::read(BYTE *pBuf, DWORD expected)
{
   bool bOK = true;
   if (expected > 0) {
      DWORD bytesRead = 0;
      
      beginLoggedOperation();
      if (/*_singleOperation ||*/ expected == 1)
         bOK = 0 != ReadFile(hPortHandle, pBuf, expected, &bytesRead, NULL);
      else {
         for (unsigned i = 0; i < expected && bOK; i++) {
            DWORD read;
            bOK = 0 != ReadFile(hPortHandle, pBuf+i, 1, &read, NULL);
            if (bOK && read == 0) bOK = false;
            bytesRead += read;
         }
      }

      if (bytesRead != expected) bOK = false;
      log(rd, bOK, pBuf, bytesRead);

      static int successiveFailures = 0;
      if (bOK)
         successiveFailures = 0;
      else {
         successiveFailures++;
         if (successiveFailures > 10) {
            beginLoggedOperation();
            reOpen();
            successiveFailures = 0;
            log(ro, bOK, 0, -1);
         }
      }
   }
   return bOK;
}

bool msComm::read(BYTE &pChar)
{
   return read(&pChar, 1);
}

//------------------------------------------------------------------------------

bool msComm::write(const BYTE *pBuf, DWORD count)
{
   bool bOK = true;
   if (count > 0) {
      DWORD bytesWritten = 0;

      beginLoggedOperation();
      if (_singleOperation || count == 1)
         bOK = 0 != WriteFile(hPortHandle, pBuf, count, &bytesWritten, NULL);
      else {
         for (unsigned i = 0; i < count && bOK; i++) {
            DWORD written;
            bOK = 0 != WriteFile(hPortHandle, pBuf+i, 1, &written, NULL);
            if (_interWriteDelay) ::Sleep(_interWriteDelay);
            bytesWritten += written;
         }
      }

      if (bytesWritten != count) bOK = false;
      log(wr, bOK, pBuf, count);
   }
   return bOK;
}

bool msComm::write(const char *pBuf)
{
   return write(reinterpret_cast<const BYTE *>(pBuf), strlen(pBuf));
}

bool msComm::write(const byteString &bString)
{
   return write(bString.ptr(), bString.len());
}

//------------------------------------------------------------------------------

bool msComm::flush()
{
   beginLoggedOperation();
   bool bOK = PurgeComm(hPortHandle, PURGE_RXCLEAR | PURGE_TXCLEAR) == TRUE;
   log(pu, bOK, 0, -1);
   return bOK;
}

//------------------------------------------------------------------------------
