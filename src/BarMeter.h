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

#ifndef BARMETER_H
#define BARMETER_H 1

class CBarMeter : public CEdit {
   COLORREF nearestColor(COLORREF c);

public:
   const void *userData;

   //DECLARE_DYNCREATE(CBarMeter)
   CBarMeter();
   virtual ~CBarMeter();

   enum MeterMemberEnum {
      meter_title,
      meter_needle,
      meter_grid,
      meter_gridalert,
      meter_value,
      meter_range,
      meter_unit,
      meter_bground,
   };

   void    ShowMeter   (CDC * pDC, CRect rectBorder);
   void    UpdateNeedle(CDC * pDC, double dPos);

   void    SetColor(enum MeterMemberEnum meter_member, COLORREF Color);
   void    SetRange(double dMin, double dMax);
   void    SetAlert(double dAlert);

   //{{AFX_VIRTUAL(CBarMeter)
   //}}AFX_VIRTUAL

protected:
   void           DrawGrid();
   void           DrawNeedle();
   void           ShowMeterImage(CDC * pDC);
   void           ActuateColors();

   COLORREF       m_colorTitle;
   COLORREF       m_colorNeedle;
   COLORREF       m_colorGrid;
   COLORREF       m_colorGridAlert;
   COLORREF       m_colorBGround;

   bool           m_boolUseBitmaps;
   bool           m_boolForceRedraw;

   int            m_nRectWidth;
   int            m_nRectHeight;
   int            m_nCXPix;
   int            m_nCYPix;

   double         m_dNeedlePos;
   double         m_dMinScale;
   double         m_dMaxScale;
   double         m_dAlertScale;

   CDC            m_dcGrid;
   CBitmap        m_bitmapGrid;
   CBitmap       *m_pbitmapOldGrid;

   CDC            m_dcNeedle;
   CBitmap        m_bitmapNeedle;
   CBitmap       *m_pbitmapOldNeedle;

   CRect          m_rectDraw;
   CRect          m_rectGfx;
   CRect          m_rectOwner;

   CPen           m_PenG_Grid;
   CBrush         m_BrushG_Grid;
   CPen           m_PenG_GridAlert;
   CBrush         m_BrushG_GridAlert;
   CPen           m_PenG_BGround;
   CBrush         m_BrushG_BGround;

   CPen           m_PenN_Needle;
   CBrush         m_BrushN_Needle;
   CPen           m_PenN_BGround;
   CBrush         m_BrushN_BGround;

   //{{AFX_MSG(CBarMeter)
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

//------------------------------------------------------------------------------
#endif
