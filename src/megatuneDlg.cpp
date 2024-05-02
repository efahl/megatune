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

#include <sys/stat.h>

#include "megatune.h"
#include "msDatabase.h"
#include "repository.h"
#include "megatuneDlg.h"
#include "dataDialog.h"
#include "userDefined.h"
#include "Dabout.h"
#include "Dconstants.h"
#include "Druntime.h"
#include "Denrichments.h"
#include "wueWizard.h"
#include "tableEditor.h"
#include "curveEditor.h"
#include "Dsetcomm.h"
#include "tuning3D.h"
#include "Dconplot.h"
#include "hexedit.h"
#include "tpgen.h"
#include "userMenu.h"
#include "userHelp.h"
#include "DlogOptions.h"
#include "dirSelect.h"
#include "selectList.h"
#include "burstLog.h"
#include "genTherm.h"
#include "genMAF.h"
#include "genO2.h"
#include "aeWizard.h"
#include "triggerWizard.h"
#include "injControl.h"
#include "portEdit.h"
#include "injectorSize.h"
#include "msqInfoDlg.h"
#include "miniTerm.h"
#include "Splash.h"

#ifdef _DEBUG
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------

class onlyOne {
   HANDLE singleton;

public:
   onlyOne(CWnd *pParent = NULL)
   {
      singleton = CreateMutex(NULL, TRUE, "Global/megatune_is_running");
      if (GetLastError() == ERROR_ALREADY_EXISTS) {
         CloseHandle(singleton);
         // Another copy is already up, so terminate.
         MessageBox(NULL, "Another copy of MegaTune is already running.\nTerminating now.", "MegaTune Terminating", MB_ICONSTOP);
         exit(1);
      }
   }
  ~onlyOne()
   {
      if (singleton) CloseHandle(singleton);
   }
};

//------------------------------------------------------------------------------

onlyOne           one;
datalogOptions    lop;
dirSelect         dir;
userMenuList      uml;
userPlugInList    upl;
userHelpList      uhl;
userIndicatorList uil;
repository        rep; // Must be in this order to force reading of ini file first.
msDatabase        mdb;

//------------------------------------------------------------------------------

CMegatuneDlg::CMegatuneDlg(CWnd *pParent)
 : CDialog         (CMegatuneDlg::IDD, pParent)
 , _timerDialog    (NULL)
 , _connected      (false)
 , _onlineMode     (true)
 , _invalidated    (true)
{
   //{{AFX_DATA_INIT(CMegatuneDlg)
   //}}AFX_DATA_INIT
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MEGATUNE);
   dataDialog::setGlobalIcon(m_hIcon);
}

//------------------------------------------------------------------------------

