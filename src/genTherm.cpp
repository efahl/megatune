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
#include "genTherm.h"
#include "msDatabase.h"
#include <math.h>

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

extern msDatabase mdb;

//------------------------------------------------------------------------------

genTherm::genTherm(int pageNo, CWnd *pParent)
 : dataDialog(genTherm::IDD, pParent)
 , pageNo    (pageNo)
{
   //{{AFX_DATA_INIT(genTherm)
     m_biasValue = 2490.0f;

     m_temp1 = -40.0; m_resi1 = 100700.0;
     m_temp2 =  30.0; m_resi2 =   2238.0;
     m_temp3 =  99.0; m_resi3 =    177.0;
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void genTherm::DoDataExchange(CDataExchange* pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(genTherm)
      DDX_Control    (pDX, IDC_TABLE_TYPE, m_tableType);
      DDX_Control    (pDX, IDC_TEMP_C,     m_tempSetting);
      DDX_Control    (pDX, IDC_TEMP_LABEL, m_tempLabel);

      DDX_Text       (pDX, IDC_BIASR, m_biasValue);
      DDV_MinMaxFloat(pDX, m_biasValue, 0.0f, 100000.0f);

      DDX_Text       (pDX, IDC_TEMP1, m_temp1);
      DDX_Text       (pDX, IDC_TEMP2, m_temp2);
      DDX_Text       (pDX, IDC_TEMP3, m_temp3);
      DDV_MinMaxFloat(pDX, m_temp1, -200.0f,    400.0f);
      DDV_MinMaxFloat(pDX, m_temp2, -200.0f,    400.0f);
      DDV_MinMaxFloat(pDX, m_temp3, -200.0f,    400.0f);

      DDX_Text       (pDX, IDC_RESI1, m_resi1);
      DDX_Text       (pDX, IDC_RESI2, m_resi2);
      DDX_Text       (pDX, IDC_RESI3, m_resi3);
      DDV_MinMaxFloat(pDX, m_resi1,   0.0f, 500000.0f);
      DDV_MinMaxFloat(pDX, m_resi2,   0.0f, 500000.0f);
      DDV_MinMaxFloat(pDX, m_resi3,   0.0f, 500000.0f);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(genTherm, dataDialog)
   //{{AFX_MSG_MAP(genTherm)
      ON_BN_CLICKED(IDC_TEMP_C, OnTemp)
      ON_BN_CLICKED(IDC_TEMP_F, OnTemp)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

void genTherm::setLabel(bool degC)
{
   char label[128];
   sprintf(label, "Temperature (° %c)", degC ? 'C' : 'F');
   m_tempLabel.SetWindowText(label);
}

//------------------------------------------------------------------------------

BOOL genTherm::OnInitDialog() 
{
   dataDialog::OnInitDialog("Table Generator");
   
   m_tableType.SetCurSel(0);
   m_tempSetting.SetCheck(true);
   setLabel(true);

   return TRUE;
}

//------------------------------------------------------------------------------

void genTherm::OnTemp() { setLabel(m_tempSetting.GetCheck() != 0);  }

//------------------------------------------------------------------------------

double genTherm::k2f(double t)                  { return (t * 9.0/5.0) - 459.67; }
double genTherm::Tk (double R) /* Kelvins    */ { return 1.0 / (A + B*log(R) + C*pow(log(R), 3)); }
double genTherm::Tf (double R) /* Fahrenheit */ { return k2f(Tk(R)); }
double genTherm::Tu (double f, bool degC)       { return degC ? ((f-32.0)*5.0/9.0) : f; }

void genTherm::OnOK() 
{
   GetDlgItem(IDOK    )->EnableWindow(FALSE);
   GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

   if (!mdb.getRuntime()) {
      msgOk("Table Generator", "Cannot connect to MS-II processor to send table.");
      dataDialog::OnOK();
      return;
   }

   CWaitCursor wait;
   UpdateData();

   bool degC = m_tempSetting.GetCheck() == BST_CHECKED;

   double t[3];
   t[0] = m_temp1;
   t[1] = m_temp2;
   t[2] = m_temp3;
   int i;
   for (i = 0; i < 3; i++) {
      if (!degC) t[i] = (t[i]-32.0)*5.0/9.0;
      t[i] += 273.15;
   }

   double c11 = log(m_resi1); double c12 = pow(c11, 3.0); double c13 = 1.0 / t[0];
   double c21 = log(m_resi2); double c22 = pow(c21, 3.0); double c23 = 1.0 / t[1];
   double c31 = log(m_resi3); double c32 = pow(c31, 3.0); double c33 = 1.0 / t[2];

   C = ((c23-c13) - (c33-c13)*(c21-c11)/(c31-c11)) / ((c22-c12) - (c32-c12)*(c21-c11)/(c31-c11));
   B = (c33-c13 - C*(c32-c12)) / (c31-c11);
   A = c13 - B*c11 - C*c12;

   // Send "t <type-byte> 1024 x 16-bit words"

#define CTS 0
#define MAT 1
   BYTE tableType = m_tableType.GetCurSel(); // 0 = CTS (see MS-II main.c)
                                             // 1 = MAT
   BYTE table[2 + 2*1024];
   table[0] = 't';
   table[1] = tableType;

   FILE *f = fopen("thermistor.log", "a");
      fprintf(f, "//------------------------------------------------------------------------------\n");
      fprintf(f, "//--  Generated by MegaTune %-50s--\n", timeStamp());
      fprintf(f, "//--  This file merely records what was sent to your MS-II, and may be        --\n");
      fprintf(f, "//--  deleted at any time.                                                    --\n");
      fprintf(f, "//--                                                                          --\n");
      fprintf(f, "//--  Type = %d (%s)                                                          --\n", table[1], tableType==CTS?"CTS":"MAT");
      fprintf(f, "//--  Bias = %7.1f                                                          --\n", m_biasValue);
      fprintf(f, "//--                                                                          --\n");
      fprintf(f, "//--  Resistance      tInput             tComputed                            --\n");
      fprintf(f, "//--  ------------    -----------------  ---------                            --\n");
      fprintf(f, "//--  %8.1f ohm  %7.1fK (% 7.1f%c) %10.1f                            --\n", m_resi1, t[0], m_temp1, degC?'C':'F', Tu(Tf(m_resi1), degC));
      fprintf(f, "//--  %8.1f ohm  %7.1fK (% 7.1f%c) %10.1f                            --\n", m_resi2, t[1], m_temp2, degC?'C':'F', Tu(Tf(m_resi2), degC));
      fprintf(f, "//--  %8.1f ohm  %7.1fK (% 7.1f%c) %10.1f                            --\n", m_resi3, t[2], m_temp3, degC?'C':'F', Tu(Tf(m_resi3), degC));
      fprintf(f, "//------------------------------------------------------------------------------\n");
      fprintf(f, "\n");
      fprintf(f, "#ifndef GCC_BUILD\n");
      fprintf(f, "#pragma ROM_VAR %s_ROM\n", tableType==CTS?"CLT":"MAT");
      fprintf(f, "#endif\n");
      fprintf(f, "const int %sfactor_table[1024] EEPROM_ATTR  = {\n", tableType==CTS?"clt":"mat");
      fprintf(f, "          //  ADC    Volts    Temp       Ohms\n");

      BYTE *b  = table+2;
      for (int adcCount = 0; adcCount < 1024; adcCount++) {
         double res  = m_biasValue / (1023.0/double(adcCount==0?0.01:adcCount) - 1.0);
         double temp = Tf(res);
         if      (temp <  -40.0) temp = tableType == CTS ? 180.0 : 70.0;
         else if (temp >  350.0) temp = tableType == CTS ? 180.0 : 70.0;
         short tt = short(temp*10);
         fprintf(f, "   %5d%c // %4d %7.2f  %7.1f  %9.1f\n", tt, adcCount<1024-1?',':' ', adcCount, 5.0*adcCount/1023.0, Tu(tt/10.0, degC), res);
         *b = tt >>   8; b++; // ??? Let controller handle endianness!!!
         *b = tt & 0xFF; b++;
      }
      fprintf(f, "};\n");
      fprintf(f, "#ifndef GCC_BUILD\n");
      fprintf(f, "#pragma ROM_VAR DEFAULT\n");
      fprintf(f, "#endif\n");
      fprintf(f, "//------------------------------------------------------------------------------\n");
   fclose(f);

   CStatic *status = static_cast<CStatic *>(GetDlgItem(IDC_DOWNLOAD_STATUS));
   mdb.cDesc.write(table, 2);
   ::Sleep(100);
   for (i = 2; i < sizeof(table); i += 2) {
      char msg[50];
      sprintf(msg, "Downloading %4d of %4d entries", i/2, sizeof(table)/2-1);
      status->SetWindowText(msg);

      mdb.cDesc.write(table+i, 2);
      ::Sleep(4);
   }

   dataDialog::OnOK();
}

//------------------------------------------------------------------------------
