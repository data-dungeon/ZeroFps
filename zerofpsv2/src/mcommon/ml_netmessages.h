#ifndef _ML_NETMESSAGES_H_
#define _ML_NETMESSAGES_H_

//	A network message from client to the server. 
enum MLNM_ClientToServer
{
	MLNM_CS_REQ_CHARACTERID,
	MLNM_CS_CONTROLS,
	MLNM_CS_SAY,
	MLNM_CS_REQ_PLAYERLIST,
	MLNM_CS_REQ_KILLME,
	MLNM_CS_ANIM,
	MLNM_CS_ACTION,
	MLNM_CS_REQ_CONTAINER,
	MLNM_CS_MOVE_ITEM,
	
};

//	A network message from the server to the client.
enum MLNM_ServerToClient
{
	MLNM_SC_CHARACTERID,
	MLNM_SC_SAY,
	MLNM_SC_PLAYERLIST,
	MLNM_SC_CONTAINER,
};

//mistlands chatmessages ID's
enum MLCM_ChatMessages
{
	MLCM_TALK,
	MLCM_WISPER,
	MLCM_SHOUT,
	MLCM_OOC_TALK,
	MLCM_OOC_MESSAGE,
	MLCM_OOC_GROUP,
	MLCM_OOC_GUILD,
	MLCM_OOC_ALL,
};


#endif