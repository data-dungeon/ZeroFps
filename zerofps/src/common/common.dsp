# Microsoft Developer Studio Project File - Name="common" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=common - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "common.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "common.mak" CFG="common - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "common - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "common - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "common - Win32 Release Profile" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "common - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMMON_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMMON_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41d /d "NDEBUG"
# ADD RSC /l 0x41d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 sdl.lib gui.lib engine.lib basic.lib render.lib script.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../bin/common.dll" /libpath:"../../bin"
# Begin Special Build Tool
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=copy                       $(ProjDir)\release\*.lib                       ..\..\bin\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "common - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMMON_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMMON_EXPORTS" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41d /d "_DEBUG"
# ADD RSC /l 0x41d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 engine.lib basic.lib gui.lib render.lib script.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /out:"../../bin/common.dll" /libpath:"../../bin"
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=copy                                   $(ProjDir)\debug\*.lib                                   ..\..\bin\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "common - Win32 Release Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "common___Win32_Release_Profile"
# PROP BASE Intermediate_Dir "common___Win32_Release_Profile"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseProfile"
# PROP Intermediate_Dir "ReleaseProfile"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMMON_EXPORTS" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMMON_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41d /d "NDEBUG"
# ADD RSC /l 0x41d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 sdl.lib engine.lib basic.lib gui.lib render.lib script.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../bin/common.dll" /libpath:"../../bin"
# ADD LINK32 sdl.lib engine.lib basic.lib gui.lib render.lib script.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /profile /map /machine:I386 /out:"../../bin/common.dll" /libpath:"../../bin"
# Begin Special Build Tool
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(ProjDir)\releaseprofile\*.lib ..\..\bin	copy $(ProjDir)\releaseprofile\*.map  ..\..\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "common - Win32 Release"
# Name "common - Win32 Debug"
# Name "common - Win32 Release Profile"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\adaptorsniper.cpp
# End Source File
# Begin Source File

SOURCE=.\ballobject.cpp
# End Source File
# Begin Source File

SOURCE=.\bunnyaiproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\bunnyobject.cpp
# End Source File
# Begin Source File

SOURCE=.\constructionmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\container.cpp
# End Source File
# Begin Source File

SOURCE=.\containerproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogbox.cpp
# End Source File
# Begin Source File

SOURCE=.\doorproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\effectsystem.cpp
# End Source File
# Begin Source File

SOURCE=.\externalcommand.cpp
# End Source File
# Begin Source File

SOURCE=.\fh.cpp
# End Source File
# Begin Source File

SOURCE=.\itemproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\logproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\machinegunprojectile.cpp
# End Source File
# Begin Source File

SOURCE=.\machinegunproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\massdriverprojectile.cpp
# End Source File
# Begin Source File

SOURCE=.\massdriverproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\p_clientinput.cpp
# End Source File
# Begin Source File

SOURCE=.\p_clientunit.cpp
# End Source File
# Begin Source File

SOURCE=.\p_fogrender.cpp
# End Source File
# Begin Source File

SOURCE=.\p_renderselection.cpp
# End Source File
# Begin Source File

SOURCE=.\p_serverinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\p_serverunit.cpp
# End Source File
# Begin Source File

SOURCE=.\p_suplygiver.cpp
# End Source File
# Begin Source File

SOURCE=.\p_unitattackai.cpp
# End Source File
# Begin Source File

SOURCE=.\p_unitbuildai.cpp
# End Source File
# Begin Source File

SOURCE=.\p_unitmoveai.cpp
# End Source File
# Begin Source File

SOURCE=.\p_unitsystem.cpp
# End Source File
# Begin Source File

SOURCE=.\pathbuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\pathfind.cpp
# End Source File
# Begin Source File

SOURCE=.\playercontrolproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\playerobject.cpp
# End Source File
# Begin Source File

SOURCE=.\statusproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\teleportproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\tileengine.cpp
# End Source File
# Begin Source File

SOURCE=.\treeidleproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\unitcommand.cpp
# End Source File
# Begin Source File

SOURCE=.\unitcommandinfo.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\adaptorsniper.h
# End Source File
# Begin Source File

SOURCE=.\aibase.h
# End Source File
# Begin Source File

SOURCE=.\ballobject.h
# End Source File
# Begin Source File

SOURCE=.\bunnyaiproperty.h
# End Source File
# Begin Source File

SOURCE=.\bunnyobject.h
# End Source File
# Begin Source File

SOURCE=.\common.pkg
# End Source File
# Begin Source File

SOURCE=.\common_x.h
# End Source File
# Begin Source File

SOURCE=.\constructionmanager.h
# End Source File
# Begin Source File

SOURCE=.\container.h
# End Source File
# Begin Source File

SOURCE=.\containerproperty.h
# End Source File
# Begin Source File

SOURCE=.\dialogbox.h
# End Source File
# Begin Source File

SOURCE=.\doorproperty.h
# End Source File
# Begin Source File

SOURCE=.\effectsystem.h
# End Source File
# Begin Source File

SOURCE=.\externalcommand.h
# End Source File
# Begin Source File

SOURCE=.\fh.h
# End Source File
# Begin Source File

SOURCE=.\inventoryproperty.h
# End Source File
# Begin Source File

SOURCE=.\itemproperty.h
# End Source File
# Begin Source File

SOURCE=.\logproperty.h
# End Source File
# Begin Source File

SOURCE=.\machinegunprojectile.h
# End Source File
# Begin Source File

SOURCE=.\machinegunproperty.h
# End Source File
# Begin Source File

SOURCE=.\massdriverprojectile.h
# End Source File
# Begin Source File

SOURCE=.\massdriverproperty.h
# End Source File
# Begin Source File

SOURCE=.\p_clientinput.h
# End Source File
# Begin Source File

SOURCE=.\p_clientunit.h
# End Source File
# Begin Source File

SOURCE=.\p_fogrender.h
# End Source File
# Begin Source File

SOURCE=.\p_renderselection.h
# End Source File
# Begin Source File

SOURCE=.\p_serverinfo.h
# End Source File
# Begin Source File

SOURCE=.\p_serverunit.h
# End Source File
# Begin Source File

SOURCE=.\p_suplygiver.h
# End Source File
# Begin Source File

SOURCE=.\p_unitattackai.h
# End Source File
# Begin Source File

SOURCE=.\p_unitbuildai.h
# End Source File
# Begin Source File

SOURCE=.\p_unitmoveai.h
# End Source File
# Begin Source File

SOURCE=.\p_unitsystem.h
# End Source File
# Begin Source File

SOURCE=.\pathbuilder.h
# End Source File
# Begin Source File

SOURCE=.\pathfind.h
# End Source File
# Begin Source File

SOURCE=.\playercontrolproperty.h
# End Source File
# Begin Source File

SOURCE=.\playerobject.h
# End Source File
# Begin Source File

SOURCE=.\statusproperty.h
# End Source File
# Begin Source File

SOURCE=.\teleportproperty.h
# End Source File
# Begin Source File

SOURCE=.\tileengine.h
# End Source File
# Begin Source File

SOURCE=.\treeidleproperty.h
# End Source File
# Begin Source File

SOURCE=.\unitcommand.h
# End Source File
# Begin Source File

SOURCE=.\unitcommandinfo.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
