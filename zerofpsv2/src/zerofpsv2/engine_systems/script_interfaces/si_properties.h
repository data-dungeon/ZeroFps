//  ______________________________________________________________________
// /																	  \
// | si_properties.h:                                                     |
// | A place to put common property functions in instead of rewritning    |
// | them for every new game.											  |
// \______________________________________________________________________/

#ifndef _SI_PROPERTIES_H_
#define _SI_PROPERTIES_H_

class ZFScriptSystem;

#include "../engine_systems_x.h"
#include "../../script/zfscript.h"
#include <iostream>

using namespace std;

class ZFVFileSystem;
class EntityManager;

namespace PropertiesLua
{

	extern ZFScriptSystem* 		g_pkScript;
	extern EntityManager*		g_pkObjMan;

	void ENGINE_SYSTEMS_API Init(ZFScriptSystem* pkScript, EntityManager* pkObjMan);
	
	// p_controller
	int ENGINE_SYSTEMS_API BindKeyLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API RotWithCamYAxisLua(lua_State* pkLua);

	// p_mad

	// p_path

	// p_...

};

#endif


