#include "p_serverunit.h"

P_ServerUnit::P_ServerUnit() : m_bUpdateCommands(true), m_pkCurrentAIState(NULL)
{
	strcpy(m_acName,"P_ServerUnit");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	m_kInfo.m_cTeam =			0;
	m_kInfo.m_cHealth =		255;
	m_kInfo.m_cWeapon =		0;
	m_kInfo.m_cArmor = 		0;
	m_kInfo.m_cPropultion =	0;
	m_kInfo.m_cViewDistance=20;
	strcpy(m_kInfo.m_cName,"NoName");
	
	m_pkClientUnit = NULL;
	m_bHaveSetRadius = false;

	
}


void P_ServerUnit::Update()
{
	if(!m_bHaveSetRadius)
		SetRadius();
	
	if(m_pkCurrentAIState)
		m_pkCurrentAIState=m_pkCurrentAIState->UpdateAI();
			
	
	GetClientUnitP();
	UpdateClient();
	//cout<<"external com:" <<m_kExternalCommands.size() <<endl;
	/*if(!m_pkClientUnit->m_kCommandsPending.empty())
	{
		cout<<"commands pendeing" <<endl;
		while(!m_pkClientUnit->m_kCommandsPending.empty())
		{
			RunExternalCommand(&m_pkClientUnit->m_kCommandsPending.front());
			m_pkClientUnit->m_kCommandsPending.pop();
		}
	}*/
}


void P_ServerUnit::GetClientUnitP()
{
	
	//dont get m_pkClientUnit if its already set
	if(m_pkClientUnit == NULL)
	{
		m_pkClientUnit = (P_ClientUnit*)m_pkObject->GetProperty("P_ClientUnit");
		
		//if we dont get any create it
		if(m_pkClientUnit == NULL)
		{
			m_pkObject->AddProperty("P_ClientUnit");
			m_pkClientUnit = (P_ClientUnit*)m_pkObject->GetProperty("P_ClientUnit");			
			
			//if we still dont get any COMPLAIN ALOT
			if(m_pkClientUnit == NULL)
				cout<<"Somthing is wrong with P_ClientUnit"<<endl;
		}	
		
	}

}

void P_ServerUnit::UpdateClient()
{
	if(m_pkClientUnit != NULL)
	{
		m_pkClientUnit->m_kInfo = m_kInfo;
		//Update the clients avalible commands
		if(m_bUpdateCommands)
		{
			m_pkClientUnit->m_kUnitCommands.clear();
			map<string, ExternalCommand*>::iterator kItor = m_kExternalCommands.begin();
			while (kItor != m_kExternalCommands.end())
			{
				if(kItor->second !=NULL)
					m_pkClientUnit->m_kUnitCommands.push_back((*kItor->second).m_kUnitCommandInfo);		
				
				kItor++;
				
			}
			m_pkClientUnit->m_bCommandsUpdated = true;
		}
	}
}

void P_ServerUnit::SetRadius()
{
	MadProperty* mp = (MadProperty*)m_pkObject->GetProperty("MadProperty");
	
	if(mp != NULL)
	{
		m_pkObject->GetRadius()=mp->GetRadius();		
		m_bHaveSetRadius = true;
	}

}

void P_ServerUnit::Save(ZFMemPackage* pkPackage)
{

}

void P_ServerUnit::Load(ZFMemPackage* pkPackage)
{

}

vector<PropertyValues> P_ServerUnit::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);
		
	kReturn[0].kValueName="m_cTeam";
	kReturn[0].iValueType=VALUETYPE_INT;
	kReturn[0].pkValue=(void*)&m_kInfo.m_cTeam;

	kReturn[1].kValueName="m_cHealth";
	kReturn[1].iValueType=VALUETYPE_INT;
	kReturn[1].pkValue=(void*)&m_kInfo.m_cHealth;

	return kReturn;
}

COMMON_API Property* Create_P_ServerUnit()
{
	return new P_ServerUnit();

}

bool P_ServerUnit::RegisterExternalCommand(ExternalCommand* kCommand)
{
	map<string, ExternalCommand*>::iterator kIt = m_kExternalCommands.find(string(kCommand->m_kUnitCommandInfo.m_acCommandName));
	if(kIt== m_kExternalCommands.end())
	{
		m_kExternalCommands[string(kCommand->m_kUnitCommandInfo.m_acCommandName)] = kCommand;
		m_bUpdateCommands = true;
		return true;
	}
	else 
		return false;

};

bool P_ServerUnit::RemoveExternalCommand(string kCommandName)
{
	map<string, ExternalCommand*>::iterator kIt = m_kExternalCommands.find(kCommandName);
	if(kIt!= m_kExternalCommands.end())
	{
		m_kExternalCommands.erase(kCommandName);
		m_bUpdateCommands = true;
		return true;
	}
	else 
		return false;
}
			
bool P_ServerUnit::RunExternalCommand(UnitCommand* kCommand)
{
	cout<<"run exrt" <<endl;
	map<string, ExternalCommand*>::iterator kIt = m_kExternalCommands.find(string(kCommand->m_acCommandName));
	if(kIt!= m_kExternalCommands.end())
	{
		
		m_pkCurrentAIState = kIt->second->RunUnitCommand(kCommand->m_iXDestinaton, kCommand->m_iYDestinaton, kCommand->m_iTarget);
		return true;
	}	
	else 
		return false;
};

void P_ServerUnit::HandleGameMessage(GameMessage& Msg)
{
	
	if(Msg.m_Name == "fisk")
	{
	UnitCommand Temp;
	strcpy(Temp.m_acCommandName,"Move");
	RunExternalCommand(&Temp);
		
	}
		
}