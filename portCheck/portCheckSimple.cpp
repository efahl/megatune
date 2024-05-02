// cl portCheck.cpp

#include <windows.h>
#include <stdio.h>
#include <assert.h>

bool isPortValid(const char *portName)
{
   HANDLE _hPortHandle = CreateFile(portName,
                    GENERIC_READ | GENERIC_WRITE,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);
   if (_hPortHandle == INVALID_HANDLE_VALUE) return false;

   bool exists = true;

   DCB  dcb    = { 0 };
   if (!GetCommState(_hPortHandle, &dcb)) exists = false;

   CloseHandle(_hPortHandle);
   return exists;
}

int main()
{
   printf("\n");
   int nPorts = 0;
   for (int _commPortNumber = 0; _commPortNumber < 100; _commPortNumber++) {
      char pPortName[6];
      sprintf(pPortName, "COM%d", _commPortNumber);
      if (isPortValid(pPortName)) {
         nPorts++;
         printf("%s OK\n", pPortName);
      }
   }
   printf("%d valid ports found.\n\n", nPorts);
   printf("Hit return when done...");
   getchar();
   return 0;
}
