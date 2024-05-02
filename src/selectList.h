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

#ifndef SELECTLIST_H
#define SELECTLIST_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dirSelect.h"

//------------------------------------------------------------------------------

class selectList : public dataDialog {
   dirSelect *_ds;
   CString    _title;
   CString    _caption;

public:
   selectList(CWnd *pParent=NULL, dirSelect *ds=NULL, CString title="");

   void setCaption(CString caption);

   //{{AFX_DATA(selectList)
      enum { IDD = IDD_SELECT_LIST };
      CListBox  m_list;
      CStatic   m_caption;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(selectList)
   protected:
      virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(selectList)
      virtual void OnOK();
      virtual BOOL OnInitDialog();
      virtual void OnCancel();
      afx_msg void OnDblclkSelList();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//------------------------------------------------------------------------------

//{{AFX_INSERT_LOCATION}}

#endif // SELECTLIST_H
