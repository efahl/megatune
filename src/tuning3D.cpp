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

#include "stdafx.h"
#include "megatune.h"
#include "megatuneDlg.h"
#include "msDatabase.h"
#include "repository.h"
#include "tuning3D.h"
#include "userMenu.h"

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------

extern msDatabase   mdb;
extern userMenuList uml;
extern repository   rep;

//------------------------------------------------------------------------------

autoTuneParameters::autoTuneParameters()
: _allowAutoTune (false)
, xRadius       (  200)  // RPM
, yRadius       (    7)  // MAP
, xMin          ( 1500)
, xMax          ( 4000)
, yMin          (   60)
, yMax          (   90)
, zMin          (   10)
, zMax          (  200)
, t0            (  1.0)
, dT            (  1.0)
, pGain         (  0.5)
, lumpiness     (    5) // Percent.
, corrector     (NULL )
{
}

void autoTuneParameters::setAutoTune(bool allow) { _allowAutoTune = allow; }
bool autoTuneParameters::autoTune()              { return _allowAutoTune;  }

bool autoTuneParameters::insideWindow(double x, double y)
{
   return x >= xMin && x <= xMax && y >= yMin && y <= yMax;
}

bool autoTuneParameters::nearVertex(double x, double y, double vX, double vY)
{
   return x >= vX - xRadius && x <= vX + xRadius
       && y >= vY - yRadius && y <= vY + yRadius;
}

//------------------------------------------------------------------------------

tuningDialog ut[tuningDialog::nUT];

tuning3D *userTuning(UINT nId)
{
   if (ut[nId].pTune == NULL) {
      ut[nId].pTune = new tuning3D(nId, NULL);
   }
   return ut[nId].pTune;
}

//------------------------------------------------------------------------------

double tuningDialog::XBIN(int idx) {
   return x->valueUser(idx);
}

double tuningDialog::YBIN(int idx) {
   return y->valueUser(idx);
// return (mdb.alphaN()
//    ? mdb.pctFromTps(mdb.Const(yOffset + idx))
//    : mdb.Const(yOffset + idx));
}

int tuningDialog::_iZVAL(int ix, int iy) {
   return z->index(ix, iy);
}

double tuningDialog::ZVAL(int ix, int iy)
{
   return z->valueUser(_iZVAL(ix, iy));
// int zval = (mdb.Const(zOffset+ _iZVAL(ix, iy)) + zTrans) * zScale;
// return mdb.alphaN() ? mdb.pctFromTps(BYTE(zval)) : zval;
}

const char *tuningDialog::ZSTR(int ix, int iy)
{
   return z->valueString(_iZVAL(ix, iy));
}

//------------------------------------------------------------------------------

double interp(double x, double x0, double y0, double x1, double y1);

double tuningDialog::interpolate(double X, double Y)
{
   int lastX = x->nValues()-1;
   int lastY = y->nValues()-1;

   if (X < XBIN(0    )) X = XBIN(0);
   if (X > XBIN(lastX)) X = XBIN(lastX);
   if (Y < YBIN(0    )) Y = YBIN(0);
   if (Y > YBIN(lastY)) Y = YBIN(lastY);

   int iX, iY;
   double X1, X2, Y1, Y2;
   for (iX = 0; iX < lastX; iX++) {
      if (X <= XBIN(iX+1)) {
         X1 = XBIN(iX);
         X2 = XBIN(iX+1);
         break;
      }
   }
   for (iY = 0; iY < lastY ; iY++) {
      if (Y <= YBIN(iY+1)) {
         Y1 = YBIN(iY);
         Y2 = YBIN(iY+1);
         break;
      }
   }

   double Z11 = ZVAL(iX+0, iY+0);
   double Z21 = ZVAL(iX+1, iY+0);
   double Z12 = ZVAL(iX+0, iY+1);
   double Z22 = ZVAL(iX+1, iY+1);

   double loMapVE = interp(X, X1, Z11, X2, Z21);
   double hiMapVE = interp(X, X1, Z12, X2, Z22);
   return interp(Y, Y1, loMapVE, Y2, hiMapVE);
}

//------------------------------------------------------------------------------

void tuningDialog::update()
{
   vXBIN = mdb.cDesc._userVar[xOch->varIndex()];
   vYBIN = mdb.cDesc._userVar[yOch->varIndex()];
   vZVAL = interpolate(vXBIN, vYBIN);
}

bool tuningDialog::nearestCellMoved()
{
   bool moved = false;

   int ix;
   for (ix = 0; ix < nX()-1 && vXBIN > XBIN(ix); ix++) ;
   if (ix > 0 && vXBIN-XBIN(ix-1) < XBIN(ix)-vXBIN) ix--;
   if (ix != iXBIN) {
      iXBIN = ix;
      moved = true;
   }

   int iy;
   for (iy = 0; iy < nY()-1 && vYBIN > YBIN(iy); iy++) ;
   if (iy > 0 && vYBIN-YBIN(iy-1) < YBIN(iy)-vYBIN) iy--;
   if (iy != iYBIN) {
      iYBIN = iy;
      moved = true;
   }

   if (moved) {
      iZVAL = _iZVAL(iXBIN, iYBIN);
   }

   return moved;
}

//------------------------------------------------------------------------------

tuning3D::tuning3D(int nId, CWnd *pParent)
 : dataDialog(tuning3D::IDD, pParent)
 , nId       (nId)

 , gauges    (aGauges)
 , autoTuning(false)
 , chasing   (false)
 , lgr       (RGB(220, 220, 220))
 , dgr       (RGB(127, 127, 127))

 , geometrySetup (false)
{
   td = ut+nId;
   mdb.cDesc._userVar[UveTuneLodIdx] = td->iYBIN;
   mdb.cDesc._userVar[UveTuneRpmIdx] = td->iXBIN;
   mdb.cDesc._userVar[UveTuneValue]  = td->ZVAL(td->iXBIN, td->iYBIN);

   td->iZVAL  = td->_iZVAL(td->iXBIN, td->iYBIN);

   _fontFace  = rep.t_fontFace; if (_fontFace.IsEmpty()) _fontFace = "FixedSys";
   _fontSize  = rep.t_fontSize; if (_fontSize == 0     ) _fontSize = 14;

   p3d = new plot3d(this);
   p3d->setColors(rep.t_backgroundColor, rep.t_gridColor, rep.t_cursorColor, rep.t_spotColor);
   p3d->setDepth (rep.vatCD, rep.vatSD);

   //{{AFX_DATA_INIT(tuning3D)
   //}}AFX_DATA_INIT
}

tuning3D::~tuning3D()
{
   delete p3d;
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(tuning3D, dataDialog)
   //{{AFX_MSG_MAP(tuning3D)
      ON_COMMAND(ID_TABLE_GET,  OnTableGet)
      ON_COMMAND(ID_TABLE_PUT,  OnTablePut)
      ON_COMMAND(ID_VEEXPORT,   OnVeExport)
      ON_COMMAND(ID_VEIMPORT,   OnVeImport)
      ON_COMMAND(ID_AUTOTUNING, OnAutoTune)

      ON_BN_CLICKED(IDC_PAGE_A, OnPageA)
      ON_BN_CLICKED(IDC_PAGE_B, OnPageB)
      ON_BN_CLICKED(IDC_PAGE_C, OnPageC)
      ON_BN_CLICKED(IDC_PAGE_D, OnPageD)


      ON_WM_DESTROY()
      ON_WM_KEYDOWN()
      ON_WM_PAINT()
      ON_WM_SIZE()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

void tuning3D::DoDataExchange(CDataExchange * pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(tuning3D)
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

void tuning3D::setMsg(const char *newMsg, int line, long clr)
{
   static char oldMsg[5][100] = {"", "", "", "", ""};
   CDC *dc = GetDC();
   if (dc) {
      CFont *pFontOld = dc->SelectObject(&_msgFont);
      dc->SetBkColor(rep.t_backgroundColor);
      dc->SetTextColor(rep.t_backgroundColor);
      dc->TextOut(wux1 + 10, wuy1 + 3 + (line*(_fontSize+4)), oldMsg[line]);
      dc->SetTextColor(clr&0xffffff);
      dc->TextOut(wux1 + 10, wuy1 + 3 + (line*(_fontSize+4)), newMsg);
      strcpy(oldMsg[line], newMsg);
      dc->SelectObject(pFontOld);
   }
   ReleaseDC(dc);
}

//------------------------------------------------------------------------------

void tuning3D::setupGauges(bool forceRedraw)
{
   char gaugeId[8] = { "gaugeNP" };
   gaugeId[6] = char('a' + gauges);
   for (int i = 0; i < nGauges; i++) {
      gaugeId[5] = char('1' + i);
      setupGauge(gauge[i], "Tuning", gaugeId);
   }

   if (forceRedraw) OnPaint();
}

//------------------------------------------------------------------------------

void logSize(CWnd *w, char *title);

BOOL tuning3D::OnInitDialog()
{
   dataDialog::OnInitDialog(td->title);
   setHelp(td->tuningHelp());

   _settings = GetMenu()->GetSubMenu(1);
   _settings->EnableMenuItem(ID_AUTOTUNING, MF_BYCOMMAND | (td->atp.autoTune() ? 0 : MF_GRAYED));
   autoTuning = false;

   uml.attach(this, td->tuningId());

   // All this setting of win sizes might be obsolete with
   // 2.25 changes to readConfig...
   bool center = false;
   if (mdb.tuneWin.left == 0 && mdb.tuneWin.right == 0) {
      mdb.tuneWin = mdb.mainWin;
      if (mdb.tuneWin.left == 0 && mdb.tuneWin.right == 0) {
         mdb.tuneWin.right  = 640;
         mdb.tuneWin.bottom = 480;
      }
      center = true;
   }
   MoveWindow(mdb.tuneWin);
   if (center) CenterWindow();

   logSize(this, "tune");

   if (_msgFont.m_hObject) _msgFont.DeleteObject();
   LOGFONT lf;
   memset(&lf, 0, sizeof(LOGFONT));
   CDC *dc = GetDC();
      lf.lfHeight = -MulDiv(_fontSize, GetDeviceCaps(dc->m_hDC, LOGPIXELSY), 72);
   ReleaseDC(dc);
   strcpy(lf.lfFaceName, _fontFace);
   _msgFont.CreateFontIndirect(&lf);

   static_cast<CButton *>(GetDlgItem(IDC_PAGE_A))->SetWindowText(rep.pageLabel(0));
   static_cast<CButton *>(GetDlgItem(IDC_PAGE_B))->SetWindowText(rep.pageLabel(1));
   static_cast<CButton *>(GetDlgItem(IDC_PAGE_C))->SetWindowText(rep.pageLabel(2));
   static_cast<CButton *>(GetDlgItem(IDC_PAGE_D))->SetWindowText(rep.pageLabel(3));
   static_cast<CButton *>(GetDlgItem(IDC_PAGE_A+gauges))->SetCheck(true);

   if (!geometrySetup) doResize(initCX, initCY);

   return TRUE;
}

void tuning3D::setupAllGeometry()
{
   double xborder = (td->XBIN(td->nX()-1) - td->XBIN(0)) * 0.0; // You can try non-zero, but it's ugly.
   minXBIN = td->XBIN(0      ) - xborder;
   maxXBIN = td->XBIN(td->nX()-1) + xborder;
   double yborder = (td->YBIN(td->nY()-1) - td->YBIN(0)) * 0.0;
   minYBIN = td->YBIN(0      ) - yborder;
   maxYBIN = td->YBIN(td->nY()-1) + yborder;

   CRect r;
   GetDlgItem(IDC_3DSTAT)->GetWindowRect(&r);
   ScreenToClient(&r);
   wux1 = r.left;
   wux2 = r.right;
   wuy1 = r.top;
   wuy2 = r.bottom;

   brushbg.DeleteObject();
   brushbg.CreateSolidBrush(rep.t_backgroundColor);

   msgRect.SetRect(wux1  - 12, wuy1  - 12, wux2  + 12, wuy2  + 12);

#define zMin (td->z->lo())
#define zMax (td->z->hi())
#define zDelta (zMax-zMin)
   GetDlgItem(IDC_3DPLOT)->GetWindowRect(&r);
   ScreenToClient(&r);
   p3d->setDeviceCoordinates(r.left, r.top, r.right, r.bottom);
   p3d->setWorldCoordinates(
       Point3(minXBIN, minYBIN, (zMin-0.5*zDelta)/td->gridScale),
       Point3(maxXBIN, maxYBIN, (zMax+zDelta)/td->gridScale));
#undef zMin
#undef zMax
#undef zDelta

   setPerspective(!td->flat);

   setupGauges(false);

   meterEGO.SetRange(rep.lotEGO, rep.hitEGO);
   meterEGO.SetAlert(rep.rdtEGO > 0.0 ? rep.rdtEGO : mdb.getByName(S_egoSwitch, 0));
   setColors(meterEGO);

   geometrySetup = true;
}

//------------------------------------------------------------------------------

void tuning3D::OnDestroy()
{
   dataDialog::OnDestroy();
   this->GetWindowRect(&mdb.tuneWin);
   mdb.writeConfig();
}

//------------------------------------------------------------------------------

char *tuning3D::locMsg(int curX, int curY, const char *suffix)
{
   static char message[200];
   sprintf(message, "(%5.0f %s, %4.0f %s) = %s %s %s",
      td->XBIN(curX),       td->x->units(),
      td->YBIN(curY),       td->y->units(),
      td->ZSTR(curX, curY), td->z->units(),
      suffix);
   return message;
}

void tuning3D::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   const char *suffix = "";

   if (nChar == 'G') {
      chasing = !chasing; // "Go to spot" mode toggles chasing.
      if (chasing)
         setMsg("GOTO mode ON, type 'G' to toggle", 4, rep.t_textColor);
      else
         setMsg("GOTO mode OFF, type 'G' to toggle", 4, rep.t_textColor);
      return;
   }

   int dx = 0;
   int dy = 0;
   int dz = 0;

   if (nChar == 'Z') {
      td->flat = !td->flat;
      setPerspective(!td->flat);
      dz = 1;
   }
   else if (nChar == 'M' || nChar == 'N') {
      static double inc = 10.0; // PI/36.0;
      if (nChar == 'N') td->xRot += inc; if (td->xRot >= 360.0) td->xRot = 0.0;
    //if (nChar == '?') td->yRot += inc; if (td->yRot >= 360.0) td->yRot = 0.0;
      if (nChar == 'M') td->zRot += inc; if (td->zRot >= 360.0) td->zRot = 0.0;
      td->flat = false;
      setPerspective(true);
      dz = 1;

      char msg[100];
      sprintf(msg, "gridOrient = %3.0f, %3.0f, %3.0f", td->xRot, td->yRot, td->zRot);
      setMsg(msg, 4, rep.t_textColor);
   }
   else if (nChar == VK_F3 || nChar == 'Q') {   // Increment bin - F3 or Q
      if (td->z->incRaw(+1, td->iZVAL)) {
         mdb.cDesc.send(td->z, td->iZVAL);
         suffix = td->upLabel;
         setMsg("Table not saved to FLASH", 4, rep.t_cursorColor);
         dz = 1;
      }
   }
   else if (nChar == VK_F4 || nChar == 'W') {    // Decrement bin - F4 or "W"
      if (td->z->incRaw(-1, td->iZVAL)) {
         mdb.cDesc.send(td->z, td->iZVAL);
         suffix = td->dnLabel;
         setMsg("Table not saved to FLASH", 4, rep.t_cursorColor);
         dz = -1;
      }
   }
   else if (nChar == 'E') {   // Increment big
      if (td->z->incRaw(+5, td->iZVAL)) {
         mdb.cDesc.send(td->z, td->iZVAL);
         suffix = td->upLabel;
         setMsg("Table not saved to FLASH", 4, rep.t_cursorColor);
         dz = +5;
      }
   }
   else if (nChar == 'R') {    // Decrement big
      if (td->z->incRaw(-5, td->iZVAL)) {
         mdb.cDesc.send(td->z, td->iZVAL);
         suffix = td->dnLabel;
         setMsg("Table not saved to FLASH", 4, rep.t_cursorColor);
         dz = -5;
      }
   }
   else if (nChar == 'F') {
      // Find the intersection nearest the pointer.
      int ix;
      for (ix = 0; ix < td->nX()-1 && td->vXBIN > td->XBIN(ix); ix++) ;
      if (ix > 0 && td->vXBIN-td->XBIN(ix-1) < td->XBIN(ix)-td->vXBIN) ix--;
      dx = ix - td->iXBIN;

      int iy;
      for (iy = 0; iy < td->nY()-1 && td->vYBIN > td->YBIN(iy); iy++) ;
      if (iy > 0 && td->vYBIN-td->YBIN(iy-1) < td->YBIN(iy)-td->vYBIN) iy--;
      dy = iy - td->iYBIN;
   }
   else if (nChar == VK_RETURN) { // Return key == next intersection.
      if (td->iXBIN < td->nX()-1)
         dx = 1;
      else {
         dx = -(td->nX()-1);
         dy = td->iYBIN < (td->nY()-1) ? 1 : -(td->nY()-1);
      }
   }
   else if (nChar == VK_F5 || nChar == 'H' || nChar == VK_LEFT) {
      if (td->iXBIN > 0) dx = -1;
   }
   else if (nChar == VK_F7 || nChar == 'K' || nChar == VK_UP) {
      if (td->iYBIN < td->nY()-1) dy = +1;
   }
   else if (nChar == VK_F6 || nChar == 'J' || nChar == VK_DOWN) {
      if (td->iYBIN > 0) dy = -1;
   }
   else if (nChar == VK_F8 || nChar == 'L' || nChar == VK_RIGHT) {
      if (td->iXBIN < td->nX()-1) dx = +1;
   }

   if (dx || dy || dz) {
      // Reposition cursor.
      td->iXBIN += dx;
      td->iYBIN += dy;
      td->iZVAL  = td->_iZVAL(td->iXBIN, td->iYBIN);
      if (dz) drawGrid();
      p3d->drawCursor(Point3(td->XBIN(td->iXBIN), td->YBIN(td->iYBIN), td->ZVAL(td->iXBIN, td->iYBIN)));

      if ((dx || dy) && chasing) {
         setMsg("GOTO mode ON, type 'G' to toggle", 4, rep.t_textColor);
      }
   }

   mdb.cDesc._userVar[UveTuneLodIdx] = td->iYBIN;
   mdb.cDesc._userVar[UveTuneRpmIdx] = td->iXBIN;
   mdb.cDesc._userVar[UveTuneValue]  = td->ZVAL(td->iXBIN, td->iYBIN);
   setMsg(locMsg(td->iXBIN, td->iYBIN, suffix), 1, rep.t_cursorColor);

   dataDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

//------------------------------------------------------------------------------

void tuning3D::setPerspective(bool threeD)
{
#define RAD(d) ((d/180.0)*PI)
#define DEG(x) ((x/PI)*180.0)
   if (threeD)
      p3d->setRotation(RAD(td->xRot), RAD(td->yRot), RAD(td->zRot));
   else
      p3d->setRotation(RAD(       0), RAD(     180), RAD(     180));
#undef RAD
#undef DEG
}

void tuning3D::drawGrid()
{
   p3d->clear(4);
   p3d->setMins(td->XBIN(0), td->YBIN(0));

   // Draw the plot grid.
   Point3 *line, p1, p2;
   line = new Point3[max(td->nX(), td->nY())];
   int    i,    j;
   double xval, yval, zval;
   for (i = 0; i < td->nX(); i++) { // Vertical lines.
      xval = td->XBIN(i);
      for (j = 0; j < td->nY(); j++) {
         yval = td->YBIN(j);
         zval = td->ZVAL(i, j);
         line[j] = Point3(xval, yval, zval);
      }
      if (i == 0 /*|| i == td->nX()-1*/) {
         p1 = line[     0]; p1.z = 0;
         p2 = line[td->nY()-1]; p2.z = 0;
         p3d->drawPolyline(p1, line[0]);
         p3d->drawPolyline(p1, p2);
         p3d->drawPolyline(p2, line[td->nY()-1]);
      }
      p3d->drawPolyline(line, td->nY());
   }

   for (j = 0; j < td->nY(); j++) { // Horizontal lines.
      yval = td->YBIN(j);
      for (i = 0; i < td->nX(); i++) {
         xval = td->XBIN(i);
         zval = td->ZVAL(i, j);
         line[i] = Point3(xval, yval, zval);
      }
      if (j == 0 /*|| j == td->nX()-1*/) {
         p1 = line[     0]; p1.z = 0;
         p2 = line[td->nX()-1]; p2.z = 0;
         p3d->drawPolyline(p1, p2);
         p3d->drawPolyline(p2, line[td->nX()-1]);
      }
      p3d->drawPolyline(line, td->nX());
   }
   delete [] line;

   mdb.cDesc._userVar[UveTuneValue]  = td->ZVAL(td->iXBIN, td->iYBIN);
   p3d->drawCursor(Point3(td->XBIN(td->iXBIN), td->YBIN(td->iYBIN), mdb.cDesc._userVar[UveTuneValue]));
   setMsg(locMsg(td->iXBIN, td->iYBIN, ""), 1, rep.t_cursorColor);
}

//------------------------------------------------------------------------------

void tuning3D::OnPaint()
{
   CPaintDC dc(this);

   if (!IsIconic()) {
      // Outline message window.
      CRect rectsav = msgRect;
      dc.Rectangle(rectsav);
      rectsav.DeflateRect(3, 3);
      dc.Draw3dRect(rectsav, (rep.t_backgroundColor) & 0xFFFFFF, (~rep.t_backgroundColor) & 0xFFFFFF);
      rectsav.DeflateRect(1, 1);
      dc.Draw3dRect(rectsav, (~lgr) & 0xFFFFFF, (~dgr) & 0xFFFFFF);
      rectsav.DeflateRect(4, 4);
      dc.FillRect(rectsav, &brushbg);

      // Draw the gauges in their frames.
      CRect r;
      for (int iGauge = 0; iGauge < nGauges; iGauge++) {
         // Really should check rep.gaugeColumns and not do the other half when we don't need to.
         GetDlgItem(IDC_GAUGE_T1+iGauge)->GetWindowRect(&r);
         ScreenToClient(&r);
         gauge[iGauge].ShowMeter(&dc, r);
      }

      GetDlgItem(IDC_METEREGOBAR)->GetWindowRect(&r);
      ScreenToClient(&r);
      meterEGO.ShowMeter(&dc, r);

      // Draw the cursor.
      drawGrid();
      p3d->drawCursor(Point3(td->XBIN(td->iXBIN), td->YBIN(td->iYBIN), td->ZVAL(td->iXBIN, td->iYBIN)));

      // Send the messages.
      static int nPaints = 0;
      char msg[128];
      sprintf(msg, "MegaTune - F1 for help %d", nPaints++);
      setMsg(msg, 0, rep.t_textColor);
   }
}

//------------------------------------------------------------------------------

bool tuning3D::doTimer(bool connected)
{
   if (!realized()) return false;

   if (!connected) {
      setMsg("Disconnected from controller", 3, rep.t_cursorColor);
      zeroGauges();
   }
   else {
      setMsg("Connected to controller", 3, rep.t_textColor);
      updateDisplay();
   }
   return true;
}

//------------------------------------------------------------------------------

void tuning3D::updateDisplay()
{
   td->update();
   p3d->drawSpot(Point3(td->vXBIN, td->vYBIN, td->vZVAL));

   char msg[100];
   sprintf(msg, "(%5.0f %s, %4.0f %s) = %.*f %s",
                          td->vXBIN, td->x->units(),
                          td->vYBIN, td->y->units(),
         td->z->digits(), td->vZVAL, td->z->units());
   setMsg(msg, 2, rep.t_spotColor);

   if (chasing || autoTuning) {
      if (td->nearestCellMoved()) {
         mdb.cDesc._userVar[UveTuneLodIdx] = td->iYBIN;
         mdb.cDesc._userVar[UveTuneRpmIdx] = td->iXBIN;
         mdb.cDesc._userVar[UveTuneValue]  = td->ZVAL(td->iXBIN, td->iYBIN);
         p3d->drawCursor(Point3(td->XBIN(td->iXBIN), td->YBIN(td->iYBIN), mdb.cDesc._userVar[UveTuneValue]));
         setMsg(locMsg(td->iXBIN, td->iYBIN, ""), 1, rep.t_cursorColor);
      }
   }

   updateGauges();
   autoTune();
}

void tuning3D::updateGauges()
{
   // Do gauges last, as the above code recalculates some runtime variables.
   CDC *dc = GetDC();
      for (int i = 0; i < nGauges; i++) {
         const gaugeConfiguration *g = reinterpret_cast<const gaugeConfiguration *>(gauge[i].userData);
         if (g) {
            gauge[i].UpdateNeedle(dc, mdb.cDesc._userVar[g->och]);
         }
      }
      meterEGO.UpdateNeedle(dc, mdb.egoLEDvalue());
   ReleaseDC(dc);
}

void tuning3D::zeroGauges()
{
   CDC *dc = GetDC();
      for (int i = 0; i < nGauges; i++) gauge[i].UpdateNeedle(dc, CAnalogMeter::missingValue);
   ReleaseDC(dc);
}

//------------------------------------------------------------------------------

void tuning3D::autoTune()
{
   if (!autoTuning || td->atp.corrector == NULL) return;

   static double nextUpdate =  0.0;
   static int    lastX      = -1;
   static int    lastY      = -1;
   static double lowerLimit =  0;
   static double upperLimit =  0;

   //---------------------------------------------------------------------------

   if (td->iXBIN != lastX || td->iYBIN != lastY) {
      // Don't update a bin unless we've spent at least t0 time there.
      lastX      = td->iXBIN;
      lastY      = td->iYBIN;

      // Calculate permitted limits at this vertex, based upon the
      // "lumpiness" parameter.
      int idx = td->_iZVAL(td->iXBIN-1, td->iYBIN);
      if (idx  > 0) {
         lowerLimit = td->z->valueUser(idx);
         upperLimit = td->z->valueUser(idx);
      }
      idx = td->_iZVAL(td->iXBIN+1, td->iYBIN);
      if (idx < td->nX()*td->nY()) {
         lowerLimit = min(lowerLimit, td->z->valueUser(idx));
         upperLimit = max(upperLimit, td->z->valueUser(idx));
      }
      idx = td->_iZVAL(td->iXBIN, td->iYBIN-1);
      if (idx > 0) {
         lowerLimit = min(lowerLimit, td->z->valueUser(idx));
         upperLimit = max(upperLimit, td->z->valueUser(idx));
      }
      idx = td->_iZVAL(td->iXBIN, td->iYBIN+1);
      if (idx < td->nX()*td->nY()) {
         lowerLimit = min(lowerLimit, td->z->valueUser(idx));
         upperLimit = max(upperLimit, td->z->valueUser(idx));
      }

      lowerLimit -= td->atp.lumpiness;
      upperLimit += td->atp.lumpiness;

      lowerLimit = max(lowerLimit, td->atp.zMin);
      upperLimit = min(upperLimit, td->atp.zMax);

      // Set the time for the next scheduled update.
      nextUpdate = timeNow() + td->atp.t0;
   }

   if (timeNow() >= nextUpdate) {
      if (!td->atp.insideWindow(td->vXBIN, td->vYBIN)) {
         setMsg("Auto-tune: Tuning point outside window", 4, rep.t_spotColor);
      }
      else if (!td->atp.nearVertex(td->vXBIN, td->vYBIN, td->XBIN(td->iXBIN), td->YBIN(td->iYBIN))) {
         setMsg("Auto-tune: Tuning point not near vertex", 4, rep.t_spotColor);
      }
      else {
         double currentValue = td->z->valueUser(td->iZVAL);
         double correction   = td->atp.corrector->valueUser();
         double steps        = td->atp.pGain * currentValue/100.0 * (correction-100);

         if (steps < 0 && currentValue+steps < lowerLimit) steps = min(0, lowerLimit - currentValue);
         if (steps > 0 && currentValue+steps > upperLimit) steps = max(0, upperLimit - currentValue);

         if (steps != 0 && td->z->incValue(steps, td->iZVAL)) {
            mdb.cDesc.send(td->z, td->iZVAL);
            drawGrid();
         }

         char msg[64];
         sprintf(msg, "Auto-tune correction: %.*f%s", td->z->digits(), steps, td->z->units());
         setMsg(msg, 4, rep.t_spotColor);
      }
      nextUpdate = timeNow() + td->atp.dT;
   }
}

//------------------------------------------------------------------------------

BOOL tuning3D::OnCommand(WPARAM wParam, LPARAM lParam)
{
   return dataDialog::OnCommand(wParam, lParam);
}

//------------------------------------------------------------------------------

void tuning3D::OnVeImport()
{
   mdb.veImport(nId);
   updateDisplay();
}

void tuning3D::OnVeExport()
{
   mdb.veExport(nId);
}

//------------------------------------------------------------------------------

void tuning3D::OnTableGet()
{
   mdb.getPage(td->pageNo);
   updateDisplay();
}

void tuning3D::OnTablePut()
{
   int savePage = mdb.setPageNo(td->pageNo);
      mdb.burnConst(td->pageNo);
      setMsg(td->z->name()+" Burned into Flash RAM", 4, rep.t_textColor);
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------

BOOL tuning3D::PreTranslateMessage(MSG *pMsg)
{
   static bool shifted = false;
   static bool ctrled  = false;
          bool alted   = (HIWORD(pMsg->lParam) & KF_ALTDOWN) != 0;

//char msg[127];sprintf(msg, "%x %x %x", pMsg->message, pMsg->wParam, pMsg->lParam);setMsg(msg, 4, rep.t_textColor);

   if (pMsg->message == WM_SYSKEYUP) {
      switch (pMsg->wParam) {
         case 'B':     OnTablePut(); return TRUE;
         case 'C':
            if (alted) PostMessage(WM_CLOSE, 0, 0);
            break;
      }
   }

   if (pMsg->message == WM_KEYUP) {
      switch (pMsg->wParam) {
         case VK_SHIFT  : shifted = false; return TRUE;
         case VK_CONTROL: ctrled  = false; return TRUE;
      }
   }

   if (pMsg->message == WM_KEYDOWN) {
      switch (pMsg->wParam) {
         case VK_SHIFT  : shifted = true;  return TRUE;
         case VK_CONTROL: ctrled  = true;  return TRUE;

         case VK_LEFT:  if (shifted) return TRUE; // Avoid accidental cursor motion.
         case 'H':
            OnKeyDown(pMsg->wParam, 1, pMsg->lParam >> 16);
            return TRUE;
         case VK_UP:    if (ctrled && shifted) pMsg->wParam = 'E';
                   else if (shifted          ) pMsg->wParam = 'Q'; // Q = rich up 38
         case 'K':
            OnKeyDown(pMsg->wParam, 1, pMsg->lParam >> 16);
            return TRUE;
         case VK_RIGHT: if (shifted) return TRUE;
         case 'L':
            OnKeyDown(pMsg->wParam, 1, pMsg->lParam >> 16);
            return TRUE;
         case VK_DOWN:  if (ctrled && shifted) pMsg->wParam = 'R';
                   else if (shifted          ) pMsg->wParam = 'W'; // W = lean dn 40
         case 'J':
            OnKeyDown(pMsg->wParam, 1, pMsg->lParam >> 16);
            return TRUE;

         case VK_SPACE:
            mdb.markLog();
            return TRUE;

         case 'M': case 'N': // ???
         case 'Z':
         case 'F': // Find command.
         case 'G': // "Go to spot" mode.
         case 'E':
         case 'Q':
         case 'R':
         case 'W':
         case VK_RETURN:
            OnKeyDown(pMsg->wParam, 1, pMsg->lParam >> 16);
            return TRUE;

         case 'A': if (ctrled) OnAutoTune();
                   // Fall through.

         case 'B': // Stuff to ignore, just so we have a complete list.
         case 'C':
         case 'D':
         case 'I':
         case 'O':
         case 'P':
         case 'S':
         case 'T':
         case 'U':
         case 'V':
         case 'X':
         case 'Y':
            return TRUE;

//       case VK_ESCAPE:
//          pMsg->message = 0x0105;
//          pMsg->wParam  = 0x0012;
//          pMsg->lParam  = 0xC0380001;
//          break;
      }
   }
   return dataDialog::PreTranslateMessage(pMsg);
}

//------------------------------------------------------------------------------

void tuning3D::OnPageA() { gauges = aGauges; setupGauges(true); }
void tuning3D::OnPageB() { gauges = bGauges; setupGauges(true); }
void tuning3D::OnPageC() { gauges = cGauges; setupGauges(true); }
void tuning3D::OnPageD() { gauges = dGauges; setupGauges(true); }

//------------------------------------------------------------------------------

void tuning3D::OnAutoTune()
{
   autoTuning = !autoTuning;
   _settings->CheckMenuItem(ID_AUTOTUNING, MF_BYCOMMAND | (autoTuning ? MF_CHECKED : MF_UNCHECKED));
   if (!autoTuning) setMsg("Auto-tune: Turned off", 4, rep.t_spotColor);
}

//------------------------------------------------------------------------------

int tuning3D::moveDown(UINT id, int bot, int &right)
{
   CRect rct;
   CWnd *s = GetDlgItem(id);
   s->GetWindowRect(&rct);
   ScreenToClient(&rct);
   int top = bot - (rct.Height() + 3);
   right -= rct.Width();
   s->MoveWindow(right, top, rct.Width(), rct.Height(), true);
   return top;
}

void tuning3D::OnSize(UINT nType, int cx, int cy)
{
   dataDialog::OnSize(nType, cx, cy);
   if (nType == SIZE_MINIMIZED) return; // Don't waste time.
   doResize(cx, cy);
}

void tuning3D::doResize(int cx, int cy)
{
   static bool oneCol = rep.gaugeColumns == 1;

   CWnd *p = GetDlgItem(IDC_3DPLOT);
   if (p == NULL) {
      initCX = cx;
      initCY = cy;
      return; // We haven't been realized, yet.
   }

   int bwd = 6 + cx/40;
   if (bwd < 18) bwd = 18;
   int blt   = int(cx * (oneCol ? 0.25 : 0.45)); // "Center" of the tuning screen.
   int bot   = cy-1;
   int right = blt - 6;
   int top   = moveDown(IDC_PAGE_D,           bot, right); right -= 3;
               moveDown(IDC_PAGE_C,           bot, right); right -= 3;
               moveDown(IDC_PAGE_B,           bot, right); right -= 3;
               moveDown(IDC_PAGE_A,           bot, right);

   CRect r;
   CWnd *v = GetDlgItem(IDC_METEREGOBAR);
   v->GetWindowRect(&r);
   ScreenToClient(&r);
   v->MoveWindow(blt, 0, bwd, bot);

   int gwd  = oneCol ? blt : blt/2;
   int ght  = top/3;
   for (int row = 0; row < 3; row++) {
      for (int col = 0; col < 2; col++) {
         CWnd *w = GetDlgItem(IDC_GAUGE_T1+row*2+col);
         if (oneCol && col == 1) w->MoveWindow(0,       0,       0,   0);
         else                    w->MoveWindow(col*gwd, row*ght, gwd, ght);
      }
   }

   int plt = blt+bwd+3; // 3 pixels right of ve bar
   int pwd = cx-plt;
   CWnd *m = GetDlgItem(IDC_3DSTAT);
   m->GetWindowRect(&r);
   ScreenToClient(&r);
   r.top = r.bottom - int((_fontSize+5) * 5);

   int pht = bot - r.Height();
   m->MoveWindow(plt+12, pht-15, pwd-24, r.Height(), true);

   p->GetWindowRect(&r);
   ScreenToClient(&r);
   p->MoveWindow(plt,         0, pwd,    pht-27,     true);

   Invalidate();
   setupAllGeometry();
}

//------------------------------------------------------------------------------
