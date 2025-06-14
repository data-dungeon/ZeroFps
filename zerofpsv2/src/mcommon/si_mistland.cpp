#include "si_mistland.h"
#include "p_ai.h"
#include "p_container.h"
#include "p_item.h"
#include "../zerofpsv2/engine_systems/propertys/p_scriptinterface.h"
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "../zerofpsv2/engine_systems/propertys/p_pfpath.h"
#include "../zerofpsv2/script/zfscript.h"
#include "../zerofpsv2/basic/plane.h"
#include <cmath>                    // for trigonometry functions


ZSSScriptSystem*			MistLandLua::g_pkScript;
ZSSEntityManager*			MistLandLua::g_pkObjMan;
P_ServerInfo*		  		MistLandLua::g_pkServerInfo;

int							MistLandLua::g_iLastCollidedID;
map<string,LogInInfo* >	MistLandLua::g_kServerList;
string						MistLandLua::g_kDefServer;

void MistLandLua::Init(ZSSEntityManager* pkObjMan,ZSSScriptSystem* pkScript)
{
	g_pkObjMan = pkObjMan;
	g_pkScript = pkScript;
	ObjectManagerLua::g_kScriptState.g_iCurrentObjectID = -1;
	g_iLastCollidedID = -1;
	g_pkServerInfo = NULL;
	
	pkScript->ExposeFunction("GetCurrentPCID",			MistLandLua::GetCurrentPCIDLua);		
	pkScript->ExposeFunction("GetLastCollidedObject",	MistLandLua::GetLastCollidedObjectLua);		
	pkScript->ExposeFunction("GetClosestObject",		MistLandLua::GetClosestObjectLua);		
	pkScript->ExposeFunction("MakePathFind",			MistLandLua::MakePathFindLua);					
	pkScript->ExposeFunction("HavePath",				MistLandLua::HavePathLua);					
	pkScript->ExposeFunction("RotateTowards",			MistLandLua::RotateTowardsLua);					
	
	pkScript->ExposeFunction("MessageCaracter",			MistLandLua::MessageCaracterLua);
	pkScript->ExposeFunction("StartPrivateSound",		MistLandLua::StartPrivateSoundLua);
	
   // char.stats-scipts
/*   
	pkScript->ExposeFunction("RollSkillDice",				MistLandLua::RollSkillDiceLua);			
   pkScript->ExposeFunction("RollAttributeDice",		MistLandLua::RollAttributeDiceLua);			
   pkScript->ExposeFunction("SetCurrentSkill",			MistLandLua::SetCurrentSkillLua);			
   pkScript->ExposeFunction("AddToSkillValue",			MistLandLua::AddSkillValueLua);			
   pkScript->ExposeFunction("AddToAttributeValue",		MistLandLua::AddAttributeValueLua);			
   pkScript->ExposeFunction("SetSkill", 			      MistLandLua::SetSkillValueLua);			
   pkScript->ExposeFunction("SetAttribute",		      MistLandLua::SetAttributeValueLua);			
   pkScript->ExposeFunction("SetData",			         MistLandLua::SetDataValueLua);			
   pkScript->ExposeFunction("SetAttack",  		      MistLandLua::SetAttackValueLua);			
   pkScript->ExposeFunction("SetDefence",		         MistLandLua::SetDefenceValueLua);			
   pkScript->ExposeFunction("SetScriptWhenHit",		   MistLandLua::SetScriptWhenHitLua);			

   pkScript->ExposeFunction("SetMoveSpeed",		      MistLandLua::SetMoveSpeedLua);			
   pkScript->ExposeFunction("AddMoveSpeed",		      MistLandLua::AddMoveSpeedLua);	

   pkScript->ExposeFunction("SetReloadTime",		      MistLandLua::SetReloadTimeLua);			
   pkScript->ExposeFunction("AddReloadTime",		      MistLandLua::AddReloadTimeLua);	

   // hp/mp stuff
   pkScript->ExposeFunction("SetHP",      				MistLandLua::SetHPLua);			
   pkScript->ExposeFunction("SetMP",            		MistLandLua::SetMPLua);			
   pkScript->ExposeFunction("SetMaxHP",     				MistLandLua::SetMaxHPLua);			
   pkScript->ExposeFunction("SetMaxMP",           		MistLandLua::SetMaxMPLua);			
   pkScript->ExposeFunction("HP",   			         MistLandLua::GetHPLua);			
   pkScript->ExposeFunction("MP",			            MistLandLua::GetMPLua);			
   pkScript->ExposeFunction("HPPercent",		         MistLandLua::GetHpPercentLua);			
   pkScript->ExposeFunction("MPPercent", 			      MistLandLua::GetMpPercentLua);			
   pkScript->ExposeFunction("AddHP",		            MistLandLua::AddHpLua);			
   pkScript->ExposeFunction("AddMP",			         MistLandLua::AddMpLua);			
   pkScript->ExposeFunction("PrintCharStats",			MistLandLua::PrintStatsLua);
   pkScript->ExposeFunction("SetRecalPosition",			MistLandLua::SetRecalPositionLua);   
   
   // item stuff
   pkScript->ExposeFunction("GetQuantity",     			MistLandLua::GetQuantityLua);			
   pkScript->ExposeFunction("SetQuantity",           	MistLandLua::SetQuantityLua);			
   pkScript->ExposeFunction("AddQuantity",   			MistLandLua::AddQuantityLua);			
   pkScript->ExposeFunction("SetQuality",			      MistLandLua::SetQualityLua);			
   pkScript->ExposeFunction("SetSkillBonus",		      MistLandLua::SetSkillBonusLua);			
   pkScript->ExposeFunction("SetAttributeBonus", 	   MistLandLua::SetAttributeBonusLua);			
   pkScript->ExposeFunction("SetAttackBonus",		   MistLandLua::SetAttackBonusLua);			
   pkScript->ExposeFunction("SetDefenceBonus",			MistLandLua::SetDefenceBonusLua);			
   pkScript->ExposeFunction("EquipOn",			         MistLandLua::EquipOnLua);
   pkScript->ExposeFunction("GetSkillBonus",		      MistLandLua::GetSkillBonusLua);			
   pkScript->ExposeFunction("GetAttributeBonus", 	   MistLandLua::GetAttributeBonusLua);			
   pkScript->ExposeFunction("GetAttackBonus",		   MistLandLua::GetAttackBonusLua);			
   pkScript->ExposeFunction("GetDefenceBonus",			MistLandLua::GetDefenceBonusLua);			
   pkScript->ExposeFunction("AddSkillBonus",		      MistLandLua::AddSkillBonusLua);			
   pkScript->ExposeFunction("AddAttributeBonus", 	   MistLandLua::AddAttributeBonusLua);			
   pkScript->ExposeFunction("AddAttackBonus",		   MistLandLua::AddAttackBonusLua);			
   pkScript->ExposeFunction("AddDefenceBonus",			MistLandLua::AddDefenceBonusLua);			
   pkScript->ExposeFunction("PrintItemStats",			MistLandLua::PrintItemStatsLua);			
   pkScript->ExposeFunction("SetItemName",		      MistLandLua::SetItemNameLua);			
   pkScript->ExposeFunction("SetItemValue",			   MistLandLua::SetItemValueLua);			
   pkScript->ExposeFunction("GetItemValue",			   MistLandLua::GetItemValueLua);			
   pkScript->ExposeFunction("AddItemValue",			   MistLandLua::AddItemValueLua);			
   pkScript->ExposeFunction("SetItemWeight",			   MistLandLua::SetItemWeightLua);
   pkScript->ExposeFunction("SetIcon",			         MistLandLua::SetIconLua);
   pkScript->ExposeFunction("UsesSkill",			      MistLandLua::UsesSkillLua);
	pkScript->ExposeFunction("SetEquipmentCategory",	MistLandLua::SetEquipmentCategoryLua);

   pkScript->ExposeFunction("AddBeforeItemName",		MistLandLua::AddBeforeItemNameLua);
   pkScript->ExposeFunction("AddAfterItemName",	      MistLandLua::AddAfterItemNameLua);

   pkScript->ExposeFunction("UseOn",	               MistLandLua::UseItemOnLua);
*/

//    pkScript->ExposeFunction("CastSpell",	            MistLandLua::CastSpellLua);

	// Lua Lua commands
   //pkScript->ExposeFunction("RunScript",			      MistLandLua::RunScriptLua);		

   // random function
   pkScript->ExposeFunction("Random",	      		   MistLandLua::RandomLua);			
   pkScript->ExposeFunction("GetRandomCategory",	   MistLandLua::GetRandomCategoryLua);
   pkScript->ExposeFunction("GetRandomSkill",	      MistLandLua::GetRandomSkillLua);	
   pkScript->ExposeFunction("GetRandomAttribute",	   MistLandLua::GetRandomAttributeLua);

   // setup ip
	pkScript->ExposeFunction("AddServer", MistLandLua::AddServerLua);
	pkScript->ExposeFunction("SetDefaultServer", MistLandLua::SetDefaultServerLua);

   // AI stuff
   pkScript->ExposeFunction("AIUseActionOn", MistLandLua::AIUseActionOnLua);
   pkScript->ExposeFunction("AIIdle", MistLandLua::AIIdleLua);
   pkScript->ExposeFunction("AIFaceDirection", MistLandLua::AIFaceDirectionLua);
   pkScript->ExposeFunction("AIFaceObject", MistLandLua::AIFaceObjectLua);
   pkScript->ExposeFunction("AIAttack", MistLandLua::AIAttackLua);
   pkScript->ExposeFunction("SetAIIsPlayer", MistLandLua::SetAIIsPlayerLua);
   pkScript->ExposeFunction("AIMoveTo", MistLandLua::AIMoveToLua);
// 	pkScript->ExposeFunction("SetAIState", MistLandLua::SetAIStateLua);

   pkScript->ExposeFunction("GetSeenPlayer", MistLandLua::GetSeenPlayerLua);
   pkScript->ExposeFunction("GetClosestItemOfType", MistLandLua::GetClosestItemOfTypeLua);
   pkScript->ExposeFunction("GetClosestPlayer", MistLandLua::GetClosestPlayerLua);
   pkScript->ExposeFunction("GetClosestObjectOfType", MistLandLua::GetClosestObjectOfTypeLua);

   pkScript->ExposeFunction("AIHaveTarget", MistLandLua::AIHaveTargetLua);
  


}

int MistLandLua::GetCurrentPCIDLua(lua_State* pkLua)
{
	g_pkScript->AddReturnValue(pkLua,ObjectManagerLua::g_kScriptState.g_iCurrentPCID);
	
	return 1;
}


int MistLandLua::GetLastCollidedObjectLua(lua_State* pkLua)
{
	g_pkScript->AddReturnValue(pkLua,g_iLastCollidedID);
	
	return 1;
}

int MistLandLua::GetClosestObjectLua(lua_State* pkLua)
{
	int iThisId = ObjectManagerLua::g_kScriptState.g_iCurrentObjectID;

	//if there is a argument us it as thisid	
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		iThisId = (int)dTemp;
	}

	Entity* pkME = g_pkObjMan->GetEntityByID(iThisId);

	//check that the object is valid
	if(!pkME)
		return 0;
		
	Vector3 kThisPos = pkME->GetWorldPosV();


	vector<Entity*> kObjects;
	g_pkObjMan->GetAllEntitys(&kObjects);
	
	
	
	float closest = 99999999;
	Entity* pkClosest = NULL;
	
	for(unsigned int i=0;i<kObjects.size();i++)
	{
		//dont compare to ur self or object 0
		if(kObjects[i]->GetEntityID() == iThisId || kObjects[i]->GetEntityID() == 0)
			continue;
			
		float d = (kThisPos - kObjects[i]->GetWorldPosV()).Length();
	
		if(d < closest)
		{
			closest = d;
			pkClosest = kObjects[i];
		}
	}
	
	
	if(pkClosest)
	{
		g_pkScript->AddReturnValue(pkLua,pkClosest->GetEntityID());
		return 1;
	}
		
	g_pkScript->AddReturnValue(pkLua,-1);		
	
	return 0;
}

int MistLandLua::MessageCaracterLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 2)
		return 0;
	
	if(!g_pkServerInfo)
	{	
		Entity* pkServerI = g_pkObjMan->GetEntityByType("t_serverinfo.lua");
		if(pkServerI)
			g_pkServerInfo = (P_ServerInfo*)pkServerI->GetProperty("P_ServerInfo");
		
		if(!g_pkServerInfo)
			return 0;
	}
	
	int id;
	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
	id = (int)dTemp;
	
	char	acEvent[128];
	g_pkScript->GetArgString(pkLua, 1, acEvent);
		
	//send message
	g_pkServerInfo->MessageCharacter(id,acEvent);
	
	return 0;
}

// Arg 1. ID p� spelar objektet som lyssnar
// Arg 2. ID p� objektet som genererar ljudet.
// Arg 3. Namn p� ljudet som skall spelas.

int MistLandLua::StartPrivateSoundLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 3)
		return 0;

	if(!g_pkServerInfo)
	{	
		Entity* pkServerI = g_pkObjMan->GetEntityByType("t_serverinfo.lua");
		if(pkServerI)
			g_pkServerInfo = (P_ServerInfo*)pkServerI->GetProperty("P_ServerInfo");
		
		if(!g_pkServerInfo)
			return 0;
	}	

	int id_player, id_object_generating_sound;
	char acFileName[64];
	double dTemp;

	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
	id_player = (int)dTemp;

	g_pkScript->GetArgNumber(pkLua, 1, &dTemp);
	id_object_generating_sound = (int)dTemp;
	
	g_pkScript->GetArgString(pkLua, 2, acFileName);
		
	//send message
	g_pkServerInfo->AddPrivateSoundToPlayer(id_player,id_object_generating_sound,acFileName);
	
	return 0;
}

int MistLandLua::MakePathFindLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 2)
	{
		double dId;
		Vector3 kPos;		
		vector<TABLE_DATA> vkData;
		
		g_pkScript->GetArgNumber(pkLua, 0, &dId);				
		g_pkScript->GetArgTable(pkLua, 2, vkData);
/*		g_pkScript->GetArgNumber(pkLua, 1, &x);		
		g_pkScript->GetArgNumber(pkLua, 2, &y);		
		g_pkScript->GetArgNumber(pkLua, 3, &z);		
*/			
		kPos = Vector3(
			(float) (*(double*) vkData[0].pData),
			(float) (*(double*) vkData[1].pData),
			(float) (*(double*) vkData[2].pData)); 
		
		Entity* pkEnt = g_pkObjMan->GetEntityByID((int)dId);
		if(pkEnt)
		{
			P_PfPath* pf = (P_PfPath*)pkEnt->GetProperty("P_PfPath");
			if(pf)
				pf->MakePathFind(kPos);
		}
	}
	return 0;
}

int MistLandLua::HavePathLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dId;		
		g_pkScript->GetArgNumber(pkLua, 0, &dId);				
		
		Entity* pkEnt = g_pkObjMan->GetEntityByID((int)dId);
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

int MistLandLua::RotateTowardsLua(lua_State* pkLua)
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


		Entity* pkEnt = g_pkObjMan->GetEntityByID((int)dId);
			
		if(pkEnt)
		{
			Matrix4 kRot;
			kRot.LookDir(kPos - pkEnt->GetWorldPosV(),Vector3(0,1,0));			
			kRot.Transponse();
			pkEnt->SetLocalRotM(kRot);
		}
		return 0;
		
	}
	return 0;
}




// ----------------------------------------------------------------------------------------------
/*
int MistLandLua::RollSkillDiceLua (lua_State* pkLua)
{
    // error
	if(g_pkScript->GetNumArgs(pkLua) != 2)
      return 0;
   else
	{

		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

		if(pkObject)
		{
         char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

         double dTemp;
     		g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iDiffuculty = (int)dTemp;

			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         if ( pkCS )
         {
            double dRoll = pkCS->RollSkillDice ( (string)acType, (float) iDiffuculty );
            g_pkScript->AddReturnValue(pkLua, dRoll);
            return 1;
         }
         else
         {
            cout << "Error! Scipt tried to use a charstat-function on a non-charstat object!" << endl;
            return 0;
         }

         
		}
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::RollAttributeDiceLua (lua_State* pkLua)
{
   // error
	if(g_pkScript->GetNumArgs(pkLua) != 2)
      return 0;
   else
	{
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

		if(pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);
         
         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iDiffuculty = (int)dTemp;

			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         g_pkScript->AddReturnValue(pkLua, pkCS->RollAttributeDice ( (string)acType, (float) iDiffuculty ) );

         return 1;
		}
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetCurrentSkillLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);
         
			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         pkCS->SetCurrentSkill ( (string)acType );
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddSkillValueLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iValue = (int)dTemp;
         
 			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         pkCS->AddSkillValue ( (string)acType, iValue );
      }
   }
   
   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddAttributeValueLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);
         
         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iValue = (int)dTemp;
         
  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         pkCS->AddAttributeValue ( (string)acType, iValue );
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetSkillValueLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iValue = (int)dTemp;
         
 			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");

         if ( pkCP )
            pkCP->GetCharStats()->SetSkill ( (string)acType, iValue );
         else
            cout << "Error! Tried to use luaFunc SetSkillValue on a object without P_CharStats" << endl;
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetAttributeValueLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iValue = (int)dTemp;
         
         CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");

         if ( pkCP )
            pkCP->GetCharStats()->SetAttribute ( (string)acType, iValue );
         else
            cout << "Error! Tried to use luaFunc SetAttributeValue on a object withour P_CharStats!!!" << endl;
			
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------
int MistLandLua::SetDataValueLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

     		char	acType2[128];
			g_pkScript->GetArgString(pkLua, 1, acType2);
         
  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         
         if ( pkCP )
            pkCP->GetCharStats()->SetData ( (string)acType, (string)acType2 );
         else
            cout << "Error! Tried to use luaFunc SetDataValue on a entity without P_CharStats" << endl;
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetDefenceValueLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iValue = (int)dTemp;
         
  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");

         if ( pkCP )
         {
            CharacterStats *pkCS = pkCP->GetCharStats();
            pkCS->SetDefenceValue ( (string)acType, iValue );
         }
         else
            cout << "ERROR! Tried to use Luafunc: SetDefenceValue och object without P_CharStats" << endl;
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetAttackValueLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iValue = (int)dTemp;
         
  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         
         if ( pkCP )
            pkCP->GetCharStats()->SetAttackValue ( (string)acType, iValue );
         else
            cout << "Error! Tried to use luaFunc SetAttackValue on a entity without P_CharStats" << endl;
      }
   }

   return 0;
}
// ----------------------------------------------------------------------------------------------

int MistLandLua::SetHPLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");

         if ( pkCP )
            pkCP->GetCharStats()->SetHP ( string(acType) );
         else
            cout << "Error! Tried to set HP on a object without P_CharStats" << endl;
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetMPLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//      		char	acType[128];
// 			g_pkScript->GetArgString(pkLua, 0, acType);
// 
//   			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
// 
//          if ( pkCP )
//          {
//             CharacterStats *pkCS = pkCP->GetCharStats();
//             pkCS->SetMP ( string(acType) );
//          }
//       }
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetMaxHPLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//      		double dTemp;
//          g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
// 
//   			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
//          
//          if ( pkCP )
//             pkCP->GetCharStats()->SetMaxHP( int(dTemp) );
//          else
//             cout << "Error! Tried to use luaFunc SetMaxHP on a entity without P_CharStats" << endl;
//       }
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetMaxMPLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//      		double dTemp;
//          g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
// 
//   			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
//          CharacterStats *pkCS = pkCP->GetCharStats();
// 
//          pkCS->SetMaxMP( int(dTemp) );
//       }
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::GetHPLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 0 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//   			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
//          CharacterStats *pkCS = pkCP->GetCharStats();
// 
//          g_pkScript->AddReturnValue(pkLua, pkCS->GetHP () );
//       }
// 
//       return 1;
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::GetMPLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 0 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
// 
//   			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
//          CharacterStats *pkCS = pkCP->GetCharStats();
// 
//          g_pkScript->AddReturnValue(pkLua, pkCS->GetMP () );
//       }
// 
//       return 1;
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::GetHpPercentLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 0 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
// 
//   			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
//          CharacterStats *pkCS = pkCP->GetCharStats();
// 
//          g_pkScript->AddReturnValue(pkLua, pkCS->GetHPPercent () );
//       }
// 
//       return 1;
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::GetMpPercentLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 0 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
// 
//   			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
//          CharacterStats *pkCS = pkCP->GetCharStats();
// 
//          g_pkScript->AddReturnValue(pkLua, pkCS->GetMPPercent () );
//       }
// 
//       return 1;
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddHpLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//      		double dTemp;
//          g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
// 
//   			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
//          CharacterStats *pkCS = pkCP->GetCharStats();
// 
//          pkCS->AddHP ( (int) dTemp );
//       }
// 
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddMpLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//      		double dTemp;
//          g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
// 
//   			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
//          CharacterStats *pkCS = pkCP->GetCharStats();
// 
//          pkCS->AddMP ( (int) dTemp );
//       }
// 
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

// which script to run when character is hit
int MistLandLua::SetScriptWhenHitLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
// 		
//      		char	acScript[128];
// 			g_pkScript->GetArgString(pkLua, 0, acScript);
// 
//   			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
// 			
// 
//          if ( pkCP )
//          {
//             CharacterStats* pkc = pkCP->GetCharStats();
//             if(pkc)
//             	pkc->m_strScriptWhenHit = acScript;
//          }
//          else
//             cout << "Warning! Tried to use SetScriptWhenHitLua on a non-character object!" << endl; 
//       }
// 
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetReloadTimeLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//   			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
// 
//          if ( pkCP )
//          {
//      		   double dSpeed;
//             g_pkScript->GetArgNumber(pkLua, 0, &dSpeed);
// 
//             pkCP->GetCharStats()->SetReloadTime ( (float) dSpeed);
//          }
//          else
//             cout << "Warning! Tried to use SetReloadTime on a non-character object!" << endl; 
//       }
// 
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddReloadTimeLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");

         if ( pkCP )
         {
     		   double dSpeed;
            g_pkScript->GetArgNumber(pkLua, 0, &dSpeed);

            pkCP->GetCharStats()->AddReloadTime ( (float) dSpeed);
         }
         else
            cout << "Warning! Tried to use SetReloadTime on a non-character object!" << endl; 
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetMoveSpeedLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");

         if ( pkCP )
         {
     		   double dSpeed;
            g_pkScript->GetArgNumber(pkLua, 0, &dSpeed);

            pkCP->GetCharStats()->SetMoveSpeed ((float) dSpeed);
         }
         else
            cout << "Warning! Tried to use SetReloadTime on a non-character object!" << endl; 
      }

   }

   return 0;

}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddMoveSpeedLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");

         if ( pkCP )
         {
     		   double dSpeed;
            g_pkScript->GetArgNumber(pkLua, 0, &dSpeed);

            pkCP->GetCharStats()->AddMoveSpeed ((float) dSpeed);
         }
         else
            cout << "Warning! Tried to use SetReloadTime on a non-character object!" << endl; 
      }

   }

   return 0;

}

// ----------------------------------------------------------------------------------------------

int MistLandLua::PrintStatsLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 0 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         pkCS->Print();
      }

   }

   return 0;

}

// ----------------------------------------------------------------------------------------------


int MistLandLua::SetRecalPositionLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		double dID;
		g_pkScript->GetArgNumber(pkLua, 0, &dID);		

		Entity* pkObject = g_pkObjMan->GetEntityByID((int)dID);

		if(pkObject)
		{
			vector<TABLE_DATA> vkData;
			g_pkScript->GetArgTable(pkLua, 2, vkData); // f�rsta argumetet startar p� 1

			Vector3 kPos = Vector3(
				(float) (*(double*) vkData[0].pData),
				(float) (*(double*) vkData[1].pData),
				(float) (*(double*) vkData[2].pData));
	
			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
	      if(pkCP)
   	   {
	   	   CharacterStats *pkCS = pkCP->GetCharStats();
		
				pkCS->SetRecalPos(kPos);		
				cout<<"Setting recal position!!"<<endl;
			}	
		
			g_pkScript->DeleteTable(vkData);
		}

		return 1;
   }
   return 0;
}


// ----------------------------------------------------------------------------------------------

int MistLandLua::GetQuantityLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 0 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
// 		   P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//          {      
//             ItemStats *pkIS = pkCP->m_pkItemStats;
//             g_pkScript->AddReturnValue(pkLua, pkIS->GetQuantity() );
//          }
//          else
//          {
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//             return 0;
//          }
//           
//          return 1;
//       }
//    }
// *
   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetQuantityLua (lua_State* pkLua)
{
/*	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
     		double dTemp;
         g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
                  
         if ( pkCP )
            pkCP->m_pkItemStats->SetQuantity ( int(dTemp) );
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;

      }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddQuantityLua (lua_State* pkLua)
{
/*
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
     		double dTemp;
         g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
            pkCP->m_pkItemStats->AddQuantity ( int(dTemp) );
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
      }

   }
*
   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetQualityLua (lua_State* pkLua)
{
/*
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
     		double dTemp;
         g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
            pkCP->m_pkItemStats->SetQuality ( (float) dTemp );
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
      }

   }
*
   return 0;
}


// ----------------------------------------------------------------------------------------------

int MistLandLua::SetIconLua (lua_State* pkLua)
{
/*
	if( g_pkScript->GetNumArgs(pkLua) == 2 ) // �ndra till 1 och �ndra alla skriptfiler...
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acPic[75];
			g_pkScript->GetArgString(pkLua, 0, acPic);

  			P_Item* pkIP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkIP )
            pkIP->m_pkItemStats->SetIcon ( acPic );
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
 
      }

   }
*
   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetEquipmentCategoryLua (lua_State* pkLua)
{
/*
	if( g_pkScript->GetNumArgs(pkLua) == 1 ) 
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acCategory[75];
			g_pkScript->GetArgString(pkLua, 0, acCategory);

  			P_Item* pkIP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkIP )
			{
				string strCategory = acCategory;
				EquipmentCategory eCategory = Item;

				if(strCategory == "Armor") eCategory = Armour;
				else if(strCategory == "Cloak") eCategory = Cloak;
				else if(strCategory == "Helm") eCategory = Helm;
				else if(strCategory == "Amulett") eCategory = Amulett;
				else if(strCategory == "Shield") eCategory = Shield;
				else if(strCategory == "Weapon") eCategory = Weapon;
				else if(strCategory == "Bracer") eCategory = Bracer;
				else if(strCategory == "Glove") eCategory = Glove;
				else if(strCategory == "Ring") eCategory = Ring;
				else if(strCategory == "Boots") eCategory = Boots;
				else if(strCategory == "Belt") eCategory = Belt;

				pkIP->m_pkItemStats->SetEquipmentCategory( eCategory );

			}
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
 
      }

   }
*
   return 0;
}


// ----------------------------------------------------------------------------------------------

int MistLandLua::UseItemOnLua (lua_State* pkLua)
{
/*
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
         double dCharacterID;
         g_pkScript->GetArgNumber(pkLua, 0, &dCharacterID);

         Entity* pkChar = g_pkObjMan->GetEntityByID( (int) dCharacterID);

  			P_Item* pkIP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkIP )
            pkIP->UseOn ( pkChar );
         else
            cout << "Warning! Tried to use a function UseItemOnLua on a non-item object or non character!" << endl;
 
      }

   }
*
   return 0;

}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetSkillBonusLua (lua_State* pkLua)
{
/*
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iValue = (int)dTemp;

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
            pkCP->m_pkItemStats->SetSkillBonus ( string(acType), iValue );
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
 
      }

   }
*
   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetAttributeBonusLua (lua_State* pkLua)
{
/*
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iValue = (int)dTemp;

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
            pkCP->m_pkItemStats->SetAttributeBonus ( string(acType), iValue );
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
 
      }

   }
*
   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetAttackBonusLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 2 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//      		char	acType[128];
// 			g_pkScript->GetArgString(pkLua, 0, acType);
// 
//          double dTemp;
//          g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
//    		int iValue = (int)dTemp;
// 
//   			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//             pkCP->m_pkItemStats->SetAttackBonus ( string(acType), iValue );
//          else
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//  
//       }
// 
//    }

   return 0;
}

//----------------------------------------------------------------------------------------------

int MistLandLua::SetDefenceBonusLua (lua_State* pkLua)
{

// 	if( g_pkScript->GetNumArgs(pkLua) == 2 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//      		char	acType[128];
// 			g_pkScript->GetArgString(pkLua, 0, acType);
// 
//          double dTemp;
//          g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
//    		int iValue = (int)dTemp;
// 
//   			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//             pkCP->m_pkItemStats->SetDefenceBonus ( string(acType), iValue );
//          else
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//  
//       }
// 
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::EquipOnLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//      		char	acType[128];
// 			g_pkScript->GetArgString(pkLua, 0, acType);
// 
//  			P_Item* pkIP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkIP )
//             pkIP->m_pkItemStats->AddCanEquipOn( string(acType) );
//          else
//             cout << "Warning! Tried to use function EquipOn on a non-item object!" << endl;
//       }
// 
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::GetSkillBonusLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//          char	acType[128];
// 			g_pkScript->GetArgString(pkLua, 0, acType);
// 
// 		   P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//          {      
//             ItemStats *pkIS = pkCP->m_pkItemStats;
//             g_pkScript->AddReturnValue(pkLua, pkIS->GetSkillBonus ( string (acType) ) );
//          }
//          else
//          {
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//             return 0;
//          }
//           
//          return 1;
//       }
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::GetAttributeBonusLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//          char	acType[128];
// 			g_pkScript->GetArgString(pkLua, 0, acType);
// 
// 		   P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//          {      
//             ItemStats *pkIS = pkCP->m_pkItemStats;
//             g_pkScript->AddReturnValue(pkLua, pkIS->GetAttributeBonus ( string (acType) ) );
//          }
//          else
//          {
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//             return 0;
//          }
//           
//          return 1;
//       }
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------
int MistLandLua::GetAttackBonusLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//          char	acType[128];
// 			g_pkScript->GetArgString(pkLua, 0, acType);
// 
// 		   P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//          {      
//             ItemStats *pkIS = pkCP->m_pkItemStats;
//             g_pkScript->AddReturnValue(pkLua, pkIS->GetAttackBonus ( string (acType) ) );
//          }
//          else
//          {
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//             return 0;
//          }
//           
//          return 1;
//       }
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------
int MistLandLua::GetDefenceBonusLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//          char	acType[128];
// 			g_pkScript->GetArgString(pkLua, 0, acType);
// 
// 		   P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//          {      
//             ItemStats *pkIS = pkCP->m_pkItemStats;
//             g_pkScript->AddReturnValue(pkLua, pkIS->GetDefenceBonus ( string (acType) ) );
//          }
//          else
//          {
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//             return 0;
//          }
//           
//          return 1;
//       }
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddSkillBonusLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 2 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//      		char	acType[128];
// 			g_pkScript->GetArgString(pkLua, 0, acType);
// 
//          double dTemp;
//          g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
//    		int iValue = (int)dTemp;
// 
//   			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//          {      
//             ItemStats *pkIS = pkCP->m_pkItemStats;
//             pkIS->AddToSkillBonus ( string(acType), iValue );
//          }
//          else
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//  
//       }
// 
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddAttributeBonusLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 2 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//      		char	acType[128];
// 			g_pkScript->GetArgString(pkLua, 0, acType);
// 
//          double dTemp;
//          g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
//    		int iValue = (int)dTemp;
// 
//   			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//          {      
//             ItemStats *pkIS = pkCP->m_pkItemStats;
//             pkIS->AddToAttributeBonus ( string(acType), iValue );
//          }
//          else
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//        }
// 
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddAttackBonusLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 2 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//      		char	acType[128];
// 			g_pkScript->GetArgString(pkLua, 0, acType);
// 
//          double dTemp;
//          g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
//    		int iValue = (int)dTemp;
// 
//   			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//          {      
//             ItemStats *pkIS = pkCP->m_pkItemStats;
//             pkIS->AddToAttackBonus ( string(acType), iValue );
//          }
//          else
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//  
//       }
// 
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddDefenceBonusLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 2 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//      		char	acType[128];
// 			g_pkScript->GetArgString(pkLua, 0, acType);
// 
//          double dTemp;
//          g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
//    		int iValue = (int)dTemp;
// 
//   			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//          {      
//             ItemStats *pkIS = pkCP->m_pkItemStats;
//             pkIS->AddToDefenceBonus ( string(acType), iValue );
//          }
//          else
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//  
//       }
// 
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::PrintItemStatsLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 0 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
// 
//   			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//          {      
//             ItemStats *pkIS = pkCP->m_pkItemStats;
//             pkIS->Print();
//          }
//          else
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//  
//       }
// 
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetItemNameLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//      		char	acType[128];
// 			g_pkScript->GetArgString(pkLua, 0, acType);
// 
//   			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//          {      
//             ItemStats *pkIS = pkCP->m_pkItemStats;
//             pkIS->SetItemName ( string (acType) );
//          }
//          else
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//  
//       }
// 
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::UsesSkillLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//      		char	acType[128];
// 			g_pkScript->GetArgString(pkLua, 0, acType);
// 
//   			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//             pkCP->m_pkItemStats->m_kUsesSkill = string (acType);
//          else
//            cout << "Warning! Tried to use luaFunc UsesSkillLua on a non-item object!" << endl;
//        }
// 
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetItemWeightLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//          double dTemp;
//          g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
// 
//   			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//          {      
//             ItemStats *pkIS = pkCP->m_pkItemStats;
//             pkIS->SetItemWeight ( (float) dTemp );
//          }
//          else
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//  
//       }
// 
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetItemValueLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//          double dTemp;
//          g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
// 
//   			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//          {      
//             ItemStats *pkIS = pkCP->m_pkItemStats;
//             pkIS->SetItemValue ( int(dTemp) );
//          }
//          else
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//  
//       }
// 
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::GetItemValueLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 0 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//   			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//          {      
//             ItemStats *pkIS = pkCP->m_pkItemStats;
//             g_pkScript->AddReturnValue(pkLua, pkIS->GetItemValue () );
//          }
//          else
//          {
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//             return 0;
//          }
// 
//          return 1;
//  
//       }
// 
//    }

   return 0;

}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddItemValueLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//          double dTemp;
//          g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
// 
//   			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//          {      
//             ItemStats *pkIS = pkCP->m_pkItemStats;
//             pkIS->AddItemValue ( int(dTemp) );
//          }
//          else
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//  
//       }
// 
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddBeforeItemNameLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//      		char	acType[128];
// 			g_pkScript->GetArgString(pkLua, 0, acType);	
// 
//   			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//             pkCP->m_pkItemStats->AddBeforeName ( string(acType ) );
//          else
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//  
//       }
// 
//    }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddAfterItemNameLua (lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//      		char	acType[128];
// 			g_pkScript->GetArgString(pkLua, 0, acType);	
// 
//   			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");
// 
//          if ( pkCP )
//             pkCP->m_pkItemStats->AddAfterName ( string(acType ) );
//          else
//             cout << "Warning! Tried to use a item function on a non-item object!" << endl;
//  
//       }
// 
//    }

   return 0;
}*/


int MistLandLua::RandomLua (lua_State* pkLua)
{
 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
      double dTemp;
      g_pkScript->GetArgNumber(pkLua, 0, &dTemp);

      if ( dTemp < 0 )
         dTemp = 1;


      g_pkScript->AddReturnValue(pkLua, rand()%int(dTemp) );

      return 1;
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::GetRandomCategoryLua (lua_State* pkLua)
{
   int iRand = rand()%6;

   string kType;

   switch (iRand)
   {
      case 0:
         kType = "Fire";
         break;
      case 1:
         kType = "Ice";
         break;
      case 2:
         kType = "Lightning";
         break;
      case 3:
         kType = "Poison";
         break;
      case 4:
         kType = "Earth";
         break;
      case 5:
         kType = "Water";
         break;
   }

   g_pkScript->AddReturnValue(pkLua, (char*)kType.c_str(), kType.size() );

   return 1;
}


// ----------------------------------------------------------------------------------------------

int MistLandLua::GetRandomSkillLua (lua_State* pkLua)
{
// 	// if stat-types isn't loaded
// 	if ( !g_kSkills.size() && !g_kAttributes.size() && !g_kData.size() )
//       LoadStatTypes();
//    
//    int iRand = rand()%g_kSkills.size();
//    g_pkScript->AddReturnValue(pkLua, (char*)g_kSkills[iRand].c_str(), g_kSkills[iRand].size() );

   return 1;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::GetRandomAttributeLua (lua_State* pkLua)
{
// 	// if stat-types isn't loaded
// 	if ( !g_kSkills.size() && !g_kAttributes.size() && !g_kData.size() )
//       LoadStatTypes();
// 
//    int iRand = rand()%g_kAttributes.size();
//    g_pkScript->AddReturnValue(pkLua, (char*)g_kAttributes[iRand].c_str(), g_kAttributes[iRand].size() );

   return 1;
}

// ----------------------------------------------------------------------------------------------
/*
int MistLandLua::RunScriptLua (lua_State* pkLua)
{
 	if( g_pkScript->GetNumArgs(pkLua) == 2 || g_pkScript->GetNumArgs(pkLua) == 1)
   {
     	char	acType[128];
		g_pkScript->GetArgString(pkLua, 0, acType);

      double temp;
		g_pkScript->GetArgNumber(pkLua, 1, &temp);
 		int objectid = (int) temp;


      ZFScriptSystem* pkZFScriptSys = g_pkScript;
	   
	   Entity* object = g_pkObjMan->GetEntityByID(objectid);
	   if(!object)
	   {
		   cout<<"parend object does not exist"<<endl;
		   return 0;
	   }
   
      // create the new object
      Entity* pkNewObj = g_pkObjMan->CreateEntityFromScriptInZone(acType, 
                         object->GetWorldPosV() );

      // return everything the way it was
      g_pkScript = pkZFScriptSys;

      // return the new object�s ID      
      g_pkScript->AddReturnValue( pkLua, pkNewObj->GetEntityID() );

      return 1; // return newcreated-object_ID
      
   } 
   return 0;  
}*/

// ----------------------------------------------------------------------------------------------
// /*
// // spell
// int MistLandLua::CastSpellLua (lua_State* pkLua)
// {
// 	if( g_pkScript->GetNumArgs(pkLua) == 3 )
//    {
// 		Entity* pkObject = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
// 	   if (pkObject)
// 		{
//          //spell
//      		char acValue[128];
// 			g_pkScript->GetArgString(pkLua, 0, acValue);
// 
//          // caster
//          double dCaster;
//          g_pkScript->GetArgNumber(pkLua, 1, &dCaster);
// 
//          // target
//          double dTarget;
//          g_pkScript->GetArgNumber(pkLua, 2, &dTarget);
// 
// 			Entity* me = g_pkObjMan->GetEntityByID((int) dCaster);
// 			if(!me)
// 				return 0;
// 
// 			//cout<<"pos:"<<me.x<<endl;
// 
//          Entity* pkSpell = 
//                  g_pkObjMan->CreateEntityFromScriptInZone(acValue,me->GetWorldPosV());
// 
//          P_Spell* pkSpellProp = (P_Spell*)pkSpell->GetProperty("P_Spell");
//          pkSpellProp->SetCaster ( (int) dCaster );
// 
//          g_pkScript->AddReturnValue( pkLua, pkSpell->GetEntityID() );
//                 
//          return 1;
//        }
//    }
// 
//    return 0;
// }*

// ----------------------------------------------------------------------------------------------

// 1:st arg = server name (string)
// 2:nd arg = ip name (ie."192.168.0.153:4242") (string)
int MistLandLua::AddServerLua(lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) != 4 )
	{
		printf("Failed to add user to list. Bad argumetns!\n");
		return 0;
	}

   char acName[128], acIP[128], acUserName[128], acPassword[128];
	
	//user name
	g_pkScript->GetArgString(pkLua, 0, acName);

   //user ip
	g_pkScript->GetArgString(pkLua, 1, acIP);

   //user ip
	g_pkScript->GetArgString(pkLua, 2, acUserName);

	//user ip
	g_pkScript->GetArgString(pkLua, 3, acPassword);

	LogInInfo* pkLogInfo = new LogInInfo;
	pkLogInfo->acIP = acIP;
	pkLogInfo->acUserName = acUserName;
	pkLogInfo->acPassword = acPassword;

	map<string,LogInInfo* >::iterator it = g_kServerList.find(string(acName));

	if(it != g_kServerList.end())
	{
		printf("Failed to add user to list. User already exist!\n");
		return 0;
	}

	g_kServerList.insert(map<string,LogInInfo*>::value_type(string(acName), pkLogInfo));

	return 1;
}

// -----------------------------------------------------------------------------------------------

// 1:st arg = server name (string)
int MistLandLua::SetDefaultServerLua(lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) != 1 )
	{
		printf("Failed to add user to list. Bad argumetns!\n");
		return 0;
	}

   char acName[128];
	
	//user name
	g_pkScript->GetArgString(pkLua, 0, acName);

	g_kDefServer = string(acName);

	return 1;
}

// -----------------------------------------------------------------------------------------------



// 1=event 2=target 3=user
int MistLandLua::AIUseActionOnLua(lua_State* pkLua) 
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 2 || g_pkScript->GetNumArgs(pkLua) == 3 )
// 	{
//       // action
//      	char acAction[128];
// 		g_pkScript->GetArgString(pkLua, 0, acAction);
// 
//       double dTarget;
//       g_pkScript->GetArgNumber(pkLua, 1, &dTarget);
// 
//       double dUser;
// 
//       if ( g_pkScript->GetNumArgs(pkLua) == 3 )
//          g_pkScript->GetArgNumber(pkLua, 2, &dUser);
//       else
//          dUser = ObjectManagerLua::g_kScriptState.g_iCurrentObjectID;
// 
//       Entity* pkObj = g_pkObjMan->GetEntityByID((int)dUser);
// 
//       if ( !pkObj )
//          return 0;
// 
//       Entity* pkTarget = g_pkObjMan->GetEntityByID((int)dTarget);
// 
//       if ( !pkTarget )
//          return 0;
// 
//       
//       P_AI* pkAI = (P_AI*)pkObj->GetProperty ("P_AI");
// 
//       // check if user is a Character with AI and target an Item
//       if ( pkObj->GetProperty ("P_CharStats") && pkAI )
//       {
//          // is AI is player, clear all other orders
//          if ( pkAI->PlayerAI() )
//             pkAI->ClearDynamicOrders();
// 
//          pkAI->AddDynamicOrder ("Action", (int) dTarget, 0, pkTarget->GetWorldPosV(), acAction );
//       }
// 
// 
//    }

   return 0;
}

// -----------------------------------------------------------------------------------------------

// 1 = target, 2 = user
int MistLandLua::AIAttackLua(lua_State* pkLua) 
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 || g_pkScript->GetNumArgs(pkLua) == 2 )
// 	{
//       double dTarget, dAttacker;
// 
//       g_pkScript->GetArgNumber(pkLua, 0, &dTarget);
// 
//       if ( g_pkScript->GetNumArgs(pkLua) == 2 )
//          g_pkScript->GetArgNumber(pkLua, 1, &dAttacker);
//       else
//          dAttacker = ObjectManagerLua::g_kScriptState.g_iCurrentObjectID;
// 
//       Entity* pkObj = g_pkObjMan->GetEntityByID((int)dAttacker);
// 
//       if ( !pkObj )
//          return 0;
// 
//       Entity* pkTarget = g_pkObjMan->GetEntityByID((int)dTarget);
// 
//       if ( !pkTarget )
//          return 0;
// 
//       P_AI* pkAI = (P_AI*)pkObj->GetProperty ("P_AI");
// 
//       // check if user is a Character with AI and target an character
//       if ( pkObj->GetProperty ("P_CharStats") && pkAI && pkTarget->GetProperty("P_CharStats") )
//       {
//          // is AI is player, clear all other orders
//          if ( pkAI->PlayerAI() )
//             pkAI->ClearDynamicOrders();
// 
//          pkAI->AddDynamicOrder ("Attack", (int) dTarget, 0, pkTarget->GetWorldPosV(), "tjoff" );
//       }
//    }

   return 0;
}

// -----------------------------------------------------------------------------------------------

int MistLandLua::AIIdleLua(lua_State* pkLua) 
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 2 )
// 	{
// 		double dId, dWaitTime;
//       
// 
// 		g_pkScript->GetArgNumber(pkLua, 0, &dId);
// 		g_pkScript->GetArgNumber(pkLua, 1, &dWaitTime);
// 		
// 		
// 		Entity* pkEnt = g_pkObjMan->GetEntityByID((int)dId);
// 
//       if(pkEnt)
// 		{
//          // check if object has AI
//          P_AI* pkAI = (P_AI*)pkEnt->GetProperty("P_AI");
// 
//          // is AI is player, clear all other orders
//          if ( pkAI->PlayerAI() )
//             pkAI->ClearDynamicOrders();
//          else if ( pkAI->PlayerAI() )
//          {
//             cout << "Warning! Tried to add static order to player" << endl;
//             return 0;
//          }
// 
//          if ( pkAI )
//             pkAI->AddStaticOrder ("Idle", (int) (dWaitTime*10000), (int) (dWaitTime*10000), Vector3::ZERO, ".");
// 		}
//       else
//          cout << "Tried to us AIMoveTo on a object without P_AI" << endl;
// 	}
	return 0;
}

// -----------------------------------------------------------------------------------------------
int MistLandLua::AIFaceDirectionLua(lua_State* pkLua) 
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 2 )
// 	{
// 		double dId, dDirection;      
// 
// 		g_pkScript->GetArgNumber(pkLua, 0, &dId);
// 		g_pkScript->GetArgNumber(pkLua, 1, &dDirection);
// 		
// 		Entity* pkObj = g_pkObjMan->GetEntityByID((int)dId);
// 
//       if(pkObj)
// 		{
//          // check if object has AI
//          P_AI* pkAI = (P_AI*)pkObj->GetProperty("P_AI");
// 
//          if ( pkAI )
//          {
//             Vector3 kDir;
//             kDir.x = (float) cos ( dDirection / degtorad );
//             kDir.y = 0;
//             kDir.z = (float) sin ( dDirection / degtorad );
//             pkAI->AddStaticOrder ("Face", 0, 0, kDir, ".");
//          }
//          else
//             cout << "Error! Tried to add AIOrder AIFaceObjectLua on a object without P_AI" << endl;
// 		}
// 
// 	}
	return 0;
}
// -----------------------------------------------------------------------------------------------

int MistLandLua::AIFaceObjectLua(lua_State* pkLua) 
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 2 )
// 	{
// 		double dId, dObjectFaceID;      
// 
// 		g_pkScript->GetArgNumber(pkLua, 0, &dId);
// 		g_pkScript->GetArgNumber(pkLua, 1, &dObjectFaceID);
// 		
// 		Entity* pkObj = g_pkObjMan->GetEntityByID((int)dId);
//       Entity* pkFaceObj = g_pkObjMan->GetEntityByID((int)dObjectFaceID);
// 
//       if(pkObj && pkFaceObj)
// 		{
//          // check if object has AI
//          P_AI* pkAI = (P_AI*)pkObj->GetProperty("P_AI");
// 
//          if ( pkAI )
//             pkAI->AddDynamicOrder ("Face", (int)dObjectFaceID, 0, Vector3::ZERO, ".");
//          else
//             cout << "Error! Tried to add AIOrder AIFaceObjectLua on a object without P_AI" << endl;
// 		}
// 
// 	}
	return 0;
}

// -----------------------------------------------------------------------------------------------

int MistLandLua::AIMoveToLua(lua_State* pkLua) 
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 2 || g_pkScript->GetNumArgs(pkLua) == 3)
// 	{
// 		double dId;
//       
//       char temp[128] = "dynamic";
// 
// 		Vector3 kPos;		
// 		vector<TABLE_DATA> vkData;
// 		
// 		g_pkScript->GetArgNumber(pkLua, 0, &dId);				
// 		g_pkScript->GetArgTable(pkLua, 2, vkData);
// 		
//       if ( g_pkScript->GetNumArgs(pkLua) == 3 )
//          g_pkScript->GetArgString(pkLua, 3, temp);
// 
// /*		g_pkScript->GetArgNumber(pkLua, 1, &x);		
// 		g_pkScript->GetArgNumber(pkLua, 2, &y);		
// 		g_pkScript->GetArgNumber(pkLua, 3, &z);		
// */			
// 		kPos = Vector3(
// 			(float) (*(double*) vkData[0].pData),
// 			(float) (*(double*) vkData[1].pData),
// 			(float) (*(double*) vkData[2].pData)); 
// 		
// 		Entity* pkEnt = g_pkObjMan->GetEntityByID((int)dId);
// 		if(pkEnt)
// 		{
//          // check if object has AI
//          P_AI* pkAI = (P_AI*)pkEnt->GetProperty("P_AI");
// 
//          if ( pkAI )
//          {
//             // is AI is player, clear all other orders
//             if ( pkAI->PlayerAI() && strcmp(temp,"dynamic") == 0)
//                pkAI->ClearDynamicOrders();
//             else if ( pkAI->PlayerAI() && strcmp(temp,"static") == 0)
//             {
//                cout << "Warning! Tried to add static order to player" << endl;
//                return 0;
//             }
// 
//                if ( strcmp(temp,"dynamic") == 0 )
//                   pkAI->AddDynamicOrder ("MoveTo", 0, 0, kPos, ".");
//                else
//                   pkAI->AddStaticOrder ("MoveTo", 0, 0, kPos, ".");
//          }
// 		}
//       else
//          cout << "Tried to us AIMoveTo on a object without P_AI" << endl;
// 	}
	return 0;
}

// -----------------------------------------------------------------------------------------------

int MistLandLua::AISetCommander(lua_State* pkLua) 
{
   return 0;
}

// -----------------------------------------------------------------------------------------------

int MistLandLua::AISetSlave(lua_State* pkLua) 
{
   return 0;
}

// -----------------------------------------------------------------------------------------------

int MistLandLua::GetClosestItemOfTypeLua(lua_State* pkLua) 
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
// 	{
//      	char	acType[128];
// 		g_pkScript->GetArgString(pkLua, 0, acType);	
// 
//       Entity* pkObj = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
//       if ( !pkObj )
//          return 0;
// 
//       Entity* pkClosestItem = 0;
// 
//       float fDistance = 99999999;      
//       
//       // TODO!!!: check more than the zone the user is in
//       ZoneData* pkZone = pkObj->m_pkEntityManager->GetZone( pkObj->GetWorldPosV() );
// 
//       vector<Entity*>* pkList = new vector<Entity*>;
// 
//       P_Item* pkItemProp = 0;
//       
//       pkZone->m_pkZone->GetAllEntitys( pkList );
// 
//       for ( unsigned int i = 0; i < pkList->size(); i++ )
//       {
//          // check if object has item property
//          if ( (pkItemProp = (P_Item*)pkList->at(i)->GetProperty ("P_Item")) != NULL )
//          {
//             // check if item is of right type
//             if ( pkItemProp->m_pkItemStats->m_kItemName == acType )
//             {
//             	Vector3 kPos = pkList->at(i)->GetWorldPosV();
//                // check if distance is smaller that the previos (if any) found
//                if ( pkObj->GetWorldPosV().DistanceTo(kPos) < fDistance )
//                {
//                   fDistance = (float) pkObj->GetWorldPosV().DistanceTo(kPos);
//                   pkClosestItem = pkList->at(i);
//                }
//             }
//             
//             pkItemProp = 0;
//          }
//       }
// 
//       delete pkList;
// 
//       if ( pkClosestItem )
//       {
//          g_pkScript->AddReturnValue(pkLua, pkClosestItem->GetEntityID());         
//       }
//       else
//       {
//          g_pkScript->AddReturnValue(pkLua, -1);
//       }
// 
// 
//       return 1;
//    }

   return 0;
}

// -----------------------------------------------------------------------------------------------

int MistLandLua::GetClosestPlayerLua(lua_State* pkLua) 
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
	{
  		double dRadius;
  		g_pkScript->GetArgNumber(pkLua, 0, &dRadius);				
      
      Entity* pkObj = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

      if ( !pkObj )
         return 0;

      Entity* pkClosestPlayer = 0;

      float fDistance = 99999999;      
      
      // TODO!!!: check more than the zone the user is in
//      ZoneData* pkZone = pkObj->m_pkEntityMan->GetZone( pkObj->GetWorldPosV() );

      vector<Entity*>* pkList = new vector<Entity*>;

      g_pkObjMan->GetAllEntitysInArea(pkList,pkObj->GetWorldPosV(),(float)dRadius);

      for ( unsigned int i = 0; i < pkList->size(); i++ )
      {
         // check if object has item property
         if ( pkList->at(i)->GetType() == "t_player.lua" )
         {
            Vector3 kPos = pkList->at(i)->GetWorldPosV();
            // check if distance is smaller that the previos (if any) found
            if ( pkObj->GetWorldPosV().DistanceTo(kPos) < fDistance )
            {
               fDistance = (float) pkObj->GetWorldPosV().DistanceTo(kPos);
               pkClosestPlayer = pkList->at(i);
            }
         }
      }

      delete pkList;

      if ( pkClosestPlayer )
         g_pkScript->AddReturnValue(pkLua, pkClosestPlayer->GetEntityID());
      else
         g_pkScript->AddReturnValue(pkLua, -1);


      return 1;
   }

   return 0;
}

// -----------------------------------------------------------------------------------------------

int MistLandLua::GetClosestObjectOfTypeLua(lua_State* pkLua) 
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
	{
    	double dRadius;
    	char	acType[128];
		g_pkScript->GetArgString(pkLua, 0, acType);		
		g_pkScript->GetArgNumber(pkLua, 1, &dRadius);				
	
      Entity* pkObj = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

      if ( !pkObj )
         return 0;

      Entity* pkClosestObject = 0;

      float fDistance = 99999999;      
      
      vector<Entity*> kList;

      g_pkObjMan->GetAllEntitysInArea(&kList,pkObj->GetWorldPosV(),(float)dRadius);

      for ( unsigned int i = 0; i <kList.size(); i++ )
      {
         // check if object has item property
         if ( kList[i]->GetType() == acType )
         {
            Vector3 kPos = kList[i]->GetWorldPosV();
            // check if distance is smaller that the previos (if any) found
            if ( pkObj->GetWorldPosV().DistanceTo(kPos) < fDistance )
            {
               fDistance = (float) pkObj->GetWorldPosV().DistanceTo(kPos);
               pkClosestObject = kList[i];
            }
         }
      }


      if ( pkClosestObject )
         g_pkScript->AddReturnValue(pkLua, pkClosestObject->GetEntityID());
      else
         g_pkScript->AddReturnValue(pkLua, -1);


      return 1;
   }

   return 0;
}

// -----------------------------------------------------------------------------------------------

int MistLandLua::GetSeenPlayerLua(lua_State* pkLua)
{
//  	if( g_pkScript->GetNumArgs(pkLua) == 0 )
// 	{
//       Entity* pkObj = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
//       CharacterProperty* pkCP = (CharacterProperty*)pkObj->GetProperty("P_CharStats");
// 
//       if ( !pkObj || !pkCP )
//          return 0;
// 
//       Entity* pkClosestObject = 0;
// 
//       float fDistance = 99999999;      
//       
//       // TODO!!!: check more than the zone the user is in
//       ZoneData* pkZone = pkObj->m_pkEntityManager->GetZone( pkObj->GetWorldPosV() );
// 
//       vector<Entity*>* pkList = new vector<Entity*>;
// 
//       pkZone->m_pkZone->GetAllEntitys ( pkList );
// 
//       for ( unsigned int i = 0; i < pkList->size(); i++ )
//       {
//          // check if object is character
//          if ( pkList->at(i)->GetType() == "t_player.lua" )
//          {
//             Vector3 kPos = pkList->at(i)->GetWorldPosV();
// 
//             float fDist = (float) pkObj->GetWorldPosV().DistanceXZTo(kPos);
// 
//             // check if player is seen
//             float fVisionRange = pkCP->GetCharStats()->m_fVision *
//                ((CharacterProperty*)pkList->at(i)->GetProperty("P_CharStats"))->GetCharStats()->m_fVisibility;
// 
//             Plane kView;
// 
//             // TODO!! Make a triangle with represents vision and test point inside it
// 
//             float fAngle = 0;
// 
//             // check if distance is smaller that the previos (if any) found
//             if ( fDist < fDistance && fVisionRange >= fDist && fAngle < 45 && fAngle > -45 )
//             {
//                fDistance = fDist;
//                pkClosestObject = pkList->at(i);
//             }
//          }
//       }
// 
//       delete pkList;
// 
//       if ( pkClosestObject )
//          g_pkScript->AddReturnValue(pkLua, pkClosestObject->GetEntityID());
//       else
//          g_pkScript->AddReturnValue(pkLua, -1);
// 
// 
//       return 1;
//    }

   return 0;

}

// -----------------------------------------------------------------------------------------------

int MistLandLua::SetAIIsPlayerLua(lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 1 )
// 	{
//       Entity* pkObj = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
//       if ( !pkObj )
//          return 0;
// 
//       P_AI* pkAI = (P_AI*)pkObj->GetProperty("P_AI");
// 
//       if ( !pkAI )
//       {
//          cout << "Warning! Tried to use luafunction SetAIIsPlayerLua on a object withour ai" << endl;
//          return 0;
//       }
// 
//       double dTemp;
//       g_pkScript->GetArgNumber(pkLua, 0, &dTemp);	
//       
// 	  if(dTemp > 0)
// 	      pkAI->SetAIIsPlayer ( true );
// 	  else
// 		  pkAI->SetAIIsPlayer ( false );
// 
//    }

   return 0;
}

// -----------------------------------------------------------------------------------------------

int MistLandLua::AIHaveTargetLua(lua_State* pkLua)
{
// 	if( g_pkScript->GetNumArgs(pkLua) == 0 )
// 	{
//       Entity* pkObj = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);
// 
//       if ( !pkObj )
//          return 0;
// 
//       P_AI* pkAI = (P_AI*)pkObj->GetProperty("P_AI");
// 
//       if ( !pkAI )
//       {
//          cout << "Warning! Tried to use luafunction SetAIIsPlayerLua on a object withour ai" << endl;
//          return 0;
//       }
// 
//       
//       g_pkScript->AddReturnValue( pkLua, pkAI->HaveTarget() );
// 
// 
//    }

   return 0;
}


// -----------------------------------------------------------------------------------------------

// int MistLandLua::SetAIStateLua(lua_State* pkLua)
// {
//  	if( g_pkScript->GetNumArgs(pkLua) == 2 )
//  	{
//     	double dEntID, dAIState;
//     	char	acType[128];
// 		g_pkScript->GetArgNumber(pkLua, 0, &dEntID);		
// 		g_pkScript->GetArgNumber(pkLua, 1, &dAIState);				
// 
// 		Entity* pkObj = g_pkObjMan->GetEntityByID(dEntID);
// 
//       if ( !pkObj )
//          return 0;
// 
// 		// Get P_AI
// 		P_AI* pkAI = (P_AI*)pkObj->GetProperty("P_AI");
// 
// 		if ( pkAI )
// 			pkAI->SetState(int(dAIState));
// 
//       return 0;
// 	}
// 
// 	return 0;
// }