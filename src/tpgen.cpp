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
#include "tpgen.h"
#include "msDatabase.h"
#include "stringConstants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern msDatabase mdb;

//------------------------------------------------------------------------------

tpgen::tpgen(CWnd *pParent)
 : dataDialog(tpgen::IDD, pParent)
{
   //{{AFX_DATA_INIT(tpgen)
   //}}AFX_DATA_INIT
}

void tpgen::DoDataExchange(CDataExchange* pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(tpgen)
      DDX_Control(pDX, IDC_TP_LO, m_tp_lo);
      DDX_Control(pDX, IDC_TP_HI, m_tp_hi);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(tpgen, dataDialog)
   //{{AFX_MSG_MAP(tpgen)
      ON_BN_CLICKED(IDC_GET_LO, OnGetLo)
      ON_BN_CLICKED(IDC_GET_HI, OnGetHi)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

void tpgen::getVal(CEdit &fld)
{
   if (mdb.getRuntime()) {
      symbol *tpsADC = mdb.cDesc.lookup(S_tpsADC);
      if (tpsADC) fld.SetWindowText(tpsADC->valueString());
      else        fld.SetWindowText("tpsADC?");
   }
}

void tpgen::OnGetLo() { getVal(m_tp_lo); } 
void tpgen::OnGetHi() { getVal(m_tp_hi); }

//------------------------------------------------------------------------------

int tpgen::pctFromAdc(int adc)
{
   if (adc <= lo) return   0;
   if (adc >= hi) return 100;
   return (adc-lo) * 100 / (hi-lo);
}

FILE *cfgOpen(const char *fileName, const char *mode);
int   cfgClose(FILE *file);
const char *timeStamp(bool longForm);

void tpgen::writeFile()
{
   if (!_looksLikeMSII) {
      FILE *tp = cfgOpen("throttlefactor.inc", "w");
      fprintf(tp, 
         "; MegaTune-generated Linear Throttle Calibration\n"
         "; Written on %s\n"
         ";\n"
         ";   Low ADC = %d    High ADC = %d\n"
         ";\n"
         "THROTTLEFACTOR:\n"
         "\t\t\t; ADC\n"
         , timeStamp(true), lo, hi);
      for (int adc = 0; adc < 256; adc++) {
         fprintf(tp, "\tDB\t%3dT\t; %3d\n", pctFromAdc(adc), adc);
      }
      cfgClose(tp);
   }
}

//------------------------------------------------------------------------------

void tpgen::OnOK()
{
   char wText[10];

   int loLim = _looksLikeMSII ?    0 :    0;
   int miLim = _looksLikeMSII ?  512 :  128;
   int hiLim = _looksLikeMSII ? 1023 :  255;

   CString hiStr;
   hiStr.Format("%d", hiLim);

   m_tp_lo.GetWindowText(wText, sizeof(wText)-1);
   lo = strtol(wText, NULL, 10);
   if (lo < loLim || lo > miLim) {
      if (MessageBox(CString("Low value ") +wText+" looks funny, should be 0 <= x <<= "+hiStr+", use it anyhow?", "Throttle Position Calibrator", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES) {
         return;
      }
   }

   m_tp_hi.GetWindowText(wText, sizeof(wText)-1);
   hi = strtol(wText, NULL, 10);
   if(hi < miLim || hi > hiLim) {
      if (MessageBox(CString("High value ") +wText+" looks funny, should be 0 <<= x <= "+hiStr+", use it anyhow?", "Throttle Position Calibrator", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES) {
         return;
      }
   }

   if (_looksLikeMSII) {
      mdb.setByName("tpsMin", 0, lo); mdb.sendByName("tpsMin", 0);
      mdb.setByName("tpsMax", 0, hi); mdb.sendByName("tpsMax", 0);
      mdb.cDesc.sendBurnCommand(0);
   }

   writeFile();

   dataDialog::OnOK();
}

void tpgen::OnCancel() 
{
   if (_looksLikeMSII) {
      // Restore the original settings.
      mdb.setByName("tpsMin", 0, _tpsMin); mdb.sendByName("tpsMin", 0);
      mdb.setByName("tpsMax", 0, _tpsMax); mdb.sendByName("tpsMax", 0);
   }
   dataDialog::OnCancel();
}

//------------------------------------------------------------------------------

BOOL tpgen::OnInitDialog()
{
   dataDialog::OnInitDialog("Calibrate TPS");

   _looksLikeMSII = false;
        
   // Stash the originals and check if they exist.
   _tpsMin = mdb.getByName("tpsMin", 0);
   _tpsMax = mdb.getByName("tpsMax", 0);

   if (_tpsMin != 0.0 || _tpsMax != 0.0) {
      // Reset to limit settings for MS-II.
      _looksLikeMSII = true;

      mdb.setByName("tpsMin", 0,    0.0); mdb.sendByName("tpsMin", 0);
      mdb.setByName("tpsMax", 0, 1023.0); mdb.sendByName("tpsMax", 0);

      CString fldVal;
      fldVal.Format("%.0f", _tpsMin);
      m_tp_lo.SetWindowText(fldVal);
      fldVal.Format("%.0f", _tpsMax);
      m_tp_hi.SetWindowText(fldVal);
   }

   return TRUE;
}

//------------------------------------------------------------------------------
