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

static char *rcsId() { return "$Id$"; }

#include "stdafx.h"
#include "megatune.h"
#include "burstLog.h"
#include "msDatabase.h"

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------

extern msDatabase mdb;

burstLog::burstLog(CWnd *pParent)
 : dataDialog(burstLog::IDD, pParent)
{
   //{{AFX_DATA_INIT(burstLog)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void burstLog::DoDataExchange(CDataExchange* pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(burstLog)
      DDX_Control(pDX, IDCANCEL,      m_button);
      DDX_Control(pDX, IDC_MONITOR,   m_monitor);
      DDX_Control(pDX, IDC_FILE_INFO, m_fileInfo);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(burstLog, dataDialog)
   //{{AFX_MSG_MAP(burstLog)
      ON_WM_TIMER()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

BOOL burstLog::OnInitDialog() 
{
   dataDialog::OnInitDialog("Burst Logging");
   m_button.SetWindowText("&Cancel");
   m_nRecords = 0;
   m_time     = 0.0;
   m_initSecl = true;

   CString statusMsg;
   statusMsg.Format("Logging to %s\nComm port:         %6d\nComm rate:         %6d",
         mdb.m_logFileName, mdb.burstCommPort, mdb.burstCommRate);
   m_fileInfo.SetWindowText(statusMsg);

   m_secl = mdb.cDesc.lookup("secl");
   if (m_secl == NULL) m_secl = mdb.cDesc.lookup("seconds");
   if (SetTimer(1, 0, NULL) == 0) MessageBox("ERROR: Cannot install timer.\nKill other useless Windows Apps.");

   return TRUE;
}

//------------------------------------------------------------------------------

double burstLog::d_time = 0.10;

void burstLog::suggestedDeltaT(double dTime)
{
   d_time = dTime;
}

//------------------------------------------------------------------------------

BOOL burstLog::DestroyWindow() 
{
   return dataDialog::DestroyWindow();
}

//------------------------------------------------------------------------------

extern bool   fakingTime;
extern double fakeTime;

void burstLog::updateMonitor()
{
   char msg[128];
   sprintf(msg, "Read record %06d,   Synthetic Time %10.1f,   Measured dT %5.3f (%.1f Hz)", m_nRecords, m_time, d_time, 1.0/d_time);
   m_monitor.SetWindowText(msg);
}

void burstLog::OnTimer(UINT nIDEvent) 
{
   if (!mdb.stillBursting()) {
      updateMonitor(); // One last time for this log.
      KillTimer(1);   
      m_button.SetWindowText("&Close");
   }
   else {
      fakingTime = true;
      fakeTime = m_time;
      if (mdb.getRuntime()) {
         m_nRecords++;
         if (m_nRecords % 10 == 0) updateMonitor();
         m_time += d_time;

         if (m_secl) { // Attempt to adjust d_time.
            static int cTime    = 0;
            static int cRecs    = 0;
            static int prevTime = 0;
                   int thisTime = m_secl->valueUser();
            if (m_initSecl) {
               if (cRecs == 0 && prevTime == thisTime-1)
                  m_initSecl = false;
               prevTime = thisTime;
            }
            else {
               cRecs++;
               if (prevTime != thisTime) {
                  prevTime = thisTime;
                  cTime++;
                  d_time = double(cTime) / double(cRecs);
               }
            }
         }
      }
      fakingTime = false;
   }
}

//------------------------------------------------------------------------------
