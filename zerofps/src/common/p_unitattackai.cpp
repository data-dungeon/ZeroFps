// p_unitattackai.cpp: implementation of the p_unitattackai class.
//
//////////////////////////////////////////////////////////////////////

#include "p_unitattackai.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

P_UnitAttackAI::P_UnitAttackAI()
	:m_pkAttackCommand(NULL),m_pkUnit(NULL),m_pkTargetObject(NULL),m_fRange(20),m_pkAi(NULL)
{
	strcpy(m_acName,"P_UnitAttackAI");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;	
	m_fRange *= m_fRange; 
}

P_UnitAttackAI::~P_UnitAttackAI()
{
	delete m_pkAttackCommand;
}

void P_UnitAttackAI::Init()
{
	RegisterExternalCommands();

}

bool P_UnitAttackAI::RegisterExternalCommands()
{
m_pkUnit=static_cast<P_ServerUnit*>(m_pkObject->GetProperty("P_ServerUnit"));
	if(m_pkUnit)
	{
		
		m_pkAttackCommand = new ExternalCommand(this, UNIT_ATTACK);
	//	m_pkAttackCommand->m_kUnitCommandInfo.m_bNeedDestination = false;
	//	m_pkAttackCommand->m_kUnitCommandInfo.m_bNeedTarget = true;
		strcpy(m_pkAttackCommand->m_kUnitCommandInfo.m_acCommandName, "Attack");
		strcpy(m_pkAttackCommand->m_kUnitCommandInfo.m_acComments, "kommentar");
		m_pkAttackCommand->m_kUnitCommandInfo.m_iIconIndex = 3;
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
				m_pkTargetObject = NULL;
				m_pkTargetObject= m_pkObject->m_pkObjectMan->GetObjectByNetWorkID(iTarget);
				if(m_pkTargetObject)
				{
					cout<< "P_UnitAttackAI:found target!" <<endl;
					return this;
				}
				else
				{
					cout<< "P_UnitAttackAI:didnt find target! :(" <<endl;
					return NULL;
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
			if(m_pkTargetObject)
			{
				Vector3 kDistVec = m_pkTargetObject->GetPos() - m_pkObject->GetPos();
				float TempDist = (kDistVec.x * kDistVec.x) + (kDistVec.y * kDistVec.y) + (kDistVec.z * kDistVec.z);
				if(TempDist<m_fRange)
				{
					if(m_pkAi)
					{
						UnitCommand TempCommand;
						strcpy(TempCommand.m_acCommandName, "Move");
						TempCommand.m_iTarget = m_pkObject->iNetWorkID;
						m_pkAi = m_pkUnit->RunExternalCommand(&TempCommand);
						m_pkAi->UpdateAI();
					}
					cout<<"P_UnitAttackAI : Target in Range : "<<TempDist <<"<" <<m_fRange <<endl;
				}
				else
				{
					cout<<"P_UnitAttackAI : Target not in Range : "<<TempDist <<">" <<m_fRange <<endl;
					if(!m_pkAi)
					{
						UnitCommand TempCommand;
						strcpy(TempCommand.m_acCommandName, "Move");
						TempCommand.m_iTarget = m_pkTargetObject->iNetWorkID;
						m_pkAi = m_pkUnit->RunExternalCommand(&TempCommand);
					}
					else m_pkAi->UpdateAI();

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