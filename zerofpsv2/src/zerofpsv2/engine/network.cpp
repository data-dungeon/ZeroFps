#include "network.h"
#include "zerofps.h"

#include "../basic/zfobjectmanger.h"

NetWork* g_pkNetWork;


RemoteNode::RemoteNode()
{
	Clear();
}



RemoteNode::~RemoteNode()
{
	
}

void RemoteNode::Clear()
{
	m_eConnectStatus		=	NETSTATUS_DISCONNECT;
	m_kAddress.host		=	INADDR_NONE;
	m_kAddress.port		=  0;

	m_iNumOfPacketsSent	= 0;
	m_iNumOfPacketsRecv  = 0;
	m_iNumOfBytesSent    = 0;
	m_iNumOfBytesRecv    = 0;
	m_iNumOfBytesRecvNetFrame = 0;

	m_fLastMessageTime   = 0;
	m_fPing					= 0;								
	
	m_iOutOfOrderRecv			= 0;
	m_iPacketLossRecv			= 0;
	m_iOutOfOrderNetFrame	= 0;
	m_iLastRecvPacket			= 0;

	m_kRecvGraph.SetMinMax(0,10000);		
	m_kRecvGraph.SetSize(100,100,50);

}

void RemoteNode::SetAddress(IPaddress* pkAddress)
{
	m_kAddress = *pkAddress;

}










NetPacket::NetPacket()
{
	m_iLength = 0;
	m_iPos = 0;
}

NetPacket::~NetPacket()
{

}

void NetPacket::Clear()
{
	m_iLength = 0;
	m_iPos = 0;
	m_bReadError = false;
}

void NetPacket::SetTarget(const char* szIp)
{
	int ha1, ha2, ha3, ha4, hp;
	int ip_addr;

	// Scan addr string.
	sscanf(szIp, "%d.%d.%d.%d:%d", &ha1, &ha2, &ha3, &ha4, &hp);

	// Build host ip.
	ip_addr = (ha1 << 24) | (ha2 << 16) | (ha3 << 8) | ha4;
	SDLNet_Write32(ip_addr, &m_kAddress.host);	
	SDLNet_Write16(hp, &m_kAddress.port);	
}

void NetPacket::Write_Str(const char* szString)
{
	ZFAssert((m_iPos + strlen(szString) + 1) < MAX_PACKET_SIZE, "NetPacket::Write_Str");

	unsigned char * add = &m_kData.m_acData[m_iPos];
	strcpy((char*)add, szString);
	m_iPos += strlen(szString) + 1;
	m_iLength += strlen(szString) + 1;
	
}

void NetPacket::Read_Str(char* szString)
{
	unsigned char * add = &m_kData.m_acData[m_iPos];
	strcpy(szString, (char*)add);
	m_iPos += strlen(szString) + 1;
}

void NetPacket::Write_NetStr (const char* szString)
{
	int iIndex = g_pkNetWork->NetString_GetIndex(szString);
	g_ZFObjSys.Logf("net", "Write_NetStr: %d, %s\n", iIndex, szString);
	
	Write(iIndex);
	if(iIndex == ZF_NET_NONETSTRING) {
		g_pkNetWork->NetString_Add(szString);
		Write_Str(szString);
		}
}

void NetPacket::Read_NetStr  (char* szString)
{	
	string strNetString;

	int iIndex;
	Read(iIndex);

	if(iIndex == ZF_NET_NONETSTRING) 
		Read_Str(szString);
	else {
		strNetString = g_pkNetWork->NetString_GetString(iIndex);
		strcpy(szString, strNetString.c_str());
		}
}


void NetPacket::Write(void* ptr, int iSize)
{
	ZFAssert((m_iPos + iSize) < MAX_PACKET_SIZE, "NetPacket::Write");

	unsigned char * add = &m_kData.m_acData[m_iPos];
	memcpy(add, ptr, iSize);
	m_iPos += iSize;
	if(m_iPos > m_iLength)	m_iLength = m_iPos;
}

void NetPacket::Read(void* ptr, int iSize)
{
	unsigned char * add = &m_kData.m_acData[m_iPos];
	memcpy(ptr, add, iSize);
	m_iPos += iSize;

}

void NetPacket::SetType(int iType)
{
	m_kData.m_kHeader.m_iPacketType = iType;
}













NetWork::NetWork()
: ZFObject("NetWork"){

	// Create Log Files.
	g_ZFObjSys.Log_Create("net");
	g_ZFObjSys.Log_Create("netpac");

	g_ZFObjSys.Log("net", "NetWork SubSystem Startup:\n");
	g_ZFObjSys.Log("netpac", "NetWork Packet Log:\n");

	m_pkConsole	= static_cast<Console*>(g_ZFObjSys.GetObjectPtr("Console"));
	m_pkZeroFps	= static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	strcpy(m_szServerName,"No Name");
	m_bAcceptClientConnections = false;
	m_pkSocket = NULL;
	m_eNetStatus = NET_NONE;
	SetMaxNodes( 4 );				// Vim - Hard coded for now. Must be same as ZeroFps.m_kClient
	m_iMaxNumberOfNodes = 4;	

	m_kStringTable.resize( ZF_NET_MAXSTRINGS );
	for(int i=0; i < ZF_NET_MAXSTRINGS; i++) {
		m_kStringTable[i].m_bInUse			= false;
		m_kStringTable[i].m_bUpdated		= false;
		m_kStringTable[i].m_bNeedUpdate	= false;
		m_kStringTable[i].m_NetString = "";
		}

	g_pkNetWork = this;

	int iInitRes = SDLNet_Init();
	g_ZFObjSys.Logf("net", "SDLNet_Init(): %d\n", iInitRes);
}

NetWork::~NetWork()
{
	for(int i=0; i < ZF_NET_MAXSTRINGS; i++) {
		if(m_kStringTable[i].m_bInUse)
			g_ZFObjSys.Logf("net", "NetString[%d]: = %s\n", i, m_kStringTable[i].m_NetString.c_str());
		}

	DisconnectAll();
	CloseSocket();
	SDLNet_Quit();
	g_ZFObjSys.Log("net", "NetWork SubSystem ShutDown:\n");
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
	g_ZFObjSys.Logf("net", "NetString Add: = %s\n", strTest.c_str());

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
	if(iIndex < 0 || iIndex >= m_kStringTable.size())
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
			g_ZFObjSys.Logf("net", "Write NetString[%d]: = %s\n", i, m_kStringTable[i].m_NetString.c_str());
			}

		// Send if packet to large.
		if(NP.m_iPos >= 512) {
			NP.Write( ZF_NET_NONETSTRING );
			g_ZFObjSys.Logf("net", "Write NetStrings: Order Client 0 : %d", m_RemoteNodes[0].m_iNumOfPacketsSent );
			SendToAllClients(&NP);

			NP.Clear();
			NP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
			NP.Write((char) ZF_NETCONTROL_NETSTRINGS);
			}
		}

	NP.Write( ZF_NET_NONETSTRING );
	g_ZFObjSys.Logf("net", "Write NetStrings: Order Client 0 : %d", m_RemoteNodes[0].m_iNumOfPacketsSent );
	SendToAllClients(&NP);
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
	SendToAllClients(&kNetPRespons);
}

int NetWork::GetNumOfClients(void)
{
	int iNumOfClients = 0;
	for(int i=0; i < m_RemoteNodes.size(); i++) {
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
	for(int i=0; i < m_RemoteNodes.size(); i++) {
		if(IsAddressEquals(pkAddress, &m_RemoteNodes[i].m_kAddress))
			return i;
		}

	return ZF_NET_NOCLIENT;
}

void NetWork::SetMaxNodes(int iMaxNode)
{
	m_RemoteNodes.resize(iMaxNode);
}

int NetWork::GetFreeClientNum()
{
	for(int i=0; m_RemoteNodes.size(); i++) {
		if(m_RemoteNodes[i].m_eConnectStatus == NETSTATUS_DISCONNECT)
			return i;
		}

	return ZF_NET_NOCLIENT;
}


void NetWork::StartSocket(bool bStartServer)
{
	if(m_pkSocket) {
		cout << "StartSocket: Socket is already open" << endl;
		return;
		}

	int iSocketNum = 4242;
	if(bStartServer)	iSocketNum = 4242;
	m_pkSocket = SDLNet_UDP_Open(4242);
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

void NetWork::ClientStart(const char* szIp)
{
	if(m_eNetStatus == NET_SERVER)
		return;

	StartSocket(false);
	m_eNetStatus = NET_CLIENT;

	NetPacket NetP;

	NetP.Clear();
	NetP.SetTarget(szIp);
	NetP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
//	NetP.Write((char) ZF_NETTYPE_CONTROL);
	NetP.Write((char) ZF_NETCONTROL_JOIN);
	Send(&NetP);
}

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

bool NetWork::Send(NetPacket* pkNetPacket)
{
	UDPpacket kPacket;
	kPacket.channel	= -1;
	kPacket.data		= (unsigned char*)&pkNetPacket->m_kData;
	kPacket.len			= pkNetPacket->m_iLength + sizeof(ZFNetHeader);
	kPacket.maxlen		= MAX_PACKET_SIZE;
	kPacket.address	= pkNetPacket->m_kAddress;

	int iRes = SDLNet_UDP_Send(m_pkSocket, -1, &kPacket);

	int iClientID;
	iClientID = GetClientNumber(&pkNetPacket->m_kAddress);
	if(iClientID != ZF_NET_NOCLIENT) {
		m_RemoteNodes[iClientID].m_iNumOfPacketsSent ++;
		m_RemoteNodes[iClientID].m_iNumOfBytesSent += pkNetPacket->m_iLength;
		}

	return true;
}

void NetWork::HandleControlMessage(NetPacket* pkNetPacket)
{
	char szText[256];

	float fEngineTime = m_pkZeroFps->GetEngineTime();

	int iClientID;
	unsigned char ucControlType;
	pkNetPacket->Read(ucControlType);

	NetPacket kNetPRespons;
	RemoteNode kNewNode;

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

			if(GetNumOfClients() == m_iMaxNumberOfNodes) {
				
				m_pkConsole->Printf("Join Ignored: To many connected clients.");
				kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
				//kNetPRespons.Write((unsigned char) ZF_NETTYPE_CONTROL);
				kNetPRespons.Write((unsigned char) ZF_NETCONTROL_JOINNO);
				kNetPRespons.Write_Str("There server is full.");
				kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
				Send(&kNetPRespons);
				}
			else {
				if(! m_pkZeroFps->PreConnect(pkNetPacket->m_kAddress, szText)) {
					m_pkConsole->Printf("Join Ignored: To many connected clients.");
					kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
					//kNetPRespons.Write((unsigned char) ZF_NETTYPE_CONTROL);
					kNetPRespons.Write((unsigned char) ZF_NETCONTROL_JOINNO);
					kNetPRespons.Write_Str("There server is full.");
					kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
					Send(&kNetPRespons);
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

				// Send Connect Yes.
				kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
				//kNetPRespons.Write((unsigned char) ZF_NETTYPE_CONTROL);
				kNetPRespons.Write((unsigned char) ZF_NETCONTROL_JOINYES);
				kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
				Send(&kNetPRespons);
				m_pkZeroFps->Connect(iClientID);
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
			m_pkZeroFps->Connect(iClientID);

			break;

		case ZF_NETCONTROL_JOINNO:
			m_pkConsole->Printf("NetWork::HandleControlMessage(ZF_NETCONTROL_JOINNO)");
			pkNetPacket->Read_Str( szText );
			m_pkConsole->Printf("Connect No: %s", szText );
			break;

		case ZF_NETCONTROL_REQCLIENTID:
			g_ZFObjSys.Log("net", "*** *** ZF_NETCONTROL_REQCLIENTID *** ***\n");
			iClientID = GetClientNumber( &pkNetPacket->m_kAddress );
			if(iClientID != ZF_NET_NOCLIENT) {
				kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
				//kNetPRespons.Write((unsigned char) ZF_NETTYPE_CONTROL);
				kNetPRespons.Write((unsigned char) ZF_NETCONTROL_CLIENTID);
				kNetPRespons.Write((int) m_pkZeroFps->m_kClient[iClientID].m_pkObject->iNetWorkID);
				kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
				Send(&kNetPRespons);
				}
				
			break;

		case ZF_NETCONTROL_CLIENTID:
			g_ZFObjSys.Log("net", "*** *** ZF_NETCONTROL_CLIENTID *** ***\n");
			int iObjId;
			pkNetPacket->Read(iObjId);
			m_pkZeroFps->m_iRTSClientObject = iObjId;
			g_ZFObjSys.Logf("net", "ZF_NETCONTROL_CLIENTID: %d\n",m_pkZeroFps->m_iRTSClientObject );
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
			g_ZFObjSys.Log("net", "*** *** ZF_NETCONTROL_NETSTRINGS *** ***\n");
			int iStringID;
			pkNetPacket->Read(iStringID);
			g_ZFObjSys.Logf("net", " NetString[%d]\n", iStringID);

			while(iStringID != ZF_NET_NONETSTRING) {
				pkNetPacket->Read_Str( szText );

				m_kStringTable[iStringID].m_bInUse		= true;
				m_kStringTable[iStringID].m_bUpdated	= false;
				m_kStringTable[iStringID].m_NetString	= szText;

				g_ZFObjSys.Logf("net", " NetString[%d] = %s\n",iStringID, m_kStringTable[iStringID].m_NetString.c_str());
				cout << "Recv NetString[" << iStringID <<    "] = " << szText << endl;

				pkNetPacket->Read(iStringID);
				}

				break;
			}


		case ZF_NETCONTROL_REQNETSTRING:
			{
			g_ZFObjSys.Log("net", "*** *** ZF_NETCONTROL_REQNETSTRING *** ***\n");

			// When we get a req for a netstring we make a packet with it and send it to all connected clients.
			int iStringID;
			pkNetPacket->Read(iStringID);
			g_ZFObjSys.Logf("net", " Request for NetString[%d]\n", iStringID);

			NetPacket kNetPRespons;
			kNetPRespons.Clear();
			kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
			kNetPRespons.Write((char) ZF_NETCONTROL_NETSTRINGS);

			kNetPRespons.Write( iStringID );
			kNetPRespons.Write_Str(m_kStringTable[ iStringID ].m_NetString.c_str());
			
			kNetPRespons.Write( ZF_NET_NONETSTRING );
			SendToAllClients(&kNetPRespons);

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

	for(int i=0; i < m_RemoteNodes.size(); i++) {
		switch(m_RemoteNodes[i].m_eConnectStatus) {
			case NETSTATUS_CONNECTING:	pkName = "CONNECTING";	break;
			case NETSTATUS_CONNECTED:	pkName = "CONNECTED";	break;
			case NETSTATUS_DISCONNECT:	pkName = "DISCONNECT";	break;
			}

		AddressToStr(&m_RemoteNodes[i].m_kAddress,szAdress);

		m_pkZeroFps->DevPrintf("conn", " Node[%d] %s %s %d/%d %d/%d - %f", i, pkName, szAdress,
			m_RemoteNodes[i].m_iNumOfPacketsSent, m_RemoteNodes[i].m_iNumOfBytesSent,
			m_RemoteNodes[i].m_iNumOfPacketsRecv, m_RemoteNodes[i].m_iNumOfBytesRecv,
			( m_RemoteNodes[i].m_fLastMessageTime + ZF_NET_CONNECTION_TIMEOUT ) - fEngineTime);

//		RemoteNodes[i].m_kRecvGraph.DrawGraph(0, 50 * i + 100);
	}
}
	
void NetWork::DrawConnectionGraphs()
{
	for(int i=0; i < m_RemoteNodes.size(); i++) {
		m_RemoteNodes[i].m_kRecvGraph.DrawGraph(0, 55 * i + 100);
	}
}

void NetWork::Run()
{	
	int iRecvBytes = 0;

	float fEngineTime = m_pkZeroFps->GetEngineTime();

	if( m_eNetStatus == NET_NONE )	return;

	g_ZFObjSys.Logf("netpac", "Engine Time: %f\n", fEngineTime );

	int i;
	for(i=0; i<m_RemoteNodes.size(); i++) {
		m_RemoteNodes[i].m_iNumOfBytesRecvNetFrame = 0;
		}
	
	NetPacket NetP;
	int iClientID;

	// Recv all packets.
	while(Recv(&NetP)) {
		// Update Stats
		iClientID = GetClientNumber(&NetP.m_kAddress);
		iRecvBytes += NetP.m_iLength;

		if(iClientID != ZF_NET_NOCLIENT) {
			m_RemoteNodes[iClientID].m_iNumOfPacketsRecv ++;
			m_RemoteNodes[iClientID].m_iNumOfBytesRecvNetFrame += NetP.m_iLength;
			m_RemoteNodes[iClientID].m_fLastMessageTime = fEngineTime;

			if(NetP.m_kData.m_kHeader.m_iOrder != (m_RemoteNodes[iClientID].m_iLastRecvPacket + 1))
				m_RemoteNodes[iClientID].m_iOutOfOrderNetFrame ++;
			m_RemoteNodes[iClientID].m_iLastRecvPacket = NetP.m_kData.m_kHeader.m_iOrder;
			}
		g_ZFObjSys.Logf("netpac", " From: %d Order: %d: Type: %d Size: %d\n",
			iClientID, 
			NetP.m_kData.m_kHeader.m_iOrder,
			NetP.m_kData.m_kHeader.m_iPacketType,
			NetP.m_iLength);
		
		switch(NetP.m_kData.m_kHeader.m_iPacketType) {
			// If controll handle_controllpacket.
			case ZF_NETTYPE_CONTROL:
				HandleControlMessage(&NetP);
				break;
	
			// Else give it to zerofps.
			case ZF_NETTYPE_UNREL:
				g_ZFObjSys.Logf("net", "Recv: ZF_NETTYPE_UNREL: Order = %d\n", NetP.m_kData.m_kHeader.m_iOrder);
				m_pkZeroFps->HandleNetworkPacket(&NetP);
				break;

			default:
				cout << "Recv: Something :)" << endl;
			}
		}

	// Check for any lost connections
	for(i=0; i<m_RemoteNodes.size(); i++) {
		if(m_RemoteNodes[i].m_eConnectStatus == NETSTATUS_DISCONNECT)
			continue;

		// Refresh num of recd bytes graphs.
		m_RemoteNodes[i].m_iNumOfBytesRecv += m_RemoteNodes[i].m_iNumOfBytesRecvNetFrame;
		if(fEngineTime > m_fStatsUpdate) {
			m_RemoteNodes[i].m_kRecvGraph.PushValue(m_RemoteNodes[i].m_iNumOfBytesRecvNetFrame);
			m_RemoteNodes[i].m_iNumOfBytesRecvNetFrame = 0;
			}

		if(fEngineTime > ( m_RemoteNodes[i].m_fLastMessageTime + ZF_NET_CONNECTION_TIMEOUT )) {
			// Time out this connection.
			m_pkZeroFps->Disconnect(i);
			m_pkConsole->Printf("Connection to %d timed out.", i);
			m_RemoteNodes[i].m_eConnectStatus = NETSTATUS_DISCONNECT;
			}
		}

	if(fEngineTime > m_fStatsUpdate)
		m_fStatsUpdate = fEngineTime + 1.0;	

	Send_NetStrings();
	NetString_Refresh();


	g_ZFObjSys.Logf("netpac", " Num of bytes total: %d\n", iRecvBytes);
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

void NetWork::SendToAllClients(NetPacket* pkNetPacket)
{
	if(m_RemoteNodes.size() <= 0)
		return;

	for(unsigned int i=0; i<m_RemoteNodes.size(); i++) {
		if(m_RemoteNodes[i].m_eConnectStatus != NETSTATUS_CONNECTED)
			continue;

		pkNetPacket->m_kAddress = m_RemoteNodes[i].m_kAddress;
		pkNetPacket->m_kData.m_kHeader.m_iOrder = m_RemoteNodes[i].m_iNumOfPacketsSent;
		g_ZFObjSys.Logf("net", "SendToAllClients[%d] : Order = %d", i, m_RemoteNodes[i].m_iNumOfPacketsSent );
		
		Send(pkNetPacket);
		}
}

void NetWork::RTS_RequestClientObjectID()
{
	g_ZFObjSys.Log("net", "RTS_RequestClientObjectID()\n");

	NetPacket NPacket;
	NPacket.Clear();
	NPacket.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
	NPacket.Write((unsigned char) ZF_NETCONTROL_REQCLIENTID);

	SendToAllClients(&NPacket);
}

// Force Disconnect on all nodes.
void NetWork::DisconnectAll()
{
	if(!m_pkSocket)	return;

	// Create a disconnect packet and sent it to all connects.
	NetPacket kNetPRespons;
	kNetPRespons.Clear();
	kNetPRespons.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
	kNetPRespons.Write((unsigned char) ZF_NETCONTROL_DISCONNECT);
	SendToAllClients(&kNetPRespons);

	for(unsigned int i=0; i<m_RemoteNodes.size(); i++) {
		m_RemoteNodes[i].m_eConnectStatus = NETSTATUS_DISCONNECT;
	}
}

void NetWork::TEST_KeepAliveALL()
{
	if(m_RemoteNodes.size() <= 0)
		return;

	NetPacket NetPacket;
	NetPacket.Clear();
	NetPacket.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_CONTROL;
	NetPacket.Write((unsigned char) ZF_NETCONTROL_NOP);
	
	for(unsigned int i=0; i<m_RemoteNodes.size(); i++) {
		if(m_RemoteNodes[i].m_eConnectStatus != NETSTATUS_CONNECTED)
			continue;

		NetPacket.m_kAddress = m_RemoteNodes[i].m_kAddress;
		Send(&NetPacket);
		}
}

/*
void NetWork::ServerList(void)
{
	int iTest = 5433;
	char cTest = 12;
	unsigned char ucTest = 168;
	string strTest("Gaaaa liksom :)");
	Vector3 vTest(1,1.5,3);

	NetPacket NetP;
	NetP.Write(iTest);
	NetP.Write_Str(strTest.c_str());
	NetP.Write(ucTest);
	NetP.Write(cTest);
	NetP.Write(vTest);
 	
	int iTest2;
	char cTest2;
	unsigned char ucTest2;
	char szTest2[256];
	Vector3 vTest2;

	NetP.m_iPos = 0;
	NetP.Read(iTest2);
	NetP.Read_Str(szTest2);
	NetP.Read(ucTest2);
	NetP.Read(cTest2);
	NetP.Read(vTest2);

	cout << "iTest2 : " << iTest2 << endl;
	cout << "szTest2 : " << szTest2 << endl;
	cout << "ucTest2 : " << (int) ucTest2 << endl;
	cout << "cTest2 : " << (int) cTest2 << endl;
	cout << "vTest2 : ";
	vTest2.Print();
}*/



