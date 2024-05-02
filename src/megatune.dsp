# Microsoft Developer Studio Project File - Name="megatune" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=megatune - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "megatune.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "megatune.mak" CFG="megatune - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "megatune - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "megatune - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "megatune - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE F90 /include:"Release/"
# ADD F90 /browser /include:"Release/"
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ELSEIF  "$(CFG)" == "megatune - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE F90 /include:"Debug/"
# ADD F90 /include:"Debug/"
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "megatune - Win32 Release"
# Name "megatune - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\aeWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\AnalogMeter.cpp
# End Source File
# Begin Source File

SOURCE=.\Application.cpp
# End Source File
# Begin Source File

SOURCE=.\BarMeter.cpp
# End Source File
# Begin Source File

SOURCE=.\burstLog.cpp
# End Source File
# Begin Source File

SOURCE=.\byteString.cxx
# End Source File
# Begin Source File

SOURCE=.\COMSymbol.cpp
# End Source File
# Begin Source File

SOURCE=.\controller.cpp
# End Source File
# Begin Source File

SOURCE=.\curveEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\Dabout.cpp
# End Source File
# Begin Source File

SOURCE=.\dataDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\datalog.cxx
# End Source File
# Begin Source File

SOURCE=.\Dconplot.cpp
# End Source File
# Begin Source File

SOURCE=.\Dconstants.cpp
# End Source File
# Begin Source File

SOURCE=.\Denrichments.cpp
# End Source File
# Begin Source File

SOURCE=.\dirSelect.cxx
# End Source File
# Begin Source File

SOURCE=.\DlogOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\Dreqfuel.cpp
# End Source File
# Begin Source File

SOURCE=.\Druntime.cpp
# End Source File
# Begin Source File

SOURCE=.\Dsetcomm.cpp
# End Source File
# Begin Source File

SOURCE=.\DveGen.cpp
# End Source File
# Begin Source File

SOURCE=.\gaugeConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\genMAF.cpp
# End Source File
# Begin Source File

SOURCE=.\genO2.cpp
# End Source File
# Begin Source File

SOURCE=.\genTherm.cpp
# End Source File
# Begin Source File

SOURCE=.\hexedit.cpp
# End Source File
# Begin Source File

SOURCE=.\injControl.cpp
# End Source File
# Begin Source File

SOURCE=.\injectorSize.cpp
# End Source File
# Begin Source File

SOURCE=.\megatune.cpp
# End Source File
# Begin Source File

SOURCE=.\Megatune.idl
# ADD MTL /h "Megatune_i.h" /iid "Megatune_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\megatune.rc
# End Source File
# Begin Source File

SOURCE=.\megatuneDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\miniDOM.cpp
# End Source File
# Begin Source File

SOURCE=.\miniTerm.cpp
# End Source File
# Begin Source File

SOURCE=.\msComm.cpp
# End Source File
# Begin Source File

SOURCE=.\msDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\msqInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\parser.cxx
# End Source File
# Begin Source File

SOURCE=.\plot3d.cpp
# End Source File
# Begin Source File

SOURCE=.\portEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\repository.cpp
# End Source File
# Begin Source File

SOURCE=.\scaleVe.cpp
# End Source File
# Begin Source File

SOURCE=.\selectList.cpp
# End Source File
# Begin Source File

SOURCE=.\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\symbol.cpp
# End Source File
# Begin Source File

SOURCE=.\tableEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\tokenizer.cpp
# End Source File
# Begin Source File

SOURCE=.\tpgen.cpp
# End Source File
# Begin Source File

SOURCE=.\transformTable.cpp
# End Source File
# Begin Source File

SOURCE=.\triggerWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\tuning3D.cpp
# End Source File
# Begin Source File

SOURCE=.\units.cpp
# End Source File
# Begin Source File

SOURCE=.\userDefined.cpp
# End Source File
# Begin Source File

SOURCE=.\userHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\userMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\vegen.cxx
# End Source File
# Begin Source File

SOURCE=.\Vex.cpp
# End Source File
# Begin Source File

SOURCE=.\wueWizard.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\aeWizard.h
# End Source File
# Begin Source File

SOURCE=.\AnalogMeter.h
# End Source File
# Begin Source File

SOURCE=.\Application.h
# End Source File
# Begin Source File

SOURCE=.\BarMeter.h
# End Source File
# Begin Source File

SOURCE=.\burstLog.h
# End Source File
# Begin Source File

SOURCE=.\byteString.h
# End Source File
# Begin Source File

SOURCE=.\COMSymbol.h
# End Source File
# Begin Source File

SOURCE=.\controller.h
# End Source File
# Begin Source File

SOURCE=.\curveEditor.h
# End Source File
# Begin Source File

SOURCE=.\Dabout.h
# End Source File
# Begin Source File

SOURCE=.\dataDialog.h
# End Source File
# Begin Source File

SOURCE=.\datalog.h
# End Source File
# Begin Source File

SOURCE=.\Dconplot.h
# End Source File
# Begin Source File

SOURCE=.\Dconstants.h
# End Source File
# Begin Source File

SOURCE=.\Denrichments.h
# End Source File
# Begin Source File

SOURCE=.\dirSelect.h
# End Source File
# Begin Source File

SOURCE=.\DlogOptions.h
# End Source File
# Begin Source File

SOURCE=.\Dreqfuel.h
# End Source File
# Begin Source File

SOURCE=.\Druntime.h
# End Source File
# Begin Source File

SOURCE=.\Dsetcomm.h
# End Source File
# Begin Source File

SOURCE=.\DveGen.h
# End Source File
# Begin Source File

SOURCE=.\gaugeConfig.h
# End Source File
# Begin Source File

SOURCE=.\genMAF.h
# End Source File
# Begin Source File

SOURCE=.\genO2.h
# End Source File
# Begin Source File

SOURCE=.\genTherm.h
# End Source File
# Begin Source File

SOURCE=.\hexedit.h
# End Source File
# Begin Source File

SOURCE=.\injControl.h
# End Source File
# Begin Source File

SOURCE=.\injectorSize.h
# End Source File
# Begin Source File

SOURCE=.\megatune.h
# End Source File
# Begin Source File

SOURCE=.\megatuneDlg.h
# End Source File
# Begin Source File

SOURCE=.\miniDOM.h
# End Source File
# Begin Source File

SOURCE=.\miniTerm.h
# End Source File
# Begin Source File

SOURCE=.\msComm.h
# End Source File
# Begin Source File

SOURCE=.\msDatabase.h
# End Source File
# Begin Source File

SOURCE=.\msqInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\parser.h
# End Source File
# Begin Source File

SOURCE=.\plot3d.h
# End Source File
# Begin Source File

SOURCE=.\portEdit.h
# End Source File
# Begin Source File

SOURCE=.\repository.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\scaleVe.h
# End Source File
# Begin Source File

SOURCE=.\selectList.h
# End Source File
# Begin Source File

SOURCE=.\Splash.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\stringConstants.h
# End Source File
# Begin Source File

SOURCE=.\symbol.h
# End Source File
# Begin Source File

SOURCE=.\tableEditor.h
# End Source File
# Begin Source File

SOURCE=.\tokenizer.h
# End Source File
# Begin Source File

SOURCE=.\tpgen.h
# End Source File
# Begin Source File

SOURCE=.\transformTable.h
# End Source File
# Begin Source File

SOURCE=.\triggerWizard.h
# End Source File
# Begin Source File

SOURCE=.\tuning3D.h
# End Source File
# Begin Source File

SOURCE=.\units.h
# End Source File
# Begin Source File

SOURCE=.\userDefined.h
# End Source File
# Begin Source File

SOURCE=.\userHelp.h
# End Source File
# Begin Source File

SOURCE=.\userMenu.h
# End Source File
# Begin Source File

SOURCE=.\vegen.h
# End Source File
# Begin Source File

SOURCE=.\Vex.h
# End Source File
# Begin Source File

SOURCE=.\wueWizard.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\dn_arrow.bmp
# End Source File
# Begin Source File

SOURCE=.\res\maxq.ico
# End Source File
# Begin Source File

SOURCE=.\res\megatune.ico
# End Source File
# Begin Source File

SOURCE=.\Splash.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\up_arrow.bmp
# End Source File
# Begin Source File

SOURCE=.\res\wue_arrow.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Application.rgs
# End Source File
# Begin Source File

SOURCE=.\hlp\Megatune.hlp
# End Source File
# Begin Source File

SOURCE=.\Megatune.rgs
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section megatune : {E6E17E90-DF38-11CF-8E74-00A0C90F26F8}
# 	2:5:Class:CMSComm
# 	2:10:HeaderFile:mscomm.h
# 	2:8:ImplFile:mscomm.cpp
# End Section
# Section megatune : {648A5600-2C6E-101B-82B6-000000000014}
# 	2:21:DefaultSinkHeaderFile:mscomm.h
# 	2:16:DefaultSinkClass:CMSComm
# End Section
# Section megatune : {003A0014-001B-003B-0B00-3D0010003E00}
# 	1:13:IDR_MEGATUNE1:104
# End Section
