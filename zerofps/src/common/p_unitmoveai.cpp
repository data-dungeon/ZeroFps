// p_unitmoveai.cpp: implementation of the p_unitmoveai class.
//
//////////////////////////////////////////////////////////////////////

#include "p_unitmoveai.h"
#include "tileengine.h"
#include <cmath>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HeightMap* P_UnitMoveAI::m_pkMap =NULL;

P_UnitMoveAI::P_UnitMoveAI() :m_pkMoveUnitCommand(NULL),m_pkUnit(NULL), m_bTemp(false) 
{
	strcpy(m_acName,"P_UnitMoveAI");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));	
		
	move.x = -1;
	move.y = -1;
	
	m_iCurrentState = -1;
	m_fSpeedMod = 1;

	m_pkPathFind = NULL; 
	m_pkSu = NULL;
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
			cout<<"didnt find the damn fucking shit asshole HeightMap fucker!!" <<endl;
	}

	int aiCost[5];
	aiCost[0] = 5; // gräs (grön nyans)
	aiCost[1] = 1; // väg (röd nyans)
	aiCost[2] = 8; // sten (blå nyans)
	aiCost[3] = 10; // öken (röd nyans)
	aiCost[4] = 999; // vatten

	PathBuilder kPathBuilder(m_pkMap, &m_pkPathFind);
	kPathBuilder.Build(aiCost);

	/////// Register commands	
}

bool P_UnitMoveAI::RegisterExternalCommands()
{
	m_pkUnit=static_cast<P_ServerUnit*>(m_pkObject->GetProperty("P_ServerUnit"));
	if(m_pkUnit)
	{
		
		m_pkMoveUnitCommand = new ExternalCommand(this, UNIT_MOVE);
		m_pkMoveUnitCommand->m_kUnitCommandInfo.m_bNeedArgument = true;
		strcpy(m_pkMoveUnitCommand->m_kUnitCommandInfo.m_acCommandName, "Move");
		strcpy(m_pkMoveUnitCommand->m_kUnitCommandInfo.m_acComments, "kommentar");
		m_pkMoveUnitCommand->m_kUnitCommandInfo.m_iIconIndex = 1;
		m_pkUnit->RegisterExternalCommand(m_pkMoveUnitCommand);

		m_pkStopUnitCommand = new ExternalCommand(this, UNIT_STOP);
		m_pkStopUnitCommand->m_kUnitCommandInfo.m_bNeedArgument = false;
		strcpy(m_pkStopUnitCommand->m_kUnitCommandInfo.m_acCommandName, "Stop");
		strcpy(m_pkStopUnitCommand->m_kUnitCommandInfo.m_acComments, "kommentar");
		m_pkStopUnitCommand->m_kUnitCommandInfo.m_iIconIndex = 2;
		m_pkUnit->RegisterExternalCommand(m_pkStopUnitCommand);
		return true;
	}
	else
		return false;
}	
	
AIBase* P_UnitMoveAI::RunUnitCommand(int iCommandID, int iXDestinaton, int iYDestinaton, int iTarget)
{
	switch(iCommandID)
	{
	case UNIT_MOVE:
		{
			cout<<"MOVEMOVE" <<endl;
			//add a waiting move destination
			if(iTarget>=0)
			{
				Object* TargetObject= m_pkObject->m_pkObjectMan->GetObjectByNetWorkID(iTarget);
				if(TargetObject)
				{
					move.x = int(m_pkMap->m_iHmSize/2+ceil((TargetObject->GetPos().x / HEIGHTMAP_SCALE)));
					move.y = int(m_pkMap->m_iHmSize/2+ceil((TargetObject->GetPos().z / HEIGHTMAP_SCALE)));
					return this;
				}
			}
			move.x = iXDestinaton;
			move.y = iYDestinaton;			
			
			return this;
		}
	case UNIT_STOP:
		{
			m_pkPathFind->Reset();
			//Vector3 kTempVect = m_pkObject->GetPos();
			//m_pkObject->SetPos(kTempVect);					
			//m_pkObject->SetPos(kTempVect);	
			m_iCurrentState = UNIT_MOVE;
			//m_fSpeedMod = 1;
			return this;
		}

	}
	return NULL;
}

