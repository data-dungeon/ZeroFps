#include "../../script/zfscript.h"
#include "si_objectmanager.h"
#include "../../engine/entitymanager.h"
#include "../../engine_systems/propertys/p_mad.h"
#include "../../script/zfscript.h"
#include "../propertys/p_tcs.h"
#include "../propertys/p_scriptinterface.h"

namespace ObjectManagerLua
{
ZFScriptSystem* g_pkScript;
EntityManager*  g_pkObjMan;

Entity*			 g_pkLastObject;
Entity*			 g_pkLastParent;
Property*		 g_pkLastProperty;
Entity*			 g_pkReturnObject;

Entity*			 g_pkLastParentBak;
Property*		 g_pkLastPropertyBak;
Entity*			 g_pkReturnObjectBak;

int				g_iCurrentObjectID;
int				g_iCurrentPCID;

void Init(EntityManager* pkObjMan, ZFScriptSystem* pkScript)
{
	g_pkObjMan = pkObjMan;
	g_pkScript = pkScript;
	
	g_iCurrentObjectID = -1;
	g_iCurrentPCID     = -1;

	Reset();
		
	//create
	pkScript->ExposeFunction("InitObject",				ObjectManagerLua::InitObjectLua);
	pkScript->ExposeFunction("InitProperty",		  	ObjectManagerLua::InitPropertyLua);
	pkScript->ExposeFunction("InitParameter",			ObjectManagerLua::InitParameterLua);
	pkScript->ExposeFunction("AttachToParent",		ObjectManagerLua::AttachToParent);
	pkScript->ExposeFunction("SetLocalPos",			ObjectManagerLua::SetLocalPosLua);
	pkScript->ExposeFunction("HaveRelativOri",		ObjectManagerLua::HaveRelativOriLua);
	pkScript->ExposeFunction("SetParentObject",  	ObjectManagerLua::SetParentObjectLua);
	pkScript->ExposeFunction("SetReturnObject",  	ObjectManagerLua::SetReturnObjectLua);
	//----

	// entity management
	pkScript->ExposeFunction("CreateEntity", 			ObjectManagerLua::CreateEntityLua);
	pkScript->ExposeFunction("Delete",  				ObjectManagerLua::DeleteLua);

	// mad handling
	pkScript->ExposeFunction("PlayAnim",				ObjectManagerLua::PlayAnim);
	pkScript->ExposeFunction("SetNextAnim",			ObjectManagerLua::SetNextAnim);
	pkScript->ExposeFunction("AddMesh",					ObjectManagerLua::AddMesh);

	// entity Variables
	pkScript->ExposeFunction("GetLocalDouble",		ObjectManagerLua::GetLocalDouble);
	pkScript->ExposeFunction("SetLocalDouble",		ObjectManagerLua::SetLocalDouble);

	// entity orientation
	pkScript->ExposeFunction("GetObjectPos",			ObjectManagerLua::GetObjectPosLua);
	pkScript->ExposeFunction("GetObjectRot",			ObjectManagerLua::GetObjectRotLua);
	pkScript->ExposeFunction("SetObjectPos",			ObjectManagerLua::SetObjectPosLua);
	pkScript->ExposeFunction("DistanceTo",				ObjectManagerLua::DistanceToLua);

	// rotation functions
	pkScript->ExposeFunction("SetRotVel",				ObjectManagerLua::SetObjectRotVelLua);

	// velocity functions
	pkScript->ExposeFunction("SetVelTo",				ObjectManagerLua::SetVelToLua);
	
	// TCS
	pkScript->ExposeFunction("ApplyImpuls",			ObjectManagerLua::ApplyImpulsLua);
	
	
	//zone management
	pkScript->ExposeFunction("GetZoneIDAtPos",		ObjectManagerLua::GetZoneIDAtPosLua);
	pkScript->ExposeFunction("GetZoneID",				ObjectManagerLua::GetZoneIDLua);
	pkScript->ExposeFunction("SetZoneModel",			ObjectManagerLua::SetZoneModelLua);


	// Common used functions , used together whit P_ScriptInterface
	pkScript->ExposeFunction("SIGetSelfID",			ObjectManagerLua::SIGetSelfIDLua);		
	pkScript->ExposeFunction("SISetHeartRate",		ObjectManagerLua::SISetHeartRateLua);
}

void Reset()
{
	g_pkLastObject   = NULL;
	g_pkLastProperty = NULL;
	g_pkLastParent   = NULL;
	g_pkReturnObject = NULL;
}

void Push()
{
	//g_pkLastObjectBak   = g_pkLastObject;
	g_pkLastPropertyBak = g_pkLastProperty;
	g_pkLastParentBak   = g_pkLastParent;
	g_pkReturnObjectBak = g_pkReturnObject;
	
	Reset();
}

void Pop()
{
	//g_pkLastObject   = g_pkLastObjectBak;
	g_pkLastProperty = g_pkLastPropertyBak;
	g_pkLastParent   = g_pkLastParentBak;
	g_pkReturnObject = g_pkReturnObjectBak;
}


/**	\fn InitObject(ScripName)
 	\relates MistLandScript
   \brief Run a script to create objects.
   \param ScripName Name of the script to run. If not given a empty object will be created.
*/
int InitObjectLua(lua_State* pkLua)
{
	
	//if an object script has ben given creat that object
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		char acName[256];
		g_pkScript->GetArg(pkLua, 0, acName);
		
		g_pkLastObject = g_pkObjMan->CreateObjectFromScript(acName);
		
		//set return object of there is none
		if(!g_pkReturnObject)
			g_pkReturnObject = g_pkLastObject;
		
		return 1;
	}
		
	//else create an empty object
	g_pkLastObject = g_pkObjMan->CreateObject();
	
	//set return object of there is none	
	if(!g_pkReturnObject)
		g_pkReturnObject = g_pkLastObject;
	
	return 1;
}	

/**	\fn SetParentObject()
 	\relates MistLandScript
	\brief Sets the last created object to be the active parent object.
*/
int SetParentObjectLua(lua_State* pkLua)
{
	if(!g_pkLastObject)
		return 0;
	
	g_pkLastParent = g_pkLastObject;

	//cout<<"setting parent"<<endl;

	return 1;
}

/**	\fn AttachToParent()
 	\relates MistLandScript
	\brief Sets the last created object to be a child to the active parent object,
*/
int AttachToParent(lua_State* pkLua)
{
	if(!g_pkLastObject)
		return 0;
		
	if(!g_pkLastParent)
		return 0;

	g_pkLastObject->SetParent(g_pkLastParent);
	
	//cout<<"Attaching object to parent"<<endl;
	
	return 1;
}	

/**	\fn InitProperty(PropertyName)
 	\relates MistLandScript
   \brief Gives a Entity a property.
   \param PropertyName Name of property to assign to object.

	Assigns the property with the def values to the last created object.	
*/
int InitPropertyLua(lua_State* pkLua)
{
	if(g_pkLastObject == NULL)
		return 0;
	
	if(g_pkScript->GetNumArgs(pkLua) != 1)
		return 0;

	char acName[100];
	g_pkScript->GetArg(pkLua, 0, acName);

	g_pkLastProperty = g_pkLastObject->AddProperty(acName);
	return 1;
}	

/**	\fn InitParameter(szName, szValue )
 	\relates MistLandScript
   \brief Sets the value of a variable in a property.
   \param szName Name of variable to set.
   \param szValue Value to set variable to.

	Sets the variable to a given value on the last created property.	
*/
int InitParameterLua(lua_State* pkLua)
{
	if(g_pkLastProperty == NULL)
		return 0;
	
	if(g_pkScript->GetNumArgs(pkLua) != 2)
		return 0;

	char acName[50];
	g_pkScript->GetArg(pkLua, 0, acName);
	
	char acData[50];
	g_pkScript->GetArgString(pkLua, 1, (char*)acData);
	
	if(!g_pkLastProperty->SetValue((string)acName,(string)acData))
		cout<<"Error setting parameter:"<<acName<<" to "<<acData<<endl;
	return 0;
}	

/**	\fn SetLocalPos(x,y,z)
 	\relates MistLandScript
	\brief Sets the local pos of the last object.
*/
int SetLocalPosLua(lua_State* pkLua)
{
	if(g_pkLastObject == NULL)
		return 0;

	if(g_pkScript->GetNumArgs(pkLua) != 3)
		return 0;	

	double x,y,z;
	
	g_pkScript->GetArg(pkLua, 0, &x);
	g_pkScript->GetArg(pkLua, 1, &y);
	g_pkScript->GetArg(pkLua, 2, &z);
	
	g_pkLastObject->SetLocalPosV(Vector3((float)x,(float)y,(float)z));
	
	return 0;
}

/**	\fn HaveRelativOri()
 	\relates MistLandScript
	\brief Sets the last object to have relative orientation.
*/

int HaveRelativOriLua(lua_State* pkLua)
{
	if(g_pkLastObject == NULL)
		return 0;
	
	g_pkLastObject->SetRelativeOri(true);
	return 0;
}


/**	\fn SetReturnObject(x,y,z)
 	\relates MistLandScript
	\brief Sets the last object to be the object to be returned as the new created object.
*/
int SetReturnObjectLua(lua_State* pkLua)
{
	if(g_pkLastObject == NULL)
		return 0;
	
	g_pkReturnObject = g_pkLastObject;

	return 0;
}
//----end of create

/**	\fn Delete( Object )
 	\relates MistLandScript
	\brief Delete the object given as a parameter.
*/
int DeleteLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 1)
		return 0;
	
	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
	
	
	Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID((int)dTemp);	
	if(pkObject)
		g_pkObjMan->Delete(pkObject);
	
	
	return 0;
}

/**	\fn PlayAnim(ObjectID, AnimName)
 	\relates MistLandScript
   \brief Sets the playing animation.
*/
int PlayAnim(lua_State* pkLua)
{
	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iId1 = (int)dTemp;

	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);

//	printf("Should Play A Animation '%s' on object %d", acName,  iId1);

	Entity* o1 = g_pkObjMan->GetObjectByNetWorkID(iId1);
	P_Mad* mp = dynamic_cast<P_Mad*>(o1->GetProperty("P_Mad"));
	mp->SetAnimation(acName,0);
	
	return 1;
}

/**	\fn SetNextAnim(ObjectID, AnimName)
 	\relates MistLandScript
   \brief Sets the next animation for a object to play.

	Sets the next animation to play on a object. Stops to looping of the currently playing animation
	(if any) and then play the one given as a parameter. That animation will the loop. 
*/
int SetNextAnim(lua_State* pkLua)
{
	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iId1 = (int)dTemp;

	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);
//	printf("Next Anim to play is '%s' on object %d", acName,  iId1);

	Entity* o1 = g_pkObjMan->GetObjectByNetWorkID(iId1);
	P_Mad* mp = dynamic_cast<P_Mad*>(o1->GetProperty("P_Mad"));
	mp->SetNextAnimation(acName);
	return 1;
}

int AddMesh(lua_State* pkLua)
{
	// Get ObjectID ID
	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iId1 = (int)dTemp;

	// Get MeshName
	g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
	int iId2 = (int)dTemp;

	Entity* o1 = g_pkObjMan->GetObjectByNetWorkID(iId1);
	P_Mad* mp = dynamic_cast<P_Mad*>(o1->GetProperty("P_Mad"));

	mp->AddMesh( iId2 );
	return 1;
}

/**	\fn GetLocalDouble(ObjectID, VariableName)
 	\relates MistLandScript
   \brief Get value of a double variable stored in Entity.
   \param ObjectID Entity to get variable from.
   \param VariableName Name of variable.
	\return Return Return value of variable

	Get value of a double variable stored in Entity. The variable is saved with the object and it
	can be used to save status flags used by the script. If no variable with the given name is 
	found a value of 0.0 will be returned.
*/
int GetLocalDouble(lua_State* pkLua)
{
	// Get ObjectID ID
	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iId1 = (int)dTemp;

	// Get Variable Name
	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);

	Entity* o1 = g_pkObjMan->GetObjectByNetWorkID(iId1);
	string arne = acName;
	double dValue = o1->GetVarDouble(arne);
	//printf("GetLocalDouble Entity[%d] = %s is %f\n", iId1, acName, dValue);
	g_pkScript->AddReturnValue(pkLua, dValue);
	return 1;

}

/**	\fn SetLocalDouble(ObjectID, VariableName, Value)
 	\relates MistLandScript
   \brief Set value of a double variable stored in Entity.
   \param ObjectID ObjectID Entity to get variable from.
   \param VariableName VariableName Name of variable.
   \param Value New value to set variable to.

	Set value of a double variable stored in Entity. If the variable don't exist it will be
	created.
*/


int SetLocalDouble(lua_State* pkLua)
{
	// Get ObjectID ID
	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iId1 = (int)dTemp;

	// Get Variable Name
	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);

	g_pkScript->GetArgNumber(pkLua, 2, &dTemp);		
	float fValue = (float) dTemp;

	Entity* o1 = g_pkObjMan->GetObjectByNetWorkID(iId1);
	//printf("SetLocalDouble Entity[%d] = %s is set to %f \n", iId1, acName,fValue);
	string arne = acName;
	o1->SetVarDouble(arne, fValue);
	return 1;	
}

/*
	SetLocalString
	GotLocalString
*/

int SetObjectRotVelLua (lua_State* pkLua)
{
	int iNrArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNrArgs != 2)
		return 0;

	double dID;
	g_pkScript->GetArgNumber(pkLua, 0, &dID);		

	Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID((int)dID);

	if(pkObject)
	{
		vector<TABLE_DATA> vkData;
		g_pkScript->GetArgTable(pkLua, 2, vkData); // första argumetet startar på 1

      // Get physic-property
      P_Tcs* pkTcs = (P_Tcs*)pkObject->GetProperty("P_Tcs");

      if ( pkTcs )
      {
		   pkTcs->SetRotVel (  Vector3(
			   (float) (*(double*) vkData[0].pData),
			   (float) (*(double*) vkData[1].pData),
			   (float) (*(double*) vkData[2].pData)) );
      }
      else
         cout << "Warning! Tried to set RotVel on a object without P_Tcs!" << endl;

		g_pkScript->DeleteTable(vkData);
	}

	return 1;
}

int SetObjectPosLua(lua_State* pkLua)
{
	int iNrArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNrArgs != 2)
	{
		printf("Script funtion SetObjectPosLua failed! Expects 2 arguments.\n");
		return 0;
	}

	double dID;
	g_pkScript->GetArgNumber(pkLua, 0, &dID);		

	Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID((int)dID);

	if(pkObject)
	{
		vector<TABLE_DATA> vkData;
		g_pkScript->GetArgTable(pkLua, 2, vkData); // första argumetet startar på 1

		pkObject->SetWorldPosV( Vector3(
			(float) (*(double*) vkData[0].pData),
			(float) (*(double*) vkData[1].pData),
			(float) (*(double*) vkData[2].pData)) );

		g_pkScript->DeleteTable(vkData);
	}

	return 1;
}

int GetObjectPosLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 1)
	{
		printf("Script funtion GetObjectPos failed! Expects 1 arguments.\n");
		return 0;
	}

	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iId = (int) dTemp;

	Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(iId);

	if(pkObject)
	{
		Vector3 pos = pkObject->GetWorldPosV();

		vector<TABLE_DATA> vkData;

		TABLE_DATA temp;

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = pos.x;
		vkData.push_back(temp);

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = pos.y;
		vkData.push_back(temp);

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = pos.z;
		vkData.push_back(temp);

		g_pkScript->AddReturnValueTable(pkLua, vkData);
	}

	return 1;
}


int GetObjectRotLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 1)
	{
		printf("Script funtion GetObjectRot failed! Expects 1 arguments.\n");
		return 0;
	}

	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iId = (int) dTemp;

	Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID(iId);

	if(pkObject)
	{
		Vector3 kRot = pkObject->GetWorldRotV();

		vector<TABLE_DATA> vkData;

		TABLE_DATA temp;

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = kRot.x;
		vkData.push_back(temp);

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = kRot.y;
		vkData.push_back(temp);

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = kRot.z;
		vkData.push_back(temp);

		g_pkScript->AddReturnValueTable(pkLua, vkData);
	}

	return 1;
}

int SIGetSelfIDLua(lua_State* pkLua)
{
	g_pkScript->AddReturnValue(pkLua,g_iCurrentObjectID);
	
	return 1;
}

int SISetHeartRateLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 2)
	{
		double dId;	
		double dHeartRate;
		g_pkScript->GetArgNumber(pkLua, 0, &dId);
		g_pkScript->GetArgNumber(pkLua, 1, &dHeartRate);		

		Entity* pkObject = g_pkObjMan->GetObjectByNetWorkID((int)dId);
		
		if(pkObject)
		{	
			P_ScriptInterface* ep = (P_ScriptInterface*)pkObject->GetProperty("P_ScriptInterface");
			if(ep)
				ep->SetHeartRate((float)dHeartRate);			
		}
	}
	else
		cout<<"SetHeartRate[ObjectID, HeartRate] {HeartRate = -1}"<<endl;
	
	return 0;
}

// takes objectID, objectID
int DistanceToLua(lua_State* pkLua)
{
	int iNrArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNrArgs != 2)
	{
		printf("Script function SetObjectPosLua failed! Expects 2 arguments.\n");
		return 0;
	}

	double dID1, dID2;
	g_pkScript->GetArgNumber(pkLua, 0, &dID1);
	g_pkScript->GetArgNumber(pkLua, 1, &dID2);

	Entity* pkObject1 = g_pkObjMan->GetObjectByNetWorkID((int)dID1);
	Entity* pkObject2 = g_pkObjMan->GetObjectByNetWorkID((int)dID2);

	if( pkObject1 != 0 && pkObject2!= 0 )
	{
		Vector3 kTemp = pkObject2->GetWorldPosV();
		double dDistance = pkObject1->GetWorldPosV().DistanceTo(kTemp);
		g_pkScript->AddReturnValue(pkLua, dDistance);

		return 1;
	}

	g_pkScript->AddReturnValue(pkLua, 999999);

	return 1;
}


int SetVelToLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 3)
	{
		double dId;

		double dVel;
		Vector3 kPos;
		vector<TABLE_DATA> vkData;

		g_pkScript->GetArgNumber(pkLua, 0, &dId);
		g_pkScript->GetArgTable(pkLua, 2, vkData);
		g_pkScript->GetArgNumber(pkLua, 2, &dVel);

		kPos = Vector3(
			(float) (*(double*) vkData[0].pData),
			(float) (*(double*) vkData[1].pData),
			(float) (*(double*) vkData[2].pData));

		Entity* pkEnt = g_pkObjMan->GetObjectByNetWorkID((int)dId);

		if(pkEnt)
		{
			Vector3 dir = (kPos - pkEnt->GetWorldPosV()).Unit();

			pkEnt->SetVel(dir*(float) dVel);
		}
		return 0;
	}

   return 0;
}

int ApplyImpulsLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 2)
	{
		double dId;
		vector<TABLE_DATA> vkData;

		g_pkScript->GetArgNumber(pkLua, 0, &dId);
		g_pkScript->GetArgTable(pkLua, 2, vkData);

		Vector3 kDir((float) (*(double*) vkData[0].pData),
						(float) (*(double*) vkData[1].pData),
						(float) (*(double*) vkData[2].pData));

		if(Entity* pkEnt = g_pkObjMan->GetObjectByNetWorkID((int)dId))
		{
			if(P_Tcs* pkTcs = (P_Tcs*)pkEnt->GetProperty("P_Tcs"))
			{
				pkTcs->ApplyImpulsForce(kDir);
				return 1;
			}
		}
		return 0;
	}

   return 0;	

}


int GetZoneIDLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dId;

		g_pkScript->GetArgNumber(pkLua, 0, &dId);

		if(Entity* pkEnt = g_pkObjMan->GetObjectByNetWorkID((int)dId))
		{
			Vector3 kPos = pkEnt->GetWorldPosV();

			if(ZoneData* pkZone = g_pkObjMan->GetZone(kPos))
			{
				g_pkScript->AddReturnValue(pkLua,pkZone->m_iZoneID);
				return 1;
			}
		}
	}


	g_pkScript->AddReturnValue(pkLua,-1);

   return 0;
}

int GetZoneIDAtPosLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		Vector3 kPos;
		vector<TABLE_DATA> vkData;

		g_pkScript->GetArgTable(pkLua, 1, vkData);

		kPos = Vector3(
			(float) (*(double*) vkData[0].pData),
			(float) (*(double*) vkData[1].pData),
			(float) (*(double*) vkData[2].pData));

		if(ZoneData* pkZone = g_pkObjMan->GetZone(kPos))
		{
			g_pkScript->AddReturnValue(pkLua,pkZone->m_iZoneID);
			return 1;
		}
	}


	g_pkScript->AddReturnValue(pkLua,-1);

   return 0;
}

int SetZoneModelLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 2)
	{
		double dId;
		char acModel[100];

		g_pkScript->GetArgNumber(pkLua, 0, &dId);
		g_pkScript->GetArg(pkLua, 1, acModel);

		g_pkObjMan->SetZoneModel(acModel,(int)dId);
	}

   return 0;
}

int CreateEntityLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2)
	{
     	char	acType[128];
		g_pkScript->GetArgString(pkLua, 0, acType);

		Vector3 kPos;
		vector<TABLE_DATA> vkData;
		g_pkScript->GetArgTable(pkLua, 2, vkData);
		kPos = Vector3(
			(float) (*(double*) vkData[0].pData),
			(float) (*(double*) vkData[1].pData),
			(float) (*(double*) vkData[2].pData));

		if(Entity* pkEntity = g_pkObjMan->CreateObjectFromScriptInZone(acType,kPos))
		{
			g_pkScript->AddReturnValue( pkLua, pkEntity->GetEntityID() );
			return 1;
		}
		else
			cout<<"Error CreateEntityLua ,while creating entity from script"<<acType<<endl;
	}

	g_pkScript->AddReturnValue( pkLua, -1 );
	return 0;

}

}
