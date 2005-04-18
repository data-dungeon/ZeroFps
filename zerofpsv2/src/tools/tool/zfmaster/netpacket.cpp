#pragma warning( disable : 4512) // 'class' : assignment operator could not be generated 

#include "netpacket.h"

NetPacket::NetPacket()
{
	m_iLength	= 0;
	m_iPos		= 0;
	m_kData.m_kHeader.m_iOrder = 0;
	m_kData.m_kHeader.m_iPacketType = -1;
}

NetPacket::~NetPacket()
{

}

void NetPacket::Clear()
{
	m_iLength = 0;
	m_iPos = 0;
	m_bReadError = false;
	m_kData.m_kHeader.m_iOrder = 0;
	m_kData.m_kHeader.m_iPacketType = -1;
}


void NetPacket::WriteNp(NetPacket* pkNp)
{
	unsigned char * add = &m_kData.m_acData[m_iPos];

	memcpy(add, &pkNp->m_kData.m_acData[0], pkNp->m_iLength);
	m_iPos +=  pkNp->m_iLength;
	m_iLength += pkNp->m_iLength;

}

void NetPacket::Write_Str(const string& strString)
{
	Write(strString.size()+1);
	Write((void*)strString.c_str(),strString.size()+1);
}

void NetPacket::Read_Str(string& strString)
{
	int iSize;
	Read(iSize);
	char* czStr = new(char[iSize]);
	
	Read(czStr,iSize);	
	strString = czStr;
	
	delete czStr;
}

void NetPacket::Write_Str(const char* szString)
{
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

void NetPacket::Write(void* ptr, int iSize)
{
//	ZFAssert((m_iPos + iSize) < MAX_PACKET_SIZE, "NetPacket::Write");

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

/*
	Set type of packet to send. Can be Control, Rel or UnRel.
*/
void NetPacket::SetType(int iType)
{
	m_kData.m_kHeader.m_iPacketType = iType;
}


