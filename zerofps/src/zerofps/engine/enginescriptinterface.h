// enginescriptinterface.h: interface for the EngineScriptInterface class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _ENGINE_ENGINESCRIPTINTERFACE
#define _ENGINE_ENGINESCRIPTINTERFACE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "engine_x.h"
#include "engine.pkg"
#include "../basic/basic.pkg"

enum OBJECT_TYPE
{
	ObjTypeSelf,		
	ObjTypeCreature,
	ObjTypeDoor,
	ObjTypeItem,
	ObjTypeNone
};

typedef int OBJECT; // same as network id

class ENGINE_API EngineScriptInterface : public ZFObject 
{

// Export functions:
public:
	EngineScriptInterface();
	virtual ~EngineScriptInterface();

	/// Recives the position for the object.
	Vector3 GetObjectPos(OBJECT obj);
	
	/// Recives the first-person-player for the client.
	OBJECT GetPlayer();

	/// Recives the closes hostile object from kFocusObj or -1 if no object exist.
	/// or kFocusObj doesn´t exist.
	OBJECT GetClosestEnemy(OBJECT kFocusObj);

	/// Recives the type of obj (or -1 if obj doesn´t exist)
	OBJECT_TYPE GetObjectType(OBJECT obj);

	void DestroyObject(OBJECT obj);

// Internal functions:
private:
	ObjectManager* m_pkObjMan;
	Object* GetObject(int iNetworkID);
};

#endif // #ifndef _ENGINE_ENGINESCRIPTINTERFACE
