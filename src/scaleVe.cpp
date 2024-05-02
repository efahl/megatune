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

#include "stdafx.h"
#include "megatune.h"
#include "scaleVe.h"
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

scaleVe::scaleVe(int nId, CWnd *pParent)
 : dataDialog(scaleVe::IDD, pParent)
 , nId       (nId)
{
   //{{AFX_DATA_INIT(scaleVe)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void scaleVe::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(scaleVe)
      DDX_Control(pDX, IDC_RESET_REQFUEL, m_reset_reqFuel);
      DDX_Control(pDX, IDC_NEW_REQ_FUEL,  m_newReqFuel);
      DDX_Control(pDX, IDC_OLD_REQ_FUEL,  m_oldReqFuel);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(scaleVe, CDialog)
   //{{AFX_MSG_MAP(scaleVe)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

BOOL scaleVe::OnInitDialog() 
{
   int savePage = mdb.setPageNo(ut[nId].pageNo);
      dataDialog::OnInitDialog(ut[nId].title);
      m_reset_reqFuel.SetCheck(true);
      setFld(&m_oldReqFuel, mdb.getByName(S_reqFuel, ut[nId].pageNo), 1.0, 0.0, 1);
   mdb.setPageNo(savePage);
   return TRUE;
}

//------------------------------------------------------------------------------

void scaleVe::OnOK()
{
   tuningDialog *td = ut+nId;
   int savePage = mdb.setPageNo(td->pageNo);
      double oldReqFuel = getRaw(&m_oldReqFuel, 0.1,   25.5, "Old Required Fuel");
      double newReqFuel = getRaw(&m_newReqFuel, 0.1,   25.5, "New Required Fuel");

      double scale = oldReqFuel / newReqFuel;

      const int nRPM = td->x->nValues();
      const int nLOD = td->y->nValues();
      for (int R = 0; R < nRPM; R++) {
         for (int M = 0; M < nLOD; M++) {
            int i = td->z->index(R, M);
            td->z->storeValue(scale*td->z->valueUser(i), i);
         }
      }
      mdb.cDesc.send(td->z);

      if (m_reset_reqFuel.GetCheck()) {
         mdb.setByName(S_reqFuel, td->pageNo, newReqFuel);
      }

   mdb.setPageNo(savePage);
   CDialog::OnOK();
}

//------------------------------------------------------------------------------
