#include <cstdlib>
#include <iostream>
#include <vector>
#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include "netpacket.h"
#include "msnetmess.h"
#include "mlmaster.h"

using namespace std;



bool ZFMasterServer::IsAddressEquals(IPaddress* pkAdr1, IPaddress* pkAdr2)
{
	if(pkAdr1->host != pkAdr2->host)	return false;
	if(pkAdr1->port != pkAdr2->port)	return false;

	return true;
}

void ZFMasterServer::RandomServers()
{
	IPaddress kServerIp;
	char* szGameName = "NoGame";

	for(int i=0; i<20; i++)
	{
		kServerIp.host = rand();
		kServerIp.port = 4242;

		int iRndGame = rand()%4;
		if(iRndGame == 0)	szGameName = "Mistlands";
		if(iRndGame == 1)	szGameName = "Dark Metropolis";
		if(iRndGame == 2)	szGameName = "StopEmUp";
		if(iRndGame == 3)	szGameName = "Oink Arena";

		AddServer(kServerIp, szGameName);
	}
}

void ZFMasterServer::PrintInfo()
{
	cout << "Num Of Servers: " << (int)m_kServers.size() << endl;

	for(int i=0; i<m_kServers.size(); i++)
	{
		cout << i << ": " << m_kServers[i].m_szGameName << endl;
	}
}


bool ZFMasterServer::PurgeOldServers()
{
	if(m_fNextPurgeTime > m_fTime)
		return false;

	for(vector<ZFServer>::iterator it = m_kServers.begin();it != m_kServers.end();it++)
	{
		if(((*it).m_fLastUpdate + MAX_SERVER_AGE) < m_fTime)
		{
			m_kServers.erase(it);
			return true;
		}
	}
	
	cout << "Num of Servers: " << (int)m_kServers.size() << endl;
	m_fNextPurgeTime = m_fTime + PURGE_PERIOD;
	cout << "Removing old servers..." << endl;
	return false;
}

void ZFMasterServer::AddServer(IPaddress kServerIp, char* szGameName)
{
	for(int i=0; i<m_kServers.size(); i++)
	{
		if(IsAddressEquals (&m_kServers[i].m_ServerIP, &kServerIp))
		{
			m_kServers[i].m_fLastUpdate = m_fTime;
			cout << "Server already registred" << endl;
			return;
		}
	}

	ZFServer kServer;
	kServer.m_ServerIP = kServerIp;
	kServer.m_fLastUpdate = m_fTime+rand()%60;
	strcpy(kServer.m_szGameName, szGameName);
	m_kServers.push_back(kServer);
   cout << "Num of servers is " << (int)m_kServers.size() << endl;
}

void ZFMasterServer::RemoveServer(IPaddress kServer)
{
	for(vector<ZFServer>::iterator it = m_kServers.begin();it != m_kServers.end();it++)
	{
 		if(IsAddressEquals(&((*it).m_ServerIP), &kServer))
		{
			m_kServers.erase(it);
			cout << "Server found, removing" << endl;
			return;
		}
	}

	cout << "Server not registred" << endl;
}



void ZFMasterServer::OnServerStart(NetPacket* pkNetPacket)
{
	char szGameName[MAX_GAME_NAME];
	cout << "ServerStart: " << endl;
	pkNetPacket->Read_Str(szGameName);
	AddServer(pkNetPacket->m_kAddress,szGameName);
}

void ZFMasterServer::OnServerDown(NetPacket* pkNetPacket)
{
	cout << "Server shutdown" << endl;
	RemoveServer(pkNetPacket->m_kAddress);
}

void ZFMasterServer::OnClientReqList(NetPacket* pkNetPack)
{
	NetPacket kServerList;
	kServerList.Clear();
	kServerList.m_kData.m_kHeader.m_iPacketType = 1;	//ZF_NETTYPE_CONTROL;
	kServerList.m_kData.m_kHeader.m_iOrder = 0;
	kServerList.m_kAddress = pkNetPack->m_kAddress;
	kServerList.Write( (char) 16 );

	kServerList.Write((int) m_kServers.size() );
	for(int i=0; i<m_kServers.size(); i++)
	{
		kServerList.Write( m_kServers[i].m_ServerIP );
	}

	UDPpacket kPacket;
	kPacket.channel	= -1;
	kPacket.data		= (unsigned char*)&kServerList.m_kData;
	kPacket.len			= kServerList.m_iLength + sizeof(ZFNetHeader);
	kPacket.maxlen		= MAX_PACKET_SIZE;
	kPacket.address	= kServerList.m_kAddress;
	int iRes = SDLNet_UDP_Send(m_pkSocket, -1, &kPacket);

}

bool ZFMasterServer::HandleNet()
{
	int iMSType;
	char szGameName[MAX_GAME_NAME];

	//UDPpacket kPacket;
	m_kSDLNetPacket.channel	= -1;
	m_kSDLNetPacket.data		= (unsigned char*)&m_kPkNetPacket.m_kData;
	m_kSDLNetPacket.len		= 0;
	m_kSDLNetPacket.maxlen	= MAX_PACKET_SIZE;
	m_kPkNetPacket.Clear();
	
	while(SDLNet_UDP_Recv(m_pkSocket, &m_kSDLNetPacket))
	{
		m_kPkNetPacket.m_kAddress = m_kSDLNetPacket.address;
		m_kPkNetPacket.m_iLength = m_kSDLNetPacket.len - sizeof(ZFNetHeader);
		// Validate Length of packet.
		// Validate Length of packet.
		m_kPkNetPacket.Read(iMSType);

		cout << "-Type: " << (int)m_kPkNetPacket.m_kData.m_kHeader.m_iPacketType;
		cout << ", Order: " << m_kPkNetPacket.m_kData.m_kHeader.m_iOrder;
		cout << ", iMSType: " << iMSType << endl;

		switch(iMSType)
		{
			case MS_SM_SERVERUP:			OnServerStart(&m_kPkNetPacket);			break;
			case MS_SM_SERVERDOWN:		OnServerDown(&m_kPkNetPacket);			break;
			case MS_CM_REQSERVERLIST:	OnClientReqList(&m_kPkNetPacket);		break;
		}

		m_kSDLNetPacket.len	= 0;
		m_kPkNetPacket.Clear();
	}

	return true;
}


ZFMasterServer::ZFMasterServer()
{
	
}

ZFMasterServer::~ZFMasterServer()
{

}


bool ZFMasterServer::StartUp()
{
  // Initialize SDL's subsystems - in this case, only video.
	if ( SDL_Init(SDL_INIT_TIMER) < 0 ) 
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}

	// Register SDL_Quit to be called at exit; makes sure things are
	// cleaned up when we quit.
	atexit(SDL_Quit);

	// Attempt to create a 640x480 window with 32bit pixels.
	screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
	SDL_WM_SetCaption("ZeroFps MasterServer", NULL);

	// If we fail, return error.
	if ( screen == NULL ) 
	{
		fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
		exit(1);
	}

	SDLNet_Init();
	m_pkSocket = SDLNet_UDP_Open( 4343 );
	UpdateTime();
	//RandomServers();

	return true;
}

bool ZFMasterServer::Run()
{
	bool bPurge;
   
	// Main loop: loop forever.
	while (1)
	{
		HandleNet();
		UpdateTime();

		bPurge = true;
		while(bPurge)
			bPurge = PurgeOldServers();


		// Poll for events, and handle the ones we care about.
		SDL_Event event;
    
		while (SDL_PollEvent(&event)) 
		{
			switch (event.type) 
			{
				case SDL_KEYDOWN:
					break;
				case SDL_KEYUP:
					// If escape is pressed, return (and thus, quit)
					if (event.key.keysym.sym == SDLK_ESCAPE)
						return 0;
					if (event.key.keysym.sym == SDLK_a)
					{
						PrintInfo();
					}
					break;
				case SDL_QUIT:
					return(0);
					break;
			}
		}
	}
}

bool ZFMasterServer::ShutDown()
{
	SDLNet_Quit();
	SDLNet_UDP_Close(m_pkSocket);

	return true;
}
	
int main(int argc, char *argv[])
{
	ZFMasterServer MasterServer;

	if(!MasterServer.StartUp())
		return 0;

	MasterServer.Run();
	MasterServer.ShutDown();

  return 0;
}

