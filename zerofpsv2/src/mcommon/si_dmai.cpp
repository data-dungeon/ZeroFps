#include "si_dm.h"
#include "../zerofpsv2/engine/p_pfpath.h" 
#include "p_dmcharacter.h"

int DMLua::HavePathLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dId;
		
		g_pkScript->GetArgNumber(pkLua, 0, &dId);				
		
		Entity* pkEnt = g_pkObjMan->GetObjectByNetWorkID((int)dId);
		if(pkEnt)
		{
			P_PfPath* pf = (P_PfPath*)pkEnt->GetProperty("P_PfPath");
			if(pf)
				if(pf->HavePath())
				{
					g_pkScript->AddReturnValue(pkLua,1);
					return 1;		
				}
		}
		
	}

	g_pkScript->AddReturnValue(pkLua,0);
	return 1;
}

// ------------------------------------------------------------------------------------------------

int DMLua::MakePathFindLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 2)
	{
		double dId;
		Vector3 kPos;		
		vector<TABLE_DATA> vkData;
		
		g_pkScript->GetArgNumber(pkLua, 0, &dId);				
		g_pkScript->GetArgTable(pkLua, 2, vkData);

		kPos = Vector3(
			(float) (*(double*) vkData[0].pData),
			(float) (*(double*) vkData[1].pData),
			(float) (*(double*) vkData[2].pData)); 
		
		Entity* pkEnt = g_pkObjMan->GetObjectByNetWorkID((int)dId);
		if(pkEnt)
		{
			P_PfPath* pf = (P_PfPath*)pkEnt->GetProperty("P_PfPath");
			if(pf)
			{
				double dSuccess = pf->MakePathFind(kPos);

				g_pkScript->AddReturnValue(pkLua,dSuccess);

				return 1;
			}
		}
	}
	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::ClearPathFindLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dId;
		
		g_pkScript->GetArgNumber(pkLua, 0, &dId);				

		Entity* pkEnt = g_pkObjMan->GetObjectByNetWorkID((int)dId);
		if(pkEnt)
		{
			P_PfPath* pf = (P_PfPath*)pkEnt->GetProperty("P_PfPath");
			if(pf)
				pf->ClearPath();
		}
	}

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::SetRunAnimLua(lua_State* pkLua)
{
	Entity* pkObj = TestScriptInput (2, pkLua);

	if ( pkObj == 0 )
	{
		cout << "Warning! DMLua::SetRunAnimLua: Takes two arguments, ID and animationName(str), or objectID not found." << endl;
		return 0;
	}

	char caAnimName[255];
	
	g_pkScript->GetArgString (pkLua, 1, caAnimName);

	if (P_PfPath* pf = (P_PfPath*)pkObj->GetProperty("P_PfPath"))
		pf->SetRunAnim(string(caAnimName));

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::SetIdleAnimLua(lua_State* pkLua)
{
	Entity* pkObj = TestScriptInput (2, pkLua);

	if ( pkObj == 0 )
	{
		cout << "Warning! DMLua::SetIdleAnimLua: Takes two arguments, ID and animationName(str), or objectID not found." << endl;
		return 0;
	}

	char caAnimName[255];
	
	g_pkScript->GetArgString (pkLua, 1, caAnimName);

	if (P_PfPath* pf = (P_PfPath*)pkObj->GetProperty("P_PfPath"))
		pf->SetIdleAnim(string(caAnimName));

	return 0;
}

// ------------------------------------------------------------------------------------------------
//
// Tar 1 argument: Entity ID of patroling object
//

int DMLua::ClearPatrolPathLua(lua_State* pkLua)
{
	double dEntID;
	Vector3 kPatrolPos;

	if( g_pkScript->GetArgNumber(pkLua, 0, &dEntID) )
	{
		map<int, PATROL_POINTS>::iterator it = m_kPatrolPoints.find(int(dEntID));
		if(it != m_kPatrolPoints.end())
		{
			it->second.m_vkPoints.clear();
			it->second.m_iCurrent = 0;
		}
	}

	return 0;
}

// ------------------------------------------------------------------------------------------------
//
// Tar 2 argument: Entity ID of patroling object and a Table with 3 double (x,y,z)
//
int DMLua::AddPatrolPointLua(lua_State* pkLua)
{
	double dEntID;
	Vector3 kPatrolPos;

	if( g_pkScript->GetArgNumber(pkLua, 0, &dEntID) )
	{
		vector<TABLE_DATA> vkData;

		if(g_pkScript->GetArgTable(pkLua, 2, vkData))
		{
			kPatrolPos = Vector3(
				(float) (*(double*) vkData[0].pData),
				(float) (*(double*) vkData[1].pData),
				(float) (*(double*) vkData[2].pData)); 

			map<int, PATROL_POINTS>::iterator it = m_kPatrolPoints.find(int(dEntID));
			if(it != m_kPatrolPoints.end())
			{
				it->second.m_vkPoints.push_back(kPatrolPos);
			}
			else
			{
				PATROL_POINTS kNew;
				kNew.m_iCurrent = 0;
				kNew.m_vkPoints.push_back(kPatrolPos);
				m_kPatrolPoints.insert( map<int,PATROL_POINTS>::value_type(int(dEntID),kNew));
			}
		}
	}

	return 0;
}

// ------------------------------------------------------------------------------------------------
//
// Tar ett objekt ID och kollar om det finns med i listan, flyttar i så fall objektet till
// nästa patrull punkt.
//
int DMLua::PatrolLua(lua_State* pkLua)
{
	double dEntID;

	if( g_pkScript->GetArgNumber(pkLua, 0, &dEntID) )
	{
		map<int, PATROL_POINTS>::iterator it = m_kPatrolPoints.find(int(dEntID));
		
		if(it != m_kPatrolPoints.end())
		{
			if(!it->second.m_vkPoints.empty())
			{
				Entity* pkEnt = g_pkObjMan->GetObjectByNetWorkID(int(dEntID));

				if(pkEnt)
				{
					P_PfPath* pf = (P_PfPath*)pkEnt->GetProperty("P_PfPath");

					if(pf)
					{
						if(pf->HavePath() == false)
						{
							it->second.m_iCurrent++; // öka upp så att vi pathfindar till nästa punkt

							// är detta 1 mer än den sista punkten (dvs == size på vektorn)
							// så pathfindar vi tillbaks till punkt 0
							if(it->second.m_iCurrent == (int) it->second.m_vkPoints.size())
								it->second.m_iCurrent = 0;

							// gör pathfind till nästa punkt
							pf->MakePathFind(it->second.m_vkPoints[it->second.m_iCurrent]);

							//printf("Step %i. Moving to (%i,%i,%i)\n", 
							//	it->second.m_iCurrent,
							//	(int)it->second.m_vkPoints[it->second.m_iCurrent].x,
							//	(int)it->second.m_vkPoints[it->second.m_iCurrent].y,
							//	(int)it->second.m_vkPoints[it->second.m_iCurrent].z);
						}
					}
				}
			}
		}
	}

	return 0;
}

// ------------------------------------------------------------------------------------------------

int DMLua::HaveOrdersLua(lua_State* pkLua)
{
	int iHasOrders = 0;
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dId;
		
		g_pkScript->GetArgNumber(pkLua, 0, &dId);				
		
		Entity* pkEnt = g_pkObjMan->GetObjectByNetWorkID((int)dId);
		if(pkEnt)
			if (P_DMCharacter* pkDMChar = (P_DMCharacter*)pkEnt->GetProperty("P_DMCharacter"))
				iHasOrders = pkDMChar->HasOrders();
		
	}

	g_pkScript->AddReturnValue(pkLua, iHasOrders);
	return 1;
}

// ------------------------------------------------------------------------------------------------







