#ifndef _ENGINE_NETWORK_H_
#define _ENGINE_NETWORK_H_

#include "engine_x.h"
#include "../basic/basic.pkg"
#include "console.h"
#include <SDL/SDL_net.h>

#define MAX_PACKET_SIZE	1024
#define MAX_NETWORKNAME	16

#define ZF_NETTYPE_CONTROL			1
#define ZF_NETTYPE_UNREL			2


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

	IPaddress					m_kAddress;				// Address of node.
	ClientConnectStatus		m_eConnectStatus;		// Status of this nodes connection.

	void SetAddress(IPaddress* pkAddress);			// Sets address of node.

	// Stats
	int				m_iNumOfPacketsSent;
	int				m_iNumOfPacketsRecv;
	unsigned int	m_iNumOfBytesSent;
	unsigned int	m_iNumOfBytesRecv;
	
	float				m_fLastMessageTime;				// Time (Engine) of last message. Use to find time outs.
	float				m_fPing;								// Ping 

	int				m_iOutOfOrderRecv;
	int				m_iPacketLossRecv;
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
		ZFAssert((m_iPos + sizeof(type)) < MAX_PACKET_SIZE, "NetPacket::Write");

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

	void Write(void* ptr, int iSize);
	void Read(void* ptr, int iSize);
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
//	bool		m_bIsConnectedToServer;
//	bool		m_bIsServer;

	UDPsocket				m_pkSocket;								// Socket we use for all our messages.

	char						szServerName[MAX_NETWORKNAME];	// Well ... VIM... ***
	bool						bAcceptClientConnections;			// If false all connect attemts are ignored.
	Console*					m_pkConsole;							// Ptr to console.
	ZeroFps*					m_pkZeroFps;							// Ptr to zerofps engine.

	IPaddress				m_kLocalIP;								// Our Own Local IP.	***
	int						m_iMaxNumberOfNodes;					// Max Number of remote Nodes we can keep track of.	***

	vector<RemoteNode>	RemoteNodes;							// Data About all our remote connections.
	IPaddress				m_kServerAddress;						// Ip of the server we are conencted to.
	char						m_szAddressBuffer[256];				// Used to convert/print address.

	void	ClearRemoteNode(RemoteNode* pkNode);				// Reset a node ***

	bool Recv(NetPacket* pkNetPacket);							// Recv a packet if any is waiting.

public:
	NetWorkStatus			m_eNetStatus;
	vector<RemoteNode>	m_akClients;

	bool Init();
	bool Close();

	NetWork();
	~NetWork();

	int GetNumOfClients(void);
	int GetClientNumber(IPaddress* pkAddress);				// Get ID of client, CLIENT_UNCONNECTED if none.	***

	void StartSocket(bool bStartServer);
	void CloseSocket();
	void ServerStart(void);
	void ServerEnd(void);
	void ClientStart(const char* szIp);
	void ClientEnd(void);

	void HandleControlMessage(NetPacket* pkNetPacket);		// Handle controll messages used by network layer. ***
	void Run(); 

	bool Send(NetPacket* pkNetPacket);
	void SendToAllClients(NetPacket* pkNetPacket);

	bool CmpNetworkAddress();
	bool StrToAddress();
	bool AddressToStr(IPaddress* pkAddress, char* szString);

	void ServerList(void);
};

#endif







