
#ifndef _STD_SCRIPTINTERFACE_H_
#define _STD_SCRIPTINTERFACE_H_

class ZFScript;

#include "../engine_systems_x.h"
#include "../../script/zfscript.h"
#include <iostream>

using namespace std;

namespace StdLua
{

	extern ZFScript* 		g_pkScript;
	
	void ENGINE_SYSTEMS_API Init(ZFScript* pkScript);
	
	int ENGINE_SYSTEMS_API PrintLua(lua_State* pkLua);	
	
	int ENGINE_SYSTEMS_API SinLua(lua_State* pkLua);		
	int ENGINE_SYSTEMS_API CosLua(lua_State* pkLua);			
	int ENGINE_SYSTEMS_API TanLua(lua_State* pkLua);				
};

#endif


