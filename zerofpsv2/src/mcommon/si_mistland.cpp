#include "si_mistland.h"
#include "rulesystem/character/characterstats.h"
#include "p_charstats.h"
#include <cmath>                    // for trigonometry functions


ZFScriptSystem*	MistLandLua::g_pkScript;
ObjectManager*		MistLandLua::g_pkObjMan;
int					MistLandLua::g_iCurrentObjectID;
int					MistLandLua::g_iLastCollidedID;
int		MCOMMON_API			MistLandLua::g_iCurrentPCID;


void MistLandLua::Init(ObjectManager* pkObjMan,ZFScriptSystem* pkScript)
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

   // char.stats-scipts
   pkScript->ExposeFunction("RollSkillDice",				MistLandLua::RollSkillDiceLua);			
   pkScript->ExposeFunction("RollAttributeDice",		MistLandLua::RollAttributeDiceLua);			
   pkScript->ExposeFunction("SetCurrentSkill",			MistLandLua::SetCurrentSkillLua);			
   pkScript->ExposeFunction("AddToSkillValue",			MistLandLua::AddSkillValueLua);			
   pkScript->ExposeFunction("AddToAttributeValue",		MistLandLua::AddAttributeValueLua);			
   pkScript->ExposeFunction("SetSkillValueTo", 			MistLandLua::SetSkillValueLua);			
   pkScript->ExposeFunction("SetAttributeValueTo",		MistLandLua::SetAttributeValueLua);			
   pkScript->ExposeFunction("SetDataValueTo",			MistLandLua::SetDataValueLua);			
   // hp/mp stuff
   pkScript->ExposeFunction("SetHP",      				MistLandLua::SetHPLua);			
   pkScript->ExposeFunction("SetMP",            		MistLandLua::SetMPLua);			
   pkScript->ExposeFunction("HP",   			         MistLandLua::GetHPLua);			
   pkScript->ExposeFunction("MP",			            MistLandLua::GetMPLua);			
   pkScript->ExposeFunction("HPPercent",		         MistLandLua::GetHpPercentLua);			
   pkScript->ExposeFunction("MPPercent", 			      MistLandLua::GetMpPercentLua);			
   pkScript->ExposeFunction("AddHP",		            MistLandLua::AddHpLua);			
   pkScript->ExposeFunction("AddMP",			         MistLandLua::AddMpLua);			
   pkScript->ExposeFunction("PrintStats",			      MistLandLua::PrintStatsLua);	

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
	Object*	pkObj = g_pkObjMan->GetObjectByNetWorkID(iId);
	
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
	Object*	pkObj = g_pkObjMan->GetObjectByNetWorkID(iId);
	
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
	Object*	pkObj = g_pkObjMan->GetObjectByNetWorkID(iId);
	
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

	Object* pkME = g_pkObjMan->GetObjectByNetWorkID(iThisId);

	//check that the object is valid
	if(!pkME)
		return 0;
		
	Vector3 kThisPos = pkME->GetWorldPosV();


	vector<Object*> kObjects;
	g_pkObjMan->GetAllObjects(&kObjects);
	
	
	
	float closest = 9999999999;
	Object* pkClosest = NULL;
	
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
	
	Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(id);
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


int MistLandLua::SetPSystemLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 0)
		return 0;

	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
		int iId = (int)dTemp;
		
		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(iId);
		
		if(pkObject)
			pkObject->DeleteProperty("PSystemProperty");

		return 0;
	}
	
	if(g_pkScript->GetNumArgs(pkLua) == 2)
	{
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
		int iId = (int)dTemp;
		
		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(iId);
		if(pkObject)
		{
			PSystemProperty* ps = (PSystemProperty*)pkObject->GetProperty("PSystemProperty");
			
			if(!ps)
				ps = (PSystemProperty*)pkObject->AddProperty("PSystemProperty");
			
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
		
	Object* o1 = g_pkObjMan->GetObjectByNetWorkID(iId1);
	Object* o2 = g_pkObjMan->GetObjectByNetWorkID(iId2);

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

		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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
		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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
		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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
		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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
		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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
		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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
		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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
		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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

int MistLandLua::SetHPLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         pkCS->SetHP ( string(acType) );
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::SetMPLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
   {
		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		char	acType[128];
			g_pkScript->GetArgString(pkLua, 0, acType);

  			CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
         CharacterStats *pkCS = pkCP->GetCharStats();

         pkCS->SetMP ( string(acType) );
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------------------------

int MistLandLua::GetHPLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 0 )
   {
		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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
		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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
		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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
		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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
		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		

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
		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

	   if (pkObject)
		{
     		double dTemp;
         g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		

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
		Object* pkObject = g_pkObjMan->GetObjectByNetWorkID(g_iCurrentObjectID);

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
