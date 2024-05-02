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

#ifndef AFX_BURSTLOG_H
#define AFX_BURSTLOG_H 1

#if _MSC_VER > 1000
#  pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------

class burstLog : public dataDialog
{
   static double  d_time; // Remember between runs.

   int     m_nRecords;
   double  m_time;
   bool    m_initSecl;
   symbol *m_secl;

   void updateMonitor();

public:
   burstLog(CWnd* pParent = NULL);   // standard constructor

   static void suggestedDeltaT(double dTime);

   //{{AFX_DATA(burstLog)
      enum { IDD = IDD_BURSTING };
      CButton   m_button;
      CStatic   m_monitor;
      CStatic   m_fileInfo;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(burstLog)
      public:
         virtual BOOL DestroyWindow();
      protected:
         virtual void DoDataExchange(CDataExchange* pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(burstLog)
      virtual BOOL OnInitDialog();
      afx_msg void OnTimer(UINT nIDEvent);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_BURSTLOG_H
