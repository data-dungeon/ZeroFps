// p_unitmoveai.cpp: implementation of the p_unitmoveai class.
//
//////////////////////////////////////////////////////////////////////

#include "p_unitmoveai.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HeightMap* P_UnitMoveAI::m_pkMap =NULL;

P_UnitMoveAI::P_UnitMoveAI() :m_pkMoveUnitCommand(NULL),m_pkUnit(NULL), m_bTemp(false) 
{
	strcpy(m_acName,"P_UnitMoveAI");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	
}

P_UnitMoveAI::~P_UnitMoveAI()
{
	delete m_pkMoveUnitCommand;
}

void P_UnitMoveAI::Init()
{
	m_bTemp=RegisterExternalCommands();
	if(!m_pkMap)
	{
		m_pkMap = static_cast<HeightMap*>(g_ZFObjSys.GetObjectPtr("HeightMap"));
		if(m_pkMap)
			cout<<"found HeightMap!!" <<endl;
		else
			cout<<"didnt find the damn HeightMap!!" <<endl;
	}
		int aiCost[5];
		aiCost[0] = 15; // gräs (grön nyans)
		aiCost[1] = 1; // väg (röd nyans)
		aiCost[2] = 7; // sten (blå nyans)
		aiCost[3] = 10; // öken (röd nyans)
		aiCost[4] = 999; // vatten

		PathBuilder kPathBuilder(m_pkMap, &m_pkPathFind);
		kPathBuilder.Build(aiCost);
	
	///////Register commands
		
}

bool P_UnitMoveAI::RegisterExternalCommands()
{
	m_pkUnit=static_cast<P_ServerUnit*>(m_pkObject->GetProperty("P_ServerUnit"));
	if(m_pkUnit)
	{
		
		m_pkMoveUnitCommand = new ExternalCommand(this, UNIT_MOVE);
		m_pkMoveUnitCommand->m_kUnitCommandInfo.m_bNeedDestination = true;
		m_pkMoveUnitCommand->m_kUnitCommandInfo.m_bNeedTarget = true;
		strcpy(m_pkMoveUnitCommand->m_kUnitCommandInfo.m_acCommandName, "Move");
		strcpy(m_pkMoveUnitCommand->m_kUnitCommandInfo.m_acComments, "kommentar");
		m_pkUnit->RegisterExternalCommand(m_pkMoveUnitCommand);
		return true;
	}
	else
		return false;
}	
	
AIBase* P_UnitMoveAI::RunUnitCommand(int iCommandID, int iXDestinaton, int iYDestinaton, int iTarget)
{
	cout<<"sdad" <<endl;
	switch(iCommandID)
	{
	case UNIT_MOVE:
		{
			
		/*Vector3 Vec3;
		Vec3= m_pkObject->GetPos();
		Vec3.z+=20;
			m_pkObject->SetPos(Vec3);*/
			
			m_kEndPoint = Point(iXDestinaton,iYDestinaton);
			m_kStartPoint.x =  m_pkMap->m_iHmSize/2+ceil((m_pkObject->GetPos()).x / HEIGHTMAP_SCALE);
			m_kStartPoint.y = m_pkMap->m_iHmSize/2+ceil((m_pkObject->GetPos()).z / HEIGHTMAP_SCALE);
			
			
			if(m_pkPathFind->Rebuild(m_kStartPoint.x, m_kStartPoint.y, m_kEndPoint.x, m_kEndPoint.y) == false)
			{
				m_kEndPoint = m_kStartPoint;
				printf("Pathfinding Failed\n");
				return NULL;
			} 
			else
			{
				m_iCurrentState=UNIT_MOVE;
				return this;
			}
		}
	}
	return NULL;
}

AIBase* P_UnitMoveAI::UpdateAI()
{
	switch(m_iCurrentState)
	{
	case UNIT_MOVE:
		{
			/*static float prev_time = 0;
			float time = pkFps->GetGameTime();
			
			//if(time - prev_time > 0.125f)
			{
				int x=-1, y=-1;
				if(!m_pkTestPath->GetNextStep(x,y))
				{
					return true; // do nothing
				}
				
				if(!(x==-1&&y==-1))
					SetObjDstPos(x, y, pkObject);
				
				prev_time = time;
			}
			
			return true;*/
			int iX=-1, iY=-1;
			if(!m_pkPathFind->GetNextStep(iX,iY))
			{
					return NULL; // do nothing
			}
			if(!(iX==-1&&iY==-1))
			{
				float fX = -(m_pkMap->m_iHmSize/2)*HEIGHTMAP_SCALE + iX*HEIGHTMAP_SCALE;
				float fZ = -(m_pkMap->m_iHmSize/2)*HEIGHTMAP_SCALE + iY*HEIGHTMAP_SCALE;

				fX -= HEIGHTMAP_SCALE/2;	// Translate to center 
				fZ -= HEIGHTMAP_SCALE/2;	// of square.*/

				float fY = m_pkMap->Height(fX,fZ);

				m_pkObject->SetPos(Vector3(fX,fY,fZ));
				return this;
			}
			else 
				return NULL;

		}
	}
	
	return NULL;
}



vector<PropertyValues> P_UnitMoveAI::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
	kReturn[0].kValueName="FoundServerUnit";
	kReturn[0].iValueType=VALUETYPE_BOOL;
	kReturn[0].pkValue=(void*)&m_bTemp;;
	return kReturn;
};

COMMON_API Property* Create_P_UnitMoveAI()
{
	return new P_UnitMoveAI();

}