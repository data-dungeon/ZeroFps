#include "objectmanager_scriptinterface.h"


void ObjectManagerLua::Init(ObjectManager* pkObjMan, ZFScript* pkScript)
{
	g_pkObjMan = pkObjMan;
	g_pkScript = pkScript;
	
	Reset();
	
	pkScript->ExposeFunction("InitObject",				ObjectManagerLua::InitObjectLua);
	pkScript->ExposeFunction("InitProperty",	  	ObjectManagerLua::InitPropertyLua);
	pkScript->ExposeFunction("InitParameter",			ObjectManagerLua::InitParameterLua);
	pkScript->ExposeFunction("AttachToClosestZone", ObjectManagerLua::AttachToClosestZoneLua);
	pkScript->ExposeFunction("AttachToParent",		ObjectManagerLua::AttachToParent);			
	pkScript->ExposeFunction("SetLocalPos",			ObjectManagerLua::SetLocalPosLua);
	pkScript->ExposeFunction("HaveRelativOri",		ObjectManagerLua::HaveRelativOriLua);	
	pkScript->ExposeFunction("SetParentObject",  	ObjectManagerLua::SetParentObjectLua);
	pkScript->ExposeFunction("SetReturnObject",  	ObjectManagerLua::SetReturnObjectLua);

}

void ObjectManagerLua::Reset()
{
	g_pkLastObject =		NULL;
	g_pkLastProperty =	NULL;
	g_pkLastParent =		NULL;
	g_pkReturnObject = 	NULL;
}

int ObjectManagerLua::InitObjectLua(lua_State* pkLua)
{
	g_pkLastObject = g_pkObjMan->CreateObject();
	
	//cout<<"Creating object in script"<<endl;
	
	return 1;
}	

int ObjectManagerLua::SetParentObjectLua(lua_State* pkLua)
{
	if(!g_pkLastObject)
		return 0;
	
	g_pkLastParent = g_pkLastObject;

	//cout<<"setting parent"<<endl;

	return 1;
}
int ObjectManagerLua::AttachToParent(lua_State* pkLua)
{
	if(!g_pkLastObject)
		return 0;
		
	if(!g_pkLastParent)
		return 0;

	g_pkLastObject->SetParent(g_pkLastParent);
	
	//cout<<"Attaching object to parent"<<endl;
	
	return 1;
}	

int ObjectManagerLua::InitPropertyLua(lua_State* pkLua)
{
	if(g_pkLastObject == NULL)
		return 0;
	
	if(g_pkScript->GetNumArgs(pkLua) != 1)
		return 0;

	char acName[100];
	g_pkScript->GetArg(pkLua, 0, acName);

	g_pkLastProperty = g_pkLastObject->AddProperty(acName);

	//cout<<"Creating property in script: "<<acName<<endl;
	
	return 1;
}	

int ObjectManagerLua::InitParameterLua(lua_State* pkLua)
{
	if(g_pkLastProperty == NULL)
		return 0;
	
	if(g_pkScript->GetNumArgs(pkLua) != 2)
		return 0;

	char acName[50];
	g_pkScript->GetArg(pkLua, 0, acName);
	
	char acData[50];
	g_pkScript->GetArgString(pkLua, 1, (char*)acData);
	


	g_pkLastProperty->SetValue((string)acName,(string)acData);

	
	//cout<<"Setting Parameter in script: "<<acName<<" to "<<acData<<endl;
	
	return 0;
}	

int ObjectManagerLua::AttachToClosestZoneLua(lua_State* pkLua)
{
	if(g_pkLastObject == NULL)
		return 0;
	
	g_pkLastObject->AttachToClosestZone();

	//cout<<"Attaching object to zone"<<endl;
	
	return 0;
}	

int ObjectManagerLua::SetLocalPosLua(lua_State* pkLua)
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
	
	//cout<<"Setting pos to "<<x<<" "<<y<<" "<<z<<endl;
	return 0;
}

int ObjectManagerLua::HaveRelativOriLua(lua_State* pkLua)
{
	if(g_pkLastObject == NULL)
		return 0;

	
	g_pkLastObject->SetRelativeOri(true);

	return 0;
}

int ObjectManagerLua::SetReturnObjectLua(lua_State* pkLua)
{
	if(g_pkLastObject == NULL)
		return 0;
	
	g_pkReturnObject = g_pkLastObject;

	return 0;
}
