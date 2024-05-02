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
#ifndef PORTEDIT_H
#define PORTEDIT_H 1

#if _MSC_VER >= 1000
#  pragma once
#endif

//------------------------------------------------------------------------------


class portEdit : public dataDialog {
   int _pageNo;

   symbol    *psEnabled;
   symbol    *psCondition;
   symbol    *psConnector;
   symbol    *psInitValue;
   symbol    *psPortValue;
   symbol    *psOutSize;
   symbol    *psOutOffset;
   symbol    *psThreshold;
   symbol    *psHysteresis;

   CComboBox *_portName;
   int        _nPorts;
   CButton   *_enabled;
   CComboBox *_connector;
   CComboBox *_initValue;
   CComboBox *_portValue;
   CComboBox *_variable  [2];
   CComboBox *_condition [2];
   CEdit     *_threshold [2];
   CEdit     *_hysteresis[2];

   void verifySettings  ();

   void setPort         ();
   void connectorChanged();
   void enableChanged   ();
   void operatorChanged (UINT nId);
   void variableChanged (UINT nId);
   void valueChanged    ();
   void outputChanged   ();

public:
   portEdit(int pageNo, CWnd *pParent=NULL);

   //{{AFX_DATA(portEdit)
      enum { IDD = IDD_PORTEDIT };
   //}}AFX_DATA

   //{{AFX_VIRTUAL(portEdit)
   protected:
      virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(portEdit)
      virtual BOOL OnInitDialog();
      afx_msg void OnUp        ();
      afx_msg void OnDown      ();
      afx_msg BOOL OnHelpInfo  (HELPINFO* pHelpInfo);
//      DECLARE_EVENTSINK_MAP()
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif
