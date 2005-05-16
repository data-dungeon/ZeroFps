/*
	Model Plugin

	This plugin contains some new propertys that loads some common 3d model formats used in games. 
	Used to test and see how other formats works. Saved for future reference. None of the propertys use
	any resource mangment and everythings is as pure and simple as possible. They will often not work 
	anyware else besides the editor and don't support load/save.

	To load plugin run pload model.dll in console.

	List:
		p_mdl:	Loads .mdl models from Quake 1 engine.
		p_md2:	Loads .md2 models from Quake 2 engine.
		p_md5:	Loads md5 files (.md5mesh/.md5anim) from doom3 engine.
		p_mdlhl:	Loads .mdl models from halflife engine.

		zssqpack:	Subsystem for loading files from quake1/quake2 pack files.
*/
#include <vector>

#include "../../zerofpsv2/engine/application.h"
#include "../../zerofpsv2/engine/entitymanager.h"
#include "p_mdl.h"
#include "p_md2.h"
#include "p_md5.h"
#include "p_mdlhl.h"

using namespace std;

#if defined(WIN32) 

#ifdef ZPGPLUGIN_EXPORTS
#define ZPGPLUGIN_API __declspec(dllexport)
#else
#define ZPGPLUGIN_API __declspec(dllimport)
#endif

#endif

#if !defined(WIN32) 
#define ZPGPLUGIN_API 
#endif

extern "C" 
{
	bool ZPGPLUGIN_API Plugin_Load()
	{
		Register_P_Mdl( dynamic_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps")));
		Register_P_Md2( dynamic_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps")));
		Register_P_Md5( dynamic_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps")));
		Register_P_MdlHL( dynamic_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps")));
		return true;
	}

	bool ZPGPLUGIN_API Plugin_Unload()
	{
		return true;
	}
}

