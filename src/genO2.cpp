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
#include "genO2.h"
#include "msDatabase.h"
#include <math.h>

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

extern msDatabase mdb;

//------------------------------------------------------------------------------

genO2::genO2(int pageNo, CWnd *pParent)
 : dataDialog(genO2::IDD, pParent)
 , pageNo    (pageNo)
{
   //{{AFX_DATA_INIT(genO2)
      m_volt1 = 1.0; m_afr1 =  9.7;
      m_volt2 = 4.0; m_afr2 = 19.7;
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void genO2::DoDataExchange(CDataExchange* pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(genO2)
      DDX_Control(pDX, IDC_TABLE_TYPE, m_tableType);
      DDX_Text(pDX, IDC_VOLT1, m_volt1);
      DDX_Text(pDX, IDC_VOLT2, m_volt2);
      DDX_Text(pDX, IDC_AFR1, m_afr1);
      DDX_Text(pDX, IDC_AFR2, m_afr2);
      DDV_MinMaxDouble(pDX, m_volt1, 0.0, 5.0);
      DDV_MinMaxDouble(pDX, m_volt2, 0.0, 5.0);
      DDV_MinMaxDouble(pDX, m_afr1, 0.0, 99.0);
      DDV_MinMaxDouble(pDX, m_afr2, 0.0, 99.0);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(genO2, dataDialog)
   //{{AFX_MSG_MAP(genO2)
      ON_CBN_SELCHANGE(IDC_TABLE_TYPE, OnSelectionChange)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

enum {
   genericWB,
   narrowBand,
   egorTech,
   diyWB,
   dynojetLinear,
   fjo,
   aemLinear,
   aemNonLinear,
   techEdgeLinear,
   techEdgeNonLinear,
   innovate12,
   innovate05,
   innovateLC1,
   lambdaBoy,
   zeitronix
};

static void addString(CComboBox &b, const char *str, int value)
{
   b.AddString(str);
   b.SetItemData(b.GetCount()-1, value);
}

BOOL genO2::OnInitDialog()
{
   dataDialog::OnInitDialog("MS-II AFR Calibrator");

   m_tableType.ResetContent();

   addString(m_tableType, "Narrow Band",                narrowBand);

   addString(m_tableType, "AEM Linear AEM-30-42xx",     aemLinear);
   addString(m_tableType, "AEM Non-linear AEM-30-230x", aemNonLinear);
   addString(m_tableType, "Daytona TwinTec",            innovate05);
   addString(m_tableType, "DIY-WB",                     diyWB);
   addString(m_tableType, "DynoJet Wideband Commander", dynojetLinear);
   addString(m_tableType, "EGORtech WB",                egorTech);
   addString(m_tableType, "FJO WB",                     fjo);
   addString(m_tableType, "Innovate 0.0-5.0 v",         innovate05);
   addString(m_tableType, "Innovate 1.0-2.0 v",         innovate12);
   addString(m_tableType, "Innovate LC-1 0.5-1.5 L",    innovateLC1);
   addString(m_tableType, "LambdaBoy",                  lambdaBoy);
   addString(m_tableType, "TechEdge SVout",             techEdgeNonLinear);
   addString(m_tableType, "TechEdge WBlin",             techEdgeLinear);
   addString(m_tableType, "Zeitronix",                  zeitronix);

   addString(m_tableType, "Generic Linear WB",          genericWB);

   m_tableType.SetCurSel(-1);
   OnSelectionChange();

   return TRUE;
}

//------------------------------------------------------------------------------

#define nADC 1024

//static double NBBv[] = { 0.00, 0.44, 0.45, 0.50, 0.51, 1.00, 5.01 };
//static double NBBa[] = { 27.0, 17.0, 14.8, 14.6, 12.0,  2.0,  0.0 };
static double NBFv (int adc) { return 100.0 * (1.0 - adc * 5.0/nADC); }

static double diywbBv[] = { 0.00,  1.40,  1.45,  1.50,  1.55,  1.60,  1.65,  1.70,  1.75,  1.80,  1.85,  1.90,  1.95,  2.00,  2.05,  2.10,  2.15,  2.20,  2.25,  2.30,  2.35,  2.40,  2.45,  2.50,  2.55,  2.60,  2.65,  2.70,  2.75,  2.80,  2.85,  2.90,  4.00,  5.01 };
static double diywbBa[] = { 7.42, 10.08, 10.23, 10.38, 10.53, 10.69, 10.86, 11.03, 11.20, 11.38, 11.57, 11.76, 11.96, 12.17, 12.38, 12.60, 12.83, 13.07, 13.31, 13.57, 13.84, 14.11, 14.40, 14.70, 15.25, 15.84, 16.48, 17.18, 17.93, 18.76, 19.66, 20.66, 40.00, 60.00 };

static double lbwbBv[] = {0.00,  2.05,  4.21,  4.98,  5.01};
static double lbwbBa[] = {1.00, 11.00, 14.70, 16.00, 99.00};

static double teSVoutBv[] = {1.024, 1.076, 1.126, 1.177, 1.227, 1.278, 1.330, 1.380, 1.431, 1.481, 1.532, 1.581,
1.626, 1.672, 1.717, 1.761, 1.802, 1.842, 1.883, 1.926, 1.971, 2.015, 2.053, 2.104, 2.150, 2.192, 2.231, 2.267,
2.305, 2.347, 2.398, 2.455, 2.514, 2.556, 2.602, 2.650, 2.698, 2.748, 2.797, 2.846, 2.900, 2.945, 2.991, 3.037,
3.083, 3.129, 3.175, 3.221, 3.266, 3.313, 3.359, 3.404, 3.451, 3.496, 3.542, 3.587, 3.634, 3.680, 3.725, 3.772,
3.817, 3.863, 3.910, 3.955, 4.001 };
static double teSVoutBa[] = { 8.95, 9.11, 9.26, 9.41, 9.56, 9.71, 9.87, 10.02, 10.17, 10.32, 10.47, 10.63, 10.78,
10.93, 11.08, 11.24, 11.39, 11.54, 11.69, 11.86, 12.04, 12.23, 12.39, 12.62, 12.83, 13.03, 13.21, 13.4, 13.59,
13.82, 14.1, 14.43, 14.83, 15.31, 15.85, 16.47, 17.15, 17.9, 18.7, 19.57, 20.5, 21.5, 22.59, 23.78, 25.1, 26.54,
28.14, 29.9, 31.87, 34.11, 36.81, 40.27, 45.1, 52.38, 63.92, 82.66, 99.0, 99.0, 99.0, 99.0, 99.0, 99.0, 99.0, 99.0,
99.0};

static double aemLinBv[] = { 0.00, 0.16, 0.31, 0.47, 0.62, 0.78, 0.94, 1.09, 1.25, 1.40, 1.56, 1.72, 1.87, 2.03,
2.18, 2.34, 2.50, 2.65, 2.81, 2.96, 3.12, 3.27, 3.43, 3.59, 3.74, 3.90, 4.05, 4.21, 4.37, 4.52, 4.68, 4.83, 4.99,
5.01 };
static double aemLinBa[] = { 9.72, 10.01, 10.35, 10.64, 10.98, 11.27, 11.55, 11.90, 12.18, 12.47, 12.81, 13.10,
13.44, 13.73, 14.01, 14.35, 14.64, 14.93, 15.27, 15.56, 15.84, 16.18, 16.47, 16.81, 17.10, 17.39, 17.73, 18.01,
18.36, 18.64, 18.93, 19.27, 19.56, 99.00 };

static double aemNonBv[] = { 0.00, 0.16, 0.31, 0.47, 0.62, 0.78, 0.94, 1.09, 1.25, 1.40, 1.56, 1.72, 1.87, 2.03,
2.18, 2.34, 2.50, 2.65, 2.81, 2.96, 3.12, 3.27, 3.43, 3.59, 3.74, 3.90, 4.05, 4.21, 4.37, 4.52, 4.68, 4.83, 4.99,
5.01 };
static double aemNonBa[] = { 8.41, 8.52, 8.64, 8.81, 8.98, 9.09, 9.26, 9.44, 9.61, 9.78, 9.95, 10.12, 10.29, 10.47,
10.69, 10.92, 11.15, 11.38, 11.67, 11.95, 12.24, 12.58, 12.92, 13.27, 13.67, 14.13, 14.64, 15.21, 15.84, 16.53,
17.27, 18.19, 19.44, 99.00 };

static double fjoBv[] = { 0.000, 0.811, 0.816, 1.256, 1.325, 1.408, 1.447, 1.667, 1.784, 1.804, 1.872, 1.984, 2.023,
2.126, 2.209, 2.268, 2.414, 2.454, 2.473, 2.502, 2.522, 2.581, 2.610, 2.717, 2.766, 2.820, 2.908, 2.933, 2.977,
3.021, 3.079, 3.099, 3.104, 5.000 };
static double fjoBa[] = { 0.000, 9.996, 10.011, 11.113, 11.290, 11.481, 11.569, 12.142, 12.451, 12.510, 12.730,
13.024, 13.142, 13.465, 13.715, 13.892, 14.377, 14.524, 14.597, 14.759, 14.876, 15.273, 15.479, 16.302, 16.714,
17.170, 18.008, 18.243, 18.684, 19.184, 19.801, 19.977, 20.007, 29.400 };

static double zeitronixBv[] = { 0.000, 0.150, 0.310, 0.460, 0.620, 0.780, 0.930, 1.090, 1.240, 1.400, 1.560,
1.710, 1.870, 2.020, 2.180, 2.340, 2.500, 2.650, 2.800, 2.960, 3.000, 3.120, 3.270, 5.010 };
static double zeitronixBa[] = { 0.000, 9.700, 9.900, 10.100, 10.300, 10.500, 10.700, 11.000, 11.400, 11.700,
12.100, 12.400, 12.800, 13.200, 13.700, 14.200, 14.700, 15.600, 16.900, 18.500, 18.800, 19.900, 21.200, 99.000 };

static double genericBv[4] = { 0.0, 1.0,  4.0, 5.01 };
static double genericBa[4] = { 0.0, 9.7, 19.7, 99.0 };

#define USE_TABLE(prefix) \
   Bv = prefix ## Bv; \
   Ba = prefix ## Ba; \
   nB = (sizeof(prefix ## Bv) / sizeof(double));

static double inno12Fv (int adc) { return         adc * 50.0 / (nADC-1.0); }
static double inno05Fv (int adc) { return 10.00 + adc * 10.0 / (nADC-1.0); }
static double innoLC1Fv(int adc) { return  7.35 + adc * 14.7 / (nADC-1.0); }
static double teWBlinFv(int adc) { return  9.00 + adc * 10.0 / (nADC-1.0); }
static double djWBlinFv(int adc) { return 10.00 + adc *  8.0 / (nADC-1.0); }

#define USE_FUNC(prefix) \
   Fv = prefix ## Fv

void genO2::OnOK()
{
   GetDlgItem(IDOK    )->EnableWindow(FALSE);
   GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

   if (!mdb.getRuntime()) {
      msgOk("AFR Calibrator", "Cannot connect to MS-II processor to send table.");
      dataDialog::OnOK();
      return;
   }

   CWaitCursor wait;
   UpdateData();

   double  *Bv;
   double  *Ba;
   int      nB = 0;
   double (*Fv)(int adc) = NULL;
   bool     NB = false;
   int     idx = m_tableType.GetCurSel();

   switch (m_tableType.GetItemData(idx)) {
      case  narrowBand       :
         // USE_TABLE(NB);        break;  // Narrow Band
            NB = true;
            USE_FUNC(NB);
            break;
      case  egorTech         :
            msgOk("GenO2", "Don't have EGOR transfer function, post it to msefi.com."); return;
         // USE_TABLE(x); break;  // EGORtech WB
      case  diyWB            : USE_TABLE(diywb);     break;  // DIY-WB
      case  dynojetLinear    : USE_FUNC (djWBlin);   break;
      case  fjo              : USE_TABLE(fjo);       break;  // FJO WB
      case  aemLinear        : USE_TABLE(aemLin);    break;  // AEM WB
      case  aemNonLinear     : USE_TABLE(aemNon);    break;
      case  techEdgeLinear   : USE_FUNC (teWBlin);   break;  // TechEdge WBlin 0-5 : 9-19
      case  techEdgeNonLinear: USE_TABLE(teSVout);   break;  // TechEdge SVout
      case  innovate12       : USE_FUNC (inno12);    break;  // Innovate 1-2 : 10-20
      case  innovate05       : USE_FUNC (inno05);    break;  // Innovate 0-5 : 10-20
      case innovateLC1       : USE_FUNC (innoLC1);   break;
      case  lambdaBoy        : USE_TABLE(lbwb);      break;  // LambdaBoy
      case  zeitronix        : USE_TABLE(zeitronix); break;  // Zeitronix

      case genericWB: { // Generic Linear WB, user-entered data.
         genericBv[1] = m_volt1; genericBa[1] = m_afr1;
         genericBv[2] = m_volt2; genericBa[2] = m_afr2;
         USE_TABLE(generic);
         break;
      }
   }

   BYTE table[nADC];

   FILE *f = fopen("afrTable.log", "a");
      int iV = 0;
      double afr = 0.0;
      CString type;
      m_tableType.GetLBText(idx, type);
      fprintf(f, "//------------------------------------------------------------------------------\n");
      fprintf(f, "//--  Generated by MegaTune %-50s--\n", timeStamp());
      fprintf(f, "//--  This file merely records what was sent to your MS-II, and may be        --\n");
      fprintf(f, "//--  deleted at any time.                                                    --\n");
      fprintf(f, "//--  Selected type: %-57s--\n", type);
      fprintf(f, "//------------------------------------------------------------------------------\n");
      fprintf(f, "#ifndef GCC_BUILD\n");
      fprintf(f, "#pragma ROM_VAR EGO_ROM\n");
      fprintf(f, "#endif\n");
      fprintf(f, "const unsigned char egofactor_table[%d] EEPROM_ATTR = {\n", nADC);
      fprintf(f, "         //     afr  adcCount voltage\n");
      for (int adcCount = 0; adcCount < nADC; adcCount++) {
         double voltage = adcCount / (nADC-1.0) * 5.0;
         if (NB) {
            afr = 0.0;
            table[adcCount] = (adcCount > nADC/5.0) ? 0 : int(nADC/5.0 - adcCount);
         }
         else {
            if (Fv) 
               afr = Fv(adcCount);
            else {
               // Use curve data from tabular expression of transfer function.
               while (voltage > Bv[iV]) iV++;
               double deltaVoltage = Bv[iV] - Bv[iV-1];
               if (fabs(deltaVoltage) < 1e-10) // Curve data is crap.
                  afr = 999.0;
               else {
                  double vPct = 1.0 - (Bv[iV] - voltage) / deltaVoltage;
                  afr  = vPct * (Ba[iV] - Ba[iV-1]) + Ba[iV-1];
               }
            }
            table[adcCount] = BYTE(afr*10.0+0.5);
         }

         fprintf(f, "   %4d%c // %7.3f   %4d   %6.3f\n", table[adcCount], (adcCount<nADC-1)?',':' ', afr, adcCount, voltage);
      }
      fprintf(f, "};\n");
      fprintf(f, "#ifndef GCC_BUILD\n");
      fprintf(f, "#pragma ROM_VAR DEFAULT\n");
      fprintf(f, "#endif\n");
      fprintf(f, "//------------------------------------------------------------------------------\n");
   fclose(f);

   CStatic *status = static_cast<CStatic *>(GetDlgItem(IDC_DOWNLOAD_STATUS));
   BYTE tblCreate[2] = {'t', '\002' };
   mdb.cDesc.write(tblCreate, 2);
   ::Sleep(100);
   CString msg;
   for (int i = 0; i < sizeof(table); i += 2) {
      msg.Format("Downloading %4d of %4d entries", i, sizeof(table));
      status->SetWindowText(msg);

      mdb.cDesc.write(table+i, 2);
      ::Sleep(4);
   }
   msg.Format("Completed writing %d entries", sizeof(table));
   status->SetWindowText(msg);
   ::Sleep(1000);

   dataDialog::OnOK();
}

//------------------------------------------------------------------------------

void genO2::OnSelectionChange() 
{
   bool on = m_tableType.GetItemData(m_tableType.GetCurSel()) == genericWB;
   GetDlgItem(IDC_VOLT1)->EnableWindow(on);
   GetDlgItem(IDC_VOLT2)->EnableWindow(on);
   GetDlgItem(IDC_AFR1 )->EnableWindow(on);
   GetDlgItem(IDC_AFR2 )->EnableWindow(on);
}

//------------------------------------------------------------------------------
