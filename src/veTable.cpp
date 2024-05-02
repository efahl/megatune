//------------------------------------------------------------------------------
//--  Copyright (c) 2004 by Eric Fahlgren, All Rights Reserved.               --
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
#include "veTable.h"
#include "Dconplot.h"
#include "scaleVE.h"
#include "transformTable.h"
#include "DveGen.h"
#include "repository.h"

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------

extern msDatabase   mdb;
extern repository   rep;
extern tuningDialog ut[tuningDialog::nUT];

//------------------------------------------------------------------------------

veTable::veTable(int nId, CWnd *pParent)
 : dataDialog(veTable::IDD, pParent)
 , nId       (nId)
 , colored   (rep.fieldColoring())
{
   td = ut+nId;
   nX = td->x->nValues();
   nY = td->y->nValues();
   //{{AFX_DATA_INIT(veTable)
   //}}AFX_DATA_INIT
}

veTable::~veTable()
{
   for (int c = 0; c < nX;    c++) delete xBox[c];
   for (int r = 0; r < nY;    r++) delete yBox[r];
   for (int i = 0; i < nX*nY; i++) delete zBox[i];
   delete [] xBox;
   delete [] yBox;
   delete [] zBox;
}

//------------------------------------------------------------------------------

void veTable::DoDataExchange(CDataExchange *pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(veTable)
      DDX_Control(pDX, IDC_XBIN, m_xLabel);
      DDX_Control(pDX, IDC_YBIN, m_yLabel);
      DDX_Control(pDX, IDC_ZVAL, m_zLabel);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(veTable, dataDialog)
   //{{AFX_MSG_MAP(veTable)
      ON_WM_CTLCOLOR()
      ON_WM_ERASEBKGND()

      ON_BN_CLICKED   (IDC_TABLE_GET, OnTableGet)
      ON_BN_CLICKED   (IDC_TABLE_PUT, OnTablePut)

      // File menu
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
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

BEGIN_EVENTSINK_MAP(veTable, dataDialog)
   //{{AFX_EVENTSINK_MAP(veTable)
   //}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

//------------------------------------------------------------------------------

BOOL veTable::OnInitDialog()
{
   flicker = true;
   dataDialog::OnInitDialog(td->title.c_str());

   //---------------------------------------------------------------------------

// CString s = CString(td->title.c_str());
// s.MakeUpper();
// if (s.Find("VE") == -1) {
//    GetMenu()->EnableMenuItem(ID_VEEXPORT, MF_BYCOMMAND | MF_GRAYED);
//    GetMenu()->EnableMenuItem(ID_VEIMPORT, MF_BYCOMMAND | MF_GRAYED);
//    GetMenu()->EnableMenuItem(ID_GEN_VE,   MF_BYCOMMAND | MF_GRAYED);
//    GetMenu()->EnableMenuItem(ID_SCALE_VE, MF_BYCOMMAND | MF_GRAYED);
// }

   //---------------------------------------------------------------------------
   int dWd = /*nX <= 8 ? 0 :*/ (nX-8)*44; // 46
   int dHt = /*nY <= 8 ? 0 :*/ (nY-8)*18; // Was 30 for a long time, but 12x12 was taller than 640x480 screens.
   
   CRect w;
   GetDesktopWindow()->GetWindowRect(&w);
   int ww = w.Width();
   int wh = w.Height();

   GetWindowRect(&w);
   w.InflateRect(dWd/2, dHt/2);
   if (w.Width () > ww) { w.left = 0; w.right  = ww; }
   if (w.Height() > wh) { w.top  = 0; w.bottom = wh; }
   MoveWindow(w);
   CenterWindow(GetDesktopWindow());

// moveItem(IDC_SCALE_VE,  dWd, dHt);
// moveItem(IDC_PLOT_VE,   dWd, dHt);
   moveItem(IDC_TABLE_GET, dWd, dHt);
   moveItem(IDC_TABLE_PUT, dWd, dHt);
   moveItem(IDCANCEL,      dWd, dHt);

   CWnd *b = GetDlgItem(IDC_ZVAL);
   b->GetWindowRect(&w); ScreenToClient(&w);
   w.right += dWd; w.bottom += dHt;
   b->MoveWindow(w);
   int H = w.Height() / (nY+1);
   int W = w.Width()  / (nX+1);
   int B = w.bottom - int((w.Height() - H*nY)/2.0);
   int L = w.left   + int((w.Width()  - W*nX)/2.0);

   b = GetDlgItem(IDC_YBIN);
   b->GetWindowRect(&w); ScreenToClient(&w);
   w.bottom += dHt;
   b->MoveWindow(w);
   int Wy = W; // w.Width()  / 1;
   int Ly = w.left   + int((w.Width()-Wy)/2.0);

   b = GetDlgItem(IDC_XBIN);
   b->GetWindowRect(&w); ScreenToClient(&w);
   w.right += dWd; w.top += dHt; w.bottom += dHt;
   b->MoveWindow(w);
   int Hx = H; // w.Height() / 1;
   int Bx = w.bottom - 5; //int((w.Height()-Hx)/2.0);

   //---------------------------------------------------------------------------
   CFont *font = this->GetFont();
   DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_RIGHT | ES_AUTOHSCROLL;
         // | ES_NUMBER???

   zBox = new Cell*[nX*nY];
   for (int r = 0; r < nY; r++) {
      CRect l;
      l.bottom = B-r*H;
      l.top    = l.bottom - H;

      for (int c = 0; c < nX; c++) {
         int b     = r*nX + c;

         l.left    = L+c*W;
         l.right   = l.left + W;

         zBox[b] = new Cell(r, c, b, td->z->index(c,r));
         zBox[b]->CreateEx(WS_EX_CLIENTEDGE, "Edit", NULL, style, l, this, IDC_ZVAL0001+b);
         zBox[b]->SetFont(font);
         zBox[b]->EnableWindow(td->zEditable);
      }
   }

   yBox = new Cell*[nY];
   for (r = 0; r < nY; r++) {
      CRect l(Ly,B-r*H-H,Ly+Wy,B-r*H);
      yBox[r] = new Cell(r, -1, r, r);
      yBox[r]->CreateEx(WS_EX_CLIENTEDGE, "Edit", NULL, style, l, this, IDC_YBIN0001+r);
      yBox[r]->SetFont(font);
      yBox[r]->EnableWindow(td->yEditable);
   }

   xBox = new Cell*[nX];
   for (int c = 0; c < nX; c++) {
      CRect l(L+c*W, Bx-Hx, L+c*W+W, Bx);
      xBox[c] = new Cell(-1, c, c, c);
      xBox[c]->CreateEx(WS_EX_CLIENTEDGE, "Edit", NULL, style, l, this, IDC_XBIN0001+c);
      xBox[c]->SetFont(font);
      xBox[c]->EnableWindow(td->xEditable);
   }

   //---------------------------------------------------------------------------

   m_xLabel.SetWindowText(td->x->units().c_str());
   m_yLabel.SetWindowText(td->y->units().c_str());
   m_zLabel.SetWindowText(td->z->units().c_str());

   fieldsFromDb();
   recalcMinMax();

// zBox[0]->SetFocus(); // Keeps old value after veImport if this is done?

   return FALSE;
}

//------------------------------------------------------------------------------

void veTable::OnDestroy()
{
   dataDialog::OnDestroy();
}

//------------------------------------------------------------------------------

void veTable::recalcMinMax()
{
   zMin = zMax = td->ZVAL(0, 0);
   int r, c;
   for (c = 0; c < nX; c++) {
      for (r = 0; r < nY; r++) {
         double zVal = td->ZVAL(c, r);
         if (zVal < zMin) zMin = zVal;
         if (zVal > zMax) zMax = zVal;
      }
   }
}

//------------------------------------------------------------------------------

HBRUSH veTable::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
   HBRUSH hbr;

   pDC->SetBkMode(TRANSPARENT);

   Cell *pCel = dynamic_cast<Cell *>(pWnd);
   if (pCel == NULL || !colored || pCel->row < 0 || pCel->col < 0)
      hbr = dataDialog::OnCtlColor(pDC, pWnd, nCtlColor);
   else {
      COLORREF color;
      Dconplot::color((td->ZVAL(pCel->col, pCel->row)-zMin) / (zMax-zMin), color, 0.3);

      static CBrush brush;
      brush.DeleteObject();
      brush.CreateSolidBrush(color);
      hbr = brush;
   }

   return hbr;
}

//------------------------------------------------------------------------------

void veTable::OnTableGet()
{
   fieldsFromDb();
   recalcMinMax();
}

//------------------------------------------------------------------------------

void veTable::OnTablePut()
{
   CButton *bg = static_cast<CButton *>(GetDlgItem(IDC_TABLE_GET));
   CButton *bp = static_cast<CButton *>(GetDlgItem(IDC_TABLE_PUT));
   bg->EnableWindow(FALSE);
   bp->EnableWindow(FALSE);

   int savePage = mdb.setPageNo(td->pageNo, true);
      mdb.cDesc.send(td->x);
      mdb.cDesc.send(td->y);
      mdb.cDesc.send(td->z);
      mdb.burnConst();
   mdb.setPageNo(savePage);

   ::Sleep(100); // Whatever...
   bg->EnableWindow(TRUE);
   bp->EnableWindow(TRUE);
}

//------------------------------------------------------------------------------

void veTable::fieldsFromDb()
{
   mdb.getConst(td->pageNo);

   for (int iX = 0; iX < nX; iX++) {
      setFld(xBox[iX], td->x, xBox[iX]->idx);
   }

   for (int iY = 0; iY < nY; iY++) {
      setFld(yBox[iY], td->y, yBox[iY]->idx);
   }

   for (int iT = 0; iT < nX*nY; iT++) {
      setFld(zBox[iT], td->z, zBox[iT]->idx);
   }
}

