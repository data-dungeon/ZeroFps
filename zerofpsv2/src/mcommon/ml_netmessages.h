#ifndef _ML_NETMESSAGES_H_
#define _ML_NETMESSAGES_H_

//	A network message from client to the server. 
enum MLNM_ClientToServer
{
	MLNM_CS_JIDDRA,
	MLNM_CS_MAX,
	
	MLNM_CS_REQ_CHARACTERID,
	MLNM_CS_CONTROLS,
};

//	A network message from the server to the client.
enum MLNM_ServerToClient
{
	MLNM_SC_MADDRAW,
	MLNM_SC_SETVIEW,
	MLNM_SC_MAX,
	
	MLNM_SC_CHARACTERID,
};


#endif