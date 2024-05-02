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

#if !defined(AFX_STDAFX_H__EEA60A70_4090_11D5_8540_000000000000__INCLUDED_)
#define AFX_STDAFX_H__EEA60A70_4090_11D5_8540_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#  pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN          // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>                         // MFC core and standard components
#include <afxext.h>                                           // MFC extensions
//#include <afxdisp.h>                              // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                  // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

	#define _ATL_APARTMENT_THREADED
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
class CMegatuneModule : public CComModule
{
public:
	LONG Unlock();
	LONG Lock();
	LPCTSTR FindOneOf(LPCTSTR p1, LPCTSTR p2);
	DWORD dwThreadID;
};
extern CMegatuneModule _Module;
#include <atlcom.h>

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif // !defined(AFX_STDAFX_H__EEA60A70_4090_11D5_8540_000000000000__INCLUDED_)
