#ifndef _ML_NETMESSAGES_H_
#define _ML_NETMESSAGES_H_

//	A network message from client to the server. 
enum MLNM_ClientToServer
{
	MLNM_CS_JIDDRA,
	MLNM_CS_MAX
};

//	A network message from the server to the client.
enum MLNM_ServerToClient
{
	MLNM_SC_MADDRAW,
	MLNM_SC_MAX
};


#endif