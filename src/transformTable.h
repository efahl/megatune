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
//
#ifndef TRANSFORMTABLE_H
#define TRANSFORMTABLE_H 1

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class transformTable : public dataDialog {
   int nId;

public:
   transformTable(int nId, CWnd *pParent=NULL);

   //{{AFX_DATA(transformTable)
      enum { IDD = IDD_XFORM_TABLE };
      CButton m_scale;
      CEdit   m_value;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(transformTable)
   protected:
      virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(transformTable)
      virtual void OnOK();
      virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif // TRANSFORMTABLE_H
