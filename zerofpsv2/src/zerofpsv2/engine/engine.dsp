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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "ENGINE_SYSTEMS_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41d /d "NDEBUG"
# ADD RSC /l 0x41d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 script.lib physics_engine.lib ogg_static.lib vorbis_static.lib vorbisfile_static.lib sdl_net.lib OpenAL32.lib ALut.lib gui.lib render.lib basic.lib glu32.lib opengl32.lib sdl.lib sdlmain.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\..\..\bin\engine.dll" /libpath:"..\..\..\bin"
# SUBTRACT LINK32 /incremental:yes
# Begin Special Build Tool
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  $(ProjDir)\release\*.lib ..\..\..\bin
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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "ENGINE_SYSTEMS_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41d /d "_DEBUG"
# ADD RSC /l 0x41d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 script.lib physics_engine.lib ogg_static.lib vorbis_static.lib vorbisfile_static.lib sdl_net.lib OpenAL32.lib ALut.lib gui.lib render.lib basic.lib glu32.lib opengl32.lib sdl.lib sdlmain.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\bin\engine.dll" /pdbtype:sept /libpath:"..\..\..\bin"
# Begin Special Build Tool
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  $(ProjDir)\debug\*.lib ..\..\..\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "engine - Win32 Release"
# Name "engine - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\application.cpp
# End Source File
# Begin Source File

SOURCE=.\astar.cpp
# End Source File
# Begin Source File

SOURCE=.\camera.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\physicsengine\collisionshape.cpp
# End Source File
# Begin Source File

SOURCE=.\console.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\mad\core.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\mad\core_bone.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\mad\core_boneanim.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\mad\core_mesh.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\mad\core_meshanim.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\physicsengine\csbox.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\physicsengine\csmech.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\physicsengine\cssphere.cpp
# End Source File
# Begin Source File

SOURCE=.\entity.cpp
# End Source File
# Begin Source File

SOURCE=.\entitymanager.cpp
# End Source File
# Begin Source File

SOURCE=..\ogl\extgl.c
# End Source File
# Begin Source File

SOURCE=.\fh.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\common\heightmap.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\common\heightmap2.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\physicsengine\heightmapcs.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\common\i_heightmap2.cpp
# End Source File
# Begin Source File

SOURCE=.\i_zerofps.cpp
# End Source File
# Begin Source File

SOURCE=.\i_zfresourcedb.cpp
# End Source File
# Begin Source File

SOURCE=.\input.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\mad\mad_modell.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\netpacket.cpp
# End Source File
# Begin Source File

SOURCE=.\network.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\audio\OggMusic.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_ambientsound.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_billboardrender.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_body.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_camera.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_crossrender.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_heightmap2.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_heightmaprender.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_hmrp2.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_light.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_lightupdate.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_linktojoint.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_mad.cpp
# End Source File
# Begin Source File

SOURCE=.\p_pfmesh.cpp
# End Source File
# Begin Source File

SOURCE=.\p_pfpath.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_physic.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_primitives3d.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_psystem.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_skyboxrender.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_tcs.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_track.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_vegitation.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_waterrender.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_worldinfo.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\physicsengine\physicsengine.cpp
# End Source File
# Begin Source File

SOURCE=.\property.cpp
# End Source File
# Begin Source File

SOURCE=.\propertyfactory.cpp
# End Source File
# Begin Source File

SOURCE=.\res_texture.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\tcs\tcs.cpp
# End Source File
# Begin Source File

SOURCE=.\zerofps.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\audio\zfaudiosystem.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\application.h
# End Source File
# Begin Source File

SOURCE=.\astar.h
# End Source File
# Begin Source File

SOURCE=.\camera.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\physicsengine\collisionshape.h
# End Source File
# Begin Source File

SOURCE=.\console.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\physicsengine\csbox.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\physicsengine\csmech.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\physicsengine\cssphere.h
# End Source File
# Begin Source File

SOURCE=.\engine_x.h
# End Source File
# Begin Source File

SOURCE=.\entity.h
# End Source File
# Begin Source File

SOURCE=.\entitymanager.h
# End Source File
# Begin Source File

SOURCE=.\fh.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\common\heightmap.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\common\heightmap2.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\physicsengine\heightmapcs.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\common\i_heightmap2.h
# End Source File
# Begin Source File

SOURCE=.\i_zerofps.h
# End Source File
# Begin Source File

SOURCE=.\i_zfresourcedb.h
# End Source File
# Begin Source File

SOURCE=.\input.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\mad\mad_core.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\mad\mad_modell.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\netpacket.h
# End Source File
# Begin Source File

SOURCE=.\network.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\audio\oggmusic.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_ambientsound.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_billboardrender.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_body.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_camera.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_clientinput.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_crossrender.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_heightmap2.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_heightmaprender.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_hmrp2.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_light.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_lightupdate.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_linktojoint.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_mad.h
# End Source File
# Begin Source File

SOURCE=.\p_pfmesh.h
# End Source File
# Begin Source File

SOURCE=.\p_pfpath.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_physic.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_primitives3d.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_psystem.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_skyboxrender.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_tcs.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_track.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_vegitation.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_waterrender.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\p_worldinfo.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\physicsengine\physicsengine.h
# End Source File
# Begin Source File

SOURCE=.\property.h
# End Source File
# Begin Source File

SOURCE=.\propertyfactory.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\propertys.pkg
# End Source File
# Begin Source File

SOURCE=.\res_texture.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\tcs\tcs.h
# End Source File
# Begin Source File

SOURCE=.\zerofps.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\audio\zfaudiosystem.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "PSystemStuff"

# PROP Default_Filter ""
# Begin Group "PSProperties"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\engine_systems\propertys\psproperties\alphatestprop.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\psproperties\alphatestprop.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\psproperties\blendpsprop.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\psproperties\blendpsprop.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\psproperties\colorpsprop.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\psproperties\colorpsprop.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\psproperties\lightpsprop.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\psproperties\lightpsprop.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\psproperties\movepsprop.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\psproperties\movepsprop.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\psproperties\particleproperty.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\psproperties\sizepsprop.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\psproperties\sizepsprop.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\psproperties\texturepsprop.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\propertys\psproperties\texturepsprop.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\engine_systems\common\psystem.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\common\psystem.h
# End Source File
# Begin Source File

SOURCE=.\psystemmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\psystemmanager.h
# End Source File
# End Group
# Begin Group "scriptinterfaces"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\engine_systems\script_interfaces\si_audio.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\script_interfaces\si_audio.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\script_interfaces\si_gui.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\script_interfaces\si_gui.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\script_interfaces\si_objectmanager.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\script_interfaces\si_objectmanager.h
# End Source File
# Begin Source File

SOURCE=..\engine_systems\script_interfaces\si_std.cpp
# End Source File
# Begin Source File

SOURCE=..\engine_systems\script_interfaces\si_std.h
# End Source File
# End Group
# End Target
# End Project
