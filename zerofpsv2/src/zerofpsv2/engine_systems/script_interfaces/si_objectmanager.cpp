#include "../../script/zfscript.h"
#include "si_objectmanager.h"
#include "../../engine/entitymanager.h"
#include "../../engine_systems/propertys/p_mad.h"
#include "../../script/zfscript.h"

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

void Init(EntityManager* pkObjMan, ZFScriptSystem* pkScript)
{
	g_pkObjMan = pkObjMan;
	g_pkScript = pkScript;
	
	
	Reset();
		
	//create
	pkScript->ExposeFunction("InitObject",				ObjectManagerLua::InitObjectLua);
	pkScript->ExposeFunction("InitProperty",	  		ObjectManagerLua::InitPropertyLua);
	pkScript->ExposeFunction("InitParameter",			ObjectManagerLua::InitParameterLua);
	pkScript->ExposeFunction("AttachToParent",		ObjectManagerLua::AttachToParent);			
	pkScript->ExposeFunction("SetLocalPos",			ObjectManagerLua::SetLocalPosLua);
	pkScript->ExposeFunction("HaveRelativOri",		ObjectManagerLua::HaveRelativOriLua);	
	pkScript->ExposeFunction("IsStatic",				ObjectManagerLua::IsStaticLua);		
	pkScript->ExposeFunction("SetParentObject",  	ObjectManagerLua::SetParentObjectLua);
	pkScript->ExposeFunction("SetReturnObject",  	ObjectManagerLua::SetReturnObjectLua);
	//----
	
	pkScript->ExposeFunction("Delete",  				ObjectManagerLua::DeleteLua);

	pkScript->ExposeFunction("PlayAnim",				ObjectManagerLua::PlayAnim);
	pkScript->ExposeFunction("SetNextAnim",			ObjectManagerLua::SetNextAnim);
	pkScript->ExposeFunction("AddMesh",					ObjectManagerLua::AddMesh);

	// Object Variables
	pkScript->ExposeFunction("GetLocalDouble",		ObjectManagerLua::GetLocalDouble);
	pkScript->ExposeFunction("SetLocalDouble",		ObjectManagerLua::SetLocalDouble);

	// object orientation
	pkScript->ExposeFunction("GetObjectPos",			ObjectManagerLua::GetObjectPosLua);
	pkScript->ExposeFunction("SetObjectPos",			ObjectManagerLua::SetObjectPosLua);
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
	
	g_pkLastObject->SetLocalPosV(Vector3(x,y,z));
	
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

/**	\fn IsStatic()
 	\relates MistLandScript
	\brief Sets the last created object to be static.
*/
int IsStaticLua(lua_State* pkLua)
{
	if(g_pkLastObject == NULL)
		return 0;

	
	g_pkLastObject->GetObjectType() = OBJECT_TYPE_STATIC;

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

/**	\page AddMesh AddMesh(ObjectID, MeshID)

	Adds a mesh to be rendered on the object.

   \param	ObjectID	ID of object to add mesh to.
   \param	MeshID	ID of mesh to add.
*/
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
	float fValue = dTemp;

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


}
