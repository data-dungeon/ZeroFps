#include "p_serverunit.h"
#include "tileengine.h"

P_ServerUnit::P_ServerUnit() : m_bUpdateCommands(true), m_pkCurrentAIState(NULL)
{
	strcpy(m_acName,"P_ServerUnit");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	m_kInfo.m_Info2.m_cTeam =			255;
	m_kInfo.m_Info2.m_cHealth =		255;
	m_kInfo.m_Info2.m_cWeapon =		0;
	m_kInfo.m_Info2.m_cArmor = 		0;
	m_kInfo.m_Info2.m_cPropultion =	0;
	m_kInfo.m_Info2.m_cViewDistance=20;
	m_kInfo.m_Info2.m_cWidth =		1;
	m_kInfo.m_Info2.m_cHeight =		1;	
	strcpy(m_kInfo.m_cName,"NoName");
	
	m_iMaxHealth = 1000;
	m_iHealth = 	1000;
	
	
	
	m_pkClientUnit = NULL;
	m_bHaveSetRadius = false;

	m_bClient = true;
	m_bHaveSetPos = false;
	
	m_kTile.x = -1;
	m_kTile.y = -1;
	
	
	
}

P_ServerUnit::~P_ServerUnit()
{
	TileEngine::m_pkInstance->RemoveUnit(m_pkObject->GetPos(),this);
}	

void P_ServerUnit::Init()
{
}


void P_ServerUnit::Update()
{
	if(!m_bHaveSetRadius)
		SetRadius();
	
	if(m_pkCurrentAIState)
		m_pkCurrentAIState=m_pkCurrentAIState->UpdateAI();
			
	if(m_bClient)
	{
		GetClientUnitP();		
		UpdateClient();
	}
		
	//set position in tile engine once after object has been created
	if(!m_bHaveSetPos){
		TileEngine::m_pkInstance->AddUnit(m_pkObject->GetPos(),this);						
		m_bHaveSetPos = true;
	}
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
	//calculate client health bar
	m_kInfo.m_Info2.m_cHealth = int(((float)m_iHealth/m_iMaxHealth)*255.0f);



	if(m_pkClientUnit != NULL)
	{
		m_pkClientUnit->m_kInfo = m_kInfo;
		//Update the clients avalible commands
		if(m_bUpdateCommands)
		{
			m_pkClientUnit->m_kUnitCommands.clear();
			//m_bUpdateCommands = false;
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
	pkPackage->Write(&m_bClient,sizeof(m_bClient));
	
	pkPackage->Write(&m_kInfo,sizeof(m_kInfo));
	pkPackage->Write(&m_kTile,sizeof(m_kTile));	
	pkPackage->Write(&m_iMaxHealth,sizeof(m_iMaxHealth));	
	pkPackage->Write(&m_iHealth,sizeof(m_iHealth));
}

void P_ServerUnit::Load(ZFMemPackage* pkPackage)
{
	pkPackage->Read(&m_bClient,sizeof(m_bClient));

	pkPackage->Read(&m_kInfo,sizeof(m_kInfo));
	pkPackage->Read(&m_kTile,sizeof(m_kTile));	
	pkPackage->Read(&m_iMaxHealth,sizeof(m_iMaxHealth));	
	pkPackage->Read(&m_iHealth,sizeof(m_iHealth));

}

vector<PropertyValues> P_ServerUnit::GetPropertyValues()
{
	vector<PropertyValues> kReturn(11);
		
	kReturn[0].kValueName="m_cTeam";
	kReturn[0].iValueType=VALUETYPE_CHARVAL;
	kReturn[0].pkValue=(void*)&m_kInfo.m_Info2.m_cTeam;

	kReturn[1].kValueName="m_cHealth";
	kReturn[1].iValueType=VALUETYPE_CHARVAL;
	kReturn[1].pkValue=(void*)&m_kInfo.m_Info2.m_cHealth;
	
	kReturn[2].kValueName="m_cWeapon";
	kReturn[2].iValueType=VALUETYPE_CHARVAL;
	kReturn[2].pkValue=(void*)&m_kInfo.m_Info2.m_cWeapon;
	
	kReturn[3].kValueName="m_cArmor";
	kReturn[3].iValueType=VALUETYPE_CHARVAL;
	kReturn[3].pkValue=(void*)&m_kInfo.m_Info2.m_cArmor;
	
	kReturn[4].kValueName="m_cPropultion";
	kReturn[4].iValueType=VALUETYPE_CHARVAL;
	kReturn[4].pkValue=(void*)&m_kInfo.m_Info2.m_cPropultion;
	
	kReturn[5].kValueName="m_cViewDistance";
	kReturn[5].iValueType=VALUETYPE_CHARVAL;
	kReturn[5].pkValue=(void*)&m_kInfo.m_Info2.m_cViewDistance;
	
	kReturn[6].kValueName="m_cWidth";
	kReturn[6].iValueType=VALUETYPE_CHARVAL;
	kReturn[6].pkValue=(void*)&m_kInfo.m_Info2.m_cWidth;
	
	kReturn[7].kValueName="m_cHeight";
	kReturn[7].iValueType=VALUETYPE_CHARVAL;
	kReturn[7].pkValue=(void*)&m_kInfo.m_Info2.m_cHeight;
	
	kReturn[8].kValueName="m_bClient";
	kReturn[8].iValueType=VALUETYPE_BOOL;
	kReturn[8].pkValue=(void*)&m_bClient;

	kReturn[9].kValueName="m_iMaxHealth";
	kReturn[9].iValueType=VALUETYPE_INT;
	kReturn[9].pkValue=(void*)&m_iMaxHealth;

	kReturn[10].kValueName="m_iHealth";
	kReturn[10].iValueType=VALUETYPE_INT;
	kReturn[10].pkValue=(void*)&m_iHealth;

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
			
AIBase* P_ServerUnit::RunExternalCommand(UnitCommand* kCommand)
{
	cout<<"run exrt" <<endl;
	map<string, ExternalCommand*>::iterator kIt = m_kExternalCommands.find(string(kCommand->m_acCommandName));
	if(kIt!= m_kExternalCommands.end())
	{
		
		m_pkCurrentAIState = kIt->second->RunUnitCommand(kCommand->m_iXDestinaton, kCommand->m_iYDestinaton, kCommand->m_iTarget);
		return m_pkCurrentAIState;
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
		Temp.m_iXDestinaton=100;
		Temp.m_iYDestinaton=100;
		RunExternalCommand(&Temp);
	}
}

bool P_ServerUnit::Damage(int iDamage)
{
	m_iHealth -= iDamage;
	cout<<"Damaging unit "<<iDamage<<endl;
	cout<<"Unit Got "<<m_iHealth<<" health left"<<endl;
		
	if(m_iHealth <= 0)
	{
		m_pkObject->m_pkObjectMan->Delete(m_pkObject);
	
		return false;
	}
	else
		return true;
}


