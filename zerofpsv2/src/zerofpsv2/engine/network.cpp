#include "network.h"
#include "zerofps.h"

#include "../basic/zfsystem.h"

//#define	NET_LOGALL	

NetWork* g_pkNetWork;

NetWork::NetWork()
: ZFSubSystem("NetWork")
{
	// Create Log Files.
	GetSystem().Log_Create("net");
	GetSystem().Log_Create("netpac");

	strcpy(m_szServerName,"No Name");
	m_bAcceptClientConnections = false;
	m_pkSocket						= NULL;
	m_eNetStatus					= NET_NONE;

	// Set Default values
	m_fConnectTimeOut		= ZF_NET_CONNECTION_TIMEOUT;
	m_iMaxNumberOfNodes	= 0;
	m_iMaxSendSize			= 1000;

	// Register Variables
	RegisterVariable("n_connecttimeout",	&m_fConnectTimeOut,		CSYS_FLOAT);	
	RegisterVariable("n_sendsize",			&m_iMaxSendSize,		CSYS_INT);		
	
	// Register Commands
	Register_Cmd("n_netgmax", FID_NETGMAX);

	m_kStringTable.resize( ZF_NET_MAXSTRINGS );
	for(int i=0; i < ZF_NET_MAXSTRINGS; i++) {
		m_kStringTable[i].m_bInUse			= false;
		m_kStringTable[i].m_bUpdated		= false;
		m_kStringTable[i].m_bNeedUpdate	= false;
		m_kStringTable[i].m_NetString = "";
		}
}

bool NetWork::StartUp()	
{ 
	m_pkConsole	= static_cast<Console*>(GetSystem().GetObjectPtr("Console"));
	m_pkZeroFps	= static_cast<ZeroFps*>(GetSystem().GetObjectPtr("ZeroFps"));

	g_pkNetWork  = this;

	int iInitRes = SDLNet_Init();

#ifdef NET_LOGALL
	GetSystem().Log("net",		"NetWork SubSystem Startup:\n");
	GetSystem().Log("netpac",	"NetWork Packet Log:\n");
	Logf("net", "SDLNet_Init(): %d\n", iInitRes);
#endif

	return true; 
}

bool NetWork::ShutDown() {	return true;	}
bool NetWork::IsValid()	 { return true;	}

NetWork::~NetWork()
{
#ifdef NET_LOGALL
	for(int i=0; i < ZF_NET_MAXSTRINGS; i++) {
		if(m_kStringTable[i].m_bInUse)
			Logf("net", "NetString[%d]: = %s\n", i, m_kStringTable[i].m_NetString.c_str());
		}
#endif

	DisconnectAll();
	CloseSocket();
	SDLNet_Quit();

#ifdef NET_LOGALL
	GetSystem().Log("net", "NetWork SubSystem ShutDown:\n");
#endif
}

string NetWork::GetLocalIP()
{
//	char MyIp[256];
//	m_kLocalIP kIp;
//	AddressToStr(kIp, MyIp);
	return string("ost");

}

// NetStrings are strings that are sent as int's over network. 
int NetWork::NetString_GetFree()
{
	for(int i=0; i < ZF_NET_MAXSTRINGS; i++) {
		if(m_kStringTable[i].m_bInUse == false)
			return i;
		}
	return ZF_NET_NONETSTRING;
}

int NetWork::NetString_Add(const char* szString)
{
	string strTest;
	strTest = /*string("A Cool ") +*/ string(szString);
#ifdef NET_LOGALL
	Logf("net", "NetString Add: = %s\n", strTest.c_str());
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

int NetWork::NetString_GetIndex(const char* szString)
{
	for(int i=0; i < ZF_NET_MAXSTRINGS; i++) {
		if(m_kStringTable[i].m_bInUse && (strcmp(szString, m_kStringTable[i].m_NetString.c_str()) == 0) )
			return i;
		}

	return ZF_NET_NONETSTRING;
}

string NetWork::NetString_GetString(int iIndex)
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

void NetWork::NetString_ReSendAll()
{
	for(int i=0; i < ZF_NET_MAXSTRINGS; i++)
		m_kStringTable[i].m_bUpdated = true;
}

// Check all netstrings for updates and send them to the other side.
void NetWork::Send_NetStrings()
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
			Logf("net", "Write NetString[%d]: = %s\n", i, m_kStringTable[i].m_NetString.c_str());
#endif
			}

		// Send if packet to large.
		if(NP.m_iPos >= 512) {
			NP.Write( ZF_NET_NONETSTRING );
#ifdef NET_LOGALL
			Logf("net", "Write NetStrings: Order Client 0 : %d", m_RemoteNodes[0].m_iNumOfPacketsSent );
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
	Logf("net", "Write NetStrings: Order Client 0 : %d", m_RemoteNodes[0].m_iNumOfPacketsSent );
#endif
	NP.TargetSetClient(ZF_NET_ALLCLIENT);
	//SendToAllClients(&NP);
	Send2(&NP);
}

bool NetWork::NetStringIsUpdated()
{
	for(int i=0; i < ZF_NET_MAXSTRINGS; i++) {
		if(m_kStringTable[i].m_bInUse && m_kStringTable[i].m_bUpdated)
			return true;
		}

	return false;
}

void NetWork::NetString_Refresh()
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

void NetWork::NetString_Request(int iIndex)
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

int NetWork::GetNumOfClients(void)
{
	int iNumOfClients = 0;
	for(unsigned int i=0; i < m_RemoteNodes.size(); i++) {
		if(m_RemoteNodes[i].m_eConnectStatus != NETSTATUS_DISCONNECT)
			iNumOfClients++;
		}

	return iNumOfClients;
}

bool NetWork::IsAddressEquals(IPaddress* pkAdr1, IPaddress* pkAdr2)
{
	if(pkAdr1->host != pkAdr2->host)	return false;
	if(pkAdr1->port != pkAdr2->port)	return false;

	return true;
}

int NetWork::GetClientNumber(IPaddress* pkAddress)
{
	for(unsigned int i=0; i < m_RemoteNodes.size(); i++) {
		if(IsAddressEquals(pkAddress, &m_RemoteNodes[i].m_kAddress))
			return i;
		}

	return ZF_NET_NOCLIENT;
}

void NetWork::SetMaxNodes(int iMaxNode)
{
	m_RemoteNodes.resize(iMaxNode);
	m_iMaxNumberOfNodes = iMaxNode;
}

int NetWork::GetFreeClientNum()
{
	for(int i=0; m_RemoteNodes.size(); i++) {
		if(m_RemoteNodes[i].m_eConnectStatus == NETSTATUS_DISCONNECT)
			return i;
		}

	return ZF_NET_NOCLIENT;
}

bool NetWork::IsConnected(int iId)
{
	if(m_RemoteNodes[iId].m_eConnectStatus == NETSTATUS_CONNECTED)
		return true;
	else
		return false;
}


void NetWork::StartSocket(bool bStartServer)
{
	if(m_pkSocket) {
		cout << "StartSocket: Socket is already open" << endl;
		return;
		}

	int iSocketNum = 0;
	if(bStartServer)	iSocketNum = 4242;
	m_pkSocket = SDLNet_UDP_Open( iSocketNum );
	if(!m_pkSocket) {
		cout << "SDLNet_UDP_Open: " <<  SDLNet_GetError() << endl;
		return;
		}
	
}

void NetWork::CloseSocket()
{
	if(!m_pkSocket)	return;
	SDLNet_UDP_Close(m_pkSocket);
	m_pkSocket = NULL;
}

void NetWork::ServerStart(void)
{
	StartSocket(true);
	m_eNetStatus = NET_SERVER;
}

void NetWork::ServerEnd(void)
{
	DisconnectAll();
	CloseSocket();
	m_eNetStatus = NET_NONE;
}

