# Microsoft Developer Studio Project File - Name="gui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=gui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gui.mak" CFG="gui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gui - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gui - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gui - Win32 Release Profile" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gui - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GUI_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GUI_EXPORTS" /FD /c
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
# ADD LINK32 sdl.lib basic.lib engine.lib render.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\..\..\bin\gui.dll" /libpath:"..\..\..\bin"
# Begin Special Build Tool
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  $(ProjDir)\release\*.lib ..\..\..\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GUI_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GUI_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41d /d "_DEBUG"
# ADD RSC /l 0x41d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 basic.lib engine.lib render.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\bin\gui.dll" /libpath:"..\..\..\bin"
# SUBTRACT LINK32 /profile /map
# Begin Special Build Tool
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  $(ProjDir)\debug\*.lib ..\..\..\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "gui - Win32 Release Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gui___Win32_Release_Profile"
# PROP BASE Intermediate_Dir "gui___Win32_Release_Profile"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseProfile"
# PROP Intermediate_Dir "ReleaseProfile"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GUI_EXPORTS" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GUI_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41d /d "NDEBUG"
# ADD RSC /l 0x41d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 sdl.lib basic.lib engine.lib render.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\..\..\bin\gui.dll" /libpath:"..\..\..\bin"
# ADD LINK32 sdl.lib basic.lib engine.lib render.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /profile /map /machine:I386 /out:"..\..\..\bin\gui.dll" /libpath:"..\..\..\bin"
# Begin Special Build Tool
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  $(ProjDir)\releaseprofile\*.lib ..\..\..\bin	copy $(ProjDir)\releaseprofile\*.map  ..\..\..\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "gui - Win32 Release"
# Name "gui - Win32 Debug"
# Name "gui - Win32 Release Profile"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\zgui.cpp
# End Source File
# Begin Source File

SOURCE=.\zguibutton.cpp
# End Source File
# Begin Source File

SOURCE=.\zguicheckbox.cpp
# End Source File
# Begin Source File

SOURCE=.\zguicombobox.cpp
# End Source File
# Begin Source File

SOURCE=.\zguicursor.cpp
# End Source File
# Begin Source File

SOURCE=.\zguilabel.cpp
# End Source File
# Begin Source File

SOURCE=.\zguilistbox.cpp
# End Source File
# Begin Source File

SOURCE=.\zguilistitem.cpp
# End Source File
# Begin Source File

SOURCE=.\zguiradiobutton.cpp
# End Source File
# Begin Source File

SOURCE=.\zguiresourcemanager.cpp
# End Source File
# Begin Source File

SOURCE=.\zguiscrollbar.cpp
# End Source File
# Begin Source File

SOURCE=.\zguislider.cpp
# End Source File
# Begin Source File

SOURCE=.\zguitabctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\zguitextbox.cpp
# End Source File
# Begin Source File

SOURCE=.\zguitreebox.cpp
# End Source File
# Begin Source File

SOURCE=.\zguiwindow.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\gui.pkg
# End Source File
# Begin Source File

SOURCE=.\gui_x.h
# End Source File
# Begin Source File

SOURCE=.\zgui.h
# End Source File
# Begin Source File

SOURCE=.\zguibutton.h
# End Source File
# Begin Source File

SOURCE=.\zguicheckbox.h
# End Source File
# Begin Source File

SOURCE=.\zguicombobox.h
# End Source File
# Begin Source File

SOURCE=.\zguicursor.h
# End Source File
# Begin Source File

SOURCE=.\zguilabel.h
# End Source File
# Begin Source File

SOURCE=.\zguilistbox.h
# End Source File
# Begin Source File

SOURCE=.\zguilistitem.h
# End Source File
# Begin Source File

SOURCE=.\zguiradiobutton.h
# End Source File
# Begin Source File

SOURCE=.\zguiresourcemanager.h
# End Source File
# Begin Source File

SOURCE=.\zguiscrollbar.h
# End Source File
# Begin Source File

SOURCE=.\zguislider.h
# End Source File
# Begin Source File

SOURCE=.\zguitabctrl.h
# End Source File
# Begin Source File

SOURCE=.\zguitextbox.h
# End Source File
# Begin Source File

SOURCE=.\zguitreebox.h
# End Source File
# Begin Source File

SOURCE=.\zguiwindow.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
