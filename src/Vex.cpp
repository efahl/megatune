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
//==============================================================================

//#ifdef WIN32
#   include "stdafx.h"
//#endif

#include "Vex.h"
#include <stdlib.h>
#include <string.h>

static char *rcsId = "$Id$";

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

#ifdef VEX_TEST
#  define cfgOpen  fopen
#  define cfgClose fclose
#else
   FILE *cfgOpen (const char *fileName, const char *mode);
   int   cfgClose(FILE *file);
#endif

//==============================================================================

double  veTable::load(int index)
{
   return (index >= 0 && index < nLoad) ? loadBins[index] : 0;
}

//------------------------------------------------------------------------------

void veTable::load(double value, int index)
{
   if (index >= 0 && index < nLoad) loadBins[index] = value;
}

//------------------------------------------------------------------------------

double  veTable::rpm (int index)
{
   return (index >= 0 && index < nRPM) ? rpmBins[index] : 0;
}

//------------------------------------------------------------------------------

void veTable::rpm (double value, int index)
{
   if (index >= 0 && index < nRPM) rpmBins[index] = value;
}

//------------------------------------------------------------------------------

double  veTable::ve  (int r, int l)
{
   return (r >= 0 && r < nRPM && l >= 0 && l < nLoad) ? veBins[l*nRPM+r] : 0;
}

//------------------------------------------------------------------------------

void veTable::ve  (double value, int r, int l)
{
   if (r >= 0 && r < nRPM && l >= 0 && l < nLoad) veBins[l*nRPM+r] = value;
}

//------------------------------------------------------------------------------

void veTable::loadType(char *type)
{
   strcpy(loadtype, type);
}

char *veTable::loadType()
{
   return loadtype;
}

//------------------------------------------------------------------------------

void veTable::cleanUp()
{
   if (rpmBins ) { delete [] rpmBins;  rpmBins  = NULL; }
   if (loadBins) { delete [] loadBins; loadBins = NULL; }
   if (veBins  ) { delete [] veBins;   veBins   = NULL; }
}

//------------------------------------------------------------------------------

bool veTable::setSize(int NRPM, int NLoad, double RpmBins[], double LoadBins[], double VeBins[])
{
   cleanUp();

   nRPM     = NRPM;
   rpmBins  = new double [nRPM];
   if (rpmBins == NULL) return false;
   for (int iR = 0; iR < nRPM;  iR++) rpmBins[iR]  = RpmBins  ? RpmBins[iR]  : iR;

   nLoad    = NLoad;
   loadBins = new double [nLoad];
   if (loadBins == NULL) return false;
   for (int iL = 0; iL < nLoad; iL++) loadBins[iL] = LoadBins ? LoadBins[iL] : iL;

   veBins   = new double [nRPM*nLoad];
   if (veBins == NULL) return false;
   for (int iV = 0; iV < nRPM*nLoad; iV++) veBins[iV] = VeBins ? VeBins[iV]  : iV;

   return true;
}

//------------------------------------------------------------------------------

static double interp(double x, double x0, double y0, double x1, double y1)
{
   return y0 + (x-x0) / (x1-x0) * (y1-y0);
}

static double round(double x, double roundTo)
{
   return int((x+(roundTo/2))/roundTo)*roundTo;
}

//------------------------------------------------------------------------------

double *veTable::interpRow(double *Y, int oldN, int newN, double roundTo)
{
   int     newI = 0;
   double *newY = new double[newN];
   for (int i = 0; i < newN; i++) {
      double newX = double(i)/(newN-1) * (oldN-1);
      while (newX > newI) newI++;
      newY[i] = newI == 0 ? Y[0] : round(interp(newX, newI-1, Y[newI-1], newI, Y[newI]), roundTo);
   }
   return newY;
}

//------------------------------------------------------------------------------

double veTable::interpolate(double rpm, double load)
{
   if (load < loadBins[0]      ) load = loadBins[0];
   if (load > loadBins[nLoad-1]) load = loadBins[nLoad-1];
   if (rpm  < rpmBins[0]       ) rpm  = rpmBins[0];
   if (rpm  > rpmBins[nRPM-1]  ) rpm  = rpmBins[nRPM-1];

   int   irpm;
   double rpm1, rpm2;
   for (irpm = 0; irpm < nRPM-1; irpm++) {
      if (rpm <= rpmBins[irpm+1]) {
         rpm1 = rpmBins[irpm];
         rpm2 = rpmBins[irpm+1];
         break;
      }
   }

   int   iload;
   double load1, load2;
   for (iload = 0; iload < nLoad-1; iload++) {
      if (load <= loadBins[iload+1]) {
         load1 = loadBins[iload  ];
         load2 = loadBins[iload+1];
         break;
      }
   }

   double ve11 = ve(irpm+0, iload+0);
   double ve21 = ve(irpm+1, iload+0);
   double ve12 = ve(irpm+0, iload+1);
   double ve22 = ve(irpm+1, iload+1);

   double loMapVE = interp(rpm, rpm1, ve11, rpm2, ve21);
   double hiMapVE = interp(rpm, rpm1, ve12, rpm2, ve22);
   return interp(load, load1, loMapVE, load2, hiMapVE);
}

//------------------------------------------------------------------------------

void veTable::resize(int newNRPM, int newNLOD, double roundRPM, double roundLOD, double roundVE)
{
   double *newRPM = interpRow(rpmBins,  nRPM,  newNRPM, roundRPM);
   double *newLOD = interpRow(loadBins, nLoad, newNLOD, roundLOD);
   double *newVE  = new double[newNRPM * newNLOD];
   for (int r = 0; r < newNRPM; r++) {
      for (int l = 0; l < newNLOD; l++) {
         newVE[l*newNRPM+r] = round(interpolate(newRPM[r], newLOD[l]), roundVE);
      }
   }
   setSize(newNRPM, newNLOD, newRPM, newLOD, newVE);
}

//------------------------------------------------------------------------------

veTable::veTable(int NRPM, int NLoad, double RpmBins[], double LoadBins[], double VeBins[])
 : rpmBins (NULL),
   loadBins(NULL),
   veBins  (NULL),
   _scale  (1.0),
   _trans  (0.0)
{
   loadType("MAP");
   setSize(NRPM, NLoad, RpmBins, LoadBins, VeBins);
}

//------------------------------------------------------------------------------

veTable::~veTable()
{
   cleanUp();
}

//==============================================================================

vex::errorStatus vex::closeFile(errorStatus status)
{
   if (cfgClose(vexFile) != 0 && status != noError) status = closeError;
   vexFile = NULL;
   return status;
}

//------------------------------------------------------------------------------
//--  Reader                                                                  --

static char *bracketedNumber(char *s, double &n)
{
   s = strchr(s, '[');
   if (s == NULL) return NULL;
   n = strtod(s+1, &s);
   while (strchr(" \t", *s)) s++;
   if (*s != ']') return NULL;
   return s+1;
}

static char *bracketedNumber(char *s, int &n)
{
   s = strchr(s, '[');
   if (s == NULL) return NULL;
   n = strtol(s+1, &s, 10);
   while (strchr(" \t", *s)) s++;
   if (*s != ']') return NULL;
   return s+1;
}

static char *prefix(char *str, char *matches)
{
   int l = strlen(matches); // Do it once.
   return strncmp(str, matches, l) == 0 ? str+l : NULL;
}

bool vex::getLine()
{
   lineBuffer[0] = '\0';
   while (fgets(lineBuffer, sizeof(lineBuffer)-1, vexFile)) {
      char *comment = strchr(lineBuffer, '#');
      if (comment) *comment = '\0';
      if (lineBuffer[0] == '\0') continue; // Ignore empty lines.
      char *eol = strchr(lineBuffer, '\n');
      if (eol) *eol = '\0';
      break;
   }
   return lineBuffer[0] != '\0';
}

vex::errorStatus vex::readTable(double *table, int rows, int cols)
{
   for (int r = 0; r < rows; r++) {
      if (!getLine()) return readError;
      int rVal;
      char *s = bracketedNumber(lineBuffer, rVal);
      if (!s) return missingBracketError;
      if (rVal != r) return rowIndexError; // Row index in file is not in order.

      s = strchr(s, '='); // Skip the separator between index and values.
      if (!s) return missingEqError;
      s++;

      for (int c = 0; c < cols; c++) {
         table[r*cols+c] = strtod(s, &s);
      }
   }
   return noError;
}

#pragma warning(push)
#pragma warning(disable:4706)

vex::errorStatus vex::read(const char *fileName)
{
   vexFile = cfgOpen(fileName, "r");
   if (vexFile == NULL) return openError;
   if (!fgets(lineBuffer, sizeof(lineBuffer)-1, vexFile)) return closeFile(readError);
   if (!prefix(lineBuffer, "EVEME 1.0")) return closeFile(nonVexError);

   int     nR = 0;
   int     nL = 0;
   double  *r = NULL; // ??? memory leak on error
   double  *l = NULL;
   double  *v = NULL;
   veTable *p = NULL;
   char     loadType[10];
   char    *tail;

   while (getLine()) {
      if ((tail = prefix(lineBuffer,  "UserRev:"))) {
         UserRev = strtod(tail, NULL) + 0.01;
      }
      else if ((tail = prefix(lineBuffer, "UserComment: "))) {
         userComment(tail);
      }
      else if ((tail = prefix(lineBuffer, "Date: "))) {
         date(tail);
      }
      else if ((tail = prefix(lineBuffer, "Time: "))) {
         time(tail);
      }

      else if ((tail = prefix(lineBuffer, "Page"))) {
         p = newPage(strtol(tail, NULL, 10));
      }
      else if ((tail = prefix(lineBuffer, "Scale"))) {
         if (p) p->setScale(strtod(tail, NULL));
      }
      else if ((tail = prefix(lineBuffer, "Translate"))) {
         if (p) p->setTranslate(strtod(tail, NULL));
      }


      else if ((tail = prefix(lineBuffer, "VE Table RPM Range"))) {
         if (!bracketedNumber(tail, nR)) return closeFile(missingBracketError);
         r = new double[nR];
         errorStatus s = readTable(r, nR, 1);

         if (s != noError) return closeFile(s);
      }

      else if ((tail = prefix(lineBuffer, "VE Table Load Range"))) {
         char *lt = strchr(tail, '(');
         if (lt == NULL) return closeFile(missingLeftParenError);
         lt++;
         tail = strchr(lt, ')');
         if (tail == NULL) return closeFile(missingRightParenError);
         *tail = '\0';
         strcpy(loadType, lt);
         tail++;

         if (!bracketedNumber(tail, nL)) return closeFile(missingBracketError);
         l = new double[nL];
         errorStatus s = readTable(l, nL, 1);

         if (s != noError) return closeFile(s);
      }

      else if ((tail = prefix(lineBuffer, "VE Table"))) {
         if (nR == 0 || nL == 0) return closeFile(tableSizeError);

         // ??? parse and verify column header line.
         // 1.0 spec only has "VE Table" on header line, need to enhance it to
         // contain the shape, too.
         getLine();

         v = new double[nR*nL];

         errorStatus s = readTable(v, nL, nR); // Load in rows, RPM in columns.

         if (s != noError) return closeFile(s);
      }

      else {
         // ??? Error of some sort, garbage in the file.
      }

      //------------------------------------------------------------------------
      //--  Time to stash it                                                  --
      if (r && l && v) {
         if (!p) p = newPage(0);
         p->loadType(loadType);
         p->setSize(nR, nL, r, l, v);
         delete [] r; r = NULL;
         delete [] l; l = NULL;
         delete [] v; v = NULL;
         p = NULL;
      }
   }

   if (r) delete [] r;
   if (l) delete [] l;
   if (v) delete [] v;

   return closeFile(noError);
}

#pragma warning(pop)

//------------------------------------------------------------------------------
//--  Writer                                                                  --

vex::errorStatus vex::write(veTable &p)
{
   if (p.scale()     != 1.0) fprintf(vexFile, "Scale %f\n", p.scale());
   if (p.translate() != 0.0) fprintf(vexFile, "Translate %f\n", p.translate());

   int nR = p.nRPMs();
   int nL = p.nLoads();
   fprintf(vexFile, "VE Table RPM Range              [%2d]\n", nR);
   int iR;
   for (iR = 0; iR < nR; iR++) fprintf(vexFile, "   [%3d] = %3.0f\n", iR, p.rpm(iR));
   fprintf(vexFile, "VE Table Load Range (%s)       [%2d]\n", p.loadType(), nL);
   int iL;
   for (iL = 0; iL < nL; iL++) fprintf(vexFile, "   [%3d] = %3.0f\n", iL, p.load(iL));
   fprintf(vexFile, "VE Table                        [%3d][%3d]\n", nR, nL);
   fprintf(vexFile, "          ");
   for (iR = 0; iR < nR; iR++) fprintf(vexFile, " [%3d]", iR);
   fprintf(vexFile, "\n");
   for (iL = 0; iL < nL; iL++) {
      fprintf(vexFile, "   [%3d] =", iL);
      for (iR = 0; iR < nR; iR++) {
         fprintf(vexFile, "%5.0f%s", p.ve(iR, iL), iR < nR-1 ? " " : "\n");
      }
   }
   return noError;
}

vex::errorStatus vex::write(const char *fileName)
{
   vexFile = fopen(fileName, "w");
   if (vexFile == NULL) return openError;

   fprintf(vexFile, "EVEME 1.0\n");

   const time_t t = ::time(NULL);
   struct tm *now = localtime(&t);
   strftime(Date, sizeof(Date), "%m-%d-%Y", now);
   strftime(Time, sizeof(Time), "%H:%M", now);

   fprintf(vexFile, "UserRev: %0.2f\n", UserRev);
   fprintf(vexFile, "UserComment: %s\n", UserComment);
   fprintf(vexFile, "Date: %s\n", Date);
   fprintf(vexFile, "Time: %s\n", Time);

   int cPage = 0;
   for (int iPage = 0; iPage < nPages; iPage++) {
      veTable *p = page(iPage);
      if (p) {
         fprintf(vexFile, "Page %d\n", cPage);
         cPage++;
         errorStatus s = write(*p);
         if (s != noError) return closeFile(s);
      }
   }
   return closeFile(noError);
}

//------------------------------------------------------------------------------
//--  Page management                                                         --

veTable *vex::page(int pageNo)
{
   if (pageNo <  0)      return NULL;
   if (pageNo >= nPages) return NULL;
   return pages[pageNo];
}

//------------------------------------------------------------------------------

veTable *vex::newPage(int pageNo)
{
   if (pageNo < 0) return false;
   if (pageNo >= nPages) {
      veTable **newPages = new veTable*[pageNo+1];
      if (newPages == NULL) return false;

      for (int i = 0; i < pageNo; i++) {
         newPages[i] = i >= nPages ? NULL : pages[i];
      }

      nPages = pageNo+1;
      if (pages) delete [] pages;
      pages = newPages;
   }

   return pages[pageNo] = new veTable;
}

//------------------------------------------------------------------------------

vex::vex()
 : pages (NULL),
   nPages(-1)
{
   userRev(1.0);
   userComment("");
   date("");
   time("");
}

//------------------------------------------------------------------------------

vex::~vex()
{
   if (pages) {
      for (int i = 0; i < nPages; i++) if (pages[i]) delete pages[i];
      delete [] pages;
   }
}

//------------------------------------------------------------------------------

const char *vex::errorMsg(errorStatus s)
{
   switch (s) {
      case noError:                return "No error detected";
      case openError:              return "File opening error";
      case readError:              return "File read error";
      case writeError:             return "File write error";
      case closeError:             return "file closing error";
      case nonVexError:            return "Missing or corrupt EVEME header";
      case missingBracketError:    return "Missing brackets on number";
      case missingLeftParenError:  return "Missing left parenthesis";
      case missingRightParenError: return "missing right parenthesis";
      case tableSizeError:         return "Table dimensions incorrect";
      case rowIndexError:          return "Row index out of order";
      case missingEqError:         return "Missing equal sign";
      default:                     return "Unknown VEX error code";
   }
}

//==============================================================================

#ifdef VEX_TEST
int main(int argc, char *argv[])
{
   vex v;
   v.read("megasquirt.vex");

   veTable &p1 = *v.page(0);
   p1.resize(12, 12, 1.0, 5.0, 1.0);
   v.write("ms12.vex");

   for (int r = 0; r < p1.nRPMs(); r++) p1.ve(2*r, r, 2);

   v.write("boink.vex");
   exit(0);
   return 0;
}
#endif