//------------------------------------------------------------------------------

void veTable::dbFromFields(UINT nId)
{
   int iT;

   if (nId >= IDC_ZVAL0001 && nId <= UINT(IDC_ZVAL0001+(nX*nY))) {
      iT = nId - IDC_ZVAL0001;
      getFld(zBox[iT], td->z, zBox[iT]->idx);
      if (colored) {
         recalcMinMax();
         Invalidate();    // Update,
         flicker = false; // but not too much...
      }
   }

   if (nId >= IDC_XBIN0001 && nId <= UINT(IDC_XBIN0001+nX)) {
      iT = nId - IDC_XBIN0001;
      getFld(xBox[iT], td->x, xBox[iT]->idx);
   }

   if (nId >= IDC_YBIN0001 && nId <= UINT(IDC_YBIN0001+nY)) {
      iT = nId - IDC_YBIN0001;
      getFld(yBox[iT], td->y, yBox[iT]->idx);
   }
}

//------------------------------------------------------------------------------

void veTable::OnPlotVE()
{
   Dconplot plot(nId);
   plot.DoModal();
}

//------------------------------------------------------------------------------

void veTable::OnScaleVE()
{
   scaleVe sve(nId);
   sve.DoModal();
   fieldsFromDb();
   recalcMinMax();
}

//------------------------------------------------------------------------------

void veTable::OnShiftTable()
{
   transformTable st(nId);
   st.DoModal();
   fieldsFromDb();
   recalcMinMax();
}

//------------------------------------------------------------------------------

BOOL veTable::OnEraseBkgnd(CDC *pDC)
{
   if (flicker) {
      return dataDialog::OnEraseBkgnd(pDC);
   }
   flicker = true;
   return TRUE;
}

//------------------------------------------------------------------------------

void veTable::OnVeExport()
{
   mdb.veExport(nId);
}

void veTable::OnVeImport()
{
   mdb.veImport(nId);
   fieldsFromDb();
   recalcMinMax();
}

//------------------------------------------------------------------------------

void veTable::OnGenerateVE()
{
   DveGen gve(nId);
   gve.DoModal();
   fieldsFromDb();
   recalcMinMax();
}

//------------------------------------------------------------------------------
