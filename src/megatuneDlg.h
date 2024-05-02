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

#ifndef MEGATUNEDLG_H
#define MEGATUNEDLG_H 1

#if _MSC_VER >= 1000
#pragma once
#endif

#include "AnalogMeter.h"
#include "BarMeter.h"

//------------------------------------------------------------------------------
//
void setColors (CBarMeter    &m);
void setupGauge(CAnalogMeter &m, const char *page, const char *name);

//------------------------------------------------------------------------------

class CMegatuneDlg : public CDialog {
private:
   dataDialog *_timerDialog;
   bool        _connected;
   bool        _onlineMode;
   bool        _invalidated;

   void         callTimerDialog(dataDialog *timerDialog);

   int moveDown(UINT, int, int &);

   CAnalogMeter gaugeSet[8];
   CBarMeter    meterEGO;

   enum { on = 1, off = 0 };
   void timer(int);

   void setTitleText();

   void setGauge (int gaugeNo);
   void showPopup(CWnd *mainWindow, CPoint point, CAnalogMeter *gaugeSet);
   void setOnlineStatus();

public:
   CMegatuneDlg(CWnd *pParent=NULL);

   void updateGauges(bool connected);
   void exec(UINT dialogId) { WindowProc(WM_COMMAND, dialogId, 0); }

   HICON       m_hIcon;

   //{{AFX_DATA(CMegatuneDlg)
      enum { IDD = IDD_MEGATUNE_DIALOG };
      CEdit    m_statusLogging;
      CEdit    m_statusFilename;
      CEdit    m_statusSaved;
      CEdit    m_statusConnected;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(CMegatuneDlg)
   public:
      virtual BOOL PreTranslateMessage(MSG* pMsg);
   protected:
      virtual void DoDataExchange(CDataExchange * pDX);
      virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
   //}}AFX_VIRTUAL

protected:
   CStatusBar  m_wndStatusBar;
   CToolBar    m_wndToolBar;

   //{{AFX_MSG(CMegatuneDlg)
      virtual BOOL    OnInitDialog();
      afx_msg HCURSOR OnQueryDragIcon();
      afx_msg void    OnCommlog();
      afx_msg void    OnConstants(UINT nId);
      afx_msg void    OnContextMenu(CWnd *pWnd, CPoint point);
      afx_msg HBRUSH  OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
      afx_msg void    OnDatalogOpt();
      afx_msg void    OnDatalog();
      afx_msg void    OnDatalogX();
      afx_msg void    OnDestroy();
      afx_msg void    OnDump();
      afx_msg void    OnInjectionControl(UINT nId);
      afx_msg void    OnInjSize    ();
      afx_msg void    OnEnrichments(UINT nId);
      afx_msg void    OnGenerateTP ();
      afx_msg void    OnGenO2      (UINT nId);
      afx_msg void    OnGenTherm   (UINT nId);
      afx_msg void    OnGenMAF     (UINT nId);
      afx_msg void    OnHelpAbout  ();
//    afx_msg void    OnHexEdit    ();
      afx_msg void    OnLButtonUp  (UINT nFlags, CPoint point);
      afx_msg void    OnMiniTerm   ();
      afx_msg void    OnMsqInfo    ();
      afx_msg void    OnOffline    ();
      afx_msg void    OnOpen       ();
      afx_msg void    OnPaint      ();
      afx_msg void    OnParentNotify(UINT message, LPARAM lParam);
      afx_msg void    OnPlugIn     (UINT nId);
      afx_msg void    OnPopup      (UINT nId);
      afx_msg void    OnPrint      ();
      afx_msg void    OnRuntime    ();
      afx_msg void    OnSave();
      afx_msg void    OnSaveAs();
      afx_msg void    OnScaleVE();
      afx_msg void    OnGenerateVE();
      afx_msg void    OnSetport();
      afx_msg void    OnSize(UINT nType, int cx, int cy);
      afx_msg void    OnTableEdit();
      afx_msg void    OnReview();
      afx_msg void    OnTimer       (UINT nIDEvent);
      afx_msg void    OnCurveEditor (UINT nId);
      afx_msg void    OnTableEditor (UINT nId);
      afx_msg void    OnPortEdit    (UINT nId);
      afx_msg void    OnTune        (UINT nId);
      afx_msg void    OnUserDefined (UINT nId);
      afx_msg void    OnUserHelp    (UINT nId);
      afx_msg void    OnVeExport();
      afx_msg void    OnVeImport();
      afx_msg void    OnAccelWizard  (UINT nId);
      afx_msg void    OnTriggerWizard(UINT nId) ;
      afx_msg void    OnWarmupWizard (UINT nId);
      afx_msg void    OnDatalogBurst();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif
