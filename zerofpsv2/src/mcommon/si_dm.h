
#ifndef _SI_DM_H_
#define _SI_DM_H_

class ZFScriptSystem;
class PSystemProperty;

#include "mcommon_x.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include "../zerofpsv2/engine/entitymanager.h"
#include <map>
#include <string>

using namespace std;

namespace DMLua
{
	extern ZFScriptSystem* 		g_pkScript;
	extern EntityManager*		g_pkObjMan;

	void MCOMMON_API Init(EntityManager* pkObjMan,ZFScriptSystem* pkScript);
	int MCOMMON_API GetClosestDMCharacterLua(lua_State* pkLua);
};

#endif


