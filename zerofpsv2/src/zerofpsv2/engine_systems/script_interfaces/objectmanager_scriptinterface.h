
#ifndef _OBJECTMANAGER_SCRIPTINTERFACE_H_
#define _OBJECTMANAGER_SCRIPTINTERFACE_H_

class ObjectManager;
class ZFScript;

#include "../engine_systems_x.h"
#include "../../engine/objectmanager.h"
#include "../../script/zfscript.h"

namespace ObjectManagerLua
{
	void ENGINE_SYSTEMS_API Init(ObjectManager* pkObjMan, ZFScript* pkScript);
	void ENGINE_SYSTEMS_API Reset();

	ObjectManager* g_pkObjMan;
	ZFScript* 		g_pkScript;

	Object*			g_pkLastObject;
	Object*			g_pkLastParent;
	Property*		g_pkLastProperty;
	Object*			g_pkReturnObject;

	int ENGINE_SYSTEMS_API InitObjectLua(lua_State* pkLua);	
	int ENGINE_SYSTEMS_API InitPropertyLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API InitParameterLua(lua_State* pkLua);
	
	int ENGINE_SYSTEMS_API AttachToClosestZoneLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API AttachToParent(lua_State* pkLua);	
	
	int ENGINE_SYSTEMS_API SetParentObjectLua(lua_State* pkLua);		
	int ENGINE_SYSTEMS_API SetLocalPosLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API SetReturnObjectLua(lua_State* pkLua);	
	
	int ENGINE_SYSTEMS_API HaveRelativOriLua(lua_State* pkLua);		
};

#endif // #ifndef _SCRIPTINTERFACES_H
