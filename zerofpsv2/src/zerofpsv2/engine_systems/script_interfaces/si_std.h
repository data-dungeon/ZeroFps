
#ifndef _SI_STD_H_
#define _SI_STD_H_

class ZFScriptSystem;

#include "../engine_systems_x.h"
#include "../../script/zfscript.h"
#include <iostream>

using namespace std;

namespace StdLua
{

	extern ZFScriptSystem* 		g_pkScript;
	
	void ENGINE_SYSTEMS_API Init(ZFScriptSystem* pkScript);
	
	int ENGINE_SYSTEMS_API PrintLua(lua_State* pkLua);	
	
	int ENGINE_SYSTEMS_API SinLua(lua_State* pkLua);		
	int ENGINE_SYSTEMS_API CosLua(lua_State* pkLua);			
	int ENGINE_SYSTEMS_API TanLua(lua_State* pkLua);

	int ENGINE_SYSTEMS_API GetFilesInFolderLua(lua_State* pkLua);
};

#endif


