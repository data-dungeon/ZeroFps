#ifndef _P_SERVERINFO_H_
#define _P_SERVERINFO_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"

using namespace std;

class MCOMMON_API P_ServerInfo: public Property {
	private:
		ZeroFps* m_pkFps;

		struct PlayerInfo
		{
			int		iId;
			string	sPlayerName;
			vector<int>	kControl;
		};

		vector<PlayerInfo>	m_kPlayers;
		

	public:
		string m_sServerName;				
		
		void CloneOf(Property* pkProperty) { }
		P_ServerInfo();
		
		void AddObject(int id,int iObjID);
		void RemoveObject(int id,int iObjID);		
		void AddPlayer(int id,string sName);
		void RemovePlayer(int id);
		void RemovePlayer(string sName);
		bool PlayerExist(string sName);
		bool PlayerExist(int id);		

		int GetNrOfPlayers(){return m_kPlayers.size();};
		
      void PackTo(NetPacket* pkNetPacket);
		void PackFrom(NetPacket* pkNetPacket);
		
};

MCOMMON_API Property* Create_P_ServerInfo();

#endif
