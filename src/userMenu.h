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

#ifndef USERMENU_H
#define USERMENU_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------

#include <vector>
#include "megatuneDlg.h"

//------------------------------------------------------------------------------

class userPlugIn {
   public:
      typedef enum { none, minimize, exit } verbType;

      userPlugIn (CString command);
      void exec  ();
      void action(verbType a);

   private:
      CString  _command;
      verbType _action;
};

class userPlugInList {
   public:
      userPlugInList();

      int  add   (CString command);
      void action(userPlugIn::verbType action);

      userPlugIn &operator [](int idx) const;

   private:
      std::vector<userPlugIn *> _upl;
};

//------------------------------------------------------------------------------

class expression;

class userSubMenu {
   public:
      userSubMenu(CString d, CString l, int p, CString e, bool pi=false);

      UINT           dbid       ()      const;
      void           dbid       (UINT id);
      const CString &dbox       ()      const;
      int            pageNo     ()      const;
      const char    *label      ()      const;
      bool           isSeparator()      const;
      bool           isStd      ()      const;
      bool           isPlugIn   ()      const;

      void           contribute ();
      void           attach     (CMenu *parent);
      bool           enable     ();

      void           action     (userPlugIn::verbType pv);

   private:
      CString    _label;  // Field label in popup menu.
      CString    _target; // Name of dialog box to invoke, or path to executable for plug-in.
      UINT       _dbid;   // Id of dialog box.
      int        _pNo;    // Page number if standard dialog.
      bool       _plugIn; // Indicator as to what target means.

      CMenu     *_parent; // This menu item's parent menu.
      UINT       _on;     // Current enable status.

      CString    _expr;   // Indicates when to enable/disable this entry.
      int        _val;    // Pointer to expression's value.
};

//------------------------------------------------------------------------------

class userMenu {
   public:
      userMenu(CString d, CString l, CString e);
     ~userMenu();

      CString            dialog  ()        const;
      CString            label   ()        const;
      const userSubMenu *subMenu (int idx) const;
      int               nSubMenus()        const;

      userSubMenu *add(CString l, CString d, int p, CString e, bool pi);

      void contribute ();
      void attach     (CMenu *parent, int idx);
      bool enable     ();

      userSubMenu &operator [](int idx) const;

   private:
      CString                    _dialog;
      CString                    _label;
      std::vector<userSubMenu *> _usm;

      CMenu                     *_parent; // This menu item's parent menu.
      int                        _idx;    // Index into parent's item list.
      UINT                       _on;     // Current enable status.

      CString                    _expr;   // Evaluate to see if we should disable this entry.
      int                        _val;    // Pointer to expression's value.
};

//------------------------------------------------------------------------------

class userMenuList {
   public:
      userMenuList();
     ~userMenuList();

      int  nMenus() const;

      void         addMenu   (CString d, CString l, CString e);
      userSubMenu *addSubMenu(CString l, CString d, int pageNo, CString e, bool plugIn=false);

      double *getVal(int &idx);

      void attach(CWnd *w, CString d);
      void review(CMegatuneDlg *w); // Pop up all the dboxes in order.
      bool enable();

      userMenu &operator [](int idx) const;

   private:
      std::vector<userMenu *> _um;
      CWnd                   *_window;
};

//------------------------------------------------------------------------------

class userIndicator : public CEdit {
   public:
      userIndicator(
         CString e,                 // Expression
         CString lf,  CString ln,   // Labels, off and on
         CString bgf, CString fgf,  // Off colors
         CString bgn, CString fgn); // On colors

      UINT     id() const;
      COLORREF fg() const;
      COLORREF bg() const;

      void attach(CWnd *p);
      bool enable();
      bool setBrush(CWnd *pWnd, CDC *pDC, CBrush &brush);

   private:
      CString   _label[2];
      CWnd     *_parent;
      CString   _expr;   // Evaluate to see if we should disable this entry.
      int       _val;    // Pointer to expression's value.
      bool      _on;
      UINT      _id;
      COLORREF  _fg[2];
      COLORREF  _bg[2];
      CString   _fgName[2];
      CString   _bgName[2];
};

class userIndicatorList {
   public:
      userIndicatorList();
     ~userIndicatorList();

      void add(
         CString w,                 // Window name
         CString e,                 // Expression
         CString lf,  CString ln,   // Labels, off and on
         CString bgf, CString fgf,  // Off colors
         CString bgn, CString fgn); // On colors

      void set(
         CString n,                 // Built-in indicator name
         CString bgf, CString fgf,  // Off colors
         CString bgn, CString fgn); // On colors

      int nIndicators(CString windowName) const;

      void reset ();
      void attach(CWnd *w, CString windowName);
      bool enable();

      userIndicator &operator [](int idx) const;

   private:
//    typedef std::vector<userIndicator *> uiList;
//    std::map<CString, uiList>    _ui;
      std::vector<userIndicator *> _ui;
      CWnd                        *_window;
};

//------------------------------------------------------------------------------
#endif // USERMENU_H
