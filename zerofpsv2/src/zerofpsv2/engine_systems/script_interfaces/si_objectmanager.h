
#ifndef _SI_OBJECTMANAGER_
#define _SI_OBJECTMANAGER_

#include "../engine_systems_x.h"

class EntityManager;
class ZFScriptSystem;
class Entity;
class Property;

/**	Script Functions for ObjectManger.
*/
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

	extern int						g_iCurrentObjectID;

	//create funktions
	int ENGINE_SYSTEMS_API InitObjectLua(lua_State* pkLua);	
	int ENGINE_SYSTEMS_API InitPropertyLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API InitParameterLua(lua_State* pkLua);
	
	int ENGINE_SYSTEMS_API AttachToParent(lua_State* pkLua);	
	
	int ENGINE_SYSTEMS_API SetParentObjectLua(lua_State* pkLua);		
	int ENGINE_SYSTEMS_API SetLocalPosLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API SetReturnObjectLua(lua_State* pkLua);	
	
	int ENGINE_SYSTEMS_API HaveRelativOriLua(lua_State* pkLua);		
	//---

	int ENGINE_SYSTEMS_API DeleteLua(lua_State* pkLua);		

	// Object Variables
	int ENGINE_SYSTEMS_API GetLocalDouble(lua_State* pkLua);		
	int ENGINE_SYSTEMS_API SetLocalDouble(lua_State* pkLua);		

	// Animations
	int ENGINE_SYSTEMS_API PlayAnim(lua_State* pkLua);
	int ENGINE_SYSTEMS_API SetNextAnim(lua_State* pkLua);
	int ENGINE_SYSTEMS_API AddMesh(lua_State* pkLua);

	// Orientation
	int ENGINE_SYSTEMS_API SetObjectPosLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API GetObjectPosLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API GetObjectRotLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API DistanceToLua(lua_State* pkLua);

	// Rotation
	int ENGINE_SYSTEMS_API SetObjectRotVelLua(lua_State* pkLua);

	// velocity
	int ENGINE_SYSTEMS_API SetVelToLua(lua_State* pkLua);

	//zone management
	int ENGINE_SYSTEMS_API GetZoneIDLua(lua_State* pkLua);					//returns the zone id that contains entity id
	int ENGINE_SYSTEMS_API SetZoneModelLua(lua_State* pkLua);				//sets the zonemodel of zone id

	// Common used functions , used together whit P_ScriptInterface
	int ENGINE_SYSTEMS_API SIGetSelfIDLua(lua_State* pkLua);					//() return the callers object id	
	int ENGINE_SYSTEMS_API SISetHeartRateLua(lua_State* pkLua);				//(int objectid,float delay) will enable the 1s update function on this object	
};

#endif // #ifndef _OBJECTMANAGER_SCRIPTINTERFACE_H_


