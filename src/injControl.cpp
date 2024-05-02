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

static char *rcsId() { return "$Id$"; }

#include "stdafx.h"
#include "megatune.h"
#include "userDefined.h"
#include "dataDialog.h"
#include "msDatabase.h"
#include "injControl.h"
#include "Dreqfuel.h"
#include <math.h>

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char    THIS_FILE[] = __FILE__;
#endif

extern msDatabase mdb;

//------------------------------------------------------------------------------

#pragma warning(disable:4355)

injControl::injControl(int pageNo, CWnd *pParent)
 : dataDialog(injControl::IDD, pParent)
 , pageNo(pageNo)
 , _fld(this)
{
   //{{AFX_DATA_INIT(injControl)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void injControl::DoDataExchange(CDataExchange *pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(injControl)
      DDX_Control(pDX, IDC_REQFUEL,        m_reqFuel);
      DDX_Control(pDX, IDC_REQFUELDOWN,    m_reqFuelDown);
      DDX_Control(pDX, IDC_NSQUIRTS,       m_nSquirts);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(injControl, dataDialog)
   //{{AFX_MSG_MAP(injControl)
      ON_BN_CLICKED(IDB_REQFUEL,            OnDreqfuel)
      ON_BN_CLICKED(IDC_UP,                 OnUp)
      ON_BN_CLICKED(IDC_DOWN,               OnDown)

      ON_EN_KILLFOCUS(IDC_REQFUEL,          dbFromFields)
      ON_EN_KILLFOCUS(IDC_REQFUELDOWN,      dbFromFields)

      ON_CBN_SELCHANGE(IDC_ALGORITHM,       dbFromFields)
      ON_CBN_SELCHANGE(IDC_ALTERNATE,       dbFromFields)
      ON_CBN_SELCHANGE(IDC_NSQUIRTS,        dbFromFields)
      ON_CBN_SELCHANGE(IDC_TWOSTROKE,       dbFromFields)
      ON_CBN_SELCHANGE(IDC_INJTYPE,         dbFromFields)
      ON_CBN_SELCHANGE(IDC_NCYLINDERS,      dbFromFields)
      ON_CBN_SELCHANGE(IDC_NINJECTORS,      dbFromFields)
      ON_CBN_SELCHANGE(IDC_ENGINETYPE,      dbFromFields)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

BOOL injControl::OnInitDialog()
{
   dataDialog::OnInitDialog("Injection Control", pageNo+1);

   _fld.add(S_algorithm,  IDC_ALGORITHM,      pageNo);
   _fld.add(S_alternate,  IDC_ALTERNATE,      pageNo);
   _fld.add(S_engineType, IDC_ENGINETYPE,     pageNo);
   _fld.add(S_injType,    IDC_INJTYPE,        pageNo);
   _fld.add(S_nCylinders, IDC_NCYLINDERS,     pageNo);
   _fld.add(S_nInjectors, IDC_NINJECTORS,     pageNo);
   _fld.add(S_reqFuel,    IDC_REQFUELDOWN,    pageNo);
   _fld.add(S_twoStroke,  IDC_TWOSTROKE,      pageNo);

   _rpmk    = mdb.lookupByPage(S_rpmk,        pageNo);
   _divider = mdb.lookupByPage(S_divider,     pageNo);

   fieldsFromDb();

   return TRUE;
}

//------------------------------------------------------------------------------

void injControl::OnDreqfuel()
{
   int savePage = mdb.setPageNo(pageNo);
      Dreqfuel rfdb(_fld.value(S_nCylinders));
      if (rfdb.DoModal() == IDOK) {
         setFld(&m_reqFuel, rfdb.getValue(), 1.0, 0.0, 1);
         dbFromFields();
      }
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------

void injControl::OnUp()
{
   fieldsFromDb();
}

//------------------------------------------------------------------------------

void injControl::OnDown()
{
   int savePage = mdb.setPageNo(pageNo);
      _fld.send();
      mdb.cDesc.send(_divider);
      mdb.cDesc.send(_rpmk   );
      mdb.burnConst(pageNo);
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------

//BEGIN_EVENTSINK_MAP(injControl, dataDialog)
//   //{{AFX_EVENTSINK_MAP(injControl)
//   //}}AFX_EVENTSINK_MAP
//END_EVENTSINK_MAP()

//------------------------------------------------------------------------------

void injControl::fieldsFromDb()
{
   int savePage = mdb.setPageNo(pageNo);
      mdb.getConst();
      _fld.setFld();

      int alternating = _fld.value(S_alternate);
      int divider     = _divider ? _divider->valueUser() : 1;
      int nSquirts    = int(0.00001 + double(_fld.value(S_nCylinders)) / divider);

      double rfqnum   = (_fld.value(S_nInjectors)) / (double(divider) * double(alternating + 1));
      double reqFuel  = _fld.value(S_reqFuel) * rfqnum + 0.0001;

      m_nSquirts.SetCurSel(nSquirts-1); // Number of squirts.

      setFld(&m_reqFuel, reqFuel, 1.0);
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------

void injControl::dbFromFields()
{
   int savePage = mdb.setPageNo(pageNo);
      _fld.getFld();

      UINT rpmk = (_fld.value(S_twoStroke) ? 6000 : 12000) / (_fld.value(S_nCylinders));
      if (_rpmk) _rpmk->storeValue(rpmk);

      int    nCylinders  = _fld.value(S_nCylinders);
      int    alternating = _fld.value(S_alternate);
      int    divider     = m_nSquirts.GetCurSel() + 1; // User-view number of squirts per cycle.
      double intCheck    = (0.0001 + double(nCylinders) / double(divider));
      BYTE   dividerdl   = intCheck;

      if (::fabs(double(dividerdl - intCheck)) > 0.1)
         MessageBox("Number of Squirts not an integer divisor of cylinder count.");
      else if (alternating && !(dividerdl < nCylinders && ::fmod(nCylinders, dividerdl*2.0) == 0.0))
         MessageBox("Cannot Alternate this Number of Squirts with this Cylinder Count.");
      else {
         if (_divider) _divider->storeValue(dividerdl);
      }

      // Alternating Injector Mode

      // Equation for determining Req_fuel_download from Req_fuel:
      //
      // REQ_FUEL_DL = reqFuel * (B * N)/NINJ
      //
      // B = 1 if simultaneous, 2 if alternating
      // N = divider_number = ncyl/number_of_squirts
      // NINJ = number of injectors


      double reqfuelsav   =  getRaw(&m_reqFuel, 0.0, 255.0, S_reqFuel)*10.0;
      double reqfueldlsav = _fld.value(S_reqFuel);
      double rfqnum       = (alternating+1) * dividerdl / double(_fld.value(S_nInjectors));
      double reqfueldl    = rfqnum * reqfuelsav;

      if (int(reqfueldl) != 10 * int(reqfueldlsav)) {
         if (reqfueldl > 65536.0)
            MessageBox("ERROR: Fuel/Squirt combination outside of Range\nIncrease Number of Squirts");
         else if (reqfueldl <= 0.0)
            MessageBox("ERROR: Injector Pulse Width Too Small\nDecrease Number of Squirts");
         else {
            setFld(&m_reqFuelDown, reqfueldl, 0.1, 0.0, 1);
            _fld[S_reqFuel]->getFld(); // Grab out the new download value.
            setFld(&m_reqFuel, _fld.value(S_reqFuel)/rfqnum, 1.0, 0.0, 1); // And update the user value.
         }
      }
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------
