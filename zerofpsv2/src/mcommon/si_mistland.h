
#ifndef _SI_MISTLAND_H_
#define _SI_MISTLAND_H_

class ZFScriptSystem;

#include "mcommon_x.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include "../zerofpsv2/engine/objectmanager.h"

using namespace std;

namespace MistLandLua
{
	extern ZFScriptSystem* 		g_pkScript;
	extern ObjectManager*		g_pkObjMan;
	
	extern int						g_iCurrentObjectID;

	void MCOMMON_API Init(ObjectManager* pkObjMan,ZFScriptSystem* pkScript);	
	int MCOMMON_API GetSelfIDLua(lua_State* pkLua);			
	
	int MCOMMON_API GetClosestObjectLua(lua_State* pkLua);			

	int MCOMMON_API RemoveObjectLua(lua_State* pkLua);			
};

#endif


