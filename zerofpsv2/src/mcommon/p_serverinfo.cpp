#include "p_serverinfo.h" 

P_ServerInfo::P_ServerInfo()
{
	strcpy(m_acName,"P_ServerInfo");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	bNetwork = true;
	m_sServerName	= 	"NoName";
	
	cout<<"server info created"<<endl;
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
	temp.kControl.clear();
	
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


void P_ServerInfo::AddObject(int id,int iObjID,int iRights)
{
	for(vector<PlayerInfo>::iterator it = m_kPlayers.begin(); it != m_kPlayers.end(); it++) 
	{
		if((*it).iId == id)
		{
			for(int i = 0;i<(*it).kControl.size();i++)
			{
				if((*it).kControl[i].first == iObjID)
					return;
			}
			
			(*it).kControl.push_back(pair<int,int>(iObjID,iRights));
		}
	}
}

void P_ServerInfo::RemoveObject(int id,int iObjID)
{
	for(vector<PlayerInfo>::iterator it = m_kPlayers.begin(); it != m_kPlayers.end(); it++) 
	{
		if((*it).iId == id)
		{
			for(vector<pair<int,int> >::iterator it2 = (*it).kControl.begin(); it2 != (*it).kControl.end(); it2++)
			{
				if((*it2).first == iObjID)
				{
					(*it).kControl.erase(it2);
					return;
				}
			
			}
		}
	}	

}

void P_ServerInfo::PackTo( NetPacket* pkNetPacket ) 
{
   pkNetPacket->Write_Str( m_sServerName.c_str());

	int players = m_kPlayers.size();	
	pkNetPacket->Write(&players,sizeof(players));
	
	for(int i=0;i<m_kPlayers.size();i++)
	{
		pkNetPacket->Write(&m_kPlayers[i].iId,sizeof(m_kPlayers[i].iId));
		pkNetPacket->Write_Str(m_kPlayers[i].sPlayerName.c_str());		
		
		int objects = m_kPlayers[i].kControl.size();
		pkNetPacket->Write(&objects,sizeof(objects));
		for(int j =0;j<objects;j++)
			pkNetPacket->Write(&m_kPlayers[i].kControl[j],sizeof(m_kPlayers[i].kControl[j]));
	}
	
}


void P_ServerInfo::PackFrom( NetPacket* pkNetPacket ) 
{   
	char tm[120];
   pkNetPacket->Read_Str( tm );
	m_sServerName = tm;

	m_kPlayers.clear();
	
	int players;
	pkNetPacket->Read(&players,sizeof(players));	
	
	for(int i=0;i<players;i++)	
	{
		PlayerInfo temp;
		pkNetPacket->Read(&temp.iId,sizeof(temp.iId));
	   char tm[120];		   
		pkNetPacket->Read_Str(tm);		
		temp.sPlayerName = tm;
		
		temp.kControl.clear();
		
		int objects; 
		pkNetPacket->Read(&objects,sizeof(objects));
		
		for(int i =0;i<objects;i++)
		{	
			pair<int,int> t;
			pkNetPacket->Read(&t,sizeof(t));
			temp.kControl.push_back(t);
		}
		m_kPlayers.push_back(temp);
	}
}

PlayerInfo* P_ServerInfo::GetPlayerInfo(int id)
{
	for(int i=0;i<m_kPlayers.size();i++)
	{
		if(m_kPlayers[i].iId == id)
			return &m_kPlayers[i];
	}
	
	return NULL;
}


Property* Create_P_ServerInfo()
{
	return new P_ServerInfo;
}


