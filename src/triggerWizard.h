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

#ifndef TRIGGER_WIZARD_H
#define TRIGGER_WIZARD_H 1

#if _MSC_VER > 1000
#  pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------

class triggerWizard : public dataDialog {
   int     _pageNo;

   double  _advance;
   symbol *_triggerOffset;
   symbol *_advanceVariable;

   void dbFromFields();

public:
   triggerWizard(int pageNo, CWnd *pParent=NULL);

   virtual bool doTimer(bool connected);

   //{{AFX_DATA(triggerWizard)
      enum { IDD = IDD_TRIGGER_WIZARD };
      CStatic   m_gauge;
      CButton   m_retardButton;
      CButton   m_advanceButton;
      CEdit     m_triggerOffset;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(triggerWizard)
      public:
         virtual BOOL PreTranslateMessage(MSG *pMsg);
      protected:
         virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(triggerWizard)
      afx_msg void   OnRetard();
      afx_msg void   OnAdvance();
      afx_msg void   OnBurn();
      virtual BOOL   OnInitDialog();
      afx_msg void   OnPaint();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif
