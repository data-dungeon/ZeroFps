#include "si_mistland.h"
#include "rulesystem/character/characterstats.h"
#include "rulesystem/rulesystem.h" // for getting all types of skills&attributes
#include "rulesystem/item/itemstats.h"
#include "p_charstats.h"
#include "p_item.h"
#include "p_spell.h"
#include "../zerofpsv2/script/zfscript.h"
#include <cmath>                    // for trigonometry functions


ZFScriptSystem*		MistLandLua::g_pkScript;
EntityManager*			MistLandLua::g_pkObjMan;
int						MistLandLua::g_iCurrentObjectID;
int						MistLandLua::g_iLastCollidedID;
int						MistLandLua::g_iCurrentPCID = -1;
map<string,string>	MistLandLua::g_kServerList;
string					MistLandLua::g_kDefServer;

void MistLandLua::Init(EntityManager* pkObjMan,ZFScriptSystem* pkScript)
{
	g_pkObjMan = pkObjMan;
	g_pkScript = pkScript;
	g_iCurrentObjectID = -1;
	g_iLastCollidedID = -1;
	
	pkScript->ExposeFunction("GetSelfID",					MistLandLua::GetSelfIDLua);	
	pkScript->ExposeFunction("GetCurrentPCID",			MistLandLua::GetCurrentPCIDLua);		
	pkScript->ExposeFunction("GetObjectType",				MistLandLua::GetObjectTypeLua);		
	pkScript->ExposeFunction("GetObjectName",				MistLandLua::GetObjectNameLua);		
	pkScript->ExposeFunction("GetLastCollidedObject",	MistLandLua::GetLastCollidedObjectLua);		
	pkScript->ExposeFunction("GetClosestObject",			MistLandLua::GetClosestObjectLua);		
	pkScript->ExposeFunction("RemoveObject",				MistLandLua::RemoveObjectLua);		
	pkScript->ExposeFunction("SendEvent",					MistLandLua::SendEventLua);			
	pkScript->ExposeFunction("SetPSystem",					MistLandLua::SetPSystemLua);		
	pkScript->ExposeFunction("SetVelTo",					MistLandLua::SetVelToLua);			

	pkScript->ExposeFunction("AddAction",					MistLandLua::AddActionLua);			

   // char.stats-scipts
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

   pkScript->ExposeFunction("AddBeforeItemName",		MistLandLua::AddBeforeItemNameLua);
   pkScript->ExposeFunction("AddAfterItemName",	      MistLandLua::AddAfterItemNameLua);

   // Lua Lua commands
   pkScript->ExposeFunction("RunScript",			      MistLandLua::RunScriptLua);		

   // equip / unequip
   pkScript->ExposeFunction("EquipFromScript",   		MistLandLua::EquipFromScriptLua);			
   pkScript->ExposeFunction("Equip",   		         MistLandLua::EquipLua);			
   pkScript->ExposeFunction("UnEquip",	      		   MistLandLua::UnEquipLua);			
	pkScript->ExposeFunction("Equip",	      		   MistLandLua::EquipLua);			

   // random function
   pkScript->ExposeFunction("Random",	      		   MistLandLua::RandomLua);			
   pkScript->ExposeFunction("GetRandomCategory",	   MistLandLua::GetRandomCategoryLua);
   pkScript->ExposeFunction("GetRandomSkill",	      MistLandLua::GetRandomSkillLua);	
   pkScript->ExposeFunction("GetRandomAttribute",	   MistLandLua::GetRandomAttributeLua);

   pkScript->ExposeFunction("SetPropertyValue",	      MistLandLua::SetPropertyValueLua);

   pkScript->ExposeFunction("CastSpell",	            MistLandLua::CastSpellLua);

	// setup ip
	pkScript->ExposeFunction("AddServer", MistLandLua::AddServerLua);
	pkScript->ExposeFunction("SetDefaultServer", MistLandLua::SetDefaultServerLua);
	

}

int MistLandLua::GetSelfIDLua(lua_State* pkLua)
{
	g_pkScript->AddReturnValue(pkLua,g_iCurrentObjectID);
	
	return 1;
}

int MistLandLua::GetCurrentPCIDLua(lua_State* pkLua)
{
	g_pkScript->AddReturnValue(pkLua,g_iCurrentPCID);
	
	return 1;
}


int MistLandLua::GetLastCollidedObjectLua(lua_State* pkLua)
{
	g_pkScript->AddReturnValue(pkLua,g_iLastCollidedID);
	
	return 1;
}

int MistLandLua::GetObjectTypeLua(lua_State* pkLua)
{
	int iId = g_iCurrentObjectID;
	
	//get id
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		iId = (int)dTemp;
	}
	
	//get object
	Entity*	pkObj = g_pkObjMan->GetObjectByNetWorkID(iId);
	
	if(pkObj)
		g_pkScript->AddReturnValue(pkLua,(char*)pkObj->GetType().c_str(),pkObj->GetType().size());
	
	return 1;
}

int MistLandLua::GetObjectNameLua(lua_State* pkLua)
{
	int iId = g_iCurrentObjectID;
	
	//get id
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		iId = (int)dTemp;
	}
	
	//get object
	Entity*	pkObj = g_pkObjMan->GetObjectByNetWorkID(iId);
	
	if(pkObj)
		g_pkScript->AddReturnValue(pkLua,(char*)pkObj->GetName().c_str(),pkObj->GetName().size());
	
	return 1;
}

int MistLandLua::RemoveObjectLua(lua_State* pkLua)
{
	int iId = g_iCurrentObjectID;
	
	//get id
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		iId = (int)dTemp;
	}
	
	//get object
	Entity*	pkObj = g_pkObjMan->GetObjectByNetWorkID(iId);
	
	if(pkObj)
		g_pkObjMan->Delete(pkObj);
	
	return 0;
}

int MistLandLua::GetClosestObjectLua(lua_State* pkLua)
{
	int iThisId = g_iCurrentObjectID;

	//if there is a argument us it as thisid	
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		iThisId = (int)dTemp;
	}

	Entity* pkME = g_pkObjMan->GetObjectByNetWorkID(iThisId);

	//check that the object is valid
	if(!pkME)
		return 0;
		
	Vector3 kThisPos = pkME->GetWorldPosV();


	vector<Entity*> kObjects;
	g_pkObjMan->GetAllObjects(&kObjects);
	
	
	
	float closest = 9999999999;
	Entity* pkClosest = NULL;
	
	for(int i=0;i<kObjects.size();i++)
	{
		//dont compare to ur self or object 0
		if(kObjects[i]->iNetWorkID == iThisId || kObjects[i]->iNetWorkID == 0)
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
		g_pkScript->AddReturnValue(pkLua,pkClosest->iNetWorkID);
		return 1;
	}
		
	g_pkScript->AddReturnValue(pkLua,-1);		
	
	return 0;
}


int MistLandLua::SendEventLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 2)
		return 0;
	
	int id;
	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
	id = (int)dTemp;
	
	Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(id);
	if(pkObject)
	{
		P_Event* pe = (P_Event*)pkObject->GetProperty("P_Event");	
		
		if(pe)
		{
			char	acEvent[128];
			g_pkScript->GetArgString(pkLua, 1, acEvent);
		
			pe->SendEvent(acEvent);
			return 0;
		}
	}
	
	return 0;
}

int MistLandLua::AddActionLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 2)
		return 0;
	
	int id;
	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
	id = (int)dTemp;
	
	Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(id);
	if(pkObject)
	{
		P_Ml* pe = (P_Ml*)pkObject->GetProperty("P_Ml");	
		
		if(pe)
		{
			char	acEvent[128];
			g_pkScript->GetArgString(pkLua, 1, acEvent);
		
			pe->AddAction(acEvent);
			return 0;
		}
		else
			cout<<"Error tried to add action on object whitout P_Ml property"<<endl;
	}
	
	return 0;
}



int MistLandLua::SetPSystemLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 0)
		return 0;

	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
		int iId = (int)dTemp;
		
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(iId);
		
		if(pkObject)
			pkObject->DeleteProperty("P_PSystem");

		return 0;
	}
	
	if(g_pkScript->GetNumArgs(pkLua) == 2)
	{
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
		int iId = (int)dTemp;
		
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(iId);
		if(pkObject)
		{
			P_PSystem* ps = (P_PSystem*)pkObject->GetProperty("P_PSystem");
			
			if(!ps)
				ps = (P_PSystem*)pkObject->AddProperty("P_PSystem");
			
			if(ps)
			{
				char	acType[128];
				g_pkScript->GetArgString(pkLua, 1, acType);
				
				ps->SetPSType((string)acType);
				return 0;
			}
		}

		
		return 0;
	}
	return 0;
}


int MistLandLua::SetVelToLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 3)
		return 0;
		
	double dTemp;
	
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iId1 = (int)dTemp;
		
	g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
	int iId2 = (int)dTemp;
		
	g_pkScript->GetArgNumber(pkLua, 2, &dTemp);		
	float fVel = (float)dTemp;
		
		
	if(iId1 == iId2)
		return 0;
		
	Entity* o1 = g_pkObjMan->GetObjectByNetWorkID(iId1);
	Entity* o2 = g_pkObjMan->GetObjectByNetWorkID(iId2);


	if(o1 && o2)
	{
		if(o2->GetWorldPosV() == o1->GetWorldPosV())
			return 0;
		
		Vector3 dir = (o2->GetWorldPosV() - o1->GetWorldPosV()).Unit();
		
		o1->GetVel() = dir*fVel;
		
	}
	return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::RollSkillDiceLua (lua_State* pkLua)
{
    // error
	if(g_pkScript->GetNumArgs(pkLua) != 2)
      return 0;
   else
	{

		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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
            double dRoll = pkCS->RollSkillDice ( (string)acType, iDiffuculty );
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
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

		if(pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);
         
         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iDiffuculty = (int)dTemp;

			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         g_pkScript->AddReturnValue(pkLua, pkCS->RollAttributeDice ( (string)acType, iDiffuculty ) );

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
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iValue = (int)dTemp;
         
 			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         pkCS->SetSkill ( (string)acType, iValue );
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetAttributeValueLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iValue = (int)dTemp;
         
 			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         pkCS->SetAttribute ( (string)acType, iValue );
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------
int MistLandLua::SetDataValueLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

     		char	acType2[128];
			g_pkScript->GetArgString(pkLua, 1, acType2);
         
  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         pkCS->SetData ( (string)acType, (string)acType2 );
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetDefenceValueLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iValue = (int)dTemp;
         
  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         pkCS->SetDefenceValue ( (string)acType, iValue );
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetAttackValueLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iValue = (int)dTemp;
         
  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         pkCS->SetAttackValue ( (string)acType, iValue );
      }
   }

   return 0;
}
// ----------------------------------------------------------------------------------------------

int MistLandLua::SetHPLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");

         if ( pkCP )
         {
            CharacterStats *pkCS = pkCP->GetCharStats();
            pkCS->SetHP ( string(acType) );
         }
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetMPLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");

         if ( pkCP )
         {
            CharacterStats *pkCS = pkCP->GetCharStats();
            pkCS->SetMP ( string(acType) );
         }
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetMaxHPLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		double dTemp;
         g_pkScript->GetArgNumber(pkLua, 0, &dTemp);

  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         pkCS->SetMaxHP( int(dTemp) );
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetMaxMPLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		double dTemp;
         g_pkScript->GetArgNumber(pkLua, 0, &dTemp);

  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         pkCS->SetMaxMP( int(dTemp) );
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::GetHPLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 0 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         g_pkScript->AddReturnValue(pkLua, pkCS->GetHP () );
      }

      return 1;
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::GetMPLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 0 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{

  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         g_pkScript->AddReturnValue(pkLua, pkCS->GetMP () );
      }

      return 1;
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::GetHpPercentLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 0 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{

  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         g_pkScript->AddReturnValue(pkLua, pkCS->GetHPPercent () );
      }

      return 1;
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::GetMpPercentLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 0 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{

  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         g_pkScript->AddReturnValue(pkLua, pkCS->GetMPPercent () );
      }

      return 1;
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddHpLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		double dTemp;
         g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		

  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         pkCS->AddHP ( dTemp );
      }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddMpLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		double dTemp;
         g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		

  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         pkCS->AddMP ( dTemp );
      }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::PrintStatsLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 0 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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

int MistLandLua::GetQuantityLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 0 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
		   P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
         {      
            ItemStats *pkIS = pkCP->m_pkItemStats;
            g_pkScript->AddReturnValue(pkLua, pkIS->GetQuantity() );
         }
         else
         {
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
            return 0;
         }
          
         return 1;
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetQuantityLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetQualityLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		double dTemp;
         g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
            pkCP->m_pkItemStats->SetQuality ( dTemp );
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
      }

   }

   return 0;
}


// ----------------------------------------------------------------------------------------------

int MistLandLua::SetIconLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acPic[128];
			g_pkScript->GetArgString(pkLua, 0, acPic);

     		char	acPicMask[128];
			g_pkScript->GetArgString(pkLua, 1, acPicMask);

  			P_Item* pkIP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkIP )
            pkIP->m_pkItemStats->LoadIcons ( acPic, acPicMask );
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
 
      }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetSkillBonusLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetAttributeBonusLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetAttackBonusLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iValue = (int)dTemp;

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
            pkCP->m_pkItemStats->SetAttackBonus ( string(acType), iValue );
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
 
      }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetDefenceBonusLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iValue = (int)dTemp;

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
            pkCP->m_pkItemStats->SetDefenceBonus ( string(acType), iValue );
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
 
      }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::EquipOnLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

 			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
            pkCP->m_pkItemStats->AddCanEquipOn( string(acType) );
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
      }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::GetSkillBonusLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
         char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

		   P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
         {      
            ItemStats *pkIS = pkCP->m_pkItemStats;
            g_pkScript->AddReturnValue(pkLua, pkIS->GetSkillBonus ( string (acType) ) );
         }
         else
         {
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
            return 0;
         }
          
         return 1;
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::GetAttributeBonusLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
         char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

		   P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
         {      
            ItemStats *pkIS = pkCP->m_pkItemStats;
            g_pkScript->AddReturnValue(pkLua, pkIS->GetAttributeBonus ( string (acType) ) );
         }
         else
         {
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
            return 0;
         }
          
         return 1;
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------
int MistLandLua::GetAttackBonusLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
         char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

		   P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
         {      
            ItemStats *pkIS = pkCP->m_pkItemStats;
            g_pkScript->AddReturnValue(pkLua, pkIS->GetAttackBonus ( string (acType) ) );
         }
         else
         {
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
            return 0;
         }
          
         return 1;
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------
int MistLandLua::GetDefenceBonusLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
         char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

		   P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
         {      
            ItemStats *pkIS = pkCP->m_pkItemStats;
            g_pkScript->AddReturnValue(pkLua, pkIS->GetDefenceBonus ( string (acType) ) );
         }
         else
         {
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
            return 0;
         }
          
         return 1;
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddSkillBonusLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iValue = (int)dTemp;

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
         {      
            ItemStats *pkIS = pkCP->m_pkItemStats;
            pkIS->AddToSkillBonus ( string(acType), iValue );
         }
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
 
      }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddAttributeBonusLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iValue = (int)dTemp;

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
         {      
            ItemStats *pkIS = pkCP->m_pkItemStats;
            pkIS->AddToAttributeBonus ( string(acType), iValue );
         }
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
       }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddAttackBonusLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iValue = (int)dTemp;

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
         {      
            ItemStats *pkIS = pkCP->m_pkItemStats;
            pkIS->AddToAttackBonus ( string(acType), iValue );
         }
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
 
      }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddDefenceBonusLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
   		int iValue = (int)dTemp;

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
         {      
            ItemStats *pkIS = pkCP->m_pkItemStats;
            pkIS->AddToDefenceBonus ( string(acType), iValue );
         }
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
 
      }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::PrintItemStatsLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 0 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
         {      
            ItemStats *pkIS = pkCP->m_pkItemStats;
            pkIS->Print();
         }
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
 
      }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetItemNameLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
         {      
            ItemStats *pkIS = pkCP->m_pkItemStats;
            pkIS->SetItemName ( string (acType) );
         }
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
 
      }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetItemWeightLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
         {      
            ItemStats *pkIS = pkCP->m_pkItemStats;
            pkIS->SetItemWeight ( dTemp );
         }
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
 
      }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetItemValueLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
         {      
            ItemStats *pkIS = pkCP->m_pkItemStats;
            pkIS->SetItemValue ( int(dTemp) );
         }
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
 
      }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::GetItemValueLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 0 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
         {      
            ItemStats *pkIS = pkCP->m_pkItemStats;
            g_pkScript->AddReturnValue(pkLua, pkIS->GetItemValue () );

         }
         else
         {
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
            return 0;
         }

         return 1;
 
      }

   }

   return 0;

}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddItemValueLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
         {      
            ItemStats *pkIS = pkCP->m_pkItemStats;
            pkIS->AddItemValue ( int(dTemp) );
         }
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
 
      }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddBeforeItemNameLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);	

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
            pkCP->m_pkItemStats->AddBeforeName ( string(acType ) );
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
 
      }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::AddAfterItemNameLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);	

  			P_Item* pkCP = (P_Item*)pkObject->GetProperty("P_Item");

         if ( pkCP )
            pkCP->m_pkItemStats->AddAfterName ( string(acType ) );
         else
            cout << "Warning! Tried to use a item function on a non-item object!" << endl;
 
      }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------


int MistLandLua::UnEquipLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acSlot[128];
			g_pkScript->GetArgString(pkLua, 0, acSlot);

         CharacterProperty *pkChar = (CharacterProperty*)pkObject->GetProperty("P_CharStats");

         // TODO! Ta emot objectet när det returneras, annars försvinner det ut i cyberspace
         if ( pkChar )
            pkChar->GetCharStats()->UnEquip( string(acSlot) );
         else
            cout << "Warning! Tried to unequip something on a non-character object!" << endl;
       }

   }

   return 0;

}

// ----------------------------------------------------------------------------------------------

int MistLandLua::EquipFromScriptLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acItem[128];
			g_pkScript->GetArgString(pkLua, 0, acItem);

         char	acSlot[128];
			g_pkScript->GetArgString(pkLua, 1, acSlot);


         CharacterProperty *pkChar = (CharacterProperty*)pkObject->GetProperty("P_CharStats");

         if ( pkChar )
         {      
            // save current Object ID
            int iOldObject = g_iCurrentObjectID;
            ZFScriptSystem* pkZFScriptSys = g_pkScript;

            // create the new object
            Entity* pkNewObj = g_pkObjMan->CreateObjectFromScriptInZone(acItem, pkObject->GetWorldPosV() );

            if ( pkNewObj )
            {
               // if object has Event propery..
               if ( pkNewObj->GetProperty ("P_Event") )
                  // run the INIT function in the script
                  pkNewObj->GetProperty("P_Event")->Update();

               // equip the new, nice object
               if ( !pkChar->GetCharStats()->Equip ( pkNewObj, string(acSlot) ) )
                  // if we couln't equip the object, delete it.
                  delete pkNewObj;
            }

            // return everything the way it was before calling the script
            g_iCurrentObjectID = iOldObject;
            g_pkScript = pkZFScriptSys;

           
         }
         else
            cout << "Warning! Tried to equip something on a non-character object!" << endl;
 
      }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::EquipLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
         double dTemp;
         g_pkScript->GetArgNumber(pkLua, 0, &dTemp);

         char	acSlot[128];
			g_pkScript->GetArgString(pkLua, 1, acSlot);

         Entity* pkCharProp = g_pkObjMan->GetObjectByNetWorkID ( int(dTemp) );

         if ( pkCharProp )
         {
            CharacterProperty *pkChar = (CharacterProperty*)pkCharProp->GetProperty("P_CharStats");   

            if ( pkChar )
            {      
               pkChar->GetCharStats()->Equip ( pkObject, string(acSlot) );    
					printf("Succeeded to equip item\n");
            }
            else
               cout << "Warning! Tried to equip something on a non-character object!" << endl;
         }
       }

   }



/* test kod

   Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

   Object* pkNewObj = g_pkObjMan->CreateObjectFromScriptInZone("data/script/objects/t_item.lua", pkObject->GetLocalPosV() );
   pkNewObj->GetProperty("P_Event")->Update();

   CharacterProperty* pCharProp = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
   pCharProp->GetCharStats()->Equip( pkNewObj, "lefthand" );

   P_Item *pkItem = (P_Item*)pkNewObj->GetProperty("P_Item");   

   Object *pkNewItObj = pkItem->Split (10);

   pkItem = (P_Item*)pkNewItObj->GetProperty("P_Item");

   pkItem->m_pkItemStats->Print();


*/

   return 0;
}

// ----------------------------------------------------------------------------------------------

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
	// if stat-types isn't loaded
	if ( !g_kSkills.size() && !g_kAttributes.size() && !g_kData.size() )
      LoadStatTypes();
   
   int iRand = rand()%g_kSkills.size();
   g_pkScript->AddReturnValue(pkLua, (char*)g_kSkills[iRand].c_str(), g_kSkills[iRand].size() );

   return 1;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::GetRandomAttributeLua (lua_State* pkLua)
{
	// if stat-types isn't loaded
	if ( !g_kSkills.size() && !g_kAttributes.size() && !g_kData.size() )
      LoadStatTypes();

   int iRand = rand()%g_kAttributes.size();
   g_pkScript->AddReturnValue(pkLua, (char*)g_kAttributes[iRand].c_str(), g_kAttributes[iRand].size() );

   return 1;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::RunScriptLua (lua_State* pkLua)
{
 	if( g_pkScript->GetNumArgs(pkLua) == 2 )
   {
     	char	acType[128];
		g_pkScript->GetArgString(pkLua, 0, acType);
		
		double temp;
		g_pkScript->GetArgNumber(pkLua, 1, &temp);
 		int objectid = temp;
 
      
      ZFScriptSystem* pkZFScriptSys = g_pkScript;
		
		Entity* object = g_pkObjMan->GetObjectByNetWorkID(objectid);
		if(!object)
		{
			cout<<"parend object does not exist"<<endl;
			return 0;
		}
		
      // create the new object
      Entity* pkNewObj = g_pkObjMan->CreateObjectFromScriptInZone(acType, 
                         object->GetWorldPosV() );

      // return everything the way it was
      g_pkScript = pkZFScriptSys;

      // return the new object´s ID      
      g_pkScript->AddReturnValue( pkLua, pkNewObj->iNetWorkID );

      return 1; // return newcreated-object_ID
      
   } 
   return 0;  

}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetPropertyValueLua(lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 3 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acProp[128];
			g_pkScript->GetArgString(pkLua, 0, acProp);

     		char	acPropData[128];
			g_pkScript->GetArgString(pkLua, 1, acPropData);

     		char	acValue[128];
			g_pkScript->GetArgString(pkLua, 2, acValue);

         Property *pkProp = pkObject->GetProperty(acProp);

         if ( pkProp )
            pkProp->SetValue ( string(acPropData), string(acValue) );
         else
            cout << "Property " << acProp << " didn't exist on object." << endl;
       }

   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

// spell
int MistLandLua::CastSpellLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 3 )
   {
		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
         //spell
     		char acValue[128];
			g_pkScript->GetArgString(pkLua, 0, acValue);

         // caster
         double dCaster;
         g_pkScript->GetArgNumber(pkLua, 1, &dCaster);

         // target
         double dTarget;
         g_pkScript->GetArgNumber(pkLua, 2, &dTarget);

			Entity* me = g_pkObjMan->GetObjectByNetWorkID(dCaster);
			if(!me)
				return 0;

			//cout<<"pos:"<<me.x<<endl;

         Entity* pkSpell = 
                 g_pkObjMan->CreateObjectFromScriptInZone(acValue,me->GetWorldPosV());

         P_Spell* pkSpellProp = (P_Spell*)pkSpell->GetProperty("P_Spell");
         pkSpellProp->SetCaster ( dCaster );

         g_pkScript->AddReturnValue( pkLua, pkSpell->iNetWorkID );
                
         return 1;
       }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

// 1:st arg = server name (string)
// 2:nd arg = ip name (ie."192.168.0.153:4242") (string)
int MistLandLua::AddServerLua(lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) != 2 )
	{
		printf("Failed to add user to list. Bad argumetns!\n");
		return 0;
	}

   char acName[128], acIP[128];
	
	//user name
	g_pkScript->GetArgString(pkLua, 0, acName);

   //user ip
	g_pkScript->GetArgString(pkLua, 1, acIP);

	map<string,string>::iterator it = g_kServerList.find(string(acName));

	if(it != g_kServerList.end())
	{
		printf("Failed to add user to list. User already exist!\n");
		return 0;
	}

	g_kServerList.insert(map<string,string>::value_type(string(acName),string(acIP)));

	return 1;
}

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