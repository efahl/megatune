//------------------------------------------------------------------------------
//--  Copyright (c) 2004,2005,2006 by Eric Fahlgren, All Rights Reserved.     --
//--                                                                          --
//--  This program is free software; you can redistribute it and/or           --
//--  modify it under the terms of the GNU General Public License             --
//--  as published by the Free Software Foundation; either version            --
//--  2 of the License, or (at your option) any later version.                --
//--  See http://www.gnu.org/licenses/gpl.txt                                 --
//------------------------------------------------------------------------------

static char *rcsId() { return "$Id$"; }

#include "stdafx.h"
#include "megatune.h"
#include "msDatabase.h"
#include "tableEditor.h"
#include "Dconplot.h"
#include "scaleVE.h"
#include "transformTable.h"
#include "DveGen.h"
#include "repository.h"
#include "userMenu.h"
#include "userHelp.h"

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------

extern msDatabase   mdb;
extern repository   rep;
extern tuningDialog ut[tuningDialog::nUT];
extern userMenuList uml;
extern userHelpList uhl;

//------------------------------------------------------------------------------

tableEditor::tableEditor(int nId, CWnd *pParent)
 : dataDialog(tableEditor::IDD, pParent)
 , nId       (nId)
 , _colored  (rep.fieldColoring())
 , _changed  (false)
 , rSelected (-1)
 , cSelected (-1)
 , rangeSelected(false)
{
   td = ut+nId;
   //{{AFX_DATA_INIT(tableEditor)
   //}}AFX_DATA_INIT
}

tableEditor::~tableEditor()
{
   for (int c = 0; c < td->nX();          c++) delete xBox[c];
   for (int r = 0; r < td->nY();          r++) delete yBox[r];
   for (int i = 0; i < td->nX()*td->nY(); i++) delete zBox[i];
   delete [] xBox;
   delete [] yBox;
   delete [] zBox;

   delete [] cellLft;
   delete [] cellTop;
}

//------------------------------------------------------------------------------

void tableEditor::DoDataExchange(CDataExchange *pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(tableEditor)
      DDX_Control(pDX, IDC_XBIN, m_xLabel);
      DDX_Control(pDX, IDC_YBIN, m_yLabel);
      DDX_Control(pDX, IDC_ZVAL, m_zLabel);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(tableEditor, dataDialog)
   //{{AFX_MSG_MAP(tableEditor)
      ON_WM_CTLCOLOR  ()
      ON_WM_ERASEBKGND()
      ON_WM_PAINT     ()
      ON_WM_LBUTTONUP ()
      ON_WM_LBUTTONDOWN()
      ON_WM_MOUSEMOVE ()
      ON_WM_CANCELMODE()

      // File menu
      ON_COMMAND(ID_TABLE_GET,        OnTableGet)
      ON_COMMAND(ID_TABLE_PUT,        OnTablePut)
      ON_COMMAND(ID_VEEXPORT,         OnVeExport)
      ON_COMMAND(ID_VEIMPORT,         OnVeImport)

      // Tools menu
      ON_COMMAND(ID_GEN_VE,           OnGenerateVE)
      ON_COMMAND(ID_SCALE_VE,         OnScaleVE)
      ON_COMMAND(ID_XFORM_TABLE,      OnShiftTable)
      ON_COMMAND(ID_PLOT_VE,          OnPlotVE)

      ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_XBIN0001, IDC_XBIN0032, dbFromFields)
      ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_YBIN0001, IDC_YBIN0032, dbFromFields)
      ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_ZVAL0001, IDC_ZVAL1024, dbFromFields)
      ON_CONTROL_RANGE(EN_CHANGE,    IDC_XBIN0001, IDC_XBIN0032, tagChanged)
      ON_CONTROL_RANGE(EN_CHANGE,    IDC_YBIN0001, IDC_YBIN0032, tagChanged)
      ON_CONTROL_RANGE(EN_CHANGE,    IDC_ZVAL0001, IDC_ZVAL1024, tagChanged)

      ON_COMMAND(IDC_TE_SET, OnTeSet)
      ON_COMMAND(IDC_TE_MUL, OnTeMul)
      ON_COMMAND(IDC_TE_SUB, OnTeSub)
      ON_COMMAND(IDC_TE_ADD, OnTeAdd)

   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

//BEGIN_EVENTSINK_MAP(tableEditor, dataDialog)
//   //{{AFX_EVENTSINK_MAP(tableEditor)
//   //}}AFX_EVENTSINK_MAP
//END_EVENTSINK_MAP()

//------------------------------------------------------------------------------

BOOL tableEditor::OnInitDialog()
{
   _flicker  = true;
   dataDialog::OnInitDialog(td->title);
   setHelp(td->tableHelp());

   uml.attach(this, td->tableId());

   //---------------------------------------------------------------------------

// CString s = CString(td->title);
// s.MakeUpper();
// if (s.Find("VE") == -1) {
//    GetMenu()->EnableMenuItem(ID_VEEXPORT, MF_BYCOMMAND | MF_GRAYED);
//    GetMenu()->EnableMenuItem(ID_VEIMPORT, MF_BYCOMMAND | MF_GRAYED);
//    GetMenu()->EnableMenuItem(ID_GEN_VE,   MF_BYCOMMAND | MF_GRAYED);
//    GetMenu()->EnableMenuItem(ID_SCALE_VE, MF_BYCOMMAND | MF_GRAYED);
// }

   //---------------------------------------------------------------------------
   int dWd = /*td->nX() <= 8 ? 0 :*/ (td->nX()-8)*44; // 46
   int dHt = /*td->nY() <= 8 ? 0 :*/ (td->nY()-8)*18; // Was 30 for a long time, but 12x12 was taller than 640x480 screens.
   
   CRect w;
   GetDesktopWindow()->GetWindowRect(&w);
   int ww = w.Width();
   int wh = w.Height();

   GetWindowRect(&w);
   w.InflateRect(dWd/2, dHt/2);
   w.bottom -= 25;
   if (w.Width () > ww) { w.left = 0; w.right  = ww; }
   if (w.Height() > wh) { w.top  = 0; w.bottom = wh; }
   MoveWindow(w);
   ww = w.Width();
   wh = w.Height();
   CenterWindow(GetDesktopWindow());

   CWnd *b = GetDlgItem(IDC_ZVAL);
   b->GetWindowRect(&w); ScreenToClient(&w);
   w.right += dWd; w.bottom += dHt;
   b->MoveWindow(w);
   int H = w.Height() / (td->nY()+1);
   int W = w.Width()  / (td->nX()+1);
   int B = w.bottom - int((w.Height() - H*td->nY())/2.0);
   int L = w.left   + int((w.Width()  - W*td->nX())/2.0);
   cellH = H - 2;
   cellW = W - 2;

   b = GetDlgItem(IDC_YBIN);
   b->GetWindowRect(&w); ScreenToClient(&w);
   w.bottom += dHt;
   b->MoveWindow(w);
   int Wy = W; // w.Width()  / 1;
   int Ly = w.left   + int((w.Width()-Wy)/2.0);
   int Py = w.Width();
   int Qy = w.left;

   b = GetDlgItem(IDC_XBIN);
   b->GetWindowRect(&w); ScreenToClient(&w);
   w.right += dWd; w.top += dHt; w.bottom += dHt;
   int T = w.top;
   b->MoveWindow(w);
   int Hx = H; // w.Height() / 1;
   int Bx = w.bottom - 7; //int((w.Height()-Hx)/2.0);

   b = GetDlgItem(IDC_COMMITTED);
   b->GetWindowRect(&w); ScreenToClient(&w);
   w.right  = Qy + Py;        w.left = Qy;
   w.bottom = T + w.Height()+H-8; w.top  = T+H-8;
   b->ShowWindow(false);
   b->MoveWindow(w);

   b = GetDlgItem(IDC_BURN);
   b->ShowWindow(false);
   int hh = w.Height();
   w.top += hh + 2; w.bottom += hh + 2;
   b->MoveWindow(w);

   b = GetDlgItem(IDC_TE_VAL); b->ShowWindow(false);
   b->GetWindowRect(&w); ScreenToClient(&w);
   int bs = ww - w.right;
   w.OffsetRect(bs, 0); b->MoveWindow(w);
   b = GetDlgItem(IDC_TE_SET); b->ShowWindow(false);
   b->GetWindowRect(&w); ScreenToClient(&w); w.OffsetRect(bs, 0); b->MoveWindow(w);
   b = GetDlgItem(IDC_TE_MUL); b->ShowWindow(false);
   b->GetWindowRect(&w); ScreenToClient(&w); w.OffsetRect(bs, 0); b->MoveWindow(w);
   b = GetDlgItem(IDC_TE_SUB); b->ShowWindow(false);
   b->GetWindowRect(&w); ScreenToClient(&w); w.OffsetRect(bs, 0); b->MoveWindow(w);
   b = GetDlgItem(IDC_TE_ADD); b->ShowWindow(false);
   b->GetWindowRect(&w); ScreenToClient(&w); w.OffsetRect(bs, 0); b->MoveWindow(w);

   //---------------------------------------------------------------------------
   CFont *font = this->GetFont();
   DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_RIGHT | ES_AUTOHSCROLL;
         // | ES_NUMBER???

   cellTop = new int[td->nY()];
   int iX, iY;
   for (iY = 0; iY < td->nY(); iY++) {
      cellTop[iY] = B-iY*H - cellH;
   }
   cellLft = new int[td->nX()];
   for (iX = 0; iX < td->nX(); iX++) {
      cellLft[iX] = L+iX*W;
   }

   zBox = new Cell*[td->nX()*td->nY()];
   CRect l;
   for (iY = 0; iY < td->nY(); iY++) {
      l.top    = cellTop[iY];
      l.bottom = l.top+cellH;

      for (iX = 0; iX < td->nX(); iX++) {
         int b     = iY*td->nX() + iX;
         l.left    = cellLft[iX];
         l.right   = l.left+cellW;

         zBox[b] = new Cell(iY, iX, b, td->_iZVAL(iX,iY));
         zBox[b]->CreateEx(WS_EX_CLIENTEDGE, "Edit", NULL, style, l, this, IDC_ZVAL0001+b);
         zBox[b]->SetFont(font);
         zBox[b]->EnableWindow(td->zEditable);
      }
   }

   yBox = new Cell*[td->nY()];
   l.left  = Ly;
   l.right = Ly+cellW;
   for (iY = 0; iY < td->nY(); iY++) {
      l.top    = cellTop[iY];
      l.bottom = l.top+cellH;
      yBox[iY] = new Cell(iY, -1, iY, iY);
      yBox[iY]->CreateEx(WS_EX_CLIENTEDGE, "Edit", NULL, style, l, this, IDC_YBIN0001+iY);
      yBox[iY]->SetFont(font);
      yBox[iY]->EnableWindow(td->yEditable);
   }

   xBox = new Cell*[td->nX()];
   l.bottom = Bx;
   l.top    = Bx-cellH;
   for (iX = 0; iX < td->nX(); iX++) {
      l.left    = cellLft[iX];
      l.right   = l.left+cellW;
      xBox[iX] = new Cell(-1, iX, iX, iX);
      xBox[iX]->CreateEx(WS_EX_CLIENTEDGE, "Edit", NULL, style, l, this, IDC_XBIN0001+iX);
      xBox[iX]->SetFont(font);
      xBox[iX]->EnableWindow(td->xEditable);
   }

   //---------------------------------------------------------------------------

   m_xLabel.SetWindowText(td->x->units());
   m_yLabel.SetWindowText(td->y->units());
   m_zLabel.SetWindowText(td->z->units());

   fieldsFromDb();
   recalcMinMax();
   td->reset();

// zBox[0]->SetFocus(); // Keeps old value after veImport if this is done?

   return FALSE;
}

//------------------------------------------------------------------------------

void tableEditor::recalcMinMax()
{
   zMin = zMax = td->ZVAL(0, 0);
   int r, c;
   for (c = 0; c < td->nX(); c++) {
      for (r = 0; r < td->nY(); r++) {
         double zVal = td->ZVAL(c, r);
         if (zVal < zMin) zMin = zVal;
         if (zVal > zMax) zMax = zVal;
      }
   }
}

//------------------------------------------------------------------------------

HBRUSH tableEditor::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
   HBRUSH        hbr;
   static CBrush brush;
   COLORREF      color = 0;

   Cell *pCel = dynamic_cast<Cell *>(pWnd);
   if (pCel == NULL) { //-- Non-cell widget ------------------------
      if (pWnd->GetDlgCtrlID() != IDC_COMMITTED && pWnd->GetDlgCtrlID() != IDC_BURN) {
         hbr = dataDialog::OnCtlColor(pDC, pWnd, nCtlColor);
         pDC->SetBkMode(OPAQUE);
      }
      else {
         pDC->SetTextColor(RGB(0,0,0));
         pDC->SetBkColor(RGB(255,0,0));

         static CBrush brush;
         brush.DeleteObject();
         brush.CreateSolidBrush(RGB(255,0,0));
         hbr = brush;
      }
   }

   else if (pCel->row < 0) { //-- X cells --------------------------------------
      pDC->SetBkMode(TRANSPARENT);
      if (cSelected < 0 || pCel->col != cSelected)
         hbr = dataDialog::OnCtlColor(pDC, pWnd, nCtlColor);
      else {
         color = rep.t_spotColor;
         pDC->SetTextColor(rep.t_spotText);
      }
   }

   else if (pCel->col < 0) { //-- Y cells --------------------------------------
      pDC->SetBkMode(TRANSPARENT);
      if (rSelected < 0 || pCel->row != rSelected)
         hbr = dataDialog::OnCtlColor(pDC, pWnd, nCtlColor);
      else {
         color = rep.t_spotColor;
         pDC->SetTextColor(rep.t_spotText);
      }
   }

   else { //-- Z cells ---------------------------------------------------------
      pDC->SetBkMode(TRANSPARENT);
      if (rSelected < 0 || cSelected < 0 || rSelected != pCel->row || cSelected != pCel->col) {
         if (!_colored)
            hbr = dataDialog::OnCtlColor(pDC, pWnd, nCtlColor);
         else {
            if (!pCel->pick)
               Dconplot::color((td->ZVAL(pCel->col, pCel->row)-zMin) / (zMax-zMin), color, 0.4);
            else {
               Dconplot::color((td->ZVAL(pCel->col, pCel->row)-zMin) / (zMax-zMin), color, 1.0, 0.5);
               pDC->SetTextColor(RGB(255,255,255)); // Put this in ini file.
            }
         }
      }
      else {
         color = rep.t_spotColor;
         pDC->SetTextColor(rep.t_spotText);
      }
   }

   if (color != 0) {
      brush.DeleteObject();
      brush.CreateSolidBrush(color);
      hbr = brush;
   }

   return hbr;
}

//------------------------------------------------------------------------------

static CBrush *box = CBrush::FromHandle(::GetSysColorBrush(0));
static int     lw  = 2;
static CSize   lws = CSize(lw, lw);

void tableEditor::snapRect(CRect &r)
{
   r.NormalizeRect();

   int iX, nX = td->nX();
   if (r.left  < cellLft[0]) { r.left = cellLft[0]-lw;   }
   if (r.right < r.left    ) { r.SetRectEmpty(); return; }

   for (iX = 0; iX < nX; iX++) {
      if (r.left <= cellLft[iX]+cellW-1) {
         r.left = cellLft[iX]-lw;
         break;
      }
   }
   for ( ; iX < nX; iX++) {
      if (r.right <= cellLft[iX]+cellW-1) {
         r.right = cellLft[iX]+cellW+lw;
         break;
      }
   }
   if (r.right > cellLft[nX-1]) r.right = cellLft[nX-1]+cellW+lw;
   if (r.left  > r.right      ) r.left  = r.right;

   int iY, nY = td->nY();
   if (r.top    < cellTop[nY-1]) { r.top = cellTop[nY-1]-lw; }
   if (r.bottom < r.top        ) { r.SetRectEmpty(); return; }

   for (iY = nY-1; iY >= 0; iY--) {
      if (r.top <= cellTop[iY]+cellH-1) {
         r.top = cellTop[iY]-lw;
         break;
      }
   }
   for (; iY >= 0; iY--) {
      if (r.bottom <= cellTop[iY]+cellH-1) {
         r.bottom = cellTop[iY]+cellH+lw;
         break;
      }
   }
   if (r.bottom > cellTop[0]) r.bottom = cellTop[0]+cellH+lw;
   if (r.top    > r.bottom  ) r.top    = r.bottom;
}

