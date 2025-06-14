#ifndef _MISTSERVER_H_
#define _MISTSERVER_H_

#ifndef _DONT_MAIN
	#define _MAINAPPLICATION_
	#define _DONT_MAIN
#endif


#include <vector>

#include "../zerofpsv2/engine/application.h"
#include "../zerofpsv2/gui/zgui.h"
#include "playerdatabase.h"


class Camera;
class ZSSEnviroment;
class ZSSMLTime;
class Entity;
class P_ServerInfo;
//class RuleSystem;

/**	\brief	Da MistServer
		\ingroup MistServer

The main class for the Server of ZeroFps. 
*/
class MistServer :public Application , public ZGuiApp 
{
	private:
		//console funktions
		enum FuncId_e
		{
			FID_NEW,
			FID_LOAD,
			FID_SAVE,
			FID_SAVEAS,
			FID_USERS,
			FID_LOCALORDER,
			FID_LIGHTMODE,
			FID_TEST_JIDDRA,		// Owned by Vim.
			
			FID_SAY,

			FID_USERADD,
			FID_USERDEL,
			FID_USERPASS,
			FID_USERLIST,
			FID_USERPREV,
		};

		//enviroment system
		ZSSEnviroment*						m_pkEnviroment;
		LightSource							m_kSun;
		
		//timer
		ZSSMLTime*							m_pkTime;		
		
		//server stuff		
		P_ServerInfo*						m_pkServerInfoP;

		vector<pair<string,Vector3> >	m_kLocations;
		
		ConVar								m_kAcceptNewLogins;
		ConVar								m_kbStartMinimized;
		ConVar								m_kiServerPort;
		
		
		//edit stuff
		Entity*	m_pkCameraObject;
		Camera*	m_pkCamera;
		Entity*	m_pkActiveCameraObject;
		Camera*	m_pkActiveCamera;
		string	m_strWorldDir;						// The current dir for the world. Use for SaveAs and Title.

		//camera
		void  CreateEditCameras();	
		void Input_Camera(float fMouseX, float fMouseY);
		
		//gui
		void SetupGuiEnviroment();		
		
		//network sends
		void SendPlayerListToClient(int iClient);
		void SendContainer(int iContainerID,int iClientID,bool bOpen);
		void SendItemInfo(int iItemID,int iClientID);
		void SendCharacterEquipment(int iCharacter,int iClientID);				
		void SendCharacterList(int iClient);
		void SendClientCharacterID(int iClientID);
		void SendPlayerHit(int iClientID,int iDamage);

		//player creation
		Vector3 GetPlayerStartPos();
		int CreatePlayer(const char* csPlayer,const char* csCharacter,const char* csLocation,int iConID);		
		bool SpawnPlayer(int iConID);	
		void DeletePlayerCharacter(int iConID);		
		int	m_iNextGroupId;

		//register stuff
		void RegisterScriptFunctions();
		void RegisterPropertys();
		void RegisterResources();		

						
	public:
		PlayerDatabase*	m_pkPlayerDB;				
		
		
		MistServer(char* aName,int iWidth,int iHeight,int iDepth);	 	
	 	void OnInit(void);
		void OnIdle(void);
		void OnHud(void);
		void OnSystem();
		
		bool StartUp();
		bool ShutDown();
		bool IsValid();
				
		void RunCommand(int cmdid, const ConCommandLine* kCommand);		
		void Init();
		void Input();
		void OnServerStart(void);
		void OnClientStart(void);
		
		void OnSystemMessage(const string& strType,int iNrOfParam,const void** pkParams);
		void OnDmc(int iClientID, string strDmc);
		void OnPCmd(int iClientID, string strDmc);

		//init client
		void ClientInit();
		

		//on client join, server runs this
		bool OnPreConnect(IPaddress kRemoteIp, char* szLogin, char* szPass, bool bIsEditor, string& strWhy);
		void OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass, bool bIsEditor);
		void OnServerClientPart(ZFClient* pkClient,int iConID);
		void RenderInterface(void);
		void OnNetworkMessage(NetPacket *PkNetMessage);


		//game stuff
		void SayToClients(const string& strMsg,const string& strSource = "Server", int iCharacterID = -1,int iClientID = -2);				
		void SendPointText(const string& strText,const Vector3& kPos,const Vector3& kVel,float fTTL,int iType);
		void OpenContainer(int iContainerID,int iClientID);
		void GiveGroupXP(const int iEntityID, int iXP);
};


//script interface for mistserver
namespace SI_MistServer
{
	int SayToCharacterLua(lua_State* pkLua);
	int OpenContainerLua(lua_State* pkLua);

	
};



#endif











