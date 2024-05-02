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
#include "selectList.h"
#include "dirSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------

selectList::selectList(CWnd* pParent, dirSelect *ds, CString title)
 : dataDialog(selectList::IDD, pParent)
 , _ds       (ds)
 , _title    (title)
{
   //{{AFX_DATA_INIT(selectList)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void selectList::DoDataExchange(CDataExchange* pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(selectList)
      DDX_Control(pDX, IDC_SEL_LIST, m_list);
      DDX_Control(pDX, IDC_CAPTION,  m_caption);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(selectList, dataDialog)
   //{{AFX_MSG_MAP(selectList)
      ON_LBN_DBLCLK(IDC_SEL_LIST, OnDblclkSelList)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

void selectList::OnOK() 
{
   EndDialog(m_list.GetCurSel());
}

void selectList::OnDblclkSelList()
{ 
   OnOK(); 
}

//------------------------------------------------------------------------------

void selectList::OnCancel() 
{
   EndDialog(-1);
}

//------------------------------------------------------------------------------

BOOL selectList::OnInitDialog() 
{
   dataDialog::OnInitDialog(_title);
   for (int i = 0; i < _ds->nDirs(); i++) {
      m_list.AddString((*_ds)[i].name);
   }
   if (_caption) m_caption.SetWindowText(_caption);
   m_list.SetCurSel(0);
   m_list.SetFocus();
   return FALSE;
}

//------------------------------------------------------------------------------

void selectList::setCaption(CString caption)
{
   _caption = caption;
}

//------------------------------------------------------------------------------
