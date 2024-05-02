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

#include "stdafx.h"
#include "megatune.h"
#include "msDatabase.h"
#include "repository.h"
#include "userDefined.h"
#include "aeWizard.h"

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------

extern msDatabase mdb;
extern repository rep;

aeWizard::aeWizard(int pageNo, CWnd *pParent)
 : dataDialog(aeWizard::IDD, pParent)
 , pageNo    (pageNo)
 , _fld      (this)
 , _lo       (  0)
 , _hi       (100)
 , _mapOn    (true)
 , _tpsOn    (true)
 , oldV      ( -1)
{
   _prp = mdb.lookupByPage(S_tpsProportion, pageNo);
   if (_prp)
      _prpMap = false;
   else {
      _prpMap = true;
      _prp = mdb.lookupByPage(S_mapProportion, pageNo);
   }
   //{{AFX_DATA_INIT(aeWizard)
   //}}AFX_DATA_INIT
}

aeWizard::~aeWizard()
{
   for (int i = 0; i < nBars; i++) delete bd[i];
}

//------------------------------------------------------------------------------

void aeWizard::DoDataExchange(CDataExchange* pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(aeWizard)
      DDX_Control(pDX, IDC_TPSDOT_UNITS,    m_tpsDotUnits);
      DDX_Control(pDX, IDC_THRESHOLD_LABEL, m_thresholdLabel);
      DDX_Control(pDX, IDC_MAPDOT_RT,       m_mapdotBar);
      DDX_Control(pDX, IDC_RATE_LABEL,      m_rateLabel);
      DDX_Control(pDX, IDC_TPSDOT_RT,       m_tpsdotBar);
      DDX_Control(pDX, IDC_TPS_PROP_LABEL,  m_tpsPropLabel);
      DDX_Control(pDX, IDC_TPS_PROPORTION,  m_tpsProportion);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(aeWizard, dataDialog)
   //{{AFX_MSG_MAP(aeWizard)
      ON_BN_CLICKED(IDC_TABLE_GET, OnUp)
      ON_BN_CLICKED(IDC_TABLE_PUT, OnDown)

      ON_WM_HSCROLL()
      ON_WM_PAINT()

      ON_EN_KILLFOCUS(IDC_TPSTHRESH,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_MAPTHRESH,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_AETIME,        dbFromFields)
      ON_EN_KILLFOCUS(IDC_AETAPERTIME,   dbFromFields)
      ON_EN_KILLFOCUS(IDC_ENDPULSEWIDTH, dbFromFields)
      ON_EN_KILLFOCUS(IDC_TPSDQ,         dbFromFields)
      ON_EN_KILLFOCUS(IDC_TPSCOLDA,      dbFromFields)
      ON_EN_KILLFOCUS(IDC_TPSCOLDM,      dbFromFields)

      ON_EN_KILLFOCUS(IDC_TPS_RATE1,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_TPS_RATE2,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_TPS_RATE3,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_TPS_RATE4,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_TPS_VAL1,      dbFromFields)
      ON_EN_KILLFOCUS(IDC_TPS_VAL2,      dbFromFields)
      ON_EN_KILLFOCUS(IDC_TPS_VAL3,      dbFromFields)
      ON_EN_KILLFOCUS(IDC_TPS_VAL4,      dbFromFields)

      ON_EN_KILLFOCUS(IDC_MAP_RATE1,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_MAP_RATE2,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_MAP_RATE3,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_MAP_RATE4,     dbFromFields)
      ON_EN_KILLFOCUS(IDC_MAP_VAL1,      dbFromFields)
      ON_EN_KILLFOCUS(IDC_MAP_VAL2,      dbFromFields)
      ON_EN_KILLFOCUS(IDC_MAP_VAL3,      dbFromFields)
      ON_EN_KILLFOCUS(IDC_MAP_VAL4,      dbFromFields)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

void aeWizard::setLabel(int ID, double v)
{
   char s[30];
   sprintf(s, "%.0f", v);
   GetDlgItem(ID)->SetWindowText(s);
}

//------------------------------------------------------------------------------

void setColors (CBarMeter &m);

BOOL aeWizard::OnInitDialog() 
{
   dataDialog::OnInitDialog("Acceleration Wizard", pageNo+1);

   setLabel(IDC_TPSDOT_LO, rep.loaTPD);
   setLabel(IDC_TPSDOT_HI, rep.hiaTPD);
   bd[0] = new barData(S_tpsDOT, this, IDC_TPSDOT_LB, IDC_TPSDOT_RT, rep.loaTPD, rep.hiaTPD, 1);
   setLabel(IDC_MAPDOT_LO, rep.loaMAD);
   setLabel(IDC_MAPDOT_HI, rep.hiaMAD);
   bd[1] = new barData(S_mapDOT, this, IDC_MAPDOT_LB, IDC_MAPDOT_RT, rep.loaMAD, rep.hiaMAD, 1);

   symbol *s = mdb.lookupByPage(S_tpsThresh, pageNo);
   if (s) {
      const CString &u = s->units();
      m_rateLabel     .SetWindowText("Rate ("+u+")");
      m_thresholdLabel.SetWindowText("TPSdot Threshold ("+u+")");
      m_tpsDotUnits   .SetWindowText(u);
   }

   _fld.add(S_tpsThresh,   IDC_TPSTHRESH,     pageNo);
   _fld.add(S_mapThresh,   IDC_MAPTHRESH,     pageNo);
   _fld.add(S_taeTime,     IDC_AETIME,        pageNo);
   _fld.add(S_aeTaperTime, IDC_AETAPERTIME,   pageNo);
   _fld.add(S_aeEndPW,     IDC_ENDPULSEWIDTH, pageNo);
   _fld.add(S_tdePct,      IDC_TPSDQ,         pageNo);
   _fld.add(S_taeColdA,    IDC_TPSCOLDA,      pageNo);
   _fld.add(S_taeColdM,    IDC_TPSCOLDM,      pageNo);

   for (int iAE = 0; iAE <  4; iAE++) {
      _fld.add(S_maeRates, IDC_MAP_RATE1+iAE, pageNo, iAE);
      _fld.add(S_maeBins,  IDC_MAP_VAL1+iAE,  pageNo, iAE);
      _fld.add(S_taeRates, IDC_TPS_RATE1+iAE, pageNo, iAE);
      _fld.add(S_taeBins,  IDC_TPS_VAL1+iAE,  pageNo, iAE);
   }

   if (!_prp)
      m_tpsProportion.EnableWindow(false);
   else {
      if (_prpMap) {
         _lo = -_prp->hi();
         _hi = -_prp->lo();
      }
      else {
         _lo = _prp->lo();
         _hi = _prp->hi();
      }
      m_tpsProportion.SetRange(_lo, _hi);
      m_tpsProportion.SetPos  (_lo); // Fixed starting point.
   }

   fieldsFromDb();

   meterEGO.SetRange(rep.lofEGO, rep.hifEGO);
   meterEGO.SetAlert(rep.rdfEGO > 0.0 ? rep.rdfEGO : mdb.getByName(S_egoSwitch, 0));
   setColors(meterEGO);

   return TRUE;
}

//------------------------------------------------------------------------------

void aeWizard::OnPaint()
{
   dataDialog::OnPaint();

   CRect r;
   GetDlgItem(IDC_LED_BAR)->GetWindowRect(&r);
   ScreenToClient(&r);

   CPaintDC dc(this);
   meterEGO.ShowMeter(&dc, r);
}

//------------------------------------------------------------------------------

bool aeWizard::doTimer(bool connected)
{
   if (!realized()) return false;

   if (!connected) {
      for (int i = 0; i < nBars; i++) bd[i]->disconnected();
   }
   else {
      for (int i = 0; i < nBars; i++) bd[i]->show();

      CDC *dc = GetDC();
         meterEGO.UpdateNeedle(dc, mdb.egoLEDvalue());
      ReleaseDC(dc);
   }
   return true;
}

//------------------------------------------------------------------------------

void aeWizard::OnUp()
{
   fieldsFromDb();
}

//------------------------------------------------------------------------------

void aeWizard::OnDown()
{
   int savePage = mdb.setPageNo(pageNo);
      dbFromFields();
      _fld.send();
      mdb.burnConst(pageNo);
   mdb.setPageNo(savePage); 
}

//------------------------------------------------------------------------------

void aeWizard::fieldsFromDb()
{
   int savePage = mdb.setPageNo(pageNo);
      mdb.getConst();
      _fld.setFld();
      if (_prp) {
         int v = _prp->valueUser();
         if (_prpMap) v = -v;
         m_tpsProportion.SetPos(v);
      }
      onProportionChanged();
   mdb.setPageNo(savePage); 
}

//------------------------------------------------------------------------------

void aeWizard::dbFromFields()
{
   int savePage = mdb.setPageNo(pageNo);
      if (_prp) {
         _prp->storeValue(abs(m_tpsProportion.GetPos()));
         mdb.cDesc.send(_prp);
      }
      _fld.getFld();
   mdb.setPageNo(savePage); 
}

//------------------------------------------------------------------------------

void aeWizard::onProportionChanged()
{
   int v = int(100.0 * double(m_tpsProportion.GetPos()-_lo) / double(_hi-_lo));
   if (_prp && v != oldV) {
      char label[128];
      sprintf(label, "MAP vs TPS (%3d%%)", v);
      m_tpsPropLabel.SetWindowText(label);
      _prp->storeValue(abs(m_tpsProportion.GetPos()));
      mdb.cDesc.send(_prp);
      oldV = v;

      _mapOn = v != 100;
      _tpsOn = v !=   0;
      for (int iAE = 0; iAE <  4; iAE++) {
         GetDlgItem(IDC_MAP_RATE1+iAE)->EnableWindow(_mapOn);
         GetDlgItem(IDC_MAP_VAL1 +iAE)->EnableWindow(_mapOn);
         GetDlgItem(IDC_TPS_RATE1+iAE)->EnableWindow(_tpsOn);
         GetDlgItem(IDC_TPS_VAL1 +iAE)->EnableWindow(_tpsOn);
      }
   }
}

void aeWizard::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   onProportionChanged();
   dataDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

//------------------------------------------------------------------------------
