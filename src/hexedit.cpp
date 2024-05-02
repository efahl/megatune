//------------------------------------------------------------------------------
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
#include "megatune.h"
#include "hexEdit.h"
#include "msDatabase.h"

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

extern msDatabase mdb;

//------------------------------------------------------------------------------

hexEdit::hexEdit(CWnd *pParent)
 : dataDialog(hexEdit::IDD, pParent)
 , _base     (16)
 , _pageNo   (-1)
{
   //{{AFX_DATA_INIT(hexEdit)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void hexEdit::DoDataExchange(CDataExchange* pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(hexEdit)
      DDX_Control(pDX, IDC_HEX_SCROLL,   m_hexScroll);
      DDX_Control(pDX, IDC_BIN,          m_units);
      DDX_Control(pDX, IDC_COUNTER,      m_counter);
      DDX_Control(pDX, IDC_PAGE_NO_TEXT, m_pageNo);
      DDX_Control(pDX, IDC_REALTIME,     m_realtime);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(hexEdit, dataDialog)
   //{{AFX_MSG_MAP(hexEdit)
      ON_BN_CLICKED  (IDC_FETCH,    OnFetch)
      ON_BN_CLICKED  (IDC_BIN,      OnBinary)
      ON_BN_CLICKED  (IDC_DEC,      OnDecimal)
      ON_BN_CLICKED  (IDC_HEX,      OnHexadecimal)
      ON_BN_CLICKED  (IDC_BURN,     OnBurn)
      ON_NOTIFY(UDN_DELTAPOS, IDC_PAGE_NO, OnPageNo)
      ON_WM_CTLCOLOR ()
      ON_WM_DESTROY  ()
      ON_WM_VSCROLL  ()
   //}}AFX_MSG_MAP
   ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_ZVAL0001, IDC_ZVAL1024, OnCell)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

static char *Bin(int n)
{
   static char d[10];
   int mask = 0x80;
   d[0] = '%';
   for (int i = 0; i < 8; i++) {
      d[i+1] = (mask & n) ? '1' : '0';
      mask = mask >> 1;
   }
   d[9] = 0;
   return d;
}

static char *Dec(int n)
{
   static char d[10];
   sprintf(d, "%d", n);
   return d;
}

static char *Hex(int n, int w=2)
{
   static char h[10];
   sprintf(h, "$%0*X", w, n);
   return h;
}

char *hexEdit::Fmt(int n)
{
   switch (_base) {
      case  2: return Bin(n);
      case 10: return Dec(n);
      case 16: return Hex(n);
      default: return "???";
   }
}

//------------------------------------------------------------------------------

void hexEdit::updateDisplay(bool force)
{
   for (int i = 0; i < nCell; i++) {
      unsigned char b = inputBuffer[i];
      if (force || displayBuffer[i] != b) {
         displayBuffer[i] = b;
         bool hasText = i < _tableLast;
         hx[i]->SetWindowText(hasText ? Fmt(b) : "");
         hx[i]->EnableWindow (hasText);
      }
   }
}

//------------------------------------------------------------------------------

void hexEdit::doScroll()
{
   _tableLast = _tableSize - _scrollPos*nCol;
   if (_tableLast > nCell) _tableLast = nCell;

   for (int i = 0; i < _tableLast; i++) inputBuffer[i] = mdb.cDesc.getB(_pageNo, i+_scrollPos*nCol);
   updateDisplay(true);
   for (int r = 0; r < nRow; r++) {
      rl[r]->SetWindowText(Hex((r+_scrollPos)<<8, 4));
   }
}

//------------------------------------------------------------------------------

void hexEdit::setPage(int pageNo)
{
   if (pageNo <  0                 ) pageNo = 0;
   if (pageNo >= mdb.cDesc.nPages()) pageNo = mdb.cDesc.nPages()-1;
   if (pageNo == _pageNo) return;

   mdb.setPageNo(pageNo, true);
   mdb.getConst (pageNo);

   _pageNo    = pageNo;
   _tableSize = mdb.cDesc.pageSize(_pageNo);
   _scrollPos = 0;
   _scrollBot = (_tableSize+nCol-1)/nCol - nRow;
   if (_scrollBot < 0) _scrollBot = 0;

   CString title;
   title.Format("Page %d (%d bytes)", _pageNo+1, _tableSize);
   m_pageNo.SetWindowText(title);

   m_hexScroll.SetScrollRange(0, _scrollBot);
   if (_scrollBot == 0) m_hexScroll.ShowScrollBar(false);

   doScroll();
}

//------------------------------------------------------------------------------

BOOL hexEdit::OnInitDialog()
{
   dataDialog::OnInitDialog("Table Editor");

   m_counter.SetWindowText("");

   CFont   *f = this->GetFont();

   CRect w;
   GetWindowRect(w);
   int ww = w.Width();
   int wh = w.Height();

   H = (wh-77)/(nRow+1);
   W = (ww-41)/(nCol+1);

   int r, c;

   // Make the row labels.
   for (r = 0; r < nRow; r++) {
      CRect l;
      l.top  = (H+7)+r*H;  l.bottom = l.top+H+1;
      l.left = 1;          l.right  = l.left+W-1;
      rl[r]  = new CStatic();
      rl[r]->Create("", WS_CHILD | WS_VISIBLE | ES_RIGHT, l, this, 1);
      rl[r]->SetFont(f);
   }

   // Make the column labels.
   for (c = 0; c < nCol; c++) {
      CRect l;
      l.top  = 6;                   l.bottom = l.top+H+1;
      l.left = (W+(c<8?6:10))+c*W;  l.right  = l.left+W-4;
      cl[c]  = new CStatic();
      cl[c]->Create(Hex(c), WS_CHILD | WS_VISIBLE | ES_RIGHT, l, this, 1);
      cl[c]->SetFont(f);
   }

   for (r = 0; r < nRow; r++) {
      CRect l;
      l.top    = (H+3)+r*H;
      l.bottom = l.top+H+1;

      for (c = 0; c < nCol; c++) {
         int b     = r*nRow + c;

         l.left    = (W+(c<8?5:9))+c*W;
         l.right   = l.left+W+2;

         DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_RIGHT | ES_AUTOHSCROLL;

         hx[b] = new Cell(r, c, b, b);
         hx[b]->CreateEx(WS_EX_CLIENTEDGE, "Edit", NULL, style, l, this, IDC_ZVAL0001+b);
         hx[b]->SetFont(f);
      }
   }

   static bool firstTime = true;
   if (firstTime) {
      static_cast<CButton *>(GetDlgItem(IDC_HEX))->SetCheck(true);
      firstTime = false;
   }

   _pageNo = -1;
   setPage(0);
   hx[0]->SetFocus();

   return FALSE;
}

//------------------------------------------------------------------------------

void hexEdit::OnDestroy()
{
   dataDialog::OnDestroy();

   for (int r = 0; r < nRow;  r++) delete rl[r];
   for (int c = 0; c < nCol;  c++) delete cl[c];
   for (int i = 0; i < nCell; i++) delete hx[i];
}

//------------------------------------------------------------------------------

void hexEdit::fetch()
{
   mdb.getConst(_pageNo);
   for (int i = 0; i < _tableLast; i++) inputBuffer[i] = mdb.cDesc.getB(_pageNo, i+_scrollPos*nCol);
}

void hexEdit::OnFetch()
{
   fetch();
   updateDisplay();
}

//------------------------------------------------------------------------------

void hexEdit::OnBurn()
{
   mdb.burnConst();
}

//------------------------------------------------------------------------------

bool hexEdit::doTimer(bool connected)
{
   if (!m_realtime.GetCheck()) return false;

   if (connected) {
      OnFetch(); // Just duplicate the button click on timer event.

      static int c = 0;
      m_counter.SetWindowText(Dec(c++));
   }

   return true;
}

//------------------------------------------------------------------------------

void hexEdit::OnBinary     () { _base =  2; updateDisplay(true); } 
void hexEdit::OnDecimal    () { _base = 10; updateDisplay(true); } 
void hexEdit::OnHexadecimal() { _base = 16; updateDisplay(true); }

//------------------------------------------------------------------------------

HBRUSH hexEdit::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
   HBRUSH hbr;

   pDC->SetBkMode(TRANSPARENT);

   Cell *pCel = dynamic_cast<Cell *>(pWnd);
   if (pCel == NULL)
      hbr = dataDialog::OnCtlColor(pDC, pWnd, nCtlColor);
   else {
      COLORREF color = RGB(208, 208, 208);
      if (pCel->ofs < nCell) {
         int r = pCel->row+_scrollPos;
         int c = 0;//pCel->col;
         color = (r%2) ? RGB(225-4*c, 225-4*c, 255) : RGB(255,255,255);
      }
      static CBrush brush;
      brush.DeleteObject();
      brush.CreateSolidBrush(color);
      hbr = brush;
   }

   return hbr;
}

//------------------------------------------------------------------------------

void hexEdit::OnCell(UINT nId)
{
   Cell *pCel = hx[nId-IDC_ZVAL0001];

   char number[20];
   pCel->GetWindowText(number, sizeof(number));

   BYTE n;
   if (msDatabase::number(number, n)) {
      if (n != displayBuffer[pCel->ofs]) {
         int mOfs = pCel->ofs + _scrollPos*nCol;
         if (mOfs < _tableSize) {
            mdb.cDesc.putB(n, _pageNo, mOfs);
            mdb.putConstByteP(_pageNo, mOfs); // sends the byte to the controller, nothing else.
         }
         inputBuffer  [pCel->ofs] = BYTE(n);
         displayBuffer[pCel->ofs] = BYTE(n);
         pCel->SetWindowText(Fmt(n));
      }
   }
}

//------------------------------------------------------------------------------

void hexEdit::OnPageNo(NMHDR *pNMHDR, LRESULT *pResult)
{
   NM_UPDOWN* pNMUpDown = reinterpret_cast<NM_UPDOWN *>(pNMHDR);
   setPage(_pageNo+pNMUpDown->iDelta);
   *pResult = 0;
}

//------------------------------------------------------------------------------

void hexEdit::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) 
{
   if (&m_hexScroll == pScrollBar) {
      switch (nSBCode) {
         case SB_ENDSCROLL    :                          break;
         case SB_BOTTOM       : _scrollPos = _scrollBot; break;
         case SB_LINEDOWN     : _scrollPos++;            break;
         case SB_LINEUP       : _scrollPos--;            break;
         case SB_PAGEDOWN     : _scrollPos += nRow;      break;
         case SB_PAGEUP       : _scrollPos -= nRow;      break;
         case SB_THUMBPOSITION:
         case SB_THUMBTRACK   : _scrollPos = nPos;       break;
         case SB_TOP          : _scrollPos = 0;          break;
      }

      if (_scrollPos < 0         ) _scrollPos = 0;
      if (_scrollPos > _scrollBot) _scrollPos = _scrollBot;
      doScroll();
      pScrollBar->SetScrollPos(_scrollPos, TRUE);
   }

   dataDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

//------------------------------------------------------------------------------
