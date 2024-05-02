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

#ifndef MEGATUNE_H
#define MEGATUNE_H 1

#if _MSC_VER >= 1000
#  pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
#  error include 'stdafx.h' before including this file for PCH
#endif

#include <malloc.h>

#include "resource.h"
#include "stringConstants.h"
#include "dataDialog.h"
#include "Megatune_i.h"

//------------------------------------------------------------------------------

void msgOk(const char *Title, const char *Format, ...);

//------------------------------------------------------------------------------

class CMegatuneApp : public CWinApp {
   HACCEL m_haccel;
   BOOL   m_bATLInited;

   BOOL   InitATL();

public:
   CMegatuneApp();

   //{{AFX_VIRTUAL(CMegatuneApp)
   public:
      virtual BOOL InitInstance();
      virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);
      virtual int  ExitInstance();
   //}}AFX_VIRTUAL

public:
   //{{AFX_MSG(CMegatuneApp)
      afx_msg void OnAppExit();
      afx_msg void OnHelp();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif
