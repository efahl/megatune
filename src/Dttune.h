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
#ifndef DTTUNE_H
#define DTTUNE_H

#if _MSC_VER >= 1000
#  pragma once
#endif

//*****************************************
//******** PC Configurator V1.00 **********
//*** (C) - 2001 B.Bowling/A. Grippo ******
//** All derivatives from this software ***
//**  are required to keep this header ****
//*****************************************

#include "AnalogMeter.h"
#include "BarMeter.h"
#include "plot3d.h"

//------------------------------------------------------------------------------

void userTuning(UINT nId); // The real entry point.

//------------------------------------------------------------------------------

class Dttune;

struct tuningDialog {
   enum { nUT = 20 };
   std::string title;
   int         pageNo;

   symbol      *x,        *y,        *z;
   std::string  xName,     yName,     zName;
   bool         xEditable, yEditable, zEditable;
   symbol      *xOch,     *yOch;
   std::string  xOchName,  yOchName;

   //--  Grid display characteristics  -----------------------------------------
   double gridScale;
   std::string upLabel;
   std::string dnLabel;

   Dttune *pTune;

   tuningDialog()
    : title("")
    , xOchName("")
    , yOchName("")
    , pTune(NULL)
    , pageNo(-1)
    , gridScale(1.0)
    , xEditable(true)
    , yEditable(true)
    , zEditable(true)
    { }

   bool addTuning  () { return true; }
   bool addSettings() { return true; }

   double XBIN(int ix);
   double YBIN(int iy);
   double ZVAL(int ix, int iy);
   const char *ZSTR(int ix, int iy);
   int  _iZVAL(int ix, int iy);

   double interpolate(double X, double Y);
};

//------------------------------------------------------------------------------

class Dttune : public dataDialog {
   bool realized;

   int          nId;
   tuningDialog *td;

   long    lgr;
   long    dgr;
   CBrush  brushbg;
   CRect   vemsgrect;

   int     iXBIN, iYBIN, iZVAL;

   int     nX,    nY;
   int     vXBIN, vYBIN;
   double  minXBIN, maxXBIN;
   double  minYBIN, maxYBIN;

public:
   Dttune(int nId, CWnd *pParent=NULL);
  ~Dttune();

   //{{AFX_DATA(Dttune)
      enum { IDD = IDD_TUNING };
      CEdit   m_tReqFuel;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(Dttune)
   public:
      virtual BOOL PreTranslateMessage(MSG* pMsg);
   protected:
      virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
      virtual void DoDataExchange(CDataExchange * pDX);
   //}}AFX_VIRTUAL

private:
   enum { on = 1, off = 0 };
   void timer(int state);

   char *locMsg(int curX, int curY, const char *suffix);

   enum gaugeState { aGauges = 0, bGauges = 1, cGauges = 2, dGauges = 3 };
   gaugeState gauges;
   void setupGauges(bool forceUpdate);
   enum { nGauges = 6 };
   CAnalogMeter gauge[nGauges];
   CBarMeter    meterEGO;

   void setupAllGeometry();
   int  moveDown(UINT, int, int &);
   bool geometrySetup;

   plot3d *p3d; // 3D plot area in VE grid window.
   static double Xrot, Yrot, Zrot;
   static bool   flat;
   void    setPerspective(bool);
   void    drawGrid();
   int     initCX, initCY;
   void    doResize(int cx, int cy);

   int     _fontSize;
   CString _fontFace;
   CFont   _msgFont;
   void  setMsg(const char *newMsg, int line, long clr);
   void  updateDisplay();
   void  updateGauges();
   void  zeroGauges();
   BOOL  spinReqFuel(int delta);

   bool    chasing; // Chase mode does an automatic "F" at timer events.

   int     wux1;
   int     wux2;
   int     wuy1;
   int     wuy2;

protected:
   //{{AFX_MSG(Dttune)
      virtual BOOL   OnInitDialog();
      afx_msg void   OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
      afx_msg void   OnPaint();
      afx_msg void   OnTimer(UINT nIDEvent);
      afx_msg void   OnBurn();
      afx_msg void   OnReqfuelSpin(NMHDR* pNMHDR, LRESULT* pResult);
      afx_msg void   OnPageA();
      afx_msg void   OnPageB();
      afx_msg void   OnPageC();
      afx_msg void   OnPageD();
      afx_msg void   OnDestroy();
      afx_msg void   OnSize(UINT nType, int cx, int cy);
      DECLARE_EVENTSINK_MAP()
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // DTTUNE_H
