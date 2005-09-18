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

/**	\class SIEntityCreate 
		\ingroup NewSi

	This scriptfunctions are ones that can only be used inside a entitys Create function. 
*/


/** \class SIEntityManger 
 *  \ingroup NewSi
 *  */

namespace ObjectManagerLua
{
ZSSScriptSystem* g_pkScript;
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

void Init(EntityManager* pkObjMan, ZSSScriptSystem* pkScript)
{
	cout << "Add SI: EntityManger" << endl;

	g_pkObjMan = pkObjMan;
	g_pkScript = pkScript;
	
	g_kScriptState.g_iCurrentObjectID = -1;
	g_kScriptState.g_iCurrentPCID     = -1;

	Reset();
		
	// entity management
	pkScript->ExposeFunction("CreateEntity", 			ObjectManagerLua::CreateEntityLua);
	pkScript->ExposeFunction("Delete",  				ObjectManagerLua::DeleteLua);
	pkScript->ExposeFunction("IsEntityValid",  		ObjectManagerLua::IsEntityValid);

	//create
	pkScript->ExposeFunction("InitObject",				ObjectManagerLua::InitObjectLua);
	pkScript->ExposeFunction("InitProperty",		  	ObjectManagerLua::InitPropertyLua);
	pkScript->ExposeFunction("InitParameter",			ObjectManagerLua::InitParameterLua);
	pkScript->ExposeFunction("AttachToParent",		ObjectManagerLua::AttachToParent);
	pkScript->ExposeFunction("SetParentObject",  	ObjectManagerLua::SetParentObjectLua);
	pkScript->ExposeFunction("SetReturnObject",  	ObjectManagerLua::SetReturnObjectLua);
	pkScript->ExposeFunction("HaveRelativOri",		ObjectManagerLua::HaveRelativOriLua);

	// entity orientation
	pkScript->ExposeFunction("DistanceTo",				ObjectManagerLua::DistanceToLua);

	//zone management
	pkScript->ExposeFunction("GetZoneID",				ObjectManagerLua::GetZoneIDLua);
	pkScript->ExposeFunction("GetZoneIDAtPos",		ObjectManagerLua::GetZoneIDAtPosLua);
	pkScript->ExposeFunction("SetZoneModel",			ObjectManagerLua::SetZoneModelLua);

	// Common used functions , used together whit P_ScriptInterface
	pkScript->ExposeFunction("GetSelfID",				ObjectManagerLua::GetSelfIDLua);		
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
     	//char	acType[128];
		string strType;
		g_pkScript->GetArgString(pkLua, 0, strType);

		Vector3 kPos;
		kPos = GetVectorArg(pkLua, 2);

		if(Entity* pkEntity = g_pkObjMan->CreateEntityFromScriptInZone(strType.c_str(),kPos))
		{
			g_pkScript->AddReturnValue( pkLua, pkEntity->GetEntityID() );
			return 1;
		}
		else
			cout<<"Error CreateEntityLua ,while creating entity from script"<<strType<<endl;
	}

	g_pkScript->AddReturnValue( pkLua, -1 );
	return 0;
}

/**	\fn Delete( Entity )
 		\relates SIEntityManger
		\param Id of entity to delete.
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

// Create ******************************************************************************************************
/**	\fn InitObject(ScripName)
 		\relates SIEntityCreate
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
 	\relates SIEntityCreate
   \brief Gives a Entity a property.
   \param PropertyName Name of property to assign to object.

	Note: This can only be called in a entity Create function. 
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
 	\relates SIEntityCreate
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

// 	char acName[50];
	string strName;
	g_pkScript->GetArgString(pkLua, 0, strName);
	
	//char acData[50];
	//g_pkScript->GetArgString(pkLua, 1, (char*)acData);
	string strData;
	g_pkScript->GetArgString(pkLua, 1, strData);
	
	//if(!g_kScriptState.g_pkLastProperty->SetValue((string)acName,(string)acData))
	//	cout<<"Error setting parameter:"<<acName<<" to "<<acData<<endl;

	if(!g_kScriptState.g_pkLastProperty->SetValue(strName, strData))
		cout<<"Error setting parameter:"<<strName<<" to "<<strData<<endl;
	return 0;
}	

/**	\fn AttachToParent()
 	\relates SIEntityCreate
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
 	\relates SIEntityCreate
	\brief Sets the last created object to be the active parent object.
*/
int SetParentObjectLua(lua_State* pkLua)
{
	int iNrArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNrArgs == 2)
	{
		double dParent, dChild;
		g_pkScript->GetArgNumber(pkLua, 0, &dParent);
		g_pkScript->GetArgNumber(pkLua, 1, &dChild);

		Entity* pkParent = g_pkObjMan->GetEntityByID((int)dParent);
		Entity* pkChild = g_pkObjMan->GetEntityByID((int)dChild);

		if ( pkParent && pkChild )
		{
			cout << "parent set" << endl;
			pkChild->SetParent(pkParent);
		}

		return 0;
	}	

	if(!g_kScriptState.g_pkLastObject)
		return 0;
	
	g_kScriptState.g_pkLastParent = g_kScriptState.g_pkLastObject;

	//cout<<"setting parent"<<endl;

	return 1;
}

/**	\fn SetReturnObject(x,y,z)
 	\relates SIEntityCreate
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
 	\relates SIEntityCreate
	\brief Sets the last object to have relative orientation.
*/
int HaveRelativOriLua(lua_State* pkLua)
{
	int iNrArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNrArgs == 1)
	{
		double dID;
		g_pkScript->GetArgNumber(pkLua, 0, &dID);

		Entity* pkEntity = g_pkObjMan->GetEntityByID((int)dID);
		
		if ( pkEntity )
			pkEntity->SetRelativeOri(true);

		return 0;
	}	

	if(g_kScriptState.g_pkLastObject == NULL)
		return 0;
	
	g_kScriptState.g_pkLastObject->SetRelativeOri(true);
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

// zone management ******************************************************************************************************
/**	\fn GetZoneIDAtPos(EntityID)
 	\relates SIEntityManger
	\brief	Returns zone id at entitys position. -1 if outside zones.
*/
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

/**	\fn GetZoneIDAtPos(Position)
 	\relates SIEntityManger
	\brief	Returns zone id at position. -1 if outside zones.
*/
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

/**	\fn SetZoneModel(ZoneId, Modelname)
 	\relates SIEntityManger
	\brief	Returns zone id at position. -1 if outside zones.
*/
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

// Common ******************************************************************************************************
/**	\fn GetSelfID()
 	\relates SIEntityManger
	\brief	Returns entity id for active entity.
*/
int GetSelfIDLua(lua_State* pkLua)
{
	g_pkScript->AddReturnValue(pkLua,g_kScriptState.g_iCurrentObjectID);
	return 1;
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
	
// 	char acEvent[128];
	string strEvent;
	g_pkScript->GetArgString(pkLua, 1, strEvent);

// 	char acEventParam[128];
	string strEventParam;
	g_pkScript->GetArgString(pkLua, 2, strEventParam);

	g_pkObjMan->SendMsg(strEvent, strEventParam, g_kScriptState.g_iCurrentObjectID, iTargetEntity);
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
