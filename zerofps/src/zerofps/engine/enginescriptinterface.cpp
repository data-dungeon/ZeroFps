// enginescriptinterface.cpp: implementation of the EngineScriptInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "enginescriptinterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EngineScriptInterface::EngineScriptInterface() : ZFObject("EngineScriptInterface")
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
	return -1;
}

OBJECT EngineScriptInterface::GetClosestEnemy(OBJECT kFocusObj) 
{
	return -1;
}

void EngineScriptInterface::DestroyObject(OBJECT obj)
{
	Object* pkObject = GetObject(obj);
	if(pkObject)
	{
		m_pkObjMan->Delete(pkObject);
	}
}