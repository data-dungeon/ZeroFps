#include "p_enviroment.h"

P_Enviroment::P_Enviroment()
{
	strcpy(m_acName,"P_Enviroment");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkObjectMan=static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));

	bNetwork = true;

	m_bEnabled = false;
	
	m_StrCurrentEnviroment = "Default";
}

void P_Enviroment::Update()
{
	//if(!m_bEnabled)
	
	
}

void P_Enviroment::ZoneChange(int iCurrent,int iNew)
{
	cout<<"Zone change:"<<endl;

	ZoneData* zd = m_pkObjectMan->GetZoneData(iNew);

	if(zd)	
	{
		m_StrCurrentEnviroment = zd->m_strEnviroment;
	}
}


void P_Enviroment::SetEnviroment(char* csEnviroment)
{
	if(m_StrCurrentEnviroment == csEnviroment)
		return;

	m_StrCurrentEnviroment = csEnviroment;

}

void P_Enviroment::PackTo(NetPacket* pkNetPacket)
{
	pkNetPacket->Write_NetStr(m_StrCurrentEnviroment.c_str());	
}

void P_Enviroment::PackFrom(NetPacket* pkNetPacket)
{
	char temp[128];
	
	pkNetPacket->Read_NetStr(temp);
	
	SetEnviroment(temp);

}

Property* Create_P_Enviroment()
{
	return new P_Enviroment;
}