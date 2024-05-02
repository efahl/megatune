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
#include "megatune.h"
#include "megatuneDlg.h"
#include "repository.h"
#include "Dabout.h"
#include "Splash.h"

extern CString splashFile;

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------

Dabout::Dabout(CWnd *pParent)
 : dataDialog(Dabout::IDD, pParent)
{
   //{{AFX_DATA_INIT(Dabout)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void Dabout::DoDataExchange(CDataExchange *pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(Dabout)
      DDX_Control(pDX, IDC_ABOUT_TEXT, m_aboutText);
      DDX_Control(pDX, IDOK,           m_okButton );
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(Dabout, CDialog)
   //{{AFX_MSG_MAP(Dabout)
      ON_WM_CTLCOLOR()
      ON_WM_PAINT()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

BOOL Dabout::OnInitDialog() 
{
   dataDialog::OnInitDialog();

   SetWindowText(" About MegaTune " VERSION_STRING);

   m_aboutText.SetWindowText(
      "MegaTune Tuning Software\n"
      "by Eric Fahlgren <efahl on msefi.com>\n"
      "Version " VERSION_STRING " built on " __DATE__ " " __TIME__ "\n"
      "\n"
      "Originally derived from MegaSquirt PC Configurator\n"
      "by Bruce Bowling and Al Grippo.\n"
      "\n"
      "See http://www.megasquirt.info/ for full details."
   );

   if (LoadBitmap(splashFile, m_hBitmap, m_hPalette)) {
      BITMAP bm;
      GetObject(m_hBitmap, sizeof(BITMAP), &bm);

      CRect rct;
      GetWindowRect(&rct);
      m_width = max(rct.Width(), bm.bmWidth+20);
      MoveWindow(0, 0, m_width, rct.Height()+bm.bmHeight+5);
      CenterWindow();

      m_aboutText.GetWindowRect(&rct);
      ScreenToClient(&rct);
      m_top   = rct.bottom+10;

      m_okButton.GetWindowRect(&rct);
      ScreenToClient(&rct);
      m_okButton.MoveWindow(m_width-rct.Width()-15, rct.top, rct.Width(), rct.Height());
   }

// m_aboutText.BringWindowToTop();

   return TRUE;
}

//------------------------------------------------------------------------------

void Dabout::OnPaint() 
{
   if (m_hBitmap) {
      CPaintDC dc(this);

      CDC dcImage;
      if (dcImage.CreateCompatibleDC(&dc)) {
         BITMAP bm;
         GetObject(m_hBitmap, sizeof(BITMAP), &bm);
 //m_top = 10;
         HBITMAP hOldBitmap = HBITMAP(SelectObject(dcImage, m_hBitmap));
            BitBlt(dc, (m_width-bm.bmWidth)/2-3, m_top, bm.bmWidth, bm.bmHeight, dcImage, 0, 0, SRCCOPY);
         SelectObject(dcImage, hOldBitmap);
      }
   }

   dataDialog::OnPaint();
}

//------------------------------------------------------------------------------

HBRUSH Dabout::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
   HBRUSH hbr;
   if (true || pWnd->GetDlgCtrlID() != IDC_ABOUT_TEXT)
      hbr = dataDialog::OnCtlColor(pDC, pWnd, nCtlColor);
   else {
      pDC->SetBkMode(TRANSPARENT);
      pDC->SetTextColor(RGB(0,0,0));
      pDC->SetBkColor(RGB(255,255,255));
      static CBrush brush;
      brush.DeleteObject();
      static CBitmap bm;
      bm.FromHandle(m_hBitmap);
      brush.CreatePatternBrush(&bm);
      hbr = brush;
   }

   return hbr;
}

//------------------------------------------------------------------------------
