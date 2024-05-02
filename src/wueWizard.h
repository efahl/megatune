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

#ifndef DWUE_WIZARD_H
#define DWUE_WIZARD_H 1

#if _MSC_VER >= 1000
#pragma once
#endif

#include "Druntime.h"

class wueWizard : public dataDialog {
   int  _pageNo;

   stdEditFieldList _fld;

   enum { nBars = 4 };
   barData *bd[nBars];
   symbol  *coolant;

   void fieldsFromDb();
   void dbFromFields();
   void moveArrow   (int i);
   void bump        (int &val, double lo, double hi, int inc);

   double colorTemperature(int idx, double temperature);

   enum { nBins = 10 };
   double wwuLo[nBins];
   double wwuHi[nBins];

   CStatic       m_arrow;//[nBins];
   CStatic       m_wwuL[nBins];
   int           m_val [nBins];
   CEdit         m_bin [nBins];

   hysteresisBar m_warmCorBar;
   hysteresisBar m_mapBar;
   hysteresisBar m_cltBar;
   hysteresisBar m_egoBar;

public:
   wueWizard(int pageNo, CWnd *pParent=NULL);
  ~wueWizard();

   virtual bool doTimer(bool connected);

   //{{AFX_DATA(wueWizard)
      enum { IDD = IDD_WUE_WIZARD };
      CStatic       m_coolantLabel;
      CStatic       m_floodclearLabel;
      CStatic       m_pw1Label;
      CStatic       m_pw2Label;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(wueWizard)
   public:
      virtual BOOL PreTranslateMessage(MSG *pMsg);
   protected:
      virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(wueWizard)
      virtual BOOL   OnInitDialog();
      afx_msg void   OnUp();
      afx_msg void   OnDown();
      afx_msg void   DoIncrement();
      afx_msg void   DoDecrement();
      afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // DWUE_WIZARD_H
