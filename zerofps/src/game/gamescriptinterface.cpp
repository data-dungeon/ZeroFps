// gamescriptinterface.cpp: implementation of the GameScriptInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include "gamescriptinterface.h"
#include "game.h"

extern Game g_kGame;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GameScriptInterface::GameScriptInterface()
{
	
}

GameScriptInterface::~GameScriptInterface()
{

}



OBJECT GameScriptInterface::GetPlayer() 
{
	OBJECT ob = g_kGame.m_pkPlayer->iNetWorkID;
	return ob;
}

static float DistanceBetween (Object* ob1, Object* ob2)
{
    Vector3 kOffset = ob1->GetPos() - ob2->GetPos();
    float fDistance = kOffset.Length();
    return (fDistance < 0.0) ? 0.0 : fDistance;
}

OBJECT GameScriptInterface::GetClosestObject(OBJECT kFocusObj) 
{
	Object* pkObjFrom = GetObject(kFocusObj);

	if(pkObjFrom == NULL)
		return -1;

	Vector3 kFromPos = pkObjFrom->GetPos();
	Object* pkZone = g_kGame.pkLevelMan->GetClosestZone(kFromPos);

	list<Object*> pakAllObjects;
	g_kGame.pkObjectMan->GetAllObjects(&pakAllObjects);

	int iPick = -1;
	float fClosest = 1000000000.0f;

	for(list<Object*>::iterator it = pakAllObjects.begin();
		it != pakAllObjects.end(); it++)
		{
			if((*it) != pkObjFrom && (*it)->GetParent() == pkZone)
			{
				float fDistBetween = DistanceBetween(pkObjFrom,(*it));

				if(fDistBetween < fClosest)
				{
					iPick = (*it)->iNetWorkID;
					fClosest = fDistBetween;
				}
			}
		}

	return iPick;
}

OBJECT GameScriptInterface::GetClosestEnemy(OBJECT kFocusObj) 
{
	return -1;
}

const char* GameScriptInterface::GetObjectType(OBJECT obj) 
{
	return GetObject(obj)->GetType();
}

Object* GameScriptInterface::GetObject(int iNetworkID) 
{
	return g_kGame.pkObjectMan->GetObjectByNetWorkID(iNetworkID);
}

Vector3 GameScriptInterface::GetObjectPos(OBJECT obj)
{
	Object* pkObject = GetObject(obj);
	return pkObject->GetPos(); 
}

void GameScriptInterface::DestroyObject(OBJECT obj)
{
	Object* pkObject = GetObject(obj);
	if(pkObject)
	{
		g_kGame.pkObjectMan->Delete(pkObject);
	}
	else
	{
		cout << "SCRIPT: Failed to destroy object!" << endl;
	}
}

float GameScriptInterface::GetMetersBetween(OBJECT obj1, OBJECT obj2)
{
	Object* ob1 = GetObject(obj1), *ob2 = GetObject(obj2);
	if(ob1 == NULL || ob2 == NULL)
		return -1;

	Vector3 fPos1 = ob1->GetPos(); 
	Vector3 fPos2 = ob2->GetPos(); 

	return Vector3(fPos1-fPos2).Length();
}
