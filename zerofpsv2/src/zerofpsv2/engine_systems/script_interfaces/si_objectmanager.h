
#ifndef _SI_OBJECTMANAGER_
#define _SI_OBJECTMANAGER_

#include "../engine_systems_x.h"

class EntityManager;
class ZFScriptSystem;
class Entity;
class Property;
class Vector3;

/**	Script Functions for ObjectManger.

	This namespace contains functions that are exported from the entitymanger to the script system.
*/
namespace ObjectManagerLua
{
	class ScriptState
	{
	public:
		Entity*			 g_pkLastObject;
		Entity*			 g_pkLastParent;
		Property*		 g_pkLastProperty;
		Entity*			 g_pkReturnObject;
		
		int				 g_iCurrentObjectID;
		int				 g_iCurrentPCID;

	};

	Vector3 ENGINE_SYSTEMS_API GetVectorArg(lua_State* pkLua, int iIndex);

	void ENGINE_SYSTEMS_API Init(EntityManager* pkObjMan, ZFScriptSystem* pkScript);
	void ENGINE_SYSTEMS_API Reset();
	void ENGINE_SYSTEMS_API Push();
	void ENGINE_SYSTEMS_API Pop();	

	extern ENGINE_SYSTEMS_API ZFScriptSystem* 	g_pkScript;
	extern ENGINE_SYSTEMS_API EntityManager*	g_pkObjMan;
	
	extern ENGINE_SYSTEMS_API	ScriptState	g_kScriptState;
	/*
	extern ENGINE_SYSTEMS_API	Entity*		g_pkLastObject;
	extern ENGINE_SYSTEMS_API	Entity*		g_pkLastParent;
	extern ENGINE_SYSTEMS_API	Property*	g_pkLastProperty;
	extern ENGINE_SYSTEMS_API	Entity*		g_pkReturnObject;

	extern ENGINE_SYSTEMS_API	Entity*		g_pkLastParentBak;
	extern ENGINE_SYSTEMS_API	Property*	g_pkLastPropertyBak;
	extern ENGINE_SYSTEMS_API	Entity*		g_pkReturnObjectBak;
	extern ENGINE_SYSTEMS_API	Entity*		g_pkLastObjectBak;
	extern ENGINE_SYSTEMS_API	int			g_iCurrentObjectBak;

	extern ENGINE_SYSTEMS_API	int			g_iCurrentObjectID;
	extern ENGINE_SYSTEMS_API 	int			g_iCurrentPCID;
	*/

	// Create/Destory Entitys
	int ENGINE_SYSTEMS_API CreateEntityLua(lua_State* pkLua);							// (strName, kPosition)
	int ENGINE_SYSTEMS_API DeleteLua(lua_State* pkLua);									// (Object)	-	Delete entity.

	// Create funktions  --- these are only used inside the create funktion
	int ENGINE_SYSTEMS_API InitObjectLua(lua_State* pkLua);								
	int ENGINE_SYSTEMS_API InitPropertyLua(lua_State* pkLua);							
	int ENGINE_SYSTEMS_API InitParameterLua(lua_State* pkLua);							

	int ENGINE_SYSTEMS_API AttachToParent(lua_State* pkLua);								
	int ENGINE_SYSTEMS_API SetParentObjectLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API SetReturnObjectLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API HaveRelativOriLua(lua_State* pkLua);
	
	// Position/Orientation.
	int ENGINE_SYSTEMS_API SetLocalPosLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API SetObjectPosLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API GetObjectPosLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API GetObjectRotLua(lua_State* pkLua);

	// Object Variables
	int ENGINE_SYSTEMS_API GetLocalDouble(lua_State* pkLua);		
	int ENGINE_SYSTEMS_API SetLocalDouble(lua_State* pkLua);		
	int ENGINE_SYSTEMS_API GetLocalString(lua_State* pkLua);		
	int ENGINE_SYSTEMS_API SetLocalString(lua_State* pkLua);		

	// Entity Information
	int ENGINE_SYSTEMS_API GetObjectTypeLua(lua_State* pkLua);				//(int objectid) return the type of the object id
	int ENGINE_SYSTEMS_API GetObjectNameLua(lua_State* pkLua);				//(int objectid) return the object name
	int ENGINE_SYSTEMS_API IsEntityValid(lua_State* pkLua);

	// Event
	int ENGINE_SYSTEMS_API SendEventLua(lua_State* pkLua);					//(int objectid,string event) send event to target object

	// Physics
	int ENGINE_SYSTEMS_API SetVelToLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API SetObjectRotVelLua(lua_State* pkLua);
	
	//zone management
	int ENGINE_SYSTEMS_API GetZoneIDLua(lua_State* pkLua);					//returns the zone id that contains entity id
	int ENGINE_SYSTEMS_API GetZoneIDAtPosLua(lua_State* pkLua);				//returns the zone id at position
	int ENGINE_SYSTEMS_API SetZoneModelLua(lua_State* pkLua);				//sets the zonemodel of zone id

	// Common used functions , used together whit P_ScriptInterface
	int ENGINE_SYSTEMS_API SIGetSelfIDLua(lua_State* pkLua);					//() return the callers object id	
	int ENGINE_SYSTEMS_API DistanceToLua(lua_State* pkLua);

};

#endif // #ifndef _OBJECTMANAGER_SCRIPTINTERFACE_H_


