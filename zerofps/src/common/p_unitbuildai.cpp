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
: m_pkUnit(NULL), m_uiCommandID(0), m_uiTechLevel(0), m_bIsReg(false)
{
	strcpy(m_acName,"P_UnitBuildAI");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;	
}

P_UnitBuildAI::~P_UnitBuildAI()
{
	if(!m_kExternalCommands.empty())
	{

		vector<ExternalCommand*>::iterator kItor = m_kExternalCommands.begin();
		while(kItor != m_kExternalCommands.end())
		{
			delete *kItor;
			kItor++;
		}
		m_kExternalCommands.clear();

	}
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
			cout<<"***************-" <<endl <<kItor->szName <<endl;
			ExternalCommand* pkTempCommand = new ExternalCommand(this, m_uiCommandID);
		//	m_pkAttackCommand->m_kUnitCommandInfo.m_bNeedDestination = false;
			pkTempCommand->m_kUnitCommandInfo.m_bNeedArgument = true;
			strcpy(pkTempCommand->m_kUnitCommandInfo.m_acCommandName, kItor->szName);
			strcpy(pkTempCommand->m_kUnitCommandInfo.m_acComments, kItor->szName);
			pkTempCommand->m_kUnitCommandInfo.m_iIconIndex = 4;
			if(m_pkUnit->RegisterExternalCommand(pkTempCommand))
				cout << "registerd" <<endl;
			else cout<< "not registerd" <<endl;
			m_kExternalCommands.push_back(pkTempCommand);
			m_uiCommandID++;
			kItor++;
		}
	return true;
	}
	return false;
}
void P_UnitBuildAI::Init2()
{
	cout<<"P_UnitBuildAI:m_uiTechLevel:"<<m_uiTechLevel <<endl;
	if(ConstructionManager::GetInstance()->GetPossibleBuildings(m_uiTechLevel, m_kStructures) > 0)//(m_uiTechLevel, m_kStructures) > 0)
	{
		cout<<"----------------------------------------" <<endl
			 <<"------------GUBBS MIGHTY DEBUG!---------" <<endl
			 <<"P_UnitBuildAI: got the buildings.." <<endl
			 <<"----------------------------------------" <<endl
			 <<"----------------------------------------" <<endl;
		if(RegisterExternalCommands())
			cout<<"P_UnitBuildAI: RegisterExternalCommands worked" <<endl;
		else 
			cout<<"P_UnitBuildAI: RegisterExternalCommands faild" <<endl;
	} 
	else
	{
		cout<<"----------------------------------------" <<endl
			 <<"------------GUBBS MIGHTY DEBUG!---------" <<endl
			 <<"P_UnitBuildAI: faild to get the buildnings.." <<endl
			 <<"----------------------------------------" <<endl
			 <<"----------------------------------------" <<endl;
	}
}

AIBase* P_UnitBuildAI::RunUnitCommand(int iCommandID, int iXDestinaton, int iYDestinaton, int iTarget)
{
	/*bool bSuccess = false;
	switch(iCommandID)
	{
	default:
		bSuccess = ConstructionManager::GetInstance()->Build("ZeroRTSFactory", 
			Point(iXDestinaton, iYDestinaton));
	}

	if(bSuccess)
		cout<<"I am building " <<endl;*/
	if(iCommandID<=m_uiCommandID)
	{
		bool bSuccess = false;
		bSuccess = ConstructionManager::GetInstance()->Build(m_kStructures[iCommandID].szName, 
			Point(iXDestinaton, iYDestinaton), m_pkUnit->m_kInfo.m_Info2.m_cTeam);
			
		cout<<m_kStructures[iCommandID].szName <<endl;
		if(bSuccess)
		{
			
			cout<<"I am building " <<endl;
		}
		else cout<<"I want to build but i cant :(" <<endl;
			
	}

	return NULL;

}

AIBase* P_UnitBuildAI::UpdateAI()
{
	return NULL;
}

vector<PropertyValues> P_UnitBuildAI::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
		
	kReturn[0].kValueName="TechLevel";
	kReturn[0].iValueType=VALUETYPE_INT;
	kReturn[0].pkValue=(void*)&m_uiTechLevel;
	return kReturn;
}

void P_UnitBuildAI::Save(ZFMemPackage* pkPackage)
{
	pkPackage->Write((void*)&m_uiTechLevel,sizeof(m_uiTechLevel));
}

void P_UnitBuildAI::Load(ZFMemPackage* pkPackage)
{
	pkPackage->Read((void*)&m_uiTechLevel,sizeof(m_uiTechLevel));
}


COMMON_API Property* Create_P_UnitBuildAI()
{
	return new P_UnitBuildAI();

}

void P_UnitBuildAI::Update()
{
	if(!m_bIsReg)
	{
		Init2();
		m_bIsReg = true;
	}
}