// enginescriptinterface.h: interface for the EngineScriptInterface class.
//
//////////////////////////////////////////////////////////////////////
/*
#ifndef _ENGINE_ENGINESCRIPTINTERFACE
#define _ENGINE_ENGINESCRIPTINTERFACE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "engine_x.h"

class Object;

typedef int OBJECT; // same as network id

class ENGINE_API EngineScriptInterface
{

// Export functions:
public:
	EngineScriptInterface();
	virtual ~EngineScriptInterface();

	/// Recives the position for the object.
	Vector3 GetObjectPos(OBJECT obj);
	
	/// Recives the closes object from kFocusObj or -1 if no object exist,
	/// or if kFocusObj doesn´t exist.
	OBJECT GetClosestObject(OBJECT kFocusObj);

	/// Recives rhe distance in meters between two objects or -1 if either of the 
	/// two objects doesn´t exist.
	float GetMetersBetween(OBJECT obj1, OBJECT obj2);

	void DestroyObject(OBJECT obj);

// Internal functions:
private:
	ObjectManager* m_pkObjMan;
	Object* GetObject(int iNetworkID);
};

#endif // #ifndef _ENGINE_ENGINESCRIPTINTERFACE*/
