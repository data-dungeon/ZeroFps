
#ifndef _SI_OBJECTMANAGER_
#define _SI_OBJECTMANAGER_

#include "../engine_systems_x.h"

class EntityManager;
class ZFScriptSystem;
class Entity;
class Property;

namespace ObjectManagerLua
{
	void ENGINE_SYSTEMS_API Init(EntityManager* pkObjMan, ZFScriptSystem* pkScript);
	void ENGINE_SYSTEMS_API Reset();
	void ENGINE_SYSTEMS_API Push();
	void ENGINE_SYSTEMS_API Pop();	

	extern ZFScriptSystem* 		g_pkScript;
	
	extern EntityManager* g_pkObjMan;
	
	extern Entity*			g_pkLastObject;
	extern Entity*			g_pkLastParent;
	extern Property*		g_pkLastProperty;
	extern Entity*			g_pkReturnObject;

	extern Entity*			g_pkLastParentBak;
	extern Property*		g_pkLastPropertyBak;
	extern Entity*			g_pkReturnObjectBak;

	//create funktions
	int ENGINE_SYSTEMS_API InitObjectLua(lua_State* pkLua);	
	int ENGINE_SYSTEMS_API InitPropertyLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API InitParameterLua(lua_State* pkLua);
	
	int ENGINE_SYSTEMS_API AttachToParent(lua_State* pkLua);	
	
	int ENGINE_SYSTEMS_API SetParentObjectLua(lua_State* pkLua);		
	int ENGINE_SYSTEMS_API SetLocalPosLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API SetReturnObjectLua(lua_State* pkLua);	
	
	int ENGINE_SYSTEMS_API HaveRelativOriLua(lua_State* pkLua);		
	int ENGINE_SYSTEMS_API IsStaticLua(lua_State* pkLua);		
	//---

	int ENGINE_SYSTEMS_API DeleteLua(lua_State* pkLua);		

	// Animations
	int ENGINE_SYSTEMS_API PlayAnim(lua_State* pkLua);
	int ENGINE_SYSTEMS_API SetNextAnim(lua_State* pkLua);
	
	
};

#endif // #ifndef _OBJECTMANAGER_SCRIPTINTERFACE_H_
