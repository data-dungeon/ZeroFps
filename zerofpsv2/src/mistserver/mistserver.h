#ifndef _MISTSERVER_H_
#define _MISTSERVER_H_

#ifndef _DONT_MAIN
	#define _MAINAPPLICATION_
	#define _DONT_MAIN
#endif

#include <iostream>
#include <cmath>
#include <list>
#include "../zerofpsv2/engine/application.h"
#include "../zerofpsv2/engine_systems/mad/mad_modell.h"
#include "../zerofpsv2/engine/astar.h"

#include "../zerofpsv2/engine_systems/propertys/p_camera.h"
#include "../zerofpsv2/gui/zgui.h"

#include <vector>


#include "../mcommon/p_event.h"
#include "../mcommon/si_mistland.h"
#include "../mcommon/p_ml.h"
#include "../mcommon/p_charstats.h"
#include "../mcommon/p_item.h"
#include "../mcommon/p_container.h"
#include "../mcommon/p_spell.h"
#include "../mcommon/p_serverinfo.h"
#include "../mcommon/p_clientcontrol.h"
#include "../mcommon/p_enviroment.h"
#include "../mcommon/p_ai.h"
#include "../mcommon/p_spawn.h"

#include "../mcommon/p_dmhq.h"
#include "../mcommon/p_dmcharacter.h"
#include "../mcommon/p_shadowblob.h"
#include "../mcommon/p_dmgun.h"
#include "../mcommon/p_dmitem.h"
#include "../mcommon/p_dmmission.h"
#include "../mcommon/p_dmclickme.h"
#include "../mcommon/p_dmshop.h"
#include "../mcommon/p_car.h"

#include "../mcommon/p_arcadecharacter.h"

#include "playerdatabase.h"



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
		};

		//enviroment system
		ZSSEnviroment*						m_pkEnviroment;
		LightSource							m_kSun;
		
		//timer
		ZSSMLTime*							m_pkTime;		
		
		//server stuff
		Entity*								m_pkServerInfo;
		P_ServerInfo*						m_pkServerInfoP;

		vector<pair<string,Vector3> >	m_kLocations;
		
		bool									m_AcceptNewLogins;
		bool									m_bStartMinimized;

		//edit stuff
		Entity*	m_pkCameraObject;
		Camera*	m_pkCamera;
		Entity*	m_pkActiveCameraObject;
		Camera*	m_pkActiveCamera;
		string	m_strWorldDir;						// The current dir for the world. Use for SaveAs and Title.

		void  CreateEditCameras();	
		
		void SetupGuiEnviroment();
		void Input_Camera(float fMouseX, float fMouseY);
		void DeletePlayerCharacter(int iConID);		
		
		void SendPlayerListToClient(int iClient);				
		Vector3 GetPlayerStartPos();

		int CreatePlayer(const char* csPlayer,const char* csCharacter,const char* csLocation,int iConID);		
		void SpawnPlayer(int iConID);	

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
				
		void RunCommand(int cmdid, const CmdArgument* kCommand);		
		void Init();
		void Input();
		void OnServerStart(void);
		void OnClientStart(void);
		
		//init client
		void ClientInit();
		

		//on client join, server runs this
		bool OnPreConnect(IPaddress kRemoteIp, char* szLogin, char* szPass, bool bIsEditor, string& strWhy);
		void OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass, bool bIsEditor);
		void OnServerClientPart(ZFClient* pkClient,int iConID);
		void RenderInterface(void);
		void OnNetworkMessage(NetPacket *PkNetMessage);

		bool StartUp();
		bool ShutDown();
		bool IsValid();

		void SayToClients(const string& strMsg,int iClientID = -2);				
		
};


//script interface for mistserver
namespace SI_MistServer
{
	int SayToCharacterLua(lua_State* pkLua);
};



#endif











