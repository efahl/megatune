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
//
#ifndef HEXEDIT_H
#define HEXEDIT_H 1

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------

struct Cell : public CEdit {
   int row;
   int col;
   int ofs;
   int idx; // Index into symbol.
   bool pick;
   Cell(int r, int c, int o, int i) : CEdit(), row(r), col(c), ofs(o), idx(i), pick(false) { }
};

//------------------------------------------------------------------------------

class hexEdit : public dataDialog {
   int H;  // Edit cell dimensions.
   int W;
   enum {
      nCol  = 16,
      nRow  = 16,
      nCell = nCol*nRow
   };

   BYTE     inputBuffer  [nCell];
   BYTE     displayBuffer[nCell];

   Cell    *hx[nCell];
   CStatic *rl[nRow];
   CStatic *cl[nCol];

   int  _pageNo;
   int  _tableSize;
   int  _tableLast;
   int  _scrollPos;
   int  _scrollBot;

   void fetch        ();
   void updateDisplay(bool force=false);
   void doScroll     ();
   void setPage      (int pageNo);

   int   _base;
   char *Fmt(int);

public:
   hexEdit(CWnd *pParent=NULL);

   virtual bool doTimer(bool connected);

   //{{AFX_DATA(hexEdit)
      enum { IDD = IDD_HEXEDIT };
      CScrollBar m_hexScroll;
      CButton    m_units;
      CStatic    m_counter;
      CStatic    m_pageNo;
      CButton    m_realtime;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(hexEdit)
      protected:
         virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(hexEdit)
      afx_msg void   OnFetch();
      virtual BOOL   OnInitDialog();
      afx_msg void   OnDestroy();
      afx_msg void   OnBinary();
      afx_msg void   OnDecimal();
      afx_msg void   OnHexadecimal();
      afx_msg void   OnBurn();
      afx_msg void   OnPageNo(NMHDR *pNMHDR, LRESULT *pResult);
      afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
      afx_msg void   OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
   //}}AFX_MSG
   afx_msg void   OnCell(UINT nId);
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif // HEXEDIT_H
