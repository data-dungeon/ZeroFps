#pragma warning( disable : 4512) // 'class' : assignment operator could not be generated 

#include "network.h"
#include "zerofps.h"

#include "../basic/zfsystem.h"

//#define	NET_LOGALL	

ZSSNetWork* g_pkNetWork;

ZSSNetWork::ZSSNetWork()
: ZFSubSystem("ZSSNetWork")
{
	strcpy(m_szGameName, "ZeroFps");
	m_bAcceptClientConnections = false;
	m_pkSocket						= NULL;
	m_eNetStatus					= NET_NONE;
	m_fStatsUpdate					= 0;

	// Set Default values
	//m_fConnectTimeOut		= ZF_NET_CONNECTION_TIMEOUT;
	m_iMaxNumberOfNodes	= 0;
	m_iDefPort				= 4242;
	//m_strMasterServer    = "dvoid.no-ip.com";
	//m_bPublishServer		= true;
	m_fMSNextPing			= 0;
	//m_strServerName		= "Mistlands_Server";
	//m_strPublishIp			= "none";
	m_iBadPackages			= 0;
	//m_iMaxOutput 			= 50000;

	// Register Variables	
	m_kfConnectTimeOut.Register(this, "n_connecttimeout",	"10");
	m_kstrMasterServer.Register(this, "n_mslink",			"dvoid.no-ip.com");
	m_kbPublishServer.Register(this, "n_mspublish",			"1");
	m_kstrServerName.Register(this, "n_servername", "Mistlands_Server");
	m_kstrPublishIp.Register(this, "n_publiship", "none");
	m_kiMaxOutput.Register(this, "n_maxoutput", "50000");

	//RegisterVariable("n_connecttimeout",	&m_fConnectTimeOut,		CSYS_FLOAT);	
	//RegisterVariable("n_mslink",				&m_strMasterServer,		CSYS_STRING);	
	//RegisterVariable("n_mspublish",			&m_bPublishServer,		CSYS_BOOL);	
   //RegisterVariable("n_servername",			&m_strServerName,			CSYS_STRING);
   //RegisterVariable("n_publiship",			&m_strPublishIp,			CSYS_STRING);
   //RegisterVariable("n_maxoutput",			&m_iMaxOutput,				CSYS_INT);
	
	// Register Commands
	Register_Cmd("n_netgmax", FID_NETGMAX);
	Register_Cmd("n_dns", FID_DNS);

	m_kStringTable.resize( ZF_NET_MAXSTRINGS );
	for(int i=0; i < ZF_NET_MAXSTRINGS; i++) {
		m_kStringTable[i].m_bInUse			= false;
		m_kStringTable[i].m_bUpdated		= false;
		m_kStringTable[i].m_bNeedUpdate	= false;
		m_kStringTable[i].m_NetString = "";
		}
}

bool ZSSNetWork::StartUp()	
{ 
	m_pkConsole	= static_cast<ZSSConsole*>(GetSystem().GetObjectPtr("ZSSConsole"));
	m_pkZeroFps	= static_cast<ZSSZeroFps*>(GetSystem().GetObjectPtr("ZSSZeroFps"));
	g_pkNetWork  = this;

	// Create Log Files.
	GetSystem().Log_Create("net");
	GetSystem().Log_Create("netpac");

	int iInitRes = SDLNet_Init();

#ifdef NET_LOGALL
	GetSystem().Log("net",		"NetWork SubSystem Startup:\n");
	GetSystem().Log("netpac",	"NetWork Packet Log:\n");
	g_ZFObjSys.Logf("net", "SDLNet_Init(): %d\n", iInitRes);
#endif

	int iMasterPort = 4343;

	if(!IsValidIPAddress( m_kstrMasterServer.GetString().c_str() ))
	{
		if(DnsLookUp( m_kstrMasterServer.GetString().c_str(), m_kMasterServerIP ))
		{
			m_kMasterServerIP.port = 0;
			m_kMasterServerIP.port = m_kMasterServerIP.port | ((iMasterPort >> 8) & 0xff);  
			m_kMasterServerIP.port = m_kMasterServerIP.port | ((iMasterPort << 8) & 0xff00);  							
		}
		else
		{
			cout << "Failed to get IP to master server" << endl;
		}
	}
	else
	{
		char szFinalTarget[256];
		sprintf(szFinalTarget, "%s:%d", m_kstrMasterServer.GetString().c_str(), iMasterPort);
		StrToAddress(szFinalTarget,&m_kMasterServerIP); 		
	}

	return true; 
}

bool ZSSNetWork::ShutDown() 
{	
	
	return true;	
}
bool ZSSNetWork::IsValid()	 { return true;	}

ZSSNetWork::~ZSSNetWork()
{
#ifdef NET_LOGALL
	for(int i=0; i < ZF_NET_MAXSTRINGS; i++) {
		if(m_kStringTable[i].m_bInUse)
			g_ZFObjSys.Logf("net", "NetString[%d]: = %s\n", i, m_kStringTable[i].m_NetString.c_str());
		}
#endif

	DisconnectAll();
	CloseSocket();
	SDLNet_Quit();

#ifdef NET_LOGALL
	g_ZFObjSys.Logf("net", "NetWork SubSystem ShutDown:\n");
#endif
}

string ZSSNetWork::GetLocalIP()
{
//	char MyIp[256];
//	m_kLocalIP kIp;
//	AddressToStr(kIp, MyIp);
	return string("ost");
}

// NetStrings are strings that are sent as int's over network. 
int ZSSNetWork::NetString_GetFree()
{
	for(int i=0; i < ZF_NET_MAXSTRINGS; i++) {
		if(m_kStringTable[i].m_bInUse == false)
			return i;
		}
	return ZF_NET_NONETSTRING;
}

int ZSSNetWork::NetString_Add(const char* szString)
{
	string strTest;
	strTest = /*string("A Cool ") +*/ string(szString);
#ifdef NET_LOGALL
	g_ZFObjSys.Logf("net", "NetString Add: = %s\n", strTest.c_str());
#endif

	if( m_eNetStatus == NET_CLIENT )	return ZF_NET_NONETSTRING;

	int iIndex = NetString_GetIndex(strTest.c_str());
	if(iIndex != ZF_NET_NONETSTRING)
		return iIndex;

	iIndex = NetString_GetFree();
	if(iIndex == ZF_NET_NONETSTRING)
		return ZF_NET_NONETSTRING;

	m_kStringTable[iIndex].m_bInUse		= true;
	m_kStringTable[iIndex].m_bUpdated	= true;
	m_kStringTable[iIndex].m_NetString	= strTest;
	return iIndex;
}

int ZSSNetWork::NetString_GetIndex(const char* szString)
{
	for(int i=0; i < ZF_NET_MAXSTRINGS; i++) {
		if(m_kStringTable[i].m_bInUse && (strcmp(szString, m_kStringTable[i].m_NetString.c_str()) == 0) )
			return i;
		}

	return ZF_NET_NONETSTRING;
}

string ZSSNetWork::NetString_GetString(int iIndex)
{
	if(iIndex < 0 || iIndex >= int(m_kStringTable.size()))
		return string("nons");

	if(m_kStringTable[iIndex].m_bInUse)
		return m_kStringTable[iIndex].m_NetString.c_str();
	else {
		m_kStringTable[iIndex].m_bNeedUpdate = true;
		return string("nons");
		}
}

void ZSSNetWork::NetString_ReSendAll()
{
	for(int i=0; i < ZF_NET_MAXSTRINGS; i++)
		m_kStringTable[i].m_bUpdated = true;
}

// Check all netstrings for updates and send them to the other side.
void ZSSNetWork::Send_NetStrings()
{
	if( m_eNetStatus == NET_NONE )	return;
	if( m_eNetStatus == NET_CLIENT )	return;

	if(!NetStringIsUpdated())
		return;

	NetPacket NP;
	NP.Clear();
	NP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
	NP.Write((char) ZF_NETCONTROL_NETSTRINGS);


	for(int i=0; i < ZF_NET_MAXSTRINGS; i++) {
		if(m_kStringTable[i].m_bInUse && m_kStringTable[i].m_bUpdated) {
			NP.Write( i );
			NP.Write_Str(m_kStringTable[i].m_NetString.c_str());
			m_kStringTable[i].m_bUpdated = false;
#ifdef NET_LOGALL
			g_ZFObjSys.Logf("net", "Write NetString[%d]: = %s\n", i, m_kStringTable[i].m_NetString.c_str());
#endif
			}

		// Send if packet to large.
		if(NP.m_iPos >= 512) {
			NP.Write( ZF_NET_NONETSTRING );
#ifdef NET_LOGALL
			g_ZFObjSys.Logf("net", "Write NetStrings: Order Client 0 : %d", m_RemoteNodes[0].m_iNumOfPacketsSent );
#endif
			NP.TargetSetClient(ZF_NET_ALLCLIENT);
			//SendToAllClients(&NP);
			Send2(&NP);

			NP.Clear();
			NP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
			NP.Write((char) ZF_NETCONTROL_NETSTRINGS);
			}
		}

	NP.Write( ZF_NET_NONETSTRING );
#ifdef NET_LOGALL
	g_ZFObjSys.Logf("net", "Write NetStrings: Order Client 0 : %d", m_RemoteNodes[0].m_iNumOfPacketsSent );
#endif
	NP.TargetSetClient(ZF_NET_ALLCLIENT);
	//SendToAllClients(&NP);
	Send2(&NP);
}

bool ZSSNetWork::NetStringIsUpdated()
{
	for(int i=0; i < ZF_NET_MAXSTRINGS; i++) {
		if(m_kStringTable[i].m_bInUse && m_kStringTable[i].m_bUpdated)
			return true;
		}

	return false;
}

void ZSSNetWork::NetString_Refresh()
{
	if( m_eNetStatus == NET_NONE )	return;
	if( m_eNetStatus == NET_SERVER )	return;

	for(int i=0; i < ZF_NET_MAXSTRINGS; i++) {
		if(m_kStringTable[i].m_bNeedUpdate) {
			m_kStringTable[i].m_bNeedUpdate = false;
			NetString_Request(i);
			}
		}
}

void ZSSNetWork::NetString_Request(int iIndex)
{
	NetPacket kNetPRespons;
	kNetPRespons.Clear();
	kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
	kNetPRespons.Write((char) ZF_NETCONTROL_REQNETSTRING);
	kNetPRespons.Write( iIndex );
//	SendToAllClients(&kNetPRespons);
	kNetPRespons.TargetSetClient(ZF_NET_ALLCLIENT);
	Send2(&kNetPRespons);
}

int ZSSNetWork::GetNumOfClients(void)
{
	int iNumOfClients = 0;
	for(unsigned int i=0; i < m_RemoteNodes.size(); i++) 
	{
		if(m_RemoteNodes[i]->m_eConnectStatus != NETSTATUS_DISCONNECT)
			iNumOfClients++;
	}

	return iNumOfClients;
}

bool ZSSNetWork::IsAddressEquals(IPaddress* pkAdr1, IPaddress* pkAdr2)
{
	if(pkAdr1->host != pkAdr2->host)	return false;
	if(pkAdr1->port != pkAdr2->port)	return false;

	return true;
}

int ZSSNetWork::GetClientNumber(IPaddress* pkAddress)
{
	for(unsigned int i=0; i < m_RemoteNodes.size(); i++) {
		if(IsAddressEquals(pkAddress, &m_RemoteNodes[i]->m_kAddress))
			return i;
		}

	return ZF_NET_NOCLIENT;
}

void ZSSNetWork::SetMaxNodes(int iMaxNode)
{
	for(unsigned int i = 0;i<m_RemoteNodes.size();i++)
	{
		delete m_RemoteNodes[i];
	}
	
	m_RemoteNodes.clear();
	
 	for(unsigned int i =0;i<iMaxNode;i++)
 		m_RemoteNodes.push_back(new RemoteNode());	
	
	m_iMaxNumberOfNodes = iMaxNode;
		

// 	m_RemoteNodes.clear();
// 
// 	RemoteNode temp;
// 	
// 	for(int i =0;i<iMaxNode;i++)
// 		m_RemoteNodes.push_back(temp);
// 	
// 	m_iMaxNumberOfNodes = iMaxNode;

}

int ZSSNetWork::GetFreeClientNum()
{
	for(int i=0; m_RemoteNodes.size(); i++) 
	{
		if(m_RemoteNodes[i]->m_eConnectStatus == NETSTATUS_DISCONNECT)
		{
			m_RemoteNodes[i]->Clear();
			return i;
		}
	}

	return ZF_NET_NOCLIENT;
}

bool ZSSNetWork::IsConnected(int iId)
{
	if(m_RemoteNodes[iId]->m_eConnectStatus == NETSTATUS_CONNECTED)
		return true;
	else
		return false;
}

int ZSSNetWork::GetClientNetSpeed(int iId)
{
	if(m_RemoteNodes[iId]->m_eConnectStatus == NETSTATUS_DISCONNECT)
		return 0;

	int iNetSpeed = m_RemoteNodes[iId]->m_iNetSpeed;
	int iMaxSpeed = m_kiMaxOutput.GetInt() / GetNumOfClients();

	return Min(iNetSpeed,iMaxSpeed);
}

void ZSSNetWork::StartSocket(bool bStartServer,int iPort)
{
	if(m_pkSocket) 
	{
		cout << "StartSocket: Socket is already open" << endl;
		return;
	}

	//client uses socket 0 (none i presume)
	int iSocketNum = 0;
	
	//server uses iPort
	if(bStartServer)	iSocketNum = iPort;
	
	//open the socket
	m_pkSocket = SDLNet_UDP_Open( iSocketNum );
	
	if(!m_pkSocket) 
	{
		g_ZFObjSys.Logf("net", "Error open socket %d: %s.\n", iSocketNum, SDLNet_GetError());
		m_pkConsole->Printf("Failed to open socket %d.", iSocketNum);
		return;
	}
}

void ZSSNetWork::CloseSocket()
{
	if(!m_pkSocket)	return;
	SDLNet_UDP_Close(m_pkSocket);
	m_pkSocket = NULL;
	m_eNetStatus = NET_NONE;
}

void ZSSNetWork::ServerStart(int iPort)
{
	StartSocket(true,iPort);
	m_eNetStatus = NET_SERVER;
}

void ZSSNetWork::ServerEnd(void)
{
	if(m_eNetStatus == NET_SERVER)
	{
		MS_ServerDown();
	}

	DisconnectAll();
	CloseSocket();
	m_eNetStatus = NET_NONE;
}

void ZSSNetWork::ClientStart()
{
	if(m_eNetStatus == NET_SERVER)
		return;

	StartSocket(false);
	m_eNetStatus = NET_CLIENT;
}

void ZSSNetWork::ClientStart(const char* szIp,int iPort ,const char* szLogin, const char* szPass, bool bConnectAsEditor,int iNetSpeed)
{
	if(m_eNetStatus == NET_SERVER)
		return;

	StartSocket(false);
	m_eNetStatus = NET_CLIENT;

	

	// DNS LookUp.
	IPaddress kTargetIP;
	
	//if not a valis IP, try dns resolve
	if(!IsValidIPAddress( szIp ))
	{
		if(DnsLookUp( szIp, kTargetIP ))
		{
			//setup port
			if(kTargetIP.port == 0)
			{
				kTargetIP.port = 0;
				kTargetIP.port = kTargetIP.port | ((iPort >> 8) & 0xff);  
				kTargetIP.port = kTargetIP.port | ((iPort << 8) & 0xff00);  							
			}
		
			char res[256];
			AddressToStr(&kTargetIP, res);
			cout<<"resolved "<<szIp<<" to "<<res<<endl;			
		}
		else
		{
			cout << "Failed dns lookup for "<<szIp<<endl;
			return;
		}
	}
	else //else asume normal ip
	{
		char szFinalTarget[256];
		sprintf(szFinalTarget, "%s:%d", szIp, iPort);
		StrToAddress(szFinalTarget,&kTargetIP); 		
	}

	
	
	NetPacket NetP;

	NetP.Clear();
	NetP.m_kAddress = kTargetIP;
	//NetP.SetTarget(szIp);
	NetP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
	NetP.m_kData.m_kHeader.m_iOrder = 0;
	NetP.Write((char) ZF_NETCONTROL_JOIN2);
	NetP.Write_Version(m_pkZeroFps->GetVersion());
	NetP.Write_Str(szLogin);
	NetP.Write_Str(szPass);
	NetP.Write((int) bConnectAsEditor);
	NetP.Write(iNetSpeed);
	SendRaw(&NetP);

	m_kServerAddress = NetP.m_kAddress;
}

void ZSSNetWork::RequestServerInfo(ServerInfo* pkServer)
{
	//save request time
	pkServer->m_fRequestTime = m_pkZeroFps->GetEngineTime();

	
	NetPacket NetP;

	NetP.Clear();
	NetP.m_kAddress = pkServer->m_kServerIp;
	NetP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
	NetP.m_kData.m_kHeader.m_iOrder = 0;
	NetP.Write((char) ZF_NETCONTROL_LIST2);
	SendRaw(&NetP);
}

void ZSSNetWork::SendServerInfo(IPaddress kIp)
{
	NetPacket NetP;

	NetP.Clear();
	NetP.m_kAddress = kIp;
	NetP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
	NetP.m_kData.m_kHeader.m_iOrder = 0;
	NetP.Write((char) ZF_NETCONTROL_SERVERINFO2);
	NetP.Write_Version(m_pkZeroFps->GetVersion());
	NetP.Write_Str(m_kstrServerName.GetString().c_str());
	NetP.Write(GetNumOfClients());
	NetP.Write(m_iMaxNumberOfNodes);

	SendRaw(&NetP);
	
}

void ZSSNetWork::GotServerInfo(NetPacket* pkNetPacket)
{
	// Read out info
	char szServerName[256];
	int iNumOfPlayers;
	int iMaxPlayers;
	ZFVersion kVersion;

	pkNetPacket->Read_Version( kVersion );
	pkNetPacket->Read_Str(szServerName);
	pkNetPacket->Read(iNumOfPlayers);
	pkNetPacket->Read(iMaxPlayers);


	for(unsigned int i=0; i<m_kServers.size(); i++)
	{
		if( IsAddressEquals(&m_kServers[i].m_kServerIp, &pkNetPacket->m_kAddress))
		{
			m_kServers[i].m_fRequestTime = m_pkZeroFps->GetEngineTime() - m_kServers[i].m_fRequestTime;
			m_kServers[i].m_iNumOfPlayers = iNumOfPlayers;
			m_kServers[i].m_iMaxPlayers = iMaxPlayers;
			m_kServers[i].m_bUpdated = true;
			m_kServers[i].m_kVersion = kVersion;
			
			strcpy(m_kServers[i].m_acServerName, szServerName);						
			GetSystem().SendSystemMessage(string("Application"),string("serverlist"),0,NULL);
			return;
		}
	}
}



void ZSSNetWork::MS_ServerIsActive()
{
	if(!m_kbPublishServer.GetBool())
		return;

/*	IPaddress kTargetIP;
	char szFinalTarget[256];
	sprintf(szFinalTarget, "%s:%d", m_strMasterServer.c_str(), 4343);
	StrToAddress(szFinalTarget,&kTargetIP); 		*/

	NetPacket NetP;

	NetP.Clear();
	NetP.m_kAddress = m_kMasterServerIP;
	NetP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
	NetP.m_kData.m_kHeader.m_iOrder = 0;
	NetP.Write((int) 0);
	NetP.Write_Str(m_szGameName);

	if(m_kstrPublishIp.GetString() != "none")
	{
		IPaddress kTargetIP;
		StrToAddress(m_kstrPublishIp.GetString().c_str(),&kTargetIP); 	
		NetP.Write((int) 1);
		NetP.Write(kTargetIP);
	}
	else
	{
		NetP.Write((int) 0);
	}

	SendRaw(&NetP);
}

void ZSSNetWork::MS_ServerDown()
{
/*	IPaddress kTargetIP;
	char szFinalTarget[256];
	sprintf(szFinalTarget, "%s:%d", m_strMasterServer.c_str(), 4343);
	StrToAddress(szFinalTarget,&kTargetIP); 		*/

	NetPacket NetP;

	NetP.Clear();
	NetP.m_kAddress = m_kMasterServerIP;
	NetP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
	NetP.m_kData.m_kHeader.m_iOrder = 0;
	NetP.Write((int) 1);
	SendRaw(&NetP);
}

void ZSSNetWork::MS_RequestServers()
{
	NetPacket NetP;

	NetP.Clear();
	NetP.m_kAddress = m_kMasterServerIP;
	NetP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
	NetP.m_kData.m_kHeader.m_iOrder = 0;
	NetP.Write((int) 2);
	SendRaw(&NetP);
	
		
	//clear serverlist and send message to application
	m_kServers.clear();			
	GetSystem().SendSystemMessage(string("Application"),string("serverlist-requested"),0,NULL);	
	
}

void ZSSNetWork::MS_GotServers(NetPacket* pkNetPack)
{
	m_kServers.clear();
	char SzAdress[128];

	int iNumOfServers;
	pkNetPack->Read(iNumOfServers);
	m_pkConsole->Printf("There are %d active servers\n",iNumOfServers );
	for(int i=0; i<iNumOfServers; i++)
	{
		ServerInfo kServer;
		kServer.m_bUpdated			= false;
		kServer.m_iNumOfPlayers		= 0;
		kServer.m_iMaxPlayers		= 0;
		kServer.m_fRequestTime		= -1;
		strcpy(kServer.m_acServerName, "Unkown");

		pkNetPack->Read(kServer.m_kServerIp);	
		AddressToStr(&kServer.m_kServerIp, SzAdress);
		m_pkConsole->Printf("[%d]: %s\n",i, SzAdress);
		RequestServerInfo(&kServer);
		m_kServers.push_back(kServer);
	}

	GetSystem().SendSystemMessage(string("Application"),string("serverlist"),0,NULL);
}



/**	\brief	Checks for incoming packets and return them.
	
	Checks if any packet have arrived and if so puts it in the netpacket,
*/
bool ZSSNetWork::Recv(NetPacket* pkNetPacket)
{
	pkNetPacket->Clear();

	UDPpacket kPacket;
	kPacket.channel	= -1;
	kPacket.data		= (unsigned char*)&pkNetPacket->m_kData;
	kPacket.len			= 0;
	kPacket.maxlen		= MAX_PACKET_SIZE;

	if(m_iMaxIncomingFrame)
	{
		if(SDLNet_UDP_Recv(m_pkSocket, &kPacket))
		{
			pkNetPacket->m_kAddress = kPacket.address;
			pkNetPacket->m_iLength	= kPacket.len - sizeof(ZFNetHeader);
			g_ZFObjSys.Logf("netpac", "Recv From Net");
			return true;
		}
	}


	for(unsigned int i=0; i<m_RemoteNodes.size(); i++)
	{
		if(m_RemoteNodes[i]->m_eConnectStatus == NETSTATUS_DISCONNECT)
			continue;

		int iIndex = m_RemoteNodes[i]->GetRelRecv( m_RemoteNodes[i]->m_iReliableRecvOrder );
		if(iIndex != -1)
		{
			memcpy(&pkNetPacket->m_kData, m_RemoteNodes[i]->m_akRelPackRecv[iIndex], sizeof( ZFNetPacketData ));
			pkNetPacket->m_kAddress = m_RemoteNodes[i]->m_kAddress;
			pkNetPacket->m_iLength  = m_RemoteNodes[i]->m_aiRelPackRecvSize[iIndex];
			m_RemoteNodes[i]->FreeRelRecv( m_RemoteNodes[i]->m_akRelPackRecv[iIndex] );
			g_ZFObjSys.Logf("netpac", "Recv From Local");
			return true;
		}
	}

	
	return false;
}

/**	\brief	Sends a package.
	
*/
bool ZSSNetWork::SendRaw(NetPacket* pkNetPacket)
{
	// Check that it is a valid package.
	if(pkNetPacket->m_kData.m_kHeader.m_iPacketType == 204)
		assert(0);

	// Update order num of package.
	if(pkNetPacket->m_iClientID != ZF_NET_NOCLIENT) 
	{
		pkNetPacket->m_kData.m_kHeader.m_iOrder = m_RemoteNodes[ pkNetPacket->m_iClientID ]->m_iNumOfPacketsSent;
		m_RemoteNodes[pkNetPacket->m_iClientID]->m_iNumOfPacketsSent ++;
		m_RemoteNodes[pkNetPacket->m_iClientID]->m_iNumOfBytesSent += pkNetPacket->m_iLength;
		
		if(pkNetPacket->m_kData.m_kHeader.m_iPacketType == ZF_NETTYPE_REL)
		{
			pkNetPacket->m_kData.m_kHeader.m_iOrder = m_RemoteNodes[ pkNetPacket->m_iClientID ]->m_iReliableSendOrder;
			m_RemoteNodes[ pkNetPacket->m_iClientID ]->m_iReliableSendOrder ++;

			int iIndex = m_RemoteNodes[pkNetPacket->m_iClientID]->GetFreeRelStore();
			if(iIndex == -1)
			{
				m_RemoteNodes[pkNetPacket->m_iClientID]->m_bIsFlood = true;
				cout << "Connection Flood" << endl;
				for(int i=0; i<ZF_NET_MAXREL; i++)
					m_RemoteNodes[pkNetPacket->m_iClientID]->m_akRelPack[i]->m_kHeader.m_iPacketType = ZF_NETTYPE_NONE;

				iIndex = m_RemoteNodes[pkNetPacket->m_iClientID]->GetFreeRelStore();
			}

			ZFNetPacketData* pkRelPak = m_RemoteNodes[pkNetPacket->m_iClientID]->m_akRelPack[iIndex];
			memcpy(pkRelPak, &pkNetPacket->m_kData, sizeof( ZFNetPacketData ));
			m_RemoteNodes[pkNetPacket->m_iClientID]->m_akRelPackSendTime[iIndex] = m_pkZeroFps->GetEngineTime();
			m_RemoteNodes[pkNetPacket->m_iClientID]->m_aiRelPackSize[iIndex] = pkNetPacket->m_iLength;
			m_RemoteNodes[pkNetPacket->m_iClientID]->m_kRelSend.insert( pkNetPacket->m_kData.m_kHeader.m_iOrder );

			// WRITE: Store time to update ping.
			if(m_RemoteNodes[pkNetPacket->m_iClientID]->m_iRelPingIndex == 0)
			{
				m_RemoteNodes[pkNetPacket->m_iClientID]->m_fPingSentEngineTime = m_pkZeroFps->GetEngineTime();
				m_RemoteNodes[pkNetPacket->m_iClientID]->m_iRelPingIndex = pkNetPacket->m_kData.m_kHeader.m_iOrder;
			}
		}
	}

	// Send it.
	UDPpacket kPacket;
	kPacket.channel	= -1;
	kPacket.data		= (unsigned char*)&pkNetPacket->m_kData;
	kPacket.len			= pkNetPacket->m_iLength + sizeof(ZFNetHeader);
	kPacket.maxlen		= MAX_PACKET_SIZE;
	kPacket.address	= pkNetPacket->m_kAddress;

	// Validate Data
	//ZFAssert(kPacket.len > 0 && kPacket.len < MAX_PACKET_SIZE, "NetWork::SendRaw - Size Errors");
	if(pkNetPacket->m_iLength < 0 || pkNetPacket->m_iLength > MAX_PACKET_SIZE)
		assert(0);
	if(pkNetPacket->m_kData.m_kHeader.m_iOrder < 0)
		assert(0);	
	if(pkNetPacket->m_kData.m_kHeader.m_iPacketType < ZF_NETTYPE_NONE || 
		pkNetPacket->m_kData.m_kHeader.m_iPacketType > ZF_NETTYPE_REL)
		assert(0);


	int iRes = SDLNet_UDP_Send(m_pkSocket, -1, &kPacket);

	return true;
}

void ZSSNetWork::SendUDP(ZFNetPacketData* pkData, int iSize, IPaddress* pkIp)
{
	UDPpacket kPacket;
	kPacket.channel	= -1;
	kPacket.data		= (unsigned char*)pkData;
	kPacket.len			= iSize + sizeof(ZFNetHeader);
	kPacket.maxlen		= MAX_PACKET_SIZE;
	kPacket.address	= *pkIp;

	//ZFAssert(kPacket.len > 0 && kPacket.len < MAX_PACKET_SIZE, "NetWork::SendRaw - Size Errors");
	if(kPacket.len < 0 && kPacket.len > MAX_PACKET_SIZE)
	{
		assert(0);
	}

	int iRes = SDLNet_UDP_Send(m_pkSocket, -1, &kPacket);
}


bool ZSSNetWork::Send2(NetPacket* pkNetPacket)
{
	static int iSpliceSize = 1018;

	//is package to big?
	if(pkNetPacket->m_iLength > iSpliceSize)
	{
		//is this a reliable package?
		if(pkNetPacket->m_kData.m_kHeader.m_iPacketType == ZF_NETTYPE_REL)
		{		
			int iTotalSize = pkNetPacket->m_iLength;
			static NetPacket kSplit;			
		
			//cout<<"package is a reliable package, splitting it  , total size "<<iTotalSize<<endl;						

			//step trough the package data and send the parts seperatly			
			for(int i = 0;i<iTotalSize;i+=iSpliceSize)
			{
				//clear and setup package
				kSplit.Clear();			
				kSplit.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_REL;
				kSplit.m_iTargetClients = pkNetPacket->m_iTargetClients;

				//set this as a split package if there is more then iSpliceSize bytes left
				if(iTotalSize - i > iSpliceSize)
					kSplit.m_kData.m_kHeader.m_iSplit = true;
				else
					kSplit.m_kData.m_kHeader.m_iSplit = false;
							
				//copy data to split package
 				kSplit.WriteNp(pkNetPacket,i,iSpliceSize);
			
 				//send splice
				RealSend2(&kSplit);					
			}
			
			//cout<<"splitet package in "<<iSplices<< " splices"<<endl;			
			return true;	
		}
		else
		{
			cout<<"WARNING: none reliable package is to big"<<endl;
			return false;
		}
	}
	else
		pkNetPacket->m_kData.m_kHeader.m_iSplit = false;
	
	return RealSend2(pkNetPacket);
}

bool ZSSNetWork::RealSend2(NetPacket* pkNetPacket)
{
	// If we have any clients to send to.
	if(pkNetPacket->m_iTargetClients.size()) 
	{
		for(unsigned int i=0; i<pkNetPacket->m_iTargetClients.size(); i++) 
		{
			if(m_RemoteNodes[ pkNetPacket->m_iTargetClients[i] ]->m_eConnectStatus != NETSTATUS_CONNECTED)
				continue;
			
			pkNetPacket->m_iClientID = pkNetPacket->m_iTargetClients[i];
			pkNetPacket->m_kAddress = m_RemoteNodes[ pkNetPacket->m_iClientID ]->m_kAddress;

			SendRaw(pkNetPacket);
		}

		return true;
	}	
	else 
	{
		pkNetPacket->m_iClientID = ZF_NET_NOCLIENT;
		SendRaw(pkNetPacket);
		return true;
	}
}

void ZSSNetWork::HandleControlMessage(NetPacket* pkNetPacket)
{
	int iObjId;
	char szText[256];
	string strNoMessage;

	float fEngineTime = m_pkZeroFps->GetEngineTime();

	int iClientID;
	unsigned char ucControlType;
	pkNetPacket->Read(ucControlType);

	NetPacket kNetPRespons;
	//RemoteNode kNewNode;

	char	szLogin[64];
	char	szPass[64];
	int 	iConnectAsEditor;
	int	iNetSpeed;
	int	iNetID;
	ZFVersion kVersion;

	switch(ucControlType) {
		// If controll handle_controllpacket.
		case ZF_NETCONTROL_LIST2:
			//m_pkConsole->Printf("NetWork::HandleControlMessage(ZF_NETCONTROL_LIST)");
			// Broadcast to find servers.
			// Server respons with name.
			SendServerInfo(pkNetPacket->m_kAddress);
			break;

		
		case 1:
			m_pkConsole->Printf("Ignore - NetWork::HandleControlMessage(ZF_NETCONTROL_LIST2)");
			break;
		case 17:
			m_pkConsole->Printf("Ignore - NetWork::HandleControlMessage(ZF_NETCONTROL_SERVERINFO)");
			break;
		case 2:
			m_pkConsole->Printf("Ignore - NetWork::HandleControlMessage(ZF_NETCONTROL_JOIN)");
			break;

		case ZF_NETCONTROL_SERVERINFO2:
			//m_pkConsole->Printf("NetWork::HandleControlMessage(ZF_NETCONTROL_LIST)");
			GotServerInfo(pkNetPacket);
			break;

		case ZF_NETCONTROL_JOIN2:
			// Sent to join a server.
			//m_pkConsole->Printf("NetWork::HandleControlMessage(ZF_NETCONTROL_JOIN)");
			AddressToStr(&pkNetPacket->m_kAddress, m_szAddressBuffer);

			// Server respons with yes/no.
			kNetPRespons.Clear();

			pkNetPacket->Read_Version( kVersion );
			if(kVersion != m_pkZeroFps->GetVersion())		
			{
				m_pkConsole->Printf("Join Ignored: Client version don't match.");
				kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
				kNetPRespons.Write((unsigned char) ZF_NETCONTROL_JOINNO);
				kNetPRespons.Write_Str( string("The servers is running another version then you.") );
				kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
				SendRaw(&kNetPRespons);
				break;
			}
	

			pkNetPacket->Read_Str(szLogin);
			pkNetPacket->Read_Str(szPass);
			pkNetPacket->Read(iConnectAsEditor);
			pkNetPacket->Read(iNetSpeed);

			m_pkConsole->Printf("'%s' try to join as '%s' in editmode '%d'", m_szAddressBuffer, szLogin, iConnectAsEditor);

			//wanted speed to low
// 			if( (iNetSpeed<1000) || iNetSpeed>100000 ) 
// 			{
// 				m_pkConsole->Printf("Join Ignored: invalid netspeed.");
// 				kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
// 				kNetPRespons.Write((unsigned char) ZF_NETCONTROL_JOINNO);
// 				kNetPRespons.Write_Str( string("Invalid netspeed.") );
// 				kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
// 				SendRaw(&kNetPRespons);
// 				break;
// 			}
			
			//check netspeed
			if(iNetSpeed < 1000)
			{
				m_pkConsole->Printf("Client joined with to low netspeed (%d), setting netspeed 1000",iNetSpeed);
				iNetSpeed = 1000;
			}
						
			if(iNetSpeed > 1000000)
			{
				m_pkConsole->Printf("Client joined with to hi netspeed (%d), setting netspeed %d",iNetSpeed,m_kiMaxOutput.GetInt());
				iNetSpeed = 1000000;
			}			
			
			//max connections
			if(GetNumOfClients() == m_iMaxNumberOfNodes) 
			{
				m_pkConsole->Printf("Join Ignored: To many connected clients.");
				kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
				kNetPRespons.Write((unsigned char) ZF_NETCONTROL_JOINNO);
				kNetPRespons.Write_Str( string("The server is full.") );
				kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
				SendRaw(&kNetPRespons);
				break;
			}

			//failed preconnect
			if(! m_pkZeroFps->PreConnect(pkNetPacket->m_kAddress, szLogin, szPass, iConnectAsEditor, strNoMessage))
			{
				m_pkConsole->Printf("Join Ignored: Preconnect no.");
				kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
				kNetPRespons.Write((unsigned char) ZF_NETCONTROL_JOINNO);
				kNetPRespons.Write_Str( strNoMessage );
				kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
				SendRaw(&kNetPRespons);
				break;
			}

			// Connect OK
			iClientID = GetFreeClientNum();
			assert(iClientID != ZF_NET_NOCLIENT);

			// Create New Connection client.
			m_RemoteNodes[iClientID]->SetAddress(&pkNetPacket->m_kAddress);
			m_RemoteNodes[iClientID]->m_eConnectStatus = NETSTATUS_CONNECTED;
			m_RemoteNodes[iClientID]->m_fLastMessageTime = fEngineTime;
			m_RemoteNodes[iClientID]->m_iNetSpeed = iNetSpeed;
			
			//kNewNode.m_kAddress = pkNetPacket->m_kAddress;
			//kNewNode.m_eConnectStatus = NETSTATUS_CONNECTED;
			//RemoteNodes.push_back(kNewNode);
			m_pkConsole->Printf("Client Connected: %s", m_szAddressBuffer);

			// Tell ZeroFPS Someone has connected.
			iNetID = m_pkZeroFps->Connect(iClientID, szLogin, szPass,iConnectAsEditor);

			// Send Connect Yes.
			kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
			kNetPRespons.Write((unsigned char) ZF_NETCONTROL_JOINYES);
			kNetPRespons.Write( iClientID ); 
			kNetPRespons.Write( iNetID ); 

			kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
			SendRaw(&kNetPRespons);
			
			NetString_ReSendAll();
			
			break;

		case ZF_NETCONTROL_JOINYES:
			// Client can join.
			iClientID = GetFreeClientNum();
			assert(iClientID != ZF_NET_NOCLIENT);

			// Create New Connection client.
			m_RemoteNodes[iClientID]->SetAddress(&pkNetPacket->m_kAddress);
			m_RemoteNodes[iClientID]->m_eConnectStatus = NETSTATUS_CONNECTED;
			m_RemoteNodes[iClientID]->m_fLastMessageTime = fEngineTime;
			
			m_kServerAddress = pkNetPacket->m_kAddress;
			//m_pkConsole->Printf("NetWork::HandleControlMessage(ZF_NETCONTROL_JOINYES)");
			AddressToStr(&pkNetPacket->m_kAddress, m_szAddressBuffer);
			m_pkConsole->Printf("Server Ip: %s", m_szAddressBuffer);

			// Read 			
			int iConnID;
			pkNetPacket->Read( iConnID ); 
			pkNetPacket->Read( iObjId ); 
			m_pkZeroFps->m_iServerConnection	= iConnID;
			m_pkZeroFps->m_iClientEntityID		= iObjId;
			m_pkZeroFps->Connect(iClientID,NULL,NULL,false);
			//m_pkConsole->Printf("ConnID: %d, ObjID: %d", iConnID, iObjId);
			break;

		case ZF_NETCONTROL_JOINNO:
			//m_pkConsole->Printf("NetWork::HandleControlMessage(ZF_NETCONTROL_JOINNO)");
			pkNetPacket->Read_Str( strNoMessage );
			//m_pkConsole->Printf("Connect No: %s", strNoMessage.c_str() );
			break;

		case ZF_NETCONTROL_REQCLIENTID:
			iClientID = GetClientNumber( &pkNetPacket->m_kAddress );
			if(iClientID != ZF_NET_NOCLIENT) {
				kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
				//kNetPRespons.Write((unsigned char) ZF_NETTYPE_CONTROL);
				kNetPRespons.Write((unsigned char) ZF_NETCONTROL_CLIENTID);
				kNetPRespons.Write((int) m_pkZeroFps->m_kClient[iClientID].m_pkObject->m_iEntityID);
				kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
				SendRaw(&kNetPRespons);
				}
				
			break;

		case ZF_NETCONTROL_CLIENTID:
			pkNetPacket->Read(iObjId);
			m_pkZeroFps->m_iClientEntityID = iObjId;
			break;

		case ZF_NETCONTROL_DISCONNECT:
			iClientID = GetClientNumber( &pkNetPacket->m_kAddress );
			if(iClientID != ZF_NET_NOCLIENT) {
				//m_pkConsole->Printf("NetWork::HandleControlMessage(ZF_NETCONTROL_DISCONNECT)");
				m_pkZeroFps->Disconnect(iClientID);
				m_RemoteNodes[iClientID]->m_eConnectStatus = NETSTATUS_DISCONNECT;
				m_RemoteNodes[iClientID]->Clear();
				}

			// Outer side disconnect.
			// Server removes client, client stops sim.
			break;

		case ZF_NETCONTROL_PING:
			kNetPRespons.Clear();
			kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
			kNetPRespons.Write((unsigned char) ZF_NETCONTROL_PONG);
			kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
			SendRaw(&kNetPRespons);
			break;

		case ZF_NETCONTROL_PONG:
			// 
			//cout << "Pong" << endl;
			break;

		case ZF_NETCONTROL_NOP:
			// 
			break;

		case ZF_NETCONTROL_SERVERLISTPAGE:
			MS_GotServers(pkNetPacket);
			break;

		case ZF_NETCONTROL_NETSTRINGS:
			{
#ifdef NET_LOGALL
			GetSystem().Log("net", "*** *** ZF_NETCONTROL_NETSTRINGS *** ***\n");
#endif
			int iStringID;
			pkNetPacket->Read(iStringID);
			g_ZFObjSys.Logf("net", " NetString[%d]\n", iStringID);

			while(iStringID != ZF_NET_NONETSTRING) {
				pkNetPacket->Read_Str( szText );

				m_kStringTable[iStringID].m_bInUse		= true;
				m_kStringTable[iStringID].m_bUpdated	= false;
				m_kStringTable[iStringID].m_NetString	= szText;

#ifdef NET_LOGALL
				g_ZFObjSys.Logf("net", " NetString[%d] = %s\n",iStringID, m_kStringTable[iStringID].m_NetString.c_str());
#endif

				pkNetPacket->Read(iStringID);
				}

				break;
			}


		case ZF_NETCONTROL_REQNETSTRING:
			{
			// When we get a req for a netstring we make a packet with it and send it to all connected clients.
			int iStringID;
			pkNetPacket->Read(iStringID);

			kNetPRespons.Clear();
			kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
			kNetPRespons.Write((char) ZF_NETCONTROL_NETSTRINGS);

			kNetPRespons.Write( iStringID );
			kNetPRespons.Write_Str(m_kStringTable[ iStringID ].m_NetString.c_str());
			
			kNetPRespons.Write( ZF_NET_NONETSTRING );
			kNetPRespons.TargetSetClient(ZF_NET_ALLCLIENT);
			Send2(&kNetPRespons);
		//	SendToAllClients(&kNetPRespons);

			break;

		case ZF_NETCONTROL_ACKREL:
			if(pkNetPacket->m_iClientID == ZF_NET_NOCLIENT)
				break;

			int iRelID, iNumOfAcks;
			pkNetPacket->Read( iNumOfAcks ); 

			for(int i=0; i<iNumOfAcks; i++)
			{
				pkNetPacket->Read( iRelID ); 
				m_RemoteNodes[ pkNetPacket->m_iClientID ]->FreeRelStore( iRelID );
				m_RemoteNodes[pkNetPacket->m_iClientID]->m_kRelSend.erase( iRelID );
				g_ZFObjSys.Logf("netpac", "Recv Ack For: %d\n", iRelID );


				if( m_RemoteNodes[pkNetPacket->m_iClientID]->m_iRelPingIndex == iRelID )
				{
					m_RemoteNodes[pkNetPacket->m_iClientID]->m_fPing = m_pkZeroFps->GetEngineTime() - m_RemoteNodes[pkNetPacket->m_iClientID]->m_fPingSentEngineTime;
					m_RemoteNodes[pkNetPacket->m_iClientID]->m_iRelPingIndex = 0;
				}	
			}

			break;
			}
	}
}

void ZSSNetWork::DevShow_ClientConnections()
{
	float fEngineTime = m_pkZeroFps->GetEngineTime();
	m_pkZeroFps->DevPrintf("conn", "Engine Time: %f", fEngineTime);

	char* pkName = "Die Vim";
	char szAdress[256];

	for(unsigned int i=0; i < m_RemoteNodes.size(); i++) {
		switch(m_RemoteNodes[i]->m_eConnectStatus) {
			case NETSTATUS_CONNECTING:	pkName = "CONNECTING";	break;
			case NETSTATUS_CONNECTED:	pkName = "CONNECTED";	break;
			case NETSTATUS_DISCONNECT:	
				continue;
				//pkName = "DISCONNECT";	
				break;
			}

		AddressToStr(&m_RemoteNodes[i]->m_kAddress,szAdress);

		// R�na antal anv�da buffers.
		int iNumOfUsesBuffers = 0;
		for(int iRel=0; iRel<ZF_NET_MAXREL; iRel++)
		{
			if(m_RemoteNodes[i]->m_akRelPack[iRel]->m_kHeader.m_iPacketType != ZF_NETTYPE_NONE)
				iNumOfUsesBuffers++;
		}

		int iPing = int(m_RemoteNodes[i]->m_fPing * 1000);

		m_pkZeroFps->DevPrintf("conn", " Node[%d] %s %s - %.2f - %d Ping=%d", i, pkName, szAdress,
			( m_RemoteNodes[i]->m_fLastMessageTime + ZF_NET_CONNECTION_TIMEOUT ) - fEngineTime, m_RemoteNodes[i]->m_iReliableRecvOrder,
			iPing);


      // evil stuff
		//m_RemoteNodes[i]->m_kRecvSizeGraph.DrawGraph(0, 50 * i + 200);
		//m_RemoteNodes[i]->m_kRecvGraph.DrawGraph(0, 50 * i + 25 + 200);
	}
}
	
int ZSSNetWork::GetPing(int iConID)
{
	if(iConID < 0 || iConID >= m_RemoteNodes.size())
		return -1;
	
	return int(m_RemoteNodes[iConID]->m_fPing * 1000);
}
	
void ZSSNetWork::DrawConnectionGraphs()
{
	for(unsigned int i=0; i < m_RemoteNodes.size(); i++) {
		//m_RemoteNodes[i]->m_kRecvGraph.DrawGraph(0, 55 * i + 100);
	}
}

void ZSSNetWork::SendAckList(int iClient, vector<int>& kAckList)
{
	if(kAckList.size() == 0)
		return;

	NetPacket kNetPRespons;
	kNetPRespons.Clear();
	kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
	kNetPRespons.Write((unsigned char) ZF_NETCONTROL_ACKREL);
	int iSize = kAckList.size();

	if(iSize >= 200)
	{
		iSize = 200;	
	}

	kNetPRespons.Write( iSize );

	for(unsigned int i=0; i<iSize; i++)
		kNetPRespons.Write( kAckList[i] ); 

	kNetPRespons.m_kAddress = m_RemoteNodes[iClient]->m_kAddress;
	
	if(kAckList.size() > 256)
	{
		cout << " ***** ERROR - ZSSNetWork::SendAckList: - Tell Vim NOW :)." << endl;
	}

	SendRaw(&kNetPRespons);
}

/**	\brief	Checks for incoming packets and return them.
	
	Updates the network by looking for incoming packages and sending them
	the the right place and also checking for timeouts on all connections.
*/
void ZSSNetWork::Run()
{	
	int iRecvBytes = 0;
	m_iBadPackages = 0;	
	float fEngineTime = m_pkZeroFps->GetEngineTime();

	if( m_eNetStatus == NET_NONE )	return;

	// Ping MasterServer
	if(m_eNetStatus == NET_SERVER)
	{
		if(m_kbPublishServer.GetInt() && fEngineTime > m_fMSNextPing)
		{
			m_fMSNextPing = fEngineTime + 60;
			MS_ServerIsActive();
		}
	}

	//reset byte counters for all connections
	unsigned int i;
	for(i=0; i<m_RemoteNodes.size(); i++) 
	{
		m_RemoteNodes[i]->m_iNumOfBytesRecvNetFrame = 0;
	}
	
	NetPacket NetP;
	NetPacket kNetPRespons;
	int iClientID;
	
	m_iMaxIncomingFrame = 25;

	// Recv all packets.
	while(Recv(&NetP)) 
	{
		if(m_iMaxIncomingFrame)
			m_iMaxIncomingFrame--;

		// Update Stats
		iClientID = GetClientNumber(&NetP.m_kAddress);
		NetP.m_iClientID = iClientID;
		iRecvBytes += NetP.m_iLength;

		if(iClientID != ZF_NET_NOCLIENT)
		{
			m_RemoteNodes[iClientID]->m_iNumOfPacketsRecv ++;
			m_RemoteNodes[iClientID]->m_iNumOfBytesRecvNetFrame += NetP.m_iLength;
			m_RemoteNodes[iClientID]->m_fLastMessageTime = fEngineTime;

			if(NetP.m_kData.m_kHeader.m_iOrder != (m_RemoteNodes[iClientID]->m_iLastRecvPacket + 1))
				m_RemoteNodes[iClientID]->m_iOutOfOrderNetFrame ++;
			m_RemoteNodes[iClientID]->m_iLastRecvPacket = NetP.m_kData.m_kHeader.m_iOrder;
			m_RemoteNodes[iClientID]->m_kRecvSizeGraph.PushValue((float)(NetP.m_iLength));
		}
		
		g_ZFObjSys.Logf("netpac", " Order: %d, Size: %d\n", NetP.m_kData.m_kHeader.m_iOrder, NetP.m_iLength);

		switch(NetP.m_kData.m_kHeader.m_iPacketType) 
		{
			// If controll handle_controllpacket.
			case ZF_NETTYPE_CONTROL:
				HandleControlMessage(&NetP);
				break;
	
			// Else give it to zerofps.
			case ZF_NETTYPE_UNREL:
				// We only pass messages from clients to zerofps.
				if(iClientID != ZF_NET_NOCLIENT) {
					m_pkZeroFps->HandleNetworkPacket(&NetP);
				}
				break;

			case ZF_NETTYPE_REL:
				// Only clients can send rel packets.
				if(iClientID != ZF_NET_NOCLIENT) 
				{
					// Check if it is the correct packet.
					if(m_RemoteNodes[iClientID]->m_iReliableRecvOrder == NetP.m_kData.m_kHeader.m_iOrder)
					{					
						static NetPacket temp;
						static bool bHaveSplit = false;
						
						//are we waiting for new package splices?
						if(bHaveSplit)
						{
							//cout<<"got another spliced package, size "<<NetP.m_iLength<<endl;
							//add this package to the split package
							temp.WriteNp(&NetP);
													
							//is this package a split to? if not handle package
							if(NetP.m_kData.m_kHeader.m_iSplit == 0)	// False
							{
								//cout<<"got the final package in the spliced package, total size "<<temp.m_iLength<<endl;
								temp.m_iPos = 0;
								m_pkZeroFps->HandleNetworkPacket(&temp);
								bHaveSplit = false;
							}
						}
						else
						{
							//is this package a spliced package?
							if(NetP.m_kData.m_kHeader.m_iSplit != 0)	// True
							{
								//cout<<"got the first package in a spliced package, size "<<NetP.m_iLength<<endl;
								bHaveSplit = true;
							
								temp.Clear();							
								temp.m_iClientID = NetP.m_iClientID;
								temp.m_kData.m_kHeader = NetP.m_kData.m_kHeader;
								temp.WriteNp(&NetP);
							}
							else
							{
								//not a spliced package
								m_pkZeroFps->HandleNetworkPacket(&NetP);	
							}
						}						
						
						
						m_RemoteNodes[iClientID]->m_iReliableRecvOrder++;
						m_RemoteNodes[iClientID]->m_kRelAckList.push_back( NetP.m_kData.m_kHeader.m_iOrder );
						m_RemoteNodes[iClientID]->FreeRelRecv( NetP.m_kData.m_kHeader.m_iOrder ); 
					}
					else
					{
						//count nr of bad packages this frame
						m_iBadPackages++;
					
						if(NetP.m_kData.m_kHeader.m_iOrder < m_RemoteNodes[iClientID]->m_iReliableRecvOrder)
						{
							//cout << "Duplicate Message: ";
							//cout << NetP.m_kData.m_kHeader.m_iOrder - m_RemoteNodes[iClientID]->m_iReliableRecvOrder << endl;
							g_ZFObjSys.Logf("netpac", "Duplicate Message: %d\n", NetP.m_kData.m_kHeader.m_iOrder - m_RemoteNodes[iClientID]->m_iReliableRecvOrder);
							m_RemoteNodes[iClientID]->m_kRelAckList.push_back( NetP.m_kData.m_kHeader.m_iOrder );
							break;	// Evil Duplicate, throw away.
						}

						if(NetP.m_kData.m_kHeader.m_iOrder > m_RemoteNodes[iClientID]->m_iReliableRecvOrder)
						{
							g_ZFObjSys.Logf("netpac", "Out Of Order Message %d while waiting for %d - ",NetP.m_kData.m_kHeader.m_iOrder, m_RemoteNodes[iClientID]->m_iReliableRecvOrder);
							g_ZFObjSys.Logf("netpac", "%d\n", NetP.m_kData.m_kHeader.m_iOrder - m_RemoteNodes[iClientID]->m_iReliableRecvOrder);

							//cout << "Out Of Order Message: " <<  << " while waiting for: " <<  << " : ";
							//cout <<  << endl;
							
							int iRecvIndex = m_RemoteNodes[iClientID]->GetFreeRelRecv();
							if(iRecvIndex != -1)
							{
								ZFNetPacketData* pkRelPak = m_RemoteNodes[ iClientID ]->m_akRelPackRecv[iRecvIndex];
								memcpy(pkRelPak, &NetP.m_kData, sizeof( ZFNetPacketData ));
								m_RemoteNodes[iClientID]->m_aiRelPackRecvSize[iRecvIndex] = NetP.m_iLength;
							}

							break;
						}
					}
				}
				break;
			
			default:
				cout << "galet paket:" << iClientID << " o:" << NetP.m_kData.m_kHeader.m_iOrder<<" t:" << int(NetP.m_kData.m_kHeader.m_iPacketType) <<" s:"<<NetP.m_iLength<<endl;
				g_ZFObjSys.Logf("netpac", " UnKnown Packet: From: %d Order: %d: Type: %d Size: %d\n",
					iClientID, 
					NetP.m_kData.m_kHeader.m_iOrder,
					NetP.m_kData.m_kHeader.m_iPacketType,
					NetP.m_iLength);

		}
	}

	// Check for any lost connections
	for(i=0; i<m_RemoteNodes.size(); i++)
	{
		if(m_RemoteNodes[i]->m_eConnectStatus == NETSTATUS_DISCONNECT)
			continue;

		m_RemoteNodes[i]->m_iNumOfBytesRecv += m_RemoteNodes[i]->m_iNumOfBytesRecvNetFrame;
		m_RemoteNodes[i]->m_kRecvGraph.AddValue( float(m_RemoteNodes[i]->m_iNumOfBytesRecvNetFrame ));
		m_RemoteNodes[i]->m_iNumOfBytesRecvNetFrame = 0;
	
		// Refresh num of recd bytes graphs
		if(fEngineTime > m_fStatsUpdate) 
		{
			m_RemoteNodes[i]->m_kRecvGraph.NextValue( );
		}

		// Disconnect if connection is flooded
		if(m_RemoteNodes[i]->m_bIsFlood)
		{
			m_pkZeroFps->Disconnect(i);
			cout << "Connection to client " << i << " was flooded." << endl;
			m_pkConsole->Printf("Connection to %d was flooded.", i);
			m_RemoteNodes[i]->m_eConnectStatus = NETSTATUS_DISCONNECT;
			m_RemoteNodes[i]->Clear();
		}

		if(fEngineTime > ( m_RemoteNodes[i]->m_fLastMessageTime + ZF_NET_CONNECTION_TIMEOUT )) //dvoid �ndra fr�n 60 till zf_net_ blaa  
		{
			// Time out this connection.
			m_pkZeroFps->Disconnect(i);
			cout << "Connection to client " << i << " timed out." << endl;
			m_pkConsole->Printf("Connection to %d timed out.", i);
			m_RemoteNodes[i]->m_eConnectStatus = NETSTATUS_DISCONNECT;
			m_RemoteNodes[i]->Clear();
		}


		//get send size
	 	int iSendSize;		
		if(m_pkZeroFps->GetSyncNetwork())
			iSendSize = int( (GetClientNetSpeed(i) / m_pkZeroFps->GetSystemFps()) * 0.5f);
 		else	
			iSendSize = int( (GetClientNetSpeed(i) / m_pkZeroFps->GetNetworkFps()) * 0.5f);		
				
		int iNumOfResends = 0;	 					 		
 		int iBytes = 0;	 				
		for(set<int>::iterator it = m_RemoteNodes[i]->m_kRelSend.begin(); it != m_RemoteNodes[i]->m_kRelSend.end(); it++)
		{
			int iRel = m_RemoteNodes[i]->GetRel( (*it) );

			if(( m_RemoteNodes[i]->m_akRelPackSendTime[iRel] + 0.25 ) < fEngineTime)
			{
 				iBytes += m_RemoteNodes[i]->m_aiRelPackSize[iRel];
				iNumOfResends++;
				
				m_RemoteNodes[i]->m_akRelPackSendTime[iRel] = fEngineTime;
				SendUDP(m_RemoteNodes[i]->m_akRelPack[iRel], m_RemoteNodes[i]->m_aiRelPackSize[iRel], &m_RemoteNodes[i]->m_kAddress);
				g_ZFObjSys.Logf("netpac", "Resending Packet: %d\n", m_RemoteNodes[i]->m_akRelPack[iRel]->m_kHeader.m_iOrder);
			}

			//if(iNumOfResends >= 20)	
			if(iBytes > iSendSize) 
			{
				//cout << "Hit Resend Limit("<<iSendSize<<") for client: " << i <<" bytes sent:"<<iBytes<< " packages:"<<iNumOfResends<< " Num of packets in que: " << m_RemoteNodes[i]->m_kRelSend.size() <<  endl;
				break;
			}
		}

		SendAckList(i, m_RemoteNodes[i]->m_kRelAckList);
		m_RemoteNodes[i]->m_kRelAckList.clear();
	}

	if(fEngineTime > m_fStatsUpdate)
		m_fStatsUpdate = fEngineTime + float(1.0);	

	Send_NetStrings();
	NetString_Refresh();
}

bool ZSSNetWork::AddressToStr(IPaddress* pkAddress, char* szString)
{
	int iPort = 0;
	iPort = iPort | ((pkAddress->port >> 8) & 0xff);  
	iPort = iPort | ((pkAddress->port << 8) & 0xff00);  

	sprintf(szString, "%d.%d.%d.%d:%d", (pkAddress->host) & 0xff, (pkAddress->host >> 8) & 0xff,
				(pkAddress->host >> 16) & 0xff, (pkAddress->host >> 24) & 0xff, iPort);
	return true;
}

bool ZSSNetWork::StrToAddress(const char* szString, IPaddress* pkAddress)
{
	int ha1, ha2, ha3, ha4, hp;
	int ip_addr;

	// Scan addr string.
	int iScanResultat = sscanf(szString, "%d.%d.%d.%d:%d", &ha1, &ha2, &ha3, &ha4, &hp);
	if(iScanResultat < 4)
		return false;

	// Build host ip.
	ip_addr = (ha1 << 24) | (ha2 << 16) | (ha3 << 8) | ha4;
	SDLNet_Write32(ip_addr, &pkAddress->host);	
	SDLNet_Write16(hp, &pkAddress->port);	
	
	return true;
}

bool ZSSNetWork::IsValidIPAddress( const char* szString )
{
	int ha1, ha2, ha3, ha4, hp;
	int iScanResultat = sscanf(szString, "%d.%d.%d.%d:%d", &ha1, &ha2, &ha3, &ha4, &hp);

	if(iScanResultat < 4)
		return false;

	if(ha1 < 0 || ha1 > 255)	return false;
	if(ha2 < 0 || ha2 > 255)	return false;
	if(ha3 < 0 || ha3 > 255)	return false;
	if(ha4 < 0 || ha4 > 255)	return false;

	if(iScanResultat == 5)
	{
		if(hp < 0 || hp > 65536)	return false;
	}

	return true;
}


// Force Disconnect on all nodes.
void ZSSNetWork::DisconnectAll()
{
	if(!m_pkSocket)	return;

	// Create a disconnect packet and sent it to all connected.
	NetPacket kNetPRespons;
	kNetPRespons.Clear();
	kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
	kNetPRespons.Write((unsigned char) ZF_NETCONTROL_DISCONNECT);
	kNetPRespons.TargetSetClient(ZF_NET_ALLCLIENT);
	Send2(&kNetPRespons);

	// Now inform the application that everyone is gone.
	for(unsigned int i=0; i<m_RemoteNodes.size(); i++)
	{
		if(m_RemoteNodes[i]->m_eConnectStatus == NETSTATUS_DISCONNECT)
			continue;

		m_pkZeroFps->Disconnect(i);
		m_RemoteNodes[i]->m_eConnectStatus = NETSTATUS_DISCONNECT;
		m_RemoteNodes[i]->Clear();
	}
}

void ZSSNetWork::RunCommand(int cmdid, const ConCommandLine* kCommand)
{
	float fMax;

	switch(cmdid) {
		case FID_NETGMAX:
			if(kCommand->m_kSplitCommand.size() <= 1)
				return;
			
			fMax = (float) (atoi(kCommand->m_kSplitCommand[1].c_str()));

			for(unsigned int i=0;i<m_RemoteNodes.size(); i++)
				m_RemoteNodes[i]->m_kRecvGraph.SetMinMax(0, fMax);
			break;

		case FID_DNS:
			//DisconnectAll();

			
			if(kCommand->m_kSplitCommand.size() <= 1)
				return;

			char szIP[256];
			IPaddress kLookUpIP;

			if( DnsLookUp( kCommand->m_kSplitCommand[1].c_str(),kLookUpIP ))
			{
				AddressToStr(&kLookUpIP, szIP);
				m_pkConsole->Printf("Org: %s. New: %s", kCommand->m_kSplitCommand[1].c_str(), szIP);
			}
			else
				m_pkConsole->Printf("Failed to find ip for: %s.", kCommand->m_kSplitCommand[1].c_str());
			
			break;
		}	

}

bool ZSSNetWork::DnsLookUp(const char* szHost,IPaddress& kIp)
{
	kIp.host	=	INADDR_NONE;
	kIp.port	=  0;

	if(!szHost)
		return false;

	if( SDLNet_ResolveHost(&kIp, szHost, 0) == -1)
		return false;

	return true;
}

void ZSSNetWork::SetGameName(char* szGameName)
{
	if(strlen(szGameName) >= ZF_MAX_GAMENAME)
		return;

   strcpy(m_szGameName, szGameName);
}

void ZSSNetWork::Ping()
{
	NetPacket kNetPRespons;
	kNetPRespons.Clear();
	kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
	kNetPRespons.Write((unsigned char) ZF_NETCONTROL_PING);

	for(unsigned int i=0; i<m_RemoteNodes.size(); i++)
	{
		if(m_RemoteNodes[i]->m_eConnectStatus == NETSTATUS_DISCONNECT)
			continue;
		kNetPRespons.m_kAddress = m_RemoteNodes[i]->m_kAddress;
		SendRaw(&kNetPRespons);
	}
}



