#include "si_dm.h"
#include "p_dmcharacter.h"
#include "p_dmgameinfo.h"
#include "../zerofpsv2/engine/p_pfpath.h" 
#include "../zerofpsv2/engine_systems/propertys/p_linktojoint.h"

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

// takes a entityID and returns true if that character is dead
int DMLua::GetStateLua(lua_State* pkLua)
{
	Entity* pkEntity = TestScriptInput (1, pkLua);

	if ( pkEntity == 0)
		return 0;
	
	P_DMCharacter* pkChar = (P_DMCharacter*)pkEntity->GetProperty("P_DMCharacter");

	if ( pkChar == 0 )
		return 0;

	if ( pkChar->GetStats()->m_iLife > 0 )
		g_pkScript->AddReturnValue(pkLua, double(pkChar->m_iState) );
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
		pkChar->ChangeState(dState);

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

	pkMovEnt = g_pkObjMan->GetObjectByNetWorkID (int(dMoveID));
	pkToEnt = g_pkObjMan->GetObjectByNetWorkID (int(dToID));

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

// takes entityID (shooter), vector3? (location to fire at)
int DMLua::FireAtLocationLua(lua_State* pkLua)
{
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

	Entity* pkShotTar = g_pkObjMan->GetObjectByNetWorkID( int(dEntID) );

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
	case 6: pkChar->GetStats()->m_fExperience = float(dStatsValue); break;
	case 7: pkChar->GetStats()->m_fNextLevel = float(dStatsValue); break; 
	case 8: pkChar->GetStats()->m_iLevel = int(dStatsValue); break;
	}																			 

	return 0;
}

// ------------------------------------------------------------------------------------------------

// Tar ett object id, en string som anger objektet som skall skapas och läggas till
// sammt ett id nummer som anger vilken container som den skall läggas i.
int DMLua::EquipLua(lua_State* pkLua)
{
	double dEntityID = -1;
	double dContainerType = -1;
	char acScript[256] = "";

	if ( g_pkScript->GetNumArgs(pkLua) == 3 )
	{
		g_pkScript->GetArgNumber(pkLua, 0, &dEntityID);
		g_pkScript->GetArgString(pkLua, 1, acScript);
		g_pkScript->GetArgNumber(pkLua, 2, &dContainerType);
	}

	if(strlen(acScript) < 1 || dEntityID == -1)
	{
		cout << "DMLua::EquipLua failed, wrong arguments\n";
		return 0;
	}

	Entity* pkEnt = g_pkObjMan->GetObjectByNetWorkID( int(dEntityID) );
	if ( pkEnt == NULL )
	{
		cout << "Warning! DMLua::EquipLua: ObjectID not found." << endl;
		return 0;
	}

	P_DMCharacter* pkChar = (P_DMCharacter*)pkEnt->GetProperty("P_DMCharacter");
	if ( pkChar == NULL )
		return 0;

   // create the new object
	Entity* pkNewObj = g_pkObjMan->CreateObjectFromScriptInZone(acScript,
		pkEnt->GetWorldPosV() );

	if(pkNewObj == NULL)
	{
		cout << "Error! DMLua::EquipLua: Failed to create object!" << endl;
		return 0;
	}

	P_DMItem* pkItemProperty = (P_DMItem*)pkNewObj->GetProperty("P_DMItem");

	if(pkItemProperty == NULL)
	{
		cout << "Error! DMLua::EquipLua: Object have no P_DMItem!" << endl;
		return 0;
	}

	switch((int)dContainerType)
	{
	case 0:
		pkChar->m_pkBackPack->AddItem(pkNewObj->GetEntityID());
		break;
	case 1:
		pkChar->m_pkHand->AddItem(pkNewObj->GetEntityID());

		// equip weapon
		
		// Get owner object
		Entity* pkOwner = pkEnt; 
		pkNewObj->SetLocalPosV ( Vector3(0,0,0) );

		P_LinkToJoint* pkLink = (P_LinkToJoint*)pkNewObj->AddProperty ("P_LinkToJoint");      
		pkLink->SetJoint( "righthand" );
		
		break;
	}

	return 0;
}