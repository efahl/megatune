//------------------------------------------------------------------------------
//--  Copyright (c) 2004,2005,2006 by Eric Fahlgren, All Rights Reserved.     --
//--                                                                          --
//--  This program is free software; you can redistribute it and/or           --
//--  modify it under the terms of the GNU General Public License             --
//--  as published by the Free Software Foundation; either version            --
//--  2 of the License, or (at your option) any later version.                --
//--  See http://www.gnu.org/licenses/gpl.txt                                 --
//------------------------------------------------------------------------------

static char *rcsId() { return "$Id$"; }

#include "stdafx.h"
#include <Mmsystem.h>
#include "megatune.h"
#include "megatuneDlg.h"
#include "msDatabase.h"
#include "Druntime.h"

extern msDatabase mdb;

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;

#  define _CRTDBG_MAP_ALLOC
#  include <crtdbg.h>
#endif
#include <initguid.h>
#include "Megatune_i.c"
#include "Application.h"

//------------------------------------------------------------------------------

void msgOk(const char *Title, const char *Format, ...)
{
   va_list Args;

   va_start(Args, Format);
      char Output[1024];
      vsprintf(Output, Format, Args);
      MessageBox(NULL, Output, Title, MB_OK);
   va_end(Args);
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CMegatuneApp, CWinApp)
   //{{AFX_MSG_MAP(CMegatuneApp)
      ON_COMMAND(ID_APP_EXIT, OnAppExit)
      ON_COMMAND(ID_HELP,     OnHelp)
   //}}AFX_MSG_MAP
// ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

void CMegatuneApp::OnHelp()
{
   CWinApp::OnHelp();
}

//------------------------------------------------------------------------------

CMegatuneApp theApp;

//------------------------------------------------------------------------------

CMegatuneApp::CMegatuneApp() { }

//------------------------------------------------------------------------------
// The three system globals.

CString installationDir("");
CString megasquirtDir  ("");
CString commonDir      ("");
CString workingDir     ("");

CString startupFile    ("");
CString splashFile     ("");

BOOL CMegatuneApp::InitInstance()
{
   if (!InitATL()) return FALSE;

   CCommandLineInfo cmdInfo;
   ParseCommandLine(cmdInfo);

#if 0
   // We want to init and start MegaTune UI even if started from a Com client
   // Well, a huge amount of initialization is done in the megatuneDlg object, so if we return from here, all COM stuff will crash.
   if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated) {
      return TRUE;
   }
#endif

   // Always update the registry with Megatune Com server settings
   _Module.UpdateRegistryFromResource(IDR_MEGATUNE1, TRUE);
   _Module.RegisterServer(TRUE);

#ifdef _DEBUG
//?int iFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
//?iFlags |= _CRTDBG_ALLOC_MEM_DF;
//?iFlags |= _CRTDBG_CHECK_ALWAYS_DF;
 //iFlags |= _CRTDBG_DELAY_FREE_MEM_DF;
//?iFlags |= _CRTDBG_CHECK_CRT_DF;
//?iFlags |= _CRTDBG_LEAK_CHECK_DF;
//?_CrtSetDbgFlag(iFlags);
#endif

//#ifdef _AFXDLL
//   Enable3dControls();          // Call this when using MFC in a shared DLL
//#else
//   Enable3dControlsStatic();    // Call this when linking to MFC statically
//#endif

#ifdef _DEBUG
//?_CrtMemState state;
//?_CrtMemCheckpoint(&state);
#endif

   WNDCLASS wc;
   GetClassInfo(NULL, _T("msctls_progress32"), &wc);
   wc.hInstance     = m_hInstance;
   wc.lpszClassName = _T("hysteresisBar");
   wc.cbWndExtra    = sizeof(hysteresisBar)-sizeof(CWnd);
   wc.cbClsExtra    = 0;
// wc.hCursor       = LoadStandardCursor(IDC_CROSS);
   SetLastError(0);
   RegisterClass(&wc);

   //---------------------------------------------------------------------------
   //--  Grab the global values.

   {
      installationDir = m_pszHelpFilePath;
      int i = installationDir.GetLength();
      do {
         installationDir.Delete(i--, 1);
      } while (installationDir[i] != '\\' && installationDir[i] != '/' && i > 0);
      if (installationDir.GetLength() > 3) {
         installationDir.Delete(i, 1);
      }
      installationDir.Replace("\\", "/");

      megasquirtDir = installationDir.Left(installationDir.ReverseFind('/'));
      commonDir     = megasquirtDir + "/mtCommon";

      if (m_lpCmdLine[0]) {
         startupFile = m_lpCmdLine;
         startupFile.Remove('"');
      }
      startupFile.Replace("\\", "/");

//    splashFile = installationDir+"/mtSplash%02d.bmp";
      splashFile = installationDir+"/mtSplash.bmp";
   }

   //---------------------------------------------------------------------------

   m_haccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MEGATUNE));

   timeBeginPeriod(1);
      CMegatuneDlg megatune;
      m_pMainWnd = &megatune;
      megatune.DoModal();
   timeEndPeriod(1);

#ifdef _DEBUG
//?int stat = _CrtCheckMemory();
//     stat = _CrtMemDumpMemoryLeaks();
//?_CrtMemDumpStatistics(&state);
#endif

   // Since the dialog has been closed, return FALSE so that we exit the
   //  application, rather than start the application's message pump.
   return FALSE;
}

//------------------------------------------------------------------------------

void CMegatuneApp::OnAppExit()
{
#ifdef _DEBUG
//?int stat = _CrtCheckMemory();
//?    stat = _CrtDumpMemoryLeaks();
#endif
}

//------------------------------------------------------------------------------

BOOL CMegatuneApp::ProcessMessageFilter(int code, LPMSG lpMsg)
{
   if (m_haccel) {
      if (::TranslateAccelerator(m_pMainWnd->m_hWnd, m_haccel, lpMsg)) return(TRUE);
   }
   return CWinApp::ProcessMessageFilter(code, lpMsg);
}

//------------------------------------------------------------------------------

CMegatuneModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
   OBJECT_ENTRY(CLSID_Application, CApplication)
END_OBJECT_MAP()

LONG CMegatuneModule::Unlock()
{
   AfxOleUnlockApp();
   return 0;
}

LONG CMegatuneModule::Lock()
{
   AfxOleLockApp();
   return 1;
}

LPCTSTR CMegatuneModule::FindOneOf(LPCTSTR p1, LPCTSTR p2)
{
   while (*p1 != NULL) {
      LPCTSTR p = p2;
      while (*p != NULL) {
         if (*p1 == *p) return CharNext(p1);
         p = CharNext(p);
      }
      p1++;
   }
   return NULL;
}


int CMegatuneApp::ExitInstance()
{
   if (m_bATLInited) {
      _Module.RevokeClassObjects();
      _Module.Term();
      CoUninitialize();
   }

   return CWinApp::ExitInstance();
}

BOOL CMegatuneApp::InitATL()
{
   m_bATLInited = TRUE;

#if _WIN32_WINNT >= 0x0400
   HRESULT hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
#else
   HRESULT hRes = CoInitialize(NULL);
#endif

   if (FAILED(hRes)) {
      m_bATLInited = FALSE;
      return FALSE;
   }

   _Module.Init(ObjectMap, AfxGetInstanceHandle());
   _Module.dwThreadID = GetCurrentThreadId();

   LPTSTR lpCmdLine = GetCommandLine(); //this line necessary for _ATL_MIN_CRT
   TCHAR szTokens[] = "-/";

   BOOL bRun = TRUE;
   LPCTSTR lpszToken = _Module.FindOneOf(lpCmdLine, szTokens);
   while (lpszToken != NULL) {
      if (lstrcmpi(lpszToken, "UnregServer") == 0) {
         _Module.UpdateRegistryFromResource(IDR_MEGATUNE1, FALSE);
         _Module.UnregisterServer(TRUE); //TRUE means typelib is unreg'd
         bRun = FALSE;
         break;
      }

      if (lstrcmpi(lpszToken, "RegServer") == 0) {
         _Module.UpdateRegistryFromResource(IDR_MEGATUNE1, TRUE);
         _Module.RegisterServer(TRUE);
         bRun = FALSE;
         break;
      }

      lpszToken = _Module.FindOneOf(lpszToken, szTokens);
   }

   if (!bRun) {
      m_bATLInited = FALSE;
      _Module.Term();
      CoUninitialize();
      return FALSE;
   }

   hRes = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE);
   if (FAILED(hRes)) {
      m_bATLInited = FALSE;
      CoUninitialize();
      return FALSE;
   }

   return TRUE;
}

//#include "Application.h"
