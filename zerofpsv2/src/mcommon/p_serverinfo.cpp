#include "p_serverinfo.h" 

P_ServerInfo::P_ServerInfo()
{
	strcpy(m_acName,"P_ServerInfo");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	bNetwork = true;
	m_sServerName	= 	"NoName";
	
}


void P_ServerInfo::AddPlayer(int id,string sName)
{
	SetNetUpdateFlag(true);	
	
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
	SetNetUpdateFlag(true);	
	
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
	SetNetUpdateFlag(true);	
	
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
	SetNetUpdateFlag(true);	
	
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
	SetNetUpdateFlag(true);
	
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

void P_ServerInfo::PackTo( NetPacket* pkNetPacket, int iConnectionID  ) 
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
			
		//only send messages to the target player
		if(iConnectionID == 	m_kPlayers[i].iId)	
		{
			int messages = m_kPlayers[i].kMessages.size();
			pkNetPacket->Write(&messages,sizeof(messages));
			while(!m_kPlayers[i].kMessages.empty())
			{
				cout<<"sent message:"<<m_kPlayers[i].kMessages.front()<<endl;				
				
				pkNetPacket->Write_Str(m_kPlayers[i].kMessages.front().c_str());
				m_kPlayers[i].kMessages.pop();		
			}

			 //----------------------------------------------------------------------- zeb
/*			int sounds = m_kPlayers[i].kSounds.size();
			pkNetPacket->Write(&sounds,sizeof(sounds)); // write nr of sounds
			while(!m_kPlayers[i].kSounds.empty())
			{
				int gen_id = m_kPlayers[i].kSounds.front().first;
				pkNetPacket->Write(&gen_id,sizeof(gen_id)); // write obj id
				pkNetPacket->Write_Str(m_kPlayers[i].kSounds.front().second.c_str()); // write sound name
				m_kPlayers[i].kSounds.pop();		
			}*/
			//----------------------------------------------------------------------- zeb 
		}
		else  //else send a dummy messages
		{
			int nomessages = 0;
			pkNetPacket->Write(&nomessages,sizeof(nomessages));
		}
	}
	
	SetNetUpdateFlag(iConnectionID,false);
}


void P_ServerInfo::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
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
			
		//read kControl vector
		temp.kControl.clear();		
		int objects; 
		pkNetPacket->Read(&objects,sizeof(objects));		
		for(int i =0;i<objects;i++)
		{	
			pair<int,int> t;
			pkNetPacket->Read(&t,sizeof(t));
			temp.kControl.push_back(t);
		}
		
		//read messages
		//while(!temp.kMessages.empty())			//clean message queue
		//	temp.kMessages.pop();

		int messages; 
		pkNetPacket->Read(&messages,sizeof(messages));		
		
		for(int i2 =0;i2<messages;i2++)
		{	
			char tempstr[256];
			pkNetPacket->Read_Str(tempstr);
			m_kMyMessages.push(tempstr);
			//temp.kMessages.push(tempstr);
			
		}

		 //----------------------------------------------------------------------- zeb

/*		// read sounds
		while(!temp.kSounds.empty())			//clean sound queue
			temp.kSounds.pop();

		int sounds; 
		pkNetPacket->Read(&sounds,sizeof(sounds));	// read nr of sounds	
			
		for(int i3 =0;i3<sounds;i3++)
		{	
			char tempstr[64];
			int object_gen_sound;

			pkNetPacket->Read(&object_gen_sound, sizeof(object_gen_sound)); // read obj id
			pkNetPacket->Read_Str(tempstr);

			temp.kSounds.push(pair<int,string>(object_gen_sound,string(tempstr)));	// read sound name 	
		}*/

		//----------------------------------------------------------------------- zeb 
		
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

void P_ServerInfo::MessageCharacter(int iObjectID,string strMessage)
{
	SetNetUpdateFlag(true);	

	cout<<"number of players"<<m_kPlayers.size()<<endl;
	

	for(int i =0 ;i< m_kPlayers.size();i++)
	{	
		cout<<"player "<<i<<" controls "<<m_kPlayers[i].kControl.size()<<"characters"<<endl;
		
		for(int j=0;j<m_kPlayers[i].kControl.size();j++)
		{	
			
			//check for object in player kontrol list
			if(m_kPlayers[i].kControl[j].first == iObjectID)
			{
				cout<<"foudn character"<<endl;
				//check rights 
				if(m_kPlayers[i].kControl[j].second & PR_CONTROLS) 
				{
					cout<<"sending "<<endl;
					m_kPlayers[i].kMessages.push(strMessage);	
				}
			}		
		}
	}
}


void P_ServerInfo::MessagePlayer(const char* czName,string strMessage)
{
	SetNetUpdateFlag(true);	
	
	for(int i =0 ;i< m_kPlayers.size();i++)
	{	
		if(m_kPlayers[i].sPlayerName == czName)
		{	
			//cout<<"found player"<<endl;
			m_kPlayers[i].kMessages.push(strMessage);	
			return;
		}
	}
}

void P_ServerInfo::MessagePlayer(int id,string strMessage)
{
	SetNetUpdateFlag(true);	

	//if target connection id is -1 send to everyone
	if(id == -1)
	{
		for(int i =0 ;i< m_kPlayers.size();i++)
		{	
			m_kPlayers[i].kMessages.push(strMessage);			
		}
		return;
	}

	PlayerInfo* pi = GetPlayerInfo(id);
	
	if(!pi)
	{
		cout<<"trying to send message to player that does not exist:"<<id<<endl;
		return;
	}
	
	//add message to players message queue
	pi->kMessages.push(strMessage);
	
}


Property* Create_P_ServerInfo()
{
	return new P_ServerInfo;
}

void P_ServerInfo::AddPrivateSoundToPlayer(int iPlayerObjectID, int iObjectGenSoundID, char *szFileName)
{
	SetNetUpdateFlag(true);	
	
	for(int i =0 ;i< m_kPlayers.size();i++)
	{	
		for(int j=0;j<m_kPlayers[i].kControl.size();j++)
		{	
			//check for object in player kontrol list
			if(m_kPlayers[i].kControl[j].first == iPlayerObjectID)
			{
				//check rights 
				if(m_kPlayers[i].kControl[j].second & PR_CONTROLS) 
				{
					m_kPlayers[i].kSounds.push(pair<int,string>(iObjectGenSoundID,string(szFileName)));	
					//printf("P_ServerInfo::AddSoundToPlayer\n");
				}
			}		
		}
	}
}

void P_ServerInfo::AddSound(int iObjectGenSoundID, char *szFileName)
{
	SetNetUpdateFlag(true);
	
	
	for(int i =0 ;i< m_kPlayers.size();i++)
	{	
		for(int j=0;j<m_kPlayers[i].kControl.size();j++)
		{	
			//check rights 
			if(m_kPlayers[i].kControl[j].second & PR_CONTROLS) 
			{
				m_kPlayers[i].kSounds.push(pair<int,string>(iObjectGenSoundID,string(szFileName)));	
				printf("P_ServerInfo::AddSoundToPlayer\n");
			}	
		}
	}
}


void P_ServerInfo::SetServerName(string strName)
{
	SetNetUpdateFlag(true);	
	
	m_sServerName = strName;
}