#ifndef _ENGINE_NETWORK_H_
#define _ENGINE_NETWORK_H_

#include "deque"
#include "set"

#pragma warning( disable : 4512) // 'class' : assignment operator could not be generated 

#include "../basic/zfassert.h"
#include "engine_x.h"
#include "console.h"
#include "netpacket.h"
#include <SDL/SDL_net.h>
#include "fh.h"

using namespace std;


#define MAX_NETWORKNAME					16					

// Diffrent types of messages the network sends.
#define ZF_NETTYPE_NONE					0	// A Unspec Packet.
#define ZF_NETTYPE_CONTROL				1	// A Connection controll message
#define ZF_NETTYPE_UNREL				2	// Unrel message that ZeroFps handles.
#define ZF_NETTYPE_REL					3	// Rel Udp message that ZeroFps handles.

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
#define ZF_NETCONTROL_ACKREL			15	// Ack that we got a rel pack.	

#define MAX_NET_CLIENTS					4		// Max number of clients (nodes).
#define ZF_NET_NOCLIENT					-1		// ID for a non client.
#define ZF_NET_ALLCLIENT				-2		// ID for a all clients.

#define ZF_NET_CONNECTION_TIMEOUT	45	// Timeout connection if no message from a client after this time (sec).

#define ZF_NET_MAXREL					20

// Status of a remote node.
enum ClientConnectStatus
{
	NETSTATUS_CONNECTING,				// Trying to connect.
	NETSTATUS_CONNECTED,					// A Connected client
	NETSTATUS_DISCONNECT					// Unconnected (free).
};



/// A Remote Node on the network.
class RemoteNode
{
public:
	RemoteNode();
	~RemoteNode();

	IPaddress					m_kAddress;				// Address of node.
	ClientConnectStatus		m_eConnectStatus;		// Status of this nodes connection.

	void SetAddress(IPaddress* pkAddress);			// Sets address of node.
	void Clear();

	int				m_iReliableSendOrder;			// Next Order num to use for next reliable packet.
	int				m_iReliableRecvOrder;			// The Order num of next reliable packet we are waiting for.
	bool			m_bIsFlood;

	// Stats
	int				m_iNumOfPacketsSent;				// Total num of packets sent (Any type).
	int				m_iNumOfPacketsRecv;				// Total num of packets recv (Any type).
	unsigned int	m_iNumOfBytesSent;				// Total num of bytes sent.
	unsigned int	m_iNumOfBytesRecv;				// Total num of bytes recv
	
	int				m_iNumOfBytesRecvNetFrame;		

	float				m_fLastMessageTime;				// Time (Engine) of last message. Use to find time outs.
	float				m_fPing;								// Ping 
	float				m_fPingSentEngineTime;
	int				m_iRelPingIndex;					// Last Reliable packet used to calculate ping.


	int				m_iOutOfOrderRecv;
	int				m_iPacketLossRecv;

	DebugGraph		m_kRecvGraph;
	DebugGraph		m_kRecvSizeGraph;

	int				m_iOutOfOrderNetFrame;

	int				m_iLastRecvPacket;				// Order num of last recv packet.

	unsigned int	m_iCurrentObject;							//current position in zoneobject list, for packtoclient
	
	ZFNetPacketData	m_akRelPack[ZF_NET_MAXREL];
	float					m_akRelPackSendTime[ZF_NET_MAXREL];
	int					m_aiRelPackSize[ZF_NET_MAXREL];
	set<int>				m_kRelSend;										// Sorted set with Id's for all packets that been sent but not ack.
   
	ZFNetPacketData	m_akRelPackRecv[ZF_NET_MAXREL];			// Reliable packets that have been recv out of order.
	int					m_aiRelPackRecvSize[ZF_NET_MAXREL];

	vector<int>			m_kRelAckList;

	int GetFreeRelStore();
	void FreeRelStore(ZFNetPacketData* pkRel);
	void FreeRelStore(int iRelID);
	int GetRel(int iID);

	int GetFreeRelRecv();
	int GetRelRecv(int iID);
	void FreeRelRecv(ZFNetPacketData* pkRel);
	void FreeRelRecv(int iOrder);
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
/// Strings Optimized for network.
struct ZFNet_String
{
	bool		m_bInUse;		// True if this net string is in use.
	bool		m_bUpdated;		// True if is updated.
	string	m_NetString;	// Da string
	bool		m_bNeedUpdate;	// True if we need to request a update from server.
};

/** \brief	NetWork SubSystem
	 \ingroup Engine
*/
class ENGINE_API NetWork : public ZFSubSystem 
{
private:
	UDPsocket				m_pkSocket;								// Socket we use for all our messages.

	char						m_szServerName[MAX_NETWORKNAME];	// Well ... VIM... ***
	bool						m_bAcceptClientConnections;			// If false all connect attemts are ignored.
	Console*					m_pkConsole;							// Ptr to console.
	ZeroFps*					m_pkZeroFps;							// Ptr to zerofps engine.

	IPaddress				m_kLocalIP;								// Our Own Local IP.	***
	int						m_iMaxNumberOfNodes;					// Max Number of remote Nodes we can keep track of.	***

	int						m_iNetSpeed;							// Max amount of bytes to send on a connection. 
	int						m_iMaxSendSize;						// Max bytes to send to client per frame


	vector<RemoteNode>	m_RemoteNodes;							// Data About all our remote connections.
	char						m_szAddressBuffer[256];				// Used to convert/print address.

	float						m_fStatsUpdate;
	
	int						m_iDefPort;

	// Console Variables
	float						m_fConnectTimeOut;					// Num of seconds without any incoming data a connection times out.
	int						m_iMaxClients;							// Num of max connected clients.

	void	ClearRemoteNode(RemoteNode* pkNode);				// Reset a node ***
	bool Recv(NetPacket* pkNetPacket);							// Recv a packet if any is waiting.
	bool IsAddressEquals(IPaddress* pkAdr1, IPaddress* pkAdr2);
	void DisconnectAll();											// Send disconenct message to all nodes.

	void SendAckList(int iClient, vector<int>& kAckList);

public:
		enum FuncId_e
		{
			FID_NETGMAX,
			FID_DNS,
		};

	string	GetLocalIP();			
	NetWorkStatus			m_eNetStatus;
//	vector<RemoteNode>	m_akClients;
	IPaddress				m_kServerAddress;						// Ip of the server we are conencted to.

	NetWork();
	~NetWork();
	bool StartUp();
	bool ShutDown();
	bool IsValid();

	bool Init();
	bool Close();

	void StartSocket(bool bStartServer,int iPort = 4242);
	void CloseSocket();
	void ServerStart(int iPort);
	void ServerEnd(void);
	void ClientStart(const char* szIp, const char* szLogin, const char* szPass, bool bConnectAsEditor);
	void ClientEnd(void);

	void	SetMaxNodes(int iMaxNode);			
	int GetNumOfClients(void);
	int GetClientNumber(IPaddress* pkAddress);				// Get ID of client, CLIENT_UNCONNECTED if none.	***
	int GetFreeClientNum();
	
	int GetMaxSendSize() {return m_iMaxSendSize;} 
	void SetMaxSendSize(int iMaxSend) { m_iMaxSendSize = iMaxSend; }
	int GetNetSpeed() { return m_iNetSpeed; }

	void UpdateNetSpeed();


	bool IsConnected(int iId);
	
	// NetStrings
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

	// Send
	bool SendRaw(NetPacket* pkNetPacket);
	bool Send2(NetPacket* pkNetPacket);
	void SendUDP(ZFNetPacketData* pkData, int iSize, IPaddress* pkIp);

//	void SendToAllClients(NetPacket* pkNetPacket);
//	void SendToClient(int iClient, NetPacket* pkNetPacket);

	// Recv
	void HandleControlMessage(NetPacket* pkNetPacket);		// Handle controll messages used by network layer. ***
	void Run(); 

	void RunCommand(int cmdid, const CmdArgument* kCommand);

	bool AddressToStr(IPaddress* pkAddress, char* szString);
	bool StrToAddress(const char* szString, IPaddress* pkAddress);

	bool IsValidIPAddress( const char* szString );
	bool DnsLookUp(const char* szHost,IPaddress& kIp);


	// Debug
	void DrawConnectionGraphs();
	void DevShow_ClientConnections();

	friend class NetPacket;
	friend class EntityManager;	
};


#endif







