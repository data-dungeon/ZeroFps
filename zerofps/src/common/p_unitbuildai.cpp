// p_unitbuildai.cpp: implementation of the p_unitbuildai class.
//
//////////////////////////////////////////////////////////////////////

#include "p_unitbuildai.h"
//#include "constructionmanager.h"
//#include <vector>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

P_UnitBuildAI::P_UnitBuildAI() 
: m_pkUnit(NULL), m_uiCommandID(0)
{
	strcpy(m_acName,"P_UnitBuildAI");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;	
}

P_UnitBuildAI::~P_UnitBuildAI()
{
	vector<ExternalCommand*>::iterator kItor = m_kExternalCommands.begin();
	while(kItor != m_kExternalCommands.end())
	{
		delete *kItor;
	}
	m_kExternalCommands.clear();
}


bool P_UnitBuildAI::RegisterExternalCommands()
{
	m_pkUnit=static_cast<P_ServerUnit*>(m_pkObject->GetProperty("P_ServerUnit"));
	if(!m_kStructures.empty() && m_pkUnit)
	{
		vector<Structure>::iterator kItor = m_kStructures.begin();
		//for(int i = 0; i < m_kStructures.size(); i++)
		while(kItor != m_kStructures.end())
		{	
			
			ExternalCommand* pkTempCommand = new ExternalCommand(this, (int)kItor->ucID);
		//	m_pkAttackCommand->m_kUnitCommandInfo.m_bNeedDestination = false;
			pkTempCommand->m_kUnitCommandInfo.m_bNeedArgument = true;
			strcpy(pkTempCommand->m_kUnitCommandInfo.m_acCommandName, kItor->szName);
			strcpy(pkTempCommand->m_kUnitCommandInfo.m_acComments, kItor->szName);
			pkTempCommand->m_kUnitCommandInfo.m_iIconIndex = 4;
			m_pkUnit->RegisterExternalCommand(pkTempCommand);
			m_kExternalCommands.push_back(pkTempCommand);
			
			kItor++;
		}
	return true;
	}
	return false;
}
void P_UnitBuildAI::Init()
{
	if(ConstructionManager::GetInstance()->GetPossibleBuildings(0, m_kStructures) > 0)//(m_uiTechLevel, m_kStructures) > 0)
	{
		cout<<"P_UnitBuildAI: got the buildings.." <<endl;
		if(RegisterExternalCommands())
			cout<<"P_UnitBuildAI: RegisterExternalCommands worked" <<endl;
		else 
			cout<<"P_UnitBuildAI: RegisterExternalCommands faild" <<endl;
	} 
	else
	{
		cout<<"P_UnitBuildAI: faild to get the buildnings.." <<endl;
	}
}

AIBase* P_UnitBuildAI::RunUnitCommand(int iCommandID, int iXDestinaton, int iYDestinaton, int iTarget)
{
	bool bSuccess = false;
	switch(iCommandID)
	{
	default:
		bSuccess = ConstructionManager::GetInstance()->Build("ZeroRTSFactory", 
			Point(iXDestinaton, iYDestinaton));
	}

	if(bSuccess)
		cout<<"I am building " <<endl;


	return NULL;

}

AIBase* P_UnitBuildAI::UpdateAI()
{
	return NULL;
}

vector<PropertyValues> P_UnitBuildAI::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
		
	kReturn[0].kValueName="m_uiTechLevel";
	kReturn[0].iValueType=VALUETYPE_INT;
	kReturn[0].pkValue=(void*)&m_uiTechLevel;
	return kReturn;
}

COMMON_API Property* Create_P_UnitBuildAI()
{
	return new P_UnitBuildAI();

}