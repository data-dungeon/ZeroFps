#include "si_mistland.h"
#include <cmath>                    // for trigonometry functions

ZFScriptSystem*	MistLandLua::g_pkScript;
ObjectManager*		MistLandLua::g_pkObjMan;
int					MistLandLua::g_iCurrentObjectID;

void MistLandLua::Init(ObjectManager* pkObjMan,ZFScriptSystem* pkScript)
{
	g_pkObjMan = pkObjMan;
	g_pkScript = pkScript;
	g_iCurrentObjectID = -1;
	
	pkScript->ExposeFunction("GetSelfID",	MistLandLua::GetSelfIDLua);	
	pkScript->ExposeFunction("GetClosestObject",	MistLandLua::GetClosestObjectLua);		
	pkScript->ExposeFunction("RemoveObject",	MistLandLua::RemoveObjectLua);		

}


int MistLandLua::GetSelfIDLua(lua_State* pkLua)
{
	g_pkScript->AddReturnValue(pkLua,g_iCurrentObjectID);
	
	return 1;
}


int MistLandLua::RemoveObjectLua(lua_State* pkLua)
{
	int iId = g_iCurrentObjectID;
	
	//get id
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		iId = (int)dTemp;
	}
	
	//get object
	Object*	pkObj = g_pkObjMan->GetObjectByNetWorkID(iId);
	
	if(pkObj)
		g_pkObjMan->Delete(pkObj);
	
	return 0;
}

int MistLandLua::GetClosestObjectLua(lua_State* pkLua)
{
	int iThisId = g_iCurrentObjectID;

	//if there is a argument us it as thisid	
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		iThisId = (int)dTemp;
	}

	Object* pkME = g_pkObjMan->GetObjectByNetWorkID(iThisId);

	//check that the object is valid
	if(!pkME)
		return 0;
		
	Vector3 kThisPos = pkME->GetWorldPosV();


	vector<Object*> kObjects;
	g_pkObjMan->GetAllObjects(&kObjects);
	
	
	
	float closest = 9999999999;
	Object* pkClosest = NULL;
	
	for(int i=0;i<kObjects.size();i++)
	{
		//dont compare to ur self or object 0
		if(kObjects[i]->iNetWorkID == iThisId || kObjects[i]->iNetWorkID == 0)
			continue;
			
		float d = (kThisPos - kObjects[i]->GetWorldPosV()).Length();
	
		if(d < closest)
		{
			closest = d;
			pkClosest = kObjects[i];
		}
	}
	
	
	if(pkClosest)
	{
		g_pkScript->AddReturnValue(pkLua,pkClosest->iNetWorkID);
		return 1;
	}
		
	g_pkScript->AddReturnValue(pkLua,-1);		
	
	return 0;
}

