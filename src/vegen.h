//------------------------------------------------------------------------------
//--  Copyright (c) 2004 by Lance Gardiner, All Rights Reserved.              --
//--                                                                          --
//--  This program is free software; you can redistribute it and/or           --
//--  modify it under the terms of the GNU General Public License             --
//--  as published by the Free Software Foundation; either version            --
//--  2 of the License, or (at your option) any later version.                --
//--  See http://www.gnu.org/licenses/gpl.txt                                 --
//------------------------------------------------------------------------------

#ifndef VEGEN_H
#define VEGEN_H 1

//------------------------------------------------------------------------------

#include "Vex.h"

class veGen {
   vex      VEX; // The VEX object containing the tableEditor that we
                 // are generating.
   veTable &VE;

   double minrpm;       // RPM, idle RPM or lower, used to set lower bound on RPM bins
   double ptqrpm;       // RPM, at which torque peak occurs
   double phprpm;       // RPM, at which HP peak occurs
   double maxrpm;       // RPM, redline RPM or higher, used to set upper bound on RPM bins

   double minmap;       // kPa, lower bound for MAP bins
   double ptqmap;       // kPa, MAP at which torque peak occurs
   double phpmap;       // kPa, MAP at which HP peak occurs
   double maxmap;       // kPa, upper bound for MAP bins

   double peak_tq;      // lb-ft
   double peak_hp;      // HP
   double displacement; // in^3

   // naMAPi is the index of the row with the value nearest 100kPa (i.e.,
   // the 'naturally aspirated' row) and is it determined in function
   // genMAPbins - set but not used in this code version.
   int    naMAPi;

   double a, b, c;
   void computeCoefficients(double x1, double y1, double x2, double y2);

   void genRPMbins();
   void genMAPbins();
   void genVEtable();

public:
   veGen(int nRPM, int nMAP, int RPMi=740, int RPMm=6000, int MAPi=35, int MAPm=250)
    : VEX      (vex()),
      VE       (*VEX.newPage(0)),
      minrpm   (RPMi),
      maxrpm   (RPMm),
      minmap   (MAPi),
      maxmap   (MAPm),

      peak_tq( 166),
      ptqrpm (3000),
      ptqmap ( 160),

      peak_hp( 158),
      phprpm (5500),
      phpmap ( 123), // Unused

      displacement(136)
   {
      VE.setSize(nRPM, nMAP);
   }

   void setSize(int nRPM, int nMAP)
   {
      VE.setSize(nRPM, nMAP);
   }

   void setComment(const char *c)
   {
      VEX.userComment(c);
   }

   void setPeakTorque(double tq, double tqr, double tqm)
   {
      peak_tq = tq;
      ptqrpm  = tqr;
      ptqmap  = tqm;
   }

   void setPeakHP(double hp, double hpr, double hpm)
   {
      peak_hp = hp;
      phprpm  = hpr;
      phpmap  = hpm;
   }

   void setDisplacement(double d)
   {
      displacement = d;
   }

   void generate()
   {
      genRPMbins();
      genMAPbins();
      genVEtable();
   }

   double rpm (int idx)      { return VE.rpm (idx); }
   double load(int idx)      { return VE.load(idx); }
   double ve  (int R, int M) { return VE.ve  (R, M); }
   bool checkInputs();
   void dump() {
      static int i = 0;
      CString fName;
      fName.Format("vegen%03d.vex", i++);
      VEX.write(fName);
   }
};

//------------------------------------------------------------------------------
#endif
