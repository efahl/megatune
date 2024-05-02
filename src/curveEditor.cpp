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
#include "megatune.h"
#include "megatuneDlg.h"
#include "repository.h"
#include "msDatabase.h"
#include "curveEditor.h"
#include "userDefined.h"
#include "userMenu.h"
#include "userHelp.h"

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------

extern msDatabase   mdb;
extern userMenuList uml;

curveDialog uc[curveDialog::nUC];

//==============================================================================

curveDialog::curveDialog()
 : iEDIT     (0)
 , iTRAK     (-1)
 , _pageNo   (0)
 , _id       ("")
 , _title    ("")
 , _xLo      (0.0)
 , _xHi      (1.0)
 , _xTix     (11)
 , _yLo      (0.0)
 , _yHi      (1.0)
 , _yTix     (11)
 , _xLabel   ("")
 , _yLabel   ("")
 , _x        (NULL)
 , _y        (NULL)
 , _xName    ("")
 , _yName    ("")
 , _xEditable(true)
 , _yEditable(true)
 , _xOch     (NULL)
 , _xOchName ("")
 , _helpDlg  ("")
{
}

//------------------------------------------------------------------------------

int     curveDialog::pageNo() { return _pageNo; }
CString curveDialog::id    () { return _id;     }
CString curveDialog::title () { return _title;  }
CString curveDialog::xLabel() { return _xLabel; }
CString curveDialog::yLabel() { return _yLabel; }
CString curveDialog::xUnits() { return _x ? ("("+_x->units()+")") : ""; }
CString curveDialog::yUnits() { return _y ? ("("+_y->units()+")") : ""; }

//------------------------------------------------------------------------------

void    curveDialog::setPageNo(int pageNo)    { _pageNo = pageNo; }
void    curveDialog::setId    (CString id)    { _id     = id;     }
void    curveDialog::setTitle (CString title) { _title  = title;  }

symbol *curveDialog::x() { return _x; }
symbol *curveDialog::y() { return _y; }

//------------------------------------------------------------------------------

void curveDialog::setLabels(CString x, CString y)
{
   _xLabel = x;
   _yLabel = y;
}

void curveDialog::setX(CString name, CString ochn, bool editable)
{
   _xName     = name;
   _xOchName  = ochn;
   _xEditable = editable;
}

void curveDialog::setY(CString name, CString ochn, bool editable)
{
   _yName     = name;
// _yOchName  = ochn;
   _yEditable = editable;
}

//------------------------------------------------------------------------------

void curveDialog::setXaxis(int lo, int hi, int tix) { _xLo  = lo; _xHi  = hi; _xTix = tix; }
void curveDialog::setYaxis(int lo, int hi, int tix) { _yLo  = lo; _yHi  = hi; _yTix = tix; }

//------------------------------------------------------------------------------

void curveDialog::resolve()
{
   _x = mdb.cDesc.lookup(_xName);
   if (_x == NULL) {
      if (_xName == "SEQUENCE")
         _xEditable = false;
      else {
         msgOk("MegaTune Startup",
               "Curve Editor titled '%s' references\nxBin constant '%s,' which cannot be found.\t",
               _title, _xName);
      }
   }

   _y = mdb.cDesc.lookup(_yName);
   if (_y == NULL) {
      msgOk("MegaTune Startup",
            "Curve Editor titled '%s' references\nyBin constant '%s,' which cannot be found.\t",
            _title, _yName);
   }

   if (_x && _y && _x->nValues() != _y->nValues()) {
      msgOk("MegaTune Startup",
            "Curve Editor titled '%s' references xBins and yBins of different length.\t",
            _title);
   }

   if (!_xOchName.IsEmpty()) {
      _xOch = mdb.cDesc.lookup(_xOchName);
      if (_xOch == NULL) {
         msgOk("MegaTune Startup",
               "Curve Editor titled '%s' references\nX variable '%s,' which cannot be found.\t",
               _title, _xOchName);
      }
   }
}

//------------------------------------------------------------------------------

CString curveDialog::xName() { return _x ? _x->name() : "X"; }
CString curveDialog::yName() { return _y ? _y->name() : "Y"; }

int    curveDialog::nBINS  ()      { return _y ? _y->nValues()    : 0; }
double curveDialog::XBIN   (int i) { return _x ? _x->valueUser(i) : i; }
double curveDialog::YBIN   (int i) { return _y ? _y->valueUser(i) : 0.0; }

double curveDialog::xLo    ()      { return _x ? _x->lo() : 0.0; }
double curveDialog::xHi    ()      { return _x ? _x->hi() : (nBINS()-1); }
double curveDialog::yLo    ()      { return _y ? _y->lo() : 0.0; }
double curveDialog::yHi    ()      { return _y ? _y->hi() : (nBINS()-1); }

double curveDialog::minXBIN()      { return _xLo; }
double curveDialog::minYBIN()      { return _yLo; }
double curveDialog::maxXBIN()      { return _xHi; }
double curveDialog::maxYBIN()      { return _yHi; }
int    curveDialog::xTix   ()      { return _xTix; }
int    curveDialog::yTix   ()      { return _yTix; }

//------------------------------------------------------------------------------

bool curveDialog::deltaY(int dy)
{
   if (_y && _yEditable) {
      if ( _y->incRaw(dy, iEDIT)) {
         mdb.cDesc.send(_y, iEDIT);
         return true;
      }
   }
   return false;
}

bool curveDialog::deltaX(int dx)
{
   if (_x && _xEditable) {
      long newX = _x->valueRaw(iEDIT)+dx;
      if (iEDIT > 0          && newX <= _x->valueRaw(iEDIT-1)) return false;
      if (iEDIT < nBINS()-1  && newX >= _x->valueRaw(iEDIT+1)) return false;
      if ( _x->incRaw(dx, iEDIT)) {
         mdb.cDesc.send(_x, iEDIT);
         return true;
      }
   }
   return false;
}

//------------------------------------------------------------------------------

int curveDialog::deltaI(int di)
{
   int oldEDIT = iEDIT;
   while (iEDIT+di <  0      ) di++;
   while (iEDIT+di >= nBINS()) di--;
   iEDIT += di;
   return oldEDIT == iEDIT ? -1 : oldEDIT;
}

//------------------------------------------------------------------------------

double interp(double x, double x0, double y0, double x1, double y1);

int curveDialog::updateTrack()
{
   int oldTRAK = iTRAK;
   if (_xOch) {
      xSpot = _xOch->valueUser();
      if (xSpot <= XBIN(0)) {
         iTRAK = 0;
         if (xSpot < minXBIN()) xSpot = minXBIN();
         ySpot = YBIN(iTRAK);
      }
      else if (xSpot >= XBIN(nBINS()-1)) {
         iTRAK = nBINS()-1;
         if (xSpot > maxXBIN()) xSpot = maxXBIN();
         ySpot = YBIN(iTRAK);
      }
      else {
         iTRAK = 0;
         while (xSpot > XBIN(iTRAK+1)) iTRAK++;
         ySpot = interp(xSpot, XBIN(iTRAK), YBIN(iTRAK), XBIN(iTRAK+1), YBIN(iTRAK+1));
         if (fabs((XBIN(iTRAK+1)-xSpot)) / (XBIN(iTRAK+1)-XBIN(iTRAK)) < 0.5) iTRAK++;
      }
   }
   return iTRAK == oldTRAK ? -1 : oldTRAK;
}

//==============================================================================

extern repository rep;

curveEditor::curveEditor(int nId, CWnd *pParent)
 : dataDialog(curveEditor::IDD, pParent)
 , _nId      (nId)
 , _doGoto   (false)
{
   _cd    = uc+nId;

   //{{AFX_DATA_INIT(curveEditor)
   //}}AFX_DATA_INIT

   _line  = new Point3[max(3, max(_cd->nBINS(), max(_cd->xTix(), _cd->yTix())))];

   _chart = new plot3d(this);
   _chart->setWorld (-1.0, 1.0);
   _chart->setColors(rep.t_backgroundColor, rep.t_gridColor, rep.t_cursorColor, rep.t_spotColor);
   _chart->setDepth (rep.vatCD, rep.vatSD);
   _chart->set2D    (true);
}

curveEditor::~curveEditor()
{
   delete [] _line;
   delete    _chart;
}

//------------------------------------------------------------------------------

void curveEditor::DoDataExchange(CDataExchange *pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(curveEditor)
      DDX_Control(pDX, IDC_X_LABEL, m_xLabel);
      DDX_Control(pDX, IDC_X_UNITS, m_xUnits);
      DDX_Control(pDX, IDC_Y_LABEL, m_yLabel);
      DDX_Control(pDX, IDC_Y_UNITS, m_yUnits);

      DDX_Control(pDX, IDC_GAUGE_FRAME, m_gaugeFrame);
      DDX_Control(pDX, IDC_CHART_FRAME, m_chartFrame);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(curveEditor, dataDialog)
   //{{AFX_MSG_MAP(curveEditor)
      ON_WM_CTLCOLOR()
      ON_WM_PAINT()
      ON_WM_SIZE()

      ON_COMMAND(ID_XFORM_TABLE, OnGenerate)
      ON_COMMAND(ID_TABLE_GET,   OnTableGet)
      ON_COMMAND(ID_TABLE_PUT,   OnTablePut)

      ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_CEX01, IDC_CEX16, dbFromFields)
      ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_CEY01, IDC_CEY16, dbFromFields)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

void logSize(CWnd *w, char *title);

BOOL curveEditor::OnInitDialog()
{
   dataDialog::OnInitDialog(_cd->title(), _cd->pageNo()+1);
   setHelp(_cd->helpDlg());

   uml.attach(this, _cd->id());

   CRect r;
   m_chartFrame.GetWindowRect(&r);
   ScreenToClient(&r);
   _chart->setDeviceCoordinates(r.left, r.top, r.right, r.bottom);
   _chart->setWorldCoordinates(Point3(_cd->minXBIN(), _cd->minYBIN(), -0.1),
                               Point3(_cd->maxXBIN(), _cd->maxYBIN(),  0.1));

   m_xLabel.SetWindowText(_cd->xLabel());
   m_xUnits.SetWindowText(_cd->xUnits());
   m_yLabel.SetWindowText(_cd->yLabel());
   m_yUnits.SetWindowText(_cd->yUnits());

   int bottomField = min(16, _cd->nBINS())-1;
   for (int i = 0; i < IDC_CEY16-IDC_CEY01+1; i++) {
      _xBox[i] = static_cast<CEdit *>(GetDlgItem(IDC_CEX01+i));
      _yBox[i] = static_cast<CEdit *>(GetDlgItem(IDC_CEY01+i));
      _xBox[i]->ShowWindow(i <= bottomField);
      _yBox[i]->ShowWindow(i <= bottomField);
   }

   _xBox[bottomField]->GetWindowRect(&r);
   ScreenToClient(&r);
   int newTop = r.bottom;

   _gaugeDisplay = !_cd->gaugeName().IsEmpty();
   if (_gaugeDisplay) {
//    _cd->gauge().ShowWindow(true); ???

      m_gaugeFrame.GetWindowRect(&r);
      ScreenToClient(&r);
      int newHgt = r.Width()+10;
      int curTop = r.bottom-newHgt;
      if (curTop < newTop) newHgt = r.bottom-newTop;
      r.top = r.bottom-newHgt;
      m_gaugeFrame.MoveWindow(r.left, r.top, r.Width(), newHgt, true);

      setupGauge(_cd->gauge(), S_GaugeConfigurations, _cd->gaugeName());
   }
// _cd->gauge().ShowWindow(_gaugeDisplay); ???

   _cd->updateTrack();
   fieldsFromDb();

   logSize(this, "curve");

   return TRUE;
}

//------------------------------------------------------------------------------

