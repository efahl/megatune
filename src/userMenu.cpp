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
#include "msDatabase.h"
#include "userMenu.h"
#include "parser.h"
#include "repository.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------

extern msDatabase     mdb;
extern userMenuList   uml;
extern userPlugInList upl;
extern repository     rep;
extern CString        installationDir;
extern CString        megasquirtDir;
extern CString        commonDir;
extern CString        workingDir;

//==============================================================================

userPlugIn::userPlugIn(CString command)
 : _command(command)
 , _action (none   )
{
   _command.Replace("%MEGASQUIRT_DIR%",  megasquirtDir);
   _command.Replace("%PLUGINS_DIR%",     megasquirtDir+"/plugIns");
   _command.Replace("%COMMON_DIR%",      commonDir);
   _command.Replace("%INSTALL_DIR%",     installationDir);
   _command.Replace("%WORKING_DIR%",     workingDir);
}

void userPlugIn::exec()
{
   char *verb = "open";
   CString ext = _command.Right(4);
   ext.MakeLower();
   if (ext == ".doc") verb = "edit";
   if (ext == ".txt") verb = "edit";
   if (ext == ".log") verb = "edit";
   UINT status = UINT(ShellExecute(NULL, verb, _command, NULL, NULL, SW_SHOWNORMAL));
   if (status < 32) {
      msgOk(" Execute Plug-in",
            "Couldn't %s %s\t\n\n"
            "ShellExecute error code %d.", verb, _command, status);
   }
   else {
      switch (_action) {
         case none:                                                   break;
         case minimize: AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);     break;
         case exit:     AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0); break;

      }
   }
}

void userPlugIn::action(verbType a) { _action = a; }

//------------------------------------------------------------------------------

userPlugInList::userPlugInList() { }

int userPlugInList::add(CString command)
{
   _upl.push_back(new userPlugIn(command));
   return ID_PLUGIN_00 + _upl.size()-1;
}

void userPlugInList::action(userPlugIn::verbType a)
{
   _upl.back()->action(a);
}

userPlugIn &userPlugInList::operator [](int idx) const { return *_upl[idx]; }

//==============================================================================

userSubMenu::userSubMenu(CString d, CString l, int p, CString e, bool plugIn)
 : _label (l)
 , _target(d)
 , _dbid  (0)
 , _pNo   (p)
 , _expr  (e)
 , _parent(NULL)
 , _on    (0)
 , _val   (-1)
 , _plugIn(plugIn)
{
   if (_plugIn) _dbid = upl.add(_target);
}

UINT           userSubMenu::dbid()        const { return _dbid;          }
void           userSubMenu::dbid(UINT id)       { _dbid = id;            }
const CString &userSubMenu::dbox ()       const { return _target;        } // Check _plugIn?
const char    *userSubMenu::label()       const { return _label;         }
int            userSubMenu::pageNo()      const { return _pNo;           }
bool           userSubMenu::isSeparator() const { return _target == "std_separator"; }
bool           userSubMenu::isStd()       const { return _target.Left(4) == "std_"; }
bool           userSubMenu::isPlugIn()    const { return _plugIn;        }
void           userSubMenu::action(userPlugIn::verbType pv) { upl.action(pv); }

void userSubMenu::contribute()
{
   if (!_expr.IsEmpty()) {
      _val = mdb.cDesc.addExpr(_expr);
   }
}

void userSubMenu::attach(CMenu *parent) { _parent = parent; }

bool userSubMenu::enable()
{
   if (_val == -1 || _parent == NULL) return false;
   UINT on = mdb.cDesc._userVar[_val] != 0.0 ? MF_ENABLED : MF_GRAYED;
   if (on == _on) return false;
   _parent->EnableMenuItem(_dbid, MF_BYCOMMAND | on);
   _on = on;
   return true;
}

//==============================================================================

userMenu::userMenu(CString d, CString l, CString e)
 : _dialog(d)
 , _label (l)
 , _expr  (e)
 , _parent(NULL)
 , _on    (0)
 , _val   (-1)
{ }

userMenu::~userMenu()
{
   for (int i = 0; i < _usm.size(); i++) {
      delete _usm[i];
   }
}

CString            userMenu::dialog   ()        const { return _dialog;     }
CString            userMenu::label    ()        const { return _label;      }
const userSubMenu *userMenu::subMenu  (int idx) const { return _usm[idx];   }
int                userMenu::nSubMenus()        const { return _usm.size(); }

userSubMenu *userMenu::add(CString l, CString d, int p, CString e, bool pi)
{
   _usm.push_back(new userSubMenu(l, d, p, e, pi));
   return _usm.back();
}

void userMenu::contribute()
{
   if (!_expr.IsEmpty()) {
      _val = mdb.cDesc.addExpr(_expr);
   }

   for (int i = 0; i < _usm.size(); i++) {
      _usm[i]->contribute();
   }
}
void userMenu::attach(CMenu *parent, int idx)
{
   _parent = parent;
   _idx    = idx;
}

bool userMenu::enable()
{
   if (_parent == NULL) return false;

   bool changed = false;

   if (_val != -1) {
      UINT on = mdb.cDesc._userVar[_val] != 0.0 ? MF_ENABLED : MF_GRAYED;
      if (on != _on) {
         _parent->EnableMenuItem(_idx, MF_BYPOSITION | on);
         changed = true;
         _on     = on;
      }
   }

   for (int i = 0; i < _usm.size(); i++) {
      changed = _usm[i]->enable() || changed;
   }
   return changed;
}

userSubMenu &userMenu::operator [](int idx) const { return *_usm[idx]; }

//==============================================================================

userMenuList::userMenuList()
 : _window(NULL)
{
}

userMenuList::~userMenuList()
{
   for (int i = 0; i < _um.size(); i++) {
      delete _um[i];
   }
}

int       userMenuList::nMenus()             const { return _um.size(); }
userMenu &userMenuList::operator [](int idx) const { return *_um[idx];  }

void userMenuList::addMenu(CString d, CString l, CString e) {
   _um.push_back(new userMenu(d, l, e));
}

userSubMenu *userMenuList::addSubMenu(CString l, CString d, int pageNo, CString e, bool plugIn) {
   return _um.back()->add(l, d, pageNo, e, plugIn);
}

//------------------------------------------------------------------------------
//--  http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vclib/html/_mfc_cmenu.3a3a.attach.asp

static CString clean(CString l) { l.Replace("&", ""); return l; }

void userMenuList::attach(CWnd *win, CString dialog)
{
   _window = win;
   CMenu *mainM = _window->GetMenu();

   int menuIdx  = 0;
   std::map<CString, CMenu *> menuList;
   for (UINT iItem = 0; iItem < mainM->GetMenuItemCount(); iItem++) {
      CString menuName;
      mainM->GetMenuString(iItem, menuName, MF_BYPOSITION);
      menuName = clean(menuName);
      menuList[menuName] = mainM->GetSubMenu(iItem);
      if (menuName == "File"    ) menuIdx = iItem; // Insert new ones after these.
      if (menuName == "Settings") menuIdx = iItem;
   }

   int iMenu;
   for (iMenu = 0; iMenu < _um.size(); iMenu++) {
      userMenu &um = *_um[iMenu];
      if (um.dialog() != dialog) continue;

      std::map<CString, CMenu *>::iterator iM = menuList.find(clean(um.label()));

      CMenu *pM;
      if (iM != menuList.end())
         pM = (*iM).second;
      else {
         pM = new CMenu();
         pM->CreatePopupMenu();
         menuList[clean(um.label())] = pM;
         menuIdx++;
         um.attach(mainM, menuIdx);
         mainM->InsertMenu(menuIdx, MF_BYPOSITION | MF_POPUP, UINT(pM->m_hMenu), um.label());
      }

      for (int iSubMenu = 0; iSubMenu < um.nSubMenus(); iSubMenu++) {
         userSubMenu &sm = um[iSubMenu];
         if (sm.isSeparator())
            pM->AppendMenu(MF_SEPARATOR, 0, "");
         else {
            pM->AppendMenu(0, sm.dbid(),  sm.label());
            sm.attach(pM);
         }
      }
   }

   CMenu *fileM = menuList[clean("File")];
   if (fileM != NULL)
      fileM->AppendMenu(MF_SEPARATOR, 0, "");
   else {
      fileM = new CMenu();
      fileM->CreatePopupMenu();
      mainM->InsertMenu(0, MF_BYPOSITION | MF_POPUP, UINT(fileM->m_hMenu), "&File");
   }

   CString exit = "E&xit";
   if (dialog != "main") exit += "\tAlt-C";
   fileM->AppendMenu(0, IDCANCEL, exit);

   for (iMenu = 0; iMenu < _um.size(); iMenu++) {
      _um[iMenu]->contribute();
   }
}

//------------------------------------------------------------------------------

void userMenuList::review(CMegatuneDlg *win)
{
   for (int iMenu = 0; iMenu < _um.size(); iMenu++) {
      userMenu &um = *_um[iMenu];
      for (int iSubMenu = 0; iSubMenu < um.nSubMenus(); iSubMenu++) {
         userSubMenu &sm = um[iSubMenu];
         if (sm.isPlugIn()                   ) continue;
         UINT id = sm.dbid();
         if (id == 0                         ) continue;
         if (id == ID_RUNTIME_REALTIMEDISPLAY) continue;
         if (id == ID_STD_GTH_00             ) continue;
         if (id == ID_STD_MAF_00             ) continue;
         if (id == ID_STD_GO2_00             ) continue;
         if (id >= ID_USER_TUNING_00 && id <= ID_USER_TUNING_49) continue;
         if (id >= ID_USER_HELP_00   && id <= ID_USER_HELP_49  ) continue;
         win->exec(id);
      }
   }
}

//------------------------------------------------------------------------------

bool userMenuList::enable()
{
   if (_window == NULL) return false;

   bool changed = false;
   for (int i = 0; i < _um.size(); i++) {
      changed = _um[i]->enable() || changed;
   }
   if (changed) _window->DrawMenuBar();
   return changed;
}

//==============================================================================

userIndicator::userIndicator(CString e, CString lf, CString ln, CString bgf, CString fgf, CString bgn, CString fgn)
 : CEdit   ()
 , _parent (NULL)
 , _expr   (e)
 , _val    (-1)
 , _on     (true)
{
   static UINT nId = 9997;
   _label[0] = lf; _fgName[0] = fgf; _bgName[0] = bgf; _fg[0] = RGB(  0,  0,  0); _fg[1] = RGB(255,255,255);
   _label[1] = ln; _fgName[1] = fgn; _bgName[1] = bgn; _bg[0] = RGB(255,255,255); _bg[1] = RGB(255,  0,  0);
   _id = nId++;
}

void userIndicator::attach(CWnd *p)
{
   _parent = p;

   static DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_READONLY;
   CRect rct(0,0,1,1);
   CreateEx(WS_EX_CLIENTEDGE, "Edit", NULL, style, rct, _parent, _id);
   SetFont(_parent->GetFont());
   SetWindowText(_label[1]);
   EnableWindow (true);

   if (!_fgName[0].IsEmpty()) _fg[0] = rep.color(_fgName[0]);
   if (!_bgName[0].IsEmpty()) _bg[0] = rep.color(_bgName[0]);
   if (!_fgName[1].IsEmpty()) _fg[1] = rep.color(_fgName[1]);
   if (!_bgName[1].IsEmpty()) _bg[1] = rep.color(_bgName[1]);

   if (!_expr.IsEmpty()) {
      _val = mdb.cDesc.addExpr(_expr);
   }
}

UINT     userIndicator::id() const { return _id;      }
COLORREF userIndicator::fg() const { return _fg[_on]; }
COLORREF userIndicator::bg() const { return _bg[_on]; }

bool userIndicator::enable()
{
   if (_val == -1 || _parent == NULL) return false;
   bool on = mdb.cDesc._userVar[_val] != 0.0;
   SetWindowText(_label[on]);
   if (on == _on) return false;
// EnableWindow(on);
   Invalidate();
   _on = on;
   return true;
}

bool userIndicator::setBrush(CWnd *pWnd, CDC *pDC, CBrush &brush)
{
   if (pWnd->GetDlgCtrlID() == id()) {
      pDC->SetTextColor(fg());
      pDC->SetBkMode(TRANSPARENT);
      brush.DeleteObject();
      brush.CreateSolidBrush(bg());
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------

userIndicatorList::userIndicatorList()
 : _window(NULL)
{
}

userIndicatorList::~userIndicatorList()
{
}

void userIndicatorList::set(CString n, CString bgf, CString fgf, CString bgn, CString fgn)
{
}

void userIndicatorList::reset()
{
   for (int iIndicator = 0; iIndicator < _ui.size(); iIndicator++) {
      delete _ui[iIndicator];
   }
   _ui.clear();
}

void userIndicatorList::add(CString w, CString e, CString lf, CString ln, CString bgf, CString fgf, CString bgn, CString fgn)
{
   _ui.push_back(new userIndicator(e, lf, ln, bgf, fgf, bgn, fgn));
}

void userIndicatorList::attach(CWnd *w, CString windowName)
{
   _window = w;
   for (int iIndicator = 0; iIndicator < _ui.size(); iIndicator++) {
      _ui[iIndicator]->attach(w);
   }
}

int userIndicatorList::nIndicators(CString windowName) const { return _ui.size(); }

bool userIndicatorList::enable()
{
   if (_window == NULL) return false;

   bool changed = false;
   for (int i = 0; i < _ui.size(); i++) {
      changed = _ui[i]->enable() || changed;
   }
   return changed;
}

userIndicator &userIndicatorList::operator [](int idx) const { return *_ui[idx]; }

//------------------------------------------------------------------------------
