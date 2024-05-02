//------------------------------------------------------------------------------
//--  Copyright (c) 2005,2006 by Eric Fahlgren, All Rights Reserved.          --
//--                                                                          --
//--  This program is free software; you can redistribute it and/or           --
//--  modify it under the terms of the GNU General Public License             --
//--  as published by the Free Software Foundation; either version            --
//--  2 of the License, or (at your option) any later version.                --
//--  See http://www.gnu.org/licenses/gpl.txt                                 --
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"

#include <sys/stat.h> // For _stat and struct

#include "Splash.h"

//------------------------------------------------------------------------------

bool        CSplashWnd::c_showSplashWnd;
CSplashWnd* CSplashWnd::c_splashWnd;
CString     CSplashWnd::c_fileName;

//------------------------------------------------------------------------------

CSplashWnd::CSplashWnd() { }

CSplashWnd::~CSplashWnd()
{
   ASSERT(c_splashWnd == this);
   c_splashWnd = NULL;
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
   //{{AFX_MSG_MAP(CSplashWnd)
      ON_WM_CREATE()
      ON_WM_PAINT()
      ON_WM_TIMER()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

void CSplashWnd::EnableSplashScreen(bool enable, CString fileName)
{
   c_showSplashWnd = enable;
   c_fileName      = fileName;
}

//------------------------------------------------------------------------------

void CSplashWnd::ShowSplashScreen(CWnd *pParentWnd)
{
   if (!c_showSplashWnd || c_splashWnd != NULL) return;

   c_splashWnd = new CSplashWnd;
   if (!c_splashWnd->Create(pParentWnd))
      delete c_splashWnd;
   else
      c_splashWnd->UpdateWindow();
}

//------------------------------------------------------------------------------

bool CSplashWnd::PreTranslateAppMessage(MSG* pMsg)
{
   if (c_splashWnd == NULL) return false;

   if (pMsg->message == WM_KEYDOWN       ||
       pMsg->message == WM_SYSKEYDOWN    ||
       pMsg->message == WM_LBUTTONDOWN   ||
       pMsg->message == WM_RBUTTONDOWN   ||
       pMsg->message == WM_MBUTTONDOWN   ||
       pMsg->message == WM_NCLBUTTONDOWN ||
       pMsg->message == WM_NCRBUTTONDOWN ||
       pMsg->message == WM_NCMBUTTONDOWN)
   {
      c_splashWnd->HideSplashScreen();
      return true;
   }

   return false;
}

//------------------------------------------------------------------------------

bool LoadBitmap(CString fileName, HBITMAP &hBitmap, HPALETTE &hPalette)
{
   CString baseFileName = fileName;
   if (baseFileName.Find('%') >= 0) {
      ::srand(::time(NULL));
      for (int i = 0; i < 100; i++) {
         fileName.Format(baseFileName, ::rand() % 100);
         struct _stat s;
         if (_stat(fileName, &s) == 0) break;
      }
   }

   hBitmap = HBITMAP(LoadImage(NULL, fileName, IMAGE_BITMAP, 0, 0,
                               LR_CREATEDIBSECTION
                             | LR_DEFAULTSIZE
                             | LR_SHARED
//                           | LR_LOADTRANSPARENT
                             | LR_LOADFROMFILE));

   hPalette = NULL;
   if (hBitmap == NULL) {
      return false;
   }

   bool createPalette = false;
   if (createPalette) {
      // If the DIBSection is 256 color or less, it has a color table
      HDC            hMemDC;
      HBITMAP        hOldBitmap;
      RGBQUAD        rgb[256];
      LPLOGPALETTE   pLogPal;
      WORD           i;

      // Create a memory DC and select the DIBSection into it
      hMemDC = CreateCompatibleDC(NULL);
      hOldBitmap = HBITMAP(SelectObject(hMemDC, hBitmap));
      // Get the DIBSection's color table
      GetDIBColorTable(hMemDC, 0, 256, rgb);
      // Create a palette from the color tabl
      pLogPal = static_cast<LOGPALETTE *>(malloc(sizeof(LOGPALETTE) + (256 * sizeof(PALETTEENTRY))));
      pLogPal->palVersion = 0x300;
      pLogPal->palNumEntries = 256;
      for (i = 0; i < 256; i++) {
         pLogPal->palPalEntry[i].peRed   = rgb[i].rgbRed;
         pLogPal->palPalEntry[i].peGreen = rgb[i].rgbGreen;
         pLogPal->palPalEntry[i].peBlue  = rgb[i].rgbBlue;
         pLogPal->palPalEntry[i].peFlags = 0;
      }
      hPalette = CreatePalette(pLogPal);

      free(pLogPal);
      SelectObject(hMemDC, hOldBitmap);
      DeleteDC(hMemDC);
   }

   return true;
}

//------------------------------------------------------------------------------

bool CSplashWnd::Create(CWnd* pParentWnd)
{
   c_showSplashWnd = LoadBitmap(c_fileName, m_hBitmap, m_hPalette);
   if (!c_showSplashWnd) return false;
   BITMAP bm;
   GetObject(m_hBitmap, sizeof(BITMAP), &bm);

   return
      CreateEx(0,
         AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
         NULL,
         WS_POPUP | WS_VISIBLE,
         0, 0, bm.bmWidth, bm.bmHeight,
         pParentWnd->GetSafeHwnd(), NULL) != 0;
}

//------------------------------------------------------------------------------

void CSplashWnd::HideSplashScreen()
{
   DestroyWindow();
   AfxGetMainWnd()->UpdateWindow();
}

//------------------------------------------------------------------------------

void CSplashWnd::PostNcDestroy()
{
   delete this;
}

//------------------------------------------------------------------------------

int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CWnd::OnCreate(lpCreateStruct) == -1) return -1;
   CenterWindow();
   m_timer = SetTimer(2, 750, NULL);

   return 0;
}

//------------------------------------------------------------------------------

void CSplashWnd::OnPaint()
{
   CPaintDC dc(this);

   CDC dcImage;
   if (!dcImage.CreateCompatibleDC(&dc)) return;

   BITMAP bm;
   GetObject(m_hBitmap, sizeof(BITMAP), &bm);
   HBITMAP hOldBitmap = HBITMAP(SelectObject(dcImage, m_hBitmap));

   HPALETTE hOldPalette;
   if (m_hPalette) {
      hOldPalette = SelectPalette(dc, m_hPalette, false);
      RealizePalette(dc);
   }

   BitBlt(dc, 0, 0, bm.bmWidth, bm.bmHeight, dcImage, 0, 0, SRCCOPY);

   if (m_hPalette) {
      SelectPalette(dc, hOldPalette, false);
      DeleteObject(m_hPalette);
   }

   SelectObject(dcImage, hOldBitmap);
   DeleteObject(m_hBitmap);
}

//------------------------------------------------------------------------------

void CSplashWnd::OnTimer(UINT nIDEvent)
{
   HideSplashScreen();
   KillTimer(m_timer);
}

//------------------------------------------------------------------------------
