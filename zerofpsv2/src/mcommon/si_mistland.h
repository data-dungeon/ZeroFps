
#ifndef _SI_MISTLAND_H_
#define _SI_MISTLAND_H_

class ZFScriptSystem;
class PSystemProperty;

#include "mcommon_x.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include "../zerofpsv2/engine/entitymanager.h"
#include "../zerofpsv2/engine_systems/propertys/p_psystem.h"
#include "p_event.h"
#include "p_ml.h"
#include <map>
#include <string>

using namespace std;

namespace MistLandLua
{
	extern ZFScriptSystem* 		g_pkScript;
	extern EntityManager*		g_pkObjMan;
	
	extern int						g_iCurrentObjectID;
	extern int						g_iLastCollidedID;
	

	extern int MCOMMON_API 		g_iCurrentPCID;

	void MCOMMON_API Init(EntityManager* pkObjMan,ZFScriptSystem* pkScript);	
	
	int MCOMMON_API GetSelfIDLua(lua_State* pkLua);					//() return the callers object id
	int MCOMMON_API GetCurrentPCIDLua(lua_State* pkLua);					//() return the callers object id	
	int MCOMMON_API GetObjectTypeLua(lua_State* pkLua);				//(int objectid) return the type of the object id
	int MCOMMON_API GetObjectNameLua(lua_State* pkLua);				//(int objectid) return the object name
	int MCOMMON_API GetLastCollidedObjectLua(lua_State* pkLua);	
	int MCOMMON_API GetClosestObjectLua(lua_State* pkLua);		//(int objectid) return the closest object to the caller or a parameter id
	int MCOMMON_API RemoveObjectLua(lua_State* pkLua);				//(int objectid) removes the caller or a parameter id
	int MCOMMON_API SendEventLua(lua_State* pkLua);					//(int objectid,string event) send event to target object

	int MCOMMON_API AddActionLua(lua_State* pkLua);				//(int objectid,string action) add action to action list of object

	// ip crap
	extern map<string,string> MCOMMON_API	g_kServerList;
	extern string MCOMMON_API g_kDefServer;
	int MCOMMON_API AddServerLua(lua_State* pkLua);
	int MCOMMON_API SetDefaultServerLua(lua_State* pkLua);

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

   int MCOMMON_API AddBeforeItemNameLua (lua_State* pkLua);
   int MCOMMON_API AddAfterItemNameLua (lua_State* pkLua);

   // equip / unequip character

   int MCOMMON_API EquipFromScriptLua (lua_State* pkLua);
   int MCOMMON_API EquipLua (lua_State* pkLua);
   int MCOMMON_API UnEquipLua (lua_State* pkLua);

	
   // Lua lua functions :)
   int MCOMMON_API RunScriptLua (lua_State* pkLua);


   // returns a stat-category (fire, ice, lightning, poision etc...)
   int MCOMMON_API GetRandomCategoryLua (lua_State* pkLua);
   int MCOMMON_API GetRandomSkillLua (lua_State* pkLua);
   int MCOMMON_API GetRandomAttributeLua (lua_State* pkLua);
   int MCOMMON_API RandomLua(lua_State* pkLua); //takes a number and returns a number between 0-entered number

   
   // spell functions (inte för stavelsekontroll)
   int MCOMMON_API CastSpellLua (lua_State* pkLua);


	int MCOMMON_API SetPSystemLua(lua_State* pkLua);				//(int objectid,string pssystem) set wich particle effect to use, if none dont send any parameters

	int MCOMMON_API SetVelToLua(lua_State* pkLua);				//(int objectid,int objectid,double speed) set the velocity of and object towards another object


   int MCOMMON_API SetPropertyValueLua(lua_State* pkLua); //which property, data in property and last, value


};

#endif


