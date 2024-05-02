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
#include "Dconstants.h"
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

Dconstants::Dconstants(int pageNo, CWnd *pParent)
 : dataDialog(Dconstants::IDD, pParent)
 , _pageNo   (pageNo)
 , _fld      (this)
{
   //{{AFX_DATA_INIT(Dconstants)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void Dconstants::DoDataExchange(CDataExchange *pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(Dconstants)
      DDX_Control(pDX, IDC_FIDLE_LABEL,    m_fidleLabel);
      DDX_Control(pDX, IDC_REQFUEL,        m_reqFuel);
      DDX_Control(pDX, IDC_REQFUELDOWN,    m_reqFuelDown);
      DDX_Control(pDX, IDC_NSQUIRTS,       m_nSquirts);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(Dconstants, dataDialog)
   //{{AFX_MSG_MAP(Dconstants)
      ON_BN_CLICKED(IDB_REQFUEL,            OnDreqfuel)
      ON_BN_CLICKED(IDC_UP,                 OnUp)
      ON_BN_CLICKED(IDC_DOWN,               OnDown)

      ON_EN_KILLFOCUS(IDC_REQFUEL,          dbFromFields)
      ON_EN_KILLFOCUS(IDC_INJOPEN,          dbFromFields)
      ON_EN_KILLFOCUS(IDC_BATTFAC,          dbFromFields)
      ON_EN_KILLFOCUS(IDC_INJPWM,           dbFromFields)
      ON_EN_KILLFOCUS(IDC_INJPWMT,          dbFromFields)
      ON_EN_KILLFOCUS(IDC_REQFUELDOWN,      dbFromFields)
      ON_EN_KILLFOCUS(IDC_FASTIDLE,         dbFromFields)

      ON_CBN_SELCHANGE(IDC_ALGORITHM,       dbFromFields)
      ON_CBN_SELCHANGE(IDC_ALTERNATE,       dbFromFields)
      ON_CBN_SELCHANGE(IDC_NSQUIRTS,        dbFromFields)
      ON_CBN_SELCHANGE(IDC_TWOSTROKE,       dbFromFields)
      ON_CBN_SELCHANGE(IDC_INJTYPE,         dbFromFields)
      ON_CBN_SELCHANGE(IDC_MAPTYPE,         dbFromFields)
      ON_CBN_SELCHANGE(IDC_NCYLINDERS,      dbFromFields)
      ON_CBN_SELCHANGE(IDC_NINJECTORS,      dbFromFields)
      ON_CBN_SELCHANGE(IDC_ENGINETYPE,      dbFromFields)
      ON_CBN_SELCHANGE(IDC_BAROCORRENABLE,  dbFromFields)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

BOOL Dconstants::OnInitDialog()
{
   dataDialog::OnInitDialog("Constants", _pageNo+1);

   _fld.add(S_algorithm,  IDC_ALGORITHM,      _pageNo);
   _fld.add(S_alternate,  IDC_ALTERNATE,      _pageNo);
   _fld.add(S_baroCorr,   IDC_BAROCORRENABLE, _pageNo);
   _fld.add(S_battFac,    IDC_BATTFAC,        _pageNo);
   _fld.add(S_engineType, IDC_ENGINETYPE,     _pageNo);
   _fld.add(S_fastIdleT,  IDC_FASTIDLE,       _pageNo);
   _fld.add(S_injOpen,    IDC_INJOPEN,        _pageNo);
   _fld.add(S_injPwmP,    IDC_INJPWM,         _pageNo);
   _fld.add(S_injPwmT,    IDC_INJPWMT,        _pageNo);
   _fld.add(S_injType,    IDC_INJTYPE,        _pageNo);
   _fld.add(S_mapType,    IDC_MAPTYPE,        _pageNo);
   _fld.add(S_nCylinders, IDC_NCYLINDERS,     _pageNo);
   _fld.add(S_nInjectors, IDC_NINJECTORS,     _pageNo);
   _fld.add(S_reqFuel,    IDC_REQFUELDOWN,    _pageNo);
   _fld.add(S_twoStroke,  IDC_TWOSTROKE,      _pageNo);

   _rpmk    = mdb.lookupByPage(S_rpmk,        _pageNo);
   _divider = mdb.lookupByPage(S_divider,     _pageNo);

   mdb.fixThermoLabel(m_fidleLabel);

   fieldsFromDb();

   return TRUE;
}

//------------------------------------------------------------------------------

void Dconstants::OnDreqfuel()
{
   int savePage = mdb.setPageNo(_pageNo);
      Dreqfuel rfdb(_fld.value(S_nCylinders));
      if (rfdb.DoModal() == IDOK) {
         setFld(&m_reqFuel, rfdb.getValue(), 1.0, 0.0, 1);
         dbFromFields();
      }
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------

void Dconstants::OnUp()
{
   fieldsFromDb();
}

//------------------------------------------------------------------------------

void Dconstants::OnDown()
{
   int savePage = mdb.setPageNo(_pageNo);
      _fld.send();
      mdb.cDesc.send(_divider);
      mdb.cDesc.send(_rpmk   );
      mdb.burnConst(_pageNo);
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------

//BEGIN_EVENTSINK_MAP(Dconstants, dataDialog)
//   //{{AFX_EVENTSINK_MAP(Dconstants)
//   //}}AFX_EVENTSINK_MAP
//END_EVENTSINK_MAP()

//------------------------------------------------------------------------------

void Dconstants::fieldsFromDb()
{
   int savePage = mdb.setPageNo(_pageNo);
      mdb.getConst();
      _fld.setFld();

      int alternating = int(_fld.value(S_alternate));
      int divider     = int(_divider ? _divider->valueUser() : 1);
      int nSquirts    = int(0.00001 + double(_fld.value(S_nCylinders)) / divider);

      double rfqnum   = (_fld.value(S_nInjectors)) / (double(divider) * double(alternating + 1));
      double reqFuel  = _fld.value(S_reqFuel) * rfqnum + 0.0001;

      m_nSquirts.SetCurSel(nSquirts-1); // Number of squirts.

      setFld(&m_reqFuel, reqFuel, 1.0);
   mdb.setPageNo(savePage);
}

//------------------------------------------------------------------------------

void Dconstants::dbFromFields()
{
   int savePage = mdb.setPageNo(_pageNo);
      _fld.getFld();

      //------------------------------------------------------------------------
      //-- Spew our obnoxious warnings about injector setup parameters.  -------
      double injOpen = _fld.value(S_injOpen);
      if (injOpen < 0.899 || injOpen > 1.301) {
         MessageBox(
            "WARNING!\n\n"
            "You must be an EFI expert to set the value of injector opening time\n"
            "to a value outside the range 0.9-1.3 ms.  Hit F1 for more.\n\n"
            "Unless you have a valid reason for doing so, set it to 1.2 ms right now.",
            "Injector Opening Time", MB_ICONWARNING);
      }

      double battFac = _fld.value(S_battFac);
      if (battFac < 0.099 || battFac > 0.201) {
         MessageBox(
            "WARNING!\n\n"
            "You must be an EFI expert to set the value of battery factor rate\n"
            "to a value outside the range 0.1-0.2 ms/V.  Hit F1 for more.\n\n"
            "Unless you have a valid reason for doing so, set it to 0.1 ms/V right now.",
            "Battery Factor Rate", MB_ICONWARNING);
      }

      UINT rpmk = UINT((_fld.value(S_twoStroke) ? 6000 : 12000) / (_fld.value(S_nCylinders)));
      if (_rpmk) _rpmk->storeValue(rpmk);

      int    nCylinders  = int(_fld.value(S_nCylinders));
      int    alternating = int(_fld.value(S_alternate));
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
