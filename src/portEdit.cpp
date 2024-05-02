//------------------------------------------------------------------------------
//--  Copyright (c) 2005,2006 by Eric Fahlgren, All Rights Reserved.          --
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
#include "dataDialog.h"
#include "msDatabase.h"
#include "portEdit.h"

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

extern msDatabase mdb;

//------------------------------------------------------------------------------

#pragma warning(disable:4355)

portEdit::portEdit(int pageNo, CWnd *pParent)
 : dataDialog(portEdit::IDD, pParent)
 , _pageNo   (pageNo)
{
   psEnabled    = mdb.lookupByPage("psEnabled",    pageNo);
   psCondition  = mdb.lookupByPage("psCondition",  pageNo);
   psConnector  = mdb.lookupByPage("psConnector",  pageNo);
   psOutSize    = mdb.lookupByPage("psOutSize",    pageNo);
   psOutOffset  = mdb.lookupByPage("psOutOffset",  pageNo);
   psThreshold  = mdb.lookupByPage("psThreshold",  pageNo);
   psHysteresis = mdb.lookupByPage("psHysteresis", pageNo);
   psInitValue  = mdb.lookupByPage("psInitValue",  pageNo);
   psPortValue  = mdb.lookupByPage("psPortValue",  pageNo);

   //{{AFX_DATA_INIT(portEdit)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void portEdit::DoDataExchange(CDataExchange* pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(portEdit)
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(portEdit, dataDialog)
   //{{AFX_MSG_MAP(portEdit)
      ON_BN_CLICKED   (IDC_UP,           OnUp)
      ON_BN_CLICKED   (IDC_DOWN,         OnDown)

      ON_CBN_SELCHANGE(IDC_PEPORT,       setPort)
      ON_BN_CLICKED   (IDC_PEENABLE,     enableChanged)
      ON_CBN_SELCHANGE(IDC_PECONNECTOR,  connectorChanged)

      ON_CONTROL_RANGE(CBN_SELCHANGE,    IDC_PECOND_1, IDC_PECOND_2, operatorChanged)
      ON_CONTROL_RANGE(CBN_SELCHANGE,    IDC_PEVAR_1,  IDC_PEVAR_2,  variableChanged)
   
      ON_CBN_SELCHANGE(IDC_PEINIT,       outputChanged)
      ON_CBN_SELCHANGE(IDC_PEVALUE,      outputChanged)

      ON_EN_KILLFOCUS (IDC_PETHRESH_1,   valueChanged)
      ON_EN_KILLFOCUS (IDC_PETHRESH_1,   valueChanged)
      ON_EN_KILLFOCUS (IDC_PEHYST_1,     valueChanged)
      ON_EN_KILLFOCUS (IDC_PEHYST_2,     valueChanged)

      ON_WM_HELPINFO()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

//BEGIN_EVENTSINK_MAP(portEdit, dataDialog)
//   //{{AFX_EVENTSINK_MAP(portEdit)
//   //}}AFX_EVENTSINK_MAP
//END_EVENTSINK_MAP()

//------------------------------------------------------------------------------

static const int   operators[]  = { '<', '=', '>' };
static const char *opLabels []  = { "<", "=", ">" };
static const int  nOperators    = sizeof(operators)/sizeof(char*);

static const int   connectors[] = { ' ',                   '&',   '|'   };
static const char *connLabel [] = { "No second condition", "And", "Or"  };
static const int  nConnectors   = sizeof(connectors)/sizeof(char*);

BOOL portEdit::OnInitDialog()
{
   dataDialog::OnInitDialog("Generic Port Settings"); //, _pageNo+1);

   //---------------------------------------------------------------------------
   //--  Get pointers to the widgets.  -----------------------------------------
   _enabled       = static_cast<CButton   *>(GetDlgItem(IDC_PEENABLE));
   _portName      = static_cast<CComboBox *>(GetDlgItem(IDC_PEPORT));
   _connector     = static_cast<CComboBox *>(GetDlgItem(IDC_PECONNECTOR));
   _initValue     = static_cast<CComboBox *>(GetDlgItem(IDC_PEINIT));
   _portValue     = static_cast<CComboBox *>(GetDlgItem(IDC_PEVALUE));

   _condition [0] = static_cast<CComboBox *>(GetDlgItem(IDC_PECOND_1));
   _condition [1] = static_cast<CComboBox *>(GetDlgItem(IDC_PECOND_2));
   _variable  [0] = static_cast<CComboBox *>(GetDlgItem(IDC_PEVAR_1));
   _variable  [1] = static_cast<CComboBox *>(GetDlgItem(IDC_PEVAR_2));
   _threshold [0] = static_cast<CEdit     *>(GetDlgItem(IDC_PETHRESH_1));
   _threshold [1] = static_cast<CEdit     *>(GetDlgItem(IDC_PETHRESH_2));
   _hysteresis[0] = static_cast<CEdit     *>(GetDlgItem(IDC_PEHYST_1));
   _hysteresis[1] = static_cast<CEdit     *>(GetDlgItem(IDC_PEHYST_2));

   //---------------------------------------------------------------------------
   //--  Set values on the simple combo boxes.  --------------------------------

   _portName->AddString("PM2 - FIdle"); // ??? move to ini...
   _portName->AddString("PM3 - Injection LED");
   _portName->AddString("PM4 - Accel LED");
   _portName->AddString("PM5 - Warmup LED");
   _portName->AddString("PT6 - IAC1");
   _portName->AddString("PT7 - IAC2");
   _portName->AddString("PA0 - Knock Enable");
   _nPorts = _portName->GetCount();

   int ii;
   for (ii = 0; ii < nConnectors; ii++) {
      _connector->AddString(connLabel[ii]);
   }

   for (ii = 0; ii < nOperators; ii++) {
      _condition[0]->AddString(opLabels[ii]);
      _condition[1]->AddString(opLabels[ii]);
   }

   _initValue->AddString("0");
   _initValue->AddString("1");
   _portValue->AddString("0");
   _portValue->AddString("1");

   //---------------------------------------------------------------------------
   //--  Set the text fields to some default.  ---------------------------------

   _threshold [0]->SetWindowText("0.0");
   _threshold [1]->SetWindowText("0.0");
   _hysteresis[0]->SetWindowText("0.0");
   _hysteresis[1]->SetWindowText("0.0");

   //---------------------------------------------------------------------------
   //--  Set up the variable combo boxes.  -------------------------------------

   verifySettings();

   symMap &st   = mdb.cDesc.symbolTab();
   CDC   *pDC   = _variable[0]->GetDC();
   int    width = 0;
   symMapIter iSym;
   for (iSym = st.begin(); iSym != st.end(); iSym++) {
      symbol *s = (*iSym).second;
      if (!s->isConst() && !s->isExpr()) {
         const CString &str = s->name();
         CSize size = pDC->GetTextExtent(str);
         if (size.cx > width) width = size.cx;
      }
   }
   _variable[0]->ReleaseDC(pDC);

   _variable[0]->SetDroppedWidth(width);
   _variable[1]->SetDroppedWidth(width);
   for (iSym = st.begin(); iSym != st.end(); iSym++) {
      symbol *s = (*iSym).second;
      if (!s->isConst() && !s->isExpr()) {
         static int ii = 0;
         const CString &str = s->name();
         _variable[0]->InsertString  (ii, str);
         _variable[0]->SetItemDataPtr(ii, s);
         _variable[1]->InsertString  (ii, str);
         _variable[1]->SetItemDataPtr(ii, s);
      }
   }

   //---------------------------------------------------------------------------

   _portName->SetCurSel(0); // Should call setPort ();
   setPort();

   return TRUE;
}

//------------------------------------------------------------------------------

void portEdit::OnUp()
{
   int savePage = mdb.setPageNo(_pageNo);
      mdb.getConst();
      setPort();
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------

void portEdit::OnDown()
{
   int savePage = mdb.setPageNo(_pageNo);
      mdb.cDesc.send(psEnabled);
      mdb.cDesc.send(psConnector);
      mdb.burnConst(_pageNo);
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------

void portEdit::verifySettings()
{
   int val, sel;
   bool changed = false;
   for (int iPort = 0; iPort < _nPorts; iPort++) {
      sel = -1;
      val = psConnector->valueRaw(iPort);
      for (int iItem = 0; iItem < nConnectors; iItem++) {
         if (val == connectors[iItem]) sel = iItem;
      }
      if (sel < 0) {
         psConnector->storeRaw(connectors[0], iPort);
         changed = true;
      }

      for (int col = 0; col < 2; col++) {
         val = psCondition->valueRaw(psCondition->index(iPort, col));
         for (int iItem = 0; iItem < nConnectors; iItem++) {
            if (val == operators[iItem]) sel = iItem;
         }
         if (sel < 0) {
            psCondition->storeRaw(operators[0], psCondition->index(iPort, col));
            changed = true;
         }

         val = psOutOffset->valueRaw(psOutOffset->index(iPort, col));
         if (val == 0) {
            psOutOffset->storeRaw(1, psOutOffset->index(iPort, col)); // May be wrong, but it's valid.
            changed = true;
         }
      }
   }

   if (changed) {
      int savePage = mdb.setPageNo(_pageNo);
         mdb.cDesc.send(psConnector);
         mdb.cDesc.send(psCondition);
         mdb.cDesc.send(psOutOffset);
      mdb.setPageNo(savePage);
   }
}

//------------------------------------------------------------------------------

void portEdit::setPort()
{
   int row  = _portName->GetCurSel();

   int val1 = psCondition->valueRaw(psCondition->index(row, 0));
   int val2 = psCondition->valueRaw(psCondition->index(row, 1));
   int sel1 = 0;
   int sel2 = 0;
   int iItem;
   for (iItem = 0; iItem < nOperators; iItem++) {
      if (val1 == operators[iItem]) sel1 = iItem;
      if (val2 == operators[iItem]) sel2 = iItem;
   }
   _condition[0]->SetCurSel(sel1);
   _condition[1]->SetCurSel(sel2);

   _initValue->SetCurSel(psInitValue->valueRaw(row));
   _portValue->SetCurSel(psPortValue->valueRaw(row));

   val1 = psConnector->valueRaw(row);
   sel1 = 0;
   for (iItem = 0; iItem < nConnectors; iItem++) {
      if (val1 == connectors[iItem]) sel1 = iItem;
   }
   _connector->SetCurSel(sel1);

   val1 = psOutOffset->valueRaw(psOutOffset->index(row, 0));
   val2 = psOutOffset->valueRaw(psOutOffset->index(row, 1));
   for (iItem = 0; iItem < _variable[0]->GetCount(); iItem++) {
      symbol *s = reinterpret_cast<symbol *>(_variable[0]->GetItemDataPtr(iItem));
      if (val1 == s->offset()) {
         sel1 = iItem;
         _threshold [0]->SetWindowText(psThreshold ->valueUsing(psThreshold ->index(row, 0), s));
         _hysteresis[0]->SetWindowText(psHysteresis->valueUsing(psHysteresis->index(row, 0), s, true));
      }
      if (val2 == s->offset()) {
         sel2 = iItem;
         _threshold [1]->SetWindowText(psThreshold ->valueUsing(psThreshold ->index(row, 1), s));
         _hysteresis[1]->SetWindowText(psHysteresis->valueUsing(psHysteresis->index(row, 1), s, true));
      }
   }
   _variable[0]->SetCurSel(sel1);
   _variable[1]->SetCurSel(sel2);

   _enabled->SetCheck(psEnabled->valueRaw(row)); // Just use the int as bool.
   enableChanged();
}

//------------------------------------------------------------------------------

void portEdit::enableChanged()
{
   int  row     = _portName->GetCurSel();
   int  enabled = _enabled->GetCheck();

   int savePage = mdb.setPageNo(_pageNo);
      psEnabled->storeRaw(enabled, row);
      mdb.cDesc.send(psEnabled);
   mdb.setPageNo(savePage);

   _variable  [0]->EnableWindow(enabled);
   _condition [0]->EnableWindow(enabled);
   _threshold [0]->EnableWindow(enabled);
   _hysteresis[0]->EnableWindow(enabled);

   _connector    ->EnableWindow(enabled);

   _variable  [1]->EnableWindow(enabled);
   _condition [1]->EnableWindow(enabled);
   _threshold [1]->EnableWindow(enabled);
   _hysteresis[1]->EnableWindow(enabled);

   _initValue    ->EnableWindow(enabled);
   _portValue    ->EnableWindow(enabled);

   if (enabled) connectorChanged();
}

//------------------------------------------------------------------------------

void portEdit::connectorChanged()
{
   int  row     = _portName->GetCurSel();
   int  guiVal  = _connector->GetCurSel();
   bool enabled = guiVal != 0;

   int savePage = mdb.setPageNo(_pageNo);
      psConnector->storeRaw(connectors[guiVal], row);
      mdb.cDesc.send(psConnector);
   mdb.setPageNo(savePage);

   _variable  [1]->EnableWindow(enabled);
   _condition [1]->EnableWindow(enabled);
   _threshold [1]->EnableWindow(enabled);
   _hysteresis[1]->EnableWindow(enabled);
}

//------------------------------------------------------------------------------

void portEdit::operatorChanged(UINT nId)
{
   int row    = _portName->GetCurSel();
   int col    = nId - IDC_PECOND_1;
   int guiVal = _condition[col]->GetCurSel();

   int savePage = mdb.setPageNo(_pageNo);
      psCondition->storeRaw(operators[guiVal], psCondition->index(row, col));
      mdb.cDesc.send(psCondition);
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------

void portEdit::variableChanged(UINT nId)
{
   int     row    = _portName->GetCurSel();
   int     col    = nId - IDC_PEVAR_1;
   int     guiVal = _variable[col]->GetCurSel();
   symbol *s      = reinterpret_cast<symbol *>(_variable[col]->GetItemDataPtr(guiVal));

   int savePage = mdb.setPageNo(_pageNo);
      psOutOffset->storeRaw(s->offset(), psOutOffset->index(row, col));
      psOutSize  ->storeRaw(s->sizeOf(), psOutSize  ->index(row, col));
      mdb.cDesc.send(psOutOffset);
      mdb.cDesc.send(psOutSize);
   mdb.setPageNo(savePage);

   valueChanged(); // Sync up the units of the variable with the displayed values.
}

//------------------------------------------------------------------------------

void portEdit::valueChanged()
{
   int row      = _portName->GetCurSel();
   int savePage = mdb.setPageNo(_pageNo);
      for (int col = 0; col < 2; col++) {
         int     var = _variable[col]->GetCurSel();
         symbol *s   = reinterpret_cast<symbol *>(_variable[col]->GetItemDataPtr(var));
         double  x   = getRaw(_threshold [col], symbol::noRange, symbol::noRange, "");
         psThreshold->storeUsing(x, psThreshold->index(row, col), s);
                 x   = getRaw(_hysteresis[col], symbol::noRange, symbol::noRange, "");
         psHysteresis->storeUsing(x, psHysteresis->index(row, col), s, true);
      }
      mdb.cDesc.send(psThreshold);
      mdb.cDesc.send(psHysteresis);
   mdb.setPageNo(savePage);
}

void portEdit::outputChanged()
{
   int row      = _portName->GetCurSel();
   int savePage = mdb.setPageNo(_pageNo);
      psInitValue->storeRaw(_initValue->GetCurSel(), row);
      psPortValue->storeRaw(_portValue->GetCurSel(), row);
      mdb.cDesc.send(psInitValue);
      mdb.cDesc.send(psPortValue);
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------

BOOL portEdit::OnHelpInfo(HELPINFO* pHelpInfo) 
{
   const char *helpUrl = "http://www.megasquirt.info/ms2/spare.htm";
   ShellExecute(NULL, "open", helpUrl, NULL, NULL, SW_SHOWNORMAL); 
   return true;
}

//------------------------------------------------------------------------------
