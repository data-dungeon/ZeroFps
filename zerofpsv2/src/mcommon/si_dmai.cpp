#include "si_dm.h"
#include "../zerofpsv2/engine/p_pfpath.h" 

int DMLua::HavePathLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dId;
		int ret = 0;
		
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
				pf->MakePathFind(kPos);
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
// Tar ett objekt ID och kollar om det finns med i listan, flyttar i s� fall objektet till
// n�sta patrull punkt.
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
							int* curr_pos = &(it->second.m_iCurrent);
							pf->MakePathFind(it->second.m_vkPoints[*curr_pos]);
							
							*curr_pos = *curr_pos +1;
							if(*curr_pos > it->second.m_vkPoints.size()-1)
							{
								*curr_pos = 0;
							}
						}
					}
				}
			}
		}
	}

	return 0;
}