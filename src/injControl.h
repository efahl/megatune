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

//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#ifndef INJCONTROL_H
#define INJCONTROL_H 1

#if _MSC_VER >= 1000
#  pragma once
#endif

//------------------------------------------------------------------------------

class injControl : public dataDialog {
   int  pageNo;

   void dbFromFields();
   void fieldsFromDb();

   stdEditFieldList _fld;
   symbol          *_rpmk;
   symbol          *_divider;

public:
   injControl(int pageNo, CWnd *pParent=NULL);

   //{{AFX_DATA(injControl)
      enum { IDD = IDD_INJCONTROL };
      CEdit     m_reqFuel;
      CEdit     m_reqFuelDown;
      CComboBox m_nSquirts;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(injControl)
   protected:
      virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL

   protected:
   //{{AFX_MSG(injControl)
      virtual BOOL OnInitDialog();
      afx_msg void OnDreqfuel();
      afx_msg void OnUp();
      afx_msg void OnDown();
//      DECLARE_EVENTSINK_MAP()
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
