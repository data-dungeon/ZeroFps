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
	int		iCharacterObjectID;
	vector<pair<int,int> >	kControl; // objekt id, r�ttigheter
	queue<string>	kMessages;
	queue<pair<int,string> > kSounds; // objekt id p� objektet som genererar ljudet, filnamn p� ljudet
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
		string 					m_sServerName;				
		vector<PlayerInfo>	m_kPlayers;		
		
		float m_fTimer;
		
	public:
		void AddSound(int iObjectGenSoundID, char* szFileName);
		void AddPrivateSoundToPlayer(int iPlayerObjectID, int iObjectGenSoundID, char* szFileName);

		queue<string>			m_kMyMessages;					//messages recived is put in this queue		
		
		void CloneOf(Property* pkProperty) { }
		P_ServerInfo();
		
		PlayerInfo* GetPlayerInfo(int id);

      vector<PlayerInfo>* GetPlayers()                { return &m_kPlayers; }
		
		void Update();
		
		int  GetPlayer(int iObjID);
		int  GetCharacterID(int id);
		void SetCharacterID(int id,int iObjID);
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
		void SetServerName(string strName);
		string GetServerName(){return m_sServerName;};

		int GetNrOfPlayers(){return m_kPlayers.size();};
		
      void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );
		
};

MCOMMON_API Property* Create_P_ServerInfo();

#endif
