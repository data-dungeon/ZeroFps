#ifndef _SI_DM_H_
#define _SI_DM_H_

class ZFScriptSystem;
class PSystemProperty;
class ZGuiResourceManager;

#include "mcommon_x.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include "../zerofpsv2/engine/entitymanager.h"
#include <map>
#include <string>

using namespace std;

namespace DMLua
{
	extern ZFScriptSystem* 			g_pkScript;
	extern EntityManager*			g_pkObjMan;
	extern ZGuiResourceManager*	g_pkGuiResMan;

	struct PATROL_POINTS
	{
		int m_iCurrent;
		vector<Vector3> m_vkPoints;
	};

	extern vector<int> m_kCallsForHelp;
	extern map<int, PATROL_POINTS> m_kPatrolPoints;

	void MCOMMON_API Init(EntityManager* pkObjMan,ZFScriptSystem* pkScript,
		ZGuiResourceManager* pkGuiResMan);
	int MCOMMON_API GetDMCharacterByNameLua(lua_State* pkLua);
	int MCOMMON_API SetDMCharacterNameLua(lua_State* pkLua);
	int MCOMMON_API GetDMCharacterClosestLua(lua_State* pkLua);
	int MCOMMON_API SetNewMissionLua(lua_State* pkLua);
	int MCOMMON_API GetNumOfLivingAgentsLua(lua_State* pkLua);

	// SI for characters
	int MCOMMON_API KillCharacterLua(lua_State* pkLua);
	int MCOMMON_API IsDeadLua(lua_State* pkLua);
	int MCOMMON_API HealLua(lua_State* pkLua);
	int MCOMMON_API BeamToLocationLua(lua_State* pkLua);
	int MCOMMON_API BeamToObjectLua(lua_State* pkLua);
	int MCOMMON_API TestSkillLua(lua_State* pkLua);
	int MCOMMON_API GetItemCharLua(lua_State* pkLua);
	int MCOMMON_API AddMoneyLua(lua_State* pkLua);
	int MCOMMON_API MoneyLua(lua_State* pkLua);
	int MCOMMON_API FireAtLocationLua(lua_State* pkLua);
	int MCOMMON_API FireAtCharacterLua(lua_State* pkLua);
	int MCOMMON_API SetMoveSpeedLua(lua_State* pkLua);
	int MCOMMON_API AddOffenciveActionQuotLua(lua_State* pkLua);
	int MCOMMON_API AddDefenciveActionQuotLua(lua_State* pkLua);
	int MCOMMON_API AddDeathSoundLua(lua_State* pkLua);
	int MCOMMON_API AddMoveCharSoundLua(lua_State* pkLua);
	int MCOMMON_API AddSelectCharSoundLua(lua_State* pkLua);
	int MCOMMON_API SetTeamLua(lua_State* pkLua);
	int MCOMMON_API GetCharStatsLua(lua_State* pkLua);
	int MCOMMON_API SetCharStatsLua(lua_State* pkLua);
	int MCOMMON_API SetStateLua(lua_State* pkLua);
	int MCOMMON_API GetWeaponRangeLua(lua_State* pkLua);
	int MCOMMON_API HaveOrdersLua(lua_State* pkLua);	
	int MCOMMON_API SetLeaderOfTeamLua(lua_State* pkLua);

	// items
	int MCOMMON_API AddItemLua(lua_State* pkLua);
	int MCOMMON_API GetItemByNameLua(lua_State* pkLua);
	
	// behaviours for characters
	int MCOMMON_API PanicAreaLua(lua_State* pkLua);
	int MCOMMON_API CallForHelpLua(lua_State* pkLua);
	int MCOMMON_API GetStateLua(lua_State* pkLua);
	int MCOMMON_API GetTargetLua(lua_State* pkLua);


	// police functions
	int MCOMMON_API GetClosestCallerLua(lua_State* pkLua);

	// SI for guns
	int MCOMMON_API SetGunFireRateLua(lua_State* pkLua);
	int MCOMMON_API SetGunDamageLua(lua_State* pkLua);
	int MCOMMON_API SetGunRangeLua(lua_State* pkLua);
	int MCOMMON_API SetGunAmmoLua(lua_State* pkLua);
	int MCOMMON_API SetGunMaxAmmoLua(lua_State* pkLua);
	int MCOMMON_API SetGunRandomLua(lua_State* pkLua);
	int MCOMMON_API SetGunBulletsPerAmmoLua(lua_State* pkLua);
	int MCOMMON_API SetGunSoundLua(lua_State* pkLua);
	int MCOMMON_API SetGunShootAnimLua(lua_State* pkLua);
	int MCOMMON_API SetGunBurstLenghtLua(lua_State* pkLua);

	// SI for items with bonuses (+ AV and such)
	int MCOMMON_API SetItemArmourLua(lua_State* pkLua);
	int MCOMMON_API SetItemMaxLifeLua(lua_State* pkLua);
	int MCOMMON_API SetItemSpeedLua(lua_State* pkLua);
	int MCOMMON_API SetItemAimLua(lua_State* pkLua);

	// lualua functions
	int MCOMMON_API RunScriptLua(lua_State* pkLua);

	// Path finding
	int MCOMMON_API HavePathLua(lua_State* pkLua);
	int MCOMMON_API MakePathFindLua(lua_State* pkLua);
	int MCOMMON_API ClearPathFindLua(lua_State* pkLua);
	int MCOMMON_API SetRunAnimLua(lua_State* pkLua);
	int MCOMMON_API SetIdleAnimLua(lua_State* pkLua);
	int MCOMMON_API AddPatrolPointLua(lua_State* pkLua);
	int MCOMMON_API PatrolLua(lua_State* pkLua);
	int MCOMMON_API ClearPatrolPathLua(lua_State* pkLua);

	// math
	int MCOMMON_API RandomLua(lua_State* pkLua);

	// common
	int MCOMMON_API SetVarLua(lua_State* pkLua);
	int MCOMMON_API GetVarLua(lua_State* pkLua);
	int MCOMMON_API AddToVarLua(lua_State* pkLua);

	// entity
	int MCOMMON_API SetEntityVarLua(lua_State* pkLua);
	int MCOMMON_API GetEntityVarLua(lua_State* pkLua);
	int MCOMMON_API AddToEntityVarLua(lua_State* pkLua);
	int MCOMMON_API GetEntityPosLua(lua_State* pkLua);
	
	// functions to get less code
	Entity* TestScriptInput (int iArgs, lua_State* pkLua);
	Entity* GetHQEntity ();
	Entity* GetGameInfoEntity ();

	// SI for houses
	int MCOMMON_API SwallowPlayerLua(lua_State* pkLua);
	int MCOMMON_API IsHouseLua(lua_State* pkLua);

	// stuff
	int MCOMMON_API ExplosionLua(lua_State* pkLua);

	// shop functions
	int MCOMMON_API AddItemToShopLua(lua_State* pkLua);

	// stuff 2
	int MCOMMON_API GetCharsByFractionLua(lua_State* pkLua);
	int MCOMMON_API GetDMObjectLua(lua_State* pkLua);

	// special gui stuff
	int HideAllMapIconsLua(lua_State* pkLua);

	// lua "tools"
	Vector3 MCOMMON_API GetVectorArg(lua_State* pkLua, int iIndex);
	

};

#endif


