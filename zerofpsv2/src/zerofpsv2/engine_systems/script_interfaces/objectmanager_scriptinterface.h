
#ifndef _OBJECTMANAGER_SCRIPTINTERFACE_H_
#define _OBJECTMANAGER_SCRIPTINTERFACE_H_

#include "../engine_systems_x.h"

class ObjectManager;
class ZFScriptSystem;
class Object;
class Property;

namespace ObjectManagerLua
{
	void ENGINE_SYSTEMS_API Init(ObjectManager* pkObjMan, ZFScriptSystem* pkScript);
	void ENGINE_SYSTEMS_API Reset();

	extern ZFScriptSystem* 		g_pkScript;
	
	extern ObjectManager* g_pkObjMan;
	extern Object*			g_pkLastObject;
	extern Object*			g_pkLastParent;
	extern Property*		g_pkLastProperty;
	extern Object*			g_pkReturnObject;

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

#endif // #ifndef _OBJECTMANAGER_SCRIPTINTERFACE_H_
