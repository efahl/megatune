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
#include <Mmsystem.h>
#include <io.h>
#include <direct.h>
#include <sys/stat.h>
#include "megatune.h"
#include "msDatabase.h"
#include "userMenu.h"

static char *rcsId = "$Id$";

#ifdef _DEBUG
#  define new DEBUG_NEW
#  undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

extern userMenuList      uml;
extern userIndicatorList uil;
extern datalogOptions    lop;

extern CString installationDir; // Defined in megatune.cpp
extern CString commonDir;
extern CString workingDir;

//------------------------------------------------------------------------------

static const char *cwd()
{
   static char _dir[_MAX_PATH];
   if (_getcwd(_dir, sizeof(_dir)) == NULL) {
      strcpy(_dir, ".");
   }
   return _dir;
}

//------------------------------------------------------------------------------

CString msDatabase::settingsFile = "";

msDatabase::thermType msDatabase::therm = msDatabase::Fahrenheit;

double msDatabase::tempFromDb(double t)
{
   if (therm == Celsius) {
      t = (t-32.0) * 5.0/9.0;
   }
   return t;
}

//double msDatabase::dbFromTemp(double t)
//{
//   if (therm == Celsius) {
//      t = t * 9.0/5.0 + 32.0;
//   }
//   return t;
//}

void msDatabase::fixThermoLabel(CStatic &l, char *fmt, double value)
{
   char b[128];
   if (fmt)
      sprintf(b, fmt, tempFromDb(value));
   else
      l.GetWindowText(b,128);
   char t = therm == Fahrenheit ? 'F' : 'C';
   int  n = strlen(b)-1; // What a hack...
   if (b[n] == ')') n--;
   b[n] = t;
   l.SetWindowText(b);
}

char *msDatabase::thermLabel()
{
   static char label[20];
   sprintf(label, "° %c", therm == Fahrenheit ? 'F' : 'C');
   return label;
}

//------------------------------------------------------------------------------

bool msDatabase::rawTPS = false;

//------------------------------------------------------------------------------

static bool _quoteDump = false;

msDatabase::msDatabase()
 : controllerResetCount(0),
   userTimerInt   (  200),
   timerInterval  (  200),
   bursting       (false),
   _pageNo        (    0),
   _loaded        (false),
   _burned        (true ), // Until we send something out.
   io             (msComm(1, 9600)),
   log            (NULL)
 , cDesc          (io)
{
#ifdef _DEBUG
   if (_heapchk() != _HEAPOK) {
      msgOk("Heap error", "MT Heap Error"__FILE__" %d", __LINE__);
   }
#endif

   commPortNumber = io.port();
   commPortRate   = io.rate();
   burstCommPort  = commPortNumber;
   burstCommRate  = 115200;

   signature[0] = 0;
}

bool msDatabase::init()
{
#ifdef _DEBUG
   if (_heapchk() != _HEAPOK) {
      msgOk("Heap error", "MT Heap Error"__FILE__" %d", __LINE__);
   }
#endif

   // for (int i = 0; i < Rlimit; i++) userVidx[i] = i; for when it's fully dynamic.
   Uundefined      = cDesc.varIndex(S_UNDEFINED);

   // This is a friggin' hack for 2.20 to allow
   // user-defined channels, but will be replaced
   // by the full expression language someday...
   UegoVoltage     = cDesc.varIndex(S_egoVoltage);
   UveTuneLodIdx   = cDesc.varIndex(S_veTuneLodIdx);
   UveTuneRpmIdx   = cDesc.varIndex(S_veTuneRpmIdx);
   UveTuneValue    = cDesc.varIndex(S_veTuneValue);

   cDesc._userVar[UveTuneLodIdx] = 0; // Defaults until the tuning dialog is displayed.
   cDesc._userVar[UveTuneRpmIdx] = 0;
   cDesc._userVar[UveTuneValue]  = 0;

   wwuX[0] = -40.0;
   wwuX[1] = -20.0;
   wwuX[2] =   0.0;
   wwuX[3] =  20.0;
   wwuX[4] =  40.0;
   wwuX[5] =  60.0;
   wwuX[6] =  80.0;
   wwuX[7] = 100.0;
   wwuX[8] = 130.0;
   wwuX[9] = 160.0;

   bool status = readConfig();
   load(); // Attempt to grab data from controller.

#ifdef _DEBUG
   if (_heapchk() != _HEAPOK) {
      msgOk("Heap error", "MT Heap Error"__FILE__" %d", __LINE__);
   }
#endif
   return status;
}

msDatabase::~msDatabase()
{
   if (log   ) delete    log;
   if (pBytes) delete [] pBytes;
}

//------------------------------------------------------------------------------

typedef std::map<FILE *, CString> fileMap;
typedef fileMap::iterator         fileMapIterator;

static fileMap  openFiles;
static FILE    *auditLog = NULL;

static void auditOpen()
{
   if (auditLog == NULL) {
      auditLog = fopen("audit.log", "w");
      fprintf(auditLog, "MegaTune - Startup file audit log\n");
      fprintf(auditLog, "Install directory: %s\n", installationDir);
      fprintf(auditLog, "Working directory: %s\n\n", workingDir);
   }
}

void cfgLog(const char *Format, ...)
{
   auditOpen();

   va_list Args;
   va_start(Args, Format);
      char Output[2048];
      vsprintf(Output, Format, Args);
      fprintf(auditLog, "%s\n", Output);
      fflush(auditLog);
   va_end(Args);
}

FILE *cfgOpen(const char *fileName, const char *mode)
{
   auditOpen();

   // The search path is:
   //    1) ./mtCfg/file
   //    2) ./file
   //    3) ../mtCommon/file
   //    4) c:/Program Files/MegaSquirt/MegaTuneN.NN/mtCfg/file

   CString configDir;

   // 0) Check for "x:", if so just try the raw and see if it
   //    works.
   FILE *file = NULL;
   if (fileName[1] == ':') {
      file = fopen(fileName, mode);
      fprintf(auditLog, "%s %s\n   %c %s\n", mode, fileName, file ? '+':'-', fileName);
   }

   // 1) If mode is "w", then this is expected to succeed.
   if (file == NULL) {
      configDir = workingDir+"/mtCfg/"+fileName;
      file = fopen(configDir, mode);
      fprintf(auditLog, "%s %s\n   %c %s\n", mode, fileName, file ? '+':'-', configDir);
   }

   if (file == NULL) {
      // 2) For backward compatibility.  Ditch this case?
      configDir = workingDir+"/"+fileName;
      file = fopen(configDir, mode);
      fprintf(auditLog, "   %c %s\n", file ? '+':'-', configDir);
   }

   if (file == NULL) {
      // 3) If no custom version supercedes this one, look in common.
      configDir = commonDir+"/"+fileName; 
      file = fopen(configDir, mode);
      fprintf(auditLog, "   %c %s\n", file ? '+':'-', configDir);
   }

   if (file == NULL) {
      // 4) If no common file was found, look in defaults.
      configDir = installationDir+"/mtCfg/"+fileName; 
      file = fopen(configDir, mode);
      fprintf(auditLog, "   %c %s\n", file ? '+':'-', configDir);
   }
   fflush(auditLog);

   openFiles[file] = fileName;

   return file;
}

int cfgClose(FILE *file)
{
   fileMapIterator f = openFiles.find(file);
   if (f == openFiles.end())
      cfgLog("Unknown File:Close");
   else {
      cfgLog("%s:Close", (*f).second);
      openFiles.erase(f);
   }
   return fclose(file);
}

//------------------------------------------------------------------------------
// There are three input messages handled here.
//
//    Signature (response to the "S" request);
//    VE/constants tables (response to the "V" request);
//    Run-time variables (response to the "A" request).
//
// The signature is arbitrary length, you must check it
// on input.  The VE/constants table is 128 bytes long and
// the run-time variables are 22 bytes.
//------------------------------------------------------------------------------

bool msDatabase::number(char *s, long &n)
{
   n = 0;

   // Prefixes: % = binary, $ = hexadecimal, ! = decimal.
   // Suffixes: Q = binary, H = hexadecimal, T = decimal.

   int  base = 10;
   char ch   = s[0];

   switch (ch) {
      case '%': base =  2; s[0] = ' '; break;
      case '$': base = 16; s[0] = ' '; break;
      case '!': base = 10; s[0] = ' '; break;

      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
      case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
         ch = s[strlen(s)-1];
         switch (ch) {
            case 'Q': case 'q': base =  2; break;
            case 'H': case 'h': base = 16; break;
            case 'T': case 't': base = 10; break;

            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
               // Everthing is ok so far.
               break;

            default:
               return false;
               break;
         }
         break;
      default:
         return false;
         break;
   }

   n = strtol(s, NULL, base);
   return true;
}

bool msDatabase::number(char *s, WORD &n)
{
   long nn;
   if (!number(s, nn))
      return false;
   else {
      if (nn > 65536 || nn < 0) return false;
      n = static_cast<WORD>(nn);
   }
   return true;
}

bool msDatabase::number(char *s, BYTE &n)
{
   long nn;
   if (!number(s, nn))
      return false;
   else {
      if (nn > 255 || nn < 0) return false;
      n = static_cast<BYTE>(nn);
   }
   return true;
}

bool msDatabase::charIn(char c, char *set)
{
   return strchr(set, c) != NULL;
}

//------------------------------------------------------------------------------

#define WHITESPACE " \t"

void msDatabase::readTable(const char *fileName, WORD **values, int &nValues)
{
   FILE   *tableFile = cfgOpen(fileName, "r");

   if (tableFile == NULL) {
      nValues = -1;
      char msg[128];
      sprintf(msg,
         "Configuration file %s not found.\n\n"
         "Function will return zeros.", fileName);
      MessageBox(NULL, msg, "MegaTune Table File Reader", MB_OK);   
   }
   else {
      nValues = 0;
      char lineBuffer[512];

      //------------------------------------------------------------------------
      //--  Parses lines of the form:                                         --
      //--                                                                    --
      //--    ^<whitespace>D[BW]<whitespace><number><any old junk>$              --
      //--                                                                    --
      //--  where character case is insignificant, and the form of numbers    --
      //--  is derived from P&E 6808 assembly language.                       --

      int tableSize = 256;
      *values = new WORD[tableSize];
      while (fgets(lineBuffer, sizeof(lineBuffer)-1, tableFile)) {
         char *s = lineBuffer;

         if (!charIn(*s, WHITESPACE)) continue;
         while (charIn(*s, WHITESPACE)) s++;        // Skip leading white space.
         if (!charIn(*s, "dD"      )) continue; else s++;
         if (!charIn(*s, "bBwW"    )) continue; else s++;
         if (!charIn(*s, WHITESPACE)) continue;
         while (charIn(*s, WHITESPACE)) s++;

         char *last = s;               // Null terminate the string of interest.
         while (charIn(*last, "0123456789$%!QHTqht")) last++;
         *last = '\0';

         if (nValues >= tableSize+1) {
            WORD *oldV = *values;
            tableSize *= 4;
            *values = new WORD[tableSize];
            memcpy(*values, oldV, sizeof(WORD)*nValues);
            delete [] oldV;
         }

         if (!number(s, (*values)[nValues++])) {
            break;
         }
      }

      cfgClose(tableFile);
   }
}

void msDatabase::read2DTable(const char *fileName, WORD **values, int &nValues)
{  
   // 'borrowed' from VEX import
   //CFileDialog dlgr(TRUE, "maf", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, );
   //dlgr.m_ofn.lpstrInitialDir = cwd();
   //dlgr.m_ofn.lpstrTitle      = " MAF Table Import";

   // This was originally for reading a 1D x 1024 ADC table (readTable)
   // We need a 2D x nPoints Table for MAF input (read2DTable) for 2.8 code
   // Table becomes 1D, with nVlaues = 2x data lines
   //  - Odd number line entries are Volts, 
   //  - Even number lines entries are Air Flow
   // Read one set of points from each line
   // (eventiually expnd to an n-D table reader/parser)

   // Open the file
   FILE   *tableFile = cfgOpen(fileName, "r"); //Open file for read
   
   // If tableFile is not found, geenrate errors, otherwise parse
   if (tableFile == NULL) {
      nValues = -1;
      char msg[128];
      sprintf(msg,
         "2D Configuration file %s not found.\n\n"
         "Function will return zeros.", fileName);
      MessageBox(NULL, msg, "MegaTune Table File Reader", MB_OK);   
   }
   else {
      // Initialize nValues to zero, we will count up as s and stick into nValues later
      nValues = 0;

      // line can be up to 521 characters
      char lineBuffer[512];

      //---------------------------------------------------------------------------------------------
      //--  Parses lines of the form:                                                              --
      //--                                                                                         --
      //-- ^<whitespace><number1><whitespace>,<whitespace><number2><whitespace>;<any old junk>$    --
      //--                                                                                         --
      //--  where character case is insignificant, and the form of numbers                         --
      //--  is derived from P&E 6808 assembly language.                                            --
      //--                                                                                         --
      //--  Table is of form:                                                                      --
      //--  <number1>     this is Volts from first line                                            --
      //--  <number2>     this is AF from first point                                              --
      //--  <number3>     this is Volts from second line                                           --
      //--  ...                                                                                    --
      //---------------------------------------------------------------------------------------------

      int tableSize = 256;
      *values = new WORD[tableSize];

      // while there are more lines to process, get the next line
      while (fgets(lineBuffer, sizeof(lineBuffer)-1, tableFile)) {
         
          // pointer *s points to beginning of current line
          char *s = lineBuffer;

         // Count the number of characters as s
         if (!charIn(*s, WHITESPACE)) continue;
         // skip over whitespace, s point to values only
         while (charIn(*s, WHITESPACE)) s++;        // Skip leading white space.
         // if (!charIn(*s, "dD"      )) continue; else s++; find D
         // if (!charIn(*s, "bBwW"    )) continue; else s++; find B or W
         // if (!charIn(*s, WHITESPACE)) continue; whitespace after the DB
         while (charIn(*s, WHITESPACE)) s++;

         char *last = s;               // Null terminate the string of interest.

         while (charIn(*last, "0123456789$%!QHTqht")) last++;
         *last = '\0';

         // If the number of values is greater than the table size
         if (nValues >= tableSize+1) {
            WORD *oldV = *values;
            tableSize *= 4;
            *values = new WORD[tableSize];
            memcpy(*values, oldV, sizeof(WORD)*nValues);
            delete [] oldV;
         }

         if (!number(s, (*values)[nValues++])) {
            break;
         }
      }

      cfgClose(tableFile);
   }
}

//------------------------------------------------------------------------------

bool msDatabase::readTable(const char *fileName, WORD *values, bool msgDisp)
{
   FILE   *tableFile = cfgOpen(fileName, "r");
   bool   ok = true;

   int nValues = 0;

   if (tableFile == NULL) {
      char msg[128];
      sprintf(msg,
         "Configuration file %s not found.\n"
         "Using default values for internal table.", fileName);
      if (msgDisp) MessageBox(NULL, msg, "MegaTune", MB_OK);
      ok = false;
   }
   else {
      char lineBuffer[512];

      //------------------------------------------------------------------------
      //--  Parses lines of the form:                                         --
      //--                                                                    --
      //--    ^<whitespace>D[BW]<whitespace><number><any old junk>$              --
      //--                                                                    --
      //--  where character case is insignificant, and the form of numbers    --
      //--  is derived from P&E 6808 assembly language.                       --

      while (fgets(lineBuffer, sizeof(lineBuffer)-1, tableFile)) {
         char *s = lineBuffer;

         if (!charIn(*s, WHITESPACE)) continue;
         while (charIn(*s, WHITESPACE)) s++;        // Skip leading white space.
         if (!charIn(*s, "dD"      )) continue; else s++;
         if (!charIn(*s, "bBwW"    )) continue; else s++;
         if (!charIn(*s, WHITESPACE)) continue;
         while (charIn(*s, WHITESPACE)) s++;

         char *last = s;               // Null terminate the string of interest.
         while (charIn(*last, "0123456789$%!QHTqht")) last++;
         *last = '\0';

         if (nValues >= 256) {
            char msg[128];
            sprintf(msg,
               "Too many values (more than 256) found in %s.\n"
               "Extra values will be ignored.", fileName);
            MessageBox(NULL, msg, NULL, MB_OK);
            ok = false;
            break;
         }
         if (!number(s, values[nValues++])) {
            char msg[128];
            sprintf(msg,
               "\"%s\" is not a number in %s.\n"
               "Internal table will be wrong.", s, fileName);
            MessageBox(NULL, msg, NULL, MB_OK);
            ok = false;
            break;
         }
      }

      cfgClose(tableFile);

      if (nValues < 256) {
         char msg[128];
         sprintf(msg,
            "Too few values (fewer than 256) found in %s.\n"
            "Internal table will be wrong.", fileName);
         MessageBox(NULL, msg, NULL, MB_OK);
         ok = false;
      }
   }

   return ok;
}

//------------------------------------------------------------------------------
// Burst Mode
// comm port
// 115200
// Stream "A" commands as fast as we can until we get 10 or so timeouts.
//------------------------------------------------------------------------------

bool msDatabase::readConfig()
{
   bool existed = false;

   int cx = ::GetSystemMetrics(SM_CXSCREEN); // Use these for default windows
   int cy = ::GetSystemMetrics(SM_CYSCREEN); // sizes on initial startup.
   int l = 0; // 50;
   int t = 0; // 50;
   int r = 0; // cx-100;
   int b = 0; // cy-100;
   mainWin.SetRect(l, t, r, b);
   tuneWin.SetRect(l, t, r, b);

   FILE *msConfig = cfgOpen("megatune.cfg", "r");
   commPortNumber =    1;
   commPortRate   = 9600;

   if (msConfig != NULL) {
      existed = true;

      char cbuf[100];
      if (fgets(cbuf, sizeof(cbuf), msConfig) != NULL) {
         commPortNumber = strtol(cbuf+3, NULL, 10);
         if (commPortNumber < 1 || commPortNumber > 99) commPortNumber = 1;
      }

      if (fgets(cbuf, sizeof(cbuf), msConfig) != NULL) {
         int igtemp;
         sscanf(cbuf, "%d", &igtemp);
         timerInterval = igtemp;
         userTimerInt  = timerInterval;
      }

      if (fgets(cbuf, sizeof(cbuf), msConfig) != NULL) {
         sscanf(cbuf, "%d", &commPortRate);
         if (commPortRate !=   9600 
          && commPortRate !=  19200
          && commPortRate !=  38400
          && commPortRate !=  56000
          && commPortRate !=  57600
          && commPortRate != 115200) commPortRate = 9600;
      }

      io.reset(commPortNumber, commPortRate);

      if (fgets(cbuf, sizeof(cbuf), msConfig) != NULL) {
         sscanf(cbuf, "%d %d %d %d", &mainWin.left, &mainWin.top, &mainWin.right, &mainWin.bottom);
         if (mainWin.left >= mainWin.right || mainWin.top >= mainWin.bottom) {
            mainWin.SetRect(50, 50, cx-100, cy-100);
         }
      }

      if (fgets(cbuf, sizeof(cbuf), msConfig) != NULL) {
         sscanf(cbuf, "%d %d %d %d", &tuneWin.left, &tuneWin.top, &tuneWin.right, &tuneWin.bottom);
         if (tuneWin.left >= tuneWin.right || tuneWin.top >= tuneWin.bottom) {
            tuneWin.SetRect(50, 50, cx-100, cy-100);
         }
      }
      cfgLog("   Comm port           = %6d", commPortNumber);
      cfgLog("   Comm rate           = %6d", commPortRate);
      cfgLog("   Timer interval      = %6d", timerInterval);
      cfgClose(msConfig);
   }

   return existed;
}

//------------------------------------------------------------------------------

void msDatabase::writeConfig(int port, int rate)
{
   if (port == -1) port = io.port();
   if (rate == -1) rate = io.rate();
   io.reset(port, rate);

   FILE *msConfig = cfgOpen("megatune.cfg", "w");
   if (msConfig == NULL)
      MessageBox(NULL, "Configuration file megatune.cfg can't be written.\nDirectory protection error?", NULL, MB_OK);
   else {
      fprintf(msConfig, "COM%d\n", io.port());
      fprintf(msConfig, "%d\n", userTimerInt);
      fprintf(msConfig, "%d\n", io.rate());
      fprintf(msConfig, "%d %d %d %d\n", mainWin.left, mainWin.top, mainWin.right, mainWin.bottom);
      fprintf(msConfig, "%d %d %d %d\n", tuneWin.left, tuneWin.top, tuneWin.right, tuneWin.bottom);
      cfgClose(msConfig);
   }
}

//------------------------------------------------------------------------------

static DWORD tickStart = timeGetTime();

double timeNow()
{
   return double(timeGetTime()-tickStart) / 1000.0;
}

void msDatabase::setRecording(bool rec, bool burstMode)
{
   if (log) delete log;
   log = NULL;

   if (!rec && bursting) {
      // Set speed and timeout back to defaults.
      io.reset(commPortNumber, commPortRate);
      timerInterval     = userTimerInt;
      bursting          = false;
   }

   if (rec) {
      CString datalogFilter = "Excel Spreadsheet (*.xls)|*.xls"
                             "|Comma Separated Values (*.csv)|*.csv"
                             "|Text File (*.txt)|*.txt"
                             "|Print File (*.prn)|*.prn"
                             "|All Files (*.*)|*.*";
      if (datalogFilter.Find(lop._defaultLogExtension) == -1) {
         datalogFilter = "MegaTune Log File (*."+lop._defaultLogExtension+")|"+lop._defaultLogExtension+"|"
                       + datalogFilter;
      }

      char fileName[128];
      sprintf(fileName, "datalog%s.%s", timeStamp(false), lop._defaultLogExtension);
      CFileDialog dlgr(FALSE, lop._defaultLogExtension, fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, datalogFilter);
      dlgr.m_ofn.lpstrInitialDir = cwd();
      dlgr.m_ofn.lpstrTitle      = " MegaTune Datalog File";
      if (dlgr.DoModal() == IDOK) {
         m_logFileName = dlgr.GetPathName();
         log = new datalog();
         if (!log->open(m_logFileName)) {
            char message[200];
            sprintf(message, "File \"%s\" can't be opened", m_logFileName);
            MessageBox(NULL, message, NULL, MB_OK);
            delete log;
            log = NULL;
         }
         else {
            if (burstMode) {
               timerInterval     = 0;
               io.reset(burstCommPort, burstCommRate);
               bursting          = true;
            }
         }
      }
   }
}

//------------------------------------------------------------------------------

typedef char path_t[_MAX_PATH];

class fileList {
   public:
      fileList(CString extension)
       : _extension (extension)
       , _maxLen    (0)
      {
      }

      void addFiles(CString path)
      {
//_fileList.push_back(CString("DIR: "+path));
         path_t fileSpec;
         sprintf(fileSpec, "%s/*", path);
         struct _finddata_t fileData;
         long               fileHandle;
         if ((fileHandle = _findfirst(fileSpec, &fileData)) != -1) {
            do {
               if (strcmp(fileData.name, "." )            == 0  ) continue;
               if (strcmp(fileData.name, "..")            == 0  ) continue;
               if (fileData.name[strlen(fileData.name)-1] == '~') continue;

               struct _stat stats;
               int status = _stat(path+"/"+fileData.name, &stats);
               if (status == 0) {
                  if (_S_IFDIR & stats.st_mode) {
                     addFiles(path+"/"+fileData.name);
                  }
                  else if (_stricmp(fileData.name+strlen(fileData.name)-strlen(_extension), _extension) == 0) {
                     _fileList.push_back(CString(path+"/"+fileData.name));
                     int len = _fileList.back().GetLength();
                     if (len > _maxLen) _maxLen = len;
                  }
               }
            } while (_findnext(fileHandle, &fileData) == 0);
            _findclose(fileHandle);
         }
      }
      
      int nNames() { return _fileList.size(); }
      int maxLen() { return _maxLen; }
      const CString &operator [](int idx) const { return _fileList[idx]; }

   private:
      CString              _extension;
      std::vector<CString> _fileList;
      int                  _maxLen;
};

//------------------------------------------------------------------------------

static char BASED_CODE szMsqFilter[] = "MegaSquirt Settings Files (*.msq)|*.msq|All Files (*.*)|*.*";

// 1.0.1 is original release through 2.00
// 2.0.0 is dual table files.
// 3.0.0 is from constants section of init file, 2.25 stuff.

#define Major 3 // File version, not MegaTune.
#define Minor 0
#define Rev   0

struct versionInfo {
   char       desc[10];          // 16 bytes, tag plus numbers.
   short int  major;
   short int  minor;
   short int  rev;

   versionInfo() : major(Major), minor(Minor), rev(Rev) {
      assert(sizeof(versionInfo) == 16);
      strcpy(desc, "#MegaTune"); // That's 10 including the null terminator.
   }
   versionInfo(short int M, short int m, short int r)
    : major(M),
      minor(m),
      rev  (r)
   {
      assert(sizeof(versionInfo) == 16);
      strcpy(desc, "#MegaTune"); // That's 10 including the null terminator.
   }

   bool isXML()
   {
      return strncmp(desc, "<?xml vers", 10) == 0;
   }

   bool operator ==(versionInfo &rhs)
   {
      return
         major == rhs.major &&
         minor == rhs.minor &&
         rev   == rhs.rev   &&
         strcmp(desc, "#MegaTune") == 0;
   }
};

static versionInfo v101(1, 0, 1);
static versionInfo v200(2, 0, 0);
static versionInfo v300(3, 0, 0);

//------------------------------------------------------------------------------

bool writeXML = true; // Allow users to switch it off, just in case.

void msDatabase::writeSettings()
{
   try {
      CFile msqFile;
         if (writeXML) {
            cDesc.saveXML(settingsFile);
         }
         else {
            msqFile.Open(settingsFile, CFile::modeCreate | CFile::modeWrite);
               versionInfo v;
               msqFile.Write(&v,            sizeof(versionInfo));
               cDesc.serialize(msqFile);
            msqFile.Close();
         }
      cDesc.changed(false);
   }
   catch (CFileException* e) {
      char message[128];
      sprintf(message, "File \"%s\" can't be opened", settingsFile);
      MessageBox(NULL, message, NULL, MB_OK);
   }
}

void msDatabase::saveAs()
{
   char fileName[128];
   sprintf(fileName, "megasquirt%s.msq", timeStamp(false));
   CFileDialog dlgr(FALSE, "msq", fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szMsqFilter);
   dlgr.m_ofn.lpstrInitialDir = cwd();
   dlgr.m_ofn.lpstrTitle      = " MegaTune Save Configuration";
   if (dlgr.DoModal() == IDOK) {
      settingsFile = dlgr.GetPathName();
      writeSettings();
   }
}

void msDatabase::save()
{
   if (settingsFile == "")
      saveAs();
   else {
      writeSettings();
   }
}

//------------------------------------------------------------------------------

std::vector<msqInfo*> msDatabase::infoSettings()
{
   std::vector<msqInfo*> mi;

   fileList f = fileList("msq");
   path_t pathName;
   _getcwd(pathName, sizeof(pathName));
   char *s;
   while (s = strchr(pathName, '\\')) *s = '/';
   s = strrchr(pathName, '/');
   if (s) *s = 0;
   f.addFiles(pathName);
   int l    = strlen(pathName)+1;
   int size = f.maxLen() - l;

   char sig[128];
   for (int i = 0; i < f.nNames(); i++) {
      FILE *m = fopen(f[i], "r");
         versionInfo v;
         if (fread(&v, sizeof(versionInfo), 1, m) != 1) memset(&v, 0, sizeof(versionInfo));
         if (v.isXML()) {
            rewind(m);
            while (fgets(sig, sizeof(sig), m)) {
               if (s = strstr(sig, "signature=")) {
                  strcpy(sig, s+11);
                  if (s = strchr(sig, '"')) *s = 0;
                  if (strlen(sig) == 1) {
                          if (sig[0] == 10) { strcpy(sig, "MegaSquirt-I 1.000"); }
                     else if (sig[0] == 20) { strcpy(sig, "MegaSquirt-I 2.xxx or 3.000"); }
                     else sprintf(sig, "%#x", sig[0]);
                  }
                  break;
               }
            }
            v.major = 4;
            v.minor = 0;
         }
         else {
                 if (v == v101) strcpy(sig, "MegaSquirt-I");
            else if (v == v200) strcpy(sig, "MegaSquirt-I or DualTable");
            else if (v == v300) {
               int sigSize = 0;
               fread(&sigSize, sizeof(int), 1, m);
               memset(sig, 0, sizeof(sig));
               fread(sig, sigSize, 1, m);
               if (sigSize == 1) {
                       if (sig[0] == 10) { strcpy(sig, "MegaSquirt-I 1.000"); }
                  else if (sig[0] == 20) { strcpy(sig, "MegaSquirt-I 2.xxx or 3.000"); }
                  else sprintf(sig, "%#x", sig[0]);
               }
            }
         }
      fclose(m);
      mi.push_back(new msqInfo(static_cast<const char *>(f[i])+l, sig, v.major, v.minor));
   }

   return mi;
}

static void dumpInfo()
{
   std::vector<msqInfo*> mi = msDatabase::infoSettings();
   int len = 0;
   unsigned i;
   for (i = 0; i < mi.size(); i++) {
      if (strlen(mi[i]->path) > len) len = strlen(mi[i]->path);
   }

   FILE *o = fopen("dumpty.txt", "w");
      fprintf(o, "%d\n", mi.size());
      for (i = 0; i < mi.size(); i++) {
         fprintf(o, "%3d %*s", i+1, -len, mi[i]->path);
         fprintf(o, "%3d.%02d  >>%s<<\n", mi[i]->major, mi[i]->minor, mi[i]->sig);
         delete mi[i];
      }
   fclose(o);
exit(1);
}


//------------------------------------------------------------------------------

void msDatabase::readSettings(bool fullLoad)
{
   TRY { try {
      CFile msqFile;
      msqFile.Open(settingsFile, CFile::modeRead);
      if ((UINT)msqFile.m_hFile == (UINT)CFile::hFileNull) {
         CFileException::ThrowOsError(LONG(::GetLastError()), settingsFile);
      }
      versionInfo v, current;
      msqFile.Read(&v, sizeof(versionInfo));
      if (v.isXML()) {
         msqFile.Close();
         int nAnomalies = cDesc.readXML(settingsFile);
         _loaded = true;
         cDesc.changed(false);
         setByName("baud", 0, io.rate());
         CString prefix;
         if (nAnomalies > 0) {
            prefix.Format("Although the file was read successfully, there were %d warnings           \n"
                          "written to the audit.log file.  You may want to review that first\n"
                          "before you send and burn the data.\n\n", nAnomalies);
         }
         if (MessageBox(NULL, "Read\n\n   "+settingsFile+"     \n\n"+prefix+"Send & burn values into controller?",
                              "MegaTune Settings File", MB_YESNO | MB_DEFBUTTON1 | MB_ICONQUESTION) == IDYES) {
             putConst();
         }
      }
      else {
         int size = 0;
         if (v == v101) size = 129;
         if (v == v200) size = 257;
         if (v == v300) size =   1;

         if (size > 1 && cDesc.signature() != "\024") { // Old configuration file.
            if (MessageBox(NULL, "Settings file\n\n     "+settingsFile+"     \n\n"
                                 "contains data in an old format only acceptable for use with MS-I,\n"
                                 "and without any version validation information.\n\n"
                                 "Are you sure you want to read it?",
                                 "MegaTune Settings File", MB_YESNO | MB_DEFBUTTON1 | MB_ICONQUESTION) != IDYES) {
               msqFile.Close();
               return;
            }
         }

         if (size > 0) {
            if (size >  1) msqFile.Read(cDesc._const,  size);
            if (size == 1) {
               CString errors;
               if (!cDesc.checkFile(msqFile, errors)) {
                  if (MessageBox(NULL, "Settings file\n\n     "+settingsFile+"     \n\n"
                                       "contains data not compatible with the current configuration:\n\n"+
                                       errors +
                                       "\n\nThis could severely mangle your configuration, are you sure you want to read it?            ",
                                       "MegaTune Settings File", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES) {
                     msqFile.Close();
                     return;
                  }
               }
               msqFile.Seek(sizeof(versionInfo), 0);
               cDesc.deserialize(msqFile);
            }
            _loaded    = true;
            cDesc.changed(false);

            // Automatic behavior to avoid confusing errors.
            setByName("baud", 0, io.rate());
            uml.enable();
            uil.enable();

            if (MessageBox(NULL, "Read\n\n   "+settingsFile+"     \n\nSend & burn values into controller?",
                                 "MegaTune Settings File", MB_YESNO | MB_DEFBUTTON1 | MB_ICONQUESTION) == IDYES) {
                putConst();
            }
         }
         else {
            char message[128];
            sprintf(message, "MSQ file has wrong MegaTune version number\n(it may not be a real MSQ file),\nexpected %d.%d.%d and found %d.%d.%d", current.major, current.minor, current.rev, v.major, v.minor, v.rev);
            MessageBox(NULL, message, NULL, MB_OK);
            _loaded  = false;
            cDesc.changed(true);
         }
         msqFile.Close();
      }
   }

   catch (CString s) {
      msgOk("File Open", s+"\n\nFile not loaded.");
   }
   }

   CATCH (CFileException, e) {
      if (settingsFile == "_autoload_.msq")
         settingsFile = "";
      else {
         char message[128];
         sprintf(message, "MegaTune cannot open the file \"%s\".\n\nIf it exists, check it's permissions.", settingsFile);
         msgOk("MegaTune File Open", message);
      }
   }
   END_CATCH
}

//------------------------------------------------------------------------------

void msDatabase::open()
{
   CFileDialog dlgr(TRUE, "msq", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szMsqFilter);
   dlgr.m_ofn.lpstrInitialDir = cwd();
   dlgr.m_ofn.lpstrTitle      = " MegaTune Open Configuration";
   if (dlgr.DoModal() == IDOK) {
      settingsFile = dlgr.GetPathName();
      readSettings();
   }
}

//------------------------------------------------------------------------------

symbol *msDatabase::lookupByPage(const char *symName, int pageNo)
{
   char pagedName[128];
   sprintf(pagedName, "%s%d", symName, pageNo+1);
   symbol *sym = cDesc.lookup(pagedName);
   if (sym == NULL) sym = cDesc.lookup(symName);
   return sym;
}

double msDatabase::getByName(const char *symName, int pageNo)
{
   symbol *b = lookupByPage(symName, pageNo);
   return b ? b->valueUser() : 0;
}

bool msDatabase::setByName(const char *symName, int pageNo, double v)
{
   symbol *b = lookupByPage(symName, pageNo);
   if (b) b->storeValue(v);
   return b != NULL;
}

bool msDatabase::sendByName(const char *symName, int pageNo)
{
   symbol *b = lookupByPage(symName, pageNo);
   if (b) cDesc.send(b);
   return b != NULL;
}

double msDatabase::egoLEDvalue()
{
   if (UegoVoltage == -1)
      return getByName("afr", 0);
   else
      return cDesc._userVar[UegoVoltage];
}

//------------------------------------------------------------------------------

double interp(double x, double x0, double y0, double x1, double y1)
{
   return y0 + (x-x0) / (x1-x0) * (y1-y0);
}

//------------------------------------------------------------------------------

int msDatabase::setPageNo(int pageNo, bool activate)
{
   int oldPage = _pageNo;
   int l = cDesc.nPages()-1;
   if (pageNo > l)  pageNo = l;
   if (pageNo < 0)  pageNo = 0;
   _pageNo = pageNo;
   if (activate) {
//    CString msg;
//    msg.Format("MDB Activate phys-page=%d", pageNo);
//    logEvent(msg);
      cDesc.sendPageActivate(_pageNo);
   }
   return oldPage;
}

//------------------------------------------------------------------------------

void msDatabase::putConst(int pageNo)
{
   int nPages   = cDesc.nPages();
   int savePage = _pageNo;

   int startPage = 0, endPage = nPages-1;
   if (pageNo != -1) {
      startPage = pageNo;
      endPage   = pageNo;
   }

#if 0
-Extra testing...
_pageNo = 3;
cDesc.sendPage(_pageNo);
cDesc.sendBurnCommand(_pageNo);
#else
   for (_pageNo = startPage; _pageNo <= endPage; _pageNo++) {
      cDesc.sendPage(_pageNo);
      burnConst(_pageNo); // Must be inside loop for -Extra to work properly.
   }
#endif
   setPageNo(savePage);
}

void msDatabase::putConstByteP(int pageNo, int offset)
{
   if (pageNo == -1) pageNo = _pageNo;
   io.write(cDesc.pageWriteValue(pageNo, offset));
   _burned = false;
}

void msDatabase::burnConst(int thisPage)
{
   int firstPage = 0;
   int lastPage  = cDesc.nPages()-1;
   if (thisPage != -1) {
      firstPage = lastPage = thisPage;
   }
   for (int iPage = firstPage; iPage <= lastPage; iPage++) {
      if (cDesc.pageModified(iPage)) {
         if (!getRuntime()) {
            char msg[128];
            sprintf(msg, "Page %d: Could not connect to controller.", iPage+1);
            MessageBox(NULL, msg, "MegaSquirt", MB_OK);
         }
         else {
            symbol *b = cDesc.lookup(S_batteryVoltage);
            if (b && b->valueUser() < 7.0) {
               if (MessageBox(NULL, "Controller voltage too low for reliable FLASH burning.\nTry anyhow?", "Burn Flash", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION | MB_SYSTEMMODAL) == IDNO) return;
            }
            cDesc.sendBurnCommand(iPage);
            _burned = true;
         }
      }
   }
}

//------------------------------------------------------------------------------

void msDatabase::getVersion()
{
   if (!bursting) {
      if (*signature == 0 && cDesc.queryCommand().len() > 0) {
         io.write(cDesc.queryCommand());
         BYTE q;
         unsigned  i;
         for (i = 0; cDesc.read(q); i++) {
            signature[i] = q;
         }
         if (i == 0) return; // Not connected yet.
         if (i >  0) signature[i+1] = 0;

         if (cDesc.signature() != signature) {
            char expected[128];
            char received[128];
            byteString ebs = cDesc.signature();
            if (ebs.len() == 1)
               sprintf(expected, "0x%02x", ebs[0]);
            else {
               for (i = 0; i < ebs.len(); i++) {
                  char c = ebs[i];
                  expected[i] = c < 32 || c > 127 ? '?' : c;
               }
               expected[i] = 0;
            }
            if (strlen(signature) == 1)
               sprintf(received, "0x%02x", signature[0]);
            else {
               for (i = 0; i < strlen(signature); i++) {
                  char c = signature[i];
                  received[i] = c < 32 || c > 127 ? '?' : c;
               }
               received[i] = 0;
            }
            char msg[1024];
            sprintf(msg,
               "Controller code version does not match signature in %s.\t\n"
               "(See audit.log for where the above file was found.)\n\n"
               "      Expected   \"%s\" (found in ini file)\n"
               "      Received   \"%s\" (from controller)\n\n"
               "Table corruption may occur, this is usually a very serious problem.\n\n"
               "Terminate MegaTune now?", cDesc.signatureFile(), expected, received);
            if (MessageBox(NULL, msg, "MegaSquirt Controller Code Version", MB_YESNO | MB_DEFBUTTON1 | MB_ICONQUESTION) == IDYES) {
               exit(1);
            }
            if (signature[0] == 0) {
               strcpy(signature, "\177"); // ??? What is this?
            }
         }

         while (isspace(signature[strlen(signature)-1])) signature[strlen(signature)-1] = '\0';
         if (cDesc.versionInfo().len() == 0)
            strcpy(title, signature);
         else {
            io.write(cDesc.versionInfo());
            for (i = 0; cDesc.read(q); i++) {
               title[i] = q;
            }
         }
              if (title[0] ==  0) strcpy(title, "Unknown code variant");
         else if (title[0] ==  1) strcpy(title, "H&F DT v.1.??");
         else if (title[0] == 20) strcpy(title, "B&G v.2.00");
      }
   }
}

//------------------------------------------------------------------------------

bool msDatabase::getMemory(BYTE pageNumber, BYTE buffer[256])
{
   getVersion();
   return false;
#if 0 // Legacy from dt.
   BYTE cmd[2] = { 'F', pageNumber };
   io.write(cmd, 2);

   if (!cDesc.read(buffer, 256)) return false;

   return true;
#endif
}

//------------------------------------------------------------------------------

#define N_RETRIES 5

BYTE *msDatabase::pBytes = NULL;

static void allocBuffer(long nBytes)
{
   static int  nB = 0;
   if (nBytes > nB) {
      if (msDatabase::pBytes) delete [] msDatabase::pBytes;
      msDatabase::pBytes  = new BYTE[nBytes];
      nB                  = nBytes;
   }
}

bool msDatabase::getConst(int pageNo)
{
   if (bursting) return true;

   CWaitCursor wait;
   if (pageNo == -1) pageNo = _pageNo;
   int  nBytes = cDesc.pageSize  (pageNo);
   int  ofs    = cDesc.pageOffset(pageNo);

   allocBuffer(nBytes);

   bool getOk = false;
   for (int i = 0; i < N_RETRIES && !getOk; i++) {
      cDesc.flush(); // Flush the comm input.
      cDesc.sendPageReadWhole(pageNo);
      getOk = cDesc.read(pBytes, nBytes); // Don't read directly into database, in case we don't get data.
   }

   if (getOk) memcpy(cDesc._const+ofs, pBytes, nBytes);

   return getOk;
}

bool msDatabase::getPage(int pageNo)
{
   if (pageNo != -1)
      return getConst(pageNo);
   else {
      for (int iPage = 0; iPage < cDesc.nPages(); iPage++) {
         if (!getConst(iPage)) return false;
      }
   }
   return true;
}

bool msDatabase::load()
{
   getVersion();
   for (int iPage = 0; iPage < cDesc.nPages(); iPage++) {
      if (!getConst(iPage)) return false;
   }
   _loaded  = true;
   cDesc.changed(false);
   return true;
}

//------------------------------------------------------------------------------

bool msDatabase::getRuntime()
{
static bool heapError = false;
if (heapError) return false;
   controllerReset = false;
   static double previousSecl = 255;

   static int iSeq = 0;
   if (iSeq >= cDesc.ochSeqCount()) iSeq = 0;

   bool getOk[controllerDescriptor::maxPages] = { true };
   for (int iP = 0; iP < cDesc.ochSeqSize(iSeq); iP++) {
      int iPage = cDesc.ochSeqValue(iSeq, iP);
      getOk[iPage] = false;
      int nBytes = cDesc.ochBlockSize(iPage);
      allocBuffer(nBytes);

#ifdef _DEBUG
      if (_heapchk() != _HEAPOK) {
         heapError = true;
         msgOk("Heap error", "MT Heap Error"__FILE__" %d", __LINE__);
         return false;
      }
#endif

      int nr = iPage == 0 ? N_RETRIES : 1;
      for (int i = 0; i < nr && !getOk[iPage]; i++) {
         cDesc.flush(); // Flush the comm input.

         if (bursting) cDesc.sendOchBurstCommand(iPage);
         else          cDesc.sendOchGetCommand  (iPage);

         getOk[iPage] = cDesc.read(pBytes, nBytes);
         cDesc.setOchPolled(iPage, getOk[iPage]);
      }

#ifdef _DEBUG
      if (_heapchk() != _HEAPOK) {
         heapError = true;
         msgOk("Heap error", "MT Heap Error"__FILE__" %d", __LINE__);
         return false;
      }
#endif

      if (getOk[iPage]) {
         for (int i = 0; i < nBytes; i++) cDesc.setOch(iPage, pBytes[i], i);
      }
      else {
         for (int i = 0; i < nBytes; i++) cDesc.setOch(iPage, 0,         i);
      }
   }
   iSeq++;

#ifdef _DEBUG
   if (_heapchk() != _HEAPOK) {
      heapError = true;
      msgOk("Heap error", "MT Heap Error"__FILE__" %d", __LINE__);
      return false;
   }
#endif

   cDesc.populateUserVars();

#ifdef _DEBUG
   if (_heapchk() != _HEAPOK) {
      heapError = true;
      msgOk("Heap error", "MT Heap Error"__FILE__" %d", __LINE__);
      return false;
   }
#endif

   cDesc.recalc();
   uml.enable();
   uil.enable();

   if (getOk[0]) {
      static symbol *sSecl = NULL;
      if (sSecl == NULL) sSecl = cDesc.lookup(S_secl);
      double secl = sSecl ? sSecl->valueUser() : 0.0;
      if (secl != previousSecl) {
#if 0
         if (secl == 0 && previousSecl == 255) {
            // Time to ping the screen so it doesn't blank.
            INPUT ip;
            ip.type       = INPUT_KEYBOARD;
            ip.ki.wVk     = '!';
            ip.ki.wScan   = 0;
            ip.ki.dwFlags = KEYEVENTF_KEYUP;
            ip.ki.time    = 0;
            SendInput(1, &ip, 1); // Emulate typing a !, which should be ignored.
         }
#endif

         if (secl == 0 && previousSecl != 255) {
            // An unexpected reset of controller has occurred.
            //MessageBeep(MB_ICONEXCLAMATION);
            Beep(2000,100);
            Beep(1000,100);
            Beep(2000,100);
            controllerReset = true;
            controllerResetCount++;
         }
         previousSecl = secl;
      }

      if (log) log->write();
   }

#ifdef _DEBUG
   if (_heapchk() != _HEAPOK) {
      msgOk("Heap error", "MT Heap Error"__FILE__" %d", __LINE__);
   }
#endif

   return getOk[0];
}

//------------------------------------------------------------------------------

static int nOut = 0;

void dPrintV(FILE *dmp, const char *fmt, va_list args)
{
   if (_quoteDump && nOut == 0) { fputc('"', dmp); nOut++; }
   nOut += vfprintf(dmp, fmt, args);
}

void dPrintN(FILE *dmp, const char *fmt, ...)
{
   va_list args;
   va_start(args, fmt);
      dPrintV(dmp, fmt, args);
   va_end(args);
}

void dPrintL(FILE *dmp, const char *fmt, ...)
{
   va_list args;
   va_start(args, fmt);
      dPrintV(dmp, fmt, args);
   va_end(args);
   if (_quoteDump && nOut > 0) { fputc('"', dmp); nOut++; }
   fputc('\n', dmp);
   nOut = 0;
}

void msDatabase::dAll(FILE *dmp, bool quote)
{
   _quoteDump = quote;
   const time_t t = time(NULL);
   char  timeS[128];
   strcpy(timeS, ctime(&t));
   *strchr(timeS, '\n') = 0;
   dPrintL(dmp, "MegaTune Configuration Dump %s", timeS);
   dPrintL(dmp, "Windows %d.%d %d.%d", _winmajor, _winminor, _osver, _winver);
   dPrintL(dmp, "");

   dPrintL(dmp, "KEY: h=sHape, o=Offset, r=Raw value, s=Scale, t=Translate, u=Units, v=user Value, z=element siZe");
   dPrintL(dmp, "");

   cDesc.dump(dmp);
}

void msDatabase::dump()
{
   FILE *dmp = fopen("MegaTune.dmp", "a");
   if (dmp == NULL)
      MessageBox(NULL, "Couldn't write to MegaTune.dmp dump file.", NULL, MB_OK);
   else {
      dAll(dmp);
      dPrintL(dmp, "");
      fclose(dmp);
   }
}

//------------------------------------------------------------------------------

#include "vex.h"

#include "tuning3D.h"
extern tuningDialog ut[tuningDialog::nUT];

static char BASED_CODE veMsqFilter[] = "Table Exchange (*.vex)|*.vex|All Files (*.*)|*.*";

void msDatabase::veExport(int tableNo)
{
   char fileName[128];
   char prefix  [64];
   if (tableNo == -1) strcpy (prefix, "megasquirt");
   else               sprintf(prefix, "%s", ut[tableNo].z->name());
   sprintf(fileName, "%s_%s.vex", prefix, timeStamp(false));

   CFileDialog dlgr(FALSE, "vex", fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, veMsqFilter);
   dlgr.m_ofn.lpstrInitialDir = cwd();
   dlgr.m_ofn.lpstrTitle      = " VEX Table Export";

   if (dlgr.DoModal() == IDOK) {
      CString fName = dlgr.GetPathName();
      FILE *exp = fopen(fName, "w");
      if (exp == NULL)
         MessageBox(NULL,"Couldn't export "+fName+" Table Exchange file.", "Table Export", MB_OK);
      else {
         fclose(exp);

         vex v;
         int savePage = _pageNo;
         int sp = tableNo == -1 ? 0                 : tableNo;
         int ep = tableNo == -1 ? tuningDialog::nUT : tableNo+1;
         for (int _ip = sp; _ip < ep; _ip++) {
            tuningDialog *td = ut+_ip;
            if (td->z == NULL) continue;
            _pageNo = td->pageNo;

            veTable *p = v.newPage(0 /*_pageNo*/);
            const int nRPM = td->x->nValues();
            const int nLOD = td->y->nValues();
            double *rpm = new double[nRPM];
            double *lod = new double[nLOD];
            double *ve  = new double[nRPM*nLOD];
            symbol *sRpm = td->z->xOrder() ? td->x : td->y;
            symbol *sLod = td->z->xOrder() ? td->y : td->x;
            int i;
            for (i = 0; i < nRPM; i++) {
               rpm[i] = sRpm->valueUser(i)/100;
            }
            for (i = 0; i < nLOD; i++) {
               lod[i] = sLod->valueUser(i);
            }
            i = 0;
            p->setScale    (td->z->scale());
            p->setTranslate(td->z->trans());
            for (int M = 0; M < nLOD; M++) {
               for (int R = 0; R < nRPM; R++) {
                  ve[i++] = td->z->valueRaw(td->z->index(R, M));
               }
            }

            p->setSize(nRPM, nLOD, rpm, lod, ve);
            delete [] rpm;
            delete [] lod;
            delete [] ve;
         }
         _pageNo = savePage;
         vex::errorStatus s = v.write(LPCTSTR(fName));
         if (s != vex::noError) MessageBox(NULL, "Could not write "+fName+"\n"+vex::errorMsg(s), "Table Export", MB_OK);
      }
   }
}

//------------------------------------------------------------------------------

void msDatabase::veImport(int tableNo)
{
   CFileDialog dlgr(TRUE, "vex", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, veMsqFilter);
   dlgr.m_ofn.lpstrInitialDir = cwd();
   dlgr.m_ofn.lpstrTitle      = " VEX Table Import";

   if (dlgr.DoModal() == IDOK) {
      CString fName = dlgr.GetPathName();
      vex v;
      vex::errorStatus s = v.read(LPCTSTR(fName));
      if (s != vex::noError) MessageBox(NULL, "Could not read "+fName+"\n"+vex::errorMsg(s), "Table Import", MB_OK);
//    int savePage = _pageNo;
      int sp = tableNo == -1 ? 0                 : tableNo;
      int ep = tableNo == -1 ? tuningDialog::nUT : tableNo+1;
      for (int _ip = sp; _ip < ep; _ip++) {
         tuningDialog *td = ut+_ip;
         if (td->z == NULL) continue;
//       _pageNo = td->pageNo;
//       veTable *p = v.page(_pageNo);
         veTable *p = v.page(0);
         if (!p) continue;

         const int nRPM = td->x->nValues();
         const int nLOD = td->y->nValues();
         if (p->nRPMs()  != nRPM || p->nLoads() != nLOD) {
            char msg[128];
            sprintf(msg, "Table was expected to be %dx%d,  but was %dx%d.\nInterpolating to expected shape...", nRPM, nLOD, p->nRPMs(), p->nLoads());
            MessageBox(NULL, msg, "Table Import", MB_OK);
            p->resize(nRPM, nLOD, 1.0, 5.0, 1.0);
         }


         symbol *sRpm = td->z->xOrder() ? td->x : td->y;
         symbol *sLod = td->z->xOrder() ? td->y : td->x;

         int i;
         for (i = 0; i < nRPM; i++) {
            sRpm->storeValue(p->rpm(i)*100,  i); // VEX rpms are scaled.
         }
         for (i = 0; i < nLOD; i++) {
            sLod->storeValue(p->load(i), i);
         }
         double s0 = p->scale(),     s1 = td->z->scale();
         double t0 = p->translate(), t1 = td->z->trans();
         for (int M = 0; M < nLOD; M++) {
            for (int R = 0; R < nRPM; R++) {
               long v = p->ve(R, M);
               if (s0 != s1 || t0 != t1) v = int((v+t0)*s0/s1-t1);
               td->z->storeRaw(v, td->z->index(R, M));
            }
         }
         cDesc.send(td->x);
         cDesc.send(td->y);
         cDesc.send(td->z);

         if (MessageBox(NULL,"Burn Table to controller?", "Burn Tables", MB_YESNO | MB_DEFBUTTON1 | MB_ICONQUESTION) == IDYES) burnConst(td->pageNo);
      }
//    _pageNo = savePage;
   }
}

//------------------------------------------------------------------------------

bool msDatabase::loaded()    { return _loaded;         }
bool msDatabase::changed()   { return cDesc.changed(); }
bool msDatabase::burned()    { return _burned;         }

bool msDatabase::recording() { return log != NULL; }

//------------------------------------------------------------------------------
