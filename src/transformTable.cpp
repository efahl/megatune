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
#include "megatune.h"
#include "transformTable.h"
#include "msDatabase.h"
#include "tuning3D.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------

extern msDatabase   mdb;
extern tuningDialog ut[tuningDialog::nUT];

//------------------------------------------------------------------------------

transformTable::transformTable(int nId, CWnd *pParent)
 : dataDialog(transformTable::IDD, pParent)
 , nId(nId)
{
   //{{AFX_DATA_INIT(transformTable)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void transformTable::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(transformTable)
      DDX_Control(pDX, IDC_SCALE_TABLE, m_scale);
      DDX_Control(pDX, IDC_VALUE,       m_value);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(transformTable, CDialog)
   //{{AFX_MSG_MAP(transformTable)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

BOOL transformTable::OnInitDialog() 
{
   int savePage = mdb.setPageNo(ut[nId].pageNo);
      dataDialog::OnInitDialog(ut[nId].title);
      m_scale.SetCheck(true);
      m_value.SetWindowText("1.0");
   mdb.setPageNo(savePage);
   return TRUE;
}

//------------------------------------------------------------------------------

void transformTable::OnOK()
{
   tuningDialog *td = ut+nId;
   int savePage = mdb.setPageNo(td->pageNo);
      bool   doScale = m_scale.GetCheck() != 0;
      double value   = getRaw(&m_value, symbol::noRange, symbol::noRange, "Scale/Shift Value");
      double scale   = doScale ? value : 1.0;
      double shift   = doScale ? 0.0 : value;

      const int nRPM = td->x->nValues();
      const int nLOD = td->y->nValues();
      for (int R = 0; R < nRPM; R++) {
         for (int M = 0; M < nLOD; M++) {
            int i = td->z->index(R, M);
            td->z->storeValue(scale*td->z->valueUser(i)+shift, i);
         }
      }
      mdb.cDesc.send(td->z);

   mdb.setPageNo(savePage);
   CDialog::OnOK();
}

//------------------------------------------------------------------------------
