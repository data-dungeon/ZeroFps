# Microsoft Developer Studio Project File - Name="mcommon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mcommon - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mcommon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mcommon.mak" CFG="mcommon - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mcommon - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mcommon - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mcommon - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MCOMMON_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MCOMMON_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41d /d "NDEBUG"
# ADD RSC /l 0x41d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "mcommon - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MCOMMON_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MCOMMON_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41d /d "_DEBUG"
# ADD RSC /l 0x41d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 engine.lib render.lib script.lib basic.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\bin\mcommon.dll" /pdbtype:sept /libpath:"..\..\bin"
# Begin Special Build Tool
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  $(ProjDir)\debug\*.lib ..\..\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "mcommon - Win32 Release"
# Name "mcommon - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\p_charstats.cpp
# End Source File
# Begin Source File

SOURCE=.\p_event.cpp
# End Source File
# Begin Source File

SOURCE=.\p_ml.cpp
# End Source File
# Begin Source File

SOURCE=.\p_serverinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\si_mistland.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\mcommon_x.h
# End Source File
# Begin Source File

SOURCE=.\p_charstats.h
# End Source File
# Begin Source File

SOURCE=.\p_event.h
# End Source File
# Begin Source File

SOURCE=.\p_ml.h
# End Source File
# Begin Source File

SOURCE=.\p_serverinfo.h
# End Source File
# Begin Source File

SOURCE=.\si_mistland.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "RuleSystem"

# PROP Default_Filter ""
# Begin Group "character"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\rulesystem\character\characterfactory.cpp
# End Source File
# Begin Source File

SOURCE=.\rulesystem\character\characterfactory.h
# End Source File
# Begin Source File

SOURCE=.\rulesystem\character\characterstats.cpp
# End Source File
# Begin Source File

SOURCE=.\rulesystem\character\characterstats.h
# End Source File
# Begin Source File

SOURCE=.\rulesystem\character\statcounter.cpp
# End Source File
# Begin Source File

SOURCE=.\rulesystem\character\statcounter.h
# End Source File
# End Group
# Begin Group "item"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\rulesystem\item\itemdatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\rulesystem\item\itemdatabase.h
# End Source File
# Begin Source File

SOURCE=.\rulesystem\item\itemstats.h
# End Source File
# End Group
# Begin Group "spell"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\rulesystem\spell\spellstats.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\rulesystem\rulesystem.cpp
# End Source File
# Begin Source File

SOURCE=.\rulesystem\rulesystem.h
# End Source File
# Begin Source File

SOURCE=.\rulesystem\rulesystemx.h
# End Source File
# End Group
# End Target
# End Project
