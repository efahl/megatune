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

#ifndef DABOUT_H
#define DABOUT_H

#if _MSC_VER > 1000
#  pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------

class Dabout : public dataDialog {
   HBITMAP  m_hBitmap;
   HPALETTE m_hPalette;
   int      m_top;
   int      m_width;

public:
   Dabout(CWnd *pParent=NULL);

   //{{AFX_DATA(Dabout)
      enum { IDD = IDD_ABOUT };
      CStatic m_aboutText;
      CButton m_okButton;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(Dabout)
   protected:
      virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL

   protected:
      //{{AFX_MSG(Dabout)
         virtual BOOL   OnInitDialog();
         afx_msg void   OnPaint();
         afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif
