#include "../../script/zfscript.h"
#include "si_objectmanager.h"
#include "../../engine/entitymanager.h"
#include "../../engine_systems/propertys/p_mad.h"
#include "../../script/zfscript.h"
#include "../propertys/p_tcs.h"
#include "../propertys/p_scriptinterface.h"
#include "../script_interfaces/si_objectmanager.h" 
#include <stack> 

using namespace ObjectManagerLua;

/**	\brief Class To Collect Script Functions,
	\ingroup si
*/
class SIEntityManger { };



namespace ObjectManagerLua
{
ZFScriptSystem* g_pkScript;
EntityManager*  g_pkObjMan;

ScriptState				g_kScriptState;
stack<ScriptState>	g_kScriptStateStack;

Vector3 GetVectorArg(lua_State* pkLua, int iIndex)
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

void Init(EntityManager* pkObjMan, ZFScriptSystem* pkScript)
{
	cout << "Add SI: EntityManger" << endl;

	g_pkObjMan = pkObjMan;
	g_pkScript = pkScript;
	
	g_kScriptState.g_iCurrentObjectID = -1;
	g_kScriptState.g_iCurrentPCID     = -1;

	Reset();
		
	//create
	pkScript->ExposeFunction("InitObject",				ObjectManagerLua::InitObjectLua);
	pkScript->ExposeFunction("InitProperty",		  	ObjectManagerLua::InitPropertyLua);
	pkScript->ExposeFunction("InitParameter",			ObjectManagerLua::InitParameterLua);
	pkScript->ExposeFunction("AttachToParent",		ObjectManagerLua::AttachToParent);
	pkScript->ExposeFunction("HaveRelativOri",		ObjectManagerLua::HaveRelativOriLua);
	pkScript->ExposeFunction("SetParentObject",  	ObjectManagerLua::SetParentObjectLua);
	pkScript->ExposeFunction("SetReturnObject",  	ObjectManagerLua::SetReturnObjectLua);
	//----

	// entity management
	pkScript->ExposeFunction("CreateEntity", 			ObjectManagerLua::CreateEntityLua);
	pkScript->ExposeFunction("Delete",  				ObjectManagerLua::DeleteLua);
	pkScript->ExposeFunction("IsEntityValid",  		ObjectManagerLua::IsEntityValid);

	// entity orientation
	pkScript->ExposeFunction("DistanceTo",				ObjectManagerLua::DistanceToLua);

	//zone management
	pkScript->ExposeFunction("GetZoneIDAtPos",		ObjectManagerLua::GetZoneIDAtPosLua);
	pkScript->ExposeFunction("GetZoneID",				ObjectManagerLua::GetZoneIDLua);
	pkScript->ExposeFunction("SetZoneModel",			ObjectManagerLua::SetZoneModelLua);

	// Common used functions , used together whit P_ScriptInterface
	pkScript->ExposeFunction("SIGetSelfID",			ObjectManagerLua::SIGetSelfIDLua);		
	pkScript->ExposeFunction("GetSelfID",				ObjectManagerLua::SIGetSelfIDLua);		
	pkScript->ExposeFunction("SendEvent",				ObjectManagerLua::SendEventLua);		
}

void Reset()
{
	g_kScriptState.g_pkLastObject			= NULL;
	g_kScriptState.g_pkLastParent			= NULL;
	g_kScriptState.g_pkLastProperty		= NULL;
	g_kScriptState.g_pkReturnObject		= NULL;
	g_kScriptState.g_iCurrentObjectID	= -1;
	g_kScriptState.g_iCurrentPCID			= -1;
}

void Push()
{
	g_kScriptStateStack.push( g_kScriptState );	
	Reset();
}

void Pop()
{
	ZFAssert( !g_kScriptStateStack.empty(), "Trying to Pop a empty Script State");

	g_kScriptState = g_kScriptStateStack.top();
	g_kScriptStateStack.pop();
}

/**	\fn CreateEntity( Template, Position)
 		\relates SIEntityManger
		\param Template Name of template script used to create entity.
		\param Position Position to create new entity at.
		\return Return id of new entity or -1 if no one was created.
		\brief Create the entity given as a parameter.
*/
int CreateEntityLua (lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 2)
	{
     	char	acType[128];
		g_pkScript->GetArgString(pkLua, 0, acType);

		Vector3 kPos;
		kPos = GetVectorArg(pkLua, 2);

		if(Entity* pkEntity = g_pkObjMan->CreateEntityFromScriptInZone(acType,kPos))
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

/**	\fn Delete( Entity )
 		\relates SIEntityManger
		\param Entity of entity to delete.
		\brief Delete a entity.
*/
int DeleteLua(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua, 1))
		return 0;

	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
	
	Entity* pkObject = g_pkObjMan->GetEntityByID((int)dTemp);	
	if(pkObject)
		g_pkObjMan->Delete(pkObject);
	return 0;
}

/**	\fn IsEntityValid( Entity )
 		\relates SIEntityManger
		\param EntityID to check.
		\brief Returns 1 if entity exist/is loaded and 0 if not.
*/
int ENGINE_SYSTEMS_API IsEntityValid(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua, 1))
		return 0;

	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
	
	Entity* pkObject = g_pkObjMan->GetEntityByID((int)dTemp);	

	int iRetValue = 0;

	if(pkObject)
		iRetValue = 1;
   
	g_pkScript->AddReturnValue( pkLua, iRetValue );
	
	return 1;
}


/**	\fn InitObject(ScripName)
 	\relates SIEntityManger
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
		
		g_kScriptState.g_pkLastObject = g_pkObjMan->CreateEntityFromScript(acName);
		g_pkScript->AddReturnValue( pkLua, g_kScriptState.g_pkLastObject->GetEntityID() );
		
		//set return object of there is none
		if(!g_kScriptState.g_pkReturnObject)
		{
			g_kScriptState.g_pkReturnObject = g_kScriptState.g_pkLastObject;
			g_kScriptState.g_iCurrentObjectID = g_kScriptState.g_pkReturnObject->GetEntityID();
		}
			
		return 1;
	}
		
	//else create an empty object
	g_kScriptState.g_pkLastObject = g_pkObjMan->CreateEntity();
	g_pkScript->AddReturnValue( pkLua, g_kScriptState.g_pkLastObject->GetEntityID() );
	
	//set return object of there is none	
	if(!g_kScriptState.g_pkReturnObject)
	{
		g_kScriptState.g_pkReturnObject = g_kScriptState.g_pkLastObject;
		g_kScriptState.g_iCurrentObjectID = g_kScriptState.g_pkReturnObject->GetEntityID();
	}	
		
		
	return 1;
}	

/**	\fn InitProperty(PropertyName)
 	\relates SIEntityManger
   \brief Gives a Entity a property.
   \param PropertyName Name of property to assign to object.

	Assigns the property with the def values to the last created object.	
*/
int InitPropertyLua(lua_State* pkLua)
{
	if(g_kScriptState.g_pkLastObject == NULL)
		return 0;
	
	if(!g_pkScript->VerifyArg(pkLua, 1))
		return 0;

	char acName[100];
	g_pkScript->GetArg(pkLua, 0, acName);

	g_kScriptState.g_pkLastProperty = g_kScriptState.g_pkLastObject->AddProperty(acName);
	return 1;
}		


/**	\fn InitParameter(szName, szValue )
 	\relates SIEntityManger
   \brief Sets the value of a variable in a property.
   \param szName Name of variable to set.
   \param szValue Value to set variable to.

	Sets the variable to a given value on the last created property.	
*/
int InitParameterLua(lua_State* pkLua)
{
	if(g_kScriptState.g_pkLastProperty == NULL)
		return 0;
	
	if(!g_pkScript->VerifyArg(pkLua, 2))
		return 0;

	char acName[50];
	g_pkScript->GetArg(pkLua, 0, acName);
	
	char acData[50];
	g_pkScript->GetArgString(pkLua, 1, (char*)acData);
	
	if(!g_kScriptState.g_pkLastProperty->SetValue((string)acName,(string)acData))
		cout<<"Error setting parameter:"<<acName<<" to "<<acData<<endl;
	return 0;
}	

/**	\fn AttachToParent()
 	\relates SIEntityManger
	\brief Sets the last created object to be a child to the active parent object,
*/
int AttachToParent(lua_State* pkLua)
{
	if(!g_kScriptState.g_pkLastObject)
		return 0;
		
	if(!g_kScriptState.g_pkLastParent)
		return 0;

	g_kScriptState.g_pkLastObject->SetParent(g_kScriptState.g_pkLastParent);
	
	//cout<<"Attaching object to parent"<<endl;
	
	return 1;
}	

/**	\fn SetParentObject()
 	\relates SIEntityManger
	\brief Sets the last created object to be the active parent object.
*/
int SetParentObjectLua(lua_State* pkLua)
{
	if(!g_kScriptState.g_pkLastObject)
		return 0;
	
	g_kScriptState.g_pkLastParent = g_kScriptState.g_pkLastObject;

	//cout<<"setting parent"<<endl;

	return 1;
}

/**	\fn SetReturnObject(x,y,z)
 	\relates SIEntityManger
	\brief Sets the last object to be the object to be returned as the new created object.
*/
int SetReturnObjectLua(lua_State* pkLua)
{
	if(g_kScriptState.g_pkLastObject == NULL)
		return 0;
	
	g_kScriptState.g_pkReturnObject = g_kScriptState.g_pkLastObject;
	g_kScriptState.g_iCurrentObjectID = g_kScriptState.g_pkReturnObject->GetEntityID();

	return 0;
}
/**	\fn HaveRelativOri()
 	\relates SIEntityManger
	\brief Sets the last object to have relative orientation.
*/

int HaveRelativOriLua(lua_State* pkLua)
{
	if(g_kScriptState.g_pkLastObject == NULL)
		return 0;
	
	g_kScriptState.g_pkLastObject->SetRelativeOri(true);
	return 0;
}
//----end of create


int SIGetSelfIDLua(lua_State* pkLua)
{
	g_pkScript->AddReturnValue(pkLua,g_kScriptState.g_iCurrentObjectID);
	return 1;
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

	Entity* pkObject1 = g_pkObjMan->GetEntityByID((int)dID1);
	Entity* pkObject2 = g_pkObjMan->GetEntityByID((int)dID2);

	if( pkObject1 != 0 && pkObject2!= 0 )
	{
		double dDistance = pkObject1->GetWorldPosV().DistanceTo(pkObject2->GetWorldPosV());

		cout << "mAgPk:" << pkObject2 << endl;
		cout << "x1:" << pkObject1->GetWorldPosV().x << " z1:" << pkObject1->GetWorldPosV().z << endl;
		cout << "x2:" << pkObject2->GetWorldPosV().x << " z2:" << pkObject2->GetWorldPosV().z << endl;
		cout << "dist:" << dDistance << endl;
		g_pkScript->AddReturnValue(pkLua, dDistance);

		return 1;
	}

	g_pkScript->AddReturnValue(pkLua, 999999);

	return 1;
}



int GetZoneIDLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dId;

		g_pkScript->GetArgNumber(pkLua, 0, &dId);

		if(Entity* pkEnt = g_pkObjMan->GetEntityByID((int)dId))
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
		//vector<TABLE_DATA> vkData;
		//g_pkScript->GetArgTable(pkLua, 1, vkData);
		/*kPos = Vector3(
			(float) (*(double*) vkData[0].pData),
			(float) (*(double*) vkData[1].pData),
			(float) (*(double*) vkData[2].pData));*/
		kPos = GetVectorArg(pkLua, 1);

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


/**	\fn SendEvent(EntityID, Name)
 	\relates SIEntityManger
	\brief	Sends the event Name to the choosen entity. Will call entity script function onName for the choosen
	entity.
*/
int SendEventLua(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua, 2))
		return 0;

	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
	int iTargetEntity = (int)dTemp;
	
	char acEvent[128];
	g_pkScript->GetArgString(pkLua, 1, acEvent);

	char acEventParam[128];
	g_pkScript->GetArgString(pkLua, 2, acEventParam);

	g_pkObjMan->SendMsg(string(acEvent), string(acEventParam), g_kScriptState.g_iCurrentObjectID, iTargetEntity);
	return 0;
}

Entity* GetEntityPtr(lua_State* pkLua, int iIndex)
{
	// Get EntityID
	int iEntityID;
	g_pkScript->GetArgInt(pkLua, iIndex, &iEntityID);

	// Get Entity Ptr
	Entity* pkEnt = g_pkObjMan->GetEntityByID(iEntityID);

	if(pkEnt == NULL)
	{
		g_pkScript->Error(pkLua, "Warning: Non valid Entity %d ", iEntityID);
	}

	return pkEnt;
}

}
