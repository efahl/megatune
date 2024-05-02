//------------------------------------------------------------------------------
//--  Copyright (c) 2005,2006 by Eric Fahlgren, All Rights Reserved.          --
//--                                                                          --
//--  This program is free software; you can redistribute it and/or           --
//--  modify it under the terms of the GNU General Public License             --
//--  as published by the Free Software Foundation; either version            --
//--  2 of the License, or (at your option) any later version.                --
//--  See http://www.gnu.org/licenses/gpl.txt                                 --
//------------------------------------------------------------------------------

#ifndef SPLASH_H
#define SPLASH_H 1
//------------------------------------------------------------------------------

bool LoadBitmap(CString fileName, HBITMAP  &hBitmap, HPALETTE &hPalette);

class CSplashWnd : public CWnd
{
   private:
      static bool    c_showSplashWnd;
      static CString c_fileName;

      UINT     m_timer;
      HBITMAP  m_hBitmap;
      HPALETTE m_hPalette;


   protected:
      CSplashWnd();

   public:
      static void EnableSplashScreen    (bool enable, CString fileName);
      static void ShowSplashScreen      (CWnd *pParentWnd=NULL);
      static bool PreTranslateAppMessage(MSG* pMsg);

      //{{AFX_VIRTUAL(CSplashWnd)
      //}}AFX_VIRTUAL

   public:
      ~CSplashWnd();
      virtual void PostNcDestroy();

   protected:
      bool               Create(CWnd *pParentWnd=NULL);
      void               HideSplashScreen();
      static CSplashWnd* c_splashWnd;

   protected:
      //{{AFX_MSG(CSplashWnd)
         afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
         afx_msg void OnPaint();
         afx_msg void OnTimer(UINT nIDEvent);
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()
};

//------------------------------------------------------------------------------
#endif
