#ifndef _ENGINE_NETWORK_H_
#define _ENGINE_NETWORK_H_

#include "engine_x.h"
#include "../basic/basic.pkg"
#include "console.h"
#include <SDL/SDL_net.h>
#include "fh.h"

#define MAX_PACKET_SIZE	1024				// Max Bytes in each packet.
#define MAX_NETWORKNAME	16					

// Diffrent types of messages the network sends.
#define ZF_NETTYPE_CONTROL				1	// A Connection controll message
#define ZF_NETTYPE_UNREL				2	// Unrel message that ZeroFps handles.

#define ZF_NETCONTROL_LIST				1	// Req server info. 
#define ZF_NETCONTROL_JOIN				2	//	Req Join.
#define ZF_NETCONTROL_JOINYES			3	// Join OK.
#define ZF_NETCONTROL_JOINNO			4	//	No Join - : string 
#define ZF_NETCONTROL_DISCONNECT		5	//	Disconnect message. Close connection.
#define ZF_NETCONTROL_NOP				6	// Ohh, nothing but i'm still here.
#define ZF_NETCONTROL_REQCLIENTID	7	//	
#define ZF_NETCONTROL_CLIENTID		8	//
#define ZF_NETCONTROL_NETSTRINGS		9	//
#define ZF_NETCONTROL_REQNETSTRING	10	//

#define MAX_NET_CLIENTS					4	// Max number of clients (nodes).
#define ZF_NET_NOCLIENT					-1	// ID for a non client.

#define ZF_NET_CONNECTION_TIMEOUT	15	// Timeout connection if no message from a client after this time (sec).

// Status of a remote node.
enum ClientConnectStatus
{
	NETSTATUS_CONNECTING,				// Trying to connect.
	NETSTATUS_CONNECTED,					// A Connected client
	NETSTATUS_DISCONNECT					// Unconnected (free).
};

/* Information about a remote node and the connection to it. */
class RemoteNode
{
public:
	RemoteNode();
	~RemoteNode();

	IPaddress					m_kAddress;				// Address of node.
	ClientConnectStatus		m_eConnectStatus;		// Status of this nodes connection.

	void SetAddress(IPaddress* pkAddress);			// Sets address of node.
	void Clear();

	// Stats
	int				m_iNumOfPacketsSent;
	int				m_iNumOfPacketsRecv;
	unsigned int	m_iNumOfBytesSent;
	unsigned int	m_iNumOfBytesRecv;
	
	int				m_iNumOfBytesRecvNetFrame;

	float				m_fLastMessageTime;				// Time (Engine) of last message. Use to find time outs.
	float				m_fPing;								// Ping 

	int				m_iOutOfOrderRecv;
	int				m_iPacketLossRecv;

	DebugGraph		m_kRecvGraph;

	int				m_iOutOfOrderNetFrame;

	int				m_iLastRecvPacket;				// Order num of last recv packet.
};

//	The ZeroFps packet header. Is added to the start of all network packets sent in zerofps.
#pragma pack( 1 )

struct ZFNetHeader
{
	int				m_iOrder;							//  Order of packet 
	unsigned char	m_iPacketType;						//  Type of packet.
};

struct ZFNetPacketData
{
	ZFNetHeader		m_kHeader;							
	unsigned char	m_acData[MAX_PACKET_SIZE];		
};

#pragma pack(  )

// A Packet that is sent over network.
class ENGINE_API NetPacket
{
public:
	NetPacket();
	~NetPacket();
	void Clear();
	
	void SetTarget(const char* szIp);

	ZFNetPacketData	m_kData;
	int					m_iLength;
	int					m_iPos;
	IPaddress			m_kAddress;
	bool					m_bReadError;

	void SetError(bool bError) { m_bReadError = bError;	}
	bool IsReadError() { return m_bReadError; }

	void Write_Str(const char* szString);
	void Read_Str(char* szString);

	void Write_NetStr (const char* szString);
	void Read_NetStr  (char* szString);

	template <class Any> 
	void Write(Any type) {
		ZFAssert((m_iPos + sizeof(type)) < MAX_PACKET_SIZE, "NetPacket::Write");

		unsigned char * add = &m_kData.m_acData[m_iPos];
		memcpy(add, &type, sizeof(type));
		m_iPos += sizeof(type);
		if(m_iPos > m_iLength)	m_iLength = m_iPos;
		}

	template <class Any> 
	void Read(Any& type) {
		unsigned char * add = &m_kData.m_acData[m_iPos];
		memcpy(&type, add, sizeof(type));
		m_iPos += sizeof(type);
		}

	void Write(void* ptr, int iSize);
	void Read(void* ptr, int iSize);

	void SetType(int iType);
};

// Status of network layer.
enum NetWorkStatus
{
	NET_NONE,		// Network is not active
	NET_SERVER,		// The network is in server mode and accepts connections.
	NET_CLIENT,		// The network is in client mode.
};

#define ZF_NET_MAXSTRINGS	1024
#define ZF_NET_NONETSTRING	-1

/*
	Strings can be sent as integer over the network. Each client have a lookup table with all
	strings and if a string is missing it ask the server for it. NS can only be updated on server
	so client must always send the full strings,
*/
struct ZFNet_String
{
	bool		m_bInUse;		// True if this net string is in use.
	bool		m_bUpdated;		// True if is updated.
	string	m_NetString;	// Da string
	bool		m_bNeedUpdate;	// True if we need to request a update from server.
};

class ENGINE_API NetWork : public ZFObject 
{
private:
	UDPsocket				m_pkSocket;								// Socket we use for all our messages.

	char						m_szServerName[MAX_NETWORKNAME];	// Well ... VIM... ***
	bool						m_bAcceptClientConnections;			// If false all connect attemts are ignored.
	Console*					m_pkConsole;							// Ptr to console.
	ZeroFps*					m_pkZeroFps;							// Ptr to zerofps engine.

	IPaddress				m_kLocalIP;								// Our Own Local IP.	***
	int						m_iMaxNumberOfNodes;					// Max Number of remote Nodes we can keep track of.	***

	vector<RemoteNode>	m_RemoteNodes;							// Data About all our remote connections.
	IPaddress				m_kServerAddress;						// Ip of the server we are conencted to.
	char						m_szAddressBuffer[256];				// Used to convert/print address.

	void	ClearRemoteNode(RemoteNode* pkNode);				// Reset a node ***
	void	SetMaxNodes(int iMaxNode);			

	bool Recv(NetPacket* pkNetPacket);							// Recv a packet if any is waiting.

	bool IsAddressEquals(IPaddress* pkAdr1, IPaddress* pkAdr2);

	void DisconnectAll();											// Send disconenct message to all nodes.

	float						m_fStatsUpdate;

public:
	NetWorkStatus			m_eNetStatus;
	vector<RemoteNode>	m_akClients;

	vector<ZFNet_String>		m_kStringTable;
	int NetString_GetFree();
	int NetString_Add(const char* szString);
	int NetString_GetIndex(const char* szString);
	string NetString_GetString(int iIndex);
	void NetString_ReSendAll();
	void Send_NetStrings();
	bool NetStringIsUpdated();
	void NetString_Request(int iIndex);
	void NetString_Refresh();

	bool Init();
	bool Close();

	NetWork();
	~NetWork();

	void DevShow_ClientConnections();

	int GetNumOfClients(void);
	int GetClientNumber(IPaddress* pkAddress);				// Get ID of client, CLIENT_UNCONNECTED if none.	***
	int GetFreeClientNum();

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

	void TEST_KeepAliveALL();										// Sends a NOP Controll message to all clients
	
	void RTS_RequestClientObjectID();


	bool CmpNetworkAddress();
	bool StrToAddress();
	bool AddressToStr(IPaddress* pkAddress, char* szString);

//	void ServerList(void);
	void DrawConnectionGraphs();

};

#endif







