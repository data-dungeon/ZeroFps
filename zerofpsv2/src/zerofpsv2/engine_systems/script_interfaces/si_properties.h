#ifndef _SI_PROPERTIES_H_
#define _SI_PROPERTIES_H_

class ZFScriptSystem;

#include "../engine_systems_x.h"
#include "../../script/zfscript.h"
#include <iostream>

using namespace std;

class ZFVFileSystem;

namespace PropertiesLua
{

	extern ZFScriptSystem* 		g_pkScript;
	extern ZFVFileSystem*		g_pkVFS;
	
	void ENGINE_SYSTEMS_API Init(ZFScriptSystem* pkScript, ZFVFileSystem* pkBasicFS);
	
	int ENGINE_SYSTEMS_API BindKeyLua(lua_State* pkLua);	

};

#endif


