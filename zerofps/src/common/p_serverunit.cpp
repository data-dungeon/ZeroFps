#include "p_serverunit.h"

P_ServerUnit::P_ServerUnit()
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

	GetClientUnitP();
	UpdateClient();	
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
