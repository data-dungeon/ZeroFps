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

void P_ServerInfo::PackTo(NetPacket* pkNetPacket)
{
//	pkNetPacket->Write(&m_kSInfo,sizeof(m_kSInfo));
	pkNetPacket->Write_Str(m_kSInfo.m_acServerName);
	pkNetPacket->Write(&m_kSInfo.m_acColor,sizeof(unsigned char) * 4);
	pkNetPacket->Write(&m_kSInfo.m_aiPlayerResourses,sizeof(int) * 4);	
}

void P_ServerInfo::PackFrom(NetPacket* pkNetPacket)
{
//	pkNetPacket->Read(&m_kSInfo, sizeof(m_kSInfo));
	pkNetPacket->Read_Str(m_kSInfo.m_acServerName);
	pkNetPacket->Read(&m_kSInfo.m_acColor,sizeof(unsigned char) * 4);
	pkNetPacket->Read(&m_kSInfo.m_aiPlayerResourses,sizeof(int) * 4);	
}

COMMON_API Property* Create_P_ServerInfo()
{
	return new P_ServerInfo();

}










