#include "network.h"
#include "zerofps.h"



RemoteNode::RemoteNode()
{
	Clear();
}



RemoteNode::~RemoteNode()
{
	
}

void RemoteNode::Clear()
{
	m_eConnectStatus	=	NETSTATUS_DISCONNECT;
	m_kAddress.host	=	INADDR_NONE;
	m_kAddress.port   =  0;

	m_iNumOfPacketsSent	= 0;
	m_iNumOfPacketsRecv  = 0;
	m_iNumOfBytesSent    = 0;
	m_iNumOfBytesRecv    = 0;
	
	m_fLastMessageTime   = 0;
	m_fPing					= 0;								
	
	m_iOutOfOrderRecv		= 0;
	m_iPacketLossRecv		= 0;
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

	unsigned char * add = &m_acData[m_iPos];
	strcpy((char*)add, szString);
	m_iPos += strlen(szString) + 1;
	m_iLength += strlen(szString) + 1;
	
}

void NetPacket::Read_Str(char* szString)
{
	unsigned char * add = &m_acData[m_iPos];
	strcpy(szString, (char*)add);
	m_iPos += strlen(szString) + 1;

}

void NetPacket::Write(void* ptr, int iSize)
{
	ZFAssert((m_iPos + iSize) < MAX_PACKET_SIZE, "NetPacket::Write");

	unsigned char * add = &m_acData[m_iPos];
	memcpy(add, ptr, iSize);
	m_iPos += iSize;
	if(m_iPos > m_iLength)	m_iLength = m_iPos;
}

void NetPacket::Read(void* ptr, int iSize)
{
	unsigned char * add = &m_acData[m_iPos];
	memcpy(ptr, add, iSize);
	m_iPos += iSize;

}













NetWork::NetWork()
: ZFObject("NetWork"){
	g_ZFObjSys.Log_Create("net");
	g_ZFObjSys.Log("net", "NetWork SubSystem Startup:\n");

	int iInitRes = SDLNet_Init();
	cout << "SDLNet_Init(): " << iInitRes<< endl;
	strcpy(szServerName,"No Name");
	bAcceptClientConnections = false;

	m_pkSocket = NULL;

	m_pkConsole = static_cast<Console*>(g_ZFObjSys.GetObjectPtr("Console"));
	m_pkZeroFps  = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));;

	m_eNetStatus = NET_NONE;

	SetMaxNodes( 4 );				// Vim - Hard coded for now. Must be same as ZeroFps.m_kClient
	m_iMaxNumberOfNodes = 4;	
}

NetWork::~NetWork()
{
	g_ZFObjSys.Log("net", "NetWork SubSystem ShutDown:\n");
	DisconnectAll();
	CloseSocket();
	SDLNet_Quit();
}


