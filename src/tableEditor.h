//------------------------------------------------------------------------------
//--  Copyright (c) 2004,2005,2006 by Eric Fahlgren, All Rights Reserved.     --
//--                                                                          --
//--  This program is free software; you can redistribute it and/or           --
//--  modify it under the terms of the GNU General Public License             --
//--  as published by the Free Software Foundation; either version            --
//--  2 of the License, or (at your option) any later version.                --
//--  See http://www.gnu.org/licenses/gpl.txt                                 --
//------------------------------------------------------------------------------

//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#ifndef TABLE_EDITOR_H
#define TABLE_EDITOR_H 1

#if _MSC_VER >= 1000
#  pragma once
#endif

//------------------------------------------------------------------------------

#include "tuning3D.h"
#include "hexedit.h"

class tableEditor : public dataDialog {
   int          nId;
   tuningDialog *td;

   Cell  **xBox;
   Cell  **yBox;
   Cell  **zBox;
   int *cellLft, cellW;
   int *cellTop, cellH;

   int    rSelected, cSelected;
   bool   rangeSelected;

   double zMin, zMax;
   bool   _colored;
   bool   _flicker; // Suppress erases when we just change cell backgrounds.
   void   recalcMinMax();

   bool   _changed;

// char *idxLabel(const char *prefix, int idx, int dims);
   void  tagChanged  (UINT nId=0);
   void  dbFromFields(UINT nId=0); // Optional arg 
   void  fieldsFromDb();

   void  snapRect  (CRect &r);
   void  xformCells(double scale, double shift);

public:
   tableEditor(int nId, CWnd *pParent=NULL);
  ~tableEditor();

   virtual bool doTimer(bool connected);

   //{{AFX_DATA(tableEditor)
      enum { IDD = IDD_TABLE_EDITOR };
      CButton   m_xLabel;
      CButton   m_yLabel;
      CButton   m_zLabel;
      CPoint    m_firstClick;
      CRect     m_dragRect;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(tableEditor)
   public:
      virtual BOOL PreTranslateMessage(MSG *pMsg);
   protected:
      virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(tableEditor)
      virtual BOOL   OnInitDialog();
      afx_msg void   OnTableGet  ();
      afx_msg void   OnTablePut  ();
      afx_msg void   OnPlotVE    ();
      afx_msg void   OnGenerateVE();
      afx_msg void   OnScaleVE   ();
      afx_msg void   OnShiftTable();
      afx_msg void   OnTeSet     ();
      afx_msg void   OnTeMul     ();
      afx_msg void   OnTeSub     ();
      afx_msg void   OnTeAdd     ();
      afx_msg void   OnVeExport  ();
      afx_msg void   OnVeImport  ();
      afx_msg HBRUSH OnCtlColor  (CDC* pDC, CWnd* pWnd, UINT nCtlColor);
      afx_msg BOOL   OnEraseBkgnd(CDC* pDC);

      afx_msg void   OnLButtonDown(UINT nFlags, CPoint point);
      afx_msg void   OnMouseMove  (UINT nFlags, CPoint point);
      afx_msg void   OnLButtonUp  (UINT nFlags, CPoint point);
      afx_msg void   OnCancelMode ();

//      DECLARE_EVENTSINK_MAP()
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif
