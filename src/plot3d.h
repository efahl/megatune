//------------------------------------------------------------------------------
//--  Plot3d.h - Plotting module for MegaTune.                                --
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

#ifndef PLOT3D_H
#define PLOT3D_H 1

//------------------------------------------------------------------------------

#define HALF_PI       1.57079632679489661923132169164E+0
#define PI            3.14159265358979323846264338328E+0
#define THREE_HALF_PI 4.71238898038468985769396507492E+0
#define TWO_PI        6.28318530717958647692528676656E+0

//------------------------------------------------------------------------------

class M33;

class Point3 {
public:
   double x, y, z;

   Point3(double x0=0.0, double y0=0.0, double z0=0.0)
    : x(x0),
      y(y0),
      z(z0)
   {
   }

   double &operator [](int idx) {
      if (idx < 0) idx = 0;
      if (idx > 2) idx = 2;
      switch (idx) {
         case 0: return x;
         case 1: return y;
         case 2: return z;
      }
      return x;
   }
   Point3 operator +(Point3 rhs) {
      Point3 result(x+rhs.x, y+rhs.y, z+rhs.z);
      return result;
   }
   Point3 operator -(Point3 rhs) {
      Point3 result(x-rhs.x, y-rhs.y, z-rhs.z);
      return result;
   }
   Point3 operator /(double rhs) {
      Point3 result(x/rhs, y/rhs, z/rhs);
      return result;
   }

   double mag();

   void xform(M33 &m);
};

//------------------------------------------------------------------------------

class M33 {
public:
   Point3 m[3];

   M33(double psi=0.0, double theta=0.0, double phi=0.0)
   {
      rot(psi, theta, phi);
   }

   void rot(double psi, double theta, double phi);
   Point3 &operator [](int idx) { return m[idx]; }
};

//------------------------------------------------------------------------------

class plot3d {
   CWnd  *hWnd;
   Point3 corner1,    corner2;
   Point3 center,     scale;
   long   minScreenX, minScreenY;
   long   maxScreenX, maxScreenY;
   double minWorld,   maxWorld;
   double slopeX,     slopeY;

   double minX,       minY; // User specified minimums.

   COLORREF _markerColor;
   CBrush    bbrush;
   CPen      pcursor, pcursor1;
   CPen      pline;
   CPen      pspot,   pspot1;
   int       spotDepth, cursorDepth;

   POINT  oldCurs[4];
   POINT  oldSpot[4];

   bool   _twoD;
   M33    _X; // Transform from world to screen.

   void   spot        (CDC*, POINT[4]);
   void   cursor      (CDC*, POINT[4]);
   void   rotate      (Point3&);
   POINT  world2screen(Point3);

   void   recalcSettings();

public:

   explicit plot3d(CWnd *pWnd)
    : hWnd       (pWnd),
      corner1    (Point3(0,0,0)),
      corner2    (Point3(1,1,1)),
      minScreenX (0),
      minScreenY (0),
      maxScreenX (1),
      maxScreenY (1),
      _twoD      (false),
      _X         (M33()),
      spotDepth  (2),
      cursorDepth(2),
      minWorld   (-1.4),
      maxWorld   ( 1.4)
   {
      bbrush  .CreateSolidBrush(RGB(0, 0, 0));
      pcursor .CreatePen       (PS_SOLID,                   3, RGB(255, 0, 0));
      pcursor1.CreatePen       (PS_SOLID,                   1, RGB(255, 0, 0));
      pline   .CreatePen       (PS_SOLID | PS_ENDCAP_ROUND, 1, RGB(150, 150, 150));
      pspot   .CreatePen       (PS_SOLID | PS_ENDCAP_ROUND, 2, RGB(0, 255, 0));
      pspot1  .CreatePen       (PS_SOLID | PS_ENDCAP_ROUND, 1, RGB(0, 255, 0));

      oldCurs[0].x = -1;
      oldSpot[0].x = -1;
   }

   void setWorld(double min, double max)
   {
      minWorld = min;
      maxWorld = max;
   }

   void setMins(double x, double y)
   {
      minX = x;
      minY = y;
   }

   void setDepth(int cDep, int sDep)
   {
      cursorDepth = cDep;
      spotDepth   = sDep;
   }

   void setColors(COLORREF bg, COLORREF grid, COLORREF cursor, COLORREF spot)
   {
      bbrush  .DeleteObject(); bbrush  .CreateSolidBrush(bg);
      pcursor .DeleteObject(); pcursor .CreatePen       (PS_SOLID,                   3, (cursor^bg)&0xffffff);
      pcursor1.DeleteObject(); pcursor1.CreatePen       (PS_SOLID,                   1, (cursor^bg)&0xffffff);
      pline   .DeleteObject(); pline   .CreatePen       (PS_SOLID | PS_ENDCAP_ROUND, 1, grid);
      pspot   .DeleteObject(); pspot   .CreatePen       (PS_SOLID | PS_ENDCAP_ROUND, 2, (spot  ^bg)&0xffffff);
      pspot1  .DeleteObject(); pspot1  .CreatePen       (PS_SOLID | PS_ENDCAP_ROUND, 1, (spot  ^bg)&0xffffff);
      _markerColor = RGB(127,127,255);
   }

   void setWorldCoordinates(Point3 corner1, Point3 corner2);
   void setDeviceCoordinates(int x0, int y0, int x1, int y1);
   void setRotation(double psi, double theta, double phi);
   void set2D(bool twoD);

   void clear(int deflateBy=0);

   void drawPolyline(Point3, Point3);
   void drawPolyline(Point3 p3[], int N);
   void drawMarkers (Point3 p3[], int N);
   void drawSpot    (Point3 p);
   void drawCursor  (Point3 p);
   void drawText    (Point3 p, CString txt, bool hor=true, int dx=0, int dy=0);
};

//------------------------------------------------------------------------------
#endif
