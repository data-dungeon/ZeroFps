#ifndef _ENGINE_ZEROFPS_H_
#define _ENGINE_ZEROFPS_H_

#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <SDL/SDL.h>
#include "../ogl/zfpsgl.h"
#include "network.h"
#include "../basic/zfresourcedb.h"
#include "fh.h"
#include "entity.h"
#include "camera.h"
#include "../engine_systems/audio/zfaudiosystem.h"
#include "../basic/zfini.h"
#include "../engine_systems/tcs/tcs.h"
#include "../engine_systems/common/zshadow.h"
#include "../render/zshadersystem.h"
#include "i_zerofps.h"
#include "../engine_systems/mad/mad_core.h"
#include "res_texture.h"
#include "../script/zfscript.h"
#include "../engine_systems/script_interfaces/si_std.h"
#include "astar.h"
//#include "inputhandle.h"


#define ZF_VERSION_NUM		"ZF 0.01"


using namespace std;


class Basic;
class Render;
class ZGui;
class ZGuiResourceManager;
class GLGuiRender;
class ZFScriptSystem;
class Application;
class NetPacket;
class PSystemManager;
class Tcs;
class InputHandle;
class ZShadow;

enum enginestates
{
	state_normal,
	state_exit,
	state_console,
	state_pause
};


#define	ZFGP_OBJECTSTATE		1
#define	ZFGP_CLIENTSTATE		2
#define	ZFGP_CLIENTCMD			3
#define	ZFGP_PRINT				4
#define	ZFGP_DELETEOBJECT		5
#define	ZFGP_REQOWNOBJECT		6
#define	ZFGP_GIVEOWNOBJECT	7
#define	ZFGP_ZONELIST			8		// LIst of active zones sent to client.
#define	ZFGP_GETSTATICDATA	9
#define	ZFGP_STATICDATA		10
#define	ZFGP_COMMAND			12
#define	ZFGP_EDIT				13
#define  ZPGP_SS_APP				14		

#define	ZFGP_ENDOFPACKET		128

class DevStringPage
{
private:
public:
	bool			m_bVisible;
	string			m_kName;
	vector<string>	m_akDevString;
};

/// Information about a connectet client
class ENGINE_API ZFClient
{
public:
	float		m_fConnectTime;
	string	m_strLogin;				// Login Name
	string	m_strCharacter;
	Entity*	m_pkObject;				// Object used for client.
	set<int>	m_iActiveZones;		// Activated Zones.
	set<int>	m_iUnloadZones;		// Activated Zones.
};

/** \brief	Main class for the ZeroFps engine.
	 \ingroup Engine
*/
class ENGINE_API ZeroFps : public I_ZeroFps {
	private:
		enum FuncId_e
		{
			FID_SETDISPLAY,
			FID_QUIT,
			FID_SLIST,			// List all servers
			FID_CONNECT,		// Connect to server.
			FID_SERVER,			// Start a Server.
			FID_LISTMAD,		// List all loaded mad's.
			FID_PRINTOBJECT,	// Print all objects to external console.

			FID_VERSION,		// Print Version info.
			FID_CREDITS,		// Print Credits to console.
			FID_ECHO,

			FID_GLDUMP,

			// DevStrings
			FID_DEV_SHOWPAGE,
			FID_DEV_HIDEPAGE,
			FID_DEV_TOGGLE,

			FID_SCREENSHOOT,
			FID_MASSSPAWN,

			FID_SERVERCOMMAND,

			FID_POS // dumps x,y,z of camera
		};


		bool							m_bDevPagesVisible;
		vector<DevStringPage>	m_DevStringPage;					
		
		float 	m_fLastFrameTime;
		float 	m_fAvrageFpsTime;
		int		m_iAvrageFrameCount;

		bool		m_bLockFps;
		float 	m_fLockFrameTime;

		float 	m_fSystemUpdateFps;				// Number of GameLogic Updates each second.
		float		m_fSystemUpdateFpsDelta;		// Time between each gamelogic update.
		float 	m_fSystemUpdateTime;
		float		m_fEngineTime;						// Time since engine start.
		bool		m_bRenderOn;
		bool		m_bDrawAxisIcon;
		bool		m_bDebugGraph;						//shuld we show debug graphics, like spheres where theres lights etc
				
		bool		m_bTcsFullframe;					//shuld the tcs system run in full or system frame time
		
		int		m_iServerConnection;				// The Connection num we have on the server.		
		int		m_iClientEntityID;						
		int		m_iMaxPlayers;

		string	m_strCurentDir;							//current console dir , used by cd,dir
				
		//materials
		ZMaterial*	m_pkDevPageMaterial;
		
		
		vector<Camera*>	m_kRenderCamera;
		

		Camera *m_pkCamera;							//current camera
		Camera *m_pkConsoleCamera;					//camera for console


		void RunCommand(int cmdid, const CmdArgument* kCommand);
		void ConfigFileRun();
		void ConfigFileSave();
		void Run_EngineShell();
		void Run_Server();
		void Run_Client();
		void Draw_EngineShell();		
		void Update_System(bool bServer);
		void MakeDelay(); 

		
		void SetApp(void);
		void Swap(void);											//	swap gl buffers		
		void HandleNetworkPacket(NetPacket* pkNetPacket);
	
		void CreateMaterials();
		void RegisterPropertys();
		void RegisterResources();
		void GetEngineCredits(vector<string>& kCreditsStrings);
		void DrawDevStrings();		
		
		void DevPrint_Show(bool bVisible);
		DevStringPage*	DevPrint_FindPage(const char* szName);		
		
		
	public:

		/*
			All Engine Systems in ZeroFps. Listed in the same order they are created in
			ZeroFps();
		*/

		ZFBasicFS*				m_pkBasicFS;				///< Low level platform depended File Functions.
		ZFVFileSystem*			m_pkZFVFileSystem;		///< ZeroFps own File System.
		TextureManager*		m_pkTexMan;					///< Manges Textures
		Input*					m_pkInput;					///< Handles all Local Input.
		PropertyFactory*		m_pkPropertyFactory;		///< Property Creation for Objects.
		Frustum*					m_pkFrustum;				///< Global Frustum object for culling.
		Light*					m_pkLight;					///< Light Engine.
		Render*					m_pkRender;					///< Renders Graphic primitives
		Console*					m_pkConsole;				///< ZeroFps Console.
		EntityManager*			m_pkObjectMan;				///< In Game Object System.
		ZFAudioSystem*			m_pkAudioSystem;			///< Sound
		NetWork*					m_pkNetWork;				///< NetWork Interface.
		GLGuiRender*			m_pkGuiRenderer;			///< Gui - Zeb
		ZGuiResourceManager*	m_pkGuiMan;					///< Gui - Zeb
		ZGui*						m_pkGui;						///< Gui - Zeb
		ZFIni*					m_pkIni;						///< Ini Files - Zeb
		ZFResourceDB*			m_pkResourceDB;			///< Resouce Handler.
		ZFScriptSystem*		m_pkScript;					///< script
		Application*			m_pkApp;						///< Application object.
		ZShaderSystem*			m_pkZShaderSystem;		///< zerofps shader system
		OggMusic*				m_pkMusic;
		PSystemManager*		m_pkPSystemManager;		///< ParticleSystemManager - Zerom
		AStar*					m_pkAStar;
		Tcs*						m_pkTcs;
		ZShadow*					m_pkZShadow;

		int		m_iState;									//	curent game state see enum enginestates
		float		m_fFps;										//	curent FPS
		float		m_fAvrageFps;
		float		m_fFrameTime;								//	frametime in MS

		bool		m_bServerMode;
		bool		m_bClientMode;
		bool		m_bGuiMode, m_bGuiTakeControl;
		bool		m_bRunWorldSim;

		bool		m_bAlwaysWork;								///< Paramater that should be set if engine should work (ie. Update) no matter if the application is minimized or not.
		int		m_iMadDraw;									//	Flags for what part's of mad's that should be draw.
		float		m_fMadLod;									//	If not 0 then force this LOD % on every mad.
		int		m_iNumOfMadRender;
		
		
		
		InputHandle*	m_pkInputHandle;
		InputHandle*	m_pkGuiInputHandle;

		vector<ZFClient>		m_kClient;

		
		//basic engine funktions
		ZeroFps(void);
		~ZeroFps();
		bool Init(int iNrOfArgs, char** paArgs);
		void MainLoop(void);
		void QuitEngine();
		
		void StartServer(bool bClient = true,bool bNetwork = true,string strServerName="Unkown");
		void StartClient(string strLogin,string strPassword,string strServerIP,int iPort = 4242);
		void StopAll();
		
		//system 
		bool StartUp();
		bool ShutDown();
		bool IsValid();

		//dont know what these are doing here anyway =P
		void PrintToClient(int iConnectionID, const char* szMsg);
		void AddHMProperty(Entity* pkEntity,int iNetWorkId, Vector3 kZoneSize);
		
		// Timer Functions.
		float GetTicks()						{	return float((SDL_GetTicks()/1000.0));		}
		float GetFrameTime()					{	return float((m_fFrameTime/1000.0));		}
		float GetLastGameUpdateTime()		{	return m_fSystemUpdateTime;					}
		float GetEngineTime()				{	return m_fEngineTime;							}
		void SetSystemFps(int iFps) 		{	m_fSystemUpdateFps = float(iFps);			}
		float GetSystemUpdateFpsDelta() 	{	return m_fSystemUpdateFpsDelta;				}
		
		//camera
		Camera *GetCam()						{	return m_pkCamera;	}		//get current render camera, can be NULL

		//render targets
		void AddRenderCamera(Camera* pkCamera);
		void RemoveRenderCamera(Camera* pkCamera);
		void ClearRenderCameras();
		void Draw_RenderCameras();
		void Draw_RenderCamera(Camera* pkCamera);
		Camera* GetRenderCamera(string strName);
		
		//toggle functions		
		void ToggleFullScreen(void);
		void ToggleGui(void);
				
		//devpage
		void DevPrintf(const char* szName, const char *fmt, ...);

		//graphs
		bool GetDebugGraph() 				{	return m_bDebugGraph;	}
		void SetDebugGraph(bool bDebug)	{	m_bDebugGraph = bDebug;	}
		bool GetDrawAxesIcon() 				{	return m_bDrawAxisIcon;	}

		// Called by network.
		bool	PreConnect(IPaddress kRemoteIp, char* szLogin, char* szPass, bool bIsEditor, string& strWhy);
		int	Connect(int iConnectionID, char* szLogin, char* szPass, bool bIsEditor);
		void	Disconnect(int iConnectionID);
		int	GetClientObjectID();
		int	GetConnectionID() 			{	return m_iServerConnection;	}		///< Return our Connection Num on the Server.
		int	GetMaxPlayers() 				{	return m_iMaxPlayers;			}
		
		void HandleEditCommand(NetPacket* pkNetPacket);						
		void RouteEditCommand(NetPacket* pkNetPacket);						
	
		friend class NetWork;
		friend class Camera;
};





#endif




