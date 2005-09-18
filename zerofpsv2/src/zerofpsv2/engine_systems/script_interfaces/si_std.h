
#ifndef _SI_STD_H_
#define _SI_STD_H_

class ZSSScriptSystem;

#include "../engine_systems_x.h"
#include "../../script/zfscript.h"
#include <iostream>

using namespace std;

class ZSSVFileSystem;
class ZSSZeroFps;

namespace StdLua
{

	extern ZSSScriptSystem* 		g_pkScript;
	extern ZSSVFileSystem*		g_pkVFS;
	extern ZSSZeroFps*				g_pkZeroFps;
	
	void ENGINE_SYSTEMS_API Init(ZSSScriptSystem* pkScript, ZSSVFileSystem* pkBasicFS,ZSSZeroFps* pkZeroFps);
	
	//GENERAL
	int ENGINE_SYSTEMS_API PrintLua(lua_State* pkLua);		
	
	//MATH
	int ENGINE_SYSTEMS_API SinLua(lua_State* pkLua);		
	int ENGINE_SYSTEMS_API CosLua(lua_State* pkLua);			
	int ENGINE_SYSTEMS_API TanLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API PowLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API NormalizeLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API VectorSubLua(lua_State* pkLua);
	
	//TIME
	int ENGINE_SYSTEMS_API GetTicksLua(lua_State* pkLua);
	
	//FILE
	int ENGINE_SYSTEMS_API GetFilesInFolderLua(lua_State* pkLua);
};

#endif