AIBase* P_UnitMoveAI::UpdateAI()
{
	//setup serverunit pointer
	if(!m_pkSu)
	{
		m_pkSu = (P_ServerUnit*)m_pkObject->GetProperty("P_ServerUnit");	
		
		if(!m_pkSu)
			return NULL;
	}
	
	//if we are not doint anything check for orders
	if(m_iCurrentState == -1)
	{
		CheckForOrder();	
	}
	
	
	switch(m_iCurrentState)
	{
		case UNIT_MOVE:
		{
			
			if(!MoveTo(m_kCurretDestination))
			{
				//cout<<"Reached destination"<<endl;

				int iX=-1, iY=-1;
				if(!m_pkPathFind->GetNextStep(iX,iY))
				{

					//cout<<"no new destination"<<endl;
				
					//set pos one finale time to prevent ugly interpolation					
					m_pkObject->SetPos(m_kCurretDestination);					
					m_pkObject->SetPos(m_kCurretDestination);					
					
					m_iCurrentState = -1;
					return NULL;
				}	
				
				//check if any new move order has been given
				if(CheckForOrder())
				{
					//set pos one finale time to prevent ugly interpolation					
					m_pkObject->SetPos(m_kCurretDestination);					
					m_pkObject->SetPos(m_kCurretDestination);					
					
					return this;
				}
				
				//cout<<"New destination is "<<iX<<" "<<iY<<endl;
			
				//remove old marker
				TileEngine::m_pkInstance->RemoveUnit(m_pkObject->GetPos(),m_pkSu);										
						
				if(!TileEngine::m_pkInstance->GetTile(iX-1,iY-1)->kUnits.empty())
				{

					//cout<<"Hit something trying to find a new way"<<endl;
					TileEngine::m_pkInstance->AddUnit(m_pkObject->GetPos(),m_pkSu);						
					
					//set pos one finale time to prevent ugly interpolation										
					m_pkObject->SetPos(m_pkObject->GetPos());										
					m_pkObject->SetPos(m_pkObject->GetPos());					
					
					m_iCurrentState = UNIT_WAIT;										
					m_kCurretDestination = m_pkObject->GetPos();
					m_fWaitTime = m_pkFps->GetGameTime() + ((rand() % 2000)/1000.0);
					m_iRetries = 0;
							
					return this;
				}
					
				m_fSpeedMod = 1.0f - (float)(m_pkPathFind->GetTerrainCost(iX,iY) / 20.0);

				//make sure nothing is wrong
				if(m_fSpeedMod < 0)
					m_fSpeedMod = 1;


				float fX = -(m_pkMap->m_iHmSize/2.0)*HEIGHTMAP_SCALE + iX*HEIGHTMAP_SCALE;
				float fZ = -(m_pkMap->m_iHmSize/2.0)*HEIGHTMAP_SCALE + iY*HEIGHTMAP_SCALE;
			
				fX -= HEIGHTMAP_SCALE/2.0;	// Translate to center 
				fZ -= HEIGHTMAP_SCALE/2.0;	// of square.*

				float fY = m_pkMap->Height(fX,fZ);
			
				m_kCurretDestination.Set(fX,fY,fZ);
								
				
				
				//tell tile engine that im standing on kCurretDestination now
				TileEngine::m_pkInstance->AddUnit(m_kCurretDestination,m_pkSu);
				
				//now we can move 
				MoveTo(m_kCurretDestination);
			}
			
			
			return this;
		}
		
		case UNIT_WAIT:
		{
			CheckForOrder();
			
			if(m_iRetries >= 2)
			{
				return NULL;
			
			}
			
			if( (m_pkFps->GetGameTime() - m_fWaitTime) > 1.0)
			{
				//cout<<"trying again " <<m_iRetries<<endl;				
				m_iRetries++;
				
				m_fWaitTime = m_pkFps->GetGameFrameTime();
				
				if(!DoPathFind(m_pkObject->GetPos(),m_kEndPos,false))
					return this;
			}
			
			
			return this;
		}
	}
	
	return NULL;
}

bool P_UnitMoveAI::MoveTo(Vector3 kPos)
{
	if(kPos == m_pkObject->GetPos())
		return false;

	float fVel = 10;			
	fVel *= m_fSpeedMod; 
	
	if(fVel < 0)
	{	
		cout<<"ojoj nu blev det fel"<<endl;
		return false;
	}
		
	if( (m_pkObject->GetPos() - kPos).Length() < (fVel * m_pkFps->GetGameFrameTime()))
	{
		return false;
	}

	Vector3 kMoveV = (kPos - m_pkObject->GetPos()).Unit();
	Vector3 kNewPos = m_pkObject->GetPos() + kMoveV * (fVel * m_pkFps->GetGameFrameTime());


	//set rotation   this rotation sux
	Vector3 rot = kMoveV.Angels();
	rot.x =0;
	rot.z =0;
	rot.y = -rot.y;
	
	
	//	rot.y = atan2(kMoveV.z,kMoveV.x) * degtorad;
	
	
	m_pkObject->SetRot(rot);		
	m_pkObject->SetRot(rot);			
	m_pkObject->SetPos(kNewPos);	
	
	return true;
}

bool P_UnitMoveAI::DoPathFind(Vector3 kStart,Vector3 kStop,bool exact)
{
	m_kStartPos = kStart;
	m_kEndPos = kStop;

	m_kEndPoint.x = int((m_pkMap->m_iHmSize/2.0)+ceil((kStop.x / HEIGHTMAP_SCALE)));
	m_kEndPoint.y = int((m_pkMap->m_iHmSize/2.0)+ceil((kStop.z / HEIGHTMAP_SCALE)));	
	
	m_kStartPoint.x = int((m_pkMap->m_iHmSize/2.0)+ceil((kStart.x / HEIGHTMAP_SCALE)));
	m_kStartPoint.y = int((m_pkMap->m_iHmSize/2.0)+ceil((kStart.z / HEIGHTMAP_SCALE)));		
	
	//temporary remove this unit so path finding
	TileEngine::m_pkInstance->RemoveUnit(m_pkObject->GetPos(),m_pkSu);						
	

	if(!m_pkPathFind->Rebuild(m_kStartPoint.x, m_kStartPoint.y, m_kEndPoint.x, m_kEndPoint.y,exact))
	{
		//put this unit back
		TileEngine::m_pkInstance->AddUnit(m_pkObject->GetPos(),m_pkSu);								
		
		m_kEndPoint = m_kStartPoint;
		//printf("Pathfinding Failed\n");
		return true;
	} 
	else
	{
		//put this unit back
		TileEngine::m_pkInstance->AddUnit(m_pkObject->GetPos(),m_pkSu);								
		
		m_kCurretDestination = m_pkObject->GetPos();
		m_fSpeedMod = 1;
		m_iCurrentState=UNIT_MOVE;
		
		//cout<<"sucessfull"<<endl;
		return true;
	}
}

bool P_UnitMoveAI::CheckForOrder()
{
	if(move.x != -1)
	{
		float fX = -(m_pkMap->m_iHmSize/2)*HEIGHTMAP_SCALE + move.x*HEIGHTMAP_SCALE;
		float fZ = -(m_pkMap->m_iHmSize/2)*HEIGHTMAP_SCALE + move.y*HEIGHTMAP_SCALE;		
		
		move.x =-1;
		move.y =-1;
		
		if(DoPathFind(m_pkObject->GetPos(),Vector3(fX,0,fZ),true))
		{
			m_iCurrentState=UNIT_MOVE; 		
			return true;
		}
	}
	return false;
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





