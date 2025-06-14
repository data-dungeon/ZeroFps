#pragma warning( disable : 4512) // 'class' : assignment operator could not be generated 

#include "network.h"
#include "zerofps.h"

#include "../basic/zfsystem.h"

extern ZSSNetWork* g_pkNetWork;



RemoteNode::RemoteNode()
{
	for(int i =0;i<ZF_NET_MAXREL;i++)
	{
		m_akRelPack[i] = 		new ZFNetPacketData();
		m_akRelPackRecv[i] = new ZFNetPacketData();	
	}

	Clear();
}

RemoteNode::~RemoteNode()
{		
	for(int i =0;i<ZF_NET_MAXREL;i++)
	{
		delete m_akRelPack[i];
		delete m_akRelPackRecv[i];
	}		
}

void RemoteNode::Clear()
{
	m_eConnectStatus		=	NETSTATUS_DISCONNECT;
	m_kAddress.host		=	INADDR_NONE;
	m_kAddress.port		=  0;

	m_iReliableSendOrder = 0;
	m_iReliableRecvOrder = 0;

	m_iNumOfPacketsSent	= 0;
	m_iNumOfPacketsRecv  = 0;
	m_iNumOfBytesSent    = 0;
	m_iNumOfBytesRecv    = 0;
	m_iNumOfBytesRecvNetFrame = 0;

	m_fLastMessageTime    = 0;
	m_fPing					 = 0;
	m_fPingSentEngineTime = 0;
	m_iRelPingIndex		 = 0;

	
	m_iOutOfOrderRecv			= 0;
	m_iPacketLossRecv			= 0;
	m_iOutOfOrderNetFrame	= 0;
	m_iLastRecvPacket			= 0;
	m_iCurrentObject			= 0;
	m_iNetSpeed					= 3000;

	m_kRecvGraph.SetMinMax(0,5000);		
	m_kRecvGraph.SetSize(100,100,25);
	m_kRecvGraph.SetBackColor(1,1,1);
	m_kRecvGraph.SetDrawColor(1,0,0);

	m_kRecvSizeGraph.SetMinMax(0,1024);		
	m_kRecvSizeGraph.SetSize(100,100,25);
	m_kRecvSizeGraph.SetBackColor(0.8, 0.8, 0.8);
	m_kRecvGraph.SetDrawColor(0,1,0);

	for(int i=0; i<ZF_NET_MAXREL; i++) 
	{
		m_akRelPack[i]->m_kHeader.m_iPacketType = ZF_NETTYPE_NONE;
		m_akRelPackRecv[i]->m_kHeader.m_iPacketType = ZF_NETTYPE_NONE;
	}

	m_kRelAckList.clear();
	m_kRelSend.clear();

	m_bIsFlood = false;
}

void RemoteNode::SetAddress(IPaddress* pkAddress)
{
	m_kAddress = *pkAddress;

}

int RemoteNode::GetFreeRelStore()
{
	for(int i=0; i<ZF_NET_MAXREL; i++) {
		if(m_akRelPack[i]->m_kHeader.m_iPacketType == ZF_NETTYPE_NONE)
			return i;
		}

	return -1;
}

void RemoteNode::FreeRelStore(ZFNetPacketData* pkRel)
{
	pkRel->m_kHeader.m_iPacketType = ZF_NETTYPE_NONE;
}

void RemoteNode::FreeRelStore(int iRelID)
{
	for(int i=0; i<ZF_NET_MAXREL; i++) 
	{
		if(m_akRelPack[i]->m_kHeader.m_iOrder == iRelID)
			m_akRelPack[i]->m_kHeader.m_iPacketType = ZF_NETTYPE_NONE;
	}
}

int RemoteNode::GetRel(int iID)
{
	for(int i=0; i<ZF_NET_MAXREL; i++) {
		if(m_akRelPack[i]->m_kHeader.m_iPacketType != ZF_NETTYPE_NONE && m_akRelPack[i]->m_kHeader.m_iOrder == iID)
			return i;
		}

	return -1;
}


int RemoteNode::GetFreeRelRecv()
{
	for(int i=0; i<ZF_NET_MAXREL; i++) {
		if(m_akRelPackRecv[i]->m_kHeader.m_iPacketType == ZF_NETTYPE_NONE)
			return i;
		}

	return -1;
}

int RemoteNode::GetRelRecv(int iID)
{
	for(int i=0; i<ZF_NET_MAXREL; i++) {
		if(m_akRelPackRecv[i]->m_kHeader.m_iPacketType != ZF_NETTYPE_NONE && m_akRelPackRecv[i]->m_kHeader.m_iOrder == iID)
			return i;
		}

	return -1;
}

void RemoteNode::FreeRelRecv(ZFNetPacketData* pkRel)
{
	pkRel->m_kHeader.m_iPacketType = ZF_NETTYPE_NONE;
}

void RemoteNode::FreeRelRecv(int iOrder)
{
	for(int i=0; i<ZF_NET_MAXREL; i++)
	{
		if(m_akRelPackRecv[i]->m_kHeader.m_iOrder == iOrder)
		{
				m_akRelPackRecv[i]->m_kHeader.m_iPacketType = ZF_NETTYPE_NONE;
		}
	}
}








NetPacket::NetPacket()
{
	m_iLength	= 0;
	m_iPos		= 0;
	m_iClientID = ZF_NET_NOCLIENT;
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

void NetPacket::WriteNp(NetPacket* pkNp)
{
	ZFAssert((m_iPos + pkNp->m_iLength) < MAX_PACKETDATA_SIZE, "NetPacket::Write_Str");
	
	unsigned char * add = &m_kData.m_acData[m_iPos];

	memcpy(add, &pkNp->m_kData.m_acData[0], pkNp->m_iLength);
	m_iPos +=  pkNp->m_iLength;
	m_iLength += pkNp->m_iLength;

}

void NetPacket::WriteNp(NetPacket* pkNp,int iStart,int iSize)
{
	if( ( pkNp->m_iLength - iStart ) < iSize)
		iSize = ( pkNp->m_iLength - iStart );

	ZFAssert((m_iPos + iSize) < MAX_PACKETDATA_SIZE, "NetPacket::Write_Str");
		
		
	unsigned char * add = &m_kData.m_acData[m_iPos];
	
	memcpy(add, &pkNp->m_kData.m_acData[iStart], iSize);
	
	m_iPos +=  iSize;
	m_iLength += iSize;

}

void NetPacket::Write_Str(const string& strString)
{
	ZFAssert((m_iPos + strString.size() + 1) < MAX_PACKETDATA_SIZE, "NetPacket::Write_Str");

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
 	ZFAssert((m_iPos + strlen(szString) + 1) < MAX_PACKETDATA_SIZE, "NetPacket::Write_Str");

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

void NetPacket::Write_Version(const ZFVersion& kVersion)
{
	Write((void*)&kVersion.m_iMajor, sizeof(kVersion.m_iMajor));	
	Write((void*)&kVersion.m_iMinor, sizeof(kVersion.m_iMinor));	
	Write((void*)&kVersion.m_iBuild, sizeof(kVersion.m_iBuild));	
}

void NetPacket::Read_Version(ZFVersion& kVersion)
{
	Read(&kVersion.m_iMajor, sizeof(kVersion.m_iMajor));	
	Read(&kVersion.m_iMinor, sizeof(kVersion.m_iMinor));	
	Read(&kVersion.m_iBuild, sizeof(kVersion.m_iBuild));	

}

void NetPacket::Write_NetStr (const char* szString)
{
	int iIndex = g_pkNetWork->NetString_GetIndex(szString);
	
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
	ZFAssert((m_iPos + iSize) < MAX_PACKETDATA_SIZE, "NetPacket::Write");

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

void NetPacket::TargetClear()	
{
	// Set Inavlid Socket.
	m_iTargetClients.clear();
	m_iClientID = -3;
}

void NetPacket::TargetSetClient(int iClientID)
{
	m_iTargetClients.clear();
	
	if(iClientID == ZF_NET_ALLCLIENT) {
		for(int i=0; i<g_pkNetWork->m_iMaxNumberOfNodes; i++)
			m_iTargetClients.push_back(i);
	}
	else
		m_iTargetClients.push_back(iClientID);
}

void NetPacket::TargetAddClient(int iClientID)	
{
	m_iTargetClients.push_back(iClientID);
}

void NetPacket::TatgetClearClient()		
{
	m_iTargetClients.clear();
}

