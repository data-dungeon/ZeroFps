#ifndef _ENGINE_NETWORK_H_
#define _ENGINE_NETWORK_H_

#include "engine_x.h"
#include "../basic/basic.pkg"
#include "console.h"
#include <SDL/SDL_net.h>

#define MAX_PACKET_SIZE	1024
#define MAX_NETWORKNAME	16

#define ZF_NETTYPE_CONTROL	1
#define ZF_NETTYPE_UNREL	2


#define ZF_NETCONTROL_LIST			1
#define ZF_NETCONTROL_JOIN			2
#define ZF_NETCONTROL_JOINYES		3
#define ZF_NETCONTROL_JOINNO		4
#define ZF_NETCONTROL_DISCONNECT	5

#define MAX_NET_CLIENTS		4

enum ClientConnectStatus
{
	NETSTATUS_CONNECTING,
	NETSTATUS_CONNECTED,
	NETSTATUS_DISCONNECT
};

class RemoteNode
{
public:
	RemoteNode() {};
	~RemoteNode() {};

	IPaddress			m_kAddress;
	ClientConnectStatus	m_eConnectStatus;

	void SetAddress(IPaddress* pkAddress);
};

class ENGINE_API NetPacket
{
public:
	NetPacket();
	~NetPacket();
	void Clear();
	
	void SetTarget(const char* szIp);

	unsigned char m_acData[MAX_PACKET_SIZE];
	int  m_iLength;
	int  m_iPos;
	IPaddress m_kAddress;

	void Write_Str(const char* szString);
	void Read_Str(char* szString);

	template <class Any> 
	void Write(Any type) {
		unsigned char * add = &m_acData[m_iPos];
		memcpy(add, &type, sizeof(type));
		m_iPos += sizeof(type);
		if(m_iPos > m_iLength)	m_iLength = m_iPos;
		}

	template <class Any> 
	void Read(Any& type) {
		unsigned char * add = &m_acData[m_iPos];
		memcpy(&type, add, sizeof(type));
		m_iPos += sizeof(type);
		}

};

enum NetWorkStatus
{
	NET_NONE,
	NET_SERVER,
	NET_CLIENT,
};

class ENGINE_API NetWork : public ZFObject 
{
private:
//	UDPsocket	m_pkServerSocket;
//	UDPsocket	m_pkClientSocket;
	UDPsocket	m_pkSocket;

	char		szServerName[MAX_NETWORKNAME];
	bool		bAcceptClientConnections;
	Console*	m_pkConsole;
	ZeroFps*	m_pkZeroFps;

	vector<RemoteNode>	RemoteNodes;

	IPaddress	m_kServerAddress;

//	bool		m_bIsConnectedToServer;
//	bool		m_bIsServer;
	
	char		m_szAddressBuffer[256];		// Used to convert/print address.

public:
	NetWork();
	~NetWork();

	NetWorkStatus	m_eNetStatus;

	int GetNumOfClients(void);

	bool Init();
	bool Close();

	void Run(); 

	bool Send(NetPacket* pkNetPacket);
	bool Recv(NetPacket* pkNetPacket);

	bool CmpNetworkAddress();
	bool StrToAddress();
	bool AddressToStr(IPaddress* pkAddress, char* szString);

	vector<RemoteNode>	m_akClients;

	void StartSocket(bool bStartServer);
	void CloseSocket();

	void ServerStart(void);
	void ServerEnd(void);

	void ClientStart(const char* szIp);
	void ClientEnd(void);
	void HandleControlMessage(NetPacket* pkNetPacket);

	void SendToAllClients(NetPacket* pkNetPacket);

	void ServerList(void);
};

#endif







