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

#ifndef GENO2_H
#define GENO2_H 1

//------------------------------------------------------------------------------

#if _MSC_VER > 1000
#  pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------

class genO2 : public dataDialog
{
   int pageNo;

public:
   genO2(int pageNo, CWnd *pParent=NULL);

   //{{AFX_DATA(genO2)
      enum { IDD = IDD_GENO2 };
      CComboBox m_tableType;
      double m_volt1;
      double m_volt2;
      double m_afr1;
      double m_afr2;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(genO2)
   protected:
      virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(genO2)
      virtual void OnOK();
      virtual BOOL OnInitDialog();
      afx_msg void OnSelectionChange();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif
