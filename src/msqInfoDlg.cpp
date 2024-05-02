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

#include "stdafx.h"
#include "megatune.h"
#include "msqInfoDlg.h"
#include "msDatabase.h"

static char *rcsId = "$Id$";

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

extern msDatabase mdb;

//------------------------------------------------------------------------------

void msqInfoDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(msqInfoDlg)
      DDX_Control(pDX, IDC_INFO, m_info);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(msqInfoDlg, CDialog)
   //{{AFX_MSG_MAP(msqInfoDlg)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

msqInfoDlg::msqInfoDlg(CWnd *pParent)
 : CDialog(msqInfoDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(msqInfoDlg)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

BOOL msqInfoDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   std::vector<msqInfo*> mi = msDatabase::infoSettings();
   int len = 0;
   int i;
   for (i = 0; i < mi.size(); i++) {
      if (strlen(mi[i]->path) > len) len = strlen(mi[i]->path);
   }

   CString txtData;
   txtData.Format("      %*s   MT Ver   MegaSquirt Signature\r\n", -len, "File Name");
   for (i = 0; i < mi.size(); i++) {
      CString s;
      s.Format("%3d   %*s   %3d.%02d   %s\r\n", i+1, -len, mi[i]->path, mi[i]->major, mi[i]->minor, mi[i]->sig);
      txtData += s;
      delete mi[i];
   }
   txtData += "\r\nNote: Version 4.00 files are portable XML.";

#if 0
   CFont x;
   x.CreateFont(10, 0, 0, 0, 400, // 400 is regular
                        false, false, false, DEFAULT_CHARSET,
                        OUT_OUTLINE_PRECIS,
                        CLIP_DEFAULT_PRECIS,
                        PROOF_QUALITY,
                        DEFAULT_PITCH | FF_DONTCARE,
                        "Courier New");
   m_info.SetFont(&x);
#else
// CFont *oldFont = m_info.GetFont();

   LOGFONT lf;
   memset(&lf, 0, sizeof(LOGFONT));
   lf.lfHeight = 15; // s_pointSize;
   strcpy(lf.lfFaceName, "Courier New"); //fontName);
   CFont *e_font = new CFont();
   e_font->CreateFontIndirect(&lf);
   lf.lfWeight = 1;
   m_info.SetFont(e_font);
#endif

// m_info.SetReadOnly(true);
   m_info.SetWindowText(txtData);
   m_info.SetFocus();
   
   return TRUE;
}

//------------------------------------------------------------------------------
