#include "network.h"

#include "zerofps.h"


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

	int iInitRes = SDLNet_Init();
	cout << "SDLNet_Init(): " << iInitRes<< endl;
	strcpy(szServerName,"No Name");
	bAcceptClientConnections = false;

	m_pkSocket = NULL;

	m_pkConsole = static_cast<Console*>(g_ZFObjSys.GetObjectPtr("Console"));
	m_pkZeroFps  = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));;

	m_eNetStatus = NET_NONE;

	g_ZFObjSys.Log_Create("net");
	g_ZFObjSys.Log("net", "NetWork SubSystem Startup:\n");

}

NetWork::~NetWork()
{
	CloseSocket();
	SDLNet_Quit();
	cout << "SDLNet_Quit()" << endl;
	g_ZFObjSys.Log("net", "NetWork SubSystem ShutDown:\n");
}

int NetWork::GetNumOfClients(void)
{
	return RemoteNodes.size();
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
	CloseSocket();
	m_eNetStatus = NET_NONE;
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

	return true;
}

void NetWork::HandleControlMessage(NetPacket* pkNetPacket)
{
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
			if(RemoteNodes.size() >= MAX_NET_CLIENTS)
				m_pkConsole->Printf("Join Ignored: To many connected clients.");
			else {
				// Create New Connection client.
				kNewNode.m_kAddress = pkNetPacket->m_kAddress;
				kNewNode.m_eConnectStatus = NETSTATUS_CONNECTED;
				RemoteNodes.push_back(kNewNode);
				m_pkConsole->Printf("Client Connected: %s", m_szAddressBuffer);

				// Send Connect Yes.
				kNetPRespons.Clear();
				//kNetPRespons.SetTarget("192.168.0.111:4242");
				kNetPRespons.Write((unsigned char) ZF_NETTYPE_CONTROL);
				kNetPRespons.Write((unsigned char) ZF_NETCONTROL_JOINYES);
				kNetPRespons.m_kAddress = pkNetPacket->m_kAddress;
				Send(&kNetPRespons);
				}
			
			break;

		case ZF_NETCONTROL_JOINYES:
			// Client can join.
			m_kServerAddress = pkNetPacket->m_kAddress;
			m_pkConsole->Printf("NetWork::HandleControlMessage(ZF_NETCONTROL_JOINYES)");
			AddressToStr(&pkNetPacket->m_kAddress, m_szAddressBuffer);
			m_pkConsole->Printf("Server Ip: %s", m_szAddressBuffer);
			break;

		case ZF_NETCONTROL_JOINNO:
			m_pkConsole->Printf("NetWork::HandleControlMessage(ZF_NETCONTROL_JOINNO)");
			// Can not join server. String why.
			break;

		case ZF_NETCONTROL_DISCONNECT:
			m_pkConsole->Printf("NetWork::HandleControlMessage(ZF_NETCONTROL_DISCONNECT)");
			// Outer side disconnect.
			// Server removes client, client stops sim.
			break;
	}

}

void NetWork::Run()
{
	if(	m_eNetStatus == NET_NONE)	return;
	NetPacket NetP;

	unsigned char ucPacketType;
//	unsigned char ucPacketLength;

	while(Recv(&NetP)) {
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

	cout << "Update Clients: ";
	for(unsigned int i=0; i<RemoteNodes.size(); i++) {
		pkNetPacket->m_kAddress = RemoteNodes[i].m_kAddress;
		Send(pkNetPacket);
		cout << ".";		
		}
	cout << endl;
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



