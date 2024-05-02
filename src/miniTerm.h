//------------------------------------------------------------------------------
//--  Copyright (c) 2005,2006 by Eric Fahlgren, All Rights Reserved.          --
//--                                                                          --
//--  This program is free software; you can redistribute it and/or           --
//--  modify it under the terms of the GNU General Public License             --
//--  as published by the Free Software Foundation; either version            --
//--  2 of the License, or (at your option) any later version.                --
//--  See http://www.gnu.org/licenses/gpl.txt                                 --
//------------------------------------------------------------------------------

#ifndef MINITERM_H
#define MINITERM_H 1
//------------------------------------------------------------------------------

#if _MSC_VER > 1000
#  pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------

class miniTerm : public dataDialog
{
   bool realized;

public:
   miniTerm(CWnd *pParent=NULL);

   //{{AFX_DATA(miniTerm)
      enum { IDD = IDD_MINITERM };
      CEdit     m_text;
      CComboBox m_baudRate;
      CComboBox m_commPort;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(miniTerm)
      public:
         virtual BOOL PreTranslateMessage(MSG* pMsg);
      protected:
         virtual void DoDataExchange(CDataExchange* pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(miniTerm)
      virtual BOOL   OnInitDialog     ();
      afx_msg void   OnSettingsChanged();
      afx_msg void   OnDestroy        ();
      afx_msg void   OnPaint          ();
      afx_msg void   OnTimer          (UINT nIDEvent);
      afx_msg HBRUSH OnCtlColor       (CDC *pDC, CWnd *pWnd, UINT nCtlColor);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif
