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
#ifndef DTTUNE_H
#define DTTUNE_H

#if _MSC_VER >= 1000
#  pragma once
#endif

#include "AnalogMeter.h"
#include "BarMeter.h"
#include "plot3d.h"

//------------------------------------------------------------------------------

class tuning3D;

struct autoTuneParameters {
      // Vertex tolerance parameters
      double      xRadius;
      double      yRadius;

      // Block tune parameters
      double      xMin, xMax;
      double      yMin, yMax;
      double      zMin, zMax;

      // Controller parameters
      symbol     *corrector; // Correcting variable, in whole percent.
      CString     correctorName;

      double      t0;         // Initial startup interval
      double      dT;         // Time-base
      double      pGain;      // Proportional gain
      double      lumpiness;  // Limit as to how much this vertex can deviate from surrounding ones.

      autoTuneParameters();

      bool insideWindow(double x, double y);
      bool nearVertex  (double x, double y, double vX, double vY);

      void setAutoTune(bool);
      bool autoTune();

   private:
      bool        _allowAutoTune;
};

struct tuningDialog {
   enum { nUT = 20 };
   CString     title;
   int         pageNo;

   symbol      *x,        *y,        *z;
   CString      xName,     yName,     zName;
   bool         xEditable, yEditable, zEditable;
   symbol      *xOch,     *yOch;
   CString      xOchName,  yOchName;
   int          iXBIN,     iYBIN,     iZVAL; // Tuning point parameters.
   double       vXBIN,     vYBIN,     vZVAL;

   //--  Grid display characteristics  -----------------------------------------
   double gridScale;
   CString upLabel;
   CString dnLabel;

   double xRot, yRot, zRot;
   bool   flat;

   autoTuneParameters atp;

   tuning3D *pTune;

   tuningDialog()
    : title        (""   )
    , xOchName     (""   )
    , yOchName     (""   )
    , pTune        (NULL )
    , pageNo       ( -1  )
    , gridScale    (  1.0)
    , xEditable    (true )
    , yEditable    (true )
    , zEditable    (true )
    , xRot         (250.0)
    , yRot         (  0.0)
    , zRot         (340.0)
    , flat         (false)
    , iXBIN        (  0  )
    , iYBIN        (  0  )
    , iZVAL        (  0  )
   {
   }

   bool addTuning   () { return true; }
   bool addSettings () { return true; }

   double      XBIN (int ix);
   double      YBIN (int iy);
   double      ZVAL (int ix, int iy);
   const char *ZSTR (int ix, int iy);
   int        _iZVAL(int ix, int iy);
   int         nX() { return x->nValues(); }
   int         nY() { return y->nValues(); }
   int         nZ() { return z->nValues(); }

   double interpolate     (double X, double Y);
   void   reset           () { iXBIN = iYBIN = iZVAL = 0; }
   void   update          ();
   bool   nearestCellMoved();


   void setIds(CString l, CString u) {
      _tableId  = l;
      _tuningId = u;
   }
   void setHelp(CString l, CString u) {
      _tableHelp = l;
      _tuningHelp = u;
   }
   CString tableId   () { return _tableId;    }
   CString tuningId  () { return _tuningId;   }
   CString tableHelp () { return _tableHelp;  }
   CString tuningHelp() { return _tuningHelp; }

   private:
      CString     _tableId;
      CString     _tuningId;

      CString     _tableHelp;
      CString     _tuningHelp;
};

//------------------------------------------------------------------------------

class tuning3D : public dataDialog {
   int          nId;
   tuningDialog *td;

   CMenu        *_settings;

   long    lgr;
   long    dgr;
   CBrush  brushbg;
   CRect   msgRect;

   double  minXBIN, maxXBIN;
   double  minYBIN, maxYBIN;

public:
   tuning3D(int nId, CWnd *pParent=NULL);
  ~tuning3D();

   virtual bool doTimer(bool connected);

   //{{AFX_DATA(tuning3D)
      enum { IDD = IDD_TUNING };
   //}}AFX_DATA

   //{{AFX_VIRTUAL(tuning3D)
   public:
      virtual BOOL PreTranslateMessage(MSG* pMsg);
   protected:
      virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
      virtual void DoDataExchange(CDataExchange * pDX);
   //}}AFX_VIRTUAL

private:
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
   void  autoTune();
   void  zeroGauges();

   bool    autoTuning; // Auto-tuning is enabled.
   bool    chasing;    // Chase mode does an automatic "F" at timer events.

   int     wux1;
   int     wux2;
   int     wuy1;
   int     wuy2;

protected:
   //{{AFX_MSG(tuning3D)
      virtual BOOL   OnInitDialog();
      afx_msg void   OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
      afx_msg void   OnPaint();
      afx_msg void   OnTableGet();
      afx_msg void   OnTablePut();
      afx_msg void   OnVeExport  ();
      afx_msg void   OnVeImport  ();
      afx_msg void   OnPageA();
      afx_msg void   OnPageB();
      afx_msg void   OnPageC();
      afx_msg void   OnPageD();
      afx_msg void   OnAutoTune();
      afx_msg void   OnDestroy();
      afx_msg void   OnSize(UINT nType, int cx, int cy);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

tuning3D *userTuning(UINT nId); // The real entry point.

//{{AFX_INSERT_LOCATION}}

#endif // DTTUNE_H
