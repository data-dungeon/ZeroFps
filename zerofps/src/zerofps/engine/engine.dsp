# Microsoft Developer Studio Project File - Name="engine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=engine - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "engine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "engine.mak" CFG="engine - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "engine - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "engine - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "engine - Win32 Release Profile" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "engine - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /FD /c
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
# ADD LINK32 sdl_net.lib OpenAL32.lib ALut.lib render.lib basic.lib glu32.lib opengl32.lib sdl_mixer.lib sdl.lib sdlmain.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\..\..\bin\engine.dll" /libpath:"..\..\..\bin"
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=copy                $(ProjDir)\release\*.lib                                 ..\..\..\bin\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41d /d "_DEBUG"
# ADD RSC /l 0x41d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 sdl_net.lib OpenAL32.lib ALut.lib render.lib basic.lib glu32.lib opengl32.lib sdl_mixer.lib sdl.lib sdlmain.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /profile /debug /machine:I386 /out:"..\..\..\bin\engine.dll" /libpath:"..\..\..\bin"
# Begin Special Build Tool
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=copy                                                                     $(ProjDir)\debug\*.lib                                                                     ..\..\..\bin\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "engine___Win32_Release_Profile"
# PROP BASE Intermediate_Dir "engine___Win32_Release_Profile"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseProfile"
# PROP Intermediate_Dir "ReleaseProfile"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "RUNPROFILE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41d /d "NDEBUG"
# ADD RSC /l 0x41d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 render.lib basic.lib glu32.lib opengl32.lib sdl_mixer.lib sdl.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\..\..\bin\engine.dll" /libpath:"..\..\..\bin"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 sdl_net.lib OpenAL32.lib ALut.lib render.lib basic.lib glu32.lib opengl32.lib sdl_mixer.lib sdl.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /profile /map /machine:I386 /out:"..\..\..\bin\engine.dll" /libpath:"..\..\..\bin"
# Begin Special Build Tool
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(ProjDir)\releaseprofile\*.lib ..\..\..\bin\	copy $(ProjDir)\releaseprofile\*.map  ..\..\..\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "engine - Win32 Release"
# Name "engine - Win32 Debug"
# Name "engine - Win32 Release Profile"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\application.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\audiomanager.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\autoparentproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\camera.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cameraproperty.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\collisionheightmap.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\collisionmanager.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\collisionpoint.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\collisionproperty.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\collisionshape.cpp
# End Source File
# Begin Source File

SOURCE=.\collisionsphere.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\console.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\crossrenderproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\cssphere.cpp
# End Source File
# Begin Source File

SOURCE=..\ogl\extgl.c

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fh.cpp
# End Source File
# Begin Source File

SOURCE=.\heightmapcs.cpp
# End Source File
# Begin Source File

SOURCE=.\heightmapobject.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\heightmaprenderproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\hmrp2.cpp
# End Source File
# Begin Source File

SOURCE=.\input.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\levelmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\lightproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\lightupdateproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\mad_modell.cpp
# End Source File
# Begin Source File

SOURCE=.\madproperty.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\main.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\modelproperty.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\music.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\netslaveobject.cpp
# End Source File
# Begin Source File

SOURCE=.\network.cpp
# End Source File
# Begin Source File

SOURCE=.\object.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\objectmanager.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\openalsystem.cpp
# End Source File
# Begin Source File

SOURCE=.\physicproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\physicsengine.cpp
# End Source File
# Begin Source File

SOURCE=.\property.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\propertyfactory.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\proxyproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\skyboxobject.cpp
# End Source File
# Begin Source File

SOURCE=.\skyboxrenderproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\soundbuffermanager.cpp
# End Source File
# Begin Source File

SOURCE=.\waterobject.cpp
# End Source File
# Begin Source File

SOURCE=.\waterrenderproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\worldinfoproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\zerofps.cpp

!IF  "$(CFG)" == "engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ELSEIF  "$(CFG)" == "engine - Win32 Release Profile"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zgui\zgui.cpp
# End Source File
# Begin Source File

SOURCE=.\zgui\zguibutton.cpp
# End Source File
# Begin Source File

SOURCE=.\zgui\zguicheckbox.cpp
# End Source File
# Begin Source File

SOURCE=.\zgui\zguicombobox.cpp
# End Source File
# Begin Source File

SOURCE=.\zgui\zguicontrol.cpp
# End Source File
# Begin Source File

SOURCE=.\zgui\zguicursor.cpp
# End Source File
# Begin Source File

SOURCE=.\zgui\zguilabel.cpp
# End Source File
# Begin Source File

SOURCE=.\zgui\zguilistbox.cpp
# End Source File
# Begin Source File

SOURCE=.\zgui\zguilistitem.cpp
# End Source File
# Begin Source File

SOURCE=.\zgui\zguiradiobutton.cpp
# End Source File
# Begin Source File

SOURCE=.\zgui\zguiresourcemanager.cpp
# End Source File
# Begin Source File

SOURCE=.\zgui\zguiscrollbar.cpp
# End Source File
# Begin Source File

SOURCE=.\zgui\zguitextbox.cpp
# End Source File
# Begin Source File

SOURCE=.\zgui\zguiwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\zoneobject.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\application.h
# End Source File
# Begin Source File

SOURCE=.\audiomanager.h
# End Source File
# Begin Source File

SOURCE=.\autoparentproperty.h
# End Source File
# Begin Source File

SOURCE=.\camera.h
# End Source File
# Begin Source File

SOURCE=.\cameraproperty.h
# End Source File
# Begin Source File

SOURCE=.\collisionheightmap.h
# End Source File
# Begin Source File

SOURCE=.\collisionmanager.h
# End Source File
# Begin Source File

SOURCE=.\collisionobject.h
# End Source File
# Begin Source File

SOURCE=.\collisionpoint.h
# End Source File
# Begin Source File

SOURCE=.\collisionproperty.h
# End Source File
# Begin Source File

SOURCE=.\collisionshape.h
# End Source File
# Begin Source File

SOURCE=.\collisionsphere.h
# End Source File
# Begin Source File

SOURCE=.\console.h
# End Source File
# Begin Source File

SOURCE=.\crossrenderproperty.h
# End Source File
# Begin Source File

SOURCE=.\cssphere.h
# End Source File
# Begin Source File

SOURCE=.\engine.pkg
# End Source File
# Begin Source File

SOURCE=.\engine_x.h
# End Source File
# Begin Source File

SOURCE=..\ogl\extgl.h
# End Source File
# Begin Source File

SOURCE=.\fh.h
# End Source File
# Begin Source File

SOURCE=.\heightmapcs.h
# End Source File
# Begin Source File

SOURCE=.\heightmapobject.h
# End Source File
# Begin Source File

SOURCE=.\heightmaprenderproperty.h
# End Source File
# Begin Source File

SOURCE=.\hmrp2.h
# End Source File
# Begin Source File

SOURCE=.\input.h
# End Source File
# Begin Source File

SOURCE=.\levelmanager.h
# End Source File
# Begin Source File

SOURCE=.\lightproperty.h
# End Source File
# Begin Source File

SOURCE=.\lightupdateproperty.h
# End Source File
# Begin Source File

SOURCE=.\mad_modell.h
# End Source File
# Begin Source File

SOURCE=.\madproperty.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\modelproperty.h
# End Source File
# Begin Source File

SOURCE=.\music.h
# End Source File
# Begin Source File

SOURCE=.\netslaveobject.h
# End Source File
# Begin Source File

SOURCE=.\network.h
# End Source File
# Begin Source File

SOURCE=.\object.h
# End Source File
# Begin Source File

SOURCE=.\objectmanager.h
# End Source File
# Begin Source File

SOURCE=.\objects.pkg
# End Source File
# Begin Source File

SOURCE=.\openalsystem.h
# End Source File
# Begin Source File

SOURCE=.\physicproperty.h
# End Source File
# Begin Source File

SOURCE=.\physicsengine.h
# End Source File
# Begin Source File

SOURCE=.\property.h
# End Source File
# Begin Source File

SOURCE=.\propertyfactory.h
# End Source File
# Begin Source File

SOURCE=.\proxyproperty.h
# End Source File
# Begin Source File

SOURCE=.\skyboxobject.h
# End Source File
# Begin Source File

SOURCE=.\skyboxrenderproperty.h
# End Source File
# Begin Source File

SOURCE=.\soundbuffermanager.h
# End Source File
# Begin Source File

SOURCE=.\waterobject.h
# End Source File
# Begin Source File

SOURCE=.\waterrenderproperty.h
# End Source File
# Begin Source File

SOURCE=.\worldinfoproperty.h
# End Source File
# Begin Source File

SOURCE=.\zerofps.h
# End Source File
# Begin Source File

SOURCE=..\ogl\zfpsgl.h
# End Source File
# Begin Source File

SOURCE=.\zgui\zgui.h
# End Source File
# Begin Source File

SOURCE=.\zgui\zguibutton.h
# End Source File
# Begin Source File

SOURCE=..\zgui\zguibutton.h
# End Source File
# Begin Source File

SOURCE=.\zgui\zguicheckbox.h
# End Source File
# Begin Source File

SOURCE=..\zgui\zguicheckbox.h
# End Source File
# Begin Source File

SOURCE=.\zgui\zguicombobox.h
# End Source File
# Begin Source File

SOURCE=..\zgui\zguicombobox.h
# End Source File
# Begin Source File

SOURCE=.\zgui\zguicontrol.h
# End Source File
# Begin Source File

SOURCE=..\zgui\zguicontrol.h
# End Source File
# Begin Source File

SOURCE=.\zgui\zguicursor.h
# End Source File
# Begin Source File

SOURCE=.\zgui\zguilabel.h
# End Source File
# Begin Source File

SOURCE=..\zgui\zguilabel.h
# End Source File
# Begin Source File

SOURCE=.\zgui\zguilistbox.h
# End Source File
# Begin Source File

SOURCE=..\zgui\zguilistbox.h
# End Source File
# Begin Source File

SOURCE=.\zgui\zguilistitem.h
# End Source File
# Begin Source File

SOURCE=..\zgui\zguilistitem.h
# End Source File
# Begin Source File

SOURCE=.\zgui\zguiradiobutton.h
# End Source File
# Begin Source File

SOURCE=..\zgui\zguiradiobutton.h
# End Source File
# Begin Source File

SOURCE=.\zgui\zguiresourcemanager.h
# End Source File
# Begin Source File

SOURCE=.\zgui\zguiscrollbar.h
# End Source File
# Begin Source File

SOURCE=..\zgui\zguiscrollbar.h
# End Source File
# Begin Source File

SOURCE=.\zgui\zguitextbox.h
# End Source File
# Begin Source File

SOURCE=..\zgui\zguitextbox.h
# End Source File
# Begin Source File

SOURCE=.\zgui\zguiwindow.h
# End Source File
# Begin Source File

SOURCE=..\zgui\zguiwindow.h
# End Source File
# Begin Source File

SOURCE=.\zoneobject.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\property.txt
# End Source File
# End Target
# End Project
