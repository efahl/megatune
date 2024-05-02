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
#include "triggerWizard.h"
#include "msDatabase.h"

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------

extern msDatabase mdb;

triggerWizard::triggerWizard(int pageNo, CWnd *pParent)
 : dataDialog(triggerWizard::IDD, pParent)
 , _pageNo   (pageNo)
 , _advance (0.0)
{
   _triggerOffset   = mdb.lookupByPage("triggerOffset", _pageNo);
   _advanceVariable = mdb.cDesc.lookup("advance");
   //{{AFX_DATA_INIT(triggerWizard)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void triggerWizard::DoDataExchange(CDataExchange* pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(triggerWizard)
      DDX_Control(pDX, IDC_GAUGE,         m_gauge);
      DDX_Control(pDX, IDC_RETARD,        m_retardButton);
      DDX_Control(pDX, IDC_ADVANCE,       m_advanceButton);
      DDX_Control(pDX, IDC_TRIGGER_ANGLE, m_triggerOffset);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(triggerWizard, dataDialog)
   //{{AFX_MSG_MAP(triggerWizard)
      ON_EN_KILLFOCUS(IDC_TRIGGER_ANGLE, dbFromFields)
      ON_BN_CLICKED  (IDC_RETARD,        OnRetard)
      ON_BN_CLICKED  (IDC_ADVANCE,       OnAdvance)
      ON_BN_CLICKED  (IDC_BURN,          OnBurn)

      ON_WM_PAINT()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

BOOL triggerWizard::OnInitDialog() 
{
   dataDialog::OnInitDialog("Trigger Offset");
                                                      
   static CBitmap ua;
   ua.LoadBitmap(IDB_UP_ARROW);
   m_advanceButton.SetBitmap(ua);

   static CBitmap da;
   da.LoadBitmap(IDB_DN_ARROW);
   m_retardButton.SetBitmap(da);   

   if (_triggerOffset) setFld(&m_triggerOffset, _triggerOffset);

   return TRUE;
}

//------------------------------------------------------------------------------

void triggerWizard::OnPaint() 
{
   CPaintDC dc(this); // device context for painting

   CFont fontNew;
   fontNew.CreateFont(144, 0, 0, 0, 400, // 400 is regular
                      false, false, false, DEFAULT_CHARSET,
                      OUT_OUTLINE_PRECIS,
                      CLIP_DEFAULT_PRECIS,
                      PROOF_QUALITY,
                      DEFAULT_PITCH | FF_DONTCARE,
                      "Verdana");
   
   CFont *fontOld = dc.SelectObject(&fontNew);

   CRect r;
   m_gauge.GetWindowRect(&r);
   ScreenToClient(&r);
   dc.Draw3dRect(r, RGB(0,0,0), RGB(0,0,0));
   r.DeflateRect(1,1);
   dc.FillSolidRect(r, RGB(255,255,255));

   TEXTMETRIC TM;
   dc.GetTextMetrics(&TM);
   CString txt;
   txt.Format("%0.1f", _advance);

   dc.SetTextAlign(TA_RIGHT | TA_TOP);
   dc.SetTextColor(RGB(0,0,0));
   dc.TextOut(r.right-10, (r.bottom-r.top)/2 - TM.tmAscent/2 - 9, txt);

   dc.SelectObject(fontOld);
   dc.TextOut(r.right-5, r.bottom-20, "Match above value with timing light reading");

   ValidateRect(r);
}

//------------------------------------------------------------------------------

bool triggerWizard::doTimer(bool connected)
{
   if (!realized()) return false;

   if (connected) {
      if (_advanceVariable && _advanceVariable->valueUser() != _advance) {
         _advance = _advanceVariable->valueUser();
         Invalidate();
      }
   }

   return true;
}

//------------------------------------------------------------------------------

void triggerWizard::OnRetard() 
{
   if (_triggerOffset) {
      _triggerOffset->incRaw(+5);
      setFld(&m_triggerOffset, _triggerOffset);
      mdb.cDesc.send(_triggerOffset);
   }
}

//------------------------------------------------------------------------------

void triggerWizard::OnAdvance() 
{
   if (_triggerOffset) {
      _triggerOffset->incRaw(-5);
      setFld(&m_triggerOffset, _triggerOffset);
      mdb.cDesc.send(_triggerOffset);
   }
}

//------------------------------------------------------------------------------

BOOL triggerWizard::PreTranslateMessage(MSG* pMsg) 
{
   static bool shifted = false;

   if (pMsg->message == WM_KEYUP) {
      switch (pMsg->wParam) {
         case VK_SHIFT  : shifted = false; return TRUE;
      }
   }

   if (pMsg->message == WM_KEYDOWN) {
      switch (pMsg->wParam) {
         case VK_SHIFT: shifted = true;  return TRUE;
         case VK_UP   :
            if (shifted) { OnAdvance(); return TRUE; }
            break;
         case VK_DOWN :
            if (shifted) { OnRetard();  return TRUE; }
            break;
      }
   }
   
   return dataDialog::PreTranslateMessage(pMsg);
}

//------------------------------------------------------------------------------

void triggerWizard::OnBurn()
{
   int savePage = mdb.setPageNo(_pageNo);
      mdb.burnConst(_pageNo);
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------

void triggerWizard::dbFromFields()
{
   int savePage = mdb.setPageNo(_pageNo);
      if (_triggerOffset) getFld(&m_triggerOffset, _triggerOffset);
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------