int NetWork::GetNumOfClients(void)
{
	int iNumOfClients = 0;
	for(int i=0; i < RemoteNodes.size(); i++) {
		if(RemoteNodes[i].m_eConnectStatus != NETSTATUS_DISCONNECT)
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
	for(int i=0; i < RemoteNodes.size(); i++) {
		if(IsAddressEquals(pkAddress, &RemoteNodes[i].m_kAddress))
			return i;
		}

	return ZF_NET_NOCLIENT;
}

void NetWork::SetMaxNodes(int iMaxNode)
{
	RemoteNodes.resize(iMaxNode);
}

int NetWork::GetFreeClientNum()
{
	for(int i=0; RemoteNodes.size(); i++) {
		if(RemoteNodes[i].m_eConnectStatus == NETSTATUS_DISCONNECT)
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
	NetP.Write((char) ZF_NETTYPE_CONTROL);
	NetP.Write((char) ZF_NETCONTROL_JOIN);
	Send(&NetP);
}














bool NetWork::Recv(NetPacket* pkNetPacket)
{
	pkNetPacket->Clear();

	UDPpacket kPacket;
	kPacket.channel = -1;
	kPacket.data = &pkNetPacket->m_acData[0];
	kPacket.len = 0;
	kPacket.maxlen = MAX_PACKET_SIZE;

	if(SDLNet_UDP_Recv(m_pkSocket, &kPacket)) {
		pkNetPacket->m_kAddress = kPacket.address;
		pkNetPacket->m_iLength = kPacket.len;
		return true;
		}

	return false;
}

bool NetWork::Send(NetPacket* pkNetPacket)
{
	UDPpacket kPacket;
	kPacket.channel = -1;
	kPacket.data = &pkNetPacket->m_acData[0];
	kPacket.len = pkNetPacket->m_iLength;
	kPacket.maxlen = MAX_PACKET_SIZE;
	kPacket.address = pkNetPacket->m_kAddress;

	int iRes = SDLNet_UDP_Send(m_pkSocket, -1, &kPacket);

	int iClientID;
	iClientID = GetClientNumber(&pkNetPacket->m_kAddress);
	if(iClientID != ZF_NET_NOCLIENT) {
		RemoteNodes[iClientID].m_iNumOfPacketsSent ++;
		RemoteNodes[iClientID].m_iNumOfBytesSent += pkNetPacket->m_iLength;
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
				kNetPRespons.Write((unsigned char) ZF_NETTYPE_CONTROL);
				kNetPRespons.Write((unsigned char) ZF_NETCONTROL_JOINNO);
				kNetPRespons.Write_Str("There server is full.");
				kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
				Send(&kNetPRespons);
				}
			else {
				if(! m_pkZeroFps->PreConnect(pkNetPacket->m_kAddress, szText)) {
					m_pkConsole->Printf("Join Ignored: To many connected clients.");
					kNetPRespons.Write((unsigned char) ZF_NETTYPE_CONTROL);
					kNetPRespons.Write((unsigned char) ZF_NETCONTROL_JOINNO);
					kNetPRespons.Write_Str("There server is full.");
					kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
					Send(&kNetPRespons);
					break;
					}

				iClientID = GetFreeClientNum();
				assert(iClientID != ZF_NET_NOCLIENT);

				// Create New Connection client.
				RemoteNodes[iClientID].SetAddress(&pkNetPacket->m_kAddress);
				RemoteNodes[iClientID].m_eConnectStatus = NETSTATUS_CONNECTED;
				RemoteNodes[iClientID].m_fLastMessageTime = fEngineTime;

				//kNewNode.m_kAddress = pkNetPacket->m_kAddress;
				//kNewNode.m_eConnectStatus = NETSTATUS_CONNECTED;
				//RemoteNodes.push_back(kNewNode);
				m_pkConsole->Printf("Client Connected: %s", m_szAddressBuffer);

				// Send Connect Yes.
				kNetPRespons.Write((unsigned char) ZF_NETTYPE_CONTROL);
				kNetPRespons.Write((unsigned char) ZF_NETCONTROL_JOINYES);
				kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
				Send(&kNetPRespons);
				m_pkZeroFps->Connect(iClientID);
				}
			
			break;

		case ZF_NETCONTROL_JOINYES:
			// Client can join.
			iClientID = GetFreeClientNum();
			assert(iClientID != ZF_NET_NOCLIENT);

			// Create New Connection client.
			RemoteNodes[iClientID].SetAddress(&pkNetPacket->m_kAddress);
			RemoteNodes[iClientID].m_eConnectStatus = NETSTATUS_CONNECTED;
			RemoteNodes[iClientID].m_fLastMessageTime = fEngineTime;
			
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
			iClientID = GetClientNumber( &pkNetPacket->m_kAddress );
			if(iClientID != ZF_NET_NOCLIENT) {
				kNetPRespons.Write((unsigned char) ZF_NETTYPE_CONTROL);
				kNetPRespons.Write((unsigned char) ZF_NETCONTROL_CLIENTID);
				kNetPRespons.Write((int) m_pkZeroFps->m_kClient[iClientID].m_pkObject->iNetWorkID);
				kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
				Send(&kNetPRespons);
				}
				
			break;

		case ZF_NETCONTROL_CLIENTID:
			int iObjId;
			pkNetPacket->Read(iObjId);
			m_pkZeroFps->m_iRTSClientObject = iObjId;
			break;

		case ZF_NETCONTROL_DISCONNECT:
			iClientID = GetClientNumber( &pkNetPacket->m_kAddress );
			if(iClientID != ZF_NET_NOCLIENT) {
				m_pkConsole->Printf("NetWork::HandleControlMessage(ZF_NETCONTROL_DISCONNECT)");
				m_pkZeroFps->Disconnect(iClientID);
				RemoteNodes[iClientID].m_eConnectStatus = NETSTATUS_DISCONNECT;
				}

			// Outer side disconnect.
			// Server removes client, client stops sim.
			break;

		case ZF_NETCONTROL_NOP:
			// 
			break;
	}

}

void NetWork::DevShow_ClientConnections()
{
	float fEngineTime = m_pkZeroFps->GetEngineTime();
	m_pkZeroFps->DevPrint_Show("conn");

	char* pkName = "Die Vim";
	char szAdress[256];

	for(int i=0; i < RemoteNodes.size(); i++) {
		switch(RemoteNodes[i].m_eConnectStatus) {
			case NETSTATUS_CONNECTING:	pkName = "CONNECTING";	break;
			case NETSTATUS_CONNECTED:	pkName = "CONNECTED";	break;
			case NETSTATUS_DISCONNECT:	pkName = "DISCONNECT";	break;
			}

		AddressToStr(&RemoteNodes[i].m_kAddress,szAdress);

		m_pkZeroFps->DevPrintf("conn", " Node[%d] %s %s %d/%d %d/%d - %f", i, pkName, szAdress,
			RemoteNodes[i].m_iNumOfPacketsSent, RemoteNodes[i].m_iNumOfBytesSent,
			RemoteNodes[i].m_iNumOfPacketsRecv, RemoteNodes[i].m_iNumOfBytesRecv,
			( RemoteNodes[i].m_fLastMessageTime + ZF_NET_CONNECTION_TIMEOUT ) - fEngineTime);
//			RemoteNodes[i].m_fLastMessageTime);
		}
}
	


void NetWork::Run()
{
	float fEngineTime = m_pkZeroFps->GetEngineTime();
	m_pkZeroFps->DevPrintf("conn", "Engine Time: %f", fEngineTime);
	DevShow_ClientConnections();

	if( m_eNetStatus == NET_NONE )	return;

//	TEST_KeepAliveALL();
	
	NetPacket NetP;
	unsigned char ucPacketType;
	int iClientID;

	// Recv all packets.
	while(Recv(&NetP)) {
		// Update Stats
		iClientID = GetClientNumber(&NetP.m_kAddress);

		if(iClientID != ZF_NET_NOCLIENT) {
			RemoteNodes[iClientID].m_iNumOfPacketsRecv ++;
			RemoteNodes[iClientID].m_iNumOfBytesRecv += NetP.m_iLength;
			RemoteNodes[iClientID].m_fLastMessageTime = fEngineTime;
			}
		
		// Read packet Type & Size
		NetP.Read(ucPacketType);

		switch(ucPacketType) {
			// If controll handle_controllpacket.
			case ZF_NETTYPE_CONTROL:
				HandleControlMessage(&NetP);
				break;
	
			// Else give it to zerofps.
			case ZF_NETTYPE_UNREL:
				g_ZFObjSys.Logf("net", "Recv: ZF_NETTYPE_UNREL\n");

				m_pkZeroFps->HandleNetworkPacket(&NetP);
				break;

			default:
				cout << "Recv: Something :)" << endl;
			}
		}

	// Check for any lost connections
	for(int i=0; i<RemoteNodes.size(); i++) {
		if(RemoteNodes[i].m_eConnectStatus == NETSTATUS_DISCONNECT)
			continue;

		if(fEngineTime > ( RemoteNodes[i].m_fLastMessageTime + ZF_NET_CONNECTION_TIMEOUT )) {
			// Time out this connection.
			m_pkZeroFps->Disconnect(i);
			m_pkConsole->Printf("Connection to %d timed out.", i);
			RemoteNodes[i].m_eConnectStatus = NETSTATUS_DISCONNECT;
			}
		}
}

bool NetWork::AddressToStr(IPaddress* pkAddress, char* szString)
{
	int iPort = 0;
	iPort = iPort | ((pkAddress->port >> 8) & 0xff);  
	iPort = iPort | ((pkAddress->port << 8) & 0xff00);  

	sprintf(szString, "%d.%d.%d.%d:%d", (pkAddress->host) & 0xff, (pkAddress->host >> 8) & 0xff,
				(pkAddress->host >> 16) & 0xff, (pkAddress->host >> 24) & 0xff, iPort);
//	sprintf(szString, "%d.%d.%d.%d:%d", (pkAddress->host >> 24) & 0xff, (pkAddress->host >> 16) & 0xff,
//				(pkAddress->host >> 8) & 0xff, (pkAddress->host) & 0xff, iPort);
	return true;
}

void NetWork::SendToAllClients(NetPacket* pkNetPacket)
{
	if(RemoteNodes.size() <= 0)
		return;

	for(unsigned int i=0; i<RemoteNodes.size(); i++) {
		if(RemoteNodes[i].m_eConnectStatus != NETSTATUS_CONNECTED)
			continue;

		pkNetPacket->m_kAddress = RemoteNodes[i].m_kAddress;
		Send(pkNetPacket);
		}
}

void NetWork::TEST_KeepAliveALL()
{
	if(RemoteNodes.size() <= 0)
		return;

	NetPacket NetPacket;
	NetPacket.Clear();
	NetPacket.Write((unsigned char) ZF_NETTYPE_CONTROL);
	NetPacket.Write((unsigned char) ZF_NETCONTROL_NOP);
	
	for(unsigned int i=0; i<RemoteNodes.size(); i++) {
		if(RemoteNodes[i].m_eConnectStatus != NETSTATUS_CONNECTED)
			continue;

		NetPacket.m_kAddress = RemoteNodes[i].m_kAddress;
		Send(&NetPacket);
		}
}

void NetWork::RTS_RequestClientObjectID()
{
	NetPacket NPacket;
	NPacket.Clear();
	NPacket.Write((unsigned char) ZF_NETTYPE_CONTROL);
	NPacket.Write((unsigned char) ZF_NETCONTROL_REQCLIENTID);

	SendToAllClients(&NPacket);
}


void NetWork::DisconnectAll()
{
	if(!m_pkSocket)	return;

	NetPacket kNetPRespons;
	kNetPRespons.Clear();
	kNetPRespons.Write((unsigned char) ZF_NETTYPE_CONTROL);
	kNetPRespons.Write((unsigned char) ZF_NETCONTROL_DISCONNECT);
	SendToAllClients(&kNetPRespons);

	for(unsigned int i=0; i<RemoteNodes.size(); i++) {
		RemoteNodes[i].m_eConnectStatus = NETSTATUS_DISCONNECT;	//NETSTATUS_CONNECTED;
	}
}


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


	// Create BroadCast LIST packet.
/*	NetPacket NetP;
	NetP.Write((char) ZF_NETTYPE_CONTROL);
	NetP.Write((char) ZF_NETCONTROL_LIST);
	
	NetP.SetTarget("255.255.255.255:4242");
	Send(&NetP);
	m_pkConsole->Printf("Hello... Hello... Anyone...?");*/
}



