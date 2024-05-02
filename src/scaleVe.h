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
#ifndef SCALEVE_H
#define SCALEVE_H 1

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class scaleVe : public dataDialog {
   int nId;

public:
   scaleVe(int nId, CWnd* pParent = NULL);

   //{{AFX_DATA(scaleVe)
      enum { IDD = IDD_SCALE_VE };
      CButton m_reset_reqFuel;
      CEdit   m_newReqFuel;
      CEdit   m_oldReqFuel;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(scaleVe)
   protected:
      virtual void DoDataExchange(CDataExchange* pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(scaleVe)
      virtual void OnOK();
      virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif // SCALEVE_H
