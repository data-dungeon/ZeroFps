#ifndef _MLMASTER_H_
#define _MLMASTER_H_

#define MAX_PACKET_SIZE		1024	// Max Size of any network packet.
#define MAX_GAME_NAME		16		// Max Size of GameNames.
#define PURGE_PERIOD			10
#define MAX_SERVER_AGE		180

class	ZFServer
{
public:
	float			m_fLastUpdate;						
	IPaddress	m_ServerIP;
	char			m_szGameName[MAX_GAME_NAME];
};

class ZFMasterServer
{
public:
	ZFMasterServer();
	~ZFMasterServer();
	bool StartUp(bool bInterative);
	bool Run();
	bool ShutDown();



private:
	SDLNet_SocketSet	m_kSocketSet;

	SDL_Surface*		screen;
	UDPsocket			m_pkSocket;	
	float					m_fTime;
	float					m_fNextPurgeTime;
	vector<ZFServer>	m_kServers;

	bool					m_bInteractive;
	
	UDPpacket			m_kSDLNetPacket;
	NetPacket			m_kPkNetPacket;

	void OnServerStart(NetPacket* pkNetPacket);
	void OnServerDown(NetPacket* pkNetPacket);
	void OnClientReqList(NetPacket* pkNetPack);
		
	void AddServer(IPaddress kServer, char* szGameName);
	void RemoveServer(IPaddress kServer);
	bool IsAddressEquals(IPaddress* pkAdr1, IPaddress* pkAdr2);

	void RandomServers();
	void UpdateTime() { m_fTime=SDL_GetTicks()/1000.0; }

	bool PurgeOldServers();
	void PrintInfo();
	bool HandleNet();


	bool AddressToStr(IPaddress* pkAddress, char* szString);
};

#endif