HBRUSH curveEditor::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
   HBRUSH hbr = dataDialog::OnCtlColor(pDC, pWnd, nCtlColor);

   pDC->SetBkMode(TRANSPARENT);

   int  ibin = -2;
   UINT nId  = pWnd->GetDlgCtrlID();
   if (nId >= IDC_CEX01 && nId <= UINT(IDC_CEX01+min(16,_cd->nBINS()))) ibin = nId - IDC_CEX01;
   if (nId >= IDC_CEY01 && nId <= UINT(IDC_CEY01+min(16,_cd->nBINS()))) ibin = nId - IDC_CEY01;

   if (ibin == _cd->iEDIT) {
      static CBrush brush;
      brush.DeleteObject();
      brush.CreateSolidBrush(rep.t_cursorColor);
      pDC->SetTextColor(rep.t_cursorText);
      hbr = brush;
   }
   else if (ibin == _cd->iTRAK) {
      static CBrush brush;
      brush.DeleteObject();
      brush.CreateSolidBrush(rep.t_spotColor);
      pDC->SetTextColor(rep.t_spotText);
      hbr = brush;
   }

   return hbr;
}

//------------------------------------------------------------------------------

void curveEditor::OnPaint()
{
   CPaintDC dc(this);

   if (!IsIconic()) {
      dataDialog::OnPaint();

      if (_gaugeDisplay) {
         CRect r;
         m_gaugeFrame.GetWindowRect(&r);
         ScreenToClient(&r);
         _cd->gauge().ShowMeter(&dc, r, true);
      }

      drawAll();
   }
}

//------------------------------------------------------------------------------

void curveEditor::OnTableGet()
{
   fieldsFromDb();
   drawAll();
}

//------------------------------------------------------------------------------

void curveEditor::OnTablePut()
{
   int savePage = mdb.setPageNo(_cd->pageNo(), true);
      if (_cd->xEditable()) mdb.cDesc.send(_cd->x());
      if (_cd->yEditable()) mdb.cDesc.send(_cd->y());
      mdb.burnConst(_cd->pageNo());
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------

void curveEditor::fieldsFromDb()
{
   mdb.getPage(_cd->pageNo());

   for (int i = 0; i < min(16, _cd->nBINS()); i++) {
      setFld(_xBox[i], _cd->x(), i);
      setFld(_yBox[i], _cd->y(), i);
   }
}

//------------------------------------------------------------------------------

void curveEditor::dbFromFields(UINT nId)
{
   int n = min(16, _cd->nBINS());

   if (nId >= IDC_CEX01 && nId <= UINT(IDC_CEX01+n)) {
      int iT = nId - IDC_CEX01;
      // Somehow enforce x order...
      getFld(_xBox[iT], _cd->x(), iT);
      drawAll();
   }

   if (nId >= IDC_CEY01 && nId <= UINT(IDC_CEY01+n)) {
      int iT = nId - IDC_CEY01;
      getFld(_yBox[iT], _cd->y(), iT);
      drawAll();
   }
}

//------------------------------------------------------------------------------

void curveEditor::drawGrid()
{
   _chart->clear(-7);
   _chart->setMins(_cd->minXBIN(), _cd->minYBIN());

   double mnX = _cd->minXBIN(), mxX = _cd->maxXBIN(), rX = mxX-mnX;
   double mnY = _cd->minYBIN(), mxY = _cd->maxYBIN(), rY = mxY-mnY;

   CString label;

   int i, n   = _cd->xTix();
   double N   = double(n-1);
   _line[0].y = mnY; _line[1].y = mxY; _line[2].y = mnY;
   _line[0].z = 0.0; _line[1].z = 0.0; _line[2].z = 0.0;
   for (i = 0; i < n; i++) {
      _line[0].x = _line[1].x = mnX + double(i)/N * rX;
      _chart->drawPolyline(_line, 2);
      label.Format(" %.1f ", _line[0].x);
      _line[2].x = _line[0].x-0;
      _chart->drawText(_line[2], label, false, -5, -4);
   }

   n = _cd->yTix();
   N = double(n-1);
   _line[0].x = mnX; _line[1].x = mxX;
   for (i = 0; i < n; i++) {
      _line[0].y = _line[1].y = mnY + double(i)/N * rY;
      _chart->drawPolyline(_line, 2);
      label.Format(" %.1f ", _line[0].y);
      _chart->drawText(_line[0], label, true,   3,  -4);
   }
}

void curveEditor::drawCurve()
{
   for (int i = 0; i < _cd->nBINS(); i++) {
      _line[i] = Point3(_cd->XBIN(i), _cd->YBIN(i), 0.0);
   }
   _chart->drawPolyline(_line, _cd->nBINS());
   _chart->drawMarkers (_line, _cd->nBINS());
}

void curveEditor::drawCursor()
{
   double x = _cd->XBIN(_cd->iEDIT);
   double y = _cd->YBIN(_cd->iEDIT);
   Point3 p(x, y, 0.0);
   _chart->drawCursor(p);
#if 0
   CString label;
   label.Format(" (%.1f,%.1f) ", x, y);
   _chart->drawText(p, label, true,   3,  -8);
#endif
}

void curveEditor::drawSpot()
{
   if (_cd->iTRAK >= 0) {
      _chart->drawSpot(Point3(_cd->xSpot, _cd->ySpot, 0.0));
   }
}

void curveEditor::drawAll()
{
   drawGrid  ();
   drawCurve ();
   drawCursor();
}

//------------------------------------------------------------------------------

bool curveEditor::doTimer(bool connected)
{
   if (!realized()) return false;

   if (connected) {
      int oldTRAK = _cd->updateTrack();
      if (oldTRAK >= 0) {
         if (oldTRAK < 16) {
            _xBox[oldTRAK   ]->Invalidate();
            _yBox[oldTRAK   ]->Invalidate();
         }
         if (_cd->iTRAK < 16) {
            _xBox[_cd->iTRAK]->Invalidate();
            _yBox[_cd->iTRAK]->Invalidate();
         }
      }
      drawSpot();

      if (_gaugeDisplay) {
         CDC *dc = GetDC();
            const gaugeConfiguration *g = reinterpret_cast<const gaugeConfiguration *>(_cd->gauge().userData);
            if (g) _cd->gauge().UpdateNeedle(dc, mdb.cDesc._userVar[g->och]);
         ReleaseDC(dc);
      }

      if (_doGoto && _cd->iTRAK >= 0 && _cd->iEDIT != _cd->iTRAK) {
         _cd->iEDIT = _cd->iTRAK;
         _yBox[_cd->iEDIT]->SetFocus();
         drawCursor();
      }
   }

   return true;
}

//------------------------------------------------------------------------------

void curveEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   bool updateCursor = false;
   bool updateCurve  = false;

   int dx = 0;
   int dy = 0;
   int di = 0;
   switch (nChar) {
      case VK_LEFT : di = -1; break;
      case VK_RIGHT: di = +1; break;

      case 'L': dx = +1; break;
      case 'H': dx = -1; break;
      case ';': dx = +5; break;
      case 'D': dx = -5; break;

      case 'Q': dy = +1; break;
      case 'W': dy = -1; break;
      case 'E': dy = +5; break;
      case 'R': dy = -5; break;

      case 'G': _doGoto = !_doGoto; if (!_doGoto) break; // else fallthrough
      case 'F': if (_cd->iTRAK >= 0) {
                   di = _cd->iTRAK-_cd->iEDIT;
                   _yBox[_cd->iTRAK]->SetFocus();
                }
                break;
   }


   int oldEDIT = _cd->deltaI(di);
   if (oldEDIT >= 0) {
      if (oldEDIT < 16) {
         _xBox[oldEDIT   ]->Invalidate();
         _yBox[oldEDIT   ]->Invalidate();
      }
      if (_cd->iEDIT < 16) {
         _xBox[_cd->iEDIT]->Invalidate();
         _yBox[_cd->iEDIT]->Invalidate();
      }
      updateCursor = true;
   }

   if (dx) {
      if (_cd->deltaX(dx)) {
         if (_cd->iEDIT < 16) setFld(_xBox[_cd->iEDIT], _cd->x(), _cd->iEDIT);
         updateCurve = true;
      }
   }
   if (dy) {
      if (_cd->deltaY(dy)) {
         if (_cd->iEDIT < 16) setFld(_yBox[_cd->iEDIT], _cd->y(), _cd->iEDIT);
         updateCurve = true;
      }
   }

   if (updateCurve ) drawAll   ();
   if (updateCursor) drawCursor();

   dataDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

//------------------------------------------------------------------------------

BOOL curveEditor::PreTranslateMessage(MSG *pMsg)
{
   static bool shifted = false;
   static bool ctrled  = false;
          bool alted   = (HIWORD(pMsg->lParam) & KF_ALTDOWN) != 0;

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

         case VK_LEFT:  if (ctrled && shifted) pMsg->wParam = 'D';
                   else if (shifted          ) pMsg->wParam = 'H'; // Avoid accidental cursor motion.
         case 'D':
         case 'H':
            OnKeyDown(pMsg->wParam, 1, pMsg->lParam >> 16);
            return TRUE;

         case VK_UP:    if (ctrled && shifted) pMsg->wParam = 'E'; // was return spinReqFuel(-1);
                   else if (shifted          ) pMsg->wParam = 'Q'; // Q = rich up 38
         case 'E':
         case 'Q':
            OnKeyDown(pMsg->wParam, 1, pMsg->lParam >> 16);
            return TRUE;

         case VK_RIGHT: if (ctrled && shifted) pMsg->wParam = ';';
                   else if (shifted          ) pMsg->wParam = 'L';
         case 'L':
            OnKeyDown(pMsg->wParam, 1, pMsg->lParam >> 16);
            return TRUE;

         case VK_DOWN:  if (ctrled && shifted) pMsg->wParam = 'R'; // was return spinReqFuel( 1);
                   else if (shifted          ) pMsg->wParam = 'W'; // W = lean dn 40
         case 'R':
         case 'W':
            OnKeyDown(pMsg->wParam, 1, pMsg->lParam >> 16);
            return TRUE;

         case VK_SPACE:
            mdb.markLog();
            return TRUE;

         case 'A': // Stuff to ignore so we don't mangle numeric values.
         case 'B':
         case 'C':
         case 'I':
         case 'J': // Down in tuning
         case 'K': // Up in tuning
         case 'M':
         case 'N':
         case 'O':
         case 'P':
         case 'T':
         case 'U':
         case 'V':
         case 'X':
         case 'Y':
         case 'Z':
            return TRUE;

         case 'F': // Find command.
         case 'G': // "Go to spot" mode.
//       case 'S': let this go through for now, make it work like tuning dialog.
         case VK_RETURN:
            OnKeyDown(pMsg->wParam, 1, pMsg->lParam >> 16);
            return TRUE;
      }
   }
   return dataDialog::PreTranslateMessage(pMsg);
}

