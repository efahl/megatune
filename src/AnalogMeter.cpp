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

//*****************************************
//******** PC Configurator V1.00 **********
//*** (C) - 2001 B.Bowling/A. Grippo ******
//** All derivatives from this software ***
//**  are required to keep this header ****
//*****************************************

static char *rcsId() { return "$Id$"; }

#include "stdafx.h"
#include <math.h>
#include "AnalogMeter.h"

//#define ROUND(x) (((x<0.0)?-1:1)*int(fabs(x)))
#define ROUND(x) (int)((x) + 0.5 - (double)((x) < 0))
#define D2R(D)   ((D)*m_dPI/180.0)

//------------------------------------------------------------------------------

static HPALETTE s_hPalette   = NULL;

static COLORREF black = RGB(  0,   0,   0);
static COLORREF white = RGB(255, 255, 255);
static COLORREF red   = RGB(255,   0,   0);
static COLORREF green = RGB(  0, 255,   0);
static COLORREF yellow= RGB(255, 255,   0);
static COLORREF blue  = RGB(  0,   0, 255);

IMPLEMENT_DYNCREATE(CAnalogMeter, CCmdTarget)

COLORREF CAnalogMeter::nearestColor(COLORREF c)
{
#if 0
   //return m_dcGrid.GetNearestColor(c);
   CDC *dc = GetDC();
   c = dc->GetNearestColor(c);
   //CPalette *p = dc->GetCurrentPalette();
   //UINT idx = p->GetNearestPaletteIndex(c);
   //c = p->GetPaletteEntries(idx,1,NULL);
   ReleaseDC(dc);
#endif
   return (c & 0xFFFFFF); // | 0x02000000;
}

COLORREF CAnalogMeter::nearestInverse(COLORREF c)
{
   return nearestColor(~c);
}

//------------------------------------------------------------------------------

double CAnalogMeter::missingValue = -123456.789;

bool LoadBitmap(CString fileName, HBITMAP  &hBitmap, HPALETTE &hPalette);

CAnalogMeter::CAnalogMeter()
 : userData    (NULL)
 , m_gaugeFace ("")
 , m_hGaugeFace(NULL)
{
   m_dPI  = 4.0 * atan(1.0);  // for trig calculations
   m_dPI2 = m_dPI / 2.0;

   // initialized rectangle locations, will be modified on first drawing
   m_rectDraw    = CRect(0, 0, 0, 0);
   m_nRectWidth  = 0;
   m_nRectHeight = 0;

   // draw the whole thing the first time
   m_boolForceRedraw = true;
   m_dRadiansPerValue = 0.0;                // will be modified on first drawing

   // false if we are printing
   m_boolUseBitmaps = true;

   // default unit, scaling and needle position
   m_dMinScale   = -10.0;
   m_dMaxScale   =  10.0;
   m_dWarningLo  = -10.0;
   m_dWarningHi  =  10.0;
   m_dDangerLo   = -10.0;
   m_dDangerHi   =  10.0;
   m_dNeedlePos  =   0.0;
   m_dNeedleMax  = m_dMinScale;
   m_strTitle    = "";
   m_strUnit     = "";

   // for numerical values
   m_nFontScale     = 100;
   m_nRangeDecimals = 1;
   m_nValueDecimals = 1;

   // switches
   m_swTitle = true;
   m_swGrid  = true;
   m_swRange = true;
   m_swValue = true;
   m_swUnit  = true;

   // title color
   m_colorTitle     = RGB( 80,  80,  80);
   // normal grid color
   m_colorGrid      = RGB( 96,  96,  96);
   // alert grid color
   m_colorGridWarn  = yellow;
   m_colorGridDang  = red;
   // current numerical value color
   m_colorValue     = black;
   // background colors
   m_colorBGround   = RGB(202, 202, 202);
   m_colorWarning   = RGB(255, 255, 190);
   m_colorDanger    = RGB(255, 160, 160);
   m_colorFace      = white;
   // needle color
   m_colorNeedle    = RGB(155,   0,   0);
   // range color
   m_colorRange     = black;

   // set pen/brush colors
   ActuateColors();
}

//------------------------------------------------------------------------------

CAnalogMeter::~CAnalogMeter()
{
   //m_dcGrid.SelectObject(m_pbitmapOldGrid) ;
   //m_dcGrid.DeleteDC() ;

   //m_dcNeedle.SelectObject(m_pbitmapOldNeedle) ;
   //m_dcNeedle.DeleteDC() ;

   //m_bitmapGrid.DeleteObject() ;
   //m_bitmapNeedle.DeleteObject() ;
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CAnalogMeter, CCmdTarget)
   //{{AFX_MSG_MAP(CAnalogMeter)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

void CAnalogMeter::Reset()
{
   m_dNeedleMax = m_dNeedlePos;
}

//------------------------------------------------------------------------------

void CAnalogMeter::ShowMeter(CDC *pDC, CRect rectBorder, bool reset)
{
   if (reset) {
//    m_dNeedlePos = m_dMinScale - 1.0; // Force redraw on next UpdateNeedle
//    m_dNeedleMax = m_dNeedlePos;
      m_dNeedlePos = m_dNeedleMax;
   }

// check for a new meter or a resize of the old one.
   // (if the rectangles have changed, then redraw from scratch).
   // If we are printing, always draw from scratch without bitmaps.
// if (m_rectOwner != rectBorder)
      m_boolForceRedraw = true;

   if (m_boolForceRedraw || (pDC->IsPrinting())) {
      m_boolForceRedraw = false;
      // first store the rectangle for the owner
      // and determine the rectangle to draw to
      m_rectOwner = rectBorder;
      if (pDC->IsPrinting()) {
         m_boolUseBitmaps = false;
         m_rectDraw = m_rectOwner;                 // draw directly to the owner
      }
      else {
         m_boolUseBitmaps  = true;
         m_rectDraw.left   = 0;                    // draw to a bitmap rectangle
         m_rectDraw.top    = 0;
         m_rectDraw.right  = rectBorder.Width();
         m_rectDraw.bottom = rectBorder.Height();
      }
      m_nRectWidth  = m_rectDraw.Width();
      m_nRectHeight = m_rectDraw.Height();

      // if we already have a memory dc, destroy it
      // (this occurs for a re-size of the meter)
      if (m_dcGrid.GetSafeHdc()) {
         m_dcGrid.SelectObject(m_pbitmapOldGrid);
         m_dcGrid.DeleteDC();

         m_dcNeedle.SelectObject(m_pbitmapOldNeedle);
         m_dcNeedle.DeleteDC();

         m_bitmapGrid.DeleteObject();
         m_bitmapNeedle.DeleteObject();
      }

      if (m_boolUseBitmaps) {
         // create a memory based dc for drawing the grid
         m_dcGrid.CreateCompatibleDC(pDC);
         m_bitmapGrid.CreateCompatibleBitmap(pDC, m_nRectWidth, m_nRectHeight);
         m_pbitmapOldGrid = m_dcGrid.SelectObject(&m_bitmapGrid);

         // create a memory based dc for drawing the needle
         m_dcNeedle.CreateCompatibleDC(pDC);
         m_bitmapNeedle.CreateCompatibleBitmap(pDC, m_nRectWidth, m_nRectHeight);
         m_pbitmapOldNeedle = m_dcNeedle.SelectObject(&m_bitmapNeedle);
      }
      else {                              // no bitmaps, draw to the destination
         // use the destination dc for the grid
         m_dcGrid.m_hDC         = pDC->m_hDC;
         m_dcGrid.m_hAttribDC   = pDC->m_hAttribDC;

         // use the destination dc for the grid
         m_dcNeedle.m_hDC       = pDC->m_hDC;
         m_dcNeedle.m_hAttribDC = pDC->m_hAttribDC;
      }

      // draw the grid in the "grid dc"
      DrawGrid();
      // draw the needle in the "needle dc"
      DrawNeedle();
   }

   // display the new image, combining the needle with the grid
   if (m_boolUseBitmaps)
      ShowMeterImage(pDC);
}

//------------------------------------------------------------------------------

void CAnalogMeter::UpdateNeedle(CDC *pDC, double dPos)
{
   // do not support updates if we are not working with
   // bitmaps images
   if (!m_boolUseBitmaps) {
      MessageBox("Bitmaps not supported");
      return;
   }

   // must have created the grid if we are going to
   // update the needle (the needle locations are
   // calculated based on the grid)
   if (!m_dcGrid.GetSafeHdc()) return;

   // if the needle hasn't changed, don't bother updating
   if (m_dNeedlePos == dPos) return;

   // store the position in the member variable
   // for availability elsewhere
   m_dNeedlePos = dPos;

   // draw the new needle image
   DrawNeedle();

   // combine the needle with the grid and display the result
   if (m_boolUseBitmaps)
      ShowMeterImage(pDC);
}

//------------------------------------------------------------------------------

void CAnalogMeter::gridArc(int startDeg, int endDeg, CPen &pen, CBrush &brush)
{
   if (startDeg < -int(m_dLimitAngleDeg)) startDeg = -int(m_dLimitAngleDeg);
   if (startDeg >  int(m_dLimitAngleDeg)) startDeg =  int(m_dLimitAngleDeg);
   if (endDeg   < -int(m_dLimitAngleDeg)) endDeg   = -int(m_dLimitAngleDeg);
   if (endDeg   >  int(m_dLimitAngleDeg)) endDeg   =  int(m_dLimitAngleDeg);

   if (startDeg == endDeg) return;

   double a = D2R(double(startDeg));
   CPoint corner(
      ROUND(m_nCXPix + (1.00*m_nRadiusPix) *  sin(a)),
      ROUND(m_nCYPix + (1.00*m_nRadiusPix) * -cos(a))
   );

   m_dcGrid.SelectObject(pen);
   m_dcGrid.SelectObject(brush);
   m_dcGrid.BeginPath();
      m_dcGrid.MoveTo(corner);

      int d;
      for (d = int(startDeg); d <= int(endDeg); d++) {
         double a = D2R(double(d));
         m_dcGrid.LineTo(
            ROUND(m_nCXPix + (1.00*m_nRadiusPix) *  sin(a)),
            ROUND(m_nCYPix + (1.00*m_nRadiusPix) * -cos(a)));
      }

      for (d = int(endDeg); d >= int(startDeg); d--) {
         double a = D2R(double(d));
         m_dcGrid.LineTo(
            ROUND(m_nCXPix + (0.90*m_nRadiusPix) *  sin(a)),
            ROUND(m_nCYPix + (0.90*m_nRadiusPix) * -cos(a)));
      }

      m_dcGrid.LineTo(corner);
   m_dcGrid.EndPath();
   m_dcGrid.StrokeAndFillPath();
}

//------------------------------------------------------------------------------

void CAnalogMeter::DrawGrid()
{
   CFont         *pFontOld;
   CString        tempString;

   bool           disable_title;
   bool           disable_range;
   bool           disable_unit;

   m_rectGfx = m_rectDraw;

   ///////////////////////
   // clear background //
   ///////////////////////

   CPen   *pPenOld   = NULL;
   CBrush *pBrushOld = NULL;
   if (m_PenG_BGround.m_hObject  ) pPenOld   = m_dcGrid.SelectObject(&m_PenG_BGround);
   if (m_BrushG_BGround.m_hObject) pBrushOld = m_dcGrid.SelectObject(&m_BrushG_BGround);

   m_dcGrid.Rectangle(m_rectGfx);
//   m_rectGfx.DeflateRect(3, 3);
   m_rectGfx.DeflateRect(1, 1);
//   m_dcGrid.Draw3dRect(m_rectGfx, nearestInverse(white), nearestInverse(black));
//   m_rectGfx.DeflateRect(1, 1);
   m_dcGrid.Draw3dRect(m_rectGfx, nearestInverse(RGB(220, 220, 220)), nearestInverse(RGB(127, 127, 127)));
   m_rectGfx.DeflateRect(4, 4);

   ////////////////
   // check size //
   ////////////////
   disable_title = false;
   disable_range = false;
   disable_unit  = false;
   if ((m_rectGfx.Height() < 50) || (m_rectGfx.Width() < 50)) {
      disable_title = true;
      disable_range = true;
      disable_unit  = true;
   }
   if ((m_rectGfx.Height() < 20) || (m_rectGfx.Width() < 20))
      return;

   // pie angle
   if (!disable_range && m_swRange) {
      m_dLimitAngleDeg = 135.0;
   }
   else {
      m_dLimitAngleDeg = 150.0;
   }

   // make a square
   if (m_rectGfx.Height() > m_rectGfx.Width()) {
      m_rectGfx.DeflateRect(0, (m_rectGfx.Height() - m_rectGfx.Width()) / 2);
   }
   if (m_rectGfx.Height() < m_rectGfx.Width()) {
      m_rectGfx.DeflateRect((m_rectGfx.Width() - m_rectGfx.Height()) / 2, 0);
   }

     /////////////////
    // create font //
   /////////////////
   m_nFontHeight = m_rectGfx.Height() / 10;

   if (((m_rectGfx.Width()) > 0) && ((m_rectGfx.Height()) > 0)) {
      int     height = 0;
      int     width  = 0;
      double  scale  = 1.0;

      do {
         m_nFontHeight = (int) ((double) m_nFontHeight * scale);
         if (m_nFontHeight > 18) m_nFontHeight = 18;
         m_fontValue.DeleteObject();
         if (!m_fontValue.CreateFont(m_nFontHeight-1, 0, 0, 0, 400, // 400 is regular
                                     false, false, false, DEFAULT_CHARSET,
                                     OUT_OUTLINE_PRECIS,
                                     CLIP_DEFAULT_PRECIS,
                                     PROOF_QUALITY,
                                     DEFAULT_PITCH | FF_DONTCARE,
                                     "Verdana")) return;
         m_fontValueB.DeleteObject();
         if (!m_fontValueB.CreateFont(m_nFontHeight+1, 0, 0, 0, 600, // 600 is boldface
                                     false, false, false, DEFAULT_CHARSET,
                                     OUT_OUTLINE_PRECIS,
                                     CLIP_DEFAULT_PRECIS,
                                     PROOF_QUALITY,
                                     DEFAULT_PITCH | FF_DONTCARE,
                                     "Verdana")) return;

         tempString.Format("%.*f", m_nRangeDecimals, m_dMinScale);
         int len1 = tempString.GetLength();

         tempString.Format("%.*f", m_nRangeDecimals, m_dMaxScale);
         int len2 = tempString.GetLength();

         width = len1 > len2 ? len1 : len2;

         TEXTMETRIC TM;
         pFontOld = m_dcGrid.SelectObject(&m_fontValue);
         m_dcGrid.GetTextMetrics(&TM);
         height = TM.tmHeight;
         width  = TM.tmAveCharWidth * width;
         m_dcGrid.SelectObject(pFontOld);

         scale -= 0.01;

      } while (((height > (m_rectGfx.Height() / 8) * m_nFontScale / 100)
             || (width  > (m_rectGfx.Width()  / 3))) && (scale > 0.0));
   }

   // place for title
   if (!disable_title && m_swTitle && m_strTitle != "") {
      TEXTMETRIC TM;

      pFontOld = m_dcGrid.SelectObject(&m_fontValue);
      m_dcGrid.GetTextMetrics(&TM);
      m_rectGfx.top += TM.tmHeight + 3;
      m_rectGfx.DeflateRect(TM.tmHeight, 0);
      m_dcGrid.SelectObject(pFontOld);
   }

   // determine the angular scaling
   m_dLimitAngleRad   = D2R(m_dLimitAngleDeg);
   m_dRadiansPerValue = (2.0 * m_dLimitAngleRad) / (m_dMaxScale - m_dMinScale);

   // determine the center point
   m_nCXPix = m_rectGfx.left + m_rectGfx.Width() / 2;
   m_nCYPix = m_rectGfx.top + m_rectGfx.Height() / 2;

   // determine the size and location of the meter "pie"
   m_nRadiusPix = m_rectGfx.Height() / 2;
   m_nHalfBaseWidth = m_nRadiusPix / 50 + 1;

   int iX = ROUND(m_nRadiusPix * sin(m_dLimitAngleRad));
   m_ptFaceL.x = m_nCXPix - iX;
   m_ptFaceR.x = m_nCXPix + iX;

   m_ptFaceL.y = ROUND(m_nCYPix - m_nRadiusPix * cos(m_dLimitAngleRad));
   m_ptFaceR.y = m_ptFaceL.y;

   // determine the placement of the current value text
   m_rectValue.left   = m_rectGfx.left + 10;
   m_rectValue.top    = m_rectGfx.top + 10;
   m_rectValue.right  = m_rectGfx.right - 10;
// m_rectValue.bottom = m_nCYPix - m_nRadiusPix / 2;
   m_rectValue.bottom = m_nCYPix + m_nRadiusPix / 4; // efahl

   // determine the placement of the minimum value
   m_rectMinValue.left   = m_ptFaceL.x;
   m_rectMinValue.top    = m_ptFaceL.y + 1;
   m_rectMinValue.right  = m_rectGfx.left + m_rectGfx.Width() / 2 + 1;
   m_rectMinValue.bottom = m_rectGfx.bottom - m_rectGfx.Height() / 50;

   // determine the placement of the maximum value
   m_rectMaxValue.right  = m_ptFaceR.x;
   m_rectMaxValue.top    = m_ptFaceL.y + 1;
   m_rectMaxValue.left   = m_rectGfx.left + m_rectGfx.Width() / 2 + 1;
   m_rectMaxValue.bottom = m_rectGfx.bottom - m_rectGfx.Height() / 50;

   if (m_strUnit == "") m_rectValue.bottom += m_nFontHeight;
   else                 m_rectValue.bottom += m_nFontHeight / 2;

     ///////////////
    // draw grid //
   ///////////////
   if (m_swGrid) {
      int            tick;
      double         len;
      int            start_tick, end_tick;

      // alert arc start/end
      if (m_dAlertScale < m_dMinScale) m_dAlertScale = m_dMinScale;
      if (m_dAlertScale > m_dMaxScale) m_dAlertScale = m_dMaxScale;
      
      double endPct   = 1.0;
      double startPct = m_dMaxScale - m_dMinScale;
      if (startPct) {
         startPct = ((m_dAlertScale - m_dMinScale) / startPct) * 2.0 - 1.0;
      }
      else {
         startPct = 1;
      }
      endPct = 1.0;

      double range    = m_dMaxScale  - m_dMinScale;
      double ldangDeg  = m_dLimitAngleDeg - (m_dMaxScale-m_dDangerLo )/range * 2*m_dLimitAngleDeg;
      double lwarnDeg  = m_dLimitAngleDeg - (m_dMaxScale-m_dWarningLo)/range * 2*m_dLimitAngleDeg;
      double hwarnDeg  = m_dLimitAngleDeg - (m_dMaxScale-m_dWarningHi)/range * 2*m_dLimitAngleDeg;
      double hdangDeg  = m_dLimitAngleDeg - (m_dMaxScale-m_dDangerHi )/range * 2*m_dLimitAngleDeg;

      // alert tick start/end
      double nTicks = fabs(range);
      if (nTicks < 5) nTicks = 5;
      while (nTicks > 12.0) nTicks /= 10.0;
      if (nTicks != int(nTicks)) nTicks = 10.0;

      start_tick = ROUND(startPct * nTicks + (0.5 * ((double) (startPct > 0.0))));
      end_tick   = int(endPct * nTicks);

      if (m_hGaugeFace) {
         CDC dcImage;
         if (dcImage.CreateCompatibleDC(&m_dcGrid)) {
            dcImage.SelectObject(m_hGaugeFace);
            int hw = min(m_nRectWidth, m_nRectHeight);
            int l = hw == m_nRectWidth  ? 0 : (m_nRectWidth -hw)/2;
            int t = hw == m_nRectHeight ? 0 : (m_nRectHeight-hw)/2;
            BITMAP bm;
            GetObject(m_hGaugeFace, sizeof(BITMAP), &bm);
            m_dcGrid.StretchBlt(l, t, hw, hw, &dcImage, 0, 0, bm.bmWidth, bm.bmHeight, NOTSRCCOPY);
         }
      }
      else {
         m_dcGrid.SelectObject(&m_PenG_Grid);
         m_dcGrid.SelectObject(&m_BrushG_Face);

         // determine the bounding rectangle for the pie slice and draw it
         m_rectFace.left   = m_nCXPix - m_nRadiusPix;
         m_rectFace.right  = m_nCXPix + m_nRadiusPix;
         m_rectFace.top    = m_nCYPix - m_nRadiusPix;
         m_rectFace.bottom = m_nCYPix + m_nRadiusPix;

         // Make the gauge face, with outline in a contrast color.
         m_dcGrid.BeginPath();
            m_dcGrid.MoveTo(m_ptFaceL);
            for (int d = -int(m_dLimitAngleDeg); d <= int(m_dLimitAngleDeg); d+=1) {
               double a = D2R(double(d));
               m_dcGrid.LineTo(
                  ROUND(m_nCXPix + m_nRadiusPix *  sin(a)),
                  ROUND(m_nCYPix + m_nRadiusPix * -cos(a)));
            }
            m_dcGrid.LineTo(m_ptFaceL);
         m_dcGrid.EndPath();
         m_dcGrid.StrokeAndFillPath();

#define COLORBANDS 1
#if COLORBANDS
         gridArc(-ROUND(m_dLimitAngleDeg), ROUND(ldangDeg),         m_PenG_Grid, m_redBrush);
         gridArc( ROUND(ldangDeg),         ROUND(lwarnDeg),         m_PenG_Grid, m_BrushG_GridWarning);
         gridArc( ROUND(lwarnDeg),         ROUND(hwarnDeg),         m_PenG_Grid, m_BrushN_BGround);
         gridArc( ROUND(hwarnDeg),         ROUND(hdangDeg),         m_PenG_Grid, m_BrushG_GridWarning);
         gridArc( ROUND(hdangDeg),         ROUND(m_dLimitAngleDeg), m_PenG_Grid, m_redBrush);
#endif

         // tick marks
         m_dcGrid.SelectObject(&m_PenG_GridW);
         double tickInc = 1.0 / nTicks;
         for (tick = -int(nTicks); tick < start_tick; tick++) {
            double dX = m_nCXPix + m_nRadiusPix * sin(m_dLimitAngleRad * tick * tickInc);
            double dY = m_nCYPix - m_nRadiusPix * cos(m_dLimitAngleRad * tick * tickInc);
            m_dcGrid.MoveTo(ROUND(dX), ROUND(dY)); // Outer end first.
            len = (tick %2) ? 0.95 : 0.90;
            dX = m_nCXPix + len * m_nRadiusPix * sin(m_dLimitAngleRad * tick * tickInc);
            dY = m_nCYPix - len * m_nRadiusPix * cos(m_dLimitAngleRad * tick * tickInc);
            m_dcGrid.LineTo(ROUND(dX), ROUND(dY)); // Inner end.
         }

         // draw alert grid
         if ((startPct >= -1.0) && (startPct < 1.0)) {
            m_dcGrid.SelectObject(&m_PenG_GridAlert);
            m_dcGrid.SelectObject(&m_BrushG_GridAlert);

            // tick marks
            m_dcGrid.SelectObject(&m_PenG_GridAlert);
#if COLORBANDS
m_dcGrid.SelectObject(&m_PenG_Grid);
#endif
            for (tick = start_tick; tick <= end_tick; tick++) {
               double dX = m_nCXPix + m_nRadiusPix * sin(m_dLimitAngleRad * tick * tickInc);
               double dY = m_nCYPix - m_nRadiusPix * cos(m_dLimitAngleRad * tick * tickInc);
               m_dcGrid.MoveTo(ROUND(dX), ROUND(dY));
               if (tick % 2) len = 1.0;
               else len = 0.95;
               dX = m_nCXPix + 0.92 * len * m_nRadiusPix * sin(m_dLimitAngleRad * tick * tickInc);
               dY = m_nCYPix - 0.92 * len * m_nRadiusPix * cos(m_dLimitAngleRad * tick * tickInc);
               m_dcGrid.LineTo(ROUND(dX), ROUND(dY));
            }
         }
      }
   }

   // grab the font and set the text color
   pFontOld = m_dcGrid.SelectObject(&m_fontValue);
   m_dcGrid.SetTextColor(nearestInverse(m_colorRange));
   m_dcGrid.SetBkColor(nearestInverse(m_colorBGround));
   m_nTextBaseSpacing = m_rectMinValue.Height() / 4;

   if (!disable_title && m_swTitle && m_strTitle != "") {
      // show the title
      m_dcGrid.SetTextAlign(TA_CENTER | TA_BOTTOM);
      m_dcGrid.SetTextColor(nearestInverse(m_colorTitle));
      m_dcGrid.SetBkColor(nearestInverse(m_colorBGround));
      m_dcGrid.TextOut((m_rectGfx.left + m_rectGfx.right) / 2,
                       m_rectGfx.top - 3, m_strTitle);
   }

   if (!disable_range && m_swRange) {
      // show the max and min (limit) values
      m_dcGrid.SetTextColor(nearestInverse(m_colorRange));
      m_dcGrid.SetBkColor(nearestInverse(m_colorBGround));

      tempString.Format("%.*f", m_nRangeDecimals, m_dMinScale);
      m_dcGrid.SetTextAlign(TA_CENTER | TA_BASELINE);
      m_dcGrid.TextOut(m_rectMinValue.left,
                       m_rectMinValue.bottom - m_nTextBaseSpacing, tempString);
      tempString.Format("%.*f", m_nRangeDecimals, m_dMaxScale);
      m_dcGrid.SetTextAlign(TA_CENTER | TA_BASELINE);
      m_dcGrid.TextOut(m_rectMaxValue.right,
                       m_rectMaxValue.bottom - m_rectMaxValue.Height() / 4, tempString);
   }

   if (!disable_unit && m_swUnit && m_strUnit != "") {
      // show the unit
      m_dcGrid.SetTextAlign(TA_CENTER | TA_BOTTOM);
      m_dcGrid.SetTextColor(nearestInverse(m_colorValue));
      //qqq m_dcGrid.SetBkColor(nearestInverse(m_colorBGround));
      m_dcGrid.SetBkColor(nearestInverse(m_colorFace));
      m_dcGrid.TextOut((m_rectGfx.left + m_rectGfx.right) / 2,
                       //m_nCYPix - m_nRadiusPix / 2 + m_nFontHeight + m_nFontHeight / 2,
                       m_nCYPix + m_nRadiusPix / 4 + m_nFontHeight + m_nFontHeight / 2, // efahl
                       m_strUnit);
   }

   if (pPenOld  ) m_dcGrid.SelectObject(pPenOld  );
   if (pBrushOld) m_dcGrid.SelectObject(pBrushOld);
   m_dcGrid.SelectObject(pFontOld);
}

//------------------------------------------------------------------------------

void CAnalogMeter::needlePt(double angle, double radius, double &x, double &y)
{
   double dCosAngle = cos(angle);
   double dSinAngle = sin(angle);

   // tip
   x = m_nCXPix + radius * dSinAngle;
   y = m_nCYPix - radius * dCosAngle;
}

//------------------------------------------------------------------------------

void CAnalogMeter::DrawNeedle()
{
   CPoint         pPoints[6];
   CString        tempString;
   CFont         *pFontOld;
   double         dAngleRad;
   double         dCosAngle, dSinAngle;
   bool           disable_value;

   if (!m_dcNeedle.GetSafeHdc())
      return;

   CPen   *pPenOld   = NULL;
   CBrush *pBrushOld = NULL;
   if (m_PenN_BGround.m_hObject  ) pPenOld   = m_dcNeedle.SelectObject(&m_PenN_BGround);
   if (m_BrushN_BGround.m_hObject) pBrushOld = m_dcNeedle.SelectObject(&m_BrushN_BGround);

   if (m_boolUseBitmaps) {
      // new pen / brush
      m_dcNeedle.Rectangle(m_rectDraw);
   }
   
   //---------------------------------------------------------------------
   //--  Colored gauge face.  --------------------------------------------
   CBrush   *bgBrush = NULL;
   COLORREF *bgColor = &white;
   if (m_dNeedlePos <= m_dWarningLo || m_dNeedlePos >= m_dWarningHi) { bgBrush = &m_BrushG_GridWarning; bgColor = &m_colorWarning; }
   if (m_dNeedlePos <= m_dDangerLo  || m_dNeedlePos >= m_dDangerHi ) { bgBrush = &m_BrushG_GridDanger;  bgColor = &m_colorDanger;  }

   m_dcNeedle.SelectObject(&m_PenG_Face);
   m_dcNeedle.SelectObject(bgBrush);

   m_dcNeedle.BeginPath();
         m_dcNeedle.MoveTo(m_ptFaceL);
         for (int d = -int(m_dLimitAngleDeg); d <= int(m_dLimitAngleDeg); d+=5) {
            double a = D2R(double(d));
            m_dcNeedle.LineTo(
               ROUND(m_nCXPix + m_nRadiusPix *  sin(a)),
               ROUND(m_nCYPix + m_nRadiusPix * -cos(a)));
         }
         m_dcNeedle.LineTo(m_ptFaceL);
#if 0
          // Morons at microsoft only support using ArcTo in paths in win2k and later...
         m_dcNeedle.MoveTo(m_ptFaceL);
         m_dcNeedle.LineTo(m_ptFaceR);
         m_dcNeedle.ArcTo(m_rectFace, m_ptFaceR, m_ptFaceL);
#endif
   m_dcNeedle.EndPath();
   m_dcNeedle.FillPath();

   //---------------------------------------------------------------------
   /////////////////
   // check sizes //
   /////////////////
   disable_value = false;
   if ((m_rectGfx.Height() < 50) || (m_rectGfx.Width() < 50)) {
      disable_value = true;
   }
   if ((m_rectGfx.Height() < 20) || (m_rectGfx.Width() < 20))
      return;

   if (!disable_value && m_swValue) {
      pFontOld = m_dcNeedle.SelectObject(&m_fontValueB);
      m_dcNeedle.SetTextAlign(TA_CENTER | TA_BASELINE);
      m_dcNeedle.SetTextColor(nearestColor(m_colorValue ^ (*bgColor)));
      m_dcNeedle.SetBkColor(nearestInverse(*bgColor));

      if (m_dNeedlePos == missingValue)
         tempString = "---";
      else
         tempString.Format("%.*f", m_nValueDecimals, m_dNeedlePos);

      m_dcNeedle.TextOut((m_rectValue.right + m_rectValue.left) / 2,
                         m_rectValue.bottom - m_nTextBaseSpacing, tempString);
      m_dcNeedle.SelectObject(pFontOld);
   }

   dAngleRad = (m_dNeedlePos - m_dMinScale) * m_dRadiansPerValue - m_dLimitAngleRad;
   dAngleRad = max(dAngleRad, -m_dLimitAngleRad);
   dAngleRad = min(dAngleRad,  m_dLimitAngleRad);
   dCosAngle = cos(dAngleRad);
   dSinAngle = sin(dAngleRad);

   // tip
   double dX = m_nCXPix + m_nRadiusPix * dSinAngle;
   double dY = m_nCYPix - m_nRadiusPix * dCosAngle;
   needlePt(dAngleRad, m_nRadiusPix, dX, dY);
   pPoints[0].x = ROUND(dX);
   pPoints[0].y = ROUND(dY);

   // left base
   dX = m_nCXPix - m_nHalfBaseWidth * dCosAngle;
   dY = m_nCYPix - m_nHalfBaseWidth * dSinAngle;
   //needlePt(dAngleRad, m_nHalfBaseWidth, dX, dY);
   pPoints[1].x = ROUND(dX);
   pPoints[1].y = ROUND(dY);

   // right base
   pPoints[2].x = m_nCXPix + (m_nCXPix - pPoints[1].x);
   pPoints[2].y = m_nCYPix + (m_nCYPix - pPoints[1].y);

   // tip
   pPoints[3].x = pPoints[0].x;
   pPoints[3].y = pPoints[0].y;

   int baseOfs = 4*m_nHalfBaseWidth; // How far needle pivot is above gauge center.
   int tipR    = 2; // Radius of ball at needle tip.

   pPoints[1].y -= baseOfs;
   pPoints[2].y -= baseOfs;

   m_dcNeedle.SelectObject(&m_PenN_Needle);
   m_dcNeedle.SelectObject(&m_BrushN_Needle);

   // draw the needle pointer
   m_dcNeedle.Polygon(pPoints, 4);
   // draw circle at the bottom
   m_dcNeedle.Ellipse(m_nCXPix - m_nHalfBaseWidth    , -baseOfs+m_nCYPix - m_nHalfBaseWidth,
                      m_nCXPix + m_nHalfBaseWidth + 1, -baseOfs+m_nCYPix + m_nHalfBaseWidth + 1);

   m_dcNeedle.Ellipse(pPoints[0].x - tipR, pPoints[0].y - tipR,
                      pPoints[0].x + tipR, pPoints[0].y + tipR);

   if (m_dAlertScale < m_dMaxScale) {
#if 1 // Telltales
      if (m_dNeedlePos > m_dNeedleMax) m_dNeedleMax = m_dNeedlePos;

      dAngleRad = (m_dNeedleMax - m_dMinScale) * m_dRadiansPerValue - m_dLimitAngleRad;
      dAngleRad = max(dAngleRad, -m_dLimitAngleRad);
      dAngleRad = min(dAngleRad, m_dLimitAngleRad);

      needlePt(dAngleRad+m_dPI2, m_nHalfBaseWidth, dX, dY);
      m_dcNeedle.MoveTo(ROUND(dX), ROUND(dY-baseOfs));
      needlePt(dAngleRad, m_nRadiusPix, dX, dY);
      m_dcNeedle.LineTo(ROUND(dX), ROUND(dY));
      needlePt(dAngleRad-m_dPI2, m_nHalfBaseWidth, dX, dY);
      m_dcNeedle.LineTo(ROUND(dX), ROUND(dY-baseOfs));
#endif
      
   // Turn on alert LEDs.
//    if (m_dNeedlePos >= m_dAlertScale)
//       m_dcGrid.SelectObject(&m_BrushG_GridAlert);
//    else
//       m_dcGrid.SelectObject(&m_BrushG_BGround);
//    m_dcGrid.Ellipse(m_nRectWidth-23, m_nRectHeight-23, m_nRectWidth-10, m_nRectHeight-10);
   }

   // old pen / brush
   if (pPenOld  ) m_dcGrid.SelectObject(pPenOld);
   if (pBrushOld) m_dcGrid.SelectObject(pBrushOld);
}

//------------------------------------------------------------------------------

void CAnalogMeter::ShowMeterImage(CDC * pDC)
{
   CDC            memDC;
   CBitmap        memBitmap;
   CBitmap       *oldBitmap;    // bitmap originally found in CMemDC

   // this function is only used when the needle and grid
   // have been drawn to bitmaps and they need to be combined
   // and sent to the destination
   if (!m_boolUseBitmaps)
      return;

   // to avoid flicker, establish a memory dc, draw to it
   // and then BitBlt it to the destination "pDC"
   memDC.CreateCompatibleDC(pDC);
   memBitmap.CreateCompatibleBitmap(pDC, m_nRectWidth, m_nRectHeight);
   oldBitmap = (CBitmap *) memDC.SelectObject(&memBitmap);

   // make sure we have the bitmaps
   if (!m_dcGrid  .GetSafeHdc()) return;
   if (!m_dcNeedle.GetSafeHdc()) return;

   if (memDC.GetSafeHdc() != NULL) {
      memDC.BitBlt(0, 0, m_nRectWidth, m_nRectHeight, &m_dcGrid,   0, 0, NOTSRCCOPY); // draw the inverted grid
      memDC.BitBlt(0, 0, m_nRectWidth, m_nRectHeight, &m_dcNeedle, 0, 0, SRCINVERT ); // merge the needle image with the grid
      pDC->BitBlt(m_rectOwner.left, m_rectOwner.top, m_nRectWidth, m_nRectHeight, &memDC, 0, 0, SRCCOPY);
   }

   memDC.SelectObject(oldBitmap);

}

//------------------------------------------------------------------------------

void CAnalogMeter::ActuateColors()
{
   if (m_PenG_Grid.m_hObject               ) m_PenG_Grid.DeleteObject();
   if (m_PenG_Grid.m_hObject == NULL       ) m_PenG_Grid.CreatePen(PS_SOLID, 0, nearestInverse(m_colorGrid));
   if (m_PenG_GridW.m_hObject              ) m_PenG_GridW.DeleteObject();
   if (m_PenG_GridW.m_hObject == NULL      ) m_PenG_GridW.CreatePen(PS_SOLID, 1, nearestInverse(m_colorGrid));
   if (m_BrushG_Grid.m_hObject             ) m_BrushG_Grid.DeleteObject();
   if (m_BrushG_Grid.m_hObject == NULL     ) m_BrushG_Grid.CreateSolidBrush(nearestInverse(m_colorGrid));

   if (m_redBrush.m_hObject == NULL        ) m_redBrush.CreateSolidBrush(nearestInverse(red));

   if (m_PenG_GridAlert.m_hObject          ) m_PenG_GridAlert.DeleteObject();
   if (m_PenG_GridAlert.m_hObject == NULL  ) m_PenG_GridAlert.CreatePen(PS_SOLID, 1, nearestInverse(m_colorGridDang));
   if (m_PenG_GridAlertW.m_hObject         ) m_PenG_GridAlertW.DeleteObject();
   if (m_PenG_GridAlertW.m_hObject == NULL ) m_PenG_GridAlertW.CreatePen(PS_SOLID, 1, nearestInverse(m_colorGridWarn));
   if (m_BrushG_GridAlert.m_hObject        ) m_BrushG_GridAlert.DeleteObject();
   if (m_BrushG_GridAlert.m_hObject == NULL) m_BrushG_GridAlert.CreateSolidBrush(nearestInverse(m_colorGridDang));

   if (m_BrushG_GridWarning.m_hObject        ) m_BrushG_GridWarning.DeleteObject();
   if (m_BrushG_GridWarning.m_hObject == NULL) m_BrushG_GridWarning.CreateSolidBrush(nearestInverse(m_colorWarning));
   if (m_BrushG_GridDanger.m_hObject        ) m_BrushG_GridDanger.DeleteObject();
   if (m_BrushG_GridDanger.m_hObject == NULL) m_BrushG_GridDanger.CreateSolidBrush(nearestInverse(m_colorDanger));

   if (m_PenG_BGround.m_hObject            ) m_PenG_BGround.DeleteObject();
   if (m_PenG_BGround.m_hObject == NULL    ) m_PenG_BGround.CreatePen(PS_SOLID, 1, nearestInverse(m_colorBGround));
   if (m_BrushG_BGround.m_hObject          ) m_BrushG_BGround.DeleteObject();
   if (m_BrushG_BGround.m_hObject == NULL  ) m_BrushG_BGround.CreateSolidBrush(nearestInverse(m_colorBGround));

   if (m_PenG_Face.m_hObject               ) m_PenG_Face.DeleteObject();
   if (m_PenG_Face.m_hObject == NULL       ) m_PenG_Face.CreatePen(PS_SOLID, 1, nearestInverse(m_colorBGround));
   if (m_BrushG_Face.m_hObject             ) m_BrushG_Face.DeleteObject();
   if (m_BrushG_Face.m_hObject == NULL     ) m_BrushG_Face.CreateSolidBrush(nearestInverse(m_colorFace));

   if (m_PenN_Needle.m_hObject             ) m_PenN_Needle.DeleteObject();
   if (m_PenN_Needle.m_hObject == NULL     ) m_PenN_Needle.CreatePen(PS_SOLID, 0, nearestInverse((~m_colorNeedle) ^m_colorBGround));
   if (m_BrushN_Needle.m_hObject           ) m_BrushN_Needle.DeleteObject();
   if (m_BrushN_Needle.m_hObject == NULL   ) m_BrushN_Needle.CreateSolidBrush(nearestInverse((~m_colorNeedle) ^ m_colorBGround));

   if (m_PenN_BGround.m_hObject            ) m_PenN_BGround.DeleteObject();
   if (m_PenN_BGround.m_hObject == NULL    ) m_PenN_BGround.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
   if (m_BrushN_BGround.m_hObject          ) m_BrushN_BGround.DeleteObject();
   if (m_BrushN_BGround.m_hObject == NULL  ) m_BrushN_BGround.CreateSolidBrush(nearestColor(RGB(0, 0, 0)));
}

//------------------------------------------------------------------------------

void CAnalogMeter::SetState(enum Member meter_member, bool State)
{
   switch (meter_member) {
      case meter_title: m_swTitle = State; break;
      case meter_grid : m_swGrid  = State; break;
      case meter_value: m_swValue = State; break;
      case meter_range: m_swRange = State; break;
      case meter_unit : m_swUnit  = State; break;
   }
}

//------------------------------------------------------------------------------

void CAnalogMeter::SetColor(enum Member meter_member, COLORREF Color)
{
   switch (meter_member) {
      case meter_title    : m_colorTitle     = Color; break;
      case meter_needle   : m_colorNeedle    = Color; break;
      case meter_grid     : m_colorGrid      = Color; break;
      case meter_gridalert: m_colorGridDang  = Color; break;
      case meter_value    : m_colorValue     = Color; break;
      case meter_range    : m_colorRange     = Color; break;
      case meter_bground  : m_colorBGround   = Color; break;
      case meter_bgwarn   : m_colorWarning   = Color; break;
      case meter_bgdang   : m_colorDanger    = Color; break;
      case meter_face     : m_colorFace      = Color; break;
   }

   // set pen/brush colors
   ActuateColors();
}

//------------------------------------------------------------------------------

void CAnalogMeter::SetRange(double dMin, double dMax)
{
   // The function does NOT force the re-drawing of the meter.
   // The owner must explicitly call the ShowMeter function
   m_dMinScale  = dMin;
   m_dMaxScale  = dMax;
   m_dNeedleMax = dMin;
   m_boolForceRedraw = true;
}

