#include "si_dm.h"
#include "../zerofpsv2/script/zfscript.h"
#include "p_dmhq.h"
#include "p_dmgun.h"
#include "p_dmmission.h"
#include "p_dmclickme.h"
#include "p_dmcharacter.h"
#include "p_dmgameinfo.h"
#include "p_dmshop.h"
#include "../zerofpsv2/engine/p_pfpath.h" 
#include "../zerofpsv2/engine_systems/propertys/p_linktojoint.h"

ZFScriptSystem*				DMLua::g_pkScript;
EntityManager*					DMLua::g_pkObjMan;
map<string, double>			DMLua::m_kVars;
vector<int>						DMLua::m_kCallsForHelp;

// ------------------------------------------------------------------------------------------------

void DMLua::Init(EntityManager* pkObjMan,ZFScriptSystem* pkScript)
{
	g_pkObjMan = pkObjMan;
	g_pkScript = pkScript;
	
	pkScript->ExposeFunction("GetNumOfLivingAgents", DMLua::GetNumOfLivingAgentsLua);
	pkScript->ExposeFunction("GetDMCharacterByName", DMLua::GetDMCharacterByNameLua);
	pkScript->ExposeFunction("SetDMCharacterName", DMLua::SetDMCharacterNameLua);
	pkScript->ExposeFunction("GetDMCharacterClosest", DMLua::GetDMCharacterClosestLua);

	// character functions
	pkScript->ExposeFunction("KillCharacter", DMLua::KillCharacterLua);
	pkScript->ExposeFunction("IsDead", DMLua::IsDeadLua);
	pkScript->ExposeFunction("GetState", DMLua::GetStateLua);
	pkScript->ExposeFunction("SetState", DMLua::SetStateLua);
	pkScript->ExposeFunction("Heal", DMLua::HealLua);
	pkScript->ExposeFunction("BeamToLocation", DMLua::BeamToLocationLua);
	pkScript->ExposeFunction("BeamToObject", DMLua::BeamToObjectLua);
	pkScript->ExposeFunction("TestSkill", DMLua::TestSkillLua);
	pkScript->ExposeFunction("GetItemChar", DMLua::GetItemCharLua);
	pkScript->ExposeFunction("AddMoney", DMLua::AddMoneyLua);
	pkScript->ExposeFunction("Money", DMLua::MoneyLua);
	pkScript->ExposeFunction("FireAtLocation", DMLua::FireAtLocationLua);
	pkScript->ExposeFunction("FireAtObject", DMLua::FireAtCharacterLua);
	pkScript->ExposeFunction("SetMoveSpeed", DMLua::SetMoveSpeedLua);
	pkScript->ExposeFunction("AddOffenciveActionQuot", DMLua::AddOffenciveActionQuotLua);
	pkScript->ExposeFunction("AddDefenciveActionQuot", DMLua::AddDefenciveActionQuotLua);
	pkScript->ExposeFunction("AddDeathSound", DMLua::AddDeathSoundLua);
	pkScript->ExposeFunction("AddMoveCharSound", DMLua::AddMoveCharSoundLua);
	pkScript->ExposeFunction("AddSelectCharSound", DMLua::AddSelectCharSoundLua);
	pkScript->ExposeFunction("SetTeam", DMLua::SetTeamLua);
	pkScript->ExposeFunction("GetCharStats", DMLua::GetCharStatsLua);
	pkScript->ExposeFunction("SetCharStats", DMLua::SetCharStatsLua);
	pkScript->ExposeFunction("Equip", DMLua::EquipLua);
	
	// character behaviours
	pkScript->ExposeFunction("PanicArea", DMLua::PanicAreaLua);
	pkScript->ExposeFunction("CallForHelp", DMLua::CallForHelp);

	// path finding
	pkScript->ExposeFunction("HavePath", DMLua::HavePathLua);					
	pkScript->ExposeFunction("MakePathFind", DMLua::MakePathFindLua);
	pkScript->ExposeFunction("ClearPathFind", DMLua::ClearPathFindLua);
	pkScript->ExposeFunction("SetRunAnim", DMLua::SetRunAnimLua);
	pkScript->ExposeFunction("SetIdleAnim", DMLua::SetIdleAnimLua);

	// math
	pkScript->ExposeFunction("Random", DMLua::RandomLua);

	// weapon script
	pkScript->ExposeFunction("SetGunFireRate", DMLua::SetGunFireRateLua);				
	pkScript->ExposeFunction("SetGunDamage", DMLua::SetGunDamageLua);				
	pkScript->ExposeFunction("SetGunRange", DMLua::SetGunRangeLua);				
	pkScript->ExposeFunction("SetGunAmmo", DMLua::SetGunAmmoLua);				
	pkScript->ExposeFunction("SetGunMaxAmmo", DMLua::SetGunAmmoLua);			
	pkScript->ExposeFunction("SetGunRandom", DMLua::SetGunRandomLua);				
	pkScript->ExposeFunction("SetGunBulletsPerAmmo", DMLua::SetGunBulletsPerAmmoLua);	
	pkScript->ExposeFunction("SetGunSound", DMLua::SetGunSoundLua);	

	// common
	pkScript->ExposeFunction("SetVar", DMLua::SetVarLua);
	pkScript->ExposeFunction("GetVar", DMLua::GetVarLua);

	// lua lua scripts
	pkScript->ExposeFunction("RunScript", DMLua::RunScriptLua);	

	// entity
	pkScript->ExposeFunction("SetEntityVar", DMLua::SetEntityVarLua);	
	pkScript->ExposeFunction("GetEntityVar", DMLua::GetEntityVarLua);
	pkScript->ExposeFunction("AddToEntityVar", DMLua::AddToEntityVarLua);
	pkScript->ExposeFunction("GetEntityPos", DMLua::GetEntityPosLua);

	// Item bonuses
	pkScript->ExposeFunction("SetItemArmourBonus", DMLua::SetItemArmourLua);
	pkScript->ExposeFunction("SetItemMaxLifeBonus", DMLua::SetItemMaxLifeLua);
	pkScript->ExposeFunction("SetItemSpeedBonus", DMLua::SetItemSpeedLua);
	pkScript->ExposeFunction("SetItemAimBonus", DMLua::SetItemAimLua);

	// SI for houses
	pkScript->ExposeFunction("SwallowPlayer", DMLua::SwallowPlayerLua);
	pkScript->ExposeFunction("SetIsHouse", DMLua::IsHouseLua);

	// stuff
	pkScript->ExposeFunction("Explosion", DMLua::ExplosionLua);

	// shop functions
	pkScript->ExposeFunction("AddItemToShop", DMLua::AddItemToShopLua);

	// hmm... team related
	pkScript->ExposeFunction("GetCharsByFraction", DMLua::GetCharsByFractionLua);
	pkScript->ExposeFunction("GetDMObject", DMLua::GetDMObjectLua);

	// police functions
	pkScript->ExposeFunction("GetClosestCaller", DMLua::GetClosestCallerLua);
	
	cout << "DM LUA Scripts Loaded" << endl;

}
// ------------------------------------------------------------------------------------------------

int DMLua::GetDMCharacterByNameLua(lua_State* pkLua) 
{
	double dObjectID = -1;

	P_DMCharacter* pkCharacter;
	char szName[256];

	g_pkScript->GetArgString(pkLua, 0, szName);

	vector<Entity*> kObjects;		
	g_pkObjMan->GetAllObjects(&kObjects);

	for(unsigned int i=0;i<kObjects.size();i++)
		if((pkCharacter = (P_DMCharacter *) kObjects[i]->GetProperty("P_DMCharacter")))
		{
			if( pkCharacter->GetStats()->m_strName.c_str() == string(szName) )
			{
				dObjectID = kObjects[i]->GetEntityID();
				break;
			}
		}

	g_pkScript->AddReturnValue(pkLua, dObjectID);

	return 1; // this function returns one (1) argument
}
// ------------------------------------------------------------------------------------------------

int DMLua::SetDMCharacterNameLua(lua_State* pkLua) 
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0 )
	{
		cout << "Warning! DMLua::SetDMCharacterNameLua: To few arguments (ID, Name) or object not found." << endl;
		return 0;
	}

	P_DMCharacter* pkCharacter = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter");

	if ( pkCharacter == 0 )
	{
		cout << "Warning! DMLua::SetDMCharacterNameLua: Object didn't have P_DMCharacter!" << endl;
		return 0;
	}

	char szName[256];
	g_pkScript->GetArgString(pkLua, 1, szName);

	pkCharacter->GetStats()->m_strName == szName;

	return 0;
}
// ------------------------------------------------------------------------------------------------

// takes CharacterID, and optional 
int DMLua::GetDMCharacterClosestLua(lua_State* pkLua) 
{
	double dObjectID = -1;

	int iClosestCharID = 0;

	if( g_pkScript->GetNumArgs(pkLua) == 1 )
	{
		P_DMCharacter* pkCharacter;
		double dFromObjectID, dClosestDistance = 999999999;

		g_pkScript->GetArgNumber(pkLua, 0, &dFromObjectID);

		// if it crashed here sometime, give zerom a kick
		Vector3 kFrom = g_pkObjMan->GetObjectByNetWorkID(int(dFromObjectID))->GetWorldPosV();

		vector<Entity*> kObjects;		
		g_pkObjMan->GetAllObjects(&kObjects);

		for(unsigned int i=0;i<kObjects.size();i++)
			if( dFromObjectID != kObjects[i]->GetEntityID() &&
			    (pkCharacter = (P_DMCharacter *) kObjects[i]->GetProperty("P_DMCharacter")) )
			{
				if(pkCharacter->m_iTeam == 0) // endast spelarens agenter
				{
					double dDist = kObjects[i]->GetWorldPosV().DistanceTo (kFrom);
					// ignore if dead
					if ( dDist < dClosestDistance && pkCharacter->GetStats()->m_iLife > 0 )
					{
						dClosestDistance = dDist;
						iClosestCharID = kObjects[i]->GetEntityID();
					}
				}
			}

	}

	g_pkScript->AddReturnValue(pkLua, iClosestCharID);

	return 1; // this function returns one (1) argument
}

int DMLua::GetNumOfLivingAgentsLua(lua_State* pkLua)
{
	double dNumAgents = 0;
	P_DMCharacter* pkCharacter;

	vector<Entity*> kObjects;		
	g_pkObjMan->GetAllObjects(&kObjects);

	for(unsigned int i=0;i<kObjects.size();i++)
		if((pkCharacter=(P_DMCharacter*)kObjects[i]->GetProperty("P_DMCharacter")))
		{
			//////////////////////////////////////////////////////////////
			// TODO: Kolla om det är en agent eller en fiende
			// (kanske se om det finns ett HQ objekt property)
			//////////////////////////////////////////////////////////////

			dNumAgents += 1;
			break;
		}

	g_pkScript->AddReturnValue(pkLua, dNumAgents);

	return 1; // this function returns one (1) arguments
}

// ------------------------------------------------------------------------------------------------

int DMLua::HavePathLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dId;
		int ret = 0;
		
		g_pkScript->GetArgNumber(pkLua, 0, &dId);				
		
		Entity* pkEnt = g_pkObjMan->GetObjectByNetWorkID((int)dId);
		if(pkEnt)
		{
			P_PfPath* pf = (P_PfPath*)pkEnt->GetProperty("P_PfPath");
			if(pf)
				if(pf->HavePath())
				{
					g_pkScript->AddReturnValue(pkLua,1);
					return 1;		
				}
		}
		
	}

	g_pkScript->AddReturnValue(pkLua,0);
	return 1;
}

// ------------------------------------------------------------------------------------------------

int DMLua::MakePathFindLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 2)
	{
		double dId;
		Vector3 kPos;		
		vector<TABLE_DATA> vkData;
		
		g_pkScript->GetArgNumber(pkLua, 0, &dId);				
		g_pkScript->GetArgTable(pkLua, 2, vkData);

		kPos = Vector3(
			(float) (*(double*) vkData[0].pData),
			(float) (*(double*) vkData[1].pData),
			(float) (*(double*) vkData[2].pData)); 
		
		Entity* pkEnt = g_pkObjMan->GetObjectByNetWorkID((int)dId);
		if(pkEnt)
		{
			P_PfPath* pf = (P_PfPath*)pkEnt->GetProperty("P_PfPath");
			if(pf)
				pf->MakePathFind(kPos);
		}
	}
	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::ClearPathFindLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dId;
		
		g_pkScript->GetArgNumber(pkLua, 0, &dId);				

		Entity* pkEnt = g_pkObjMan->GetObjectByNetWorkID((int)dId);
		if(pkEnt)
		{
			P_PfPath* pf = (P_PfPath*)pkEnt->GetProperty("P_PfPath");
			if(pf)
				pf->ClearPath();
		}
	}

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::SetRunAnimLua(lua_State* pkLua)
{
	Entity* pkObj = TestScriptInput (2, pkLua);

	if ( pkObj == 0 )
	{
		cout << "Warning! DMLua::SetRunAnimLua: Takes two arguments, ID and animationName(str), or objectID not found." << endl;
		return 0;
	}

	char caAnimName[255];
	
	g_pkScript->GetArgString (pkLua, 1, caAnimName);

	if (P_PfPath* pf = (P_PfPath*)pkObj->GetProperty("P_PfPath"))
		pf->SetRunAnim(string(caAnimName));

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::SetIdleAnimLua(lua_State* pkLua)
{
	Entity* pkObj = TestScriptInput (2, pkLua);

	if ( pkObj == 0 )
	{
		cout << "Warning! DMLua::SetIdleAnimLua: Takes two arguments, ID and animationName(str), or objectID not found." << endl;
		return 0;
	}

	char caAnimName[255];
	
	g_pkScript->GetArgString (pkLua, 1, caAnimName);

	if (P_PfPath* pf = (P_PfPath*)pkObj->GetProperty("P_PfPath"))
		pf->SetIdleAnim(string(caAnimName));

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::RandomLua (lua_State* pkLua)
{
 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
      double dTemp;
      g_pkScript->GetArgNumber(pkLua, 0, &dTemp);

      if ( dTemp < 0 )
         dTemp = 1;

	  dTemp = rand()%int(dTemp);

      g_pkScript->AddReturnValue(pkLua, dTemp );

      return 1;
   }

   return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::SetGunRandomLua (lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0 )
		return 0;

	// get P_DMGun
	P_DMGun* pkGunP = (P_DMGun*)pkEntity->GetProperty("P_DMGun");

	if ( pkGunP == 0 )
	{
		cout << "Warning! DMLua::SetGunRandom: Must use on an object with P_DMGun!" << endl;
		return 0;
	}

	double dRandom;
	g_pkScript->GetArgNumber(pkLua, 1, &dRandom);
	pkGunP->SetRandom ( float(dRandom) );

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::SetGunFireRateLua (lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0 )
		return 0;

	// get P_DMGun
	P_DMGun* pkGunP = (P_DMGun*)pkEntity->GetProperty("P_DMGun");

	if ( pkGunP == 0 )
	{
		cout << "Warning! DMLua::SetGunFireRate: Must use on an object with P_DMGun!" << endl;
		return 0;
	}

	double dFireRate;
	g_pkScript->GetArgNumber(pkLua, 1, &dFireRate);
	pkGunP->SetFireRate ( float(dFireRate) );

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::SetGunDamageLua (lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0 )
		return 0;

	// get P_DMGun
	P_DMGun* pkGunP = (P_DMGun*)pkEntity->GetProperty("P_DMGun");

	if ( pkGunP == 0 )
	{
		cout << "Warning! DMLua::SetGunDamage: Must use on an object with P_DMGun!" << endl;
		return 0;
	}

	double dDamage;
	g_pkScript->GetArgNumber(pkLua, 1, &dDamage);
	pkGunP->SetDamage ( float(dDamage) );

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::SetGunRangeLua (lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0 )
		return 0;

	// get P_DMGun
	P_DMGun* pkGunP = (P_DMGun*)pkEntity->GetProperty("P_DMGun");

	if ( pkGunP == 0 )
	{
		cout << "Warning! DMLua::SetGunRange: Must use on an object with P_DMGun!" << endl;
		return 0;
	}

	double dRange;
	g_pkScript->GetArgNumber(pkLua, 1, &dRange);
	pkGunP->SetRange ( float(dRange) );

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::SetGunMaxAmmoLua (lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0 )
		return 0;

	// get P_DMGun
	P_DMGun* pkGunP = (P_DMGun*)pkEntity->GetProperty("P_DMGun");

	if ( pkGunP == 0 )
	{
		cout << "Warning! DMLua::SetGunMaxAmmo: Must use on an object with P_DMGun!" << endl;
		return 0;
	}

	double dAmmo;
	g_pkScript->GetArgNumber(pkLua, 1, &dAmmo);
	pkGunP->SetMaxAmmo ( int(dAmmo) );

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::SetGunAmmoLua (lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0 )
		return 0;

	// get P_DMGun
	P_DMGun* pkGunP = (P_DMGun*)pkEntity->GetProperty("P_DMGun");

	if ( pkGunP == 0 )
	{
		cout << "Warning! DMLua::SetGunAmmo: Must use on an object with P_DMGun!" << endl;
		return 0;
	}

	double dAmmo;
	g_pkScript->GetArgNumber(pkLua, 1, &dAmmo);
	pkGunP->SetAmmo ( int(dAmmo) );

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::SetGunBulletsPerAmmoLua (lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0 )
		return 0;

	// get P_DMGun
	P_DMGun* pkGunP = (P_DMGun*)pkEntity->GetProperty("P_DMGun");

	if ( pkGunP == 0 )
	{
		cout << "Warning! DMLua::SetBurstLenghtLua: Must use on an object with P_DMGun!" << endl;
		return 0;
	}

	double dBLenght;
	g_pkScript->GetArgNumber(pkLua, 1, &dBLenght);
	pkGunP->SetBulletsPerAmmo ( int(dBLenght) );

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::SetGunSoundLua (lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0 )
		return 0;

	// get P_DMGun
	P_DMGun* pkGunP = (P_DMGun*)pkEntity->GetProperty("P_DMGun");

	if ( pkGunP == 0 )
	{
		cout << "Warning! DMLua::SetBurstLenghtLua: Must use on an object with P_DMGun!" << endl;
		return 0;
	}

	char sound[256];
	g_pkScript->GetArgString(pkLua, 1, sound);
	pkGunP->SetSound ( string(sound) );

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::RunScriptLua (lua_State* pkLua)
{
 	if( g_pkScript->GetNumArgs(pkLua) == 2 || g_pkScript->GetNumArgs(pkLua) == 1)
   {
     	char	acType[128];
		g_pkScript->GetArgString(pkLua, 0, acType);

      double temp;
		g_pkScript->GetArgNumber(pkLua, 1, &temp);
 		int objectid = (int) temp;


      ZFScriptSystem* pkZFScriptSys = g_pkScript;
	   
	   Entity* object = g_pkObjMan->GetObjectByNetWorkID(objectid);
	   if(!object)
	   {
		   cout<<"parent object does not exist"<<endl;
		   return 0;
	   }
   
      // create the new object
      Entity* pkNewObj = g_pkObjMan->CreateObjectFromScriptInZone(acType, 
                         object->GetWorldPosV() );

      // return everything the way it was
      g_pkScript = pkZFScriptSys;

      // return the new object´s ID      
      g_pkScript->AddReturnValue( pkLua, pkNewObj->GetEntityID() );

      return 1; // return newcreated-object_ID
      
   } 
   return 0;  

}

// ------------------------------------------------------------------------------------------------

// takes entityID, Key
int DMLua::GetEntityVarLua (lua_State* pkLua)
{
	char cKey[255];
	double dValue, dEntID;
	
	
	if ( g_pkScript->GetNumArgs(pkLua) != 2 )
	{
		cout << "Warning! DMLua::GetEntityVarLua: Wrong number of arguments! Takes entityID and key(string)." << endl;
		return 0;
	}

	g_pkScript->GetArgNumber(pkLua, 0, &dEntID);

	Entity* pkEntity = g_pkObjMan->GetObjectByNetWorkID( int(dEntID) );

	if ( pkEntity == 0)
	{
		cout << "Warning! DMLua::GetEntityVarLua: No entity found with that ID." << endl;
		return 0;
	}

	g_pkScript->GetArgString(pkLua, 1, cKey);

	string strTemp = cKey;
	EntityVariable* pkEntVar = pkEntity->GetVar(strTemp);

	if ( pkEntVar == 0 )
		dValue = 0;
	else
		dValue = pkEntVar->m_fValue;

	g_pkScript->AddReturnValue(pkLua, dValue);

	return 1;

}

// ------------------------------------------------------------------------------------------------

// takes entityID, Key, value (double)
int DMLua::SetEntityVarLua (lua_State* pkLua)
{
	char cKey[255];
	double dValue, dEntID;
	
	
	if ( g_pkScript->GetNumArgs(pkLua) != 3 )
	{
		cout << "Warning! DMLua::SetEntityVarLua: Wrong number of arguments! Takes entityID, key(string) and value(double)." << endl;
		return 0;
	}

	g_pkScript->GetArgNumber(pkLua, 0, &dEntID);

	Entity* pkEntity = g_pkObjMan->GetObjectByNetWorkID( int(dEntID) );

	if ( pkEntity == 0)
	{
		cout << "Warning! DMLua::SetEntityVarLua: No entity found with that ID." << endl;
		return 0;
	}

	g_pkScript->GetArgString(pkLua, 1, cKey);
	g_pkScript->GetArgNumber(pkLua, 2, &dValue);


	string strTemp = cKey;
	pkEntity->SetVarDouble (strTemp, dValue);

	return 0;

}

// ------------------------------------------------------------------------------------------------

// takes entityID, Key, value (double) to add
int DMLua::AddToEntityVarLua (lua_State* pkLua)
{
	char cKey[255];
	double dAddValue, dEntID;	
	
	if ( g_pkScript->GetNumArgs(pkLua) != 3 )
	{
		cout << "Warning! DMLua::AddToEntityVarLua: Wrong number of arguments! Takes entityID, key(string) and value(double)." << endl;
		return 0;
	}

	g_pkScript->GetArgNumber(pkLua, 0, &dEntID);

	Entity* pkEntity = g_pkObjMan->GetObjectByNetWorkID( int(dEntID) );

	if ( pkEntity == 0)
	{
		cout << "Warning! DMLua::AddToEntityVarLua: No entity found with that ID." << endl;
		return 0;
	}

	g_pkScript->GetArgString(pkLua, 1, cKey);
	g_pkScript->GetArgNumber(pkLua, 2, &dAddValue);

	double dVar;

	string strTemp = cKey;
	EntityVariable* pkEntVar = pkEntity->GetVar(strTemp);

	if ( pkEntVar == 0 )
		dVar = dAddValue;
	else
		dVar = pkEntVar->m_fValue + dAddValue;

	string strTemp2 = cKey;
	pkEntity->SetVarDouble (strTemp2, dVar);

	return 0;

}

// ------------------------------------------------------------------------------------------------

int DMLua::SetVarLua (lua_State* pkLua)
{
	if ( g_pkScript->GetNumArgs(pkLua) != 2 )
	{
		cout << "Warning! DMLua::SetVarLua: Wrong number of arguments! Takes key(string) and value(double)." << endl;
		return 0;
	}

	char cKey[255];
	double dValue;

	g_pkScript->GetArgString(pkLua, 0, cKey);
	g_pkScript->GetArgNumber(pkLua, 1, &dValue);

	m_kVars[cKey] = dValue;

	return 0;

}

// ------------------------------------------------------------------------------------------------

int DMLua::GetVarLua (lua_State* pkLua)
{
	if ( g_pkScript->GetNumArgs(pkLua) != 1 )
	{
		cout << "Warning! DMLua::GetVarLua: Wrong number of arguments! Takes key(string)." << endl;
		return 0;
	}

	char cKey[255];

	g_pkScript->GetArgString(pkLua, 0, cKey);

	map<string, double>::iterator kIte = m_kVars.find(cKey);

	if ( kIte == m_kVars.end() )
	{
		cout << "Warning! DMLua::GetVarLua: Coulnd't find key:" << cKey << ". Setting value to 0." << endl;
		m_kVars[cKey] = 0;
	}

	double dValue;

	dValue = m_kVars[cKey];

	g_pkScript->AddReturnValue(pkLua, dValue);

	return 1;

}

// ------------------------------------------------------------------------------------------------

Entity* DMLua::TestScriptInput (int iArgs, lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) != iArgs )
		return false;

	double dEntID;

	g_pkScript->GetArgNumber(pkLua, 0, &dEntID);

	Entity* pkEntity = g_pkObjMan->GetObjectByNetWorkID( int(dEntID) );

	if ( pkEntity )
		return pkEntity;
	else
		return 0;	
}

// ------------------------------------------------------------------------------------------------

Entity* DMLua::GetHQEntity()
{
	vector<Entity*> kObjects;		
	g_pkObjMan->GetAllObjects(&kObjects);

	for(unsigned int i=0;i<kObjects.size();i++)
		if( (P_DMHQ*)kObjects[i]->GetProperty("P_DMHQ") != 0 )
			return kObjects[i];

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::SetItemArmourLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0 )
	{
		cout << "Warning! DMLua::SetItemArmourLua: Wrong number of arg (ID, value) or entityID not found." << endl;
		return 0;
	}

	P_DMItem* pkItem = (P_DMItem*)pkEntity->GetProperty("P_DMItem");
	if ( pkItem )
	{
		double dValue;		
		g_pkScript->GetArgNumber(pkLua, 1, &dValue);
        pkItem->m_kItemStats.m_fArmourVal = float(dValue);
	}
	else
		cout << "Warning! DMLua::SetItemArmourLua: Tried to set Item value on a non-item entity." << endl;

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::SetItemMaxLifeLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0 )
	{
		cout << "Warning! DMLua::SetItemMaxLifeLua: Wrong number of arg (ID, value) or entityID not found." << endl;
		return 0;
	}

	P_DMItem* pkItem = (P_DMItem*)pkEntity->GetProperty("P_DMItem");
	if ( pkItem )
	{
		double dValue;		
		g_pkScript->GetArgNumber(pkLua, 1, &dValue);
        pkItem->m_kItemStats.m_iMaxLifeVal = float(dValue);
	}
	else
		cout << "Warning! DMLua::SetItemMaxLifeLua: Tried to set Item value on a non-item entity." << endl;

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::SetItemAimLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0 )
	{
		cout << "Warning! DMLua::SetItemAimLua: Wrong number of arg (ID, value) or entityID not found." << endl;
		return 0;
	}

	P_DMItem* pkItem = (P_DMItem*)pkEntity->GetProperty("P_DMItem");
	if ( pkItem )
	{
		double dValue;		
		g_pkScript->GetArgNumber(pkLua, 1, &dValue);
        pkItem->m_kItemStats.m_fAim = float(dValue);
	}
	else
		cout << "Warning! DMLua::SetItemAimLua: Tried to set Item value on a non-item entity." << endl;

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::SetItemSpeedLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0 )
	{
		cout << "Warning! DMLua::SetItemSpeedLua: Wrong number of arg (ID, value) or entityID not found." << endl;
		return 0;
	}

	P_DMItem* pkItem = (P_DMItem*)pkEntity->GetProperty("P_DMItem");
	if ( pkItem )
	{
		double dValue;		
		g_pkScript->GetArgNumber(pkLua, 1, &dValue);
        pkItem->m_kItemStats.m_fSpeedVal = float(dValue);
	}
	else
		cout << "Warning! DMLua::SetItemSpeedLua: Tried to set Item value on a non-item entity." << endl;

	return 0;
}

// ------------------------------------------------------------------------------------------------
// Takes HouseID, PlayerID, Time before spitting out again, -1 = never
int DMLua::SwallowPlayerLua(lua_State* pkLua)
{
	Entity* pkHouse = TestScriptInput (3, pkLua);

	double dTime, dVisiter;

	if ( pkHouse == 0 )
	{
		cout << "Warning! DMLua::SwallowPlayerLua: Wrong number of arg (HouseID, PlayerId, Time) or entityID not found." << endl;
		return 0;
	}

	g_pkScript->GetArgNumber(pkLua, 1, &dVisiter);
	g_pkScript->GetArgNumber(pkLua, 2, &dTime);

	Entity* pkInhabit = g_pkObjMan->GetObjectByNetWorkID( int(dVisiter) );

	if ( pkInhabit == 0 )
	{
		cout << "Warning! DMLua::SwallowPlayerLua: ObjectID not found." << endl;
		return 0;
	}

	P_DMClickMe* pkClick = (P_DMClickMe*)pkHouse->GetProperty("P_DMClickMe");
	
	if ( pkClick == 0 )
	{
		cout << "Warning! DMLua::SwallowPlayerLua: House doesn't have a P_DMClickMe!" << endl;
		return 0;
	}

	pkClick->AddVisiter ( int(dVisiter), float(dTime) );

	// turn off inhabitant
	pkInhabit->SetUpdateStatus (UPDATE_NONE);

	return 0;
}

// ------------------------------------------------------------------------------------------------
// Takes entID, bool
int DMLua::IsHouseLua(lua_State* pkLua)
{
	Entity* pkHouse = TestScriptInput (2, pkLua);

	double dBool;

	if ( pkHouse == 0 )
	{
		cout << "Warning! DMLua::IsHouseLua: Wrong number of arg (HouseID, IsHouse(bool)) or entityID not found." << endl;
		return 0;
	}

	P_DMClickMe* pkClick = (P_DMClickMe*)pkHouse->GetProperty("P_DMClickMe");
	
	if ( pkClick == 0 )
	{
		cout << "Warning! DMLua::IsHouseLua: House doesn't have a P_DMClickMe!" << endl;
		return 0;
	}

	g_pkScript->GetArgNumber(pkLua, 1, &dBool);

	pkClick->m_bIsHouse = dBool;

	return 0;
}

// ------------------------------------------------------------------------------------------------

// Takes radie, damage
int DMLua::ExplosionLua(lua_State* pkLua)
{
	Entity* pkExpl = TestScriptInput (3, pkLua);

	if ( pkExpl == 0 )
	{
		cout << "Warning! DMLua::ExplosionLua: Takes 3 arguments, objectID, radie and damage." << endl;
		return 0;
	}

	double dRadie, dDamage;

	g_pkScript->GetArgNumber(pkLua, 1, &dRadie);
	g_pkScript->GetArgNumber(pkLua, 2, &dDamage);

	vector<Entity*> kObj;

	g_pkObjMan->GetAllObjects(&kObj);

	Vector3 kBla = pkExpl->GetWorldPosV();

	for ( unsigned int i = 0; i < kObj.size(); i++ )
	{
		// check for characters
		if ( P_DMCharacter* pkChar = (P_DMCharacter*)kObj[i]->GetProperty("P_DMCharacter") )
		{
			// check distance
			double dDist = kObj[i]->GetWorldPosV().DistanceTo(kBla);

			if ( dDist <= dRadie )
			{
				pkChar->Damage (0, int(dDamage * (1.0 - (dDist / dRadie))));
			}
		}
	}

	return 0;
}

// ------------------------------------------------------------------------------------------------

// 1:st arg = Self object ID
// 2:nd arg = Name of the scriptobject to spawn
// 3:rd arg = Price of the new object

int DMLua::AddItemToShopLua(lua_State* pkLua)
{
	Entity* pkShop = TestScriptInput (3, pkLua);

	if ( pkShop == NULL )
	{
		cout << "Warning! DMLua::AddItemToShopLua: Wrong number of arguments" << endl;
		return 0;
	}

	P_DMShop* pkProperty = (P_DMShop*)pkShop->GetProperty("P_DMShop");

	if ( pkProperty == NULL )
	{
		cout << "Error! DMLua::AddItemToShopLua: Shop doesn't have a P_DMShop!" << endl;
		return 0;
	}

   char	acScript[128];
	g_pkScript->GetArgString(pkLua, 1, acScript);

   double dPrice;
	g_pkScript->GetArgNumber(pkLua, 2, &dPrice);

   // create the new object
	Entity* pkNewObj = g_pkObjMan->CreateObjectFromScriptInZone(acScript,
		pkShop->GetWorldPosV() );

	if(pkNewObj == NULL)
	{
		cout << "Error! DMLua::AddItemToShopLua: Failed to create object!" << endl;
		return 0;
	}

	P_DMItem* pkItemProperty = (P_DMItem*)pkNewObj->GetProperty("P_DMItem");

	if(pkItemProperty == NULL)
	{
		cout << "Error! DMLua::AddItemToShopLua: Object have no P_DMItem!" << endl;
		return 0;
	}
	
	int iNewItemID = pkNewObj->GetEntityID();

	if(pkProperty->m_pkItems == NULL)
	{
		printf("DMLua::AddItemToShopLua failed! Shop have no contatiner.\n");
		return 0;
	}

	if(pkProperty->m_pkItems->AddItem(iNewItemID) == false)
	{
		printf("DMLua::AddItemToShopLua failed! Shop is full.\n");
		return 0;
	}

	if( pkProperty->SetPrice( pkItemProperty->GetName(), int(dPrice)) == false)
	{
		printf("DMLua::AddItemToShopLua failed! P_DMShop::SetPrice failed.\n");
		return 0;
	}

	//vector<ContainerInfo> kItemList;
	//pkProperty->m_pkItems->GetItemList(&kItemList);

	//for(int i=0; i<kItemList.size(); i++)
	//{
	//	int id = *pkProperty->m_pkItems->GetItem(
	//		kItemList[i].m_iItemX, kItemList[i].m_iItemY);

	//	if(id == iNewItemID)
	//	{
	//		printf("Creating new item (%i), price: %i, and adding to shop at slot (%i,%i)\n",
	//			iNewItemID, (int) dPrice, kItemList[i].m_iItemX, kItemList[i].m_iItemY);

	//		pkProperty->SetPrice( pkItemProperty->GetName(), dPrice);
	//		break;
	//	}
	//}

	return 0;
}

// ------------------------------------------------------------------------------------------------

Entity* DMLua::GetGameInfoEntity()
{
	Entity* pkGlobal =  g_pkObjMan->GetGlobalObject();

	vector<Entity*> kObjects;

	pkGlobal->GetAllEntitys(&kObjects);

	for(unsigned int i=0;i<kObjects.size();i++)
		if(kObjects[i]->GetProperty("P_DMGameInfo"))
			return kObjects[i];

	return 0;
}

// ------------------------------------------------------------------------------------------------

// sourceID, radie (all characters (civilians) in radie from sourceID panics)
int DMLua::PanicAreaLua(lua_State* pkLua)
{
	Entity* pkObj = TestScriptInput (2, pkLua);

	if ( pkObj == NULL )
	{
		cout << "Warning! DMLua::PanicLua: Function takes two arguments, ID and radie (float) or entityID not found." << endl;
		return 0;
	}

	vector<Entity*> kObj;

	g_pkObjMan->GetAllObjects(&kObj);

	double dRadie;

	g_pkScript->GetArgNumber(pkLua, 1, &dRadie);

	Vector3 kObjPos = pkObj->GetWorldPosV();
	
	for ( unsigned int i = 0; i < kObj.size(); i++ )
	{
		// check for characters
		if ( P_DMCharacter* pkChar = (P_DMCharacter*)kObj[i]->GetProperty("P_DMCharacter") )
		{
			// check distance
			double dDist = kObj[i]->GetWorldPosV().DistanceTo(kObjPos);

			if ( dDist <= dRadie && pkChar->m_iState != PANIC && pkChar->m_iState != DEAD && pkChar->m_iTeam == 1)
				pkChar->ChangeState(PANIC);

		}
	}

	return 0;

}

// Take 2 arguments: Entity ID and 1 (to start call) or -1 (to stop call).
int DMLua::CallForHelp(lua_State* pkLua)
{
	double dEntIDCalling;
	g_pkScript->GetArgNumber(pkLua, 0, &dEntIDCalling);

	Entity* pkEnt = g_pkObjMan->GetObjectByNetWorkID(int(dEntIDCalling));
	if(pkEnt != NULL)
	{
		bool bFound = false;
		double dCallForHelp;
		g_pkScript->GetArgNumber(pkLua, 1, &dCallForHelp);

		vector<int>::iterator it = m_kCallsForHelp.begin();
		for( ; it != m_kCallsForHelp.end(); it++)
		{
			if((*it) == dEntIDCalling)
			{
				// Remove from list to stop calling?
				if(dCallForHelp < 0)
					m_kCallsForHelp.erase(it);

				bFound = true;
				break;
			}
		}

		// Insert caller in list if not already there.
		if(bFound == false)			
			m_kCallsForHelp.push_back(dEntIDCalling);
	}
	
	return 0;
}

// Takes 1 arguments: Entity ID of the police that try to the hear the call.
int DMLua::GetClosestCallerLua(lua_State* pkLua)
{
	double dClosestCaller = -1;
	double dPoliceEntityID = -1;

 	if(g_pkScript->GetArgNumber(pkLua, 0, &dPoliceEntityID))
	{
		Entity* pkEnt = g_pkObjMan->GetObjectByNetWorkID(int(dPoliceEntityID));
		if(pkEnt != NULL)
		{
			float fClosestDist = 99999.9f;
			Vector3 kPolicePos = pkEnt->GetWorldPosV();
	
			vector<int>::iterator it = m_kCallsForHelp.begin();
			for( ; it != m_kCallsForHelp.end(); it++)
			{
				Entity* pkCaller = g_pkObjMan->GetObjectByNetWorkID((*it));

				if(pkCaller)
				{
					Vector3 kCallerPos = pkEnt->GetWorldPosV();

					float fDist = (float) kPolicePos.DistanceTo(kCallerPos);
					if(fDist < fClosestDist)
					{
						fClosestDist = fDist;
						dClosestCaller = (double) (*it);
					}
				}
			}
		}
	}

	g_pkScript->AddReturnValue(pkLua, dClosestCaller);
	return 1;
}

// ------------------------------------------------------------------------------------------------
//
// Fråga om en lista med objectID-nummer på alla av en viss typ. 
// Arg(0) = Team (0,1... 10)
// Returnerar: En lista med nummer. OBS! Listan börjar räkna på element 2. Element [0] är ogiltligt,
// och element [1] anger size. Säker funktion, returnerar en lista med första elementet satt till 0
// även om inget argument sänds in.
int DMLua::GetCharsByFractionLua(lua_State* pkLua)
{
	vector<TABLE_DATA> vkData;
	TABLE_DATA temp;

	double dTeam = -1;
	if(g_pkScript->GetArgNumber(pkLua, 0, &dTeam) == false)
	{
		printf("DMLua::GetCharsByFractionLua failed! Arg 1 isnt correct\n");
		
		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = 0;
		vkData.push_back(temp);
	}
	else
	{
		int iAntal = 0;
		vector<Entity*> kObj;
		g_pkObjMan->GetAllObjects(&kObj);

		for ( unsigned int i = 0; i < kObj.size(); i++ )
		{
			// check for characters
			if ( P_DMCharacter* pkChar = (P_DMCharacter*)kObj[i]->GetProperty("P_DMCharacter") )
			{
				if( pkChar->m_iTeam == (int) dTeam)
				{
					temp.bNumber = true;
					temp.pData = new double;
					(*(double*) temp.pData) = (double) kObj[i]->GetEntityID();
					vkData.push_back(temp);

					iAntal++;
				}
			}
		}

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = (double) iAntal;
		vkData.insert(vkData.begin(), temp);
	}

	g_pkScript->AddReturnValueTable(pkLua, vkData);

	return 1;
}

// ------------------------------------------------------------------------------------------------
// takes entityID (double) 
// returns a table with 3 numbers (pos vector)
// if the function fails the x,y and z pos is -99999
int DMLua::GetEntityPosLua (lua_State* pkLua)
{
	double xPos=-99999, yPos=-99999, zPos=-99999;
	double dEntID;	
	
	if ( g_pkScript->GetNumArgs(pkLua) == 1 )
	{
		if( g_pkScript->GetArgNumber(pkLua, 0, &dEntID) )
		{
			Entity* pkEnt = g_pkObjMan->GetObjectByNetWorkID(int(dEntID));

			if(pkEnt != NULL)
			{
				xPos = pkEnt->GetWorldPosV().x;
				yPos = pkEnt->GetWorldPosV().y;
				zPos = pkEnt->GetWorldPosV().z;
			}
		}
	}

	vector<TABLE_DATA> vkData;
	TABLE_DATA temp;

	temp.bNumber = true;
	temp.pData = new double;
	(*(double*) temp.pData) = (double) xPos;
	vkData.push_back(temp);

	temp.bNumber = true;
	temp.pData = new double;
	(*(double*) temp.pData) = (double) yPos;
	vkData.push_back(temp);

	temp.bNumber = true;
	temp.pData = new double;
	(*(double*) temp.pData) = (double) zPos;
	vkData.push_back(temp);

	g_pkScript->AddReturnValueTable(pkLua, vkData);

	return 1;

}

// ------------------------------------------------------------------------------------------------
//
// Tar ett nummer, returnerar Entity ID på ett object eller -1 om inget finns.
// 0 = HQ
// 1 = Hospital
int DMLua::GetDMObjectLua(lua_State* pkLua)
{
	double dObjectType = -1;
	double dEntID = -1;

	if ( g_pkScript->GetNumArgs(pkLua) == 1 )
	{
		if( g_pkScript->GetArgNumber(pkLua, 0, &dObjectType) )
		{
			vector<Entity*> kObjs;
			g_pkObjMan->GetAllObjects(&kObjs);

			unsigned int i;

			switch((int)dObjectType)
			{
			case 0: // HQ
				for ( i = 0; i < kObjs.size(); i++ )
				{
					P_DMHQ* pkHQProperty = (P_DMHQ*)kObjs[i]->GetProperty("P_DMHQ");
					if(pkHQProperty && pkHQProperty->GetActive())
					{
						dEntID = (double)kObjs[i]->GetEntityID();
						break;
					}
				}
				break;

			case 1: // Hospital
				for ( i = 0; i < kObjs.size(); i++ )
				{
					P_DMClickMe* pkProperty = (P_DMClickMe*)kObjs[i]->GetProperty("P_DMClickMe");
					if(pkProperty)
					{
						string strEntName = kObjs[i]->GetName();

						// Kolla om det är ett sjukhus genom att titta på objektets namn
						if( strEntName.find("t_door_hospit") != string::npos)
						{
							dEntID = (double)kObjs[i]->GetEntityID();
							break;
						}
					}
				}				
				break;
			}
		}
	}

	g_pkScript->AddReturnValue( pkLua, dEntID );
	return 1;
}