void NetWork::ClientStart(const char* szIp, const char* szLogin, const char* szPass)
{
	if(m_eNetStatus == NET_SERVER)
		return;

	StartSocket(false);
	m_eNetStatus = NET_CLIENT;

	NetPacket NetP;

	NetP.Clear();
	NetP.SetTarget(szIp);
	NetP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
	NetP.Write((char) ZF_NETCONTROL_JOIN);
	NetP.Write_Str(szLogin);
	NetP.Write_Str(szPass);
	SendRaw(&NetP);
}

/**	\brief	Checks for incoming packets and return them.
	
	Checks if any packet have arrived and if so puts it in the netpacket,
*/
bool NetWork::Recv(NetPacket* pkNetPacket)
{
	pkNetPacket->Clear();

	UDPpacket kPacket;
	kPacket.channel	= -1;
	kPacket.data		= (unsigned char*)&pkNetPacket->m_kData;
	kPacket.len			= 0;
	kPacket.maxlen		= MAX_PACKET_SIZE;

	if(SDLNet_UDP_Recv(m_pkSocket, &kPacket)) {
		pkNetPacket->m_kAddress = kPacket.address;
		pkNetPacket->m_iLength	= kPacket.len - sizeof(ZFNetHeader);
		return true;
		}

	return false;
}

/**	\brief	Sends a package.
	
*/
bool NetWork::SendRaw(NetPacket* pkNetPacket)
{
	// Check that it is a valid package.
	if(pkNetPacket->m_kData.m_kHeader.m_iPacketType == 204)
		assert(0);

	// Update order num of package.
	if(pkNetPacket->m_iClientID != ZF_NET_NOCLIENT) {
		pkNetPacket->m_kData.m_kHeader.m_iOrder = m_RemoteNodes[ pkNetPacket->m_iClientID ].m_iNumOfPacketsSent;
		m_RemoteNodes[pkNetPacket->m_iClientID].m_iNumOfPacketsSent ++;
		m_RemoteNodes[pkNetPacket->m_iClientID].m_iNumOfBytesSent += pkNetPacket->m_iLength;
		
		//m_RemoteNodes[pkNetPacket->m_iClientID].m_RelPackages.push_back( pkNetPacket->m_kData );
		//m_RemoteNodes[pkNetPacket->m_iClientID].m_RelPackages.pop_back( );

		}

	// Send it.
	UDPpacket kPacket;
	kPacket.channel	= -1;
	kPacket.data		= (unsigned char*)&pkNetPacket->m_kData;
	kPacket.len			= pkNetPacket->m_iLength + sizeof(ZFNetHeader);
	kPacket.maxlen		= MAX_PACKET_SIZE;
	kPacket.address	= pkNetPacket->m_kAddress;

	int iRes = SDLNet_UDP_Send(m_pkSocket, -1, &kPacket);

	return true;
}

bool NetWork::Send2(NetPacket* pkNetPacket)
{
	// If we have any clients to send to.
	if(pkNetPacket->m_iTargetClients.size()) {
		for(int i=0; i<pkNetPacket->m_iTargetClients.size(); i++) {
			if(m_RemoteNodes[ pkNetPacket->m_iTargetClients[i] ].m_eConnectStatus != NETSTATUS_CONNECTED)
				continue;
			
			pkNetPacket->m_iClientID = pkNetPacket->m_iTargetClients[i];
			pkNetPacket->m_kAddress = m_RemoteNodes[ pkNetPacket->m_iClientID ].m_kAddress;

			SendRaw(pkNetPacket);
			}

		return true;
		}
	
	else {
		pkNetPacket->m_iClientID = ZF_NET_NOCLIENT;
		SendRaw(pkNetPacket);
		return true;
		}
}


void NetWork::HandleControlMessage(NetPacket* pkNetPacket)
{
	int iObjId;
	char szText[256];

	float fEngineTime = m_pkZeroFps->GetEngineTime();

	int iClientID;
	unsigned char ucControlType;
	pkNetPacket->Read(ucControlType);

	NetPacket kNetPRespons;
	RemoteNode kNewNode;

	char szLogin[64];
	char szPass[64];

	switch(ucControlType) {
		// If controll handle_controllpacket.
		case ZF_NETCONTROL_LIST:
			m_pkConsole->Printf("NetWork::HandleControlMessage(ZF_NETCONTROL_LIST)");
			// Broadcast to find servers.
			// Server respons with name.
			break;

		case ZF_NETCONTROL_JOIN:
			// Sent to join a server.
			m_pkConsole->Printf("NetWork::HandleControlMessage(ZF_NETCONTROL_JOIN)");
			AddressToStr(&pkNetPacket->m_kAddress, m_szAddressBuffer);
			m_pkConsole->Printf("Ip: %s", m_szAddressBuffer);

			// Server respons with yes/no.
			kNetPRespons.Clear();

			pkNetPacket->Read_Str(szLogin);
			pkNetPacket->Read_Str(szPass);

			if(GetNumOfClients() == m_iMaxNumberOfNodes) {
				m_pkConsole->Printf("Join Ignored: To many connected clients.");
				kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
				//kNetPRespons.Write((unsigned char) ZF_NETTYPE_CONTROL);
				kNetPRespons.Write((unsigned char) ZF_NETCONTROL_JOINNO);
				kNetPRespons.Write_Str("There server is full.");
				kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
				SendRaw(&kNetPRespons);
				}
			else {
				if(! m_pkZeroFps->PreConnect(pkNetPacket->m_kAddress, szLogin, szPass, szText)) {
					m_pkConsole->Printf("Join Ignored: To many connected clients.");
					kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
					//kNetPRespons.Write((unsigned char) ZF_NETTYPE_CONTROL);
					kNetPRespons.Write((unsigned char) ZF_NETCONTROL_JOINNO);
					kNetPRespons.Write_Str("There server is full.");
					kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
					SendRaw(&kNetPRespons);
					break;
					}

				iClientID = GetFreeClientNum();
				assert(iClientID != ZF_NET_NOCLIENT);

				// Create New Connection client.
				m_RemoteNodes[iClientID].SetAddress(&pkNetPacket->m_kAddress);
				m_RemoteNodes[iClientID].m_eConnectStatus = NETSTATUS_CONNECTED;
				m_RemoteNodes[iClientID].m_fLastMessageTime = fEngineTime;

				//kNewNode.m_kAddress = pkNetPacket->m_kAddress;
				//kNewNode.m_eConnectStatus = NETSTATUS_CONNECTED;
				//RemoteNodes.push_back(kNewNode);
				m_pkConsole->Printf("Client Connected: %s", m_szAddressBuffer);

				// Tell ZeroFPS Someone has connected.
				int iNetID = m_pkZeroFps->Connect(iClientID, szLogin, szPass);

				// Send Connect Yes.
				kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
				kNetPRespons.Write((unsigned char) ZF_NETCONTROL_JOINYES);
				kNetPRespons.Write( iClientID ); 
				kNetPRespons.Write( iNetID ); 

				kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
				SendRaw(&kNetPRespons);
				
				NetString_ReSendAll();
				}
			
			break;

		case ZF_NETCONTROL_JOINYES:
			// Client can join.
			iClientID = GetFreeClientNum();
			assert(iClientID != ZF_NET_NOCLIENT);

			// Create New Connection client.
			m_RemoteNodes[iClientID].SetAddress(&pkNetPacket->m_kAddress);
			m_RemoteNodes[iClientID].m_eConnectStatus = NETSTATUS_CONNECTED;
			m_RemoteNodes[iClientID].m_fLastMessageTime = fEngineTime;
			
			m_kServerAddress = pkNetPacket->m_kAddress;
			m_pkConsole->Printf("NetWork::HandleControlMessage(ZF_NETCONTROL_JOINYES)");
			AddressToStr(&pkNetPacket->m_kAddress, m_szAddressBuffer);
			m_pkConsole->Printf("Server Ip: %s", m_szAddressBuffer);

			// Read 			
			int iConnID;
			pkNetPacket->Read( iConnID ); 
			pkNetPacket->Read( iObjId ); 
			m_pkZeroFps->m_iServerConnection	= iConnID;
			m_pkZeroFps->m_iRTSClientObject		= iObjId;
			m_pkZeroFps->Connect(iClientID,NULL,NULL);
			m_pkConsole->Printf("ConnID: %d, ObjID: %d", iConnID, iObjId);
			break;

		case ZF_NETCONTROL_JOINNO:
			m_pkConsole->Printf("NetWork::HandleControlMessage(ZF_NETCONTROL_JOINNO)");
			pkNetPacket->Read_Str( szText );
			m_pkConsole->Printf("Connect No: %s", szText );
			break;

		case ZF_NETCONTROL_REQCLIENTID:
			iClientID = GetClientNumber( &pkNetPacket->m_kAddress );
			if(iClientID != ZF_NET_NOCLIENT) {
				kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
				//kNetPRespons.Write((unsigned char) ZF_NETTYPE_CONTROL);
				kNetPRespons.Write((unsigned char) ZF_NETCONTROL_CLIENTID);
				kNetPRespons.Write((int) m_pkZeroFps->m_kClient[iClientID].m_pkObject->iNetWorkID);
				kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
				SendRaw(&kNetPRespons);
				}
				
			break;

		case ZF_NETCONTROL_CLIENTID:
			pkNetPacket->Read(iObjId);
			m_pkZeroFps->m_iRTSClientObject = iObjId;
			break;

		case ZF_NETCONTROL_DISCONNECT:
			iClientID = GetClientNumber( &pkNetPacket->m_kAddress );
			if(iClientID != ZF_NET_NOCLIENT) {
				m_pkConsole->Printf("NetWork::HandleControlMessage(ZF_NETCONTROL_DISCONNECT)");
				m_pkZeroFps->Disconnect(iClientID);
				m_RemoteNodes[iClientID].m_eConnectStatus = NETSTATUS_DISCONNECT;
				}

			// Outer side disconnect.
			// Server removes client, client stops sim.
			break;

		case ZF_NETCONTROL_NOP:
			// 
			break;

		case ZF_NETCONTROL_NETSTRINGS:
			{
#ifdef NET_LOGALL
			GetSystem().Log("net", "*** *** ZF_NETCONTROL_NETSTRINGS *** ***\n");
#endif
			int iStringID;
			pkNetPacket->Read(iStringID);
			Logf("net", " NetString[%d]\n", iStringID);

			while(iStringID != ZF_NET_NONETSTRING) {
				pkNetPacket->Read_Str( szText );

				m_kStringTable[iStringID].m_bInUse		= true;
				m_kStringTable[iStringID].m_bUpdated	= false;
				m_kStringTable[iStringID].m_NetString	= szText;

#ifdef NET_LOGALL
				Logf("net", " NetString[%d] = %s\n",iStringID, m_kStringTable[iStringID].m_NetString.c_str());
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

			NetPacket kNetPRespons;
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
			}
	}
}

void NetWork::DevShow_ClientConnections()
{
	float fEngineTime = m_pkZeroFps->GetEngineTime();
	m_pkZeroFps->DevPrintf("conn", "Engine Time: %f", fEngineTime);

	char* pkName = "Die Vim";
	char szAdress[256];

	for(unsigned int i=0; i < m_RemoteNodes.size(); i++) {
		switch(m_RemoteNodes[i].m_eConnectStatus) {
			case NETSTATUS_CONNECTING:	pkName = "CONNECTING";	break;
			case NETSTATUS_CONNECTED:	pkName = "CONNECTED";	break;
			case NETSTATUS_DISCONNECT:	
				continue;
				//pkName = "DISCONNECT";	
				break;
			}

		AddressToStr(&m_RemoteNodes[i].m_kAddress,szAdress);

		m_pkZeroFps->DevPrintf("conn", " Node[%d] %s %s %d/%d %d/%d - %f - %d", i, pkName, szAdress,
			m_RemoteNodes[i].m_iNumOfPacketsSent, m_RemoteNodes[i].m_iNumOfBytesSent,
			m_RemoteNodes[i].m_iNumOfPacketsRecv, m_RemoteNodes[i].m_iNumOfBytesRecv,
			( m_RemoteNodes[i].m_fLastMessageTime + ZF_NET_CONNECTION_TIMEOUT ) - fEngineTime, m_RemoteNodes[i].m_iOutOfOrderNetFrame);

		//m_RemoteNodes[i].m_kRecvGraph.DrawGraph(0, 50 * i + 200);
	}
}
	
void NetWork::DrawConnectionGraphs()
{
	for(unsigned int i=0; i < m_RemoteNodes.size(); i++) {
		//m_RemoteNodes[i].m_kRecvGraph.DrawGraph(0, 55 * i + 100);
	}
}

/**	\brief	Checks for incoming packets and return them.
	
	Updates the network by looking for incoming packages and sending them
	the the right place and also checking for timeouts on all connections.
*/
void NetWork::Run()
{	
	int iRecvBytes = 0;

	float fEngineTime = m_pkZeroFps->GetEngineTime();

	if( m_eNetStatus == NET_NONE )	return;

	unsigned int i;
	for(i=0; i<m_RemoteNodes.size(); i++) {
		m_RemoteNodes[i].m_iNumOfBytesRecvNetFrame = 0;
		}
	
	NetPacket NetP;
	int iClientID;

	// Recv all packets.
	while(Recv(&NetP)) {
		// Update Stats
		iClientID = GetClientNumber(&NetP.m_kAddress);
		NetP.m_iClientID = iClientID;
		iRecvBytes += NetP.m_iLength;

		if(iClientID != ZF_NET_NOCLIENT) {
			m_RemoteNodes[iClientID].m_iNumOfPacketsRecv ++;
			m_RemoteNodes[iClientID].m_iNumOfBytesRecvNetFrame += NetP.m_iLength;
			m_RemoteNodes[iClientID].m_fLastMessageTime = fEngineTime;

			if(NetP.m_kData.m_kHeader.m_iOrder != (m_RemoteNodes[iClientID].m_iLastRecvPacket + 1))
				m_RemoteNodes[iClientID].m_iOutOfOrderNetFrame ++;
			m_RemoteNodes[iClientID].m_iLastRecvPacket = NetP.m_kData.m_kHeader.m_iOrder;
			}
		
		switch(NetP.m_kData.m_kHeader.m_iPacketType) {
			// If controll handle_controllpacket.
			case ZF_NETTYPE_CONTROL:
				HandleControlMessage(&NetP);
				break;
	
			// Else give it to zerofps.
			case ZF_NETTYPE_UNREL:
				m_pkZeroFps->HandleNetworkPacket(&NetP);
				break;

			default:
				Logf("netpac", " UnKnown Packet: From: %d Order: %d: Type: %d Size: %d\n",
					iClientID, 
					NetP.m_kData.m_kHeader.m_iOrder,
					NetP.m_kData.m_kHeader.m_iPacketType,
					NetP.m_iLength);

			}
		}

	// Check for any lost connections
	for(i=0; i<m_RemoteNodes.size(); i++) {
		if(m_RemoteNodes[i].m_eConnectStatus == NETSTATUS_DISCONNECT)
			continue;

		m_RemoteNodes[i].m_iNumOfBytesRecv += m_RemoteNodes[i].m_iNumOfBytesRecvNetFrame;
		m_RemoteNodes[i].m_kRecvGraph.AddValue( float(m_RemoteNodes[i].m_iNumOfBytesRecvNetFrame ));
		m_RemoteNodes[i].m_iNumOfBytesRecvNetFrame = 0;
	
		// Refresh num of recd bytes graphs
		if(fEngineTime > m_fStatsUpdate) {
			m_RemoteNodes[i].m_kRecvGraph.NextValue( );
			}

		if(fEngineTime > ( m_RemoteNodes[i].m_fLastMessageTime + 60 )) {
			// Time out this connection.
			m_pkZeroFps->Disconnect(i);
			m_pkConsole->Printf("Connection to %d timed out.", i);
			m_RemoteNodes[i].m_eConnectStatus = NETSTATUS_DISCONNECT;
			}
		}

	if(fEngineTime > m_fStatsUpdate)
		m_fStatsUpdate = fEngineTime + float(1.0);	

	Send_NetStrings();
	NetString_Refresh();
}

bool NetWork::AddressToStr(IPaddress* pkAddress, char* szString)
{
	int iPort = 0;
	iPort = iPort | ((pkAddress->port >> 8) & 0xff);  
	iPort = iPort | ((pkAddress->port << 8) & 0xff00);  

	sprintf(szString, "%d.%d.%d.%d:%d", (pkAddress->host) & 0xff, (pkAddress->host >> 8) & 0xff,
				(pkAddress->host >> 16) & 0xff, (pkAddress->host >> 24) & 0xff, iPort);
	return true;
}

/*
void NetWork::SendToClient(int iClient, NetPacket* pkNetPacket)
{
	if(m_RemoteNodes[iClient].m_eConnectStatus != NETSTATUS_CONNECTED) {
		cout << "Warning: Try to send to unconnected client " << iClient << endl;
		return;
		}

	pkNetPacket->m_kAddress = m_RemoteNodes[iClient].m_kAddress;
	pkNetPacket->m_kData.m_kHeader.m_iOrder = m_RemoteNodes[iClient].m_iNumOfPacketsSent;

	//m_RemoteNodes[iClient].m_RelPackages.push_back( pkNetPacket->m_kData );
	Send(pkNetPacket);
	//m_RemoteNodes[iClient].m_RelPackages.pop_front();
}

void NetWork::SendToAllClients(NetPacket* pkNetPacket)
{
	if(m_RemoteNodes.size() <= 0)
		return;

	for(unsigned int i=0; i<m_RemoteNodes.size(); i++) {
		if(m_RemoteNodes[i].m_eConnectStatus != NETSTATUS_CONNECTED)
			continue;

		pkNetPacket->m_kAddress = m_RemoteNodes[i].m_kAddress;
		pkNetPacket->m_kData.m_kHeader.m_iOrder = m_RemoteNodes[i].m_iNumOfPacketsSent;
		
		//m_RemoteNodes[i].m_RelPackages.push_back( pkNetPacket->m_kData );
		Send(pkNetPacket);
		//m_RemoteNodes[i].m_RelPackages.pop_front();
		}
}*/

// Force Disconnect on all nodes.
void NetWork::DisconnectAll()
{
	if(!m_pkSocket)	return;

	// Create a disconnect packet and sent it to all connects.
	NetPacket kNetPRespons;
	kNetPRespons.Clear();
	kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
	kNetPRespons.Write((unsigned char) ZF_NETCONTROL_DISCONNECT);
	kNetPRespons.TargetSetClient(ZF_NET_ALLCLIENT);
	Send2(&kNetPRespons);
	//SendToAllClients(&kNetPRespons);

	for(unsigned int i=0; i<m_RemoteNodes.size(); i++) {
		m_RemoteNodes[i].m_eConnectStatus = NETSTATUS_DISCONNECT;
	}
}

void NetWork::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) {
		case FID_NETGMAX:
			if(kCommand->m_kSplitCommand.size() <= 1)
				return;
			
			float fMax = atoi(kCommand->m_kSplitCommand[1].c_str());

			for(int i=0;i<m_RemoteNodes.size(); i++)
				m_RemoteNodes[i].m_kRecvGraph.SetMinMax(0, fMax);
			break;
	
		}	

}