#include "p_suplygiver.h"

P_SuplyGiver::P_SuplyGiver()
{
	strcpy(m_acName,"P_SuplyGiver");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));			
	
	m_pkServerInfo=NULL;
	m_pkServerUnit=NULL;
	m_fTime = m_pkFps->GetGameTime();
	m_iAmount = 100;
	
}

void P_SuplyGiver::Update()
{
	if(!m_pkServerInfo)
	{
		Object* sio = m_pkObject->m_pkObjectMan->GetObject("A ServerInfoObject");
		
		if(sio)
			m_pkServerInfo =  (P_ServerInfo*)sio->GetProperty("P_ServerInfo");		
	}		
	
	if(!m_pkServerUnit)
	{
		m_pkServerUnit =  (P_ServerUnit*)m_pkObject->GetProperty("P_ServerUnit");		
	}		


	if(m_pkServerUnit && m_pkServerInfo)
		if(m_pkFps->GetGameTime() - m_fTime >2)
		{
			m_fTime = m_pkFps->GetGameTime();
	
			int team = int(m_pkServerUnit->m_kInfo.m_Info2.m_cTeam);
		
			cout<<"TEAM:"<<team<<endl;
		
			if(team >= 0 && team <= 4)			
				m_pkServerInfo->m_kSInfo.m_aiPlayerResourses[team] += m_iAmount;
			else			
				cout<<"Gods dont need money"<<endl;

		}
}



COMMON_API Property* Create_P_SuplyGiver()
{
	return new P_SuplyGiver();

}

