#include "p_serverinfo.h" 

P_ServerInfo::P_ServerInfo()
{
	strcpy(m_acName,"P_ServerInfo");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));



	m_sServerName	= 	"NoName";
	m_iNrOfPlayers	=	0;
}

void P_ServerInfo::Update()
{

}


Property* Create_P_ServerInfo()
{
	return new P_ServerInfo;
}


