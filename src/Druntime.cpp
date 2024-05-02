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
#include "repository.h"
#include "Druntime.h"

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

extern msDatabase mdb;
extern repository rep;

//------------------------------------------------------------------------------

hysteresisBar::hysteresisBar()
 : CProgressCtrl()
 , m_decayInterval(rep.barHysteresis())
 , m_currentTime  (0.0)
 , m_startTime    (timeNow())
 , m_highPercent  (0.0)
{
// TRACE0("Hystbar ctor\n");
}

BEGIN_MESSAGE_MAP(hysteresisBar, CProgressCtrl)
   //{{AFX_MSG_MAP(hysteresisBar)
      ON_WM_PAINT()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

int hysteresisBar::SetPos(int nPos)
{
   int oldPos = CProgressCtrl::SetPos(nPos);
   if (m_highPercent > 0) {
      m_currentTime = timeNow() - m_startTime;
      if (m_currentTime > m_decayInterval) {
         m_highPercent = 0.0;
         Invalidate();
      }
   }
   return oldPos;
}

void hysteresisBar::OnPaint() 
{
// TRACE0("Hystbar paint\n");
   COLORREF hiColor = ::GetSysColor(COLOR_HIGHLIGHT);
   COLORREF hyColor = RGB(255,0,0);
   COLORREF loColor = ::GetSysColor(COLOR_SCROLLBAR);

   CDC *dc = GetDC();
   if (dc == NULL) return;
   dc->SelectObject(GetParent()->GetFont());

   //--  Current position and hysteresis  --------------------------------------

   int lo, hi;
   GetRange(lo, hi);
   double percentage = GetPos() / double(hi-lo);

   if (percentage > m_highPercent) {
      m_highPercent = percentage;
      m_startTime   = timeNow();
   }

   double hyPercentage = m_highPercent - percentage;

   //--  Bar  ------------------------------------------------------------------
   CRect rect;
   GetClientRect(rect);
   CRect loBar = rect;
   CRect hyBar = rect;
   CRect hiBar = rect;

   if (GetStyle() & PBS_VERTICAL) {
      int range = rect.top - rect.bottom;
      hiBar.top    = hiBar.bottom + int(range*percentage+0.5);
      hyBar.bottom = hiBar.top;
      hyBar.top    = hyBar.bottom + int(range*hyPercentage+0.5);
      loBar.bottom = hyBar.top;
   }
   else {
      int range = rect.right - rect.left;
      hiBar.right = hiBar.left + int(range*percentage+0.5);
      hyBar.left  = hiBar.right;
      hyBar.right = hyBar.left + int(range*hyPercentage+0.5);
      loBar.left  = hyBar.right;
   }

   dc->FillSolidRect(hiBar, hiColor);
   if (hyPercentage > 0.0) dc->FillSolidRect(hyBar, hyColor);
   dc->FillSolidRect(loBar, loColor);
   ReleaseDC(dc);

   ValidateRect(rect);
}

//------------------------------------------------------------------------------

barData::barData(const char *symName, CWnd* db, UINT boxId, UINT barId, double lo, double hi, int digits)
 : lo    (lo)
 , hi    (hi)
 , digits(digits)
{
   sym = mdb.cDesc.lookup(symName);
   box = static_cast<CEdit         *>(db->GetDlgItem(boxId)); 
   bar = static_cast<hysteresisBar *>(db->GetDlgItem(barId));
}

void barData::show()
{
   if (sym == NULL) {
      box->SetWindowText("No Var!");
      return;
   }

   double fv = sym->valueUser();
   int    bv = int(fv);
   if (lo != hi) {
      bv = int(100 * (fv-lo) / (hi-lo));
   }

   if (box) {
      CString s;
      s.Format("%.*f", digits, fv);
      box->SetWindowText(s);
   }
   if (bar) {
      bar->SetPos(bv);
   }
}

void barData::disconnected()
{
   if (bar) bar->SetPos(0);
// if (box) box->SetWindowText("---"); // The last value is actually useful.
}

//------------------------------------------------------------------------------

Druntime::Druntime(CWnd* pParent /*=NULL*/)
 : dataDialog(Druntime::IDD, pParent)
 , redBrush   (RGB(255,0,0))
 , yellowBrush(RGB(255,255,0))
 , blueBrush  (RGB(0,0,255))
 , whiteBrush (RGB(255,255,255))
{
   //{{AFX_DATA_INIT(Druntime)
   //}}AFX_DATA_INIT
   isConnected = false;
   for (int i = 0; i < nWidgets; i++) {
      bd[i] = NULL;
   }
}

Druntime::~Druntime()
{
   for (int i = 0; i < nWidgets; i++) {
      if (bd[i]) delete bd[i];
   }
}

//------------------------------------------------------------------------------

void Druntime::DoDataExchange(CDataExchange* pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(Druntime)
      DDX_Control(pDX, IDC_ACCELCOR_BAR,  m_accelCorBar);
      DDX_Control(pDX, IDC_AIRCOR_BAR,    m_airCorBar);
      DDX_Control(pDX, IDC_BAROCOR_BAR,   m_baroCorBar);
      DDX_Control(pDX, IDC_BATT_BAR,      m_battBar);
      DDX_Control(pDX, IDC_CLT_BAR,       m_cltBar);
      DDX_Control(pDX, IDC_DUTYCYCLE_BAR, m_dutycycleBar);
      DDX_Control(pDX, IDC_GAMMA_BAR,     m_gammaBar);
      DDX_Control(pDX, IDC_EGOCOR_BAR,    m_egoCorBar);
      DDX_Control(pDX, IDC_EGO_BAR,       m_egoBar);
      DDX_Control(pDX, IDC_MAP_BAR,       m_mapBar);
      DDX_Control(pDX, IDC_MAT_BAR,       m_matBar);
      DDX_Control(pDX, IDC_PW_BAR,        m_pwBar);
      DDX_Control(pDX, IDC_RPM_BAR,       m_rpmBar);
      DDX_Control(pDX, IDC_TPS_BAR,       m_tpsBar);
      DDX_Control(pDX, IDC_VECOR_BAR,     m_veCorBar);
      DDX_Control(pDX, IDC_WARMCOR_BAR,   m_warmCorBar);

      DDX_Control(pDX, IDC_MAT_LABEL,     m_matLabel);
      DDX_Control(pDX, IDC_COOLANT_LABEL, m_coolantLabel);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(Druntime, dataDialog)
   //{{AFX_MSG_MAP(Druntime)
      ON_WM_CTLCOLOR()
      ON_WM_PAINT   ()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

//BEGIN_EVENTSINK_MAP(Druntime, dataDialog)
//   //{{AFX_EVENTSINK_MAP(Druntime)
//   //}}AFX_EVENTSINK_MAP
//END_EVENTSINK_MAP()

//------------------------------------------------------------------------------

void logSize(CWnd *w, char *title);

BOOL Druntime::OnInitDialog()
{
   dataDialog::OnInitDialog("Output Channel Display");

   logSize(this, "runtime");

   sEngine = mdb.cDesc.lookup(S_engine);

#define getBar(s,ced,cbr,l,h,d) bd[idx++] = new barData(s, this, ced, cbr, l, h, d)

   int idx = 0;
   getBar(S_barometer,      IDC_BARO,      0,                 0.0,        0.0,        0);
   getBar(S_secl,           IDC_SECONDS,   0,                 0.0,        0.0,        0);
   getBar(S_egoVoltage,     IDC_EGO,       IDC_EGO_BAR,       rep.lorEGO, rep.hirEGO, 3);
   getBar(S_map,            IDC_MAP,       IDC_MAP_BAR,       rep.lorMAP, rep.hirMAP, 0);
   getBar(S_mat,            IDC_MAT,       IDC_MAT_BAR,       rep.lorMAT, rep.hirMAT, 0);
   getBar(S_coolant,        IDC_CLT,       IDC_CLT_BAR,       rep.lorCT,  rep.hirCT,  0);
   getBar(S_throttle,       IDC_TPS,       IDC_TPS_BAR,       rep.lorTR,  rep.hirTR,  1);
   getBar(S_batteryVoltage, IDC_BATT,      IDC_BATT_BAR,      rep.lorBAT, rep.hirBAT, 2);
   getBar(S_rpm,            IDC_RPM,       IDC_RPM_BAR,       rep.lorRPM, rep.hirRPM, 0);
   getBar(S_pulseWidth,     IDC_PW,        IDC_PW_BAR,        rep.lorPW,  rep.hirPW,  1);
   getBar(S_egoCorrection,  IDC_EGOCOR,    IDC_EGOCOR_BAR,    rep.lorEGC, rep.hirEGC, 0);
   getBar(S_airCorrection,  IDC_AIRCOR,    IDC_AIRCOR_BAR,    rep.lorADC, rep.hirADC, 0);
   getBar(S_warmupEnrich,   IDC_WARMCOR,   IDC_WARMCOR_BAR,   rep.lorWC,  rep.hirWC,  0);
   getBar(S_accelEnrich,    IDC_ACCELCOR,  IDC_ACCELCOR_BAR,  rep.lorACC, rep.hirACC, 0);
   getBar(S_baroCorrection, IDC_BAROCOR,   IDC_BAROCOR_BAR,   rep.lorBC,  rep.hirBC,  0);
   getBar(S_veCurr,         IDC_VECOR,     IDC_VECOR_BAR,     rep.lorVE,  rep.hirVE,  0);
   getBar(S_gammaEnrich,    IDC_GAMMAECOR, IDC_GAMMA_BAR,     rep.lorGE,  rep.hirGE,  0);
   getBar(S_idleDC,         IDC_DUTYCYCLE, IDC_DUTYCYCLE_BAR, rep.lorDC,  rep.hirDC,  1);
   assert(idx == nWidgets);

   connected = static_cast<CEdit *>(GetDlgItem(IDC_CONNECTED_TXT));

   if (!mdb.loaded()) mdb.load();

   mdb.fixThermoLabel(m_coolantLabel);
   mdb.fixThermoLabel(m_matLabel);

   return TRUE;
}

//------------------------------------------------------------------------------

void Druntime::updateDisplay()
{
   double ft = bd[0]->sym->valueUser();
   CString s;
   s.Format("%.0f kPa  %.2f in-Hg", ft, ft*0.2953007);
   bd[0]->box->SetWindowText(s);

   for (int i = 1; i < nWidgets; i++) {
      bd[i]->show();
   }

   /* Output runtime message */
   unsigned bits = sEngine ? unsigned(sEngine->valueUser()) : 0;
   static_cast<CEdit *>(GetDlgItem(IDC_RUNNING_TXT   ))->EnableWindow((bits &  1) != 0);
   static_cast<CEdit *>(GetDlgItem(IDC_CRANKING_TXT  ))->EnableWindow((bits &  2) != 0);
   static_cast<CEdit *>(GetDlgItem(IDC_AFTERSTART_TXT))->EnableWindow((bits &  4) != 0);
   static_cast<CEdit *>(GetDlgItem(IDC_WARMUP_TXT    ))->EnableWindow((bits &  8) != 0);
   static_cast<CEdit *>(GetDlgItem(IDC_ACCEL_TXT     ))->EnableWindow((bits & 16) != 0);
   static_cast<CEdit *>(GetDlgItem(IDC_DECEL_TXT     ))->EnableWindow((bits & 32) != 0);
}

//------------------------------------------------------------------------------

HBRUSH Druntime::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
   // Call the base class implementation first! Otherwise, it may
   // undo what we're trying to accomplish here.
   HBRUSH hbr = dataDialog::OnCtlColor(pDC, pWnd, nCtlColor);
   
   unsigned it = sEngine ? unsigned(sEngine->valueUser()) : 0;

   if (pWnd->GetDlgCtrlID() == IDC_CONNECTED_TXT  && isConnected      ||
       pWnd->GetDlgCtrlID() == IDC_RUNNING_TXT    && ((it &  1) != 0)) {
      pDC->SetTextColor(RGB(0,0,0));
      pDC->SetBkMode(TRANSPARENT);
      hbr = whiteBrush;
   }

   if (pWnd->GetDlgCtrlID() == IDC_CRANKING_TXT   && ((it &  2) != 0) ||
       pWnd->GetDlgCtrlID() == IDC_AFTERSTART_TXT && ((it &  4) != 0) ||
       pWnd->GetDlgCtrlID() == IDC_WARMUP_TXT     && ((it &  8) != 0) ||
       pWnd->GetDlgCtrlID() == IDC_ACCEL_TXT      && ((it & 16) != 0)) {
      pDC->SetTextColor(RGB(0,0,0));
      pDC->SetBkMode(TRANSPARENT);
      hbr = redBrush;
   }

   if (pWnd->GetDlgCtrlID() == IDC_DECEL_TXT      && ((it & 32) != 0)){
      pDC->SetTextColor(RGB(255,255,255));
      pDC->SetBkMode(TRANSPARENT);
      hbr = blueBrush;
   }

   return hbr;
}

//------------------------------------------------------------------------------

bool Druntime::doTimer(bool con)
{
   if (!realized()) return false;

   isConnected = con;
   if (isConnected) {
      connected->EnableWindow(true);
      updateDisplay();
   }
   else {
      connected->EnableWindow(false);
      static_cast<CEdit *>(GetDlgItem(IDC_RUNNING_TXT))->EnableWindow(false);
      for (int idx = 0; idx < nWidgets; idx++) {
         bd[idx]->disconnected();
      }
   }
   return true;
}

//------------------------------------------------------------------------------

BOOL Druntime::DestroyWindow()
{
   return dataDialog::DestroyWindow();
}

//------------------------------------------------------------------------------

BOOL Druntime::PreTranslateMessage(MSG* pMsg) 
{
   if (pMsg->message == WM_KEYDOWN) {
      if (pMsg->wParam == VK_SPACE) {
         mdb.markLog();
         return TRUE;
      }
   }

   return dataDialog::PreTranslateMessage(pMsg);
}

//------------------------------------------------------------------------------