void CMegatuneDlg::DoDataExchange(CDataExchange *pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CMegatuneDlg)
      DDX_Control(pDX, IDC_STATUS_LOGGING,   m_statusLogging);
      DDX_Control(pDX, IDC_STATUS_FILENAME,  m_statusFilename);
      DDX_Control(pDX, IDC_STATUS_SAVED,     m_statusSaved);
      DDX_Control(pDX, IDC_STATUS_CONNECTED, m_statusConnected);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CMegatuneDlg, CDialog)
   //{{AFX_MSG_MAP(CMegatuneDlg)
      ON_COMMAND(ID_COMMLOG,                  OnCommlog)
      ON_COMMAND(ID_OFFLINE,                  OnOffline)
      ON_COMMAND(ID_DATALOG_OPT,              OnDatalogOpt)
      ON_COMMAND(ID_DATALOG,                  OnDatalog)
      ON_COMMAND(ID_DATALOG_X,                OnDatalogX)
      ON_COMMAND(ID_DATALOG_BURST,            OnDatalogBurst)
      ON_COMMAND(ID_FILE_DUMP,                OnDump)
      ON_COMMAND(ID_HELP_ABOUT,               OnHelpAbout)
      ON_COMMAND(ID_INJ_SIZE,                 OnInjSize)
      ON_COMMAND(ID_TOOLS_GENTP,              OnGenerateTP)
      ON_COMMAND(ID_STD_MINITERM,             OnMiniTerm)
      ON_COMMAND(ID_FILE_OPEN,                OnOpen)
      ON_COMMAND(ID_FILE_SAVE,                OnSave)
      ON_COMMAND(ID_FILE_SAVEAS,              OnSaveAs)
      ON_COMMAND(ID_MSQ_INFO,                 OnMsqInfo)
      ON_COMMAND(ID_FILE_PRINT,               OnPrint)
      ON_COMMAND(ID_TOOLS_REVIEW,             OnReview)
      ON_COMMAND(ID_RUNTIME_TABLEEDIT,        OnTableEdit)
      ON_COMMAND(ID_VEEXPORT,                 OnVeExport)
      ON_COMMAND(ID_VEIMPORT,                 OnVeImport)
      ON_COMMAND(ID_COMMUNICATIONS_SETTINGS,  OnSetport)
      ON_COMMAND(ID_RUNTIME_REALTIMEDISPLAY,  OnRuntime)

      ON_CONTROL_RANGE(BN_CLICKED, ID_STD_CON_00,      ID_STD_CON_00+9,    OnConstants)
      ON_CONTROL_RANGE(BN_CLICKED, ID_STD_INJ_00,      ID_STD_INJ_00+9,    OnInjectionControl)
      ON_CONTROL_RANGE(BN_CLICKED, ID_STD_ENR_00,      ID_STD_ENR_00+9,    OnEnrichments)
      ON_CONTROL_RANGE(BN_CLICKED, ID_STD_GO2_00,      ID_STD_GO2_00+0,    OnGenO2)
      ON_CONTROL_RANGE(BN_CLICKED, ID_STD_GTH_00,      ID_STD_GTH_00+0,    OnGenTherm)
      ON_CONTROL_RANGE(BN_CLICKED, ID_STD_MAF_00,      ID_STD_MAF_00+0,    OnGenMAF)
      ON_CONTROL_RANGE(BN_CLICKED, ID_STD_WWZ_00,      ID_STD_WWZ_00+9,    OnWarmupWizard)
      ON_CONTROL_RANGE(BN_CLICKED, ID_STD_AWZ_00,      ID_STD_AWZ_00+9,    OnAccelWizard)
      ON_CONTROL_RANGE(BN_CLICKED, ID_STD_TWZ_00,      ID_STD_TWZ_00+9,    OnTriggerWizard)
      ON_CONTROL_RANGE(BN_CLICKED, ID_USER_DEFINED_00, ID_USER_DEFINED_49, OnUserDefined)
      ON_CONTROL_RANGE(BN_CLICKED, ID_PLUGIN_00,       ID_PLUGIN_49,       OnPlugIn)
      ON_CONTROL_RANGE(BN_CLICKED, ID_USER_HELP_00,    ID_USER_HELP_49,    OnUserHelp)
      ON_CONTROL_RANGE(BN_CLICKED, ID_USER_TUNING_00,  ID_USER_TUNING_49,  OnTune)
      ON_CONTROL_RANGE(BN_CLICKED, ID_USER_CURVE_00,   ID_USER_CURVE_49,   OnCurveEditor)
      ON_CONTROL_RANGE(BN_CLICKED, ID_USER_TABLE_00,   ID_USER_TABLE_49,   OnTableEditor)
      ON_CONTROL_RANGE(BN_CLICKED, ID_STD_PORTEDIT,    ID_STD_PORTEDIT+0,  OnPortEdit)
      ON_CONTROL_RANGE(BN_CLICKED, ID_POPUP_000,       ID_POPUP_999,       OnPopup)

      ON_WM_CONTEXTMENU()
      ON_WM_CTLCOLOR()
      ON_WM_DESTROY()
      ON_WM_LBUTTONUP()
      ON_WM_PAINT()
      ON_WM_PARENTNOTIFY()
      ON_WM_QUERYDRAGICON()
      ON_WM_SIZE()
      ON_WM_TIMER()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

#if 1
   void logSize(CWnd *, char *) { }
#else
   FILE *log = NULL;

   void logSize(CWnd *w, char *title)
   {
      if (log == NULL) log = fopen("event.log", "w");
      CRect r;
      w->GetWindowRect(&r);
   // ScreenToClient(&r);
      fprintf(log, "%-12s w %d h %d\n", title, r.right-r.left, r.bottom-r.top);
      fflush(log);

      CWnd *s = w->GetDlgItem(IDC_STATUS_FILENAME);
      CRect rct; // Filename box is the "spring," stretching to fill.
      s->GetWindowRect(&rct);
      int txtHt = rct.Height();
      fprintf(log, "hgt = %d\n", txtHt);
   }
#endif

//------------------------------------------------------------------------------

void setColors(CBarMeter &m)
{
   m.SetColor(CBarMeter::meter_needle,    rep.g_ledColor);
   m.SetColor(CBarMeter::meter_gridalert, rep.g_ledAlertColor);
   m.SetColor(CBarMeter::meter_bground,   rep.g_backgroundColor);
}

void setupGauge(CAnalogMeter &m, const gaugeConfiguration *g)
{
   m.userData = g;

   if (!g)
      m.SetTitle("Undefined Gauge");
   else {
      m.SetTitle (g->title);
      m.SetRange (g->lo, g->hi);
      m.SetRanges(g->loW, g->hiW, g->loD, g->hiD);
      if (strcmp(g->units, S_TEMPfmt)     == 0) m.SetUnit(mdb.thermLabel());
      else if (strcmp(g->units, S_TPSfmt) == 0) m.SetUnit(mdb.rawTPS ? S_raw : S_pct);
      else                                      m.SetUnit(g->units);
      m.SetValueDecimals(g->vDecimals);
      m.SetRangeDecimals(g->rDecimals);
      m.setFaceBmp(g->faceFile);
   }

   m.SetColor(CAnalogMeter::meter_title,     rep.g_titleColor);
   m.SetColor(CAnalogMeter::meter_needle,    rep.g_needleColor);
   m.SetColor(CAnalogMeter::meter_grid,      rep.g_scaleColor);
   m.SetColor(CAnalogMeter::meter_gridalert, rep.g_alertColor);
   m.SetColor(CAnalogMeter::meter_value,     rep.g_valueColor);
   m.SetColor(CAnalogMeter::meter_range,     rep.g_rangeColor);
   m.SetColor(CAnalogMeter::meter_bground,   rep.g_backgroundColor);
   m.SetColor(CAnalogMeter::meter_face,      rep.g_faceColor);
   m.SetColor(CAnalogMeter::meter_bgwarn,    rep.g_bgWarningColor);
   m.SetColor(CAnalogMeter::meter_bgdang,    rep.g_bgDangerColor);

   if (m.m_hWnd) m.Invalidate();
}

void setupGauge(CAnalogMeter &m, const char *page, const char *name)
{
   const gaugeConfiguration *g = rep.gauge(page, name);
   setupGauge(m, g);
}

//------------------------------------------------------------------------------

void CMegatuneDlg::setTitleText()
{
   char title[128];
   strcpy(title, " MegaTune " VERSION_STRING);
   if (*mdb.title) {
      strcat(title, " - ");
      strcat(title, mdb.title);
   }
   SetWindowText(title);
}

//------------------------------------------------------------------------------

extern CString installationDir;
extern CString workingDir;
extern CString startupFile;
extern CString splashFile;

void CMegatuneDlg::setGauge(int gaugeNo)
{
   static char  gaugeLabel[8] = { S_gauge "X" };
   static char *ll = strchr(gaugeLabel, 'X');
   *ll = static_cast<char>('1'+gaugeNo);
   setupGauge(gaugeSet[gaugeNo], S_FrontPage, gaugeLabel);
}

void cfgLog(const char *Format, ...);

BOOL CMegatuneDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

// cfgLog("Startup File:           "+startupFile);

   dir.init();
   workingDir = dir.selected;
   rep.init(); // Must be in this order to force reading of ini file first.
   if (!mdb.init()) OnSetport();

   if (!mdb.commOpened()) {
      char msg[300];
      sprintf(msg,
         "The selected communication port (COM%d) could not be opened.\n\n"
         "To fix this click Communications -> Settings and change the port,\t\n"
         "then click File -> Offline to connect to your ECU.\n\n"
         "Go offline until you select a valid port?",
         mdb.port());
      int response = MessageBox(msg, "MegaTune Open Error", MB_YESNO | MB_DEFBUTTON1 | MB_ICONQUESTION);
      if (response == IDYES) {
         _onlineMode = false;
         setOnlineStatus();
      }
   }

   bool showSplash = startupFile.IsEmpty() != 0;
   CSplashWnd::EnableSplashScreen(showSplash, splashFile);
   CSplashWnd::ShowSplashScreen(this);

#if 0
   // ??? Moved to OnPaint?
   if (startupFile.IsEmpty())
      mdb.settingsFile = "_autoload_.msq";
   else
      mdb.settingsFile = startupFile;
   mdb.readSettings();
#endif

   if (mdb.mainWin.left != 0 || mdb.mainWin.right != 0) {
      MoveWindow(mdb.mainWin);
   }

   logSize(this, "megatuneDlg");

   //---------------------------------------------------------------------------
   //--  Set up the user-defined menu items.                                  --
   //--  http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vclib/html/_mfc_cmenu.3a3a.attach.asp

   uml.attach(this, "main");
   uil.attach(this, "main");

   // ??? Just turn them off for now...
// GetMenu()->EnableMenuItem(ID_VEEXPORT, MF_BYCOMMAND | MF_GRAYED);
// GetMenu()->EnableMenuItem(ID_VEIMPORT, MF_BYCOMMAND | MF_GRAYED);
   GetMenu()->DeleteMenu(ID_VEEXPORT, MF_BYCOMMAND);
   GetMenu()->DeleteMenu(ID_VEIMPORT, MF_BYCOMMAND);

   SetIcon(m_hIcon, TRUE);
   SetIcon(m_hIcon, FALSE);

   setTitleText();

   for (int i = 0; i < 8; i++) setGauge(i);

   meterEGO.SetRange(rep.lofEGO, rep.hifEGO);
   meterEGO.SetAlert(rep.rdfEGO > 0.0 ? rep.rdfEGO : mdb.getByName(S_egoSwitch, 0));
   setColors(meterEGO);

   m_statusConnected.SetWindowText("CONNECTED");
   m_statusConnected.EnableWindow(false);
   m_statusSaved.SetWindowText("SAVED");
   m_statusSaved.EnableWindow(false);
   m_statusLogging.SetWindowText("LOGGING");
   m_statusLogging.EnableWindow(false);
   m_statusFilename.SetWindowText(*mdb.settingsFile ? mdb.settingsFile : "No file loaded");
   timer(on);

   return TRUE;
}

//------------------------------------------------------------------------------

void CMegatuneDlg::OnPaint()
{
   CPaintDC dc(this);
   if (IsIconic()) {
      SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

      // Center icon in client rectangle
      int cxIcon = ::GetSystemMetrics(SM_CXICON);
      int cyIcon = ::GetSystemMetrics(SM_CYICON);

      CRect          rect;
      GetClientRect(&rect);
      int x = (rect.Width () - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;

      dc.DrawIcon(x, y, m_hIcon);
   }
   else {
      CDialog::OnPaint();
      CRect r;

      for (int iMeter = 0; iMeter < 8; iMeter++) {
         GetDlgItem(IDC_FRAME_1+iMeter)->GetWindowRect(&r);
         ScreenToClient(&r);
         gaugeSet[iMeter].ShowMeter(&dc, r, true);
      }

      GetDlgItem(IDC_LED_BAR)->GetWindowRect(&r);
      ScreenToClient(&r);
      meterEGO.ShowMeter(&dc, r);

      static bool firstTime = true;
      if (firstTime) {
         firstTime = false;
         if (startupFile.IsEmpty()) mdb.settingsFile = "_autoload_.msq";
         else                       mdb.settingsFile = startupFile;
         mdb.readSettings();
      }
   }
}

//------------------------------------------------------------------------------

void CMegatuneDlg::updateGauges(bool connected)
{
   if (!connected) {
      m_statusConnected.SetWindowText("CONNECTED");
      m_statusConnected.EnableWindow(false);
      CDC *dc = GetDC();
         for (int i = 0; i < 8; i++) {
            gaugeSet[i].UpdateNeedle(dc, CAnalogMeter::missingValue);
         }
      ReleaseDC(dc);
   }
   else {
      if (mdb.controllerReset) {
         char msg[20];
         sprintf(msg, "RESET %d", mdb.controllerResetCount);
         m_statusConnected.SetWindowText(msg);
         mdb.markLog(msg);
      }

      m_statusConnected.EnableWindow(true);

      CDC *dc = GetDC();
         meterEGO.UpdateNeedle(dc, mdb.egoLEDvalue());
         for (int i = 0; i < 8; i++) {
            const gaugeConfiguration *g = reinterpret_cast<const gaugeConfiguration *>(gaugeSet[i].userData);
            if (g) {
               gaugeSet[i].UpdateNeedle(dc, mdb.cDesc._userVar[g->och]);
            }
         }
      ReleaseDC(dc);
   }
   m_statusSaved.EnableWindow(!mdb.changed());
}

//------------------------------------------------------------------------------

void CMegatuneDlg::setOnlineStatus()
{
   GetMenu()->CheckMenuItem(ID_OFFLINE, MF_BYCOMMAND | (_onlineMode ? MF_UNCHECKED : MF_CHECKED));
   m_statusConnected.SetWindowText(_onlineMode ? "CONNECTED" : "OFFLINE");
}

void CMegatuneDlg::OnOffline()
{
   _onlineMode = !_onlineMode;
   timer(on);
   setOnlineStatus();
}

//------------------------------------------------------------------------------

void CMegatuneDlg::timer(int state)
{
   if (state == on && _onlineMode) {
      if (SetTimer(1, mdb.timerInterval, NULL) == 0) {
         MessageBox("ERROR: Cannot install timer.\nKill other useless Windows Apps.");
      }
   }
   else {
      KillTimer(1);
      ::Sleep(mdb.timerInterval);
   }
}

//------------------------------------------------------------------------------

void CMegatuneDlg::OnTimer(UINT nIDEvent)
{
#ifdef _DEBUG
   if (_heapchk() != _HEAPOK) {
      MessageBox("Heap error", "MT Heap Error "__FILE__" %d", __LINE__);
   }
#endif

   bool _connected = mdb.getRuntime();
   if (_connected) {
      static bool firstRead = true;
      if (firstRead) {
         if (!mdb.loaded()) mdb.load();
         firstRead = false;
         setTitleText();
      }
   }

   if (_timerDialog) _timerDialog->doTimer(_connected);
   else              updateGauges(_connected);

   CDialog::OnTimer(nIDEvent);
}

void CMegatuneDlg::callTimerDialog(dataDialog *timerDialog)
{
   dataDialog *oldTimerDialog = _timerDialog;
   _timerDialog = timerDialog;
      _timerDialog->DoModal();
   _timerDialog = oldTimerDialog;
} 

//------------------------------------------------------------------------------
//--  The system calls this to obtain the cursor to display while the user    --
//--  drags the minimized window.                                             --

HCURSOR CMegatuneDlg::OnQueryDragIcon()
{
   return (HCURSOR) m_hIcon;
}

void CMegatuneDlg::OnReview()
{
   uml.review(this);
}

void CMegatuneDlg::OnUserDefined(UINT nId)
{
   timer(off);
      userDefined udd(nId-ID_USER_DEFINED_00);
      udd.DoModal();
   timer(on);
}

void CMegatuneDlg::OnPlugIn(UINT nId)
{
   upl[nId-ID_PLUGIN_00].exec();
}

void CMegatuneDlg::OnUserHelp(UINT nId)
{
   timer(off);
      userHelp help(nId-ID_USER_HELP_00);
      help.DoModal();
   timer(on);
}

void CMegatuneDlg::OnConstants(UINT nId)
{
   timer(off);
      Dconstants con(nId-ID_STD_CON_00);
      con.DoModal();
   timer(on);
}

void CMegatuneDlg::OnInjectionControl(UINT nId)
{
   timer(off);
      injControl inj(nId-ID_STD_INJ_00);
      inj.DoModal();
   timer(on);
}

void CMegatuneDlg::OnEnrichments(UINT nId)
{
   timer(off);
      Denrichments enr(nId-ID_STD_ENR_00);
      enr.DoModal();
   timer(on);
}

void CMegatuneDlg::OnGenO2(UINT nId)
{
   timer(off);
      genO2 go2(nId-ID_STD_GO2_00);
      go2.DoModal();
   timer(on);
}

void CMegatuneDlg::OnGenTherm(UINT nId)
{
   timer(off);
      genTherm gth(nId-ID_STD_GTH_00);
      gth.DoModal();
   timer(on);
}

void CMegatuneDlg::OnGenMAF(UINT nId)
{
   timer(off);
      genMAF gth(nId-ID_STD_MAF_00);
      gth.DoModal();
   timer(on);
}

void CMegatuneDlg::OnRuntime()
{
   Druntime run;
   callTimerDialog(&run);
}

void CMegatuneDlg::OnSetport()
{
   timer(off);
      Dsetcomm port;
      port.DoModal();
   timer(on);
}

void CMegatuneDlg::OnTableEditor(UINT nId)
{
   tableEditor ted(nId-ID_USER_TABLE_00);
   callTimerDialog(&ted);
}

void CMegatuneDlg::OnCurveEditor(UINT nId)
{
   curveEditor ced(nId-ID_USER_CURVE_00);
   callTimerDialog(&ced);
}

void CMegatuneDlg::OnPortEdit(UINT nId)
{
   timer(off);
      portEdit spt(nId-ID_STD_PORTEDIT);
      spt.DoModal();
   timer(on);
}

void CMegatuneDlg::OnMiniTerm()
{
   timer(off);
      miniTerm edsp;
      edsp.DoModal();
   timer(on);
}

void CMegatuneDlg::OnTune(UINT nId)
{
   tuning3D   *td = userTuning(nId-ID_USER_TUNING_00);
   callTimerDialog(td);
}

void CMegatuneDlg::OnTableEdit()
{
   static hexEdit *xyz = new hexEdit();
   callTimerDialog(xyz);
}

void CMegatuneDlg::OnWarmupWizard(UINT nId)
{
   wueWizard xyz(nId-ID_STD_WWZ_00);
   callTimerDialog(&xyz);
}

void CMegatuneDlg::OnTriggerWizard(UINT nId)
{
   triggerWizard xyz(nId-ID_STD_TWZ_00);
   callTimerDialog(&xyz);
}

void CMegatuneDlg::OnAccelWizard(UINT nId)
{
   aeWizard xyz(nId-ID_STD_AWZ_00);
   callTimerDialog(&xyz);
}

//------------------------------------------------------------------------------

static int gaugeNo = 0;

void CMegatuneDlg::OnPopup(UINT nId)
{
   switch (nId) {
      case ID_POPUP_RESET_ONE:
         gaugeSet[gaugeNo].Reset();
         break;

      case ID_POPUP_RESET_ALL:
         for (gaugeNo = 0; gaugeNo < 8; gaugeNo++) {
            gaugeSet[gaugeNo].Reset();
         }
         gaugeNo = 0; // Invalidate first one, should redraw all.
         break;
   
      default:
         UINT iGauge = nId-ID_POPUP_000;
         const gaugeConfiguration *g = rep.gauge(S_GaugeConfigurations, rep.gaugeConfigs()[iGauge]);
         setupGauge(gaugeSet[gaugeNo], g);
         break;
   }
   /*gaugeSet[gaugeNo].*/Invalidate();
}

void CMegatuneDlg::showPopup(CWnd *mainWindow, CPoint point, CAnalogMeter *gaugeSet)
{
   CRect rct;
   gaugeNo = -1;
   for (int iGauge = 0; iGauge < 8; iGauge++) {
      GetDlgItem(IDC_FRAME_1+iGauge)->GetWindowRect(&rct); // Work in screen coordinates.
      if (point.x >= rct.left && point.x <= rct.right
       && point.y >= rct.top  && point.y <= rct.bottom) {
         gaugeNo = iGauge;

         CMenu menu;
         VERIFY(menu.LoadMenu(IDR_GAUGE_POPUP));
         CMenu *popup = menu.GetSubMenu(0);

#if 1
         int top = 14;
#else
         int top = 42;

//       need to figure out how to add this as a title bar on the popup
         const gaugeConfiguration *g = reinterpret_cast<const gaugeConfiguration *>(gaugeSet[gaugeNo].userData);
         CString title;
         title.Format("Gauge %d (%s)", gaugeNo+1, g->title);
         popup->InsertMenu(0, MF_BYPOSITION | MF_STRING | MF_DISABLED, 0, title);
         popup->InsertMenu(1, MF_BYPOSITION | MF_SEPARATOR,            0, LPSTR(NULL));
         SetMenuDefaultItem(popup->m_hMenu, 0, TRUE);
#endif

         CMenu gaugeList; // Add gauge list in a right popup to keep first one pretty.
         gaugeList.CreatePopupMenu();
         popup->AppendMenu(MF_POPUP, UINT(gaugeList.m_hMenu), "&Swap To");

         std::vector<CString> names  = rep.gaugeConfigs();
         int                  nNames = names.size();

#        define MAXROWS 30
         int cols = 1;
         int rows = nNames / cols;
         while (rows > MAXROWS) { cols++; rows = nNames / cols; }
         while (rows * cols > nNames) rows--;
         rows++;

         CString menuEntry;
         for (iGauge = 0; iGauge < nNames; iGauge++) {
            const gaugeConfiguration *g = rep.gauge(S_GaugeConfigurations, names[iGauge]);
            menuEntry.Format("%s\t\"%s\"", names[iGauge], g->title);
            gaugeList.AppendMenu(MF_STRING | (iGauge%rows==0?MFT_MENUBARBREAK:0), ID_POPUP_000+iGauge, menuEntry);
         }

         popup->TrackPopupMenu(TPM_CENTERALIGN | TPM_RIGHTBUTTON, point.x, point.y-top, mainWindow);
         break;
      }
   }
}

void CMegatuneDlg::OnContextMenu(CWnd *pWnd, CPoint point)
{
   showPopup(this, point, gaugeSet);
}

void CMegatuneDlg::OnParentNotify(UINT message, LPARAM lParam)
{
   if (message != WM_RBUTTONUP)
      CDialog::OnParentNotify(message, lParam);
   else {
      CPoint point(LOWORD(lParam), HIWORD(lParam));
      showPopup(this, point, gaugeSet);
   }
}

//------------------------------------------------------------------------------

BOOL CMegatuneDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
#if 0
   if (log==NULL) log=fopen("event.log", "w");
   fprintf(log, "wParam = 0x%08x  lParam = 0x%08x\n", wParam, lParam);
   fflush(log);
#endif

   if (wParam == IDOK) return TRUE;

   if (wParam == IDCANCEL && mdb.changed()) {
//    timer(off);
      int response = MessageBox("Save changes before quitting?", "Save", MB_YESNOCANCEL | MB_DEFBUTTON1 | MB_ICONQUESTION);
      switch (response) {
         case IDCANCEL:
//          timer(on);
            return TRUE;
         case IDYES:
            OnSaveAs();
            break;
      }
   }

   return CDialog::OnCommand(wParam, lParam);
}

//------------------------------------------------------------------------------

void CMegatuneDlg::OnSave()
{
   static bool inOnSave = false;
   if (!inOnSave) {
      inOnSave = true;
         timer(off);
            mdb.save();
            m_statusFilename.SetWindowText(mdb.settingsFile);
         timer(on);
      inOnSave = false;
   }
}

void CMegatuneDlg::OnSaveAs()
{
   timer(off);
      mdb.saveAs();
      m_statusFilename.SetWindowText(mdb.settingsFile);
   timer(on);
}

void CMegatuneDlg::OnMsqInfo()
{
   timer(off);
      msqInfoDlg mi;
      mi.DoModal();
   timer(on);
}

//------------------------------------------------------------------------------

void CMegatuneDlg::OnOpen()
{
   static bool inOnOpen = false;
   if (!inOnOpen) {
      inOnOpen = true;
         timer(off);
            mdb.open();
            m_statusFilename.SetWindowText(mdb.settingsFile);
         timer(on);
      inOnOpen = false;
   }
}

//------------------------------------------------------------------------------

void CMegatuneDlg::OnPrint()
{
   CPrintDialog dlg(FALSE);
   if (dlg.DoModal() == IDOK) {
      // Get a handle to the printer device context (DC).
      HDC hdc = dlg.GetPrinterDC();
      ASSERT(hdc);
      CDC *cdc = CDC::FromHandle(hdc);

      // Do something with the HDC...
      Print(cdc, PRF_CLIENT | PRF_ERASEBKGND);

      // Clean up.
      cdc->DeleteDC();
   }
}

//------------------------------------------------------------------------------

void CMegatuneDlg::OnDatalogBurst()
{
   timer(off);
      mdb.setRecording(true, true);
      m_statusLogging.EnableWindow(mdb.recording());
      m_statusLogging.SetWindowText("BURST");
      burstLog bl;
      bl.DoModal();
      OnDatalogX();
   timer(on);
}

void CMegatuneDlg::OnDatalog()
{
   timer(off);
      mdb.setRecording(true);
      m_statusLogging.EnableWindow(mdb.recording());
   timer(on);
}

void CMegatuneDlg::OnDatalogX()
{
   timer(off);
      mdb.setRecording(false);
      m_statusLogging.EnableWindow(mdb.recording());
      m_statusLogging.SetWindowText("LOGGING");
   timer(on);
}

void CMegatuneDlg::OnDatalogOpt()
{
   timer(off);
      DlogOptions lo;
      lo.DoModal();
   timer(on);
}

//------------------------------------------------------------------------------
void CMegatuneDlg::OnDump()
{
   timer(off);
      mdb.dump();
   timer(on);
}

//------------------------------------------------------------------------------

void CMegatuneDlg::OnInjSize()
{
   injectorSize is;
   is.DoModal();
}

//------------------------------------------------------------------------------

void CMegatuneDlg::OnVeExport()
{
   timer(off);
      mdb.veExport();
   timer(on);
}

void CMegatuneDlg::OnVeImport()
{
   timer(off);
      mdb.veImport();
   timer(on);
}

//------------------------------------------------------------------------------

void CMegatuneDlg::OnGenerateTP()
{
   timer(off);
      tpgen gen;
      gen.DoModal();
   timer(on);
}

//------------------------------------------------------------------------------

void CMegatuneDlg::OnHelpAbout()
{
// timer(off);
      Dabout about;
      about.DoModal();
// timer(on);
}

//------------------------------------------------------------------------------

void CMegatuneDlg::OnCommlog()
{
   bool state = mdb.toggleCommLogging();
   GetMenu()->CheckMenuItem(ID_COMMLOG, MF_BYCOMMAND | (state ? MF_CHECKED : MF_UNCHECKED));
}

//------------------------------------------------------------------------------

void CMegatuneDlg::OnDestroy()
{
   CDialog::OnDestroy();

   this->GetWindowRect(&mdb.mainWin);
   mdb.writeConfig();
}

//------------------------------------------------------------------------------

HBRUSH CMegatuneDlg::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
   HBRUSH        hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
   static CBrush brush;

   pDC->SetBkMode(TRANSPARENT);

   for (int i = 0; i < uil.nIndicators("main"); i++) {
      if (uil[i].setBrush(pWnd, pDC, brush)) hbr = brush;
   }

// if (pWnd->GetDlgCtrlID() == IDC_STATUS_FILENAME) {
//    brush.DeleteObject();
//    brush.CreateSolidBrush(RGB(255,0,255));
//    hbr = brush;
// }

   if (pWnd->GetDlgCtrlID() == m_statusConnected.GetDlgCtrlID()) {
   }
   if (pWnd->GetDlgCtrlID() == m_statusSaved.GetDlgCtrlID()) {
   }
   if (pWnd->GetDlgCtrlID() == m_statusLogging.GetDlgCtrlID()) {
   }

   return hbr;
}

//------------------------------------------------------------------------------

void CMegatuneDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
   if (_invalidated) {
      _invalidated = false;
      Invalidate();
   }
}

//------------------------------------------------------------------------------

int CMegatuneDlg::moveDown(UINT id, int cy, int &right)
{
   CRect rct;
   CWnd *s = GetDlgItem(id);
   s->GetWindowRect(&rct);
   ScreenToClient(&rct);
   int top = cy - (rct.Height() + 3);
   right  -= rct.Width();
   s->MoveWindow(right, top, rct.Width(), rct.Height());
   return top;
}

void CMegatuneDlg::OnSize(UINT nType, int cx, int cy)
{
   _invalidated = true;

   CDialog::OnSize(nType, cx, cy);
   if (nType == SIZE_MINIMIZED) return; // Don't waste time.

   CWnd *s = GetDlgItem(IDC_STATUS_FILENAME);
   if (s == NULL) return; // We haven't been realized, yet.

   // There has got to be a better way than this, doesn't
   // MFC have a decent layout manager like Tk or Qt?
   int right = cx;
   moveDown(IDC_STATUS_CONNECTED, cy, right); right -= 3;
   moveDown(IDC_STATUS_LOGGING,   cy, right); right -= 3;
   moveDown(IDC_STATUS_SAVED,     cy, right); right -= 3;

   CRect rct; // Filename box is the horizontal spring.
   s->GetWindowRect(&rct);
   ScreenToClient  (&rct);
   static int txtHt = rct.Height();
   int top = cy - (txtHt + 3);
   s->MoveWindow(rct.left, top, right-rct.left-3, txtHt);

   int nIndicators  = uil.nIndicators("main");
   if (nIndicators > 0) {
      top        -= txtHt;
      rct.top     = top;
      rct.bottom -= txtHt;
      for (int i = 0; i < nIndicators; i++) {
         rct.left    = int((i+0) * cx/nIndicators);
         rct.right   = int((i+1) * cx/nIndicators);
         if (uil[i].m_hWnd) uil[i].MoveWindow(rct, true);
      }
   }

   CWnd *e = GetDlgItem(IDC_LED_BAR);
   e->GetWindowRect(&rct);
   ScreenToClient(&rct);
   int barHt = 11 + top/40;
   if (barHt < 22) barHt = 22;
   top -= (barHt + 3);
   e->MoveWindow(rct.left, top, cx, barHt);

   int wd = cx/4;
   int gaugeHt = top/2;

   int frame[8] = {
      IDC_FRAME_1, IDC_FRAME_2, IDC_FRAME_3, IDC_FRAME_4,
      IDC_FRAME_5, IDC_FRAME_6, IDC_FRAME_7, IDC_FRAME_8
   };

   for (int r = 0; r < 2; r++) {
      for (int c = 0; c < 4; c++) {
         CWnd *w = GetDlgItem(frame[r*4+c]);
         if (w) w->MoveWindow(c*wd, r*gaugeHt, wd, gaugeHt);
      }
   }
// for (int iG = 0; iG < 8; iG++) gaugeSet[iG].Invalidate();

   Invalidate();
}

//------------------------------------------------------------------------------

BOOL CMegatuneDlg::PreTranslateMessage(MSG *pMsg)
{
   if (CSplashWnd::PreTranslateAppMessage(pMsg)) {
      return TRUE;
   }

   if (pMsg->message == WM_KEYDOWN) {
      if (pMsg->wParam == VK_SPACE) {
         mdb.markLog();
         return TRUE;
      }
   }

   return CDialog::PreTranslateMessage(pMsg);
}

//------------------------------------------------------------------------------
