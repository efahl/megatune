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

//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#ifndef DENRICHMENTS_H
#define DENRICHMENTS_H 1

#if _MSC_VER >= 1000
#  pragma once
#endif

//*****************************************
//******** PC Configurator V1.00 **********
//*** (C) - 2001 B.Bowling/A. Grippo ******
//** All derivatives from this software ***
//**  are required to keep this header ****
//*****************************************

//------------------------------------------------------------------------------

class Denrichments:public dataDialog {
private:
   int  pageNo;

   void fieldsFromDb();
   void dbFromFields();

   stdEditFieldList _fld;

 public:
   Denrichments(int pageNo, CWnd *pParent=NULL);

   //{{AFX_DATA(Denrichments)
      enum { IDD = IDD_ENRICHMENTS };
      CStatic   m_wwu9L;
      CStatic   m_wwu8L;
      CStatic   m_wwu7L;
      CStatic   m_wwu6L;
      CStatic   m_wwu5L;
      CStatic   m_wwu4L;
      CStatic   m_wwu3L;
      CStatic   m_wwu2L;
      CStatic   m_wwu1L;
      CStatic   m_wwu0L;
      CStatic   m_coolantLabel;
      CStatic   m_pw2Label;
      CStatic   m_pw1Label;
      CEdit     m_wueBin9;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(Denrichments)
   protected:
      virtual void   DoDataExchange(CDataExchange * pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(Denrichments)
      afx_msg void   OnUp();
      afx_msg void   OnDown();
      virtual BOOL   OnInitDialog();
//      DECLARE_EVENTSINK_MAP()
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif
