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

#include "stdafx.h"
#include "megatune.h"
#include "DlogOptions.h"
#include "datalog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern datalogOptions lop;

//------------------------------------------------------------------------------

DlogOptions::DlogOptions(CWnd *pParent)
 : dataDialog(DlogOptions::IDD, pParent)
{
   //{{AFX_DATA_INIT(DlogOptions)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void DlogOptions::DoDataExchange(CDataExchange *pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(DlogOptions)
      DDX_Control(pDX, IDC_LOG_FILE_COMMENTS, m_commentText);
      DDX_Control(pDX, IDC_LOG_DUMP_BEFORE,   m_logBefore);
      DDX_Control(pDX, IDC_LOG_DUMP_AFTER,    m_logAfter);
    //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(DlogOptions, CDialog)
   //{{AFX_MSG_MAP(DlogOptions)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

BOOL DlogOptions::OnInitDialog() 
{
   dataDialog::OnInitDialog("Datalog Options");

   m_logAfter .SetCheck(lop._dumpAfter  ? BST_CHECKED : BST_UNCHECKED);
   m_logBefore.SetCheck(lop._dumpBefore ? BST_CHECKED : BST_UNCHECKED);

   m_commentText.SetLimitText(sizeof(lop._commentText));
   m_commentText.SetWindowText(lop._commentText);
   m_commentText.SetModify(false);
   m_commentText.SetFocus();

   return false;
}

//------------------------------------------------------------------------------

void DlogOptions::OnOK() 
{
   lop._dumpAfter  = m_logAfter. GetCheck() == BST_CHECKED;
   lop._dumpBefore = m_logBefore.GetCheck() == BST_CHECKED;

   if (m_commentText.GetModify()) {
      m_commentText.GetWindowText(lop._commentText, sizeof(lop._commentText));
   }

   CDialog::OnOK();
}

//------------------------------------------------------------------------------
