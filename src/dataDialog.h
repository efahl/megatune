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

#ifndef DATADIALOG_H
#define DATADIALOG_H 1

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------

#include "symbol.h"

class dataDialog : public CDialog {
   public:
      dataDialog(int ID, CWnd *pParent=NULL);

      static void setGlobalIcon(HICON icon) { m_hIcon = icon; }

      //{{AFX_DATA(dataDialog)
         enum { IDD = IDD_DATADIALOG };
      //}}AFX_DATA

      //{{AFX_VIRTUAL(dataDialog)
      protected:
         virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
      //}}AFX_VIRTUAL

   public:
      virtual int DoModal();

   protected:
      void moveItem(UINT id, int deltaH, int deltaV);

      // Old forms, get rid of them.
      void   setFld(CEdit *c, int    i);
      void   setFld(CEdit *c, double v, double scale=1.0, double trans=0.0, int w=1);
      void   setCmb(CComboBox *c, int i, int bitLo, int bitHi);

      int    getFld(CEdit *fld, double lo, double hi, const char *label, double scale=1.0, int trans=0, int digits=0);
      double getRaw(CEdit *fld, double lo=symbol::noRange, double hi=symbol::noRange, const char *label="");
      int    getCmb(CComboBox *cmb, int byteVal, int bitLo, int bitHi);

   public:
      // New forms, use only them.
      void   setFld(CEdit     *c, const symbol *s, int index=0);
      void   setCmb(CComboBox *c, const symbol *s);

      void   getFld(CEdit     *c, const symbol *s, int index=0);
      void   getCmb(CComboBox *c, const symbol *s);

   protected:
      //{{AFX_MSG(dataDialog)
         virtual BOOL OnInitDialog(const char *title="Data", int pageNo=0);
         afx_msg BOOL OnHelpInfo  (HELPINFO* pHelpInfo);
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()

   public:
      const char *helpDlg  () const;
      void        setHelp  (CString h);

   protected:
      bool doHelp();
      bool realized();

   public:
      virtual bool doTimer(bool connected) { return true; } // = 0;

   private:
      static HICON m_hIcon;

      bool         _realized;
      CString      _helpDlg;
      UINT         _hId;
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif
