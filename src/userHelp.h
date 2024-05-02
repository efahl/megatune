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

#ifndef USERHELP_H
#define USERHELP_H 1

#if _MSC_VER > 1000
#  pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------

#include <vector>
#include <map>
#include "byteString.h"

//------------------------------------------------------------------------------

class userHelpData {
public:
   userHelpData(CString t, UINT id);

   void add(CString text);
   void uri(CString uri);

   CString  content();
   CString &title  ();
   CString &uri    ();
   
private:
   CString     _title;
   byteString  _content;
   CString     _uri;
   UINT        _id;
};

class userHelpList {
   typedef std::map<CString, UINT> strMap;
   typedef strMap::const_iterator  strMapIter;

public:
   userHelpList();

   void addHelp(CString d, CString t);
   void addText(CString t);
   void addUri (CString u);

   UINT id(const CString &d) const;
   userHelpData *help(int idx)  const;

private:
   std::vector<userHelpData *> _uh;
   strMap                      _uhId;
};

//------------------------------------------------------------------------------

class userHelp : public dataDialog {
public:
   userHelp(UINT id, CWnd *pParent=NULL);

   //{{AFX_DATA(userHelp)
      enum { IDD = IDD_USERHELP };
      CButton m_webHelpButton;
      CEdit   m_text;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(userHelp)
   protected:
      virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(userHelp)
      virtual BOOL   OnInitDialog();
      afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
      afx_msg void OnWebHelp();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

private:
   userHelpData *hd;
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif
