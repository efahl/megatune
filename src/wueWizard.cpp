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
#include "msDatabase.h"
#include "repository.h"
#include "userDefined.h"
#include "wueWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern msDatabase mdb;
extern repository rep;

//------------------------------------------------------------------------------

#pragma warning(disable:4355)

wueWizard::wueWizard(int pageNo, CWnd *pParent)
 : dataDialog(wueWizard::IDD, pParent)
 , _pageNo   (pageNo)
 , _fld      (this)
{
   //{{AFX_DATA_INIT(wueWizard)
   //}}AFX_DATA_INIT
   m_val[0] = 0;
   m_val[1] = 0;
   m_val[2] = 0;
   m_val[3] = 0;
   m_val[4] = 0;
   m_val[5] = 0;
   m_val[6] = 0;
   m_val[7] = 0;
   m_val[8] = 0;
   m_val[9] = 0;

   int savePage = mdb.setPageNo(_pageNo);
      wwuLo[0] = mdb.wwuX[0]-60; wwuHi[0] = mdb.wwuX[0]+15; // -40 15
      wwuLo[1] = mdb.wwuX[1]-15; wwuHi[1] = mdb.wwuX[1]+15; // -20 15
      wwuLo[2] = mdb.wwuX[2]-15; wwuHi[2] = mdb.wwuX[2]+15; //   0 15
      wwuLo[3] = mdb.wwuX[3]-15; wwuHi[3] = mdb.wwuX[3]+15; //  20 15
      wwuLo[4] = mdb.wwuX[4]-15; wwuHi[4] = mdb.wwuX[4]+15; //  40 15
      wwuLo[5] = mdb.wwuX[5]-15; wwuHi[5] = mdb.wwuX[5]+15; //  60 15
      wwuLo[6] = mdb.wwuX[6]-15; wwuHi[6] = mdb.wwuX[6]+16; //  80 15
      wwuLo[7] = mdb.wwuX[7]-15; wwuHi[7] = mdb.wwuX[7]+20; // 100 20
      wwuLo[8] = mdb.wwuX[8]-20; wwuHi[8] = mdb.wwuX[8]+20; // 130 20
      wwuLo[9] = mdb.wwuX[9]-20; wwuHi[9] = mdb.wwuX[9]+ 0; // 160 20
   mdb.setPageNo(savePage);
}

wueWizard::~wueWizard()
{
   for (int i = 0; i < nBars; i++) delete bd[i];
}

//------------------------------------------------------------------------------

void wueWizard::DoDataExchange(CDataExchange *pDX)
{
   dataDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_ARROW0, m_arrow);

   DDX_Text   (pDX, IDC_WUEBIN0,   m_val  [0]);
   DDX_Text   (pDX, IDC_WUEBIN1,   m_val  [1]);
   DDX_Text   (pDX, IDC_WUEBIN2,   m_val  [2]);
   DDX_Text   (pDX, IDC_WUEBIN3,   m_val  [3]);
   DDX_Text   (pDX, IDC_WUEBIN4,   m_val  [4]);
   DDX_Text   (pDX, IDC_WUEBIN5,   m_val  [5]);
   DDX_Text   (pDX, IDC_WUEBIN6,   m_val  [6]);
   DDX_Text   (pDX, IDC_WUEBIN7,   m_val  [7]);
   DDX_Text   (pDX, IDC_WUEBIN8,   m_val  [8]);
   DDX_Text   (pDX, IDC_WUEBIN9,   m_val  [9]);

   DDX_Control(pDX, IDC_WUEBIN0,   m_bin  [0]);
   DDX_Control(pDX, IDC_WUEBIN1,   m_bin  [1]);
   DDX_Control(pDX, IDC_WUEBIN2,   m_bin  [2]);
   DDX_Control(pDX, IDC_WUEBIN3,   m_bin  [3]);
   DDX_Control(pDX, IDC_WUEBIN4,   m_bin  [4]);
   DDX_Control(pDX, IDC_WUEBIN5,   m_bin  [5]);
   DDX_Control(pDX, IDC_WUEBIN6,   m_bin  [6]);
   DDX_Control(pDX, IDC_WUEBIN7,   m_bin  [7]);
   DDX_Control(pDX, IDC_WUEBIN8,   m_bin  [8]);
   DDX_Control(pDX, IDC_WUEBIN9,   m_bin  [9]);

   DDX_Control(pDX, IDC_WUEBIN0_L, m_wwuL [0]);
   DDX_Control(pDX, IDC_WUEBIN1_L, m_wwuL [1]);
   DDX_Control(pDX, IDC_WUEBIN2_L, m_wwuL [2]);
   DDX_Control(pDX, IDC_WUEBIN3_L, m_wwuL [3]);
   DDX_Control(pDX, IDC_WUEBIN4_L, m_wwuL [4]);
   DDX_Control(pDX, IDC_WUEBIN5_L, m_wwuL [5]);
   DDX_Control(pDX, IDC_WUEBIN6_L, m_wwuL [6]);
   DDX_Control(pDX, IDC_WUEBIN7_L, m_wwuL [7]);
   DDX_Control(pDX, IDC_WUEBIN8_L, m_wwuL [8]);
   DDX_Control(pDX, IDC_WUEBIN9_L, m_wwuL [9]);

   DDX_Control(pDX, IDC_WARMCOR_BAR,         m_warmCorBar);
   DDX_Control(pDX, IDC_MAP_BAR,             m_mapBar);
   DDX_Control(pDX, IDC_CLT_BAR,             m_cltBar);
   DDX_Control(pDX, IDC_EGO_BAR,             m_egoBar);

   //{{AFX_DATA_MAP(wueWizard)
      DDX_Control(pDX, IDC_PW1_LABEL,        m_pw1Label);
      DDX_Control(pDX, IDC_PW2_LABEL,        m_pw2Label);
      DDX_Control(pDX, IDC_FLOODCLEAR_LABEL, m_floodclearLabel);
      DDX_Control(pDX, IDC_COOLANT_LABEL,    m_coolantLabel);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(wueWizard, dataDialog)
   //{{AFX_MSG_MAP(wueWizard)
      ON_BN_CLICKED(IDC_UP,           OnUp)
      ON_BN_CLICKED(IDC_DOWN,         OnDown)
      ON_BN_CLICKED(IDC_INCREMENT,    DoIncrement)
      ON_BN_CLICKED(IDC_DECREMENT,    DoDecrement)
      ON_WM_CTLCOLOR()
      ON_EN_KILLFOCUS(IDC_PRIMEP,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_CWU,        dbFromFields)
      ON_EN_KILLFOCUS(IDC_CWH ,       dbFromFields)
      ON_EN_KILLFOCUS(IDC_FLOODCLEAR, dbFromFields)
      ON_EN_KILLFOCUS(IDC_AWEV,       dbFromFields)
      ON_EN_KILLFOCUS(IDC_AWC ,       dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN0,    dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN1,    dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN2,    dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN3,    dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN4,    dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN5,    dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN6,    dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN7,    dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN8,    dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN9,    dbFromFields)

      ON_WM_KEYDOWN()
      ON_WM_PAINT()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

void logSize(CWnd *w, char *title);

BOOL wueWizard::OnInitDialog() 
{
   dataDialog::OnInitDialog("Warmup Wizard", _pageNo+1);
   
   logSize(this, "wue_wizard");

   coolant = mdb.cDesc.lookup(S_coolant);

   _fld.add(S_primePulse, IDC_PRIMEP,         _pageNo);
   _fld.add(S_crankCold,  IDC_CWU,            _pageNo);
   _fld.add(S_crankHot,   IDC_CWH,            _pageNo);
   _fld.add(S_floodClear, IDC_FLOODCLEAR,     _pageNo);
   _fld.add(S_asePct,     IDC_AWEV,           _pageNo);
   _fld.add(S_aseCount,   IDC_AWC,            _pageNo);

   for (int iWE = 0; iWE < 10; iWE++) _fld.add(S_wueBins, IDC_WUEBIN0+iWE, _pageNo, iWE);

   bd[0] = new barData(S_warmupEnrich, this, IDC_WARMCOR, IDC_WARMCOR_BAR, rep.lorWC,  rep.hirWC,  0);
   bd[1] = new barData(S_map,          this, IDC_MAP,     IDC_MAP_BAR,     rep.lorMAP, rep.hirMAP, 0);
   bd[2] = new barData(S_coolant,      this, IDC_CLT,     IDC_CLT_BAR,     rep.lorCT,  rep.hirCT,  0);
   bd[3] = new barData(S_egoVoltage,   this, IDC_EGO,     IDC_EGO_BAR,     rep.lorEGO, rep.hirEGO, 3);

   symbol *s = mdb.lookupByPage(S_floodClear, _pageNo);
   if (s) {
      const CString &u = s->units();
      m_floodclearLabel.SetWindowText(("Flood Clear Threshold ("+u+")"));
   }

   int savePage = mdb.setPageNo(_pageNo);
      symbol *tempTable = mdb.cDesc.lookup(S_tempTable);

      if (tempTable) {
         m_pw1Label.SetWindowText(CString("Pulsewidth at ")+tempTable->valueString(      0)+tempTable->units());
         m_pw2Label.SetWindowText(CString("Pulsewidth at ")+tempTable->valueString(nBins-1)+tempTable->units());
      }
      else {
         mdb.fixThermoLabel(m_pw1Label, "Pulsewidth at % 3.0f° F", -40.0);
         mdb.fixThermoLabel(m_pw2Label, "Pulsewidth at % 3.0f° F", 170.0);
      }

      mdb.fixThermoLabel(m_coolantLabel);

      for (int i = 0; i < nBins; i++) {
         if (tempTable)
            m_wwuL[i].SetWindowText(tempTable->valueString(i) + tempTable->units());
         else
            mdb.fixThermoLabel(m_wwuL[i], "% 3.0f° ", mdb.wwuX[i]);
      }
   mdb.setPageNo(savePage);

   fieldsFromDb();

   return TRUE;
}

//------------------------------------------------------------------------------

void wueWizard::OnUp() 
{
   fieldsFromDb();
}

void wueWizard::OnDown() 
{
   int savePage = mdb.setPageNo(_pageNo);
      dbFromFields();
      _fld.send();
      mdb.burnConst(_pageNo);
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------

void wueWizard::fieldsFromDb()
{
   int savePage = mdb.setPageNo(_pageNo);
      mdb.getConst(); // Make sure we've loaded this page.
      _fld.setFld();
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------

void wueWizard::dbFromFields()
{
   int savePage = mdb.setPageNo(_pageNo);
      _fld.getFld();

      if (_fld.value(S_wueBins, 9) != 100) {
         m_bin[9].SetWindowText("100");
         m_bin[9].SetFocus();
//       if (mdb.loaded()) MessageBox("Warning: Warmup Enrichment value at 160 degrees\nshould be set to 100%.");
      }
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------

void wueWizard::moveArrow(int i)
{
   if (i < 0 || i >= nBins-1) return;

   double t  = coolant->valueUser();
   double lo = mdb.tempFromDb(mdb.wwuX[i]);
   double hi = mdb.tempFromDb(mdb.wwuX[i+1]);
   if (t > hi)
      m_arrow.ShowWindow(false);
   else if (t >= lo && t <= hi) {
      CRect rAbove, rBelow, rArrow;
      m_bin[i  ].GetWindowRect(&rAbove); ScreenToClient(&rAbove);
      m_bin[i+1].GetWindowRect(&rBelow); ScreenToClient(&rBelow);
      m_arrow   .GetWindowRect(&rArrow); ScreenToClient(&rArrow);

      int hgt       = rArrow.Height();
      rArrow.top    = rAbove.top + (rBelow.top-rAbove.top) * (t-lo)/(hi-lo) + 2; // 2 == field border fudge
      rArrow.bottom = rArrow.top+hgt;
      m_arrow.ShowWindow(true);
      m_arrow.MoveWindow(rArrow);
   }
}

bool wueWizard::doTimer(bool connected)
{
   if (!realized()) return false;

   if (connected) {
      for (int i = 0; i < nBars; i++) bd[i]->show();

      static int oldT = -100;
      if (int(coolant->valueUser()) != oldT) {
         for (int i = 0; i < nBins; i++) {
            moveArrow(i);
            m_bin[i].Invalidate();
         }
         oldT = int(coolant->valueUser());
      }
   }
   else {
      for (int i = 0; i < nBars; i++) bd[i]->disconnected();

      CRect rBin, rArrow;
      m_bin[9].GetWindowRect(&rBin  ); ScreenToClient(&rBin  );
      m_arrow .GetWindowRect(&rArrow); ScreenToClient(&rArrow);
      rArrow.top    += 10;
      rArrow.bottom += 10;
      if (rArrow.top > rBin.top) {
         int hgt = rArrow.Height();
         m_bin[0].GetWindowRect(&rBin); ScreenToClient(&rBin);
         rArrow.top    = rBin.top;
         rArrow.bottom = rArrow.top+hgt;
      }
      m_arrow.ShowWindow(true);
      m_arrow.MoveWindow(rArrow);
   }

   return true;
}

//------------------------------------------------------------------------------

void wueWizard::bump(int &val, double lo, double hi, int inc)
{
   int ilo = int(mdb.tempFromDb(lo)+0.5);
   int ihi = int(mdb.tempFromDb(hi)+0.5);
   double t = coolant->valueUser();
   if (t >= ilo && t <= ihi) val += inc;
}

void wueWizard::DoIncrement() 
{
   UpdateData(TRUE);
      for (int i = 0; i < nBins; i++) {
         bump(m_val[i], wwuLo[i], wwuHi[i], 1);
      }
   UpdateData(FALSE);
   OnDown();
}

void wueWizard::DoDecrement() 
{
   UpdateData(TRUE);
      for (int i = 0; i < nBins; i++) {
         bump(m_val[i], wwuLo[i], wwuHi[i], -1);
      }
   UpdateData(FALSE);
   OnDown();
}

//------------------------------------------------------------------------------

BOOL wueWizard::PreTranslateMessage(MSG *pMsg) 
{
   static bool shifted = false;

   if (pMsg->message == WM_KEYUP) {
      switch (pMsg->wParam) {
         case VK_SHIFT  : shifted = false; return TRUE;
      }
   }

   if (pMsg->message  == WM_KEYDOWN) {
      switch (pMsg->wParam) {
         case VK_SHIFT  : shifted = true;  return TRUE;
         case VK_UP:
            if (shifted) {
               DoIncrement();
               return TRUE;
            }
            break;

         case VK_DOWN:
            if (shifted) {
               DoDecrement();
               return TRUE;
            }
            break;
      }
   }

   return dataDialog::PreTranslateMessage(pMsg);
}

//------------------------------------------------------------------------------

double wueWizard::colorTemperature(int idx, double temperature)
{
   double ct = 0.0;

   if (temperature < mdb.tempFromDb(mdb.wwuX[0])) {
       return idx == 0 ? 1.0 : 0.0;
   }
   if (temperature > mdb.tempFromDb(mdb.wwuX[nBins-1])) {
       return idx == nBins-1 ? 1.0 : 0.0;
   }

   int loIdx = 0; double loT = -100;
   int hiIdx = 0; double hiT = -100;
   for (int i = 1; i < nBins; i++) {
      if (temperature < mdb.tempFromDb(mdb.wwuX[i])) {
         loIdx = i-1; loT = mdb.tempFromDb(mdb.wwuX[i-1]);
         hiIdx = i  ; hiT = mdb.tempFromDb(mdb.wwuX[i  ]);
         break;
      }
   }

   if (idx == loIdx) ct = 1.0 - (temperature-loT) / (hiT-loT);
   if (idx == hiIdx) ct = 1.0 - (hiT-temperature) / (hiT-loT);

   return ct;
}

HBRUSH wueWizard::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor) 
{
   HBRUSH hbr = dataDialog::OnCtlColor(pDC, pWnd, nCtlColor);
   
   if (rep.fieldColoring()) {
      int iFld = pWnd->GetDlgCtrlID()-IDC_WUEBIN0;
      if (iFld >= 0 && iFld < nBins) {
         double ct = colorTemperature(iFld, coolant->valueUser());
         if (ct > 0.0) {
            pDC->SetBkMode(TRANSPARENT);
            COLORREF color = RGB(255, 225-int(ct*100), 225-int(ct*100));
            static CBrush brush;
            brush.DeleteObject();
            brush.CreateSolidBrush(color);
            hbr = brush;
         }
      }
   }

   return hbr;
}

//------------------------------------------------------------------------------
