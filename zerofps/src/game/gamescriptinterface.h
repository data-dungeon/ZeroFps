// gamescriptinterface.h: interface for the GameScriptInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMESCRIPTINTERFACE_H__23C97A7A_5814_474B_A338_FFB51FB8824E__INCLUDED_)
#define AFX_GAMESCRIPTINTERFACE_H__23C97A7A_5814_474B_A338_FFB51FB8824E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum OBJECT_TYPE
{
	ObjTypeSelf,		
	ObjTypeCreature,
	ObjTypeDoor,
	ObjTypeItem,
	ObjTypeNone
};

typedef int OBJECT; // same as network id

class GameScriptInterface  
{
public:
	GameScriptInterface();
	virtual ~GameScriptInterface();

	/// Recives the first-person-player for the client.
	OBJECT GetPlayer();

	/// Recives the closes hostile object from kFocusObj or -1 if no object exist,
	/// or if kFocusObj doesn´t exist.
	OBJECT GetClosestEnemy(OBJECT kFocusObj);

	/// Recives the type of obj (or -1 if obj doesn´t exist)
	OBJECT_TYPE GetObjectType(OBJECT obj);

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
	//ObjectManager* m_pkObjMan;
	Object* GetObject(int iNetworkID);

};

#endif // !defined(AFX_GAMESCRIPTINTERFACE_H__23C97A7A_5814_474B_A338_FFB51FB8824E__INCLUDED_)
