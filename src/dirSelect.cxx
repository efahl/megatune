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

#include <io.h>
#include <direct.h>
#include <sys/stat.h>

#include "megatune.h"
#include "dirSelect.h"
#include "selectList.h"

//------------------------------------------------------------------------------

extern CString installationDir; // Defined in megatune.cpp
extern CString commonDir;

void dirSelect::init()
{
   bool restartOnZero = true;
restart:
   _path pathName;
   _getcwd(pathName, sizeof(pathName));
   char *s;
   while (s = strchr(pathName, '\\')) *s = '/';

   // Go up one if we are in the startup directory.
   if (_stricmp(pathName, installationDir) == 0) {
      _chdir("..");
      _getcwd(pathName, sizeof(pathName));
      while (s = strchr(pathName, '\\')) *s = '/';
   }

   struct _stat dirStats;
   int status = _stat(pathName, &dirStats);
   if (status || !(_S_IFDIR & dirStats.st_mode)) {
      // Some error, handle it.
   }

   _path dirSpec;
   sprintf(dirSpec, "%s/mtCfg", pathName);
   if (commonDir != dirSpec) {
//    msgOk("whatever", commonDir + "\n" + dirSpec);
      status = _stat(dirSpec, &dirStats);
      if (status == 0 && (_S_IFDIR & dirStats.st_mode)) {
//       msgOk("MegaTune Project", "Already there %s", pathName);
         selected = pathName;
         return; // We are starting in the appropriate directory.
      }
   }

   sprintf(dirSpec, "%s/*.*", pathName);

   struct _finddata_t thisEntry;
   long               dirHandle;
   if ((dirHandle = _findfirst(dirSpec, &thisEntry)) != -1) {
      do {
         if (strcmp(thisEntry.name, "." ) == 0) continue;
         if (strcmp(thisEntry.name, "..") == 0) continue;
         // Ignore all MT installations.
         if (_strnicmp(thisEntry.name, "MegaTune", 8) == 0) continue;

         _path path;
         sprintf(path, "%s/%s/mtCfg", pathName, thisEntry.name);

         status = _stat(path, &dirStats);
         if (status == 0) {
            if (_S_IFDIR & dirStats.st_mode) {
               sprintf(path, "%s/%s", pathName, thisEntry.name);
               add(thisEntry.name, path);
            }
         }
      }
      while (_findnext(dirHandle, &thisEntry) == 0);
      _findclose(dirHandle);
   }

   if (restartOnZero && nDirs() == 0) {
      // When started as a COM service, we are usually in $windir/system32,
      // which gets us nowhere.  We will try to restart in the install dir.
      _chdir(installationDir);
      _chdir("..");
      restartOnZero = false;
      goto restart;
   }

   //printf("nDirs: %d\n", nDirs());
   if (nDirs() == 1) {
      _chdir(entry[0]->path);
      selected = entry[0]->path;
//    msgOk(entry[0]->name, "Single start %s", entry[0]->path);
   }
   else {
      selectList sl(NULL, this, "Select Project");
      sl.setCaption("Select a project...");
      int idx = sl.DoModal();
      if (idx < 0) {
         //msgOk("MegaTune Exit", "No MegaTune project selected.\n\nTerminating...");
         exit(0);
      }
      _chdir(entry[idx]->path);
      selected = entry[idx]->path;
      selected.Replace("\\", "/");
//    msgOk(entry[idx]->name, "Selected start in %s", entry[idx]->path);
   }
}

//------------------------------------------------------------------------------

dirSelect::~dirSelect()
{
   for (int i = 0; i < nDirs(); i++) {
      delete entry[i];
   }
}

//------------------------------------------------------------------------------
#if 0
int main()
{
   _chdir("c:/Program Files/MegaSquirt"); // "Start in:" directory for shortcut.
   setDir();
   return 0;
}
#endif
