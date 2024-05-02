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
#include "megatuneDlg.h"
#include "msDatabase.h"
#include "userDefined.h"
#include "Denrichments.h"

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

extern msDatabase mdb;

//------------------------------------------------------------------------------

#pragma warning(disable:4355)

Denrichments::Denrichments(int pageNo, CWnd *pParent)
 : dataDialog(Denrichments::IDD, pParent)
 , pageNo    (pageNo)
 , _fld      (this)
{
   //{{AFX_DATA_INIT(Denrichments)
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void Denrichments::DoDataExchange(CDataExchange *pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(Denrichments)
      DDX_Control(pDX, IDC_WUEBIN0_L,     m_wwu0L);
      DDX_Control(pDX, IDC_WUEBIN1_L,     m_wwu1L);
      DDX_Control(pDX, IDC_WUEBIN2_L,     m_wwu2L);
      DDX_Control(pDX, IDC_WUEBIN3_L,     m_wwu3L);
      DDX_Control(pDX, IDC_WUEBIN4_L,     m_wwu4L);
      DDX_Control(pDX, IDC_WUEBIN5_L,     m_wwu5L);
      DDX_Control(pDX, IDC_WUEBIN6_L,     m_wwu6L);
      DDX_Control(pDX, IDC_WUEBIN7_L,     m_wwu7L);
      DDX_Control(pDX, IDC_WUEBIN8_L,     m_wwu8L);
      DDX_Control(pDX, IDC_WUEBIN9_L,     m_wwu9L);
      DDX_Control(pDX, IDC_COOLANT_LABEL, m_coolantLabel);
      DDX_Control(pDX, IDC_PW1_LABEL,     m_pw1Label);
      DDX_Control(pDX, IDC_PW2_LABEL,     m_pw2Label);
      DDX_Control(pDX, IDC_WUEBIN9,       m_wueBin9);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(Denrichments, CDialog)
   //{{AFX_MSG_MAP(Denrichments)
      ON_BN_CLICKED(IDC_UP,   OnUp)
      ON_BN_CLICKED(IDC_DOWN, OnDown)

      ON_EN_KILLFOCUS(IDC_CWU,         dbFromFields)
      ON_EN_KILLFOCUS(IDC_CWH ,        dbFromFields)
      ON_EN_KILLFOCUS(IDC_AWEV,        dbFromFields)
      ON_EN_KILLFOCUS(IDC_AWC ,        dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN0,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN1,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN2,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN3,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN4,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN5,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN6,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN7,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN8,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_WUEBIN9,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_TPSTHRESH,   dbFromFields)
      ON_EN_KILLFOCUS(IDC_AETIME,      dbFromFields)
      ON_EN_KILLFOCUS(IDC_TAEBIN0,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_TAEBIN1,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_TAEBIN2,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_TAEBIN3,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_TPSDQ,       dbFromFields)
      ON_EN_KILLFOCUS(IDC_EGOTEMP,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_EGOCOUNTCMP, dbFromFields)
      ON_EN_KILLFOCUS(IDC_EGODELTA,    dbFromFields)
      ON_EN_KILLFOCUS(IDC_EGOLIMIT,    dbFromFields)
      ON_EN_KILLFOCUS(IDC_TPSCOLDA,    dbFromFields)
      ON_EN_KILLFOCUS(IDC_TPSCOLDM,    dbFromFields)
      ON_EN_KILLFOCUS(IDC_PRIMEP,      dbFromFields)
      ON_EN_KILLFOCUS(IDC_RPMOXLIMIT,  dbFromFields)
      ON_EN_KILLFOCUS(IDC_EGOSWITCHV,  dbFromFields)
      ON_CBN_SELCHANGE(IDC_EGOTYPE,    dbFromFields)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------
void logSize(CWnd *w, char *title);

BOOL Denrichments::OnInitDialog()
{
   dataDialog::OnInitDialog("Enrichments", pageNo+1);

   logSize(this, "enrich");

   _fld.add(S_egoType,    IDC_EGOTYPE,        pageNo);
   _fld.add(S_primePulse, IDC_PRIMEP,         pageNo);
   _fld.add(S_crankCold,  IDC_CWU,            pageNo);
   _fld.add(S_crankHot,   IDC_CWH,            pageNo);
   _fld.add(S_asePct,     IDC_AWEV,           pageNo);
   _fld.add(S_aseCount,   IDC_AWC,            pageNo);
   _fld.add(S_tpsThresh,  IDC_TPSTHRESH,      pageNo);
   _fld.add(S_taeTime,    IDC_AETIME,         pageNo);
   _fld.add(S_tdePct,     IDC_TPSDQ,          pageNo);
   _fld.add(S_taeColdA,   IDC_TPSCOLDA,       pageNo);
   _fld.add(S_taeColdM,   IDC_TPSCOLDM,       pageNo);
   _fld.add(S_egoRPM,     IDC_RPMOXLIMIT,     pageNo);
   _fld.add(S_egoTemp,    IDC_EGOTEMP,        pageNo);
   _fld.add(S_egoCount,   IDC_EGOCOUNTCMP,    pageNo);
   _fld.add(S_egoDelta,   IDC_EGODELTA,       pageNo);
   _fld.add(S_egoLimit,   IDC_EGOLIMIT,       pageNo);
   _fld.add(S_egoSwitch,  IDC_EGOSWITCHV,     pageNo);

   for (int iWE = 0; iWE < 10; iWE++) _fld.add(S_wueBins, IDC_WUEBIN0+iWE, pageNo, iWE);
   for (int iAE = 0; iAE <  4; iAE++) _fld.add(S_taeBins, IDC_TAEBIN0+iAE, pageNo, iAE);

   int savePage = mdb.setPageNo(pageNo);
      mdb.fixThermoLabel(m_pw1Label, "Pulsewidth at % 3.0f° F", -40.0);
      mdb.fixThermoLabel(m_pw2Label, "Pulsewidth at % 3.0f° F", 170.0);

      mdb.fixThermoLabel(m_coolantLabel);

      mdb.fixThermoLabel(m_wwu0L, "% 3.0f° ", mdb.wwuX[0]);
      mdb.fixThermoLabel(m_wwu1L, "% 3.0f° ", mdb.wwuX[1]);
      mdb.fixThermoLabel(m_wwu2L, "% 3.0f° ", mdb.wwuX[2]);
      mdb.fixThermoLabel(m_wwu3L, "% 3.0f° ", mdb.wwuX[3]);
      mdb.fixThermoLabel(m_wwu4L, "% 3.0f° ", mdb.wwuX[4]);
      mdb.fixThermoLabel(m_wwu5L, "% 3.0f° ", mdb.wwuX[5]);
      mdb.fixThermoLabel(m_wwu6L, "% 3.0f° ", mdb.wwuX[6]);
      mdb.fixThermoLabel(m_wwu7L, "% 3.0f° ", mdb.wwuX[7]);
      mdb.fixThermoLabel(m_wwu8L, "% 3.0f° ", mdb.wwuX[8]);
      mdb.fixThermoLabel(m_wwu9L, "% 3.0f° ", mdb.wwuX[9]);
   mdb.setPageNo(savePage); 

   fieldsFromDb();

   return TRUE;
}

//------------------------------------------------------------------------------

void Denrichments::OnUp()
{
   fieldsFromDb();
}

//------------------------------------------------------------------------------

void Denrichments::OnDown()
{
   int savePage = mdb.setPageNo(pageNo);
      dbFromFields();
      _fld.send();
      mdb.burnConst(pageNo);
   mdb.setPageNo(savePage); 
}

//------------------------------------------------------------------------------

//BEGIN_EVENTSINK_MAP(Denrichments, CDialog)
//   //{{AFX_EVENTSINK_MAP(Denrichments)
//   //}}AFX_EVENTSINK_MAP
//END_EVENTSINK_MAP()

//------------------------------------------------------------------------------

void Denrichments::fieldsFromDb()
{
   int savePage = mdb.setPageNo(pageNo);
      mdb.getConst();
      _fld.setFld();
   mdb.setPageNo(savePage); 
}

//------------------------------------------------------------------------------

void Denrichments::dbFromFields()
{
   int savePage = mdb.setPageNo(pageNo);
      _fld.getFld();

      if (_fld.value(S_wueBins, 9) != 100) {
         m_wueBin9.SetWindowText("100");
         m_wueBin9.SetFocus();
         if (mdb.loaded()) MessageBox("Warning: Warmup Enrichment value at 160 degrees\nshould be set to 100%.");
      }
   mdb.setPageNo(savePage); 
}

//------------------------------------------------------------------------------
