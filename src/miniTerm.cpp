//------------------------------------------------------------------------------
//--  Copyright (c) 2005,2006 by Eric Fahlgren, All Rights Reserved.          --
//--                                                                          --
//--  This program is free software; you can redistribute it and/or           --
//--  modify it under the terms of the GNU General Public License             --
//--  as published by the Free Software Foundation; either version            --
//--  2 of the License, or (at your option) any later version.                --
//--  See http://www.gnu.org/licenses/gpl.txt                                 --
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "megatune.h"
#include "miniTerm.h"
#include "msDatabase.h"

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

extern msDatabase mdb;

//------------------------------------------------------------------------------

miniTerm::miniTerm(CWnd *pParent)
 : dataDialog(miniTerm::IDD, pParent)
 , realized  (false)
{
   //{{AFX_DATA_INIT(miniTerm)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void miniTerm::DoDataExchange(CDataExchange* pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(miniTerm)
      DDX_Control(pDX, IDC_TEXT,        m_text);
      DDX_Control(pDX, IDC_SELECTSPEED, m_baudRate);
      DDX_Control(pDX, IDC_SELECTCOMM,  m_commPort);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(miniTerm, dataDialog)
   //{{AFX_MSG_MAP(miniTerm)
      ON_WM_CTLCOLOR()
      ON_WM_DESTROY ()
      ON_WM_PAINT   ()
      ON_WM_TIMER   ()
      ON_CBN_SELCHANGE(IDC_SELECTCOMM,  OnSettingsChanged)
      ON_CBN_SELCHANGE(IDC_SELECTSPEED, OnSettingsChanged)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

BOOL miniTerm::OnInitDialog() 
{
   realized = false;

   dataDialog::OnInitDialog("MiniTerm");

   CString s;
   for (int i = 1; i <= 99; i++) {
      s.Format("COM%d", i);
      m_commPort.AddString(s);
   }
   m_commPort.SetCurSel(mdb.port() - 1);

   m_baudRate.AddString(  "9600");
   m_baudRate.AddString( "19200");
   m_baudRate.AddString( "38400");
   m_baudRate.AddString( "57600");
   m_baudRate.AddString("115200");
   switch (mdb.rate()) {
      case   9600: m_baudRate.SetCurSel(0); break;
      case  19200: m_baudRate.SetCurSel(1); break;
      case  38400: m_baudRate.SetCurSel(2); break;
      case  57600: m_baudRate.SetCurSel(3); break;
      case 115200: m_baudRate.SetCurSel(4); break;
      default    : m_baudRate.SetCurSel(0); break;
   }
   
   return TRUE;
}

void miniTerm::OnDestroy() 
{
   KillTimer(1);
}

//------------------------------------------------------------------------------

void miniTerm::OnSettingsChanged()
{
   int commPortNumber = m_commPort.GetCurSel() + 1;

   int commPortRate = 9600;
   switch (m_baudRate.GetCurSel()) {
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

void miniTerm::OnPaint() 
{
   CPaintDC dc(this);

   if (!realized) {
      SetTimer(1, 10, NULL);
      realized = true;
   }
}

//------------------------------------------------------------------------------

HBRUSH miniTerm::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
   HBRUSH hbr = dataDialog::OnCtlColor(pDC, pWnd, nCtlColor);

   if (pWnd->GetDlgCtrlID() == IDC_TEXT) {
      pDC->SetTextColor(RGB(0,0,0));
      pDC->SetBkColor(RGB(255,255,255));

      static CBrush brush;
      brush.DeleteObject();
      brush.CreateSolidBrush(RGB(255,255,255));
      hbr = brush;
   }

   return hbr;
}

//------------------------------------------------------------------------------

void miniTerm::OnTimer(UINT nIDEvent) 
{
   CString errorMsg;
   BYTE q;
   while (mdb.cDesc.read(q)) errorMsg += q;
   if (!errorMsg.IsEmpty()) {
      m_text.SetSel(9999,9999);
      m_text.ReplaceSel(errorMsg);
      m_text.UpdateWindow();
   }

   dataDialog::OnTimer(nIDEvent);
}

//------------------------------------------------------------------------------

BOOL miniTerm::PreTranslateMessage(MSG* pMsg) 
{
   static bool shifted = false;
   static bool ctrled  = false;

   if (pMsg->message == WM_KEYUP) {
      switch (pMsg->wParam) {
         case VK_SHIFT  : shifted = false; return TRUE;
         case VK_CONTROL: ctrled  = false; return TRUE;
      }
   }
   if (pMsg->message == WM_KEYDOWN) {
      if (pMsg->wParam == VK_SHIFT  ) { shifted = true;  return TRUE; }
      if (pMsg->wParam == VK_CONTROL) { ctrled  = true;  return TRUE; }

      char c = MapVirtualKey(pMsg->wParam, 2);
      if (c >= ' ' && c < 127) {
         if (!shifted) {
            if (c >= 'A' && c <= 'Z') c |= 0x20;
         }
         else {
            switch (c) {
               case '`' : c = '~'; break;
               case '1' : c = '!'; break;
               case '2' : c = '@'; break;
               case '3' : c = '#'; break;
               case '4' : c = '$'; break;
               case '5' : c = '%'; break;
               case '6' : c = '^'; break;
               case '7' : c = '&'; break;
               case '8' : c = '*'; break;
               case '9' : c = '('; break;
               case '0' : c = ')'; break;
               case '-' : c = '_'; break;
               case '=' : c = '+'; break;
               case '[' : c = '{'; break;
               case ']' : c = '}'; break;
               case '\\': c = '|'; break;
               case ';' : c = ':'; break;
               case '\'': c = '"'; break;
               case ',' : c = '<'; break;
               case '.' : c = '>'; break;
               case '/' : c = '?'; break;
            }
         }
         if ( ctrled ) c &= 0x1F;
         mdb.cDesc.write(reinterpret_cast<BYTE *>(&c), 1);
         CString s;
         s.Format("%c", c);
         m_text.SetSel(9999,9999);
         m_text.ReplaceSel(s);
      }
   }
   
   return dataDialog::PreTranslateMessage(pMsg);
}

//------------------------------------------------------------------------------
