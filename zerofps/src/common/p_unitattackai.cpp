// p_unitattackai.cpp: implementation of the p_unitattackai class.
//
//////////////////////////////////////////////////////////////////////

#include "p_unitattackai.h"
#include "tileengine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

P_UnitAttackAI::P_UnitAttackAI()
:m_pkAttackCommand(NULL),m_pkUnit(NULL),m_pkTargetObject(NULL),
m_fRange(20),m_pkAi(NULL), m_pkUnitSystem(NULL), m_iTargetID(-1)
{
	strcpy(m_acName,"P_UnitAttackAI");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;	
	
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));			
	
	m_fRange *= m_fRange; 
	m_fLastFireTime = m_pkFps->GetGameTime();
	

}

P_UnitAttackAI::~P_UnitAttackAI()
{
	delete m_pkAttackCommand;
}

void P_UnitAttackAI::Init()
{
	RegisterExternalCommands();
	m_pkMap = static_cast<HeightMap*>(g_ZFObjSys.GetObjectPtr("HeightMap"));
}

bool P_UnitAttackAI::RegisterExternalCommands()
{
	m_pkUnit=static_cast<P_ServerUnit*>(m_pkObject->GetProperty("P_ServerUnit"));
	if(m_pkUnit)
	{
		
		m_pkAttackCommand = new ExternalCommand(this, UNIT_ATTACK);
		//	m_pkAttackCommand->m_kUnitCommandInfo.m_bNeedDestination = false;
		m_pkAttackCommand->m_kUnitCommandInfo.m_bNeedArgument = true;
		strcpy(m_pkAttackCommand->m_kUnitCommandInfo.m_acCommandName, "Attack");
		strcpy(m_pkAttackCommand->m_kUnitCommandInfo.m_acComments, "kommentar");
		m_pkAttackCommand->m_kUnitCommandInfo.m_iIconIndex = 0;
		m_pkUnit->RegisterExternalCommand(m_pkAttackCommand);
		return true;
	}
	else
		return false;
}

AIBase* P_UnitAttackAI::RunUnitCommand(int iCommandID, int iXDestinaton, int iYDestinaton, int iTarget)
{
	switch(iCommandID)
	{
	case UNIT_ATTACK:
		{
			if(iTarget>=0)
			{
				m_iTargetID = iTarget;
				m_pkTargetObject = NULL;
				m_pkTargetObject= m_pkObject->m_pkObjectMan->GetObjectByNetWorkID(iTarget);
				if(m_pkTargetObject)
				{
					cout<< "P_UnitAttackAI:found target!" <<endl;
					m_iCurrentState = UNIT_ATTACK;
					m_kTargetPrevPos = m_pkTargetObject->GetPos();
					return this;
				}
				else
				{
					cout<< "P_UnitAttackAI:didnt find target! :(" <<endl;
					
					UnitCommand TempCommand;
					strcpy(TempCommand.m_acCommandName, "Stop");
					//TempCommand.m_iTarget = m_pkObject->iNetWorkID;
					return m_pkUnit->RunExternalCommand(&TempCommand);
				}
				
				
			}
		}
	}
	return NULL;
}

AIBase* P_UnitAttackAI::UpdateAI()
{
	switch(m_iCurrentState)
	{
	case UNIT_ATTACK:
		{
			if(m_iTargetID >= 0)//cout<<"hehjehejejhe" <<endl;
				m_pkTargetObject= m_pkObject->m_pkObjectMan->GetObjectByNetWorkID(m_iTargetID);
			if(!m_pkTargetObject)
			{
				m_iTargetID = -1;
				UnitCommand TempCommand;
				strcpy(TempCommand.m_acCommandName, "Stop");
				return m_pkUnit->RunExternalCommand(&TempCommand);
			}
				
			else
			{
				if(!m_pkUnitSystem)
				{
					Object* sio = m_pkObject->m_pkObjectMan->GetObject("A ServerInfoObject");
					if(sio)
						m_pkUnitSystem=static_cast<P_UnitSystem*>(sio->GetProperty("P_UnitSystem"));
				}
				
				if(!m_pkUnitSystem)
					return NULL;
				
				
				Weapon* pkWep = m_pkUnitSystem->GetWeaponPointer((int)m_pkUnit->m_kInfo.m_Info2.m_cTeam,(int)m_pkUnit->m_kInfo.m_Info2.m_cWeapon);
				if(pkWep)
				{	
					m_fRange = pkWep->fRange;				
					m_fRange *=m_fRange;
				} else
				{
					cout<<"Error while getting weapon"<<endl;
					return NULL;
				}
			
				Vector3 kDistVec = m_pkTargetObject->GetPos() - m_pkObject->GetPos();
				float TempDist = (kDistVec.x * kDistVec.x) + (kDistVec.y * kDistVec.y) + (kDistVec.z * kDistVec.z);
				if(TempDist<m_fRange)
				{
					//cout<<"P_UnitAttackAI : Target in Range : "<<TempDist <<"<" <<m_fRange <<endl;
					
					//set rotation   this rotation sux
					Vector3 rot = kDistVec.Angels();
					rot.x =0;
					rot.z =0;
					rot.y = -rot.y;
	
					m_pkObject->SetRot(rot);		
					m_pkObject->SetRot(rot);			

					if(m_pkFps->GetGameTime() - m_fLastFireTime >= (1.0/pkWep->fFireRate))
						m_fLastFireTime =m_pkFps->GetGameTime(); 
					else 
					{
						if(m_pkAi)
							m_pkAi = m_pkAi->UpdateAI();
						return this;
					}
					
					if(m_pkAi)
					{
						UnitCommand TempCommand;
						strcpy(TempCommand.m_acCommandName, "Stop");
						//TempCommand.m_iTarget = m_pkObject->iNetWorkID;
						m_pkAi = m_pkUnit->RunExternalCommand(&TempCommand);
					}
					if(m_pkUnitSystem)
					{
						if(m_pkMap)
						{
							Point Target = TileEngine::m_pkInstance->GetSqrFromPos(m_pkTargetObject->GetPos());
							//Point Target(int(m_pkMap->m_iHmSize/2+ceil((m_pkTargetObject->GetPos().x / HEIGHTMAP_SCALE))),
							//	int(m_pkMap->m_iHmSize/2+ceil((m_pkTargetObject->GetPos().z / HEIGHTMAP_SCALE))));
							if(!m_pkUnitSystem->FireWeapon(m_pkUnit,Target,0))//m_pkUnit->m_kInfo.m_cWeapon)
							{
								cout<<"fwpe: false" <<endl;
								if(m_pkAi)
									return m_pkAi->UpdateAI();
								else return NULL;
							}
							else
							{
								if(m_pkAi)
									m_pkAi = m_pkAi->UpdateAI();
								return this;
							}
						}
					}
					else
					{ 
						cout<<"no P_UnitSystem" <<endl;
						return m_pkAi->UpdateAI(); 
					}
					//P_UnitSystem
					//m_pkAi->UpdateAI();
					//if(m_pkAi)
					//m_pkAi = m_pkAi->UpdateAI();
					
				}
				
				
				else
				{
					//cout<<"P_UnitAttackAI : Target not in Range : "<<TempDist <<">" <<m_fRange <<endl;
					if(!m_pkAi)// || ((m_pkTargetObject->GetPos() - m_kTargetPrevPos) > 300))
					{
						UnitCommand TempCommand;
						strcpy(TempCommand.m_acCommandName, "Move");
						TempCommand.m_iTarget = m_pkTargetObject->iNetWorkID;
						m_pkAi = m_pkUnit->RunExternalCommand(&TempCommand);
					}
					if(m_pkAi)
						m_pkAi = m_pkAi->UpdateAI();
					m_kTargetPrevPos = m_pkTargetObject->GetPos();
					return this;
				}
			}
		}     
		
	}
	
	return NULL;
}

COMMON_API Property* Create_P_UnitAttackAI()
{
	return new P_UnitAttackAI();

}