
#ifndef _SI_MISTLAND_H_
#define _SI_MISTLAND_H_

class ZFScriptSystem;
class PSystemProperty;

#include "mcommon_x.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include "../zerofpsv2/engine/objectmanager.h"
#include "../zerofpsv2/engine_systems/propertys/psystemproperty.h"
#include "p_event.h"

using namespace std;

namespace MistLandLua
{
	extern ZFScriptSystem* 		g_pkScript;
	extern ObjectManager*		g_pkObjMan;
	
	extern int						g_iCurrentObjectID;
	extern int						g_iLastCollidedID;

	void MCOMMON_API Init(ObjectManager* pkObjMan,ZFScriptSystem* pkScript);	
	
	int MCOMMON_API GetSelfIDLua(lua_State* pkLua);					//() return the callers object id
	int MCOMMON_API GetObjectTypeLua(lua_State* pkLua);				//(int objectid) return the type of the object id
	int MCOMMON_API GetObjectNameLua(lua_State* pkLua);				//(int objectid) return the object name
	int MCOMMON_API GetLastCollidedObjectLua(lua_State* pkLua);	
	int MCOMMON_API GetClosestObjectLua(lua_State* pkLua);		//(int objectid) return the closest object to the caller or a parameter id
	int MCOMMON_API RemoveObjectLua(lua_State* pkLua);				//(int objectid) removes the caller or a parameter id
	int MCOMMON_API SendEventLua(lua_State* pkLua);					//(int objectid,string event) send event to target object
	
	int MCOMMON_API SetPSystemLua(lua_State* pkLua);				//(int objectid,string pssystem) set wich particle effect to use, if none dont send any parameters

};

#endif