void tableEditor::OnLButtonDown(UINT nFlags, CPoint point)
{
   dataDialog::OnLButtonDown(nFlags, point);

   SetCapture();
   CDC *pDC = GetDC();
      m_firstClick = point;
      m_dragRect   = CRect(point, point);
      snapRect(m_dragRect);
      pDC->DrawDragRect(&m_dragRect, lws, NULL, lws, box, box);
   ReleaseDC(pDC);
}

void tableEditor::OnMouseMove(UINT nFlags, CPoint point)
{
   if (GetCapture() == this) {
      CDC *pDC = GetDC();
         CRect rect(m_firstClick, point);
         snapRect(rect);

         pDC->DrawDragRect(&rect, lws, &m_dragRect, lws, box, box);
         m_dragRect = rect;
      ReleaseDC(pDC);
   }

   dataDialog::OnMouseMove(nFlags, point);
}

void tableEditor::OnLButtonUp(UINT nFlags, CPoint point)
{
   if (GetCapture() == this) {
      ReleaseCapture();

      rangeSelected = false;
      for (int iT = 0; iT < td->nX()*td->nY(); iT++) {
         CRect pick(m_dragRect);
         CRect cell;
         zBox[iT]->GetWindowRect(&cell);
         ScreenToClient(&cell);
         zBox[iT]->pick = pick.IntersectRect(pick, cell) != 0;
         if (zBox[iT]->pick) rangeSelected = true;
      }
      GetDlgItem(IDC_TE_VAL)->ShowWindow(rangeSelected);
      GetDlgItem(IDC_TE_SET)->ShowWindow(rangeSelected);
      GetDlgItem(IDC_TE_MUL)->ShowWindow(rangeSelected);
      GetDlgItem(IDC_TE_SUB)->ShowWindow(rangeSelected);
      GetDlgItem(IDC_TE_ADD)->ShowWindow(rangeSelected);

      CDC *pDC = GetDC();
         CRect rect(0,0,0,0);
         pDC->DrawDragRect(rect, lws, &m_dragRect, lws, box, box);
      ReleaseDC(pDC);

      Invalidate();
   }

   dataDialog::OnLButtonUp(nFlags, point);
}

void tableEditor::OnCancelMode()
{
   if (GetCapture() == this) {
      m_dragRect.SetRectEmpty();
      CDC * pDC = GetDC();
         CRect rect(0, 0, 0, 0);
         pDC->DrawDragRect(rect, lws, &m_dragRect, lws, NULL, NULL);
      ReleaseDC(pDC);
      ReleaseCapture();
   }
   dataDialog::OnCancelMode();
}

//------------------------------------------------------------------------------

void tableEditor::OnTableGet()
{
   GetDlgItem(IDC_BURN)->ShowWindow(false);
   fieldsFromDb();
   recalcMinMax();
}

//------------------------------------------------------------------------------

void tableEditor::OnTablePut()
{
   int savePage = mdb.setPageNo(td->pageNo, true);
      mdb.cDesc.send(td->x);
      mdb.cDesc.send(td->y);
      mdb.cDesc.send(td->z);
      mdb.burnConst(td->pageNo);
   mdb.setPageNo(savePage);
   GetDlgItem(IDC_BURN)->ShowWindow(false);
}

//------------------------------------------------------------------------------

void tableEditor::fieldsFromDb()
{
   mdb.getPage(td->pageNo);

   for (int iX = 0; iX < td->nX(); iX++) {
      setFld(xBox[iX], td->x, xBox[iX]->idx);
   }

   for (int iY = 0; iY < td->nY(); iY++) {
      setFld(yBox[iY], td->y, yBox[iY]->idx);
   }

   for (int iT = 0; iT < td->nX()*td->nY(); iT++) {
      setFld(zBox[iT], td->z, zBox[iT]->idx);
   }

   _changed = false;
   GetDlgItem(IDC_COMMITTED)->ShowWindow(false);
}

//------------------------------------------------------------------------------

void tableEditor::tagChanged(UINT nId)
{
   _changed = true;
   GetDlgItem(IDC_COMMITTED)->ShowWindow(true);
}

void tableEditor::dbFromFields(UINT nId)
{
   if (_changed) {
      GetDlgItem(IDC_COMMITTED)->ShowWindow(false);
      GetDlgItem(IDC_BURN     )->ShowWindow(true);

      int iT;

      if (nId >= IDC_ZVAL0001 && nId <= UINT(IDC_ZVAL0001+td->nZ())) {
         iT = nId - IDC_ZVAL0001;
         getFld(zBox[iT], td->z, zBox[iT]->idx);
         if (_colored) {
            recalcMinMax();
            Invalidate();    // Update,
            _flicker = false; // but not too much...
         }
      }

      if (nId >= IDC_XBIN0001 && nId <= UINT(IDC_XBIN0001+td->nX())) {
         iT = nId - IDC_XBIN0001;
         getFld(xBox[iT], td->x, xBox[iT]->idx);
      }

      if (nId >= IDC_YBIN0001 && nId <= UINT(IDC_YBIN0001+td->nY())) {
         iT = nId - IDC_YBIN0001;
         getFld(yBox[iT], td->y, yBox[iT]->idx);
      }
   }

   _changed = false;
}

//------------------------------------------------------------------------------

void tableEditor::OnPlotVE()
{
   Dconplot plot(nId);
      plot.DoModal();
}

//------------------------------------------------------------------------------

void tableEditor::OnScaleVE()
{
   scaleVe sve(nId);
      sve.DoModal();
   fieldsFromDb();
   recalcMinMax();
}

//------------------------------------------------------------------------------

void tableEditor::OnShiftTable()
{
   transformTable st(nId);
      st.DoModal();
   fieldsFromDb();
   recalcMinMax();
}

//------------------------------------------------------------------------------

void tableEditor::xformCells(double scale, double shift)
{
   bool changes = false;
   for (int iT = 0; iT < td->nX()*td->nY(); iT++) {
      if (zBox[iT]->pick) {
         int idx = zBox[iT]->idx;
         td->z->storeValue(scale*td->z->valueUser(idx)+shift, idx);
         changes = true;
      }
   }
   if (changes) {
      mdb.cDesc.send(td->z);
      fieldsFromDb();
      recalcMinMax();
      GetDlgItem(IDC_BURN)->ShowWindow(true);
   }
}

void tableEditor::OnTeSet()
{
   double x = getRaw(static_cast<CEdit *>(GetDlgItem(IDC_TE_VAL)));
   xformCells(0.0, x);
}

void tableEditor::OnTeMul()
{
   double x = getRaw(static_cast<CEdit *>(GetDlgItem(IDC_TE_VAL)));
   xformCells(x, 0.0);
}

void tableEditor::OnTeSub()
{
   double x = getRaw(static_cast<CEdit *>(GetDlgItem(IDC_TE_VAL)));
   xformCells(1.0, -x);
}

void tableEditor::OnTeAdd()
{
   double x = getRaw(static_cast<CEdit *>(GetDlgItem(IDC_TE_VAL)));
   xformCells(1.0, x);
}

//------------------------------------------------------------------------------

BOOL tableEditor::OnEraseBkgnd(CDC *pDC)
{
   if (_flicker) {
      return dataDialog::OnEraseBkgnd(pDC);
   }
   _flicker = true;
   return TRUE;
}

//------------------------------------------------------------------------------

void tableEditor::OnVeExport()
{
   mdb.veExport(nId);
}

void tableEditor::OnVeImport()
{
   mdb.veImport(nId);
   fieldsFromDb();
   recalcMinMax();
}

//------------------------------------------------------------------------------

void tableEditor::OnGenerateVE()
{
   DveGen gve(nId);
      gve.DoModal();
   fieldsFromDb();
   recalcMinMax();
}

//------------------------------------------------------------------------------

bool tableEditor::doTimer(bool connected)
{
   if (!realized()) return false;

   if (connected) {
      td->update();
      if (td->nearestCellMoved()) {
         if (                  cSelected >= 0) xBox[                   cSelected]->Invalidate();
         if (rSelected >= 0                  ) yBox[rSelected                   ]->Invalidate();
         if (rSelected >= 0 && cSelected >= 0) zBox[rSelected*td->nX()+cSelected]->Invalidate();
         cSelected = td->iXBIN;
         rSelected = td->iYBIN;
         if (                  cSelected >= 0) xBox[                   cSelected]->Invalidate();
         if (rSelected >= 0                  ) yBox[rSelected                   ]->Invalidate();
         if (rSelected >= 0 && cSelected >= 0) zBox[rSelected*td->nX()+cSelected]->Invalidate();
      }
   }

   return true;
}

//------------------------------------------------------------------------------

BOOL tableEditor::PreTranslateMessage(MSG *pMsg)
{
   static bool shifted = false;
   static bool ctrled  = false;
          bool alted   = (HIWORD(pMsg->lParam) & KF_ALTDOWN) != 0;

   if (pMsg->message == WM_SYSKEYUP) {
      switch (pMsg->wParam) {
         case 'B':     OnTablePut(); return TRUE;
         case 'C':
            if (alted) PostMessage(WM_CLOSE, 0, 0);
            return TRUE;
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

         case VK_SPACE:
            mdb.markLog();
            return TRUE;

         case 'A': // Stuff to ignore so we don't mangle numeric values.
         case 'B':
         case 'C':
         case 'D':
         case 'E':
         case 'F':
         case 'G':
         case 'H':
         case 'I':
         case 'J':
         case 'K':
         case 'L':
         case 'M':
         case 'N':
         case 'O':
         case 'P':
         case 'Q':
         case 'R':
         case 'S':
         case 'T':
         case 'U':
         case 'V':
         case 'X':
         case 'Y':
         case 'Z':
            return TRUE;

//       case 'F': // Find command.
//       case 'G': // "Go to spot" mode.
//       case 'S': let this go through for now, make it work like tuning dialog.
         case VK_RETURN:
            OnKeyDown(pMsg->wParam, 1, pMsg->lParam >> 16);
            return TRUE;
      }
   }
   return dataDialog::PreTranslateMessage(pMsg);
}

//------------------------------------------------------------------------------

