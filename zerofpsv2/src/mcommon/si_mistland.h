
#ifndef _SI_MISTLAND_H_
#define _SI_MISTLAND_H_

class ZFScriptSystem;
class PSystemProperty;

#include "mcommon_x.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include "../zerofpsv2/engine/entitymanager.h"
#include "../zerofpsv2/engine_systems/propertys/p_psystem.h"
#include "../zerofpsv2/engine_systems/propertys/p_tcs.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_objectmanager.h"
#include "p_event.h"
#include "p_ml.h"
#include <map>
#include <string>
#include "p_serverinfo.h"

struct LogInInfo
{
	string acIP;
	string acUserName;
	string acPassword;
};

using namespace std;

namespace MistLandLua
{
	extern ZFScriptSystem* 		g_pkScript;
	extern EntityManager*		g_pkObjMan;
	extern P_ServerInfo*			g_pkServerInfo;

	extern int						g_iLastCollidedID;

	void MCOMMON_API Init(EntityManager* pkObjMan,ZFScriptSystem* pkScript);	
	
	int MCOMMON_API GetCurrentPCIDLua(lua_State* pkLua);				//() return the callers object id	
	int MCOMMON_API GetLastCollidedObjectLua(lua_State* pkLua);	
	int MCOMMON_API GetClosestObjectLua(lua_State* pkLua);			//(int objectid) return the closest object to the caller or a parameter id
	int MCOMMON_API MessageCaracterLua(lua_State* pkLua);				//(int objectid,string message) send a message to caracters controling player

	// sound crap
	int MCOMMON_API StartPrivateSoundLua(lua_State* pkLua);

	// ip crap
	extern map<string,LogInInfo*> MCOMMON_API	g_kServerList;
	extern string MCOMMON_API g_kDefServer;
	int MCOMMON_API AddServerLua(lua_State* pkLua);
	int MCOMMON_API SetDefaultServerLua(lua_State* pkLua);

/*	
   // character status
   int MCOMMON_API RollSkillDiceLua (lua_State* pkLua);       // returns failed/suceeded
   int MCOMMON_API RollAttributeDiceLua (lua_State* pkLua);   // returns failed/suceeded
   int MCOMMON_API SetCurrentSkillLua (lua_State* pkLua);
   int MCOMMON_API AddSkillValueLua (lua_State* pkLua);
   int MCOMMON_API AddAttributeValueLua (lua_State* pkLua);
   int MCOMMON_API SetSkillValueLua (lua_State* pkLua);
   int MCOMMON_API SetAttributeValueLua (lua_State* pkLua);
   int MCOMMON_API SetDefenceValueLua (lua_State* pkLua);
   int MCOMMON_API SetAttackValueLua (lua_State* pkLua);
   int MCOMMON_API SetDataValueLua (lua_State* pkLua);
   int MCOMMON_API SetHPLua (lua_State* pkLua);
   int MCOMMON_API SetMPLua (lua_State* pkLua);
   int MCOMMON_API SetMaxHPLua (lua_State* pkLua);
   int MCOMMON_API SetMaxMPLua (lua_State* pkLua);
   int MCOMMON_API GetHPLua (lua_State* pkLua);
   int MCOMMON_API GetMPLua (lua_State* pkLua);
   int MCOMMON_API GetHpPercentLua (lua_State* pkLua);
   int MCOMMON_API GetMpPercentLua (lua_State* pkLua);
   int MCOMMON_API AddHpLua (lua_State* pkLua);
   int MCOMMON_API AddMpLua (lua_State* pkLua);
   int MCOMMON_API PrintStatsLua (lua_State* pkLua);
   int MCOMMON_API SetScriptWhenHitLua (lua_State* pkLua);

   int MCOMMON_API SetMoveSpeedLua (lua_State* pkLua);
   int MCOMMON_API AddMoveSpeedLua (lua_State* pkLua);

   int MCOMMON_API SetReloadTimeLua (lua_State* pkLua);
   int MCOMMON_API AddReloadTimeLua (lua_State* pkLua);

	int MCOMMON_API SetRecalPositionLua (lua_State* pkLua);
	
   // item stuff
   int MCOMMON_API GetQuantityLua (lua_State* pkLua);
   int MCOMMON_API SetQuantityLua (lua_State* pkLua);
   int MCOMMON_API AddQuantityLua (lua_State* pkLua);
   
   int MCOMMON_API SetQuantityLua (lua_State* pkLua);
   int MCOMMON_API SetQualityLua (lua_State* pkLua);
   int MCOMMON_API SetSkillBonusLua (lua_State* pkLua);
   int MCOMMON_API SetAttributeBonusLua (lua_State* pkLua);
   int MCOMMON_API SetAttackBonusLua (lua_State* pkLua);
   int MCOMMON_API SetDefenceBonusLua (lua_State* pkLua);
   int MCOMMON_API EquipOnLua (lua_State* pkLua);
   
   int MCOMMON_API GetSkillBonusLua (lua_State* pkLua);
   int MCOMMON_API GetAttributeBonusLua (lua_State* pkLua);
   int MCOMMON_API GetAttackBonusLua (lua_State* pkLua);
   int MCOMMON_API GetDefenceBonusLua (lua_State* pkLua);

   int MCOMMON_API AddSkillBonusLua (lua_State* pkLua);
   int MCOMMON_API AddAttributeBonusLua (lua_State* pkLua);
   int MCOMMON_API AddAttackBonusLua (lua_State* pkLua);
   int MCOMMON_API AddDefenceBonusLua (lua_State* pkLua);
   
   int MCOMMON_API PrintItemStatsLua (lua_State* pkLua);
   int MCOMMON_API SetItemNameLua (lua_State* pkLua);
   int MCOMMON_API SetItemWeightLua (lua_State* pkLua);

   int MCOMMON_API SetItemValueLua (lua_State* pkLua);
   int MCOMMON_API GetItemValueLua (lua_State* pkLua);
   int MCOMMON_API AddItemValueLua (lua_State* pkLua);

   int MCOMMON_API SetIconLua (lua_State* pkLua);
	int MCOMMON_API SetEquipmentCategoryLua(lua_State* pkLua);

   int MCOMMON_API AddBeforeItemNameLua (lua_State* pkLua);
   int MCOMMON_API AddAfterItemNameLua (lua_State* pkLua);

   int MCOMMON_API UseItemOnLua (lua_State* pkLua);

   int MCOMMON_API UsesSkillLua (lua_State* pkLua);

*/	
   // Lua lua functions :)
   //int MCOMMON_API RunScriptLua (lua_State* pkLua);

   // returns a stat-category (fire, ice, lightning, poision etc...)
   int MCOMMON_API GetRandomCategoryLua (lua_State* pkLua);
   int MCOMMON_API GetRandomSkillLua (lua_State* pkLua);
   int MCOMMON_API GetRandomAttributeLua (lua_State* pkLua);
   int MCOMMON_API RandomLua(lua_State* pkLua); //takes a number and returns a number between 0-entered number

   // spell functions (inte för stavelsekontroll)
	// int MCOMMON_API CastSpellLua (lua_State* pkLua);

	//movement
	int MCOMMON_API MakePathFindLua(lua_State* pkLua);				//(int objectid,table (x,y,z)) do a pathfind for the object
	int MCOMMON_API HavePathLua(lua_State* pkLua);				//(int objectid) returns 1 if object has a path to walk
	int MCOMMON_API RotateTowardsLua(lua_State* pkLua);				//(int objectid) returns 1 if object has a path to walk
      
   // AI stuff
	int MCOMMON_API AIUseActionOnLua(lua_State* pkLua);
	int MCOMMON_API AIMoveToLua(lua_State* pkLua);
   int MCOMMON_API AIIdleLua(lua_State* pkLua);
   int MCOMMON_API AIAttackLua(lua_State* pkLua);
   int MCOMMON_API AIFaceDirectionLua(lua_State* pkLua);
   int MCOMMON_API AIFaceObjectLua(lua_State* pkLua);
   int MCOMMON_API AISetCommander(lua_State* pkLua);
   int MCOMMON_API AISetSlave(lua_State* pkLua);
	int MCOMMON_API GetClosestItemOfTypeLua(lua_State* pkLua);
   int MCOMMON_API GetClosestPlayerLua(lua_State* pkLua);
	int MCOMMON_API GetClosestObjectOfTypeLua(lua_State* pkLua);	// (string type,flaot radius)
   int MCOMMON_API GetSeenPlayerLua(lua_State* pkLua);
   int MCOMMON_API SetAIIsPlayerLua(lua_State* pkLua);
   int MCOMMON_API AIHaveTargetLua(lua_State* pkLua);
};

#endif


