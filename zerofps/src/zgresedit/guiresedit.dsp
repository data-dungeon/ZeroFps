# Microsoft Developer Studio Project File - Name="ZGResEdit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ZGResEdit - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "guiresedit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "guiresedit.mak" CFG="ZGResEdit - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ZGResEdit - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ZGResEdit - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ZGResEdit - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x41d /d "NDEBUG"
# ADD RSC /l 0x41d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 opengl32.lib sdlmain.lib sdl.lib basic.lib render.lib engine.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /force /out:"..\..\bin\zgresedit.exe" /libpath:"..\..\bin"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ZGResEdit - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x41d /d "_DEBUG"
# ADD RSC /l 0x41d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib sdlmain.lib sdl.lib gui.lib basic.lib render.lib engine.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\..\bin\zgresedit.exe" /pdbtype:sept /libpath:"..\..\bin"
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "ZGResEdit - Win32 Release"
# Name "ZGResEdit - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\controlbox.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogbox.cpp
# End Source File
# Begin Source File

SOURCE=..\zeroedit\fileopendlg.cpp
# End Source File
# Begin Source File

SOURCE=.\guibuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\msgbox.cpp
# End Source File
# Begin Source File

SOURCE=.\propertybox.cpp
# End Source File
# Begin Source File

SOURCE=.\selectwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\serialization.cpp
# End Source File
# Begin Source File

SOURCE=.\skinbox.cpp
# End Source File
# Begin Source File

SOURCE=.\skintexinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\zgresedit.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\controlbox.h
# End Source File
# Begin Source File

SOURCE=.\dialogbox.h
# End Source File
# Begin Source File

SOURCE=..\zeroedit\fileopendlg.h
# End Source File
# Begin Source File

SOURCE=.\guibuilder.h
# End Source File
# Begin Source File

SOURCE=.\msgbox.h
# End Source File
# Begin Source File

SOURCE=.\propertybox.h
# End Source File
# Begin Source File

SOURCE=.\resource_id.h
# End Source File
# Begin Source File

SOURCE=.\selectwnd.h
# End Source File
# Begin Source File

SOURCE=.\serialization.h
# End Source File
# Begin Source File

SOURCE=.\skinbox.h
# End Source File
# Begin Source File

SOURCE=.\skintexinfo.h
# End Source File
# Begin Source File

SOURCE=.\zgresedit.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\mainicon.ico
# End Source File
# End Group
# End Target
# End Project
