#include "../../script/zfscript.h"
#include "si_objectmanager.h"
#include "../../engine/objectmanager.h"
#include "../../engine_systems/propertys/madproperty.h"
#include "../../script/zfscript.h"

ZFScriptSystem* ObjectManagerLua::g_pkScript;
ObjectManager*  ObjectManagerLua::g_pkObjMan;

Object*			 ObjectManagerLua::g_pkLastObject;
Object*			 ObjectManagerLua::g_pkLastParent;
Property*		 ObjectManagerLua::g_pkLastProperty;
Object*			 ObjectManagerLua::g_pkReturnObject;

//Object*			 ObjectManagerLua::g_pkLastObjectBak;
Object*			 ObjectManagerLua::g_pkLastParentBak;
Property*		 ObjectManagerLua::g_pkLastPropertyBak;
Object*			 ObjectManagerLua::g_pkReturnObjectBak;


void ObjectManagerLua::Init(ObjectManager* pkObjMan, ZFScriptSystem* pkScript)
{
	g_pkObjMan = pkObjMan;
	g_pkScript = pkScript;
	
	Reset();
	
	pkScript->ExposeFunction("InitObject",				ObjectManagerLua::InitObjectLua);
	pkScript->ExposeFunction("InitProperty",	  	ObjectManagerLua::InitPropertyLua);
	pkScript->ExposeFunction("InitParameter",			ObjectManagerLua::InitParameterLua);
	pkScript->ExposeFunction("AttachToParent",		ObjectManagerLua::AttachToParent);			
	pkScript->ExposeFunction("SetLocalPos",			ObjectManagerLua::SetLocalPosLua);
	pkScript->ExposeFunction("HaveRelativOri",		ObjectManagerLua::HaveRelativOriLua);	
	pkScript->ExposeFunction("SetParentObject",  	ObjectManagerLua::SetParentObjectLua);
	pkScript->ExposeFunction("SetReturnObject",  	ObjectManagerLua::SetReturnObjectLua);

	pkScript->ExposeFunction("PlayAnim",				ObjectManagerLua::PlayAnim);
	pkScript->ExposeFunction("SetNextAnim",			ObjectManagerLua::SetNextAnim);

}

void ObjectManagerLua::Reset()
{
	g_pkLastObject   = NULL;
	g_pkLastProperty = NULL;
	g_pkLastParent   = NULL;
	g_pkReturnObject = NULL;
}

void ObjectManagerLua::Push()
{
	//g_pkLastObjectBak   = g_pkLastObject;
	g_pkLastPropertyBak = g_pkLastProperty;
	g_pkLastParentBak   = g_pkLastParent;
	g_pkReturnObjectBak = g_pkReturnObject;
	
	Reset();
}

void ObjectManagerLua::Pop()
{
	//g_pkLastObject   = g_pkLastObjectBak;
	g_pkLastProperty = g_pkLastPropertyBak;
	g_pkLastParent   = g_pkLastParentBak;
	g_pkReturnObject = g_pkReturnObjectBak;
}

int ObjectManagerLua::InitObjectLua(lua_State* pkLua)
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

int ObjectManagerLua::PlayAnim(lua_State* pkLua)
{
	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iId1 = (int)dTemp;

	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);

//	printf("Should Play A Animation '%s' on object %d", acName,  iId1);

	Object* o1 = g_pkObjMan->GetObjectByNetWorkID(iId1);
	P_Mad* mp = dynamic_cast<P_Mad*>(o1->GetProperty("P_Mad"));
	mp->PlayAnimation(acName,0);
	
	return 1;
}

int ObjectManagerLua::SetNextAnim(lua_State* pkLua)
{
	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iId1 = (int)dTemp;

	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);
//	printf("Next Anim to play is '%s' on object %d", acName,  iId1);

	Object* o1 = g_pkObjMan->GetObjectByNetWorkID(iId1);
	P_Mad* mp = dynamic_cast<P_Mad*>(o1->GetProperty("P_Mad"));
	mp->SetNextAnimation(acName);
	return 1;
}

