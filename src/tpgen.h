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

#ifndef TPGEN_H
#define TPGEN_H 1

#if _MSC_VER > 1000
#pragma once
#endif

class tpgen : public dataDialog {
   int lo;
   int hi;

   double _tpsMin;
   double _tpsMax;
   bool   _looksLikeMSII;

   void getVal(CEdit &fld);
   int  pctFromAdc(int adc);
   void writeFile();

public:
   tpgen(CWnd* pParent = NULL);

   //{{AFX_DATA(tpgen)
      enum { IDD = IDD_TP_CALIBRATE };
      CEdit   m_tp_lo;
      CEdit   m_tp_hi;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(tpgen)
   protected:
      virtual void DoDataExchange(CDataExchange* pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(tpgen)
      virtual void OnOK();
      afx_msg void OnGetLo();
      afx_msg void OnGetHi();
      virtual BOOL OnInitDialog();
      virtual void OnCancel();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif // TPGEN_H
