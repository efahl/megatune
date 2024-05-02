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

#ifndef DREQFUEL_H
#define DREQFUEL_H

#if _MSC_VER >= 1000
#  pragma once
#endif

class Dreqfuel : public dataDialog {
   int    _nCylinders;
   double _reqFuel;

public:
   Dreqfuel(int nCylinder, CWnd *pParent=NULL);

   double getValue();

   static double cid;
   static double injectorFlow;
   static double afr;

   //{{AFX_DATA(Dreqfuel)
      enum { IDD = IDD_REQFUEL };
      CEdit   m_cid;
      CEdit   m_nCylinders;
      CEdit   m_injectorFlow;
      CEdit   m_afr;
      CButton m_dispCID;
      CButton m_flowLBH;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(Dreqfuel)
protected:
      virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(Dreqfuel)
      afx_msg void OnDestroy();
      virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif
