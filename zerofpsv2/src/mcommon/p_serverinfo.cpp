#include "p_serverinfo.h" 

P_ServerInfo::P_ServerInfo()
{
	strcpy(m_acName,"P_ServerInfo");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));


	m_sServerName	= 	"NoName";
}


void P_ServerInfo::AddPlayer(int id,string sName)
{
	if(PlayerExist(sName))
	{
		cout<<"Error: player "<<sName<<" already connected"<<endl;
		return;
	}

	PlayerInfo temp;
	temp.iId = id;
	temp.sPlayerName = sName;

	m_kPlayers.push_back(temp);
}

void P_ServerInfo::RemovePlayer(int id)
{
	for(vector<PlayerInfo>::iterator it = m_kPlayers.begin(); it != m_kPlayers.end(); it++) 
	{
		if((*it).iId == id)
		{
			m_kPlayers.erase(it);
			break;
		}
	}
}

void P_ServerInfo::RemovePlayer(string sName)
{
	for(vector<PlayerInfo>::iterator it = m_kPlayers.begin(); it != m_kPlayers.end(); it++) 
	{
		if((*it).sPlayerName == sName)
		{
			m_kPlayers.erase(it);
			break;
		}
	}
}

bool P_ServerInfo::PlayerExist(string sName)
{
	for(vector<PlayerInfo>::iterator it = m_kPlayers.begin(); it != m_kPlayers.end(); it++) 
	{
		if((*it).sPlayerName == sName)
		{
			return true;
		}
	}
	return false;
}

bool P_ServerInfo::PlayerExist(int id)
{
	for(vector<PlayerInfo>::iterator it = m_kPlayers.begin(); it != m_kPlayers.end(); it++) 
	{
		if((*it).iId == id)
		{
			return true;
		}
	}
	return false;
}

Property* Create_P_ServerInfo()
{
	return new P_ServerInfo;
}


