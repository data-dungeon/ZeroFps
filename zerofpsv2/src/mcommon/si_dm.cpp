#include "si_dm.h"
#include "../zerofpsv2/script/zfscript.h"
#include "p_dmhq.h"
#include "p_dmmission.h"
#include "p_dmcharacter.h"

ZFScriptSystem*			DMLua::g_pkScript;
EntityManager*				DMLua::g_pkObjMan;
int							DMLua::g_iCurrentMission;

void DMLua::Init(EntityManager* pkObjMan,ZFScriptSystem* pkScript)
{
	g_pkObjMan = pkObjMan;
	g_pkScript = pkScript;
	g_iCurrentMission = 0;
	
	pkScript->ExposeFunction("GetDMCharacterByName", DMLua::GetDMCharacterByNameLua);
	pkScript->ExposeFunction("GetDMCharacterClosest", DMLua::GetDMCharacterClosestLua);
	pkScript->ExposeFunction("SetNewMission", DMLua::SetNewMissionLua);
	pkScript->ExposeVariable("CurrentMission", (int*)&DMLua::g_iCurrentMission, tINT); 
}

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

int DMLua::GetDMCharacterClosestLua(lua_State* pkLua) 
{
	double dObjectID = -1;

	if( g_pkScript->GetNumArgs(pkLua) == 1 )
	{
		P_DMCharacter* pkCharacter;
		double dFromObjectID;

		g_pkScript->GetArgNumber(pkLua, 0, &dFromObjectID);

		vector<Entity*> kObjects;		
		g_pkObjMan->GetAllObjects(&kObjects);

		for(unsigned int i=0;i<kObjects.size();i++)
			if((pkCharacter = (P_DMCharacter *) kObjects[i]->GetProperty("P_DMCharacter")))
			{
				// etc
			}
	}

	g_pkScript->AddReturnValue(pkLua, dObjectID);

	return 1; // this function returns one (1) argument
}

int DMLua::SetNewMissionLua(lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
	{
		char szName[256];
		double dDifficulty;
		P_DMMission* pkMissionProp = NULL;
		
		g_pkScript->GetArgString(pkLua, 0, szName);
		g_pkScript->GetArgNumber(pkLua, 1, &dDifficulty);

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

		pkMissionProp->SetCurrentMission( szName, (int) dDifficulty );
	}

	return 0;
}