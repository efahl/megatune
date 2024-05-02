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

#ifndef LOGOPTIONS_H
#define LOGOPTIONS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------

class DlogOptions : public dataDialog {
public:
   DlogOptions(CWnd *pParent=NULL);

   //{{AFX_DATA(DlogOptions)
      enum { IDD = IDD_LOG_OPTIONS };
      CEdit   m_commentText;
      CButton m_logBefore;
      CButton m_logAfter;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(DlogOptions)
protected:
      virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(DlogOptions)
      virtual void OnOK();
      virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif
