//------------------------------------------------------------------------------
//--  Copyright (c) 2004 by Eric Fahlgren, All Rights Reserved.               --
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

#include "stdafx.h"
#include "megatune.h"
#include "megatuneDlg.h"
#include "msDatabase.h"
#include "repository.h"
#include "Dttune.h"

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------

extern msDatabase mdb;
extern repository rep;

//------------------------------------------------------------------------------

tuningDialog ut[tuningDialog::nUT];

void userTuning(UINT nId)
{
   if (ut[nId].pTune == NULL) {
      ut[nId].pTune = new Dttune(nId, NULL);
   }
   ut[nId].pTune->DoModal();
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

   int iX, iY, X1, X2, Y1, Y2;
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

double Dttune::Xrot, Dttune::Yrot, Dttune::Zrot;
bool   Dttune::flat = false;

//------------------------------------------------------------------------------

Dttune::Dttune(int nId, CWnd *pParent)
 : dataDialog(Dttune::IDD, pParent)
 , nId       (nId)

 , iXBIN     (0)
 , iYBIN     (0)

 , vXBIN     (0)
 , vYBIN     (0)
 , gauges    (aGauges)
 , chasing   (false)
 , lgr       (RGB(220, 220, 220))
 , dgr       (RGB(127, 127, 127))

 , geometrySetup (false)
 , realized      (false)

{
   td = ut+nId;
   nX = td->x->nValues();
   nY = td->y->nValues();
   mdb.cDesc._userVar[UveTuneLodIdx] = iYBIN;
   mdb.cDesc._userVar[UveTuneRpmIdx] = iXBIN;
   mdb.cDesc._userVar[UveTuneValue]  = td->ZVAL(iXBIN, iYBIN);

   iZVAL  = td->_iZVAL(iXBIN, iYBIN);

   _fontFace  = rep.t_fontFace; if (_fontFace.IsEmpty()) _fontFace = "FixedSys";
   _fontSize  = rep.t_fontSize; if (_fontSize == 0     ) _fontSize = 14;

   p3d = new plot3d(this);
   p3d->setColors(rep.t_backgroundColor, rep.t_gridColor, rep.t_cursorColor, rep.t_spotColor);
   p3d->setDepth (rep.vatCD, rep.vatSD);

   //{{AFX_DATA_INIT(Dttune)
   //}}AFX_DATA_INIT
}

Dttune::~Dttune()
{
   delete p3d;
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(Dttune, dataDialog)
   //{{AFX_MSG_MAP(Dttune)
      ON_BN_CLICKED(IDC_BURN,   OnBurn)
      ON_BN_CLICKED(IDC_PAGE_A, OnPageA)
      ON_BN_CLICKED(IDC_PAGE_B, OnPageB)
      ON_BN_CLICKED(IDC_PAGE_C, OnPageC)
      ON_BN_CLICKED(IDC_PAGE_D, OnPageD)
      ON_NOTIFY(UDN_DELTAPOS, IDC_TREQFUEL_SPIN, OnReqfuelSpin)
      ON_WM_DESTROY()
      ON_WM_KEYDOWN()
      ON_WM_PAINT()
      ON_WM_SIZE()
      ON_WM_TIMER()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

void Dttune::DoDataExchange(CDataExchange * pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(Dttune)
      DDX_Control(pDX, IDC_TREQFUEL, m_tReqFuel);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

void Dttune::setMsg(const char *newMsg, int line, long clr)
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

void Dttune::timer(int state)
{
   if (state == on) {
      if (SetTimer(1, mdb.timerInterval, NULL) == 0) {
         MessageBox("ERROR: Cannot install timer.\nKill other useless Windows Apps.");
      }
   }
   else {
      KillTimer(1);
      Sleep(mdb.timerInterval);
   }
}

//------------------------------------------------------------------------------

void setColors (CBarMeter    &m);
void setupGauge(CAnalogMeter &m, const char *page, const char *name);

void Dttune::setupGauges(bool forceRedraw)
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

BOOL Dttune::OnInitDialog()
{
   realized = false;

   dataDialog::OnInitDialog(td->title.c_str());

   GetDlgItem(IDC_TREQFUEL_SPIN )->ShowWindow(FALSE);
   GetDlgItem(IDC_TREQFUEL_LABEL)->ShowWindow(FALSE);
   GetDlgItem(IDC_TREQFUEL      )->ShowWindow(FALSE);

   // Need to do this every time as user might have changed from
   // SD to A-N since the last time we displayed.
// strcpy(td->x->units(), "RPM");
// strcpy(td->y->units(), mdb.alphaN() == 1 ? (mdb.rawTPS ? " raw" : "%") : " kPa");

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
   lf.lfHeight = -MulDiv(_fontSize, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72);
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

void Dttune::setupAllGeometry()
{
   double xborder = (td->XBIN(nX-1) - td->XBIN(0)) * 0.0; // You can try non-zero, but it's ugly.
   minXBIN = td->XBIN(0      ) - xborder;
   maxXBIN = td->XBIN(nX-1) + xborder;
   double yborder = (td->YBIN(nY-1) - td->YBIN(0)) * 0.0;
   minYBIN = td->YBIN(0      ) - yborder;
   maxYBIN = td->YBIN(nY-1) + yborder;

// mdb.reqFuel = mdb.Const(Dreq_fuel+mdb.hackOfsl) / 10.0;
   char s[10];
   sprintf(s, "%.1f", mdb.reqFuel);
   m_tReqFuel.SetWindowText(s);

   CRect r;
   GetDlgItem(IDC_WU)->GetWindowRect(&r);
   ScreenToClient(&r);
   wux1 = r.left;
   wux2 = r.right;
   wuy1 = r.top;
   wuy2 = r.bottom;

   brushbg.DeleteObject();
   brushbg.CreateSolidBrush(rep.t_backgroundColor);

   vemsgrect.SetRect(wux1  - 12, wuy1  - 12, wux2  + 12, wuy2  + 12);

#define zMin (td->z->lo())
#define zMax (td->z->hi())
#define zDelta (zMax-zMin)
   GetDlgItem(IDC_VEP)->GetWindowRect(&r);
   ScreenToClient(&r);
   p3d->setDeviceCoordinates(r.left, r.top, r.right, r.bottom);
   p3d->setWorldCoordinates(
       Point3(minXBIN, minYBIN, (zMin-0.5*zDelta)/td->gridScale),
       Point3(maxXBIN, maxYBIN, (zMax+zDelta)/td->gridScale));

#define RAD(d) ((d/180.0)*PI)
   Xrot = RAD(rep.t_xRot);
   Yrot = RAD(rep.t_yRot);
   Zrot = RAD(rep.t_zRot);
   setPerspective(true);

   setupGauges(false);

   meterEGO.SetRange(rep.lotEGO, rep.hitEGO);
   meterEGO.SetAlert(rep.rdtEGO > 0.0 ? rep.rdtEGO : mdb.getByName(S_egoSwitch, 0));
   setColors(meterEGO);

   geometrySetup = true;
}

//------------------------------------------------------------------------------

void Dttune::OnDestroy()
{
   dataDialog::OnDestroy();
   this->GetWindowRect(&mdb.tuneWin);
   mdb.writeConfig();
}

//------------------------------------------------------------------------------

char *Dttune::locMsg(int curX, int curY, const char *suffix)
{
   static char message[200];
   sprintf(message, "(%5.0f %s, %4.0f %s) = %s %s %s",
      td->XBIN(curX),       td->x->units().c_str(),
      td->YBIN(curY),       td->y->units().c_str(),
      td->ZSTR(curX, curY), td->z->units().c_str(),
      suffix);
   return message;
}

void Dttune::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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
      flat = !flat;
      setPerspective(!flat);
      dz = 1;
   }
   else if (nChar == 'M' || nChar == 'N') {
      static double inc = PI/36.0;
      if (nChar == 'N') Xrot += inc; if (Xrot >= TWO_PI) Xrot = 0.0;
    //if (nChar == '?') Yrot += inc; if (Yrot >= TWO_PI) Yrot = 0.0;
      if (nChar == 'M') Zrot += inc; if (Zrot >= TWO_PI) Zrot = 0.0;
      setPerspective(true); dz = 1;
      char msg[100];
#define DEG(x) ((x/PI)*180.0)
      sprintf(msg, "gridOrient = %3.0f, %3.0f, %3.0f", DEG(Xrot), DEG(Yrot), DEG(Zrot));
      setMsg(msg, 4, rep.t_textColor);
   }
   else if (nChar == 'S') {   // Burn flash - "s"
      OnBurn();
   }
   else if (nChar == 'X') {   // Exit
      timer(off);
      if (MessageBox("Quit tuning?", "Tuning Go Bye-Bye", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION | MB_SYSTEMMODAL) == IDYES) {
         dataDialog::OnOK();
      }
      timer(on);
   }
   else if (nChar == VK_F3 || nChar == 'Q') {   // Increment bin - F3 or Q
      if (td->z->incRaw(+1, iZVAL)) {
         mdb.cDesc.send(td->z, iZVAL);
         suffix = td->upLabel.c_str();
         setMsg("Table not saved to FLASH", 4, rep.t_cursorColor);
         dz = 1;
      }
   }
   else if (nChar == VK_F4 || nChar == 'W') {    // Decrement bin - F4 or "W"
      if (td->z->incRaw(-1, iZVAL)) {
         mdb.cDesc.send(td->z, iZVAL);
         suffix = td->dnLabel.c_str();
         setMsg("Table not saved to FLASH", 4, rep.t_cursorColor);
         dz = -1;
      }
   }
   else if (nChar == 'E') {   // Increment big
      if (td->z->incRaw(+5, iZVAL)) {
         mdb.cDesc.send(td->z, iZVAL);
         suffix = td->upLabel.c_str();
         setMsg("Table not saved to FLASH", 4, rep.t_cursorColor);
         dz = +5;
      }
   }
   else if (nChar == 'R') {    // Decrement big
      if (td->z->incRaw(-5, iZVAL)) {
         mdb.cDesc.send(td->z, iZVAL);
         suffix = td->dnLabel.c_str();
         setMsg("Table not saved to FLASH", 4, rep.t_cursorColor);
         dz = -5;
      }
   }
   else if (nChar == 'F') {
      // Find the intersection nearest the pointer.
      for (int ix = 0; ix < nX-1 && vXBIN > td->XBIN(ix); ix++) ;
      if (ix > 0 && vXBIN-td->XBIN(ix-1) < td->XBIN(ix)-vXBIN) ix--;
      dx = ix - iXBIN;

      for (int iy = 0; iy < nY-1 && vYBIN > td->YBIN(iy); iy++) ;
      if (iy > 0 && vYBIN-td->YBIN(iy-1) < td->YBIN(iy)-vYBIN) iy--;
      dy = iy - iYBIN;
   }
   else if (nChar == VK_RETURN) { // Return key == next intersection.
      if (iXBIN < nX-1)
         dx = 1;
      else {
         dx = -(nX-1);
         dy = iYBIN < (nY-1) ? 1 : -(nY-1);
      }
   }
   else if (nChar == VK_F5 || nChar == 'H' || nChar == VK_LEFT) {
      if (iXBIN > 0) dx = -1;
   }
   else if (nChar == VK_F7 || nChar == 'K' || nChar == VK_UP) {
      if (iYBIN < nY-1) dy = +1;
   }
   else if (nChar == VK_F6 || nChar == 'J' || nChar == VK_DOWN) {
      if (iYBIN > 0) dy = -1;
   }
   else if (nChar == VK_F8 || nChar == 'L' || nChar == VK_RIGHT) {
      if (iXBIN < nX-1) dx = +1;
   }

   if (dx || dy || dz) {
      // Reposition cursor.
      iXBIN += dx;
      iYBIN += dy;
      iZVAL  = td->_iZVAL(iXBIN, iYBIN);
      if (dz) drawGrid();
      p3d->drawCursor(Point3(td->XBIN(iXBIN), td->YBIN(iYBIN), td->ZVAL(iXBIN, iYBIN)));

      if ((dx || dy) && chasing) {
         setMsg("GOTO mode ON, type 'G' to toggle", 4, rep.t_textColor);
      }
   }

   mdb.cDesc._userVar[UveTuneLodIdx] = iYBIN;
   mdb.cDesc._userVar[UveTuneRpmIdx] = iXBIN;
   mdb.cDesc._userVar[UveTuneValue]  = td->ZVAL(iXBIN, iYBIN);
   setMsg(locMsg(iXBIN, iYBIN, suffix), 1, rep.t_cursorColor);

   dataDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

//------------------------------------------------------------------------------

void Dttune::setPerspective(bool threeD)
{
   if (threeD)
      p3d->setRotation(Xrot,     Yrot,     Zrot);
   else
      p3d->setRotation(RAD(  0), RAD(180), RAD(180));
}

void Dttune::drawGrid()
{
   p3d->clear();
   p3d->setMins(td->XBIN(0), td->YBIN(0));

   // Draw the plot grid.
   Point3 *line, p1, p2;
   line = new Point3[max(nX, nY)];
   int    i,    j;
   double xval, yval, zval;
   for (i = 0; i < nX; i++) { // Vertical lines.
      xval = td->XBIN(i);
      for (j = 0; j < nY; j++) {
         yval = td->YBIN(j);
         zval = td->ZVAL(i, j);
         line[j] = Point3(xval, yval, zval);
      }
      if (i == 0 /*|| i == nX-1*/) {
         p1 = line[     0]; p1.z = 0;
         p2 = line[nY-1]; p2.z = 0;
         p3d->drawPolyline(p1, line[0]);
         p3d->drawPolyline(p1, p2);
         p3d->drawPolyline(p2, line[nY-1]);
      }
      p3d->drawPolyline(line, nY);
   }

   for (j = 0; j < nY; j++) { // Horizontal lines.
      yval = td->YBIN(j);
      for (i = 0; i < nX; i++) {
         xval = td->XBIN(i);
         zval = td->ZVAL(i, j);
         line[i] = Point3(xval, yval, zval);
      }
      if (j == 0 /*|| j == nX-1*/) {
         p1 = line[     0]; p1.z = 0;
         p2 = line[nX-1]; p2.z = 0;
         p3d->drawPolyline(p1, p2);
         p3d->drawPolyline(p2, line[nX-1]);
      }
      p3d->drawPolyline(line, nY);
   }
   delete [] line;

   mdb.cDesc._userVar[UveTuneValue]  = td->ZVAL(iXBIN, iYBIN);
   p3d->drawCursor(Point3(td->XBIN(iXBIN), td->YBIN(iYBIN), mdb.cDesc._userVar[UveTuneValue]));
   setMsg(locMsg(iXBIN, iYBIN, ""), 1, rep.t_cursorColor);
}

//------------------------------------------------------------------------------

void Dttune::OnPaint()
{
   CPaintDC dc(this);

   // Outline message window.
   CRect rectsav = vemsgrect;
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
   p3d->drawCursor(Point3(td->XBIN(iXBIN), td->YBIN(iYBIN), td->ZVAL(iXBIN, iYBIN)));

   // Send the messages.
   static int nPaints = 0;
   char msg[128];
   sprintf(msg, TITLE " - F1 for help %d", nPaints++);
   setMsg(msg, 0, rep.t_textColor);

   if (!realized) {
      timer(on);
      realized = true;
   }
}

//------------------------------------------------------------------------------

void Dttune::OnTimer(UINT nIDEvent)
{
   if (!mdb.getRuntime()) {
      setMsg("Disconnected from controller", 3, rep.t_cursorColor);
      zeroGauges();
   }
   else {
      setMsg("Connected to controller", 3, rep.t_textColor);
      updateDisplay();
   }
   dataDialog::OnTimer(nIDEvent);
}

//------------------------------------------------------------------------------

BEGIN_EVENTSINK_MAP(Dttune, dataDialog)
   //{{AFX_EVENTSINK_MAP(Dttune)
   //}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

//------------------------------------------------------------------------------

void Dttune::updateDisplay()
{
   vXBIN = int(mdb.cDesc._userVar[td->xOch->varIndex()]);
   vYBIN = int(mdb.cDesc._userVar[td->yOch->varIndex()]);
   double vZVAL = td->interpolate(vXBIN, vYBIN);
   p3d->drawSpot(Point3(vXBIN, vYBIN, vZVAL));

   char msg[100];
   sprintf(msg, "(%5d %s, %4d %s) = %.*f %s",
                          vXBIN, td->x->units().c_str(),
                          vYBIN, td->y->units().c_str(),
         td->z->digits(), vZVAL, td->z->units().c_str());
   setMsg(msg, 2, rep.t_spotColor);

   if (chasing) {
      // Find the intersection nearest the pointer.
      bool moved = false;

      for (int ix = 0; ix < nX-1 && vXBIN > td->XBIN(ix); ix++) ;
      if (ix > 0 && vXBIN-td->XBIN(ix-1) < td->XBIN(ix)-vXBIN) ix--;
      if (ix != iXBIN) {
         iXBIN = ix;
         moved    = true;
      }

      for (int iy = 0; iy < nY-1 && vYBIN > td->YBIN(iy); iy++) ;
      if (iy > 0 && vYBIN-td->YBIN(iy-1) < td->YBIN(iy)-vYBIN) iy--;
      if (iy != iYBIN) {
         iYBIN = iy;
         moved    = true;
      }

      if (moved) {
         // Reposition cursor.
         iZVAL = td->_iZVAL(iXBIN, iYBIN);
         mdb.cDesc._userVar[UveTuneLodIdx] = iYBIN;
         mdb.cDesc._userVar[UveTuneRpmIdx] = iXBIN;
         mdb.cDesc._userVar[UveTuneValue]  = td->ZVAL(iXBIN, iYBIN);
         p3d->drawCursor(Point3(td->XBIN(iXBIN), td->YBIN(iYBIN), mdb.cDesc._userVar[UveTuneValue]));
         setMsg(locMsg(iXBIN, iYBIN, ""), 1, rep.t_cursorColor);
      }
   }

   updateGauges();
}
  
void Dttune::updateGauges()
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

void Dttune::zeroGauges()
{
   CDC *dc = GetDC();
      for (int i = 0; i < nGauges; i++) gauge[i].UpdateNeedle(dc, CAnalogMeter::missingValue);
   ReleaseDC(dc);
}

//------------------------------------------------------------------------------

BOOL Dttune::OnCommand(WPARAM wParam, LPARAM lParam)
{
   if (wParam == IDCANCEL) {
      timer(off);
   }
   return dataDialog::OnCommand(wParam, lParam);
}

//------------------------------------------------------------------------------

void Dttune::OnBurn()
{
   int savePage = mdb.setPageNo(td->pageNo);
      mdb.burnConst();
      setMsg((td->z->name()+" Burned into Flash RAM").c_str(), 4, rep.t_textColor);
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------

void Dttune::OnReqfuelSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
   NM_UPDOWN* pNMUpDown = reinterpret_cast<NM_UPDOWN *>(pNMHDR);
   spinReqFuel(pNMUpDown->iDelta);
   *pResult = 0;
}

//------------------------------------------------------------------------------

BOOL Dttune::spinReqFuel(int delta)
{
// unsigned char reqFuel = mdb.Const(Dreq_fuel+mdb.hackOfsl);
// if (delta ==  1 && reqFuel >   1) mdb.putConstByte(Dreq_fuel+mdb.hackOfsl, reqFuel-1, true);
// if (delta == -1 && reqFuel < 255) mdb.putConstByte(Dreq_fuel+mdb.hackOfsl, reqFuel+1, true);

// mdb.reqFuel = mdb.Const(Dreq_fuel+mdb.hackOfsl) / 10.0;
   char s[10];
   sprintf(s, "%.1f", mdb.reqFuel);
   m_tReqFuel.SetWindowText(s);

   return TRUE;
}

//------------------------------------------------------------------------------

BOOL Dttune::PreTranslateMessage(MSG* pMsg)
{
   static bool shifted = false;
   static bool ctrled  = false;

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
         case VK_UP:    if (ctrled && shifted) pMsg->wParam = 'E'; // was return spinReqFuel(-1);
                   else if (shifted          ) pMsg->wParam = 'Q'; // Q = rich up 38
         case 'K':
            OnKeyDown(pMsg->wParam, 1, pMsg->lParam >> 16);
            return TRUE;
         case VK_RIGHT: if (shifted) return TRUE;
         case 'L':
            OnKeyDown(pMsg->wParam, 1, pMsg->lParam >> 16);
            return TRUE;
         case VK_DOWN:  if (ctrled && shifted) pMsg->wParam = 'R'; // was return spinReqFuel( 1);
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
         case 'Q':
         case 'W':
         case 'X':
         case 'S':
         case VK_RETURN:
            OnKeyDown(pMsg->wParam, 1, pMsg->lParam >> 16);
            return TRUE;
      }
   }
   return dataDialog::PreTranslateMessage(pMsg);
}

//------------------------------------------------------------------------------

void Dttune::OnPageA() { gauges = aGauges; setupGauges(true); }
void Dttune::OnPageB() { gauges = bGauges; setupGauges(true); }
void Dttune::OnPageC() { gauges = cGauges; setupGauges(true); }
void Dttune::OnPageD() { gauges = dGauges; setupGauges(true); }

//------------------------------------------------------------------------------

int Dttune::moveDown(UINT id, int bot, int &right)
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

void Dttune::OnSize(UINT nType, int cx, int cy)
{
   dataDialog::OnSize(nType, cx, cy);
   if (nType == SIZE_MINIMIZED) return; // Don't waste time.
   doResize(cx, cy);
}

void Dttune::doResize(int cx, int cy)
{
   static bool oneCol = rep.gaugeColumns == 1;

   CWnd *p = GetDlgItem(IDC_VEP);
   if (p == NULL) {
      initCX = cx;
      initCY = cy;
      return; // We haven't been realized, yet.
   }

   int right = cx-5;
   int bot   = cy-3;
   int top = moveDown(IDCANCEL,   bot, right); right -= 3;
   moveDown(IDC_BURN,             bot, right); right -= 6;
   moveDown(IDC_TREQFUEL_SPIN,    bot, right);
   moveDown(IDC_TREQFUEL,         bot, right); right -= 3;
   moveDown(IDC_TREQFUEL_LABEL,   bot, right);

   int bwd = 6 + cx/40;
   if (bwd < 18) bwd = 18;
   int blt = int(cx * (oneCol ? 0.25 : 0.45)); // "Center" of the tuning screen.
   bot   +=  2;
   right  = blt - 6;
   moveDown(IDC_PAGE_D,           bot, right); right -= 3;
   moveDown(IDC_PAGE_C,           bot, right); right -= 3;
   moveDown(IDC_PAGE_B,           bot, right); right -= 3;
   moveDown(IDC_PAGE_A,           bot, right);

   CRect r;
   CWnd *v = GetDlgItem(IDC_METEREGOBAR);
   v->GetWindowRect(&r);
   ScreenToClient(&r);
   v->MoveWindow(blt, 0, bwd, cy-5);

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

   CWnd *m = GetDlgItem(IDC_WU);
   m->GetWindowRect(&r);
   ScreenToClient(&r);
   r.top = r.bottom - int((_fontSize+5) * 5);
   int pht = top - r.Height();
   m->MoveWindow(plt+12, pht-15, pwd-24, r.Height(), true);

   p->GetWindowRect(&r);
   ScreenToClient(&r);
   p->MoveWindow(plt,         0, pwd,    pht-27,     true);

   Invalidate();
   setupAllGeometry();
}

//------------------------------------------------------------------------------
