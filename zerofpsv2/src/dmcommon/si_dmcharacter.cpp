#include "si_dm.h"
#include "p_dmcharacter.h"
#include "p_dmgun.h"
#include "p_dmgameinfo.h"
#include "../zerofpsv2/engine/p_pfpath.h" 
#include "../zerofpsv2/engine_systems/propertys/p_linktojoint.h"

// ------------------------------------------------------------------------------------------------

int DMLua::GetDMCharacterByNameLua(lua_State* pkLua) 
{
	double dObjectID = -1;

	P_DMCharacter* pkCharacter;
	char szName[256];

	g_pkScript->GetArgString(pkLua, 0, szName);

	vector<Entity*> kObjects;		
	g_pkObjMan->GetAllEntitys(&kObjects);

	for(unsigned int i=0;i<kObjects.size();i++)
		if((pkCharacter = (P_DMCharacter *) kObjects[i]->GetProperty("P_DMCharacter")) != NULL)
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

// takes an entityID and kills that character
int DMLua::KillCharacterLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (1, pkLua);

	if ( pkEntity == 0)
		return 0;
	
	P_DMCharacter* pkChar = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter");

	if ( pkChar == 0 )
		return 0;

	pkChar->Damage (0, 99999);

	return 0;
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
	double dClosestCharID = -1;

	if( g_pkScript->GetNumArgs(pkLua) == 1 )
	{
		P_DMCharacter* pkCharacter;
		double dFromObjectID, dClosestDistance = 999999999;

		if(g_pkScript->GetArgNumber(pkLua, 0, &dFromObjectID))
		{
			// if it crashed here sometime, give zerom a kick
			Vector3 kFrom = g_pkObjMan->GetEntityByID(int(dFromObjectID))->GetWorldPosV();

			vector<Entity*> kObjects;		
			g_pkObjMan->GetAllEntitys(&kObjects);

			for(unsigned int i=0;i<kObjects.size();i++)
				if( dFromObjectID != kObjects[i]->GetEntityID() &&
					(pkCharacter = (P_DMCharacter *) kObjects[i]->GetProperty("P_DMCharacter")) != NULL)
				{
					if(pkCharacter->m_iTeam == 0) // endast spelarens agenter
					{
						double dDist = kObjects[i]->GetWorldPosV().DistanceTo (kFrom);
						// ignore if dead
						if ( dDist < dClosestDistance && pkCharacter->GetStats()->m_iLife > 0 )
						{
							dClosestDistance = dDist;
							dClosestCharID = (double) kObjects[i]->GetEntityID();
						}
					}
				}
		}
	}

	g_pkScript->AddReturnValue(pkLua, dClosestCharID);

	return 1; // this function returns one (1) argument
}


// ------------------------------------------------------------------------------------------------

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

// ------------------------------------------------------------------------------------------------

// takes a entityID and returns character state (0=idle, 1=dead, 2=crouch, 3=panic, 4=agressive)
int DMLua::GetStateLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (1, pkLua);

	if ( pkEntity == 0)
		return 0;
	
	P_DMCharacter* pkChar = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter");

	if ( pkChar == 0 )
		return 0;

	if ( pkChar->GetStats()->m_iLife > 0 )
		g_pkScript->AddReturnValue(pkLua, double(pkChar->GetState()) );
	else
		g_pkScript->AddReturnValue(pkLua, -1);

	return 1;
}

// ------------------------------------------------------------------------------------------------

// takes a entityID (0) and a state value (1)
int DMLua::SetStateLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0)
		return 0;
	
	P_DMCharacter* pkChar = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter");

	if ( pkChar == 0 )
		return 0;

	double dState;
	g_pkScript->GetArgNumber(pkLua, 1, &dState);

	if ( pkChar->GetStats()->m_iLife > 0 )
		pkChar->ChangeState(int(dState));

	return 1;
}

// ------------------------------------------------------------------------------------------------

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

// ------------------------------------------------------------------------------------------------

// takes a entityID and location (vector3?) and moves that entity to the given position
int DMLua::BeamToLocationLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0 )
	{
		cout << "Warning! DMLua::BeamToLua: No object found with given ID." << endl;
		return 0;
	}

	Vector3 kPos;
	vector<TABLE_DATA> vkData;
		
	g_pkScript->GetArgTable(pkLua, 2, vkData);
		
	kPos = Vector3(
		(float) (*(double*) vkData[0].pData),
		(float) (*(double*) vkData[1].pData),
		(float) (*(double*) vkData[2].pData));

    pkEntity->SetWorldPosV ( kPos );

	return 0;
}

// ------------------------------------------------------------------------------------------------

// takes an entityID and moves it to EntityID2's location (id, id)
int DMLua::BeamToObjectLua(lua_State* pkLua)
{
	if ( g_pkScript->GetNumArgs(pkLua) != 2 )
	{
		cout << "Warning! DMLua::BeamToObjectLua: Wrong number of arguments!" << endl;
		return 0;
	}

	double dMoveID, dToID;

	Entity* pkMovEnt, *pkToEnt;

	g_pkScript->GetArgNumber(pkLua, 0, &dMoveID);
	g_pkScript->GetArgNumber(pkLua, 1, &dToID);

//	cout << "MoveID:" << dMoveID << " ToID:" << dToID << endl;

	pkMovEnt = g_pkObjMan->GetEntityByID (int(dMoveID));
	pkToEnt = g_pkObjMan->GetEntityByID (int(dToID));

	if ( pkMovEnt == 0 || pkToEnt == 0 )
	{
		cout << "Warning! DMLua::BeamToObjectLua: One object didn't exist!" << endl;
		return 0;
	}

	pkMovEnt->SetWorldPosV ( pkToEnt->GetWorldPosV() );

	return 0;
}

// ------------------------------------------------------------------------------------------------

// takes entityID, skillname and difficulty, returns true if suceeded
int DMLua::TestSkillLua(lua_State* pkLua)
{
	return 0;
}

// ------------------------------------------------------------------------------------------------

// takes entityID, itemTYPE and returns true if exist
int DMLua::GetItemCharLua(lua_State* pkLua)
{
	double dReturnValue = 0;
	double dType = -1;
	unsigned int i=0;

	Entity* pkEntity = TestScriptInput (1, pkLua);

	if ( pkEntity == 0)
	{
		g_pkScript->AddReturnValue(pkLua, dReturnValue );
		return 0;
	}
	
	P_DMCharacter* pkChar = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter");

	if ( pkChar == 0 )
		return 0;

	g_pkScript->GetArgNumber(pkLua, 1, &dType);

	if(dType < 1)
	{
		g_pkScript->AddReturnValue(pkLua, dReturnValue );
		return 1;
	}

	DMContainer* kContainers[] = 
	{
		pkChar->m_pkBackPack,
		pkChar->m_pkBody,
		pkChar->m_pkBelt,
		pkChar->m_pkHand,
		pkChar->m_pkImplants,
	};

	for(int c=0; c<sizeof(kContainers) / sizeof(kContainers[0]); c++)
	{
		vector<ContainerInfo> kItemList;
		kContainers[c]->GetItemList(&kItemList);
		for(i=0; i<kItemList.size(); i++)
			if(kItemList[i].m_iType == dType)
			{
				int id = *kContainers[c]->GetItem(
					kItemList[i].m_iItemX, kItemList[i].m_iItemY);
				dReturnValue = id;
				break;
			}

		if(dReturnValue > 0)
			break;
	}

	g_pkScript->AddReturnValue(pkLua, dReturnValue);
	return 0;
}

// ------------------------------------------------------------------------------------------------

// takes an integer and add that much money to the players HQ
int DMLua::AddMoneyLua(lua_State* pkLua)
{
	Entity* pkGameInfo = GetGameInfoEntity();

	if ( pkGameInfo == 0 )
	{
		cout << "DMLua::MoneyLua: Warning! No GameInfo object found!";
		return 0;
	}

	if ( g_pkScript->GetNumArgs(pkLua) != 1 )
	{
		cout << "DMLua::AddMoneyLua: Wrong number of args given! Function takes AddMoney (int Money)" << endl;
		return 0;
	}

	double dAddMoney;

	g_pkScript->GetArgNumber(pkLua, 0, &dAddMoney);

	((P_DMGameInfo*)pkGameInfo->GetProperty("P_DMGameInfo"))->m_iMoney += int(dAddMoney);

	if ( ((P_DMGameInfo*)pkGameInfo->GetProperty("P_DMGameInfo"))->m_iMoney < 0 )
		((P_DMGameInfo*)pkGameInfo->GetProperty("P_DMGameInfo"))->m_iMoney = 0;

	return 0;
}

// ------------------------------------------------------------------------------------------------

// takes nothing, returns (int) players money
int DMLua::MoneyLua(lua_State* pkLua)
{
	Entity* pkGameInfo = GetGameInfoEntity();

	if ( pkGameInfo == 0 )
	{
		cout << "DMLua::MoneyLua: Warning! No GameInfo object found!";
		return 0;
	}
	
	int iMoney = ((P_DMGameInfo*)pkGameInfo->GetProperty("P_DMGameInfo"))->m_iMoney;

	g_pkScript->AddReturnValue(pkLua, double(iMoney));

	return 1;
	
}

// ------------------------------------------------------------------------------------------------

// takes entityID (shooter), vector3 (location to fire at)
int DMLua::FireAtLocationLua(lua_State* pkLua)
{
	Entity* pkShotEnt = TestScriptInput (2, pkLua);

	if ( pkShotEnt == 0)
		return 0;

	double dEntID;
	g_pkScript->GetArgNumber(pkLua, 0, &dEntID);

	Vector3 kPos;
	kPos = GetVectorArg(pkLua, 2);

	if ( Entity* pkChar = g_pkObjMan->GetEntityByID( int(dEntID) ) )
		if (P_DMCharacter* pkDMChar = (P_DMCharacter*)pkShotEnt->GetProperty("P_DMCharacter"))
			pkDMChar->Shoot ( kPos );

	return 0;
}

// ------------------------------------------------------------------------------------------------

// takes entityID (shooter), entityID (target)
int DMLua::FireAtCharacterLua(lua_State* pkLua)
{
	Entity* pkShotEnt = TestScriptInput (2, pkLua);

	if ( pkShotEnt == 0)
		return 0;

	double dEntID;
	g_pkScript->GetArgNumber(pkLua, 1, &dEntID);

	Entity* pkShotTar = g_pkObjMan->GetEntityByID( int(dEntID) );

	if ( pkShotTar == 0)
		return 0;
	
	P_DMCharacter* pkShotChar = (P_DMCharacter*)pkShotEnt->GetProperty("P_DMCharacter");

	if ( pkShotChar == 0 )
		return 0;

	pkShotChar->Shoot ( pkShotTar->GetWorldPosV() );

	return 0;

}

// ------------------------------------------------------------------------------------------------

// takes characterID, moveSpeed (float)
int DMLua::SetMoveSpeedLua(lua_State* pkLua)
{
	Entity* pkEnt = TestScriptInput(2, pkLua);

	if ( pkEnt == 0 )
	{
		cout << "Warning! DMLua::SetMoveSpeedLua: Takes 2 arguments, entityID and speed." << endl;
		return 0;
	}

	double dSpeed;
	g_pkScript->GetArgNumber(pkLua, 1, &dSpeed);

	// check first for character
	P_DMCharacter* pkChar = (P_DMCharacter*)pkEnt->GetProperty("P_DMCharacter");

	if (pkChar)
	{
		pkChar->SetMoveSpeed(float(dSpeed));
		return 0;
	}


	P_PfPath* pkPF = (P_PfPath*)pkEnt->GetProperty("P_PfPath");

	if ( pkPF == 0 )
		return 0;

	pkPF->SetSpeed ( float (dSpeed) );

	return 0;	
}

// ------------------------------------------------------------------------------------------------

int DMLua::AddOffenciveActionQuotLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0)
		return 0;
	
	P_DMCharacter* pkChar = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter");

	if ( pkChar == 0 )
		return 0;

	char szSound[256];
	g_pkScript->GetArgString(pkLua, 1, szSound);
	pkChar->m_vkOffenciveActionQuots.push_back( string(szSound) );

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::AddDefenciveActionQuotLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0)
		return 0;
	
	P_DMCharacter* pkChar = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter");

	if ( pkChar == 0 )
		return 0;

	char szSound[256];
	g_pkScript->GetArgString(pkLua, 1, szSound);
	pkChar->m_vkDefenciveActionQuots.push_back( string(szSound) );

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::AddDeathSoundLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0)
		return 0;
	
	P_DMCharacter* pkChar = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter");

	if ( pkChar == 0 )
		return 0;

	char szSound[256];
	g_pkScript->GetArgString(pkLua, 1, szSound);
	pkChar->m_vkDeathSounds.push_back( string(szSound) );

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::AddMoveCharSoundLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0)
		return 0;
	
	P_DMCharacter* pkChar = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter");

	if ( pkChar == 0 )
		return 0;

	char szSound[256];
	g_pkScript->GetArgString(pkLua, 1, szSound);
	pkChar->m_vkMoveSounds.push_back( string(szSound) );

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::AddSelectCharSoundLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0)
		return 0;
	
	P_DMCharacter* pkChar = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter");

	if ( pkChar == 0 )
		return 0;

	char szSound[256];
	g_pkScript->GetArgString(pkLua, 1, szSound);
	pkChar->m_vkSelectSounds.push_back( string(szSound) );

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::SetTeamLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0)
		return 0;
	
	P_DMCharacter* pkChar = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter");

	if ( pkChar == 0 )
		return 0;

	double dTeam;

	g_pkScript->GetArgNumber(pkLua, 1, &dTeam);
	pkChar->m_iTeam = int(dTeam);

	return 0;
}

// ------------------------------------------------------------------------------------------------

// Takes a entityID and a type of stats value (a).
// (a)	(ie. varible)
// 0 = 	m_iLife;
// 1 = 	m_iMaxLife;
// 2 = 	m_fSpeed;	
// 3 = 	m_fArmour;
// 4 = 	m_fWage;
// 5 = 	m_fAim;
// 6 = 	m_fExperience;
// 7 = 	m_fNextLevel;
// 8 = 	m_iLevel;

int DMLua::GetCharStatsLua(lua_State* pkLua)
{
	double dStatsValue = -1.0f;

	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity != 0)
	{
		P_DMCharacter* pkChar = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter");

		if ( pkChar != 0 )
		{
			double dStatsType;
			
			g_pkScript->GetArgNumber(pkLua, 1, &dStatsType);

			switch((int)dStatsType)
			{
			case 0: dStatsValue = pkChar->GetStats()->m_iLife; break; 
			case 1: dStatsValue = pkChar->GetStats()->m_iMaxLife; break;
			case 2: dStatsValue = pkChar->GetStats()->m_fSpeed; break;
			case 3: dStatsValue = pkChar->GetStats()->m_fArmour; break;
			case 4: dStatsValue = pkChar->GetStats()->m_fWage; break;
			case 5: dStatsValue = pkChar->GetStats()->m_fAim; break; 
			case 6: dStatsValue = pkChar->GetStats()->m_fExperience; break;
			case 7: dStatsValue = pkChar->GetStats()->m_fNextLevel; break; 
			case 8: dStatsValue = pkChar->GetStats()->m_iLevel; break;
			}								
		}
	}

	g_pkScript->AddReturnValue(pkLua,double(dStatsValue));

	return 1;
}

// ------------------------------------------------------------------------------------------------

int DMLua::GetWeaponRangeLua(lua_State* pkLua)
{
	if ( g_pkScript->GetNumArgs(pkLua) != 1 )
	{
		cout << "Warning! DMLua::GetWeaponRangeLua: Wrong number of arguments! Takes ID" << endl;
		return 0;
	}
	
	double dEntID;

	g_pkScript->GetArgNumber(pkLua, 0, &dEntID);

	Entity* pkEntity = g_pkObjMan->GetEntityByID( int(dEntID) );

	if ( pkEntity == 0 )
	{
		cout << "Warning! DMLua::GetWeaponRangeLua: no object with ID found." << endl;
		return 0;
	}

	P_DMGun* pkGun = (P_DMGun*)pkEntity->GetProperty("P_DMGun");

	if ( pkGun )
	{		
		g_pkScript->AddReturnValue(pkLua, double(pkGun->Range()));
		return 1;
	}

	if ( P_DMCharacter* pkChar = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter") )
	{
		if ( P_DMGun* pkGun2 = pkChar->GetGun() )
		{		
			g_pkScript->AddReturnValue(pkLua, pkGun2->Range());
			return 1;
		}
	}


	return 0;
}

// ---------------------------------------------------------------------------------------------

// Takes a entityID, a type of stats value (a), and a stats value (b).
// (a)	(ie. varible)
// 0 = 	m_iLife;
// 1 = 	m_iMaxLife;
// 2 = 	m_fSpeed;	
// 3 = 	m_fArmour;
// 4 = 	m_fWage;
// 5 = 	m_fAim;
// 6 = 	m_fExperience;
// 7 = 	m_fNextLevel;
// 8 = 	m_iLevel;

int DMLua::SetCharStatsLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (3, pkLua);

	if ( pkEntity == 0)
		return 0;
	
	P_DMCharacter* pkChar = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter");

	if ( pkChar == 0 )
		return 0;

	double dStatsType;
	double dStatsValue = -1.0f;

	g_pkScript->GetArgNumber(pkLua, 1, &dStatsType);
	g_pkScript->GetArgNumber(pkLua, 2, &dStatsValue);

	switch((int)dStatsType)
	{
	case 0: pkChar->GetStats()->m_iLife = int(dStatsValue); break; 
	case 1: pkChar->GetStats()->m_iMaxLife = int(dStatsValue); break;
	case 2: pkChar->GetStats()->m_fSpeed = float(dStatsValue); break;
	case 3: pkChar->GetStats()->m_fArmour = float(dStatsValue); break;
	case 4: pkChar->GetStats()->m_fWage = float(dStatsValue); break;
	case 5: pkChar->GetStats()->m_fAim = float(dStatsValue); break; 
	case 6: pkChar->GetStats()->m_fExperience = int(dStatsValue); break;
	case 7: pkChar->GetStats()->m_fNextLevel = int(dStatsValue); break; 
	case 8: pkChar->GetStats()->m_iLevel = int(dStatsValue); break;
	}																			 

	return 0;
}

// ------------------------------------------------------------------------------------------

int DMLua::SetLeaderOfTeamLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (2, pkLua);

	if ( pkEntity == 0 )
		return 0;
	
	P_DMCharacter* pkChar = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter");

	if ( pkChar == 0 )
		return 0;

	double dTeam;
	g_pkScript->GetArgNumber(pkLua, 1, &dTeam);

	pkChar->m_iLeaderOfTeam = int(dTeam);

	return 0;
}

// ------------------------------------------------------------------------------------------




Vector3 DMLua::GetVectorArg(lua_State* pkLua, int iIndex)
{
	Vector3 kPos;
	vector<TABLE_DATA> vkData;
	g_pkScript->GetArgTable(pkLua, iIndex, vkData);
	kPos = Vector3(
		(float) (*(double*) vkData[0].pData),
		(float) (*(double*) vkData[1].pData),
		(float) (*(double*) vkData[2].pData));
	g_pkScript->DeleteTable(vkData);
	return kPos;
}