//------------------------------------------------------------------------------

//void CAnalogMeter::SetAlert(double dAlert)
//{
//   // The function does NOT force the re-drawing of the meter.
//   // The owner must explicitly call the ShowMeter function
//   if (dAlert < m_dMinScale) dAlert = m_dMinScale;
//   if (dAlert > m_dMaxScale) dAlert = m_dMaxScale;
//   m_dAlertScale = dAlert;
//   m_boolForceRedraw = true;
//}

//------------------------------------------------------------------------------

void CAnalogMeter::SetRanges(double loW, double hiW, double loD, double hiD)
{
   // The function does NOT force the re-drawing of the meter.
   // The owner must explicitly call the ShowMeter function
   m_dWarningLo      = loW;
   m_dWarningHi      = hiW;
   m_dDangerLo       = loD;
   m_dDangerHi       = hiD;
   m_dAlertScale     = hiD;
   m_boolForceRedraw = true;
}

//------------------------------------------------------------------------------

void CAnalogMeter::SetFontScale(int nFontScale)
{
   // The function does NOT force the re-drawing of the meter.
   // The owner must explicitly call the ShowMeter function
   if (m_nFontScale < 1) m_nFontScale = 1;
   if (m_nFontScale > 100) m_nFontScale = 100;
   m_nFontScale = nFontScale;
   m_boolForceRedraw = true;
}

//------------------------------------------------------------------------------

void CAnalogMeter::SetRangeDecimals(int nRangeDecimals)
{
   // The function does NOT force the re-drawing of the meter.
   // The owner must explicitly call the ShowMeter function
   if (m_nRangeDecimals < 0) m_nRangeDecimals = 0;
   if (m_nRangeDecimals > 100) m_nRangeDecimals = 100;
   m_nRangeDecimals = nRangeDecimals;
   m_boolForceRedraw = true;
}

//------------------------------------------------------------------------------

void CAnalogMeter::SetValueDecimals(int nValueDecimals)
{
   // The function does NOT force the re-drawing of the meter.
   // The owner must explicitly call the ShowMeter function
   if (m_nValueDecimals < 0) m_nValueDecimals = 0;
   if (m_nValueDecimals > 100) m_nValueDecimals = 100;
   m_nValueDecimals = nValueDecimals;
   m_boolForceRedraw = true;
}

//------------------------------------------------------------------------------

void CAnalogMeter::SetTitle(CString strTitle)
{
   // The function does NOT force the re-drawing of the meter.
   // The owner must explicitly call the ShowMeter function
   m_strTitle = strTitle;
   m_boolForceRedraw = true;
}

//------------------------------------------------------------------------------

void CAnalogMeter::SetUnit(CString strUnit)
{
   // The function does NOT force the re-drawing of the meter.
   // The owner must explicitly call the ShowMeter function
   m_strUnit = strUnit;
   m_boolForceRedraw = true;
}

//------------------------------------------------------------------------------

void CAnalogMeter::setFaceBmp(CString fileName)
{
   if (!fileName.IsEmpty()) {
      m_gaugeFace = fileName;
      m_boolForceRedraw = true;
      if (m_hGaugeFace != NULL) DeleteObject(m_hGaugeFace);
      LoadBitmap(m_gaugeFace, m_hGaugeFace, s_hPalette);
   }
}

//------------------------------------------------------------------------------

void CAnalogMeter::GetColor(enum Member meter_member, COLORREF * pColor)
{
   switch (meter_member) {
      case meter_title    : *pColor = m_colorTitle;     break;
      case meter_needle   : *pColor = m_colorNeedle;    break;
      case meter_grid     : *pColor = m_colorGrid;      break;
      case meter_gridalert: *pColor = m_colorGridDang;  break;
      case meter_value    : *pColor = m_colorValue;     break;
      case meter_range    : *pColor = m_colorRange;     break;
      case meter_bground  : *pColor = m_colorBGround;   break;
   }
}

//------------------------------------------------------------------------------

void CAnalogMeter::GetState(enum Member meter_member, bool * pState)
{
   switch (meter_member) {
      case meter_title: *pState = m_swTitle; break;
      case meter_grid : *pState = m_swGrid;  break;
      case meter_value: *pState = m_swValue; break;
      case meter_range: *pState = m_swRange; break;
      case meter_unit : *pState = m_swUnit;  break;
   }
}

//------------------------------------------------------------------------------
