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

#ifndef USERDEFINED_H
#define USERDEFINED_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------

#include <vector>
#include <map>
#include "symbol.h"

//------------------------------------------------------------------------------

class stdEditField {
public:
   stdEditField(dataDialog *p);

   bool   add(const CString &symName, UINT nId, int pageNo, int idx=0);

   void   send();
   void   setFld();
   void   getFld();

   double value();

private:
   dataDialog *_par;
   CEdit      *_fld;
   CComboBox  *_cmb;
   symbol     *_sym;
   int         _idx;
};

class stdEditFieldList {
   typedef std::map<const CString, stdEditField *> fldMap;
   typedef fldMap::iterator                        fldIter;

public:
   stdEditFieldList(dataDialog *p);
  ~stdEditFieldList();

   void add(const char *varName, UINT widgetId, int pageNo, int idx=0);

   void send();
   void setFld();
   void getFld();

   stdEditField *operator[](const char *varName);

   double value(const char *varName, int idx=0);

private:
   dataDialog *_par;
   fldMap      _map;

   CString     n(const char *v, int i);
};

//------------------------------------------------------------------------------

class userEditField {
   public:
      typedef enum { hRight = 1, hAlarm = 2, hTitle = 4 }; // Bit settings

      userEditField(CString t, CString l, CString a, CString e);
      userEditField(CString l, CString c, CString e);

      symbol        *sym()           const;
      symbol        *sym(symbol *s);
      bool           isBits   ()     const;
      const CString &name     ()     const;
      bool           isEnabled()     const;
      bool           hasExpr  ()     const;
      int            highlight()     const;
      CString        title    ()     const;
      int            nValues  ()     const;
      CString        label(int idx)  const;

      typedef enum { fSingle, fArray1D, fArray2D } fieldType;
      fieldType      type     ()     const;

      void contribute();

   private:
      void      setHighlight();

      fieldType _type;
      CString   _title;     // Title bar for array values.
      CString   _label;     // Field label in dialog box.
      CString   _name;      // Name of symbol.
      symbol   *_sym;
      int       _highlight;

      CString   _expr;      // Expression determining whether this field is enabled.
      int       _val;       // Index into channels for value of _expr.
};

class userDialog {
   public:
      userDialog(CString t, CString d, UINT id, int p);
     ~userDialog();

      const char *dbid   () const;
      const char *title  () const;
      int         pageNo () const;
      int        nFields () const;

      void add          (CString t, CString l, CString a, CString e);
      void add          (CString l, CString c, CString e);
      void contribute   ();

      userEditField &operator[](int idx);

      const char *helpDlg  () const;
      void        setHelp  (CString h);

   private:
      std::vector<userEditField *> _uef;
      CString _title;
      CString _dbid;
      UINT    _idno;
      int     _pageNo;
      CString _helpDlg;
};

//------------------------------------------------------------------------------

class userDialogList {
   typedef std::map<CString, UINT> strMap;
   typedef strMap::const_iterator  strMapIter;

public:
   userDialogList();
  ~userDialogList();

   const userDialog *dialog (int idx) const;
   int              nDialogs()        const;

   void registerDialog(CString d, UINT id);
   void addDialog     (CString d, CString t, int p);

   UINT id(const CString &d) const;

   void init();

   userDialog &operator[](int idx) const;

private:
   std::vector<userDialog *> _ud;
   strMap                    _udId;

   void registerStdDialogs();
};

//------------------------------------------------------------------------------

class udCell {
public:
   udCell();

   void init(dataDialog *dlg, int idx, userEditField *d);

   bool setLabel  (CWnd *wnd, const CString label);
   bool setEdit   (UINT base);
   void setEnabled();
   void setFocus  ();

   bool hasExpr();
   bool hasData();

   UINT lblId();
   UINT base ();

public:
   CStatic       *lbl;
   CEdit         *edt;
   CComboBox     *cmb;
   userEditField *fld;
   UINT           bas;
};

//------------------------------------------------------------------------------

class userDefined : public dataDialog {
   enum { nMaxRows = 20 };
   udCell _cell  [nMaxRows];
   int    _nRows;
   int    _page;
   bool   _hasExprs;

   std::vector<CButton *> _boxList;

   void   addGroupBox(CString title, int tIdx, int nRows);
   void   moveButton(UINT id, int top);
   void   resize();
   void   resetEnabled();

public:
   userDefined(int pageNo, CWnd* pParent = NULL);

   //{{AFX_DATA(userDefined)
      enum { IDD = IDD_USER_DEFINED };
   //}}AFX_DATA

   //{{AFX_VIRTUAL(userDefined)
   protected:
      virtual void DoDataExchange(CDataExchange* pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(userDefined)
      afx_msg void OnDown();
      afx_msg void OnUp();
      virtual BOOL OnInitDialog();
      afx_msg void OnDestroy();
      afx_msg void OnValueF(UINT nId);
      afx_msg void OnValueC(UINT nId);
      afx_msg void OnPaint();
      afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif // USERDEFINED_H
