//------------------------------------------------------------------------------
//--  Copyright (c) 2004 by Eric Fahlgren, All Rights Reserved.               --
//--                                                                          --
//--  This program is free software; you can redistribute it and/or           --
//--  modify it under the terms of the GNU General Public License             --
//--  as published by the Free Software Foundation; either version            --
//--  2 of the License, or (at your option) any later version.                --
//--  See http://www.gnu.org/licenses/gpl.txt                                 --
//------------------------------------------------------------------------------

//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#ifndef VETABLE_H
#define VETABLE_H 1

#if _MSC_VER >= 1000
#  pragma once
#endif

//*****************************************
//******** PC Configurator V1.00 **********
//*** (C) - 2001 B.Bowling/A. Grippo ******
//** All derivatives from this software ***
//**  are required to keep this header ****
//*****************************************

//------------------------------------------------------------------------------

#include "Dttune.h"
#include "hexedit.h"

class veTable : public dataDialog {
   int          nId;
   tuningDialog *td;

   Cell  **xBox;
   Cell  **yBox;
   Cell  **zBox;

   int    nX,   nY;
   double zMin, zMax;
   bool   colored;
   bool   flicker; // Suppress erases when we just change cell backgrounds.
   void   recalcMinMax();

// char *idxLabel(const char *prefix, int idx, int dims);
   void  dbFromFields(UINT nId=0); // Optional arg 
   void  fieldsFromDb();

public:
   veTable(int nId, CWnd *pParent = NULL);
  ~veTable();

   //{{AFX_DATA(veTable)
      enum { IDD = IDD_VETABLE };
      CButton   m_xLabel;
      CButton   m_yLabel;
      CButton   m_zLabel;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(veTable)
   protected:
      virtual void   DoDataExchange(CDataExchange * pDX);  // DDX/DDV support
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(veTable)
      virtual BOOL   OnInitDialog();
      afx_msg void   OnDestroy();
      afx_msg void   OnTableGet();
      afx_msg void   OnTablePut();
      afx_msg void   OnPlotVE();
      afx_msg void   OnGenerateVE();
      afx_msg void   OnScaleVE();
      afx_msg void   OnShiftTable();
      afx_msg void   OnVeExport();
      afx_msg void   OnVeImport();
      afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
      afx_msg BOOL   OnEraseBkgnd(CDC* pDC);
      DECLARE_EVENTSINK_MAP()
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif // VETABLE_H
