# Microsoft Developer Studio Project File - Name="basic" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=basic - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "basic.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "basic.mak" CFG="basic - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "basic - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "basic - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "basic - Win32 Release Profile" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "basic - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BASIC_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BASIC_EXPORTS" /FD /c
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
# ADD LINK32 sdl.lib sdl_image.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../../bin/basic.dll" /libpath:"..\..\..\bin"
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=copy                                 $(ProjDir)\debug\*.lib                                 ..\..\..\bin\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "basic - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BASIC_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BASIC_EXPORTS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41d /d "_DEBUG"
# ADD RSC /l 0x41d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 sdl.lib sdl_image.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /profile /debug /machine:I386 /out:"../../../bin/basic.dll"
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=copy                             $(ProjDir)\debug\*.lib                             ..\..\..\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "basic - Win32 Release Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "basic___Win32_Release_Profile"
# PROP BASE Intermediate_Dir "basic___Win32_Release_Profile"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseProfile"
# PROP Intermediate_Dir "ReleaseProfile"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BASIC_EXPORTS" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BASIC_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41d /d "NDEBUG"
# ADD RSC /l 0x41d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 sdl.lib sdl_image.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../../bin/basic.dll" /libpath:"..\..\..\bin"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 sdl.lib sdl_image.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /profile /machine:I386 /out:"../../../bin/basic.dll" /libpath:"..\..\..\bin"
# Begin Special Build Tool
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=copy                                 $(ProjDir)\debug\*.lib                                 ..\..\..\bin\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "basic - Win32 Release"
# Name "basic - Win32 Debug"
# Name "basic - Win32 Release Profile"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\basicconsole.cpp
# End Source File
# Begin Source File

SOURCE=.\cmdsystem.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\fileio.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\globals.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\halfedge.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\heightmap.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\image.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix3.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\matrix4.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\quaternion.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\vector3.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\vector4.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\zffile.cpp
# End Source File
# Begin Source File

SOURCE=.\zfiofile.cpp
# End Source File
# Begin Source File

SOURCE=.\zfobject.cpp

!IF  "$(CFG)" == "basic - Win32 Release"

!ELSEIF  "$(CFG)" == "basic - Win32 Debug"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "basic - Win32 Release Profile"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zfobjectmanger.cpp

!IF  "$(CFG)" == "basic - Win32 Release"

!ELSEIF  "$(CFG)" == "basic - Win32 Debug"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "basic - Win32 Release Profile"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\basic.pkg
# End Source File
# Begin Source File

SOURCE=.\basic_x.h
# End Source File
# Begin Source File

SOURCE=.\basicconsole.h
# End Source File
# Begin Source File

SOURCE=.\basicmath.pkg
# End Source File
# Begin Source File

SOURCE=.\cmdsystem.h
# End Source File
# Begin Source File

SOURCE=.\fileio.h
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\halfedge.h
# End Source File
# Begin Source File

SOURCE=.\heightmap.h
# End Source File
# Begin Source File

SOURCE=.\image.h
# End Source File
# Begin Source File

SOURCE=.\matrix3.h
# End Source File
# Begin Source File

SOURCE=.\matrix4.h
# End Source File
# Begin Source File

SOURCE=.\os.h
# End Source File
# Begin Source File

SOURCE=.\os_linux.h
# End Source File
# Begin Source File

SOURCE=.\os_win32.h
# End Source File
# Begin Source File

SOURCE=.\quaternion.h
# End Source File
# Begin Source File

SOURCE=.\vector3.h
# End Source File
# Begin Source File

SOURCE=.\vector4.h
# End Source File
# Begin Source File

SOURCE=.\zfassert.h
# End Source File
# Begin Source File

SOURCE=.\zffile.h
# End Source File
# Begin Source File

SOURCE=.\zfio.h
# End Source File
# Begin Source File

SOURCE=.\zfiofile.h
# End Source File
# Begin Source File

SOURCE=.\zfmempackage.h
# End Source File
# Begin Source File

SOURCE=.\zfobject.h
# End Source File
# Begin Source File

SOURCE=.\zfobjectmanger.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
