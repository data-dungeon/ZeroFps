// enginescriptinterface.cpp: implementation of the EngineScriptInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "engine.pkg"
#include "../basic/basic.pkg"
#include "enginescriptinterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EngineScriptInterface::EngineScriptInterface()
{
	m_pkObjMan = static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));
}

EngineScriptInterface::~EngineScriptInterface()
{

}

Object* EngineScriptInterface::GetObject(int iNetworkID) 
{
	return m_pkObjMan->GetObjectByNetWorkID(iNetworkID);
}

Vector3 EngineScriptInterface::GetObjectPos(OBJECT obj)
{
	Object* pkObject = GetObject(obj);
	return pkObject->GetPos(); 
}

OBJECT_TYPE EngineScriptInterface::GetObjectType(OBJECT obj) 
{
	return ObjTypeNone;
}

OBJECT EngineScriptInterface::GetPlayer() 
{
	printf("apa apa apa apa apa apa apa apa apa\n");
	return -1;
}

OBJECT EngineScriptInterface::GetClosestEnemy(OBJECT kFocusObj) 
{
	return -1;
}

OBJECT EngineScriptInterface::GetClosestObject(OBJECT kFocusObj) 
{
	return -1;
}

void EngineScriptInterface::DestroyObject(int obj)
{
/*	Object* pkObject = GetObject(obj);
	if(pkObject)
	{
		m_pkObjMan->Delete(pkObject);
	}*/
}

float EngineScriptInterface::GetMetersBetween(OBJECT obj1, OBJECT obj2)
{
	Object* ob1 = GetObject(obj1), *ob2 = GetObject(obj2);
	if(ob1 == NULL || ob2 == NULL)
		return -1;

	Vector3 fPos1 = ob1->GetPos(); 
	Vector3 fPos2 = ob2->GetPos(); 

	return Vector3(fPos1-fPos2).Length();
}