#ifndef _ENGINE_NETPACKET_H_
#define _ENGINE_NETPACKET_H_

#include <vector>
#include "../basic/zfassert.h"
#include "engine_x.h"
#include <SDL/SDL_net.h>

using namespace std;

#define MAX_PACKET_SIZE					1024				// Max Bytes in each packet.

//	The ZeroFps packet header. Is added to the start of all network packets sent in zerofps.
#pragma pack( 1 )

/// The ZeroFps packet header.
/// Is added to the start of all network packets sent in zerofps.
struct ZFNetHeader
{
	int				m_iOrder;							//  Order of packet 
	unsigned char	m_iPacketType;						//  Type of packet.
};

/// The Full Data in a Network packet.
struct ZFNetPacketData
{
	ZFNetHeader		m_kHeader;							
	unsigned char	m_acData[MAX_PACKET_SIZE];		
};

#pragma pack(  )


/// A Packet that is sent over network.
class ENGINE_API NetPacket
{
public:
	NetPacket();
	~NetPacket();
	void Clear();
	

	ZFNetPacketData	m_kData;
	int					m_iLength;
	int					m_iPos;
	bool					m_bReadError;
	
	// Target Data
	/* Group data will be here. */
	vector<int>			m_iTargetClients;	// List of connections that this packet will be sent to.
	int					m_iClientID;		// Client ID this packet was send/recv from.
	IPaddress			m_kAddress;			// The IP this packet will be sent to or was recvd from.


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

	/* Target functions.
		Decides to who this packet will be sent. This can be one of the following.
			A Group:					
			One or more clients:	
			IP:						
		
		This list is also in order of importance. If one of them is set all others below it
		will be ignored.
		*/
	void TargetClear();							// Clear all targets for this package.
	void TargetSetClient(int iClientID);	// Clears all active clients targets and set only this one.
	void TargetAddClient(int iClientID);	// Add this to list of target clients.
	void TatgetClearClient();					// Clear Target list of clients.
	void SetTarget(const char* szIp);
};


#endif