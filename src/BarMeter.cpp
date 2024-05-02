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

static char *rcsId() { return "$Id$"; }

#include "stdafx.h"
#include <math.h>
#include "BarMeter.h"

#define ROUND(x) (int)((x) + 0.5 - (double)((x) < 0))

//------------------------------------------------------------------------------

//IMPLEMENT_DYNCREATE(CBarMeter, CCmdTarget)

COLORREF CBarMeter::nearestColor(COLORREF c)
{
#if 000
   return m_dcGrid.GetNearestColor(c);
   CDC *dc = GetDC();
   c = dc->GetNearestColor(c);
   ReleaseDC(dc);
#endif
   return c;
}

//------------------------------------------------------------------------------

CBarMeter::CBarMeter()
 : userData(NULL)
{
   // initialized rectangle locations, will be modified on first drawing
   m_rectDraw    = CRect(0, 0, 0, 0);
   m_nRectWidth  = 0;
   m_nRectHeight = 0;

   // draw the whole thing the first time
   m_boolForceRedraw = true;

   // false if we are printing
   m_boolUseBitmaps = true;

   // default unit, scaling and needle position
   m_dMinScale   = -10.0;
   m_dMaxScale   =  10.0;
   m_dAlertScale =  10.0;
   m_dNeedlePos  =   0.0;

   m_colorTitle     = RGB( 80,  80,  80);
   m_colorGrid      = ::GetSysColor(COLOR_3DFACE);
   m_colorGridAlert = RGB(255,   0,   0);
   m_colorBGround   = ::GetSysColor(COLOR_3DFACE);
   m_colorNeedle    = RGB(  0, 155,   0);

   // set pen/brush colors
   ActuateColors();
}

//------------------------------------------------------------------------------
CBarMeter::~CBarMeter()
{
   //m_dcGrid.SelectObject(m_pbitmapOldGrid) ;
   //m_dcGrid.DeleteDC() ;

   //m_dcNeedle.SelectObject(m_pbitmapOldNeedle) ;
   //m_dcNeedle.DeleteDC() ;

   //m_bitmapGrid.DeleteObject() ;
   //m_bitmapNeedle.DeleteObject() ;
}

//------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CBarMeter, CCmdTarget)
   //{{AFX_MSG_MAP(CBarMeter)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


//------------------------------------------------------------------------------

void CBarMeter::ShowMeter(CDC * pDC, CRect rectBorder)
{
// check for a new meter or a resize of the old one.
   // (if the rectangles have changed, then redraw from scratch).
   // If we are printing, always draw from scratch without bitmaps.
   if (m_rectOwner != rectBorder)
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
         m_dcGrid.m_hDC = pDC->m_hDC;
         m_dcGrid.m_hAttribDC = pDC->m_hAttribDC;

         // use the destination dc for the grid
         m_dcNeedle.m_hDC = pDC->m_hDC;
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
void CBarMeter::UpdateNeedle(CDC * pDC, double dPos)
{
   // do not support updates if we are not working with
   // bitmaps images
   if (!m_boolUseBitmaps) {
      MessageBox("Bitmaps not supported");
      return;
   }

   // must have created the grid if we are going to
   // update the needle (the needle locations are
   // calculateed based on the grid)
   if (!m_dcGrid.GetSafeHdc())
      return;

   // if the needle hasn't changed, don't bother updating
   if (m_dNeedlePos == dPos)
      return;

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

void CBarMeter::DrawGrid()
{
   m_rectGfx = m_rectDraw;

   CPen   *pPenOld   = NULL;
   CBrush *pBrushOld = NULL;
   if (m_PenG_BGround.m_hObject  ) pPenOld   = m_dcGrid.SelectObject(&m_PenG_BGround);
   if (m_BrushG_BGround.m_hObject) pBrushOld = m_dcGrid.SelectObject(&m_BrushG_BGround);

   m_dcGrid.Rectangle(m_rectGfx);
   m_rectGfx.DeflateRect(4, 4);
}

//------------------------------------------------------------------------------

void CBarMeter::DrawNeedle()
{
   CPen          *pPenOld;
   CBrush        *pBrushOld;

   if (!m_dcNeedle.GetSafeHdc()) return;

   if (m_boolUseBitmaps) {
      pPenOld   = NULL;
      pBrushOld = NULL;
      if (m_PenN_BGround.m_hObject  ) pPenOld   = m_dcNeedle.SelectObject(&m_PenN_BGround);
      if (m_BrushN_BGround.m_hObject) pBrushOld = m_dcNeedle.SelectObject(&m_BrushN_BGround);
         m_dcNeedle.Rectangle(m_rectDraw);
      if (pPenOld  ) m_dcGrid.SelectObject(pPenOld);
      if (pBrushOld) m_dcGrid.SelectObject(pBrushOld);
   }

   m_dcNeedle.FillRect(m_rectDraw, NULL); // Clear the background before painting.

   if ((m_rectGfx.Height() < 10) || (m_rectGfx.Width() < 10)) return;

   // Save old pen and brush.
   pPenOld   = NULL;
   pBrushOld = NULL;
   if (m_PenN_Needle.m_hObject  ) pPenOld   = m_dcGrid.SelectObject(&m_PenN_Needle);
   if (m_BrushN_Needle.m_hObject) pBrushOld = m_dcGrid.SelectObject(&m_BrushN_Needle);

   // Turn on alert LEDs.
   if (m_nRectWidth > m_nRectHeight) { // Horizontal bar.
      int t      = m_rectGfx.top;
      int d      = m_rectGfx.bottom - t;
      int nLEDs  = (m_nRectWidth / (d+2) / 2) * 2;
      int l      = m_rectGfx.left + (m_rectGfx.right-m_rectGfx.left)/2 - int(double(nLEDs)/2.0*(d+2));
      int nAlert = int(nLEDs * (m_dAlertScale-m_dMinScale) / (m_dMaxScale-m_dMinScale));
      int nOn    = int(nLEDs * (m_dNeedlePos -m_dMinScale) / (m_dMaxScale-m_dMinScale));
      for (int iLED = 0; iLED < nLEDs; iLED++) {
         if (iLED >= nAlert) {
            m_dcGrid.SelectObject(&m_BrushG_GridAlert);
            m_dcGrid.SelectObject(&m_PenG_GridAlert);
         }
         if (iLED >  nOn   ) m_dcGrid.SelectObject(&m_BrushG_BGround);
         m_dcGrid.Ellipse(l, t, l+d, t+d);
         l += d+2;
      }
   }
   else { // Vertical bar.
      int l      = m_rectGfx.left;
      int d      = m_rectGfx.right - l;
      int nLEDs  = (m_nRectHeight / (d+2) / 2) * 2;
      int b      = m_rectGfx.bottom - ((m_rectGfx.bottom-m_rectGfx.top)/2 - int(double(nLEDs)/2.0*(d+2)));
      int nAlert = int(nLEDs * (m_dAlertScale-m_dMinScale) / (m_dMaxScale-m_dMinScale));
      int nOn    = int(nLEDs * (m_dNeedlePos -m_dMinScale) / (m_dMaxScale-m_dMinScale));
      for (int iLED = 0; iLED < nLEDs; iLED++) {
         if (iLED >= nAlert) {
            m_dcGrid.SelectObject(&m_BrushG_GridAlert);
            m_dcGrid.SelectObject(&m_PenG_GridAlert);
         }
         if (iLED >  nOn   ) m_dcGrid.SelectObject(&m_BrushG_BGround);
         m_dcGrid.Ellipse(l, b-d, l+d, b);
         b -= d+2;
      }
   }

   // Restore them.
   if (pPenOld  ) m_dcGrid.SelectObject(pPenOld);
   if (pBrushOld) m_dcGrid.SelectObject(pBrushOld);
}

//------------------------------------------------------------------------------

void CBarMeter::ShowMeterImage(CDC * pDC)
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
   if (!m_dcGrid.GetSafeHdc()  ) return;
   if (!m_dcNeedle.GetSafeHdc()) return;

   if (memDC.GetSafeHdc() != NULL) {
      // Draw the inverted grid.
      memDC.BitBlt(0, 0, m_nRectWidth, m_nRectHeight, &m_dcGrid,   0, 0, NOTSRCCOPY);
      // Merge the needle image with the grid.
      memDC.BitBlt(0, 0, m_nRectWidth, m_nRectHeight, &m_dcNeedle, 0, 0, SRCINVERT);
      // Copy the resulting bitmap to the destination.
      pDC->BitBlt(m_rectOwner.left, m_rectOwner.top, m_nRectWidth, m_nRectHeight,
                  &memDC, 0, 0, SRCCOPY);
   }

   memDC.SelectObject(oldBitmap);
}

//------------------------------------------------------------------------------

void CBarMeter::ActuateColors()
{
#define setPenColor(p,c) \
   if (p.m_hObject          ) p.DeleteObject(); \
   if (p.m_hObject == NULL  ) p.CreatePen(PS_SOLID, 0, nearestColor((~c) & 0xFFFFFF));

#define setBrushColor(b,c) \
   if (b.m_hObject        ) b.DeleteObject(); \
   if (b.m_hObject == NULL) b.CreateSolidBrush(nearestColor((~c) & 0xFFFFFF));

#define white RGB(255,255,255)

   setPenColor  (m_PenG_Grid,        m_colorGrid);
   setBrushColor(m_BrushG_Grid,      m_colorGrid);
   setPenColor  (m_PenG_GridAlert,   m_colorGridAlert);
   setBrushColor(m_BrushG_GridAlert, m_colorGridAlert);
   setPenColor  (m_PenG_BGround,     m_colorBGround);
   setBrushColor(m_BrushG_BGround,   m_colorBGround);
   setPenColor  (m_PenN_Needle,      m_colorNeedle);
   setBrushColor(m_BrushN_Needle,    m_colorNeedle);
   setPenColor  (m_PenN_BGround,     white);
   setBrushColor(m_BrushN_BGround,   white);

#undef setPenColor
#undef setBrushColor
}

//------------------------------------------------------------------------------

void CBarMeter::SetColor(enum MeterMemberEnum meter_member, COLORREF Color)
{
   switch (meter_member) {
      case meter_title    : m_colorTitle     = Color; break;
      case meter_needle   : m_colorNeedle    = Color; break;
      case meter_grid     : m_colorGrid      = Color; break;
      case meter_gridalert: m_colorGridAlert = Color; break;
      case meter_bground  : m_colorBGround   = Color; break;
   }

   ActuateColors();
}

//------------------------------------------------------------------------------

void CBarMeter::SetRange(double dMin, double dMax)
{
   // The function does NOT force the re-drawing of the meter.
   // The owner must explicitly call the ShowMeter function
   m_dMinScale = dMin;
   m_dMaxScale = dMax;
   m_boolForceRedraw = true;
}

//------------------------------------------------------------------------------

void CBarMeter::SetAlert(double dAlert)
{
   // The function does NOT force the re-drawing of the meter.
   // The owner must explicitly call the ShowMeter function
   double lo = m_dMinScale < m_dMaxScale ? m_dMinScale : m_dMaxScale;
   double hi = m_dMinScale < m_dMaxScale ? m_dMaxScale : m_dMinScale;
   if (dAlert < lo) dAlert = m_dMinScale;
   if (dAlert > hi) dAlert = m_dMaxScale;
   m_dAlertScale = dAlert;
   m_boolForceRedraw = true;
}

//------------------------------------------------------------------------------
