//------------------------------------------------------------------------------
//--  Plot3d.cpp - Plotting module for MegaTune.                              --
//--                                                                          --
//--  Copyright (c) 2003,2004,2005,2006 by Eric Fahlgren                      --
//--  All Rights Reserved.                                                    --
//--                                                                          --
//--  This program is free software; you can redistribute it and/or           --
//--  modify it under the terms of the GNU General Public License             --
//--  as published by the Free Software Foundation; either version            --
//--  2 of the License, or (at your option) any later version.                --
//--  See http://www.gnu.org/licenses/gpl.txt                                 --
//------------------------------------------------------------------------------

#include "stdafx.h"
#include <math.h>
#include "plot3d.h"

//------------------------------------------------------------------------------

void Point3::xform(M33 &m)
{
   // Destroys the original point.
   double x0 = x, y0 = y, z0 = z;

   x = x0 * m[0][0] + y0 * m[1][0] + z0 * m[2][0];
   y = x0 * m[0][1] + y0 * m[1][1] + z0 * m[2][1];
   z = x0 * m[0][2] + y0 * m[1][2] + z0 * m[2][2];
}

double Point3::mag()
{
   return sqrt(x*x + y*y + z*z);
}

//------------------------------------------------------------------------------

void M33::rot(double a1, double a2, double a3)
{
   // Spacecraft Dynamics by Thomas R. Kane, ASIN 0070378436
   // McGraw Hill College Division, Jan, 1983
   double s1 = sin(a1), c1 = cos(a1);
   double s2 = sin(a2), c2 = cos(a2);
   double s3 = sin(a3), c3 = cos(a3);

   // Space 123, easy on the brain for this app.
   m[0][0] =  c2 * c3;
   m[0][1] =  s1 * s2 * c3 - s3 * c1;
   m[0][2] =  c1 * s2 * c3 + s3 * s1 ;
   m[1][0] =  c2 * s3;
   m[1][1] =  s1 * s2 * s3 + c3 * c1;
   m[1][2] =  c1 * s2 * s3 - c3 * s1;
   m[2][0] = -s2;
   m[2][1] =  s1 * c2;
   m[2][2] =  c1 * c2;

   /* Body 313, standard Euler angles...
   m[0][0] =  c3 * c1      - s3 * s1 * c2;
   m[0][1] = -s3 * c1      - c3 * s1 * c2;
   m[0][2] =  s2 * s1;
   m[1][0] =  c3 * s1      + s3 * c1 * c2;
   m[1][1] =  c3 * c1 * c2 - s3 * s1;
   m[1][2] = -s2 * c1;
   m[2][0] =  s2 * s3;
   m[2][1] =  s2 * c3;
   m[2][2] =  c2;
   */
}

//------------------------------------------------------------------------------

void plot3d::rotate(Point3 &p3)
{
   p3 = p3 - center;
   p3.x /= scale.x;
   p3.y /= scale.y;
   if (_twoD) {
      p3.y = -p3.y;
      p3.z = 0.0;
   }
   else {
      p3.z /= scale.z;
      p3.xform(_X);
   }
   //printf("%f %f %f\n", p3.x, p3.y, p3.z);
   //p3 = p3 + center;
}

POINT plot3d::world2screen(Point3 p3)
{
   rotate(p3);
   POINT p = {
      long(slopeX * (p3.x - minWorld) + double(minScreenX)),
      long(slopeY * (p3.y - minWorld) + double(minScreenY))
   };
   p.x = min(max(p.x, minScreenX), maxScreenX);
   p.y = min(max(p.y, minScreenY), maxScreenY);
   return p;
}

//------------------------------------------------------------------------------

void plot3d::recalcSettings()
{
   center = (corner1+corner2) / 2.0;
   scale  = (corner2-corner1) / 2.0;

// Point3 c1 = corner1; rotate(c1);
// Point3 c2 = corner2; rotate(c2);
// minWorld = -1.4; // sqrt(2), really. //min(min(c1.x, c2.x), min(c1.y, c2.y));
// maxWorld =  1.4; //max(max(c1.x, c2.x), max(c1.y, c2.y));

   slopeX = double(maxScreenX-minScreenX) / (maxWorld-minWorld);
   slopeY = double(maxScreenY-minScreenY) / (maxWorld-minWorld);
}

void plot3d::setWorldCoordinates(Point3 c1, Point3 c2)
{
   corner1 = c1;
   corner2 = c2;
   recalcSettings();
}

void plot3d::setDeviceCoordinates(int x1, int y1, int x2, int y2)
{
   minScreenX = x1; maxScreenX = x2;
   minScreenY = y1; maxScreenY = y2;
   recalcSettings();
}

void plot3d::setRotation(double psi, double theta, double phi)
{
   _X.rot(psi, theta, phi);
   recalcSettings();
}

void plot3d::set2D(bool twoD)
{
   _twoD = twoD;
   recalcSettings();
}

//------------------------------------------------------------------------------

void plot3d::clear(int deflateBy)
{
   CRect plot(minScreenX, minScreenY, maxScreenX, maxScreenY);
   plot.DeflateRect(deflateBy, deflateBy);
   CDC *dc = hWnd->GetDC();
      oldSpot[0].x = -1;
      oldCurs[0].x = -1;

      dc->FillRect(plot, &bbrush);
   hWnd->ReleaseDC(dc);
}

//------------------------------------------------------------------------------

void plot3d::drawPolyline(Point3 p1, Point3 p2)
{
   POINT pp1 = world2screen(p1);
   POINT pp2 = world2screen(p2);
   CDC *dc = hWnd->GetDC();
      dc->SelectObject(&pline);
      dc->MoveTo(pp1);
      dc->LineTo(pp2);
   hWnd->ReleaseDC(dc);
}

void plot3d::drawPolyline(Point3 p3[], int N)
{
   static int   nP2 = 0;
   static POINT *p2 = NULL;
   if (N > nP2) {
      if (p2) delete [] p2;
      p2  = new POINT[N];
      nP2 = N;
   }

   for (int i = 0; i < N; i++) {
      p2[i] = world2screen(p3[i]);
   }
   CDC *dc = hWnd->GetDC();
      dc->SelectObject(&pline);
      dc->Polyline(p2, N);
   hWnd->ReleaseDC(dc);
}

//------------------------------------------------------------------------------

void plot3d::drawMarkers(Point3 p3[], int N)
{
   enum { size = 3 };
   CDC *dc = hWnd->GetDC();
      dc->SelectObject(&pline);
      POINT p2;
      for (int i = 0; i < N; i++) {
         p2 = world2screen(p3[i]);
         dc->FillSolidRect(p2.x-2, p2.y-2, 5, 5, _markerColor);
      }
   hWnd->ReleaseDC(dc);
}

//------------------------------------------------------------------------------

void plot3d::spot(CDC *dc, POINT p[4])
{
   enum { size = 5 };
   dc->SelectObject(&pspot);
   dc->Ellipse(p[0].x - size, p[0].y - size, p[0].x + size, p[0].y + size);

   if (spotDepth > 0) {
      dc->SelectObject(&pspot1);
      dc->MoveTo(p[0]); dc->LineTo(p[1]);
   }
   if (spotDepth > 1) {
      dc->MoveTo(p[1]); dc->LineTo(p[2]);
      dc->MoveTo(p[1]); dc->LineTo(p[3]);
   }
}

void plot3d::drawSpot(Point3 p)
{
   POINT Spot[4];
   Spot[0] = world2screen(p);
   p.z = 0.0;
   Spot[1] = world2screen(p);
   Point3 px = Point3(p.x, minY, 0.0);
   Spot[2] = world2screen(px);
   Point3 py = Point3(minX, p.y, 0.0);
   Spot[3] = world2screen(py);

   if (Spot[0].x != oldSpot[0].x || Spot[0].y != oldSpot[0].y) {
      CDC *dc = hWnd->GetDC();
         int nOldMode = dc->GetROP2();
         dc->SetROP2(R2_XORPEN);
      
         //dc->SelectObject(&pspot);
         if (oldSpot[0].x != -1) {
            spot(dc, oldSpot);
         }
         spot(dc, Spot);
         dc->SetROP2(nOldMode);
      hWnd->ReleaseDC(dc);
      memcpy(oldSpot, Spot, sizeof(Spot));
   }
}

//------------------------------------------------------------------------------

void plot3d::cursor(CDC *dc, POINT p[4])
{
   enum { sizei = 2, sizeo = 6 };
   dc->SelectObject(&pcursor);
   dc->MoveTo(p[0].x-sizei, p[0].y-sizei); dc->LineTo(p[0].x-sizeo, p[0].y-sizeo);
   dc->MoveTo(p[0].x-sizei, p[0].y+sizei); dc->LineTo(p[0].x-sizeo, p[0].y+sizeo);
   dc->MoveTo(p[0].x+sizei, p[0].y-sizei); dc->LineTo(p[0].x+sizeo, p[0].y-sizeo);
   dc->MoveTo(p[0].x+sizei, p[0].y+sizei); dc->LineTo(p[0].x+sizeo, p[0].y+sizeo);

   if (cursorDepth > 0) {
      dc->SelectObject(&pcursor1);
      dc->MoveTo(p[0]); dc->LineTo(p[1]);
   }
   if (cursorDepth > 1) {
      dc->MoveTo(p[1]); dc->LineTo(p[2]);
      dc->MoveTo(p[1]); dc->LineTo(p[3]);
  }
}

void plot3d::drawCursor(Point3 p)
{
   POINT Cursor[4];
   Cursor[0] = world2screen(p);
   p.z = 0.0;
   Cursor[1] = world2screen(p);
   Point3 px = Point3(p.x, minY, 0.0);
   Cursor[2] = world2screen(px);
   Point3 py = Point3(minX, p.y, 0.0);
   Cursor[3] = world2screen(py);

   if (Cursor[0].x != oldCurs[0].x || Cursor[0].y != oldCurs[0].y) {
      CDC *dc = hWnd->GetDC();
         int nOldMode = dc->GetROP2();
         dc->SetROP2(R2_XORPEN);

         if (oldCurs[0].x != -1) { // Erase the old one.
            cursor(dc, oldCurs);
         }

         cursor(dc, Cursor);

         dc->SetROP2(nOldMode);
      hWnd->ReleaseDC(dc);
      memcpy(oldCurs, Cursor, sizeof(Cursor));
   }
}

//------------------------------------------------------------------------------

void plot3d::drawText(Point3 p3d, CString txt, bool hor, int dx, int dy)
{
   int rot = hor ? 0 : 900;
   CFont txtFont;
   txtFont.CreateFont(9,
                      0,
                      rot,
                      0,
                      FW_NORMAL,
                      FALSE,
                      FALSE,
                      0,
                      DEFAULT_CHARSET,
                      OUT_DEFAULT_PRECIS,
                      CLIP_DEFAULT_PRECIS,
                      DEFAULT_QUALITY,
                      DEFAULT_PITCH | FF_SWISS,
                      "Verdana");

   CDC *dc = hWnd->GetDC();
      CFont *pOldFont = dc->SelectObject(&txtFont);
      dc->SelectObject(&txtFont);
      POINT p2d = world2screen(p3d);
      dc->SetBkColor  (RGB(  0,  0,  0));
      dc->SetTextColor(RGB(255,255,255));
//    dc->SetTextAlign(TA_CENTER);
      dc->TextOut(p2d.x+dx, p2d.y+dy, txt);
      dc->SelectObject(pOldFont);
   hWnd->ReleaseDC(dc);
}

//------------------------------------------------------------------------------
