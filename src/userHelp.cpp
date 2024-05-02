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
#include "userHelp.h"

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------

extern userHelpList uhl;

//------------------------------------------------------------------------------

userHelpData::userHelpData(CString t, UINT id)
 : _title  (t)
 , _content("")
 , _id     (id)
{
}

void userHelpData::add(CString text)
{
   if (text.GetLength() == 0 || text[text.GetLength()-1] != '>')
      _content.append(LPCTSTR(text+" "));
   else
      _content.append(LPCTSTR(text));
   _content.xlate();
   _content.replace("<br>", "\r\n");
}

void userHelpData::uri(CString uri)
{
   _uri = uri;
}

CString userHelpData::content() { return CString(_content.str()); }
CString &userHelpData::title () { return _title;                  }
CString &userHelpData::uri   () { return _uri;                    }

//------------------------------------------------------------------------------

userHelpList::userHelpList()
{
}

void userHelpList::addHelp(CString d, CString t)
{
   static int nId = 0;
   UINT id = ID_USER_HELP_00+nId;
   nId++;
   _uh.push_back(new userHelpData(t, id));
   _uhId[d] = id;
}

void userHelpList::addText(CString t)
{
   _uh.back()->add(t);
}

void userHelpList::addUri(CString u)
{
   _uh.back()->uri(u);
}

userHelpData *userHelpList::help(int idx) const { return _uh[idx]; }

UINT userHelpList::id(const CString &d) const
{
   strMapIter i = _uhId.find(d);
   return i != _uhId.end() ? (*i).second : 0;
}

//------------------------------------------------------------------------------

userHelp::userHelp(UINT id, CWnd *pParent)
 : dataDialog(userHelp::IDD, pParent)
{
   hd = uhl.help(id);
   //{{AFX_DATA_INIT(userHelp)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void userHelp::DoDataExchange(CDataExchange *pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(userHelp)
      DDX_Control(pDX, IDC_WEBHELP, m_webHelpButton);
      DDX_Control(pDX, IDC_TEXT, m_text);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(userHelp, dataDialog)
   //{{AFX_MSG_MAP(userHelp)
      ON_WM_CTLCOLOR()
      ON_BN_CLICKED(IDC_WEBHELP, OnWebHelp)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

BOOL userHelp::OnInitDialog() 
{
   CString ss = hd->title();
   dataDialog::OnInitDialog(hd->title());

#if 0
   CFont x;
   x.CreateFont(16, 0, 0, 0, 400, // 400 is regular
                        false, false, false, DEFAULT_CHARSET,
                        OUT_OUTLINE_PRECIS,
                        CLIP_DEFAULT_PRECIS,
                        PROOF_QUALITY,
                        DEFAULT_PITCH | FF_DONTCARE,
                        "Lucida Console");
   m_text.SetFont(&x);
#endif

   m_text.SetWindowText(hd->content());

   CRect r;
   m_text.GetRect(&r);
   r.DeflateRect(5, 5);
   m_text.SetRect(&r);

   m_webHelpButton.EnableWindow(!hd->uri().IsEmpty());

   return TRUE;
}

//------------------------------------------------------------------------------

HBRUSH userHelp::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor) 
{
   HBRUSH hbr = dataDialog::OnCtlColor(pDC, pWnd, nCtlColor);

   if (pWnd->GetDlgCtrlID() == IDC_TEXT) {
      pDC->SetTextColor(RGB(0,0,0));
      pDC->SetBkColor(RGB(255,255,255));

      static CBrush brush;
      brush.DeleteObject();
      brush.CreateSolidBrush(RGB(255,255,255));
      hbr = brush;
   }

   return hbr;
}

//------------------------------------------------------------------------------

void userHelp::OnWebHelp() 
{
   // Spawn a browser with specified URI.
   ShellExecute(NULL, "open", hd->uri(), NULL, NULL, SW_SHOWNORMAL); 
}

//------------------------------------------------------------------------------
