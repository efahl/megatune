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
#include "injectorSize.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------

injectorSize::injectorSize(CWnd *pParent)
 : dataDialog(injectorSize::IDD, pParent)
 , m_nInjectors         ( 8 )
 , m_ocTime             ( 0.7 )
 , m_eventsPerCycle     ( 1 )
 , m_desiredDutycycle   ( 95 )
 , m_actualFuelPressure ( 43.5  )
 , m_ratedFuelPressure  ( 43.5  )
 , m_fuelSpecificGravity(  0.725)
 , m_estimatedHP        ( 500.0 )
 , m_estimatedBSFC      ( 0.48 )
 , m_maxRPM             ( 8000 )
 , m_HPatMaxRPM         ( 450.0 )
{
   //{{AFX_DATA_INIT(injectorSize)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void injectorSize::DoDataExchange(CDataExchange* pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(injectorSize)
      DDX_Text        (pDX, IDC_N_INJECTORS,          m_nInjectors);
      DDX_Text        (pDX, IDC_OC_TIME,              m_ocTime);
      DDX_Text        (pDX, IDC_EVENTS_PER_CYCLE,     m_eventsPerCycle);
      DDX_Text        (pDX, IDC_DESIRED_DC,           m_desiredDutycycle);
      DDV_MinMaxInt   (pDX, m_nInjectors,             1,    32);
      DDV_MinMaxDouble(pDX, m_ocTime,                 0.01, 5.0);
      DDV_MinMaxInt   (pDX, m_eventsPerCycle,         1,    32);
      DDV_MinMaxDouble(pDX, m_desiredDutycycle,       0.0, 100.0);

      DDX_Text        (pDX, IDC_RATED_FUEL_PRESSURE,  m_ratedFuelPressure);
      DDX_Text        (pDX, IDC_ACTUAL_FUEL_PRESSURE, m_actualFuelPressure);
      DDX_Text        (pDX, IDC_FUEL_SG,              m_fuelSpecificGravity);
      DDV_MinMaxDouble(pDX, m_ratedFuelPressure,      0.0, 2000.0);
      DDV_MinMaxDouble(pDX, m_actualFuelPressure,     0.0, 2000.0);
      DDV_MinMaxDouble(pDX, m_fuelSpecificGravity,    0.2,    2.0);

      DDX_Text        (pDX, IDC_ESTIMATED_HP,         m_estimatedHP);
      DDX_Text        (pDX, IDC_BSFC,                 m_estimatedBSFC);
      DDX_Text        (pDX, IDC_MAX_RPM,              m_maxRPM);
      DDX_Text        (pDX, IDC_MAX_RPM_HP,           m_HPatMaxRPM);
      DDV_MinMaxDouble(pDX, m_estimatedHP,            1.0, 8000.0);
      DDV_MinMaxDouble(pDX, m_estimatedBSFC,          0.0, 10.0);
      DDV_MinMaxInt   (pDX, m_maxRPM,                 100, 25000);
      DDV_MinMaxDouble(pDX, m_HPatMaxRPM,             1.0, 8000.0);
      //#define IDC_RESULT                      3325
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(injectorSize, dataDialog)
   //{{AFX_MSG_MAP(injectorSize)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

void injectorSize::OnOK()
{
   dataDialog::OnOK();
}

//------------------------------------------------------------------------------
/*
Inputs:
   Number of injectors
   Injector Open/Close Time
   Injection events per injector per engine rev
   Desired max DC (real max dc is an output)

   Rated fuel pressure
   Actual fuel pressure
   Specific Gravity

   Estimated peak HP
   Estimated BSFC at peak HP
   Maximum RPM
   Estimated HP at max RPM

Outputs:
   Required total flow
   Required per injector flow

   HP or window limited
   Maximum duty cycle @ RPM

   Rated injector flow cc/min
   Rated injector flow lb/hr 

*/
