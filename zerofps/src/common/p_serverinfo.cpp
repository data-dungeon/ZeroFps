#include "p_serverinfo.h"

P_ServerInfo::P_ServerInfo()
{
	strcpy(m_acName,"P_ServerInfo");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	bNetwork = true;


	strcpy(m_kSInfo.m_acServerName,"Test Server");
	
	m_kSInfo.m_acColor[0] = 0;
	m_kSInfo.m_acColor[1] = 1;	
	m_kSInfo.m_acColor[2] = 2;	
	m_kSInfo.m_acColor[3] = 3;	
	
	m_kSInfo.m_aiPlayerResourses[0] = 2000;
	m_kSInfo.m_aiPlayerResourses[1] = 2000;	
	m_kSInfo.m_aiPlayerResourses[2] = 2000;	
	m_kSInfo.m_aiPlayerResourses[3] = 2000;	
	
	cout<<"server info created"<<endl;
}

void P_ServerInfo::Update()
{
	HandleEvents();
}

void P_ServerInfo::PackTo(NetPacket* pkNetPacket)
{
	pkNetPacket->Write_Str(m_kSInfo.m_acServerName);
	pkNetPacket->Write(&m_kSInfo.m_acColor,sizeof(unsigned char) * 4);
	pkNetPacket->Write(&m_kSInfo.m_aiPlayerResourses,sizeof(int) * 4);	
	
	
	int nrofevents = m_kServerEvents.size();
	pkNetPacket->Write(&nrofevents,sizeof(nrofevents));	
	
	while(!m_kServerEvents.empty())
	{
		pkNetPacket->Write(&m_kServerEvents.front(),sizeof(Event));		
		m_kServerEvents.pop();
	}
}

void P_ServerInfo::PackFrom(NetPacket* pkNetPacket)
{
	pkNetPacket->Read_Str(m_kSInfo.m_acServerName);
	pkNetPacket->Read(&m_kSInfo.m_acColor,sizeof(unsigned char) * 4);
	pkNetPacket->Read(&m_kSInfo.m_aiPlayerResourses,sizeof(int) * 4);	


	int nrofevents;
	pkNetPacket->Read(&nrofevents,sizeof(nrofevents));	
	
	for(int i=0;i<nrofevents;i++)
	{
		Event temp;
		pkNetPacket->Read(&temp,sizeof(Event));		
		m_kClientEvents.push(temp);
	}

}

void P_ServerInfo::AddEvent(Event kEv)
{
	m_kServerEvents.push(kEv);
	m_kClientEvents.push(kEv);	
}

void P_ServerInfo::HandleEvents()
{
	while(!m_kClientEvents.empty())
	{
		switch(m_kClientEvents.front().m_iType)
		{
			case 0:
				cout<<"got damn nice expolsion effect event from server"<<endl;
				break;
		
		}
	
		m_kClientEvents.pop();	
	}
}

COMMON_API Property* Create_P_ServerInfo()
{
	return new P_ServerInfo();

}










