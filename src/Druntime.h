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

//{{AFX_INCLUDES()
//}}AFX_INCLUDES

#ifndef DRUNTIME_H
#define DRUNTIME_H 1

#if _MSC_VER >= 1000
#  pragma once
#endif

//==============================================================================

class hysteresisBar : public CProgressCtrl {
   public:
      hysteresisBar();
      virtual ~hysteresisBar() { }

      int  SetPos(int nPos);

   protected:
      //{{AFX_MSG(hysteresisBar)
      afx_msg void   OnPaint();
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()

   private:
      double m_highPercent;
      double m_decayInterval;
      double m_currentTime;
      double m_startTime;
};

//------------------------------------------------------------------------------

struct barData {
   CEdit         *box;
   hysteresisBar *bar;
   symbol        *sym;
   double         lo, hi;
   int            digits;
   barData(const char *, CWnd *, UINT, UINT, double, double, int);
   void show();
   void disconnected();
};

//------------------------------------------------------------------------------

class Druntime : public dataDialog {
   void updateDisplay();

   enum { nWidgets = 18 };
   barData       *bd[nWidgets];

   symbol        *sEngine;

   CEdit         *connected;
   bool           isConnected;

   CBrush redBrush;
   CBrush yellowBrush;
   CBrush whiteBrush;
   CBrush blueBrush;

   hysteresisBar m_accelCorBar;
   hysteresisBar m_airCorBar;
   hysteresisBar m_baroCorBar;
   hysteresisBar m_battBar;
   hysteresisBar m_cltBar;
   hysteresisBar m_dutycycleBar;
   hysteresisBar m_gammaBar;
   hysteresisBar m_egoCorBar;
   hysteresisBar m_egoBar;
   hysteresisBar m_mapBar;
   hysteresisBar m_matBar;
   hysteresisBar m_pwBar;
   hysteresisBar m_rpmBar;
   hysteresisBar m_tpsBar;
   hysteresisBar m_veCorBar;
   hysteresisBar m_warmCorBar;

protected:
   //{{AFX_MSG(Druntime)
      virtual BOOL   OnInitDialog();
      afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
//      DECLARE_EVENTSINK_MAP()
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

public:
   Druntime(CWnd * pParent = NULL);
  ~Druntime();

   virtual bool doTimer(bool connected);

   //{{AFX_DATA(Druntime)
      enum { IDD = IDD_RUNTIME };
      CStatic m_matLabel;
      CStatic m_coolantLabel;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(Druntime)
      public:
         virtual BOOL DestroyWindow();
         virtual BOOL PreTranslateMessage(MSG *pMsg);
      protected:
         virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif // DRUNTIME_H
