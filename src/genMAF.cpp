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
//-- GenMAF created from genTherm by L. Gardiner, December 2006

#include "stdafx.h"
#include "megatune.h"
#include "genMAF.h"
#include "msDatabase.h"
#include <math.h>

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

extern msDatabase mdb;

//------------------------------------------------------------------------------

genMAF::genMAF(int pageNo, CWnd *pParent)
 : dataDialog(genMAF::IDD, pParent)
 , pageNo    (pageNo)
{
   //{{AFX_DATA_INIT(genMAF)
     // Default Values

 //  m_biasValue = 24.0f;

     // these values for testing only, replace with Ford curve when ready
     // give crazy values with log fitting, etc.

     m_af1 =    300.0; m_mafvolts1 =    20.0;
     m_af2 =   3000.0; m_mafvolts2 =    80.0;
     m_af3 =  30000.0; m_mafvolts3 =   160.0;
   //}}AFX_DATA_INIT
}

//------------------------------------------------------------------------------

void genMAF::DoDataExchange(CDataExchange* pDX)
{
   dataDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(genMAF)

      // Table Properties
      DDX_Control    (pDX, IDC_TABLE_TYPE, m_tableType);
      DDX_Control    (pDX, IDC_MG_SEC,     m_tempSetting);
      DDX_Control    (pDX, IDC_TEMP_LABEL, m_tempLabel);


      // Bias resistor - not used in genMAF
//      DDX_Text       (pDX, IDC_BIASR, m_biasValue);
//      DDV_MinMaxFloat(pDX, m_biasValue, 0.0f, 100000.0f);


      // 3 Temps - will become Air Flow in genMAF
      DDX_Text       (pDX, IDC_AF1, m_af1);
      DDX_Text       (pDX, IDC_AF2, m_af2);
      DDX_Text       (pDX, IDC_AF3, m_af3);
      DDV_MinMaxFloat(pDX, m_af1, 0.0f,    300000.0f);
      DDV_MinMaxFloat(pDX, m_af2, 0.0f,    300000.0f);
      DDV_MinMaxFloat(pDX, m_af3, 0.0f,    300000.0f);


      // 3 Resistances - will become Volts in genMAF
      DDX_Text       (pDX, IDC_MAFVOLTS1, m_mafvolts1);
      DDX_Text       (pDX, IDC_MAFVOLTS2, m_mafvolts2);
      DDX_Text       (pDX, IDC_MAFVOLTS3, m_mafvolts3);
      DDV_MinMaxFloat(pDX, m_mafvolts1,   0.0f, 500000.0f);
      DDV_MinMaxFloat(pDX, m_mafvolts2,   0.0f, 500000.0f);
      DDV_MinMaxFloat(pDX, m_mafvolts3,   0.0f, 500000.0f);
   //}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(genMAF, dataDialog)
   //{{AFX_MSG_MAP(genMAF)
      ON_BN_CLICKED(IDC_MG_SEC, OnTemp)
      ON_BN_CLICKED(IDC_GR_HR, OnTemp)
   //}}AFX_MSG_MAP
   ON_EN_CHANGE(IDC_AF1, OnEnChangeTemp1)
   ON_CBN_SELCHANGE(IDC_TABLE_TYPE, OnCbnSelchangeTableType)
   ON_EN_CHANGE(IDC_AF3, OnEnChangeTemp3)
   ON_EN_CHANGE(IDC_MAFVOLTS2, OnEnChangeResi2)
   ON_STN_CLICKED(IDC_TEMP_LABEL, OnStnClickedTempLabel)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

void genMAF::setLabel(bool unitsMAF)
{
   char label[128];
   // was sprintf(label, "Temperature (° %c)", unitsMAF ? 'C' : 'F'); could use for mg/sec or grams/hr
   sprintf(label, "Volts");
   m_tempLabel.SetWindowText(label);
}

//------------------------------------------------------------------------------

BOOL genMAF::OnInitDialog() 
{
   dataDialog::OnInitDialog("Table Generator");
   
   m_tableType.SetCurSel(0);
   m_tempSetting.SetCheck(true);
   setLabel(true);

   return TRUE;
}

//------------------------------------------------------------------------------

void genMAF::OnTemp() { setLabel(m_tempSetting.GetCheck() != 0);  }

//------------------------------------------------------------------------------

// double genMAF::k2f(double t)                  { return (t * 9.0/5.0) - 459.67; }
// double genMAF::Tk (double R) /* Kelvins    */ { return 1.0 / (A + B*log(R) + C*pow(log(R), 3)); }
// double genMAF::Tf (double R) /* Fahrenheit */ { return k2f(Tk(R)); }
// double genMAF::Tu (double f, bool unitsMAF)       { return unitsMAF ? ((f-32.0)*5.0/9.0) : f; }

void genMAF::OnOK() 
{
   GetDlgItem(IDOK    )->EnableWindow(FALSE);
   GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

   if (!mdb.getRuntime()) {
      msgOk("Table Generator", "Cannot connect to MS-II processor to send table.");
      dataDialog::OnOK();
      return;
   }

// Eric, "If you use the old MS-I include file format for the 
// MAF tables then you can use the msDatabase::readTable method 
// and not have to do much work at all."

//------------------------------------------------------------------------
//--  maftable.inc is of the form:                                      --
//--                                                                    --
//--    ^<whitespace>D[BW]<whitespace><number><any old junk>$           --
//--                                                                    --
//--  where character case is insignificant, and the form of numbers    --
//--  is derived from P&E 6808 assembly language.                       --

// Eric says, "Stick this in just above the writer code, see if you can make it 
// read a file:"

    int  nValues;
    WORD *value;
    mdb.read2DTable("maftable.inc", &value, nValues); // need .inc suffix?

// end Eric

   CWaitCursor wait;
   UpdateData();

   bool unitsMAF = m_tempSetting.GetCheck() == BST_CHECKED;

  // double t[3];
  // t[0] = m_af1;
  // t[1] = m_af2;
  //  t[2] = m_af3;
//   for (int i = 0; i < 3; i++) {
//      if (!unitsMAF) t[i] = (t[i]-32.0)*5.0/9.0;
//      t[i] += 273.15;
//   }

 //  double c11 = log(m_mafvolts1); double c12 = pow(c11, 3.0); double c13 = 1.0 / t[0];
 //  double c21 = log(m_mafvolts2); double c22 = pow(c21, 3.0); double c23 = 1.0 / t[1];
 //  double c31 = log(m_mafvolts3); double c32 = pow(c31, 3.0); double c33 = 1.0 / t[2];

 //  C = ((c23-c13) - (c33-c13)*(c21-c11)/(c31-c11)) / ((c22-c12) - (c32-c12)*(c21-c11)/(c31-c11));
 //  B = (c33-c13 - C*(c32-c12)) / (c31-c11);
 //  A = c13 - B*c11 - C*c12;

   // Send "t <type-byte> 1024 x 16-bit words"
   // ---------------------------------------
#define CTS 0
#define MAT 1
   BYTE tableType = m_tableType.GetCurSel(); // 0 = CTS (see MS-II main.c)
                                             // 1 = MAT
   BYTE table[2 + 2*1024];
   table[0] = 't';
   table[1] = tableType;

   // Open and print the maf_factor.maf file
   // --------------------------------------
   // need to delete first??

   FILE *f = fopen("maf_factor.adc", "a");
      fprintf(f, "//------------------------------------------------------------------------------\n");
      fprintf(f, "//--  Generated by MegaTune %-50s--\n", timeStamp());
      fprintf(f, "//--  This file merely records what was sent to your MS-II, and may be        --\n");
      fprintf(f, "//--  deleted at any time.                                                    --\n");
      fprintf(f, "//--                                                                          --\n");
//      fprintf(f, "//--  Type = %d (%s)  --\n", table[1], tableType=="MAF");
 
      fprintf(f, "//--  Type = MAF                                                              --\n");
  
//     fprintf(f, "//--  Bias = %7.1f                                                          --\n", m_biasValue);
      fprintf(f, "//--                                                                          --\n");
//      fprintf(f, "//--  Resistance      tInput             tComputed                            --\n");
//      fprintf(f, "//--  ------------    -----------------  ---------                            --\n");
//      fprintf(f, "//--  %8.1f ohm  %7.1fK (% 7.1f%c) %10.1f                            --\n", m_mafvolts1, t[0], m_af1, unitsMAF?'C':'F', Tu(Tf(m_mafvolts1), unitsMAF));
//      fprintf(f, "//--  %8.1f ohm  %7.1fK (% 7.1f%c) %10.1f                            --\n", m_mafvolts2, t[1], m_af2, unitsMAF?'C':'F', Tu(Tf(m_mafvolts2), unitsMAF));
//      fprintf(f, "//--  %8.1f ohm  %7.1fK (% 7.1f%c) %10.1f                            --\n", m_mafvolts3, t[2], m_af3, unitsMAF?'C':'F', Tu(Tf(m_mafvolts3), unitsMAF));
      fprintf(f, "//------------------------------------------------------------------------------\n");
      fprintf(f, "\n");
      fprintf(f, "#ifndef GCC_BUILD\n");
      fprintf(f, "#pragma ROM_VAR %s_ROM\n", tableType==CTS?"CLT":"MAT");
      fprintf(f, "#endif\n");
      fprintf(f, "const int maffactor_table[1024] EEPROM_ATTR  = {\n", tableType==CTS?"clt":"mat");
      fprintf(f, "          //  ADC    Volts    AirFlow \n");

      BYTE *b  = table+2;
      for (int adcCount = 0; adcCount < 1024; adcCount++) {
 //        double res  = m_biasValue / (1023.0/double(adcCount==0?0.01:adcCount) - 1.0);
         double volts  = 24 / (1023.0/double(adcCount==0?0.01:adcCount) - 1.0);
         // double airflow = Tf(volts);
         double airflow = 0;
 //        if      (airflow <  -40.0) airflow = tableType == CTS ? 180.0 : 70.0;
 //        else if (airflow >  350.0) airflow = tableType == CTS ? 180.0 : 70.0;
         short tt = short(airflow*1);
//         fprintf(f, "   %5d%c // %4d %7.2f  %7.1f %9.1f\n", tt, adcCount<1024-1?',':' ', adcCount, 5.0*adcCount/1023.0, Tu(tt/10.0, unitsMAF), volts);
         fprintf(f, "   %5d%c // %4d %7.2f  %7.1f \n", tt, adcCount<1024-1?',':' ', adcCount, 5.0*adcCount/1023.0);
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

// Write the value to MS-II
// ------------------------
// The cDesc (controller descriptor) object manages all the IO 
// from the connected device, and the tblCreate variable is 
// always sent just prior to the loop which updates the GUI and 
// sends the table two bytes at a time.



   BYTE tblCreate[2] = {'t', '\003' }; // 003 is maffactor, 002 is egofactor, 001 is matfactor, 000 is cltfactor
   mdb.cDesc.write(tblCreate, 2);
   ::Sleep(100);

// Then change the loop to use the nValues instead of "sizeof" operator.
//    for (i = 0; i < nValues; i++) {
//       ...
//       mdb.cDesc.write(value+i, 2);
//       ::Sleep(4);
//    }
//
//    delete [] value;
   
   for (int i = 2; i < sizeof(table); i += 2) {
      char msg[50];
      sprintf(msg, "Downloading %4d of %4d entries", i/2, sizeof(table)/2-1);
      status->SetWindowText(msg);

      mdb.cDesc.write(table+i, 2);
      
      // I'm not sure if the 4 millisecond sleep is required inside the loop, 
      // but it seemed to help with robustness early on so I never got 
      // back to seeing if we could live without it.
      ::Sleep(4);
   }

// Eric says, "Use the audit log to write debug print output, 
// since you can't use the standard output 
// (printf goes to the bit bucket on windows, 
// so do stuff like this when you need to write things:
//    cfgLog("Write a number %d", 1);"

   dataDialog::OnOK();
}

//------------------------------------------------------------------------------

void genMAF::OnEnChangeTemp1()
{
   // TODO:  If this is a RICHEDIT control, the control will not
   // send this notification unless you override the dataDialog::OnInitDialog()
   // function and call CRichEditCtrl().SetEventMask()
   // with the ENM_CHANGE flag ORed into the mask.

   // TODO:  Add your control notification handler code here
}

void genMAF::OnCbnSelchangeTableType()
{
   // TODO: Add your control notification handler code here
}

void genMAF::OnEnChangeTemp3()
{
   // TODO:  If this is a RICHEDIT control, the control will not
   // send this notification unless you override the dataDialog::OnInitDialog()
   // function and call CRichEditCtrl().SetEventMask()
   // with the ENM_CHANGE flag ORed into the mask.

   // TODO:  Add your control notification handler code here
}

void genMAF::OnEnChangeResi2()
{
   // TODO:  If this is a RICHEDIT control, the control will not
   // send this notification unless you override the dataDialog::OnInitDialog()
   // function and call CRichEditCtrl().SetEventMask()
   // with the ENM_CHANGE flag ORed into the mask.

   // TODO:  Add your control notification handler code here
}

void genMAF::OnStnClickedTempLabel()
{
   // TODO: Add your control notification handler code here
}
