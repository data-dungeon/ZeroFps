#include "p_serverinfo.h"

P_ServerInfo::P_ServerInfo()
{
	strcpy(m_acName,"P_ServerInfo");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	bNetwork = true;


	strcpy(m_kSInfo.m_acServerName,"Test Server");
	
	cout<<"server info created"<<endl;
}

void P_ServerInfo::PackTo(NetPacket* pkNetPacket)
{
	pkNetPacket->Write(&m_kSInfo,sizeof(m_kSInfo));
}

void P_ServerInfo::PackFrom(NetPacket* pkNetPacket)
{
	pkNetPacket->Read(&m_kSInfo, sizeof(m_kSInfo));
	
}

COMMON_API Property* Create_P_ServerInfo()
{
	return new P_ServerInfo();

}










