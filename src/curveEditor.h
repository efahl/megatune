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
#ifndef CURVEEDITOR_H
#define CURVEEDITOR_H 1

#if _MSC_VER >= 1000
#  pragma once
#endif

//------------------------------------------------------------------------------

#include "plot3d.h"

class curveDialog {
   public:
      curveDialog();

      enum { nUC = 50 };

      int     pageNo();
      CString id    ();
      CString title ();
      CString xLabel();
      CString yLabel();
      CString xUnits();
      CString yUnits();

      void    setPageNo(int);
      void    setId    (CString);
      void    setTitle (CString);
      void    setLabels(CString, CString);
      void    setX(CString, CString, bool);
      void    setY(CString, CString, bool);
      void    setXaxis(int lo, int hi, int tix);
      void    setYaxis(int lo, int hi, int tix);

      void    resolve();

      symbol *x(); CString xName();
      symbol *y(); CString yName();

      int    iEDIT; // Edit point
      int    iTRAK; // Run-time tracking point
      double xSpot;
      double ySpot;

      int    nBINS  ();
      double XBIN   (int i); double xLo(); double xHi();
      double YBIN   (int i); double yLo(); double yHi();
      double minXBIN();
      double minYBIN();
      double maxXBIN();
      double maxYBIN();
      int    xTix   ();
      int    yTix   ();

      bool deltaY(int dy);
      bool deltaX(int dx);
      int  deltaI(int di);

      int    updateTrack();
      double xOch();

      const char   *helpDlg  () const      { return _helpDlg; }
      void          setHelp  (CString h  ) { _helpDlg = h;    } 

      void          setGauge (CString gn) { _gaugeName = gn;   }
      CString       gaugeName()           { return _gaugeName; }
      CAnalogMeter &gauge()               { return _gauge;     }

      bool xEditable() const { return _xEditable; }
      bool yEditable() const { return _yEditable; }

   private:
      int          _pageNo;
      CString      _id;
      CString      _title;
      double       _xLo, _xHi; int _xTix;
      double       _yLo, _yHi; int _yTix;

      CString      _xLabel,    _yLabel;
      symbol      *_x,        *_y;
      CString      _xName,     _yName;
      bool         _xEditable, _yEditable;
      symbol      *_xOch;
      CString      _xOchName;

      CString      _helpDlg;

      CString      _gaugeName;
      CAnalogMeter _gauge;
};

//------------------------------------------------------------------------------

class curveEditor : public dataDialog {
   private:
      int          _nId;
      curveDialog *_cd;
      Point3      *_line;
      plot3d      *_chart;
      bool         _gaugeDisplay;

      bool         _doGoto;

      void         drawGrid  ();
      void         drawCurve ();
      void         drawCursor();
      void         drawSpot  ();
      void         drawAll   ();

      CEdit       *_xBox[IDC_CEX16-IDC_CEX01+1];
      CEdit       *_yBox[IDC_CEY16-IDC_CEY01+1];

   public:
      curveEditor(int nId, CWnd *pParent=NULL);
     ~curveEditor();

      virtual bool doTimer(bool connected);

   public:
   //{{AFX_DATA(curveEditor)
      enum { IDD = IDD_CURVE_EDITOR };
      CStatic m_xLabel, m_xUnits;
      CStatic m_yLabel, m_yUnits;
      CStatic m_gaugeFrame;
      CStatic m_chartFrame;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(curveEditor)
   public:
      virtual BOOL PreTranslateMessage(MSG *pMsg);
   protected:
      virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL

   protected:
   //{{AFX_MSG(curveEditor)
      afx_msg HBRUSH OnCtlColor  (CDC *pDC, CWnd *pWnd, UINT nCtlColor);
      virtual BOOL   OnInitDialog();
      afx_msg void   OnKeyDown   (UINT nChar, UINT nRepCnt, UINT nFlags);
      afx_msg void   OnPaint     ();
      afx_msg void   OnSize      (UINT nType, int cx, int cy);
      afx_msg void   OnTableGet  ();
      afx_msg void   OnTablePut  ();
      afx_msg void   fieldsFromDb();
      afx_msg void   dbFromFields(UINT nId);
      afx_msg void   OnGenerate  ();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif
