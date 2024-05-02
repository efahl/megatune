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

#ifndef AEWIZARD_H
#define AEWIZARD_H 1

#if _MSC_VER > 1000
#  pragma once
#endif // _MSC_VER > 1000

#include "Druntime.h"
#include "BarMeter.h"

//------------------------------------------------------------------------------

class aeWizard : public dataDialog {
   int  pageNo;
   int  oldV;

   void fieldsFromDb();
   void dbFromFields();
   void onProportionChanged();

   symbol           *_prp;
   bool              _prpMap;
   stdEditFieldList  _fld;
   int               _lo;
   int               _hi;
   bool              _mapOn;
   bool              _tpsOn;

   enum { nBars = 2 };
   barData *bd[nBars];

   CBarMeter    meterEGO;

   void setLabel(int ID, double v);

public:
   aeWizard(int pageNo, CWnd *pParent=NULL);
  ~aeWizard();

   virtual bool doTimer(bool connected);

   //{{AFX_DATA(aeWizard)
      enum { IDD = IDD_AE_WIZARD };
      CStatic       m_tpsDotUnits;
      CStatic       m_thresholdLabel;
      hysteresisBar m_mapdotBar;
      CStatic       m_rateLabel;
      hysteresisBar m_tpsdotBar;
      CStatic       m_tpsPropLabel;
      CSliderCtrl   m_tpsProportion;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(aeWizard)
   protected:
      virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(aeWizard)
      virtual BOOL OnInitDialog();
      afx_msg void OnUp();
      afx_msg void OnDown();
      afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
      afx_msg void OnPaint();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif // AEWIZARD_H
