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

#ifndef DVEGEN_H
#define DVEGEN_H 1

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------

class DveGen : public dataDialog {
   int nId;

public:
   DveGen(int nId, CWnd *pParent=NULL);

   //{{AFX_DATA(DveGen)
      enum { IDD = IDD_DVE_GEN };
      CButton m_dispCID;
      CEdit   m_disp;
      CEdit   m_idleRPM;
      CEdit   m_idleMAP;
      CEdit   m_ptq;
      CEdit   m_ptqRPM;
      CEdit   m_ptqMAP;
      CEdit   m_php;
      CEdit   m_phpRPM;
      CEdit   m_phpMAP;
      CEdit   m_redRPM;
      CEdit   m_redMAP;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(DveGen)
   protected:
      virtual void DoDataExchange(CDataExchange* pDX);
   //}}AFX_VIRTUAL

protected:

   // Generated message map functions
   //{{AFX_MSG(DveGen)
      virtual void OnOK();
      virtual BOOL OnInitDialog();
      afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
