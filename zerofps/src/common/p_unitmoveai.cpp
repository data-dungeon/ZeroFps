// p_unitmoveai.cpp: implementation of the p_unitmoveai class.
//
//////////////////////////////////////////////////////////////////////

#include "p_unitmoveai.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

P_UnitMoveAI::P_UnitMoveAI() :m_pkMoveUnitCommand(NULL),m_pkUnit(NULL) 
{
	strcpy(m_acName,"P_UnitMoveAI");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	
}

P_UnitMoveAI::~P_UnitMoveAI()
{
	delete m_pkMoveUnitCommand;
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
	if(iCommandID = UNIT_MOVE)
	{
		Vector3 Vec3;
		Vec3= m_pkObject->GetPos();
		Vec3.z+=20;
		m_pkObject->SetPos(Vec3);
	}
	return NULL;
}

AIBase* P_UnitMoveAI::UpdateAI()
{
	return false;
}

COMMON_API Property* Create_P_UnitMoveAI()
{
	return new P_UnitMoveAI();

}

void P_UnitMoveAI::Init()
{
	RegisterExternalCommands();
}