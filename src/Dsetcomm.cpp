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
//*****************************************
//******** PC Configurator V1.00 **********
//*** (C) - 2001 B.Bowling/A. Grippo ******
//** All derivatives from this software ***
//**  are required to keep this header ****
//*****************************************

static char *rcsId() { return "$Id$"; }

#include "stdafx.h"
#include "megatune.h"
#include "msDatabase.h"
#include "Dsetcomm.h"

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

extern msDatabase mdb;

//------------------------------------------------------------------------------

Dsetcomm::Dsetcomm(CWnd *pParent)
 : dataDialog(Dsetcomm::IDD, pParent)
{
   //{{AFX_DATA_INIT(Dsetcomm)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void Dsetcomm::DoDataExchange(CDataExchange *pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(Dsetcomm)
      DDX_Control(pDX, IDC_SELECTSPEED,   m_selspeed);
      DDX_Control(pDX, IDC_TEST_STATUS,   m_Status);
      DDX_Control(pDX, IDC_TIMERINTERVAL, m_timerInterval);
      DDX_Control(pDX, IDC_SELECTCOMM,    m_selserial);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(Dsetcomm, dataDialog)
   //{{AFX_MSG_MAP(Dsetcomm)
      ON_BN_CLICKED   (IDC_TESTCOMM,      OnTestcomm)
      ON_CBN_SELCHANGE(IDC_SELECTCOMM,    OnSelchangeSelectcomm)
      ON_EN_KILLFOCUS (IDC_TIMERINTERVAL, OnSelchangeSelectcomm)
      ON_CBN_SELCHANGE(IDC_SELECTSPEED,   OnSelchangeSelectcomm)
      ON_WM_CTLCOLOR()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

void Dsetcomm::OnTestcomm()
{
   if (!mdb.getRuntime())
      m_Status.SetWindowText("No response!");
   else
      m_Status.SetWindowText("Success!");
}

//------------------------------------------------------------------------------

void Dsetcomm::OnSelchangeSelectcomm()
{
   m_Status.SetWindowText("Untested.");

   int commPortNumber = m_selserial.GetCurSel() + 1;

   CString hhh;
   LPTSTR  pp;
   int     ti;
   m_timerInterval.GetWindowText(hhh);
   pp = hhh.GetBuffer(10); sscanf(pp, "%d", &ti); hhh.ReleaseBuffer();
   mdb.userTimerInt = ti;

   int commPortRate = 9600;
   switch (m_selspeed.GetCurSel()) {
      case  0: commPortRate =   9600; break; 
      case  1: commPortRate =  19200; break;
      case  2: commPortRate =  38400; break;
      case  3: commPortRate =  57600; break;
      case  4: commPortRate = 115200; break;
      default: commPortRate =   9600; break;
   }

   if (mdb.setByName("baud", 0, commPortRate)) { // Try to set controller value.
      mdb.sendByName("baud", 0);
      mdb.cDesc.sendBurnCommand(0);
   }

   mdb.writeConfig(commPortNumber, commPortRate);

   UpdateData(false);
}

//------------------------------------------------------------------------------

BOOL Dsetcomm::OnInitDialog()
{
   dataDialog::OnInitDialog("Communication Settings");

   m_Status.SetWindowText("Untested.");

   char cbuf[100];

   for (int i = 1; i <= 99; i++) {
      sprintf(cbuf, "COM%d", i);
      m_selserial.AddString(cbuf);
   }
   m_selserial.SetCurSel(mdb.port() - 1);

   m_selspeed.AddString(  "9600");
   m_selspeed.AddString( "19200");
   m_selspeed.AddString( "38400");
   m_selspeed.AddString( "57600");
   m_selspeed.AddString("115200");
   switch (mdb.rate()) {
      case   9600: m_selspeed.SetCurSel(0); break;
      case  19200: m_selspeed.SetCurSel(1); break;
      case  38400: m_selspeed.SetCurSel(2); break;
      case  57600: m_selspeed.SetCurSel(3); break;
      case 115200: m_selspeed.SetCurSel(4); break;
      default    : m_selspeed.SetCurSel(0); break;
   }

   sprintf(cbuf, "%d", mdb.userTimerInt);
   m_timerInterval.SetWindowText(cbuf);
   return TRUE;
}

//------------------------------------------------------------------------------

HBRUSH Dsetcomm::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor) 
{
   HBRUSH hbr = dataDialog::OnCtlColor(pDC, pWnd, nCtlColor);
   if (pWnd->GetDlgCtrlID() == IDC_COMM_WARNING) {
      pDC->SetTextColor(RGB(255,255,255));
      pDC->SetBkMode(TRANSPARENT);
      static CBrush brush;
      brush.DeleteObject();
      brush.CreateSolidBrush(RGB(255,0,0));
      hbr = brush;
   }
   return hbr;
}

//------------------------------------------------------------------------------
