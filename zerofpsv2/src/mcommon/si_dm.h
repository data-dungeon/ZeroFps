
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

	extern int g_iMissionStatus;

	void MCOMMON_API Init(EntityManager* pkObjMan,ZFScriptSystem* pkScript);
	int MCOMMON_API GetDMCharacterByNameLua(lua_State* pkLua);
	int MCOMMON_API GetDMCharacterClosestLua(lua_State* pkLua);
	int MCOMMON_API SetNewMissionLua(lua_State* pkLua);
	int MCOMMON_API GetNumOfLivingAgentsLua(lua_State* pkLua);

	// SI for characters
	int MCOMMON_API KillCharacterLua(lua_State* pkLua);
	int MCOMMON_API IsDeadLua(lua_State* pkLua);
	int MCOMMON_API HealLua(lua_State* pkLua);
	int MCOMMON_API BeamToLua(lua_State* pkLua);
	int MCOMMON_API TestSkillLua(lua_State* pkLua);
	int MCOMMON_API AddMoneyLua(lua_State* pkLua);
	int MCOMMON_API MoneyLua(lua_State* pkLua);
	int MCOMMON_API FireAtLocationLua(lua_State* pkLua);
	int MCOMMON_API FireAtCharacterLua(lua_State* pkLua);

	// lualua functions
	int MCOMMON_API RunScriptLua(lua_State* pkLua);

	// functions to get less code
	Entity* MCOMMON_API TestScriptInput (int iArgs, lua_State* pkLua);

};

#endif


