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
#include "megatune.h"
#include "msDatabase.h"
#include "tuning3D.h"
#include "Dconplot.h"

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

extern msDatabase mdb;
extern tuningDialog ut[tuningDialog::nUT];

//------------------------------------------------------------------------------

Dconplot::Dconplot(int nId, CWnd * pParent /*=NULL*/)
 : dataDialog(Dconplot::IDD, pParent)
 , nId(nId)
 , xDigits(0)
 , yDigits(0)
{
   td = ut+nId;
   nX = td->x->nValues();
   nY = td->y->nValues();

   _savePage = mdb.setPageNo(td->pageNo, true);

   xPlotVals = new double [nX];
   yPlotVals = new double [nY];
   zPlotVals = new double*[nX];
   for (int iX = 0; iX < nX; iX++) {
      zPlotVals[iX] = new double[nY];
   }
   //{{AFX_DATA_INIT(Dconplot)
   //}}AFX_DATA_INIT
}

Dconplot::~Dconplot()
{
   for (int iX = 0; iX < nX; iX++) {
      delete [] zPlotVals[iX];
   }
   delete [] xPlotVals;
   delete [] yPlotVals;
   delete [] zPlotVals;

   mdb.setPageNo(_savePage);
}

//------------------------------------------------------------------------------

void Dconplot::DoDataExchange(CDataExchange * pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(Dconplot)
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(Dconplot, CDialog)
   //{{AFX_MSG_MAP(Dconplot)
      ON_WM_PAINT()
      ON_WM_SIZE()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------
// Dconplot message handlers

#define MIN(x,y) ((x<y)?(x):(y))
#define MAX(x,y) ((x>y)?(x):(y))

BOOL Dconplot::OnInitDialog()
{
   dataDialog::OnInitDialog(td->title);

/* CPen pborder(PS_SOLID,10,RGB(0,0,0));

   CClientDC dc(this);
   dc.SelectObject(&pborder);
   dc.MoveTo(10,10);
   dc.LineTo(110,110);
 */

   int iX, iY;
   minZ = maxZ = td->ZVAL(0, 0);
   for (iY = 0; iY < nY; iY++) {
      for (iX = 0; iX < nX; iX++) {
         double v = td->ZVAL(iX, iY);
         zPlotVals[iX][iY] = v;
         minZ = MIN(v, minZ);
         maxZ = MAX(v, maxZ);
      }
   }

   for (iX = 0; iX < nX; iX++) {
      xPlotVals[iX] = td->XBIN(iX);
   }
   for (iY = 0; iY < nY; iY++) {
      yPlotVals[iY] = td->YBIN(iY);
   }
   xDigits = td->x->digits();
   yDigits = td->y->digits();

   ShowWindow(SW_MAXIMIZE);
   return TRUE;
}

//------------------------------------------------------------------------------

void Dconplot::color(double pct, COLORREF &rgb, double intensity, double blackness)
{
   if (pct > 1.0) pct = 1.0; 
   if (pct < 0.0) pct = 0.0;
   pct = 1.0 - pct;

   int r, g, b;
   if (pct < 0.33) {
      r = 255;
      g = MIN(255, int(pct * 3 * 255));
      b =   0;
   }
   else if (pct < 0.66) {
      r = MIN(255, int((0.66 - pct) * 3 * 255));
      g = 255;
      b =   0;
   }
   else {
      r =   0;
      g = MIN(255, int((1.0 - pct) * 3 *255));
      b = 255 - g;
   }
   if (intensity < 1.0 || blackness > 0.0) {
      int white =  int(255.0*(1.0-intensity));
      blackness = 1.0-blackness;
      r = int((r*intensity + white) * blackness);
      g = int((g*intensity + white) * blackness);
      b = int((b*intensity + white) * blackness);
   }
   rgb = RGB(r,g,b);
}

//------------------------------------------------------------------------------

void Dconplot::OnPaint()
{
   CPaintDC dc(this);

   CRect r;
   GetWindowRect(&r);
   ScreenToClient(&r);

   int BORDERX1 = r.left   + 10;
   int BORDERY1 = r.top    + 30;
   int BORDERX2 = r.right  - 10;
   int BORDERY2 = r.bottom - 45;
   int CPX1     = BORDERX1 + 50;
   int CPY1     = BORDERY1 + 30;
   int CPX2     = BORDERX2 - 30;
   int CPY2     = BORDERY2 - 45;

   CPen           pborder(PS_SOLID, 2, RGB(0, 0, 0));
   CPen           pgrid  (PS_SOLID, 1, RGB(0, 0, 0));
   double         slopeX, slopeY, f1, f2, X, Y;
   int            iX, iY;

   CPen          *pOldPen = dc.SelectObject(&pborder);

   dc.Rectangle(BORDERX1, BORDERY1, BORDERX2, BORDERY2);
   //dc.MoveTo(10,10);
   //dc.LineTo(110,110);

   dc.Rectangle(CPX1, CPY1, CPX2, CPY2);

   // set up color table
   COLORREF vecolors[256];
   double rangeZ = maxZ - minZ;
   for (int ii = 0; ii < 256; ii++) {
      color(double(ii/255.0), vecolors[ii]);
   }

   slopeX = (xPlotVals[nX-1] - xPlotVals[0]) / (CPX2 - CPX1);
   slopeY = (yPlotVals[nY-1] - yPlotVals[0]) / (CPY2 - CPY1);

   for (int i = CPX1 + 1; i < CPX2 - 1; i++) {
      iX = 0;
      X = slopeX * (i - CPX1) + xPlotVals[0];
      for (iX = nX-1; iX > -1; iX--) {
         if (X > xPlotVals[iX]) break;
      }
      if (iX < 0) break; // When the bins are mangled we see this.

      for (int j = CPY1 + 1; j < CPY2 - 1; j++) {
         Y = slopeY * (CPY2 - j) + yPlotVals[0];
         for (iY = nY-1; iY > -1; iY--) {
            if (Y > yPlotVals[iY]) break;
         }
         if (iY < 0) break; // Again, bins are not in order.

         // Change this to use interpolate function in mdb.
         f1 = (X - xPlotVals[iX]) / (xPlotVals[iX + 1] - xPlotVals[iX]);
         f2 = (Y - yPlotVals[iY]) / (yPlotVals[iY + 1] - yPlotVals[iY]);
         double pixelZ = (1.0 - f1) * (1.0 - f2) * zPlotVals[iX]  [iY]
                       + f1         * (1.0 - f2) * zPlotVals[iX+1][iY]
                       + f2         * (1.0 - f1) * zPlotVals[iX]  [iY+1]
                       + f1         * f2         * zPlotVals[iX+1][iY+1];
         int colorIndex = int((pixelZ-minZ) / rangeZ * 255.0);
         dc.SetPixel(i, j, vecolors[colorIndex]);
      }
   }

   CFont horFont, verFont;

   horFont.CreateFont(14,
                     0,
                     0,
                     0,
                     400,
                     FALSE,
                     FALSE,
                     0,
                     DEFAULT_CHARSET,
                     OUT_DEFAULT_PRECIS,
                     CLIP_DEFAULT_PRECIS,
                     DEFAULT_QUALITY,
                     DEFAULT_PITCH | FF_SWISS,
                     "Verdana");

   verFont.CreateFont(14,
                      0,
                      900, // Rotated 90.0 d from horFont
                      0,
                      400,
                      FALSE,
                      FALSE,
                      0,
                      DEFAULT_CHARSET,
                      OUT_DEFAULT_PRECIS,
                      CLIP_DEFAULT_PRECIS,
                      DEFAULT_QUALITY,
                      DEFAULT_PITCH | FF_SWISS,
                      "Verdana");

   CFont *pOldFont = dc.SelectObject(&horFont);

   dc.SelectObject(&pgrid);

   dc.SetTextColor(RGB(  0,  0,  0));
   dc.SetBkColor  (RGB(255,255,255));
   dc.SetTextAlign(TA_RIGHT);

   CString pp;
   for (iX = 0; iX < nX; iX++) {
      // Horizontal axis labels and grid lines
      f1 = (1.0 / slopeX) * (xPlotVals[iX] - xPlotVals[0]) + CPX1;
      dc.MoveTo((int) f1, CPY1);
      dc.LineTo((int) f1, CPY2 + 6);

      pp.Format("%.*f", xDigits, xPlotVals[iX]);
      dc.TextOut((int) f1 + 5, CPY2 + 8, pp);
   }

   for (iY = 0; iY < nY; iY++) {
      // Vertical axis labels and grid lines
      f1 = (1.0 / slopeY) * (yPlotVals[nY-1] - yPlotVals[iY]) + CPY1;
      dc.MoveTo(CPX1 - 6, (int) f1);
      dc.LineTo(CPX2, (int) f1);

      pp.Format("%.*f", yDigits, yPlotVals[iY]);
      dc.TextOut(CPX1 - 3, (int) f1 - 8, pp);
   }

   dc.SetTextAlign(TA_CENTER);
   int xCenter = (CPX1+CPX2) / 2;
   int yCenter = (CPY1+CPY2) / 2;

   dc.TextOut(xCenter, 15       , "Contour Plot Of "+td->z->name()+" ("+td->z->units()+")"); // ???
   dc.TextOut(xCenter, CPY2 + 25, td->x->units());
   dc.SelectObject(&verFont);
   dc.TextOut(     12, yCenter  , td->y->units());
   dc.SelectObject(pOldFont);

   dc.SelectObject(pOldPen);

   // Do not call CDialog::OnPaint() for painting messages!
}

//------------------------------------------------------------------------------

void Dconplot::OnSize(UINT nType, int cx, int cy) 
{
   CDialog::OnSize(nType, cx, cy);

   if (nType == SIZE_MINIMIZED) return; // Don't waste time.

   CRect rct;
   CWnd *s = GetDlgItem(IDCANCEL);
   s->GetWindowRect(&rct);
   ScreenToClient(&rct);
   int right = cx-5;
   int bot   = cy-3;
   int top   = bot - (rct.Height() + 3);
   right    -= rct.Width();
   s->MoveWindow(right, top, rct.Width(), rct.Height(), true);

   Invalidate();
}

//------------------------------------------------------------------------------
