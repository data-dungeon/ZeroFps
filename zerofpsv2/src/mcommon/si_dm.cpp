#include "si_dm.h"
#include "../zerofpsv2/script/zfscript.h"
#include "p_dmhq.h"
#include "p_dmmission.h"
#include "p_dmcharacter.h"

ZFScriptSystem*			DMLua::g_pkScript;
EntityManager*				DMLua::g_pkObjMan;
int							DMLua::g_iMissionStatus;

// ------------------------------------------------------------------------------------------------

void DMLua::Init(EntityManager* pkObjMan,ZFScriptSystem* pkScript)
{
	g_pkObjMan = pkObjMan;
	g_pkScript = pkScript;
	g_iMissionStatus = 0; // 0 = active/inactive, 1 = success, -1 = failed
	
	pkScript->ExposeFunction("GetNumOfLivingAgents", DMLua::GetNumOfLivingAgentsLua);
	pkScript->ExposeFunction("GetDMCharacterByName", DMLua::GetDMCharacterByNameLua);
	pkScript->ExposeFunction("GetDMCharacterClosest", DMLua::GetDMCharacterClosestLua);
	pkScript->ExposeFunction("SetNewMission", DMLua::SetNewMissionLua);
	pkScript->ExposeVariable("CurrentMission", (int*)&DMLua::g_iMissionStatus, tINT); 

	// character functins
	pkScript->ExposeFunction("KillCharacter", DMLua::KillCharacterLua);
	pkScript->ExposeFunction("IsDead", DMLua::IsDeadLua);
	pkScript->ExposeFunction("Heal", DMLua::HealLua);
	pkScript->ExposeFunction("BeamTo", DMLua::BeamToLua);
	pkScript->ExposeFunction("TestSkill", DMLua::TestSkillLua);
	pkScript->ExposeFunction("AddMoney", DMLua::AddMoneyLua);
	pkScript->ExposeFunction("Money", DMLua::MoneyLua);
	pkScript->ExposeFunction("FireAtLocation", DMLua::FireAtLocationLua);
	pkScript->ExposeFunction("FireAtObject", DMLua::FireAtCharacterLua);

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
		Vector3 kFrom = g_pkObjMan->GetObjectByNetWorkID(dFromObjectID)->GetWorldPosV();

		vector<Entity*> kObjects;		
		g_pkObjMan->GetAllObjects(&kObjects);

		for(unsigned int i=0;i<kObjects.size();i++)
			if( dFromObjectID != kObjects[i]->GetEntityID() &&
			    (pkCharacter = (P_DMCharacter *) kObjects[i]->GetProperty("P_DMCharacter")) )
			{
				double dDist = kObjects[i]->GetWorldPosV().DistanceTo (kFrom);
				if ( dDist < dClosestDistance )
				{
					dClosestDistance = dDist;
					iClosestCharID = kObjects[i]->GetEntityID();
				}
			}

	}

	g_pkScript->AddReturnValue(pkLua, iClosestCharID);

	return 1; // this function returns one (1) argument
}
// ------------------------------------------------------------------------------------------------

int DMLua::SetNewMissionLua(lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
	{
		char szName[256];
		P_DMMission* pkMissionProp = NULL;
	
		g_pkScript->GetArgString(pkLua, 0, szName);

		vector<Entity*> kObjects;		
		g_pkObjMan->GetAllObjects(&kObjects);

		for(unsigned int i=0;i<kObjects.size();i++)
			if(kObjects[i]->GetProperty("P_DMHQ"))
			{
				pkMissionProp = (P_DMMission*) kObjects[i]->GetProperty("P_DMMission");
				break;
			}

		if(pkMissionProp == NULL)
		{
			printf("Failed to change mission, failed to find property\n");
			return 0;
		}

		pkMissionProp->SetCurrentMission( szName);
	}

	return 0; // this function returns zero (0) argument
}

// ------------------------------------------------------------------------------------------------

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

// takes an entityID and kills that character
int DMLua::KillCharacterLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (1, pkLua);

	if ( pkEntity == 0)
		return 0;
	
	P_DMCharacter* pkChar = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter");

	if ( pkChar == 0 )
		return 0;

	pkChar->GetStats()->m_iLife = 0;

	return 0;
}

// takes a entityID and returns true if that character is dead
int DMLua::IsDeadLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (1, pkLua);

	if ( pkEntity == 0)
		return 0;
	
	P_DMCharacter* pkChar = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter");

	if ( pkChar == 0 )
		return 0;

	if ( pkChar->GetStats()->m_iLife <= 0 )
		g_pkScript->AddReturnValue(pkLua, 1);
	else
		g_pkScript->AddReturnValue(pkLua, 0);

	return 1;
}


// takes a entityID and gives that character full life
int DMLua::HealLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (1, pkLua);

	if ( pkEntity == 0)
		return 0;
	
	P_DMCharacter* pkChar = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter");

	if ( pkChar == 0 )
		return 0;

	pkChar->GetStats()->m_iLife = 100;

	return 0;
}


// takes a entityID and location (vector3?) and moves that entity to the given position
int DMLua::BeamToLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);





	return 0;
}


// takes entityID, skillname and difficulty, returns true if suceeded
int DMLua::TestSkillLua(lua_State* pkLua)
{
	return 0;
}


// takes an integer and add that much money to the players HQ
int DMLua::AddMoneyLua(lua_State* pkLua)
{
	return 0;
}

// takes nothing, returns (int) players money
int DMLua::MoneyLua(lua_State* pkLua)
{
	return 0;
}

// takes entityID (shooter), vector3? (location to fire at)
int DMLua::FireAtLocationLua(lua_State* pkLua)
{
	return 0;
}

// takes entityID (shooter), entityID (target)
int DMLua::FireAtCharacterLua(lua_State* pkLua)
{
	Entity* pkShotEnt = TestScriptInput (2, pkLua);

	if ( pkShotEnt == 0)
		return 0;

	double dEntID;
	g_pkScript->GetArgNumber(pkLua, 1, &dEntID);

	Entity* pkShotTar = g_pkObjMan->GetObjectByNetWorkID( int(dEntID) );

	if ( pkShotTar == 0)
		return 0;
	
	P_DMCharacter* pkShotChar = (P_DMCharacter*)pkShotEnt->GetProperty("P_DMCharacter");

	if ( pkShotChar == 0 )
		return 0;

	pkShotChar->Shoot ( pkShotTar->GetWorldPosV() );

	return 0;

}


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