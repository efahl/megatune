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

#ifndef GENMAF_H
#define GENMAF_H 1

//------------------------------------------------------------------------------

#if _MSC_VER > 1000
#  pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------

class genMAF : public dataDialog
{
   int pageNo;

   void setLabel(bool degC);

   double A, B, C;

   double k2f(double t);
   double Tk (double R);
   double Tf (double R);
   double Tu (double f, bool degC);

public:
   genMAF(int pageNo, CWnd *pParent=NULL);

   //{{AFX_DATA(genMAF)
      enum { IDD = IDD_GENMAF };
      CComboBox      m_tableType;
      CButton        m_tempSetting;
      CStatic        m_tempLabel;
      float          m_biasValue;
      float          m_af1;
      float          m_af2;
      float          m_af3;
      float          m_mafvolts1;
      float          m_mafvolts2;
      float          m_mafvolts3;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(genMAF)
   protected:
      virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(genMAF)
      virtual void OnOK();
      virtual BOOL OnInitDialog();
      afx_msg void OnTemp();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnEnChangeTemp1();
   afx_msg void OnCbnSelchangeTableType();
   afx_msg void OnEnChangeTemp3();
   afx_msg void OnEnChangeResi2();
   afx_msg void OnStnClickedTempLabel();
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif
