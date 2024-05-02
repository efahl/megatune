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

#ifndef DSETCOMM_H
#define DSETCOMM_H

#if _MSC_VER >= 1000
#  pragma once
#endif

//*****************************************
//******** PC Configurator V1.00 **********
//*** (C) - 2001 B.Bowling/A. Grippo ******
//** All derivatives from this software ***
//**  are required to keep this header ****
//*****************************************

//------------------------------------------------------------------------------

class Dsetcomm:public dataDialog {
public:
   Dsetcomm(CWnd *pParent=NULL);

   //{{AFX_DATA(Dsetcomm)
      enum { IDD = IDD_SETCOMM };
      CComboBox m_selspeed;
      CEdit     m_Status;
      CEdit     m_timerInterval;
      CComboBox m_selserial;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(Dsetcomm)
   protected:
      virtual void   DoDataExchange(CDataExchange *pDX);  // DDX/DDV support
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(Dsetcomm)
      afx_msg void   OnTestcomm();
      afx_msg void   OnSelchangeSelectcomm();
      virtual BOOL   OnInitDialog();
      afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif
