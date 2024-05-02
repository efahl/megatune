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
//*****************************************
//******** PC Configurator V1.00 **********
//*** (C) - 2001 B.Bowling/A. Grippo ******
//** All derivatives from this software ***
//**  are required to keep this header ****
//*****************************************

static char *rcsId() { return "$Id$"; }

#include "stdafx.h"
#include "megatune.h"
#include "msDatabase.h"
#include "Dreqfuel.h"

#ifdef _DEBUG
# define new DEBUG_NEW
# undef THIS_FILE
   static char    THIS_FILE[] = __FILE__;
#endif

extern msDatabase mdb;

//------------------------------------------------------------------------------

double Dreqfuel::cid            = 350.0;
double Dreqfuel::injectorFlow   =  42.0;
double Dreqfuel::afr            =  14.7;

Dreqfuel::Dreqfuel(int nCylinders, CWnd *pParent)
 : dataDialog (Dreqfuel::IDD, pParent)
 , _nCylinders(nCylinders)
{
   //{{AFX_DATA_INIT(Dreqfuel)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void Dreqfuel::DoDataExchange(CDataExchange * pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(Dreqfuel)
      DDX_Control(pDX, IDC_CID,            m_cid         );
      DDX_Control(pDX, IDC_NCYL,           m_nCylinders  );
      DDX_Control(pDX, IDC_FLOW,           m_injectorFlow);
      DDX_Control(pDX, IDC_AFR,            m_afr         );
      DDX_Control(pDX, IDC_DISP_UNITS_CID, m_dispCID     );
      DDX_Control(pDX, IDC_INJ_UNITS_LBH,  m_flowLBH     );
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(Dreqfuel, dataDialog)
   //{{AFX_MSG_MAP(Dreqfuel)
      ON_WM_DESTROY()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

void Dreqfuel::OnDestroy()
{
   dataDialog::OnDestroy();

   _reqFuel    = 0.0;

   double dispUnits = m_dispCID.GetCheck() ? 1.0 : 16.38706;
   double flowUnits = m_flowLBH.GetCheck() ? 1.0 : 10.5;

   cid = getRaw(&m_cid, 1,  25000, "Engine Displacement");
   cid = int(cid / dispUnits);
   if (cid <= 0 || cid > 1000) return;

   injectorFlow = getRaw(&m_injectorFlow, 0, 2000, "Injector Flow");
   injectorFlow = injectorFlow / flowUnits;
   if (injectorFlow <= 0.0 || injectorFlow > 200.0) return;

   afr = getRaw(&m_afr, 0.0, 25.0, "Air:Fuel Ratio");
   if (afr <= 0.0 || afr > 25.0) return;

   double dtmp1 = 36.0E6 * cid * 4.27793e-05;
   double dtmp2 = double(_nCylinders) * afr * injectorFlow;
   _reqFuel = dtmp1 / dtmp2 / 10.0;
}

//------------------------------------------------------------------------------

double Dreqfuel::getValue()
{
   return _reqFuel;
}

//------------------------------------------------------------------------------

BOOL Dreqfuel::OnInitDialog()
{
   dataDialog::OnInitDialog("Required Fuel Calculator");

// setFld(&m_cid,          cid);
   setFld(&m_nCylinders,   _nCylinders);
// setFld(&m_injectorFlow, injectorFlow);
// setFld(&m_afr,          afr);

   m_dispCID.SetCheck(true);
   m_flowLBH.SetCheck(true);

   return TRUE;
}

//------------------------------------------------------------------------------
