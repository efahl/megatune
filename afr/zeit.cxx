#include <stdio.h>
#include <math.h>

typedef unsigned char BYTE;

#define nADC 256

static double zeitronixBv[] = { 0.000, 0.150, 0.310, 0.460,  0.620,  0.780,  0.930,  1.090,  1.240,  1.400,  1.560,  1.710,  1.870,  2.020,  2.180,  2.340,  2.500,  2.650,  2.800,  2.960,  3.000,  3.120,  3.270,  5.010  };
static double zeitronixBa[] = { 0.000, 9.700, 9.900, 10.100, 10.300, 10.500, 10.700, 11.000, 11.400, 11.700, 12.100, 12.400, 12.800, 13.200, 13.700, 14.200, 14.700, 15.600, 16.900, 18.500, 18.800, 19.900, 21.200, 99.000 };

#define USE_TABLE(prefix) \
   Bv = prefix ## Bv; \
   Ba = prefix ## Ba; \
   nB = (sizeof(prefix ## Bv) / sizeof(double));

static double inno12Fv (int adc) { return        adc * 50.0 / (nADC-1.0); }
static double inno05Fv (int adc) { return 10.0 + adc * 10.0 / (nADC-1.0); }
static double teWBlinFv(int adc) { return  9.0 + adc * 10.0 / (nADC-1.0); }

#define USE_FUNC(prefix) \
   Fv = prefix ## Fv

int main()
{
   double  *Bv;
   double  *Ba;
   int      nB = 0;
   double (*Fv)(int adc) = NULL;

   USE_TABLE(zeitronix);  // Zeitronix

   BYTE table[nADC];

   FILE *f = fopen("afr.tbl", "w");
      int iV = 0;
      double afr;
      for (int adcCount = 0; adcCount < nADC; adcCount++) {
         double voltage = adcCount / (nADC-1.0) * 5.0;

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

//; Generated using:
//;    DIY WB Vout for Gasoline
//;    Robert Rauscher July 2001
//        DW      742     ;   0  0.00
//        DW      746     ;   1  0.02
         table[adcCount] = BYTE(afr*10.0+0.5);
         fprintf(f, "   DW  %4.0f  ; %4d  %6.3f  %6.3f\n", afr*100.0, adcCount, afr, voltage);
      }
      fprintf(f, "\n");
   fclose(f);

#if 0
   CStatic *status = static_cast<CStatic *>(GetDlgItem(IDC_DOWNLOAD_STATUS));
   BYTE tblCreate[2] = {'t', '\002' };
   mdb.cDesc.write(tblCreate, 2);
   ::Sleep(100);
   for (int i = 2; i < sizeof(table); i += 2) {
      char msg[50];
      sprintf(msg, "Downloading %4d of %4d entries", i, sizeof(table));
      status->SetWindowText(msg);

      mdb.cDesc.write(table+i, 2);
      ::Sleep(4);
   }

   dataDialog::OnOK();
#endif
   return 0;
}
