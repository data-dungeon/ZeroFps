#ifndef _P_SERVERINFO_H_
#define _P_SERVERINFO_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <queue>

using namespace std;

struct MCOMMON_API PlayerInfo
{
	int		iId;
	string	sPlayerName;
	vector<pair<int,int> >	kControl; // objekt id, rättigheter
	queue<string>	kMessages;
};

enum PlayerRights
{
	PR_OWNER=1,
	PR_CONTROLS=8,
	PR_LOOKAT=16,
};

class MCOMMON_API P_ServerInfo: public Property {
	private:
		ZeroFps* m_pkFps;

		vector<PlayerInfo>	m_kPlayers;		
	public:
		string 					m_sServerName;				
		queue<string>			m_kMyMessages;					//messages recived is put in this queue		
		
		void CloneOf(Property* pkProperty) { }
		P_ServerInfo();
		
		PlayerInfo* GetPlayerInfo(int id);
		
		void AddObject(int id,int iObjID,int iRights);
		void RemoveObject(int id,int iObjID);		
		void AddPlayer(int id,string sName);
		void RemovePlayer(int id);
		void RemovePlayer(string sName);
		bool PlayerExist(string sName);
		bool PlayerExist(int id);		
	
		void MessageCharacter(int iObjectID,string strMessage);
		void MessagePlayer(int id,string strMessage);
		void MessagePlayer(const char* czName,string strMessage);


		int GetNrOfPlayers(){return m_kPlayers.size();};
		
      void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );
		
};

MCOMMON_API Property* Create_P_ServerInfo();

#endif
