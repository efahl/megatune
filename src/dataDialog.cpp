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

#include "stdafx.h"
#include "megatune.h"
#include "symbol.h"
#include "dataDialog.h"
#include "msDatabase.h"
#include "userHelp.h"
#include <math.h>

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

extern msDatabase   mdb;
extern userHelpList uhl;

//------------------------------------------------------------------------------

HICON dataDialog::m_hIcon = NULL;

dataDialog::dataDialog(int ID, CWnd *pParent)
 : CDialog  (ID, pParent)
 , _realized(false)
{
   //{{AFX_DATA_INIT(dataDialog)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void dataDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(dataDialog)
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(dataDialog, CDialog)
   //{{AFX_MSG_MAP(dataDialog)
      ON_WM_HELPINFO()
      ON_BN_CLICKED (ID_HELP_ABOUT, doHelp)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

BOOL dataDialog::OnInitDialog(const char *title, int pageNo)
{
   CDialog::OnInitDialog();

   _helpDlg = "";
   _hId     = 0;

   if (m_hIcon) {
      SetIcon(m_hIcon, TRUE);       // Set big icon
      SetIcon(m_hIcon, FALSE);      // Set small icon
   }

   char _title[128];
   if (title == NULL || title[0] == '\0') title = "MegaTune";
   if (mdb.cDesc.nPages() == 1 || pageNo == 0)
      sprintf(_title, " %s", title);
   else
      sprintf(_title, " %s - Page %d", title, pageNo);
   SetWindowText(_title);

   return TRUE;
}

//------------------------------------------------------------------------------

//void AfxHookWindowCreate  (CWnd *);
//void AfxUnhookWindowCreate();

bool dataDialog::realized() { return _realized; }

int dataDialog::DoModal()
{
   ASSERT(m_lpszTemplateName != NULL || m_hDialogTemplate != NULL || m_lpDialogTemplate != NULL);

   LPCDLGTEMPLATE lpDialogTemplate = m_lpDialogTemplate;
   HGLOBAL        hDialogTemplate  = m_hDialogTemplate;
   HINSTANCE      hInst            = AfxGetResourceHandle();

   if (m_lpszTemplateName != NULL) {
      hInst           = AfxFindResourceHandle(m_lpszTemplateName, RT_DIALOG);
      HRSRC hResource = ::FindResource(hInst, m_lpszTemplateName, RT_DIALOG);
      hDialogTemplate = LoadResource  (hInst, hResource);
   }

   if (hDialogTemplate != NULL) {
      lpDialogTemplate = LPCDLGTEMPLATE(LockResource(hDialogTemplate));
   }

   if (lpDialogTemplate == NULL) return -1;

   HWND hWndParent = PreModal();
// AfxUnhookWindowCreate();
   bool bEnableParent = false;
   if (hWndParent != NULL && ::IsWindowEnabled(hWndParent)) {
      ::EnableWindow(hWndParent, FALSE);
      bEnableParent = true;
   }

   TRY {
//    AfxHookWindowCreate(this);
      if (CreateDlgIndirect(lpDialogTemplate, CWnd::FromHandle(hWndParent), hInst)) {
         ShowWindow(SW_SHOWNORMAL);
         UpdateWindow();
         _realized = true;
         if (m_nFlags & WF_CONTINUEMODAL) {
            DWORD dwFlags = MLF_SHOWONIDLE;
            if (GetStyle() & DS_NOIDLEMSG) dwFlags |= MLF_NOIDLEMSG;
            VERIFY(RunModalLoop(dwFlags) == m_nModalResult);
         }

         if (m_hWnd != NULL) {
            SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW
                                         | SWP_NOSIZE
                                         | SWP_NOMOVE
                                         | SWP_NOACTIVATE
                                         | SWP_NOZORDER);
         }
      }
   }
   CATCH_ALL(e) {
//    DELETE_EXCEPTION(e);
      m_nModalResult = -1;
   }
   END_CATCH_ALL

   if (bEnableParent                                      ) ::EnableWindow   (hWndParent, TRUE);
   if (hWndParent != NULL && ::GetActiveWindow() == m_hWnd) ::SetActiveWindow(hWndParent);

   _realized = false;
   DestroyWindow();
   PostModal();

   if (m_lpszTemplateName != NULL || m_hDialogTemplate != NULL) UnlockResource(hDialogTemplate);
   if (m_lpszTemplateName != NULL                             ) FreeResource  (hDialogTemplate);

   return m_nModalResult;
}

//------------------------------------------------------------------------------

void dataDialog::moveItem(UINT id, int deltaH, int deltaV)
{
   CRect rct;
   CWnd *s = GetDlgItem(id);
   s->GetWindowRect(&rct);
   ScreenToClient(&rct);
   rct.OffsetRect(deltaH, deltaV);
   s->MoveWindow(rct, true);
}

//------------------------------------------------------------------------------

#define toUser(v,s,t) double(((v)+t)*s)
#define toIntr(v,s,t) int((v)/s-t)

void dataDialog::setFld(CEdit *c, int    i)
{
   char s[64];
   sprintf(s, "%d", i);
   c->SetWindowText(s);
}

void dataDialog::setFld(CEdit *c, double v, double scale, double trans, int w)
{
   char s[64];
   sprintf(s, "%.*f", w, toUser(v, scale, trans));
   c->SetWindowText(s);
}

void dataDialog::setCmb(CComboBox *c, int v, int bitLo, int bitHi)
{
   int mask = ((1 << (bitHi-bitLo+1)) - 1) << bitLo;
   int val  = (v & mask) >> bitLo;
   for (int i = 0; i < c->GetCount(); i++) {
      if (c->GetItemData(i) == val) {
         c->SetCurSel(i);
         break;
      }
   }
}

//------------------------------------------------------------------------------

double dataDialog::getRaw(CEdit *fld, double lo, double hi, const char *label)
{
   char   pp [ 64];
   char   msg[255];
   double dtt;
   int    fldWid = 1;

   fld->GetWindowText(pp, sizeof(pp)-1);
   sscanf(pp, "%lf", &dtt);

   if (lo != symbol::noRange && dtt < lo) {
      sprintf(msg, "%.*f", fldWid, lo);
      fld->SetWindowText(msg);
      fld->SetFocus();
      sprintf(msg, "ERROR: %s values must be >= %.*f, entry is %.*f (set to %.*f)", label, fldWid, lo, fldWid, dtt, fldWid, lo);
      MessageBox(msg);
   }

   if (hi != symbol::noRange && dtt > hi) {
      sprintf(msg, "%.*f", fldWid, hi);
      fld->SetWindowText(msg);
      fld->SetFocus();
      sprintf(msg, "ERROR: %s values must be <= %.*f, entry is %.*f (set to %.*f)", label, fldWid, hi, fldWid, dtt, fldWid, hi);
      MessageBox(msg);
   }

   return dtt;
}

//------------------------------------------------------------------------------

int dataDialog::getFld(CEdit *fld, double lo, double hi, const char *label, double scale, int trans, int digits)
{
   char   msg[255];
   int    fldWid = scale >= 1.0 ? 0 : 1;

   char   pp[64];
   fld->GetWindowText(pp, sizeof(pp)-1);

   double round  = digits > 0 ? 0.5 * pow(10.0, -digits) : 0.0;
   double dtt    = strtod(pp, NULL);
   if (dtt < 0.0) round = -round;

   int itt = toIntr((dtt+round), scale, trans);

   if (lo != symbol::noRange && dtt < lo) {
      itt = toIntr(lo, scale, trans);
      sprintf(msg, "ERROR: %s must be >= %.*f, entry is %.*f (set to %.*f).", label, fldWid, lo, fldWid, dtt, fldWid, lo);
      MessageBox(msg);
      sprintf(msg, "%.*f", fldWid, toUser(itt, scale, trans));
      fld->SetWindowText(msg);
//    fld->SetFocus();
   }

   if (hi != symbol::noRange && dtt > hi) {
      itt = toIntr(hi, scale, trans);
      sprintf(msg, "ERROR: %s must be <= %.*f, entry is %.*f (set to %.*f).", label, fldWid, hi, fldWid, dtt, fldWid, hi);
      MessageBox(msg);
      sprintf(msg, "%.*f", fldWid, toUser(itt, scale, trans));
      fld->SetWindowText(msg);
//    fld->SetFocus();
   }

   return itt;
}

//------------------------------------------------------------------------------

int dataDialog::getCmb(CComboBox *cmb, int byteVal, int bitLo, int bitHi)
{
   int maxVal = (1 << (bitHi-bitLo+1)) - 1;
   int val    = cmb->GetItemData(cmb->GetCurSel());
   if (val < 0     ) val = 0;
   if (val > maxVal) val = maxVal;
   maxVal <<= bitLo; // Turn it into the mask.
   return (byteVal & ~maxVal) | (val << bitLo);
}

//==============================================================================

void dataDialog::setFld(CEdit *c, const symbol *s, int index)
{
   if (s != NULL)
      c->SetWindowText(s->valueString(index));
   else {
      c->EnableWindow(false);
      CString ss; ss.Format("%d", index);
      c->SetWindowText(ss);
   }
}

void dataDialog::setCmb(CComboBox *c, const symbol *s)
{
   for (int i = 0; i < c->GetCount(); i++) {
      if (c->GetItemData(i) == s->valueRaw()) {
         c->SetCurSel(i);
         break;
      }
   }
}

//------------------------------------------------------------------------------

void dataDialog::getFld(CEdit *c, const symbol *s, int index)
{
   char pp[64];
   c->GetWindowText(pp, sizeof(pp)-1);
   double dtt = strtod(pp, NULL);

   if (s->tooLow(dtt)) {
      char msg[255];
      sprintf(msg, "ERROR: %s must be >= %s, entry is %s (set to %s).",
                   s->label(index), s->loString(), pp, s->loString());
      MessageBox(msg);
      dtt = s->lo();
      c->SetWindowText(s->loString()); // Restore it to whatever it was before.
   }

   if (s->tooHigh(dtt)) {
      char msg[255];
      sprintf(msg, "ERROR: %s must be <= %s, entry is %s (set to %s).",
                   s->label(index), s->hiString(), pp, s->hiString());
      MessageBox(msg);
      dtt = s->hi();
      c->SetWindowText(s->hiString());
   }

   s->storeValue(dtt, index);
   mdb.cDesc.send(s, index);
}

//------------------------------------------------------------------------------

void dataDialog::getCmb(CComboBox *c, const symbol *s)
{
   s->storeRaw(c->GetItemData(c->GetCurSel()));
   mdb.cDesc.send(s);
}

//==============================================================================

const char *dataDialog::helpDlg() const { return _helpDlg; }

void        dataDialog::setHelp(CString h)
{
   _helpDlg = h; 
   _hId     = uhl.id(h);
}

bool dataDialog::doHelp()
{
   if (_hId) {
      userHelp help(_hId-ID_USER_HELP_00);
      help.DoModal();
      return true;
   }
   else if (!_helpDlg.IsEmpty()) {
      ShellExecute(NULL, "open", _helpDlg, NULL, NULL, SW_SHOWNORMAL); 
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------

BOOL dataDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
   if (doHelp())
      return TRUE;
   else
      return CDialog::OnHelpInfo(pHelpInfo);
}

//------------------------------------------------------------------------------
