#include "si_dm.h"
#include "../zerofpsv2/script/zfscript.h"
#include "p_dmhq.h"
#include "p_dmgun.h"
#include "p_dmmission.h"
#include "p_dmcharacter.h"
#include "../zerofpsv2/engine/p_pfpath.h" 

ZFScriptSystem*				DMLua::g_pkScript;
EntityManager*				DMLua::g_pkObjMan;
int							DMLua::g_iMissionStatus;
map<string, double>			DMLua::m_kVars;

// ------------------------------------------------------------------------------------------------

void DMLua::Init(EntityManager* pkObjMan,ZFScriptSystem* pkScript)
{
	g_pkObjMan = pkObjMan;
	g_pkScript = pkScript;
	g_iMissionStatus = 0; // 0 = active/inactive, 1 = success, -1 = failed
	
	pkScript->ExposeFunction("GetNumOfLivingAgents", DMLua::GetNumOfLivingAgentsLua);
	pkScript->ExposeFunction("GetDMCharacterByName", DMLua::GetDMCharacterByNameLua);
	pkScript->ExposeFunction("SetDMCharacterName", DMLua::SetDMCharacterNameLua);
	pkScript->ExposeFunction("GetDMCharacterClosest", DMLua::GetDMCharacterClosestLua);
	pkScript->ExposeFunction("SetNewMission", DMLua::SetNewMissionLua);
	pkScript->ExposeVariable("CurrentMission", (int*)&DMLua::g_iMissionStatus, tINT); 

	// character functions
	pkScript->ExposeFunction("KillCharacter", DMLua::KillCharacterLua);
	pkScript->ExposeFunction("IsDead", DMLua::IsDeadLua);
	pkScript->ExposeFunction("Heal", DMLua::HealLua);
	pkScript->ExposeFunction("BeamToLocation", DMLua::BeamToLocationLua);
	pkScript->ExposeFunction("BeamToObject", DMLua::BeamToObjectLua);
	pkScript->ExposeFunction("TestSkill", DMLua::TestSkillLua);
	pkScript->ExposeFunction("AddMoney", DMLua::AddMoneyLua);
	pkScript->ExposeFunction("Money", DMLua::MoneyLua);
	pkScript->ExposeFunction("FireAtLocation", DMLua::FireAtLocationLua);
	pkScript->ExposeFunction("FireAtObject", DMLua::FireAtCharacterLua);
	pkScript->ExposeFunction("SetMoveSpeed", DMLua::SetMoveSpeedLua);

	// path finding
	pkScript->ExposeFunction("HavePath", DMLua::HavePathLua);					
	pkScript->ExposeFunction("MakePathFind", DMLua::MakePathFindLua);
	pkScript->ExposeFunction("ClearPathFind", DMLua::ClearPathFindLua);

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
				double dDist = kObjects[i]->GetWorldPosV().DistanceTo (kFrom);
				// ignore if dead
				if ( dDist < dClosestDistance && pkCharacter->GetStats()->m_iLife > 0 )
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

		//vector<Entity*> kObjects;		
		//g_pkObjMan->GetAllObjects(&kObjects);

		////
		//// PROBLEM!
		//// 
		//for(unsigned int i=0;i<kObjects.size();i++)
		//	if(kObjects[i]->GetProperty("P_DMHQ"))
		//	{
		//		pkMissionProp = (P_DMMission*) kObjects[i]->GetProperty("P_DMMission");
		//		break;
		//	}

		Entity* pkGlobal = g_pkObjMan->GetGlobalObject();

		vector<Entity*> kObjects;
		pkGlobal->GetAllEntitys(&kObjects);

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

// takes an integer and add that much money to the players HQ
int DMLua::AddMoneyLua(lua_State* pkLua)
{
	Entity* pkHQ = GetHQEntity();

	if ( pkHQ == 0 )
	{
		cout << "DMLua::MoneyLua: Warning! No HQ object found!";
		return 0;
	}

	if ( g_pkScript->GetNumArgs(pkLua) < 1 )
	{
		cout << "DMLua::AddMoneyLua: Wrong number of args given! Function takes AddMoney (int Money)" << endl;
		return 0;
	}

	double dAddMoney;

	g_pkScript->GetArgNumber(pkLua, 0, &dAddMoney);

	int *piMoney = ((P_DMHQ*)pkHQ->GetProperty("P_DMHQ"))->GetMoney();
	*piMoney += int(dAddMoney);

	if ( *piMoney < 0 )
		*piMoney = 0;

	return 0;
}

// ------------------------------------------------------------------------------------------------

// takes nothing, returns (int) players money
int DMLua::MoneyLua(lua_State* pkLua)
{
	Entity* pkHQ = GetHQEntity();

	if ( pkHQ == 0 )
	{
		cout << "DMLua::MoneyLua: Warning! No HQ object found!";
		return 0;
	}

	g_pkScript->AddReturnValue(pkLua, double(*((P_DMHQ*)pkHQ->GetProperty("P_DMHQ"))->GetMoney()) );

	return 1;
	
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

	P_PfPath* pkPF = (P_PfPath*)pkEnt->GetProperty("P_PfPath");

	if ( pkPF == 0 )
		return 0;

	double dSpeed;

	g_pkScript->GetArgNumber(pkLua, 1, &dSpeed);

	cout << "SetSpeed:" << dSpeed << endl;

	pkPF->SetSpeed ( float (dSpeed) );

	return 0;	
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
	return 0;
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
		cout << "Warning! DMLua::GetVarLua: Coulnd't find key:" << cKey << ". Setting value to -1." << endl;
		m_kVars[cKey] = -1;
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
