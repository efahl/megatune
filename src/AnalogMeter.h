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

#ifndef ANALOGMETER_H
#define ANALOGMETER_H 1

//------------------------------------------------------------------------------

//*****************************************
//******** PC Configurator V1.00 **********
//*** (C) - 2001 B.Bowling/A. Grippo ******
//** All derivatives from this software ***
//**  are required to keep this header ****
//*****************************************

class CAnalogMeter : public CEdit {
public:
   const void *userData;

   static double missingValue;
   enum Member {
      meter_title,
      meter_needle,
      meter_grid,
      meter_gridalert,
      meter_value,
      meter_range,
      meter_unit,
      meter_bground,
      meter_bgwarn,
      meter_bgdang,
      meter_face
   };

   DECLARE_DYNCREATE(CAnalogMeter)
public:
   CAnalogMeter();
   virtual ~CAnalogMeter();

private:
   COLORREF nearestColor  (COLORREF c);
   COLORREF nearestInverse(COLORREF c);

public:
   void    ShowMeter   (CDC *pDC, CRect rectBorder, bool reset=false);
   void    UpdateNeedle(CDC *pDC, double dPos);
   void    Reset();

   void    SetColor(enum Member meter_member, COLORREF Color);
   void    SetState(enum Member meter_member, bool State);
   void    SetRange(double dMin, double dMax);
   void    SetRanges(double loW, double hiW, double loD, double hiD);
   void    SetFontScale(int nFontScale);
   void    SetRangeDecimals(int nRangeDecimals);
   void    SetValueDecimals(int nValueDecimals);
   void    SetTitle(CString strTitle);
   void    SetUnit(CString strUnit);
   void    setFaceBmp(CString fileName);


   void    GetColor(enum Member meter_member, COLORREF * pColor);
   void    GetState(enum Member meter_member, bool * pState);
   double  GetMinRange()      { return m_dMinScale;      }
   double  GetMaxRange()      { return m_dMaxScale;      }
   int     GetFontScale()     { return m_nFontScale;     }
   int     GetRangeDecimals() { return m_nRangeDecimals; }
   int     GetValueDecimals() { return m_nValueDecimals; }

   CString GetTitle() { return m_strTitle; }
   CString GetUnit()  { return m_strUnit;  }

protected:
   void           needlePt(double angle, double radius, double &x, double &y);
   void           DrawGrid();
   void           DrawNeedle();
   void           ShowMeterImage(CDC * pDC);
   void           ActuateColors();
   void           gridArc(int startDeg, int endDeg, CPen &pen, CBrush &brush);


   bool           m_swTitle;
   bool           m_swGrid;
   bool           m_swValue;
   bool           m_swRange;
   bool           m_swUnit;

   COLORREF       m_colorTitle;
   COLORREF       m_colorNeedle;
   COLORREF       m_colorGrid;
   COLORREF       m_colorGridDang;
   COLORREF       m_colorGridWarn;
   COLORREF       m_colorValue;
   COLORREF       m_colorRange;
   COLORREF       m_colorBGround;
   COLORREF       m_colorFace;
   COLORREF       m_colorWarning;
   COLORREF       m_colorDanger;

   bool           m_boolUseBitmaps;
   bool           m_boolForceRedraw;

   int            m_nFontScale;
   int            m_nRangeDecimals;
   int            m_nValueDecimals;
   int            m_nRectWidth;
   int            m_nRectHeight;
   int            m_nCXPix;
   int            m_nCYPix;
   int            m_nRadiusPix;
   int            m_nHalfBaseWidth;
   int            m_nTextBaseSpacing;
   int            m_nFontHeight;


   double         m_dPI;
   double         m_dPI2;
   double         m_dLimitAngleDeg;
   double         m_dLimitAngleRad;
   double         m_dRadiansPerValue;
   double         m_dNeedlePos;
   double         m_dNeedleMax;
   double         m_dMinScale;
   double         m_dMaxScale;
   double         m_dAlertScale;
   double         m_dWarningLo;
   double         m_dWarningHi;
   double         m_dDangerLo;
   double         m_dDangerHi;


   CString        m_strTitle;
   CString        m_strUnit;
   CString        m_gaugeFace;
   HBITMAP        m_hGaugeFace;

   CDC            m_dcGrid;
   CBitmap        m_bitmapGrid;
   CBitmap       *m_pbitmapOldGrid;

   CDC            m_dcNeedle;
   CBitmap        m_bitmapNeedle;
   CBitmap       *m_pbitmapOldNeedle;

   CRect          m_rectDraw;
   CRect          m_rectGfx;
   CRect          m_rectOwner;
   CRect          m_rectMinValue;
   CRect          m_rectMaxValue;
   CRect          m_rectValue;
   CRect          m_rectFace;
   CPoint         m_ptFaceL;
   CPoint         m_ptFaceR;

   CFont          m_fontValue;
   CFont          m_fontValueB;

   CPen           m_PenG_Grid;
   CPen           m_PenG_GridW; // Wide pen.
   CBrush         m_BrushG_Grid;
   CPen           m_PenG_GridAlert;
   CPen           m_PenG_GridAlertW;
   CBrush         m_BrushG_GridAlert;
   CBrush         m_BrushG_GridWarning;
   CBrush         m_BrushG_GridDanger;
   CPen           m_PenG_BGround;
   CBrush         m_BrushG_BGround;
   CPen           m_PenG_Face;
   CBrush         m_BrushG_Face;
   CBrush         m_redBrush;

   CPen           m_PenN_Needle;
   CBrush         m_BrushN_Needle;
   CPen           m_PenN_BGround;
   CBrush         m_BrushN_BGround;

   DECLARE_MESSAGE_MAP()
};

//------------------------------------------------------------------------------
#endif
