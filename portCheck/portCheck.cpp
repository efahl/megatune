//------------------------------------------------------------------------------
// cl portCheck.cpp

//------------------------------------------------------------------------------

#include <windows.h>
#include <stdio.h>
#include <assert.h>

//------------------------------------------------------------------------------

typedef unsigned char UINT08;

class port {
   public:
      port(UINT32 portNumber, UINT32 baudRate)
       : _portHandle(0)
       , _portNumber(portNumber)
       , _baudRate  (baudRate)
      {
      }

      HANDLE open()
      {
         bool exists = true;

         char portName[10];
         sprintf(portName, "COM%d", _portNumber);
         _portHandle = CreateFile(portName,
                          GENERIC_READ | GENERIC_WRITE,
                          0,
                          NULL,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL);

         if (_portHandle == INVALID_HANDLE_VALUE)
            exists = false;
         else {
            DCB  dcb    = { 0 };
            if (!GetCommState(_portHandle, &dcb))
               exists = false;
            else {
               dcb.BaudRate = _baudRate;
               dcb.ByteSize = 8;
               dcb.Parity   = NOPARITY;
               dcb.StopBits = ONESTOPBIT;
               dcb.fOutX    = false;
               dcb.fInX     = false;
               if (!SetCommState(_portHandle, &dcb))
                  exists = false;
               else {
                  COMMTIMEOUTS timeout;
                  if (!GetCommTimeouts(_portHandle, &timeout))
                     exists = false;
                  else {
                     timeout.ReadIntervalTimeout         =   0; // ms between chars on read.
                     timeout.ReadTotalTimeoutMultiplier  =   1;
                     timeout.ReadTotalTimeoutConstant    = 250;
                     timeout.WriteTotalTimeoutMultiplier =   1;
                     timeout.WriteTotalTimeoutConstant   =  25;
                     if (!SetCommTimeouts(_portHandle, &timeout)) exists = false;
                  }
               }
            }
         }
         if (!exists) close();

         return _portHandle;
      }

      bool isValid() const { return _portHandle != 0; }

      void close()
      {
         if (_portHandle) {
            CloseHandle(_portHandle);
            _portHandle = 0;
         }
      }

      void reopen()
      {
         close();
         open ();
      }

      bool write(const UINT08 *bytes, UINT32 count)
      {
         DWORD bytesWritten;
         bool  OK = WriteFile(_portHandle, bytes, count, &bytesWritten, NULL) != 0;
         if (bytesWritten != count) OK = false;
         return OK;
      }
      bool write(const char *s) { return write(reinterpret_cast<const UINT08 *>(s), strlen(s)); }

      bool read(UINT08 &byte)
      {
         DWORD bytesRead;
         bool  OK = 0 != ReadFile(_portHandle, &byte, 1, &bytesRead, NULL);
         if (bytesRead != 1) OK = false;
         return OK;
      }

   private:
      HANDLE _portHandle;
      UINT32 _portNumber;
      UINT32 _baudRate;
};

//------------------------------------------------------------------------------

bool readSignature(port &p)
{
   bool  foundSignature = false;
   bool  echoTestOnly   = true;

   char       *commands[]       = { "Echo test", "s", "S", "t", "T", "q", "Q", NULL };
   static char inputBuffer[128] = { 0 };

   int iCommand = 0;
   if (echoTestOnly) iCommand++;
   char *command = commands[iCommand];
   for ( ; command; iCommand++, command = commands[iCommand]) {
      printf("   Trying '%s'           \r", command); fflush(stdout);
      p.write(command);
      ::Sleep(100);
      int iBuf = 0;
      UINT08 b;
      while (p.read(b)) {
         inputBuffer[iBuf++] = b;
      }
      inputBuffer[iBuf] = '\0';
      if (iBuf > 0) {
         if (iBuf == 1) {
            if (inputBuffer[0] ==  1) strcpy(inputBuffer, "Hill & Fahlgren DualTable v. 1.??");
            if (inputBuffer[0] == 10) strcpy(inputBuffer, "Bowling & Grippo v. 1.000");
            if (inputBuffer[0] == 20) strcpy(inputBuffer, "Bowling & Grippo v. 2.xxx or 3.000");
         }

         printf("   \"%s\"\n", inputBuffer);
         foundSignature = true;
         break;
      }
   }

   return foundSignature;
}

//------------------------------------------------------------------------------

int main()
{
   printf("\n");
   int nValidPorts = 0;

   UINT32 baudRates[] = { 9600, 115200 };
   const int nBaudRates = sizeof(baudRates) / sizeof(UINT32);

   for (int portNumber = 0; portNumber < 10; portNumber++) {
      for (int iBaudRate = 0; iBaudRate < nBaudRates; iBaudRate++) {
         port p(portNumber, baudRates[iBaudRate]);
         HANDLE portHandle = p.open();
         if (p.isValid()) {
            if (iBaudRate == 0) {
               nValidPorts++;
               printf("COM%d Opened\n", portNumber);
            }
            printf("   %1d baud\n", baudRates[iBaudRate]);
            if (!readSignature(p)) printf("   No connection made.\n");

         }
         p.close();
      }
   }

   printf("\n%d valid port%s found.\n\n", nValidPorts, nValidPorts != 1?"s":"");
   printf("Hit return when done...");
   getchar();
   printf("\n");
   return 0;
}

//------------------------------------------------------------------------------
