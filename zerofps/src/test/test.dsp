# Microsoft Developer Studio Project File - Name="test" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=test - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "test.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "test.mak" CFG="test - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "test - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "test - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "test - Win32 Release Profile" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "test - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x41d /d "NDEBUG"
# ADD RSC /l 0x41d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 opengl32.lib sdlmain.lib sdl.lib basic.lib render.lib engine.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"..\..\bin\test.exe" /libpath:"..\..\bin"
# SUBTRACT LINK32 /profile /incremental:yes

!ELSEIF  "$(CFG)" == "test - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x41d /d "_DEBUG"
# ADD RSC /l 0x41d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib sdlmain.lib sdl.lib basic.lib render.lib engine.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /out:"..\..\bin\test.exe" /libpath:"..\..\bin"
# SUBTRACT LINK32 /profile /force

!ELSEIF  "$(CFG)" == "test - Win32 Release Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "test___Win32_Release_Profile"
# PROP BASE Intermediate_Dir "test___Win32_Release_Profile"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseProfile"
# PROP Intermediate_Dir "ReleaseProfile"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x41d /d "NDEBUG"
# ADD RSC /l 0x41d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 opengl32.lib sdlmain.lib sdl.lib basic.lib render.lib engine.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /profile /machine:I386 /out:"..\..\bin\test.exe" /libpath:"..\..\bin"
# ADD LINK32 opengl32.lib sdlmain.lib sdl.lib basic.lib render.lib engine.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /profile /machine:I386 /out:"..\..\bin\test.exe" /libpath:"..\..\bin"

!ENDIF 

# Begin Target

# Name "test - Win32 Release"
# Name "test - Win32 Debug"
# Name "test - Win32 Release Profile"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ballobject.cpp

!IF  "$(CFG)" == "test - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "test - Win32 Debug"

!ELSEIF  "$(CFG)" == "test - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bunnyaiproperty.cpp

!IF  "$(CFG)" == "test - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "test - Win32 Debug"

!ELSEIF  "$(CFG)" == "test - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bunnyobject.cpp

!IF  "$(CFG)" == "test - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "test - Win32 Debug"

!ELSEIF  "$(CFG)" == "test - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\floatproperty.cpp

!IF  "$(CFG)" == "test - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "test - Win32 Debug"

!ELSEIF  "$(CFG)" == "test - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gravityproperty.cpp

!IF  "$(CFG)" == "test - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "test - Win32 Debug"

!ELSEIF  "$(CFG)" == "test - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\normaldrawproperty.cpp

!IF  "$(CFG)" == "test - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "test - Win32 Debug"

!ELSEIF  "$(CFG)" == "test - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\playerballobject.cpp

!IF  "$(CFG)" == "test - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "test - Win32 Debug"

!ELSEIF  "$(CFG)" == "test - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\playercontrolproperty.cpp

!IF  "$(CFG)" == "test - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "test - Win32 Debug"

!ELSEIF  "$(CFG)" == "test - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\playerobject.cpp

!IF  "$(CFG)" == "test - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "test - Win32 Debug"

!ELSEIF  "$(CFG)" == "test - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\test.cpp

!IF  "$(CFG)" == "test - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "test - Win32 Debug"

!ELSEIF  "$(CFG)" == "test - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\walker.cpp
# End Source File
# Begin Source File

SOURCE=.\walkeraiproperty.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
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

SOURCE=.\floatproperty.h
# End Source File
# Begin Source File

SOURCE=.\gravityproperty.h
# End Source File
# Begin Source File

SOURCE=.\normaldrawproperty.h
# End Source File
# Begin Source File

SOURCE=.\playerballobject.h
# End Source File
# Begin Source File

SOURCE=.\playercontrolproperty.h
# End Source File
# Begin Source File

SOURCE=.\playerobject.h
# End Source File
# Begin Source File

SOURCE=.\test.h
# End Source File
# Begin Source File

SOURCE=.\walker.h
# End Source File
# Begin Source File

SOURCE=.\walkeraiproperty.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
