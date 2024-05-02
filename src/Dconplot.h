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

#ifndef DCONPLOT_H
#define DCONPLOT_H 1

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//*****************************************
//******** PC Configurator V1.00 **********
//*** (C) - 2001 B.Bowling/A. Grippo ******
//** All derivatives from this software ***
//**  are required to keep this header ****
//*****************************************

//------------------------------------------------------------------------------

class Dconplot : public dataDialog {
   int          nId;
   tuningDialog *td;

   int _savePage;

   int        nX, xDigits;
   int        nY, yDigits;
   double     *xPlotVals;
   double     *yPlotVals;
   double    **zPlotVals;
   double    minZ, maxZ;

public:
   Dconplot(int nId, CWnd *pParent = NULL);
  ~Dconplot();

   // Public for others to use same coloring scheme.
   static void color(double pct, COLORREF &rgb, double intensity=1.0, double blackness=0.0);

   //{{AFX_DATA(Dconplot)
      enum { IDD = IDD_CONPLOT };
   //}}AFX_DATA

   //{{AFX_VIRTUAL(Dconplot)
protected:
      virtual void DoDataExchange(CDataExchange * pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(Dconplot)
      virtual BOOL OnInitDialog();
      afx_msg void OnPaint();
      afx_msg void OnSize(UINT nType, int cx, int cy);
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // DCONPLOT_H
