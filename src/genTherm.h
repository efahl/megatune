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

#ifndef GENTHERM_H
#define GENTHERM_H 1

//------------------------------------------------------------------------------

#if _MSC_VER > 1000
#  pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------

class genTherm : public dataDialog
{
   int pageNo;

   void setLabel(bool degC);

   double A, B, C;

   double k2f(double t);
   double Tk (double R);
   double Tf (double R);
   double Tu (double f, bool degC);

public:
   genTherm(int pageNo, CWnd *pParent=NULL);

   //{{AFX_DATA(genTherm)
      enum { IDD = IDD_GENTHERM };
      CComboBox      m_tableType;
      CButton        m_tempSetting;
      CStatic        m_tempLabel;
      float          m_biasValue;
      float          m_temp1;
      float          m_temp2;
      float          m_temp3;
      float          m_resi1;
      float          m_resi2;
      float          m_resi3;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(genTherm)
   protected:
      virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(genTherm)
      virtual void OnOK();
      virtual BOOL OnInitDialog();
      afx_msg void OnTemp();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif
