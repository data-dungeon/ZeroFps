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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BASIC_EXPORTS" /FD /c
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
# ADD LINK32 lua.lib tolua.lib sdl.lib sdl_image.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../../bin/basic.dll"
# Begin Special Build Tool
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=copy                                 $(ProjDir)\release\*.lib                                                  ..\..\..\bin\ 
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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BASIC_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41d /d "_DEBUG"
# ADD RSC /l 0x41d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 sdl.lib sdl_image.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\bin\basic.dll" /pdbtype:sept
# Begin Special Build Tool
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  $(ProjDir)\debug\*.lib  ..\..\..\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "basic - Win32 Release"
# Name "basic - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\basicconsole.cpp
# End Source File
# Begin Source File

SOURCE=.\cmdsystem.cpp
# End Source File
# Begin Source File

SOURCE=.\globals.cpp
# End Source File
# Begin Source File

SOURCE=.\image.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix3.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix4.cpp
# End Source File
# Begin Source File

SOURCE=.\plane.cpp
# End Source File
# Begin Source File

SOURCE=.\quaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\rect.cpp
# End Source File
# Begin Source File

SOURCE=.\simplescript.cpp
# End Source File
# Begin Source File

SOURCE=.\vector3.cpp
# End Source File
# Begin Source File

SOURCE=.\vector4.cpp
# End Source File
# Begin Source File

SOURCE=.\zfbasicfs_linux.cpp
# End Source File
# Begin Source File

SOURCE=.\zfbasicfs_windows.cpp
# End Source File
# Begin Source File

SOURCE=.\zffile.cpp
# End Source File
# Begin Source File

SOURCE=.\zfini.cpp
# End Source File
# Begin Source File

SOURCE=.\zfiofile.cpp
# End Source File
# Begin Source File

SOURCE=.\zfmempackage.cpp
# End Source File
# Begin Source File

SOURCE=.\zfobject.cpp
# End Source File
# Begin Source File

SOURCE=.\zfobjectmanger.cpp
# End Source File
# Begin Source File

SOURCE=.\zfresource.cpp
# End Source File
# Begin Source File

SOURCE=.\zfresourcedb.cpp
# End Source File
# Begin Source File

SOURCE=.\zfvfs.cpp
# End Source File
# Begin Source File

SOURCE=.\zguifont.cpp
# End Source File
# Begin Source File

SOURCE=.\zguiskin.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\basic_x.h
# End Source File
# Begin Source File

SOURCE=.\basicconsole.h
# End Source File
# Begin Source File

SOURCE=.\basicconsole_script.h
# End Source File
# Begin Source File

SOURCE=.\cmdsystem.h
# End Source File
# Begin Source File

SOURCE=.\globals.h
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

SOURCE=.\plane.h
# End Source File
# Begin Source File

SOURCE=.\quaternion.h
# End Source File
# Begin Source File

SOURCE=.\rect.h
# End Source File
# Begin Source File

SOURCE=.\simplescript.h
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

SOURCE=.\zfbasicfs.h
# End Source File
# Begin Source File

SOURCE=.\zffile.h
# End Source File
# Begin Source File

SOURCE=.\zfini.h
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
# Begin Source File

SOURCE=.\zfresource.h
# End Source File
# Begin Source File

SOURCE=.\zfresourcedb.h
# End Source File
# Begin Source File

SOURCE=.\zfvfs.h
# End Source File
# Begin Source File

SOURCE=.\zguifont.h
# End Source File
# Begin Source File

SOURCE=.\zguiskin.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