//------------------------------------------------------------------------------

void curveEditor::OnSize(UINT nType, int cx, int cy)
{
   dataDialog::OnSize(nType, cx, cy);
}

//------------------------------------------------------------------------------

class generator : public dataDialog {
   public:
      generator(UINT curveId, CWnd *pParent=NULL)
       : dataDialog(IDD_GENERATOR, pParent)
       , _cd       (uc[curveId])
      { }

     ~generator() { }

   protected:
      virtual BOOL OnInitDialog();
      virtual void OnOK();
      afx_msg void OnUseX();

   DECLARE_MESSAGE_MAP()

   private:
      curveDialog &_cd;
      CEdit       *_i0, *_i1;
      CEdit       *_x0, *_x1;
      CEdit       *_y0, *_y1;
      CEdit       *_ofs;
      CComboBox   *_ln;
      CButton     *_ux;
};

BEGIN_MESSAGE_MAP(generator, dataDialog)
   ON_BN_CLICKED(IDC_GEN_UX, OnUseX)
END_MESSAGE_MAP()

BOOL generator::OnInitDialog()
{
   dataDialog::OnInitDialog(_cd.title()+" Curve");

   GetDlgItem(IDC_X_UNITS)->SetWindowText("X "+_cd.xUnits());
   GetDlgItem(IDC_Y_UNITS)->SetWindowText("Y "+_cd.yUnits());

   _ux = static_cast<CButton    *>(GetDlgItem(IDC_GEN_UX));
   _ux->SetCheck(true);

    _ln = static_cast<CComboBox *>(GetDlgItem(IDC_GEN_LN));
   _ln->AddString("Linear");
   _ln->AddString("Exponential");
   _ln->AddString("Logarithmic");
   _ln->SetCurSel(0);

   _i0  = static_cast<CEdit *>(GetDlgItem(IDC_GEN_I0));
   _i1  = static_cast<CEdit *>(GetDlgItem(IDC_GEN_I1));
   _x0  = static_cast<CEdit *>(GetDlgItem(IDC_GEN_X0));
   _x1  = static_cast<CEdit *>(GetDlgItem(IDC_GEN_X1));
   _y0  = static_cast<CEdit *>(GetDlgItem(IDC_GEN_Y0));
   _y1  = static_cast<CEdit *>(GetDlgItem(IDC_GEN_Y1));
   _ofs = static_cast<CEdit *>(GetDlgItem(IDC_GEN_OF));

   setFld(_i0,  1          ); setFld(_x0, _cd.x(), 0            ); setFld(_y0, _cd.y(), 0            );
   setFld(_i1,  _cd.nBINS()); setFld(_x1, _cd.x(), _cd.nBINS()-1); setFld(_y1, _cd.y(), _cd.nBINS()-1);
   setFld(_ofs, 0);

   OnUseX();

   return TRUE;
}

void generator::OnUseX()
{
   int useX = _ux->GetCheck();
   _i0->EnableWindow(!useX);
   _i1->EnableWindow(!useX);
}

void generator::OnOK()
{
   enum { tLinear = 0, tExponential = 1, tLogarithmic = 2 };

   int curveType = _ln->GetCurSel();
   int useX      = _ux->GetCheck();

   int    n   = _cd.nBINS();
   int    i0  = getRaw(_i0) - 1; if (i0 < 0) i0 = 0; if (i0 >= n) i0 = n-1;
   int    i1  = getRaw(_i1) - 1; if (i1 < 0) i1 = 0; if (i1 >= n) i1 = n-1;

   double ofs = getRaw(_ofs);
   double x0  = getRaw(_x0,  _cd.xLo(), _cd.xHi(), _cd.xName());
   double x1  = getRaw(_x1,  _cd.xLo(), _cd.xHi(), _cd.xName());
   double y0  = getRaw(_y0,  _cd.yLo(), _cd.yHi(), _cd.yName()) + ofs;
   double y1  = getRaw(_y1,  _cd.yLo(), _cd.yHi(), _cd.yName()) + ofs;

   double m = (y1-y0) / (x1-x0);
   double b = y0 - m * x0;
   double cm, cb, ly0, ly1;
   switch (curveType) {
      case tExponential:
         ly0 = ::log10(y0);
         ly1 = ::log10(y1);
         cm = (ly1-ly0) / (x1-x0);
         cb = ly0 - cm * x0;
         break;

      case tLogarithmic:
         ly0 = ::log10(y0);
         ly1 = ::log10(y1);
         cm = (y1-y0) / (ly1-ly0);
         cb = y0 - cm * ly0;
         break;
   }

   symbol *x    = _cd.x();
   symbol *y    = _cd.y();
   double  xInc = (x1 - x0) / (i1 - i0);
   x0 += double(-i0)*xInc;

   for (int i = 0; i < n; i++) {
      double xV = useX ? _cd.XBIN(i) : x0 + double(i)*xInc;
      if (xV < _cd.xLo()) xV = _cd.xLo();
      if (xV > _cd.xHi()) xV = _cd.xHi();

      double yV  = m*xV + b;
      switch (curveType) {
         case tExponential: yV = ::pow(10.0, cm*xV + cb); break;
         case tLogarithmic: yV = cm*::log10(yV) + cb;     break;
      }
      yV -= ofs;
      if (yV < _cd.yLo()) yV = _cd.yLo();
      if (yV > _cd.yHi()) yV = _cd.yHi();

      if (_cd.xEditable() && x) x->storeValue(xV, i);
      if (_cd.yEditable() && y) y->storeValue(yV, i);
   }

   if (_cd.xEditable() && x) mdb.cDesc.send(x);
   if (_cd.yEditable() && y) mdb.cDesc.send(y);

   dataDialog::OnOK();
}

//------------------------------------------------------------------------------

void curveEditor::OnGenerate()
{
   generator gen(_nId, this);
   if (gen.DoModal() == IDOK) {
      OnTableGet();
   }
}

//------------------------------------------------------------------------------
