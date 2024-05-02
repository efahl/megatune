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

#pragma warning(disable:4786)

#include "megatune.h"
#include "msDatabase.h"
#include "userDefined.h"
#include "userHelp.h"

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

extern msDatabase mdb;

//------------------------------------------------------------------------------

userDialogList ud;

//==============================================================================
//==  Substitute values of symbols into %symName% strings, expand tabs and    ==
//==  do any sort of common reformatting needed for any labels or titles.     ==

static void reformat(CString &s, int idx=0)
{
   s.Replace("\\t", "\t");

   CString substr;

   int first = s.Find("%");
   while (first >= 0) {
      int last = s.Find("%", first+1);
      if (last == -1) break;

      CString symName = s.Mid(first+1, last-first-1);
      bool    count   = false;
      if (symName[0] == '#') {
         count = true;
         symName.Delete(0, 1);
         s.Delete(first+1, 1);
      }

      symbol *val = mdb.cDesc.lookup(symName);
      if (val) {
         if (count)
            substr.Format("%d", val->nValues());
         else
            substr = val->valueString(idx)+val->units();
      }
      else {
         if (symName == "INDEX")
            substr.Format("%d", idx+1);
         else {
            first++;
            continue; // Restart at next %
         }
      }

      s.Replace("%"+symName+"%", substr);
      first = s.Find("%"); // Start at beginning.
   }
}
//==============================================================================

static void setItemData(CComboBox *cmb, symbol *sym)
{
   int nVals  = sym->nValues();
   CDC *pDC   = cmb->GetDC();
   int  width = 0;
   for (int ii = 0; ii < nVals; ii++) {
      CString &str = sym->userStrings[ii];
      if (str != "INVALID") {
         CSize size = pDC->GetTextExtent(str);
         if (size.cx > width) width = size.cx;
         cmb->AddString(str);
         cmb->SetItemData(cmb->GetCount()-1, ii);
      }
   }
   cmb->ReleaseDC(pDC);
// width += ::GetSystemMetrics(SM_CXVSCROLL) + 2 * ::GetSystemMetrics(SM_CXEDGE);
// if (cmb->GetDroppedWidth() < width)
      cmb->SetDroppedWidth(width);
}

//==============================================================================

stdEditField::stdEditField(dataDialog *p)
  : _par(p)
  , _fld(NULL)
  , _cmb(NULL)
  , _sym(NULL)
{
}

bool stdEditField::add(const CString &symName, UINT nId, int pageNo, int idx)
{
   _sym = mdb.lookupByPage(symName, pageNo);
   _idx = idx;
   _par->GetDlgItem(nId)->EnableWindow(_sym != NULL);
   if (_sym) {
      if (pageNo > 0 && _sym->page() != pageNo) {
         msgOk("Standard Dialog", "Symbol \"%s\" was found on page %d, but should be on page %d.", symName, _sym->page()+1, pageNo+1);
      }
      if (!_sym->isBits())
         _fld = static_cast<CEdit     *>(_par->GetDlgItem(nId)); 
      else {
         _cmb = static_cast<CComboBox *>(_par->GetDlgItem(nId));
         setItemData(_cmb, _sym);
      }
   }
   return _sym != NULL;
}

void stdEditField::send()
{
   if (_sym) mdb.cDesc.send(_sym, _idx);
}

void stdEditField::setFld()
{
   if (_sym) {
      if (_fld) _par->setFld(_fld, _sym, _idx);
      if (_cmb) _par->setCmb(_cmb, _sym);
   }
}

void stdEditField::getFld()
{
   if (_sym) {
      if (_fld) _par->getFld(_fld, _sym, _idx);
      if (_cmb) _par->getCmb(_cmb, _sym);
   }
}

double stdEditField::value()
{
   return _sym ? _sym->valueUser(_idx) : 0.0;
}

//------------------------------------------------------------------------------

CString stdEditFieldList::n(const char *v, int i)
{
   if (i == 0) return v;
   CString nn;
   nn.Format("%s%d", v, i);
   return nn;
}

stdEditFieldList::stdEditFieldList(dataDialog *p)
 : _par(p)
{
}

stdEditFieldList::~stdEditFieldList()
{
   for (fldIter i = _map.begin(); i != _map.end(); i++) {
      delete (*i).second;
   }
}

void stdEditFieldList::add(const char *varName, UINT widgetId, int pageNo, int idx)
{
   CString vn = n(varName,idx);
   stdEditField *stf = new stdEditField(_par);
   stf->add(varName, widgetId, pageNo, idx);
   _map[vn] = stf;   
}

void stdEditFieldList::send()
{
   for (fldIter i = _map.begin(); i != _map.end(); i++) {
      (*i).second->send();
   }
}

void stdEditFieldList::setFld()
{
   for (fldIter i = _map.begin(); i != _map.end(); i++) {
      (*i).second->setFld();
   }
}

void stdEditFieldList::getFld()
{
   for (fldIter i = _map.begin(); i != _map.end(); i++) {
      (*i).second->getFld();
   }
}

stdEditField *stdEditFieldList::operator[](const char *varName) { return _map[n(varName,0)]; }

double stdEditFieldList::value(const char *varName, int idx)
{
   CString vn = n(varName,idx);
   if (_map.find(vn) == _map.end()) return 0.0;
   return _map[vn]->value();
}

//==============================================================================

udCell::udCell() : edt(NULL), lbl(NULL), cmb(NULL), fld(NULL), bas(0) { }

void udCell::init(dataDialog *dlg, int idx, userEditField *d)
{
   cmb = static_cast<CComboBox *>(dlg->GetDlgItem(IDC_UDC01+idx)); cmb->ShowWindow(false);
   edt = static_cast<CEdit     *>(dlg->GetDlgItem(IDC_UDV01+idx)); edt->ShowWindow(false);
   lbl = static_cast<CStatic   *>(dlg->GetDlgItem(IDC_UDL01+idx)); lbl->ShowWindow(false);
   fld = d;
}

UINT udCell::base   ()          { return bas; }

bool udCell::setLabel(CWnd *wnd, const CString label)
{
   if (!label.IsEmpty()) {
      CRect rct;
      edt->GetWindowRect(&rct);
      wnd->ScreenToClient(&rct);
      int rgt = rct.right;
      if (fld->sym() != NULL) {
         if (fld->isBits()) {
            cmb->GetWindowRect(&rct);
            wnd->ScreenToClient(&rct);
         }
         rgt = rct.left-10;
      }

      lbl->GetWindowRect(&rct);
      wnd->ScreenToClient(&rct);
      rct.right = rgt;
      lbl->MoveWindow(rct, true);

      lbl->ShowWindow(true);
      lbl->SetWindowText(" "+label+" ");
      if (fld->highlight() & userEditField::hRight)
         lbl->ModifyStyle(SS_LEFT, SS_RIGHT);
      else
         lbl->ModifyStyle(SS_RIGHT, SS_LEFT);
      return true;
   }
   return false;
}

bool udCell::setEdit(UINT base)
{
   bas = base;
   symbol *s = fld->sym();
   if (s) {
      if (s->isBits()) {
         cmb->ShowWindow(true);
         setItemData(cmb, s);
         return true;
      }
      else {
         edt->ShowWindow(true);
         return true;
      }
   }
   return false;
}

void udCell::setEnabled()
{
   if (fld && fld->sym()) {
      if (fld->isBits())
         cmb->EnableWindow(fld->isEnabled());
      else
         edt->EnableWindow(fld->isEnabled());
   }
}

void udCell::setFocus()
{
   if (fld && fld->sym()) {
      if (fld->isBits())
         cmb->SetFocus();
      else
         edt->SetFocus();
   }
}

bool udCell::hasExpr() { return fld && fld->hasExpr(); }
bool udCell::hasData() { return !fld && !cmb;          }

UINT udCell::lblId() { return lbl->GetDlgCtrlID(); }

//==============================================================================

userEditField::userEditField(CString l, CString c, CString e)
 : _type      (fSingle)
 , _title     ("")
 , _label     (l)
 , _name      (c)
 , _expr      (e)
 , _sym       (NULL)
 , _val       (-1)
 , _highlight (0)
{
   setHighlight();
}

userEditField::userEditField(CString t, CString l, CString a, CString e)
 : _type      (fArray1D)
 , _title     (t)
 , _label     (l)
 , _name      (a)
 , _expr      (e)
 , _sym       (NULL)
 , _val       (-1)
 , _highlight (0)
{
   setHighlight();
}

void userEditField::setHighlight()
{
   bool del;
   do {
      del = false;
      if (_label.GetLength() > 0) {
         switch (_label[0]) {
            case '>': _highlight |= hRight; del = true; break;
            case '!': _highlight |= hAlarm; del = true; break;
            case '#': _highlight |= hTitle; del = true; break;
         }
      }
      if (del) _label.Delete(0);
   } while (del);
}

symbol        *userEditField::sym    ()      const { return _sym;           }
symbol        *userEditField::sym(symbol *s)       { return _sym = s;       }
bool           userEditField::isBits ()      const { return _sym->isBits(); }
const CString &userEditField::name   ()      const { return _name;          }
bool           userEditField::hasExpr()      const { return !_expr.IsEmpty(); }

int userEditField::nValues() const {
   switch (_type) {
      case fSingle:
         return 1;

      case fArray1D:
      case fArray2D:
         if (_sym == NULL) return 0;
         return _sym->nValues();
   }
   return 0;
}

bool userEditField::isEnabled() const {
   return _val == -1 || mdb.cDesc._userVar[_val] != 0.0;
}

userEditField::fieldType userEditField::type() const { return _type; }

CString userEditField::title() const
{
   CString s = _title;
   reformat(s);
   return s;
}

CString userEditField::label(int idx) const
{
   CString s = _label;
   reformat(s, idx);
   if (_sym && !_sym->units().IsEmpty()) {
      s += " (" + _sym->units() + ")";
   }
   return s;
}

int userEditField::highlight() const
{
   return _highlight;
}

void userEditField::contribute()
{
   if (!_expr.IsEmpty()) {
      _val = mdb.cDesc.addExpr(_expr);
   }
}

//==============================================================================

userDialog::userDialog(CString t, CString d, UINT id, int p)
 : _title  (t)
 , _dbid   (d)
 , _idno   (id)
 , _pageNo (p)
 , _helpDlg("")
{ }

userDialog::~userDialog()
{
   for (unsigned i = 0; i < _uef.size(); i++) {
      delete _uef[i];
   }
}

const char *userDialog::dbid   () const { return _dbid;    }
const char *userDialog::title  () const { return _title;   }
int         userDialog::pageNo () const { return _pageNo;          }
int         userDialog::nFields() const { return _uef.size();      }

void userDialog::add(CString t, CString l, CString a, CString e) {
   _uef.push_back(new userEditField(t, l, a, e));
}
void userDialog::add(CString l, CString c, CString e) {
   _uef.push_back(new userEditField(l, c, e));
}

void userDialog::contribute()
{
   for (int i = 0; i < nFields(); i++) {
      _uef[i]->contribute();
   }
}

const char *userDialog::helpDlg  () const      { return _helpDlg; }
void        userDialog::setHelp  (CString h  ) { _helpDlg = h;    } 

userEditField &userDialog::operator[](int idx) { return *_uef[idx]; }

//------------------------------------------------------------------------------

userDialogList::userDialogList() { registerStdDialogs(); }
userDialogList::~userDialogList() { for (unsigned i = 0; i < _ud.size(); i++) delete _ud[i]; }

//------------------------------------------------------------------------------

const userDialog *userDialogList::dialog (int idx) const { return _ud[idx];   }
int              userDialogList::nDialogs()        const { return _ud.size(); }

//------------------------------------------------------------------------------

void userDialogList::registerDialog(CString d, UINT id)
{
   _udId[d] = id;
}
void userDialogList::addDialog(CString d, CString t, int p)
{
   static int nId = 0;
   UINT id = ID_USER_DEFINED_00+nId;
   nId++;
   _ud.push_back(new userDialog(t, d, id, p));
   _udId[d] = id;
}

//------------------------------------------------------------------------------

UINT userDialogList::id(const CString &d) const {
   strMapIter i = _udId.find(d);
   return i != _udId.end() ? (*i).second : 0;
}

//------------------------------------------------------------------------------

void userDialogList::init()
{
   for (int i = 0; i < nDialogs(); i++) {
      _ud[i]->contribute();
   }
}

//------------------------------------------------------------------------------

userDialog &userDialogList::operator[](int idx) const { return *_ud[idx]; }

//------------------------------------------------------------------------------

void userDialogList::registerStdDialogs()
{
   registerDialog("std_accel",         ID_STD_AWZ_00);
   registerDialog("std_constants",     ID_STD_CON_00);
   registerDialog("std_enrichments",   ID_STD_ENR_00);
   registerDialog("std_injection",     ID_STD_INJ_00);
   registerDialog("std_realtime",      ID_RUNTIME_REALTIMEDISPLAY);
   registerDialog("std_warmup",        ID_STD_WWZ_00);
   registerDialog("std_trigwiz",       ID_STD_TWZ_00);

   registerDialog("std_separator",     0);

   registerDialog("std_ms2gentherm",   ID_STD_GTH_00);
   registerDialog("std_ms2geno2",      ID_STD_GO2_00);
   registerDialog("std_ms2genmaf",     ID_STD_MAF_00);
   registerDialog("std_port_edit",     ID_STD_PORTEDIT);
}

//------------------------------------------------------------------------------

userDefined::userDefined(int pageNo, CWnd *pParent)
 : dataDialog(userDefined::IDD, pParent),
   _page      (pageNo),
   _nRows     (0)
{
   //{{AFX_DATA_INIT(userDefined)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void userDefined::moveButton(UINT id, int top)
{
   CRect br;
   CWnd *b = GetDlgItem(id);
   b->GetWindowRect(&br);
   ScreenToClient(&br);
   b->MoveWindow(br.left, top, br.Width(), br.Height(), true);
}

void userDefined::resize()
{
   CRect rct1; GetDlgItem(IDC_UDL01)->GetWindowRect(&rct1); ScreenToClient(&rct1);
   CRect rct2; GetDlgItem(IDC_UDL02)->GetWindowRect(&rct2); ScreenToClient(&rct2);

   int bht = rct2.top-rct1.top;
   int top = rct1.top + (_nRows-1)*bht + 25;
   moveButton(ID_HELP_ABOUT, top);
   moveButton(IDC_UP       , top);
   moveButton(IDC_DOWN     , top);
   moveButton(IDCANCEL     , top);

   CRect wr;
   GetWindowRect(&wr);
   wr.bottom -= bht * (nMaxRows-_nRows) + 6;
   MoveWindow(wr);

   Invalidate();
}

//------------------------------------------------------------------------------

void userDefined::addGroupBox(CString title, int tIdx, int nContainedRows)
{
   int bIdx = tIdx+nContainedRows;

   CRect rct1; GetDlgItem(IDC_UDL01+tIdx)->GetWindowRect(&rct1); ScreenToClient(&rct1);
   CRect rct2; GetDlgItem(IDC_UDC01+bIdx)->GetWindowRect(&rct2); ScreenToClient(&rct2);
   int top = rct1.top;
   int bot = rct2.bottom+2;

   CButton *box = new CButton();
   CRect rct(rct1.left-5, top, rct2.right+5, bot);
   box->Create(title, WS_CHILD | WS_VISIBLE | BS_GROUPBOX, rct, this, 0);
   box->SetFont(this->GetFont());
   _boxList.push_back(box);
}

//------------------------------------------------------------------------------

BOOL userDefined::OnInitDialog() 
{
   userDialog &d = ud[_page];
   dataDialog::OnInitDialog(d.title());
   setHelp(d.helpDlg());

   GetDlgItem(ID_HELP_ABOUT)->ShowWindow(d.helpDlg()[0] != 0);

   int first = -1;

   _hasExprs = false;
   for (int iFld = 0, r = 0; r < nMaxRows; r++) {
      if (iFld >= d.nFields())
         _cell[r].init(this, r, NULL);
      else {
         userEditField &f = d[iFld++];
         _hasExprs |= f.hasExpr();

         if (f.type() != userEditField::fSingle && !f.title().IsEmpty()) {
            _cell[r].init(this, r, NULL);
            addGroupBox(f.title(), r, f.nValues());
            r++;
         }

         int base = r;
         for (int idx = 0; idx < f.nValues(); idx++) {
            int rr = base+idx;
            if (rr >= nMaxRows) {
               _cell[rr-1].setLabel(this, "ERROR, TOO MANY ROWS");
               break; // Need better error recovery.
            }

            _cell[rr].init(this, rr, &f);

            if (_cell[rr].setLabel(this, f.label(idx))) _nRows = rr+1;
            if (_cell[rr].setEdit(base) && first < 0) first = rr;
         }
         r += f.nValues()-1;
      }
   }

   resetEnabled();
   OnUp();

   if (first >= 0) _cell[first].setFocus();

   resize();

   return false;
}

void userDefined::OnDestroy()
{
   for (unsigned iBox = 0; iBox < _boxList.size(); iBox++) {
      delete _boxList[iBox];
   }
   _boxList.clear();
   dataDialog::OnDestroy();
}

//------------------------------------------------------------------------------

void userDefined::resetEnabled()
{
   if (_hasExprs) {
      mdb.getRuntime(); // Update our expressions.
      for (int r = 0; r < _nRows; r++) {
         _cell[r].setEnabled();
      }
   }
}

//------------------------------------------------------------------------------

void userDefined::DoDataExchange(CDataExchange* pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(userDefined)
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(userDefined, dataDialog)
   //{{AFX_MSG_MAP(userDefined)
      ON_BN_CLICKED   (               IDC_DOWN,             OnDown)
      ON_BN_CLICKED   (               IDC_UP,               OnUp)
      ON_CONTROL_RANGE(EN_KILLFOCUS,  IDC_UDV01, IDC_UDV20, OnValueF)
      ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_UDC01, IDC_UDC20, OnValueC)
      ON_WM_CTLCOLOR  ()
      ON_WM_DESTROY   ()
      ON_WM_PAINT     ()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

void userDefined::OnDown() 
{
   // We re-send everything since resets may occur at any time.
   int savePage = mdb.setPageNo(ud[_page].pageNo(), true);
//    CString event;
//    event.Format("DBOX %s page=%d send", ud[_page].title(), ud[_page].pageNo());
//    mdb.logEvent(event);

      std::map<symbol *, bool> sent;
      for (int r = 0; r < _nRows; r++) {
         userEditField *d = _cell[r].fld;
         if (d == NULL) continue;
         symbol *s = d->sym();
         if (s) {
            if (sent.find(s) == sent.end()) {
               mdb.cDesc.send(s);
               sent[s] = true;
            }
         }
      }
      mdb.burnConst(ud[_page].pageNo()); // Burn it into flash.
   mdb.setPageNo(savePage);
   resetEnabled();
}

//------------------------------------------------------------------------------

void userDefined::OnUp() 
{
   int savePage = mdb.setPageNo(ud[_page].pageNo(), true);
//    CString event;
//    event.Format("DBOX %s page=%d load", ud[_page].title(), ud[_page].pageNo());
//    mdb.logEvent(event);

      mdb.getPage(ud[_page].pageNo());
      for (int r = 0; r < _nRows; r++) {
         userEditField *d = _cell[r].fld;
         if (d == NULL) continue;
         symbol        *s = d->sym();
         if (s) {
            if (d->isBits()) setCmb(_cell[r].cmb, s);
            else             setFld(_cell[r].edt, s, r-_cell[r].base());
         }
      }
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------

void userDefined::OnValueF(UINT nId)
{
   UINT           idx  = nId-IDC_UDV01;
   udCell        *pVal = _cell+idx;
   userEditField *d    = pVal->fld;

   int savePage = mdb.setPageNo(ud[_page].pageNo(), true);
//    CString event;
//    event.Format("DBOX %s page=%d value-changed", ud[_page].title(), ud[_page].pageNo());
//    mdb.logEvent(event);

      getFld(pVal->edt, d->sym(), idx-pVal->base());
   mdb.setPageNo(savePage);
   resetEnabled();
}

void userDefined::OnValueC(UINT nId)
{
   udCell        *pVal = _cell+(nId-IDC_UDC01);
   userEditField *d    = pVal->fld;

   int savePage = mdb.setPageNo(ud[_page].pageNo(), true);
//    CString event;
//    event.Format("DBOX %s page=%d value-changed", ud[_page].title(), ud[_page].pageNo());
//    mdb.logEvent(event);

      getCmb(pVal->cmb, d->sym());
   mdb.setPageNo(savePage);
   resetEnabled();
}

//------------------------------------------------------------------------------

static COLORREF grn = RGB(  0, 222,   0);
static COLORREF red = RGB(255,   0,   0);
static COLORREF wht = RGB(255, 255, 255);

void userDefined::OnPaint()
{
   if (!IsIconic()) {
      dataDialog::OnPaint();
#if 0
//    Try to put a border on the highlighted lines.
      CPaintDC dc(this);
      dc.GetSafeHdc();

      CStatic *pWnd = (CStatic *)GetDlgItem(IDC_UDL01);
      CRect r;
      pWnd->GetClientRect(&r);
      r.InflateRect(20,20);
   GetWindowRect(&r);
   ScreenToClient(r);
      CPen   redp; redp.CreatePen(PS_SOLID, 0, red);
      CBrush redb; redb.CreateSolidBrush(red);
      dc.SelectObject(redp);
      dc.SelectObject(redb);
      dc.DrawEdge(r, EDGE_RAISED, BF_FLAT);
      dc.Ellipse(r);
   dc.Draw3dRect(r, (red) & 0xFFFFFF, (~red) & 0xFFFFFF);
#endif
   }
}

//------------------------------------------------------------------------------

HBRUSH userDefined::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
   HBRUSH hbr = dataDialog::OnCtlColor(pDC, pWnd, nCtlColor);
   pDC->SetBkMode(TRANSPARENT);

   UINT nId  = pWnd->GetDlgCtrlID();
   int  iRow = nId - IDC_UDL01;
   if (iRow >= 0 && iRow < nMaxRows) {
      userEditField *f = _cell[iRow].fld;
      if (f && (f->highlight() & (userEditField::hTitle | userEditField::hAlarm))) {
         COLORREF bkg = ::GetSysColor(COLOR_BACKGROUND);
         COLORREF txt = ::GetSysColor(COLOR_WINDOWTEXT);

         if (f->highlight() & userEditField::hTitle) {
            bkg = ::GetSysColor(COLOR_ACTIVECAPTION);
            txt = ::GetSysColor(COLOR_CAPTIONTEXT  );
         }
         if (f->highlight() & userEditField::hAlarm) {
            bkg = red;
            txt = wht;
         }

         pDC->SetTextColor(txt);

         static CBrush brush;
         brush.DeleteObject();
         brush.CreateSolidBrush(bkg);
         hbr = brush;
      }
   }

   return hbr;
}

//------------------------------------------------------------------------------
