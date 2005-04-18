#ifndef _ENGINE_NETPACKET_H_
#define _ENGINE_NETPACKET_H_

#include <vector>
#include <string>
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


#pragma warning( disable : 4512) // 'class' : assignment operator could not be generated 


/// A Packet that is sent over network.
class NetPacket
{
public:
	NetPacket();
	~NetPacket();
	void Clear();
	

	ZFNetPacketData	m_kData;
	int					m_iLength;
	int					m_iPos;
	bool					m_bReadError;
	
	IPaddress			m_kAddress;			// The IP this packet will be sent to or was recvd from.


	void SetError(bool bError) { m_bReadError = bError;	}
	bool IsReadError() { return m_bReadError; }

	void WriteNp(NetPacket* pkNp);

	void Write_Str(const char* szString);
	void Read_Str(char* szString);

	void Write_Str(const string& strString);
	void Read_Str(string& strString);
	

	template <class Any> 
	void Write(Any type) {
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


#endif










