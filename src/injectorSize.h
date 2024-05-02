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

#ifndef INJECTORSIZE_H
#define INJECTORSIZE_H 1
//------------------------------------------------------------------------------

#if _MSC_VER > 1000
#  pragma once
#endif

//------------------------------------------------------------------------------

class injectorSize : public dataDialog
{
public:
   injectorSize(CWnd *pParent=NULL);

   //{{AFX_DATA(injectorSize)
      enum { IDD = IDD_INJ_SIZE };
      int    m_nInjectors;
      double m_ocTime;
      int    m_eventsPerCycle;
      double m_desiredDutycycle;
      double m_ratedFuelPressure;
      double m_actualFuelPressure;
      double m_fuelSpecificGravity;
      double m_estimatedHP;
      double m_estimatedBSFC;
      int    m_maxRPM;
      double m_HPatMaxRPM;
   //}}AFX_DATA

   //{{AFX_VIRTUAL(injectorSize)
   protected:
      virtual void OnOK();
      virtual void DoDataExchange(CDataExchange *pDX);
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(injectorSize)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

//------------------------------------------------------------------------------
#endif
