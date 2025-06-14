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
#include "fh.h"
#include "entity.h"
#include "camera.h"
#include "i_zerofps.h"
#include "../render/res_texture.h"
#include "../script/zfscript.h"
#include "../engine_systems/script_interfaces/si_std.h"
#include "../basic/zfversion.h"
#include "../basic/concommand.h"

using namespace std;

class Basic;
class ZSSRender;
class ZGui;
class ZGuiResourceManager;
class GLGuiRender;
class ZSSScriptSystem;
class Application;
class NetPacket;
class PSystemManager;
class Tcs;
class InputHandle;
class ZShadow;
class ZSSAudioSystem;
class Network;
class ZSSAStar;
class ZSSRenderEngine;

// #define	ZFGP_OBJECTSTATE		1
// #define	ZFGP_CLIENTSTATE		2
// #define	ZFGP_CLIENTCMD			3
// #define	ZFGP_PRINT				4
// #define	ZFGP_DELETEOBJECT		5
// #define	ZFGP_REQOWNOBJECT		6
// #define	ZFGP_GIVEOWNOBJECT	7
// #define	ZFGP_COMMAND			12
// #define	ZFGP_EDIT				13
// #define  ZPGP_SS_APP				14		
// #define  ZPGP_ZED_ZONELIST		15
// #define	ZPGP_DELETELIST		16
// #define	ZFGP_ENDOFPACKET		128

enum ZPGP_IDS
{
	ZFGP_OBJECTSTATE		=1,
	ZFGP_CLIENTSTATE		=2,
	ZFGP_CLIENTCMD			=3,
	ZFGP_PRINT				=4,
	ZFGP_DELETEOBJECT		=5,
	ZFGP_REQOWNOBJECT		=6,
	ZFGP_GIVEOWNOBJECT	=7,
	ZFGP_COMMAND			=12,
	ZFGP_EDIT				=13,
	ZPGP_SS_APP				=14,	
	ZPGP_ZED_ZONELIST		=15,
	ZPGP_DELETELIST		=16,
	ZFGP_ENDOFPACKET		=128,	
};

/// a dev page
class DevStringPage
{
	public:
		bool				m_bVisible;
		string			m_kName;
		vector<string>	m_akDevString;
};

/// Information about a connectet client
class ENGINE_API ZFClient
{
	public:	
		bool			m_bLogin;					// True if client is in login state.
		float			m_fConnectTime;
		string		m_strLogin;					// Login Name
		string		m_strCharacter;
		Entity*		m_pkObject;					// Object used for client.	
		bool			m_bIsEditor;		
		set<int>		m_kActiveZones;		// Activated Zones.
		set<int>		m_kUnloadZones;		// Activated Zones.
		queue<int>	m_kDeleteQueue;		// clients delete queue (contains is of entitys that shuld be deleted on the client)
};


/** \brief	Main class for the ZeroFps engine.
	 \ingroup Engine
*/

class ENGINE_API ZSSZeroFps : public ZFSubSystem, public I_ZeroFps 
{
	private:
		enum ENGINE_STATES
		{
			ENGINE_STATE_NORMAL,
			ENGINE_STATE_EXIT,
		};
	
		enum FuncId_e
		{
			FID_SETDISPLAY,	// Set resulution,depth and fullscreen
			FID_QUIT,			// Set exit state
			FID_SLIST,			// List all servers
			FID_CONNECT,		// Connect to server.
			FID_SERVER,			// Start a Server.
			FID_LISTMAD,		// List all loaded mad's.
			FID_PRINTOBJECT,	// Print all objects to external console.
			FID_VERSION,		// Print Version info.
			FID_CREDITS,		// Print Credits to console.
			FID_ECHO,			// Echo input
			FID_GLDUMP,			// Dump gl states			
			FID_DEV_SHOWPAGE,	// Show a devpage
			FID_DEV_HIDEPAGE,	// Hide a devpage
			FID_DEV_TOGGLE,	// Show/hide a devpage
			FID_SCREENSHOOT,	// Take screenshot at end of frame
			FID_MASSSPAWN,		// Some weird shit
			FID_POS, 			// Dumps x,y,z of camera
			FID_PROFILEMODE,
			
			FID_CAMERALIST,			// List rendered cameras
			FID_CAMERAADDPLUGIN,		// Add a pre render plugin to a camera			
			FID_CAMERAREMOVEPLUGIN,	// Add a pre render plugin to a camera			
			FID_RENDERPLUGINLIST,
		};

		ZFVersion		m_kVersion;

		bool							m_bDevPagesVisible;
		vector<DevStringPage>	m_DevStringPage;					
		
		//time and fps
		float 			m_fLastFrameTime;
		float 			m_fAvrageFpsTime;
		int				m_iAvrageFrameCount;
		unsigned int	m_iCurrentFrame;
		float				m_fEngineTime;						// Time since engine start.
		ConVar			m_kbLockFps;
		float 			m_fLockFrameTime;
		ConVar 			m_kfSystemUpdateFps;				// Number of GameLogic Updates each second.
		float				m_fSystemUpdateFpsDelta;		// Time between each gamelogic update.
		float 			m_fSystemUpdateTime;				// last system update
		
		bool				m_bProfileMode;

		//network
		ConVar			m_kbSyncNetwork;
		ConVar			m_kfNetworkUpdateFps;				// number of network updates each second
		float				m_fNetworkUpdateFpsDelta;		// time between each network update
		float 			m_fNetworkUpdateTime;			// last system update
		ConVar			m_kiConnectionSpeed;				// speed of outgoing connections

		int				m_iServerConnection;				// The Connection num we have on the server.		
		int				m_iClientEntityID;						
		ConVar			m_kiMaxPlayers;
				
		bool				m_bClientLoginState;				// True if loginstate is used for clients.
				
		//rendering and such
		ConVar			m_kbRenderOn;
		bool				m_bMinimized;
		ConVar			m_kbDebugGraph;						//shuld we show debug graphics, like spheres where theres lights etc

		ConVar			m_kbShadowMap;
		ConVar			m_kbShadowMapRealtime;
		ConVar			m_kiShadowMapMode;
		ConVar			m_kiShadowMapQuality;

		ConVar			m_kAxisIcon;
		ConVar			m_kVegetation;
		ConVar			m_kVegitationDistance;		
		ConVar			m_kViewDistance;
	

		ConVar			m_kbTcsFullframe;					//shuld the tcs system run in full or system frame time
		


		//profile information
		vector<TimerInfo > 	m_kProfileData;		
		int						m_iProfileTotalTime;
				
		//materials
		ZMaterial*			m_pkDevPageMaterial;
		
		
		vector<Camera*>	m_kRenderCamera;
		

		Camera *m_pkCamera;							//current camera
		Camera *m_pkConsoleCamera;					//camera for console


		void RunCommand(int cmdid, const ConCommandLine* kCommand);
		void ConfigFileRun();
		void ConfigFileSave();
		void Run_EngineShell();
		void Run_Server();
		void Run_Client();
		void Draw_EngineShell();		
		void Update_System();				//system updates
		void Update_Network();				//network updates
		void MakeDelay(); 

		void UpdateDevPages();
		void UpdateGuiInput();
		void UpdateMouse();
		
		void SetApp(void);
		void Swap(void);											//	swap gl buffers		
		void HandleNetworkPacket(NetPacket* pkNetPacket);
	
		void CreateMaterials();
		void RegisterPropertys();
		void RegisterResources();
		void GetEngineCredits(vector<string>& kCreditsStrings);
		void DrawDevStrings();				
		DevStringPage*	DevPrint_FindPage(const string& strName);		
		
		//zeroed network stuff
		void SendZoneList(bool bRemove,int iClientID,int iZoneID = -1);
		void RotateZoneModel(int iZoneID,Vector3 kRot);
		vector<int>	GetSelected(NetPacket* pkNetPack);

	public:

		/*
			All Engine Systems in ZeroFps. Listed in the same order they are created in
			ZeroFps();
		*/

		ZSSBasicFS*				m_pkBasicFS;				///< Low level platform depended File Functions.
		ZSSVFileSystem*		m_pkZFVFileSystem;		///< ZeroFps own File System.
		TextureManager*		m_pkTexMan;					///< Manges Textures
		ZSSInput*				m_pkInput;					///< Handles all Local Input.
		ZSSPropertyFactory*	m_pkPropertyFactory;		///< Property Creation for Objects.
		Frustum*					m_pkFrustum;				///< Global Frustum object for culling.
		ZSSLight*				m_pkLight;					///< ZSSLight Engine.
		ZSSRender*				m_pkRender;					///< Renders Graphic primitives
		ZSSConsole*				m_pkConsole;				///< ZeroFps Console.
		ZSSEntityManager*		m_pkEntityManager;		///< In Game Object System.
		ZSSAudioSystem*		m_pkAudioSystem;			///< Soundy
		ZSSNetWork*				m_pkNetWork;				///< NetWork Interface.
		GLGuiRender*			m_pkGuiRenderer;			///< Gui - Zeb
		ZGuiResourceManager*	m_pkGuiMan;					///< Gui - Zeb
		ZGui*						m_pkGui;						///< Gui - Zeb
		ZFIni*					m_pkIni;						///< Ini Files - Zeb
		ZSSResourceDB*			m_pkResourceDB;			///< Resouce Handler.
		ZSSScriptSystem*		m_pkScript;					///< script
		Application*			m_pkApp;						///< Application object.
		ZShaderSystem*			m_pkZShaderSystem;		///< zerofps shader system
		PSystemManager*		m_pkPSystemManager;		///< ParticleSystemManager - Zerom
		ZSSAStar*				m_pkAStar;					///< Path finding subsystem
		Tcs*						m_pkTcs;						///< Tiny Collission system, rigid body physics system
		ZShadow*					m_pkZShadow;				///< old stencil shadow system
		ZSSRenderEngine*		m_pkZSSRenderEngine;		///< Render engine, handles all ingame rendering

		int		m_iState;									//	curent game state see enum enginestates
		float		m_fFps;										//	curent FPS
		float		m_fAvrageFps;
		float		m_fFrameTime;								//	frametime in MS

		bool		m_bServerMode;
		bool		m_bClientMode;
		bool		m_bEditMode;
		bool		m_bGuiMode, m_bGuiTakeControl;
		ConVar	m_kbRunWorldSim;

		bool		m_bAlwaysWork;								///< Paramater that should be set if engine should work (ie. Update) no matter if the application is minimized or not.
		float		m_fMadLod;									//	If not 0 then force this LOD % on every mad.
		int		m_iNumOfMadRender;
		
		int		m_iOcculedObjects;
		int		m_iNotOcculedObjects;
		ConVar	m_kbOcculusionCulling;
		
		ConVar	m_kAI_ShowInfo;
		ConVar	m_kAI_Distance;
		
		InputHandle*	m_pkInputHandle;
		InputHandle*	m_pkGuiInputHandle;

		vector<ZFClient>		m_kClient;

		
		//basic engine funktions
		ZSSZeroFps(void);
		~ZSSZeroFps();
		bool Init(int iNrOfArgs, char** paArgs);
		void MainLoop(void);
		void QuitEngine();
		
		void StartServer(bool bClient = true,bool bNetwork = true,int iPort = 4242,string strServerName="Unkown");
		void StartClient(string strLogin,string strPassword,string strServerIP);
		void StopAll();
		
		//system 
		bool StartUp();
		bool ShutDown();
		bool IsValid();

		//dont know what these are doing here anyway =P
		void PrintToClient(int iConnectionID, const char* szMsg);
		void AddHMProperty(Entity* pkEntity,Vector3 kZoneSize);
		
		
		// gets
		const ZFVersion& GetVersion()	const			{	return m_kVersion;								}
		bool GetSyncNetwork() const					{	return m_kbSyncNetwork.GetBool();			}
		float GetNetworkFps() const					{	return m_kfNetworkUpdateFps.GetFloat();	}
		float GetSystemFps()	const						{	return m_kfSystemUpdateFps.GetFloat();		}				
		float GetRealTime() const						{	return float((SDL_GetTicks()/1000.0));		}
		float GetFrameTime()	const						{	return float((m_fFrameTime/1000.0));		}
		float GetLastGameUpdateTime() const			{	return m_fSystemUpdateTime;					}
		float GetEngineTime() const					{	return m_fEngineTime;							}
		float GetSystemUpdateFpsDelta() const 		{	return m_fSystemUpdateFpsDelta;				}		
		unsigned int	GetCurrentFrame() const		{	return m_iCurrentFrame;							}
		const int GetConnectionSpeed() const		{  return m_kiConnectionSpeed.GetFloat();		}
				
		bool GetMinimized() const						{	return m_bMinimized;								}
		bool GetRenderOn() const						{	return m_kbRenderOn.GetFloat();				}
		bool GetDebugGraph() const 					{	return m_kbDebugGraph.GetFloat();			}
		bool GetDrawAxesIcon() const 					{	return m_kAxisIcon.GetBool();					}		
		bool GetShadowMap() const						{	return m_kbShadowMap.GetBool();				}
		bool GetShadowMapRealtime() const			{	return m_kbShadowMapRealtime.GetBool();	}
		int  GetShadowMapMode() const					{	return m_kiShadowMapMode.GetInt();			}
		int  GetShadowMapQuality() const				{	return m_kiShadowMapQuality.GetInt();		}		
		bool GetVegetation() const						{	return m_kVegetation.GetBool();				}
		float GetVegetationDistance() const			{	return m_kVegitationDistance.GetFloat();	}
		float GetViewDistance() const					{	return m_kViewDistance.GetFloat();			}
		bool GetOcculusionCulling() const			{	return m_kbOcculusionCulling.GetBool();	}		
					
		//sets
		void	SetDebugGraph(bool bDebug)				{	m_kbDebugGraph.SetBool( bDebug );			}		
		void  SetSyncNetwork(bool bSync)				{	m_kbSyncNetwork.SetBool(bSync);				}
		void  SetSystemFps(int iFps) 					{	m_kfSystemUpdateFps.SetFloat(iFps);			}
		void  SetNetworkFps(int iFps)					{	m_kfNetworkUpdateFps.SetFloat(iFps);		}		

		//render targets
		void AddRenderCamera(Camera* pkCamera);
		void RemoveRenderCamera(Camera* pkCamera);
		void ClearRenderCameras();
		void Draw_RenderCameras();
		Camera* GetRenderCamera(const string& strName) const;
		Camera* GetCam() const							{	return m_pkCamera;}		//get current render camera, can be NULL
		
		//toggle functions		
		void ToggleFullScreen(void);
		void ToggleGui(void);
				
		//devpage
		void DevPrintf(const char* szName, const char *fmt, ...);
		bool DevPrintPageVisible(const char* szName);
		void SetDevPageVisible(const char* szName,bool bVisible);
		void DevPrint_Show(bool bVisible);

		// Called by network.
		bool	PreConnect(IPaddress kRemoteIp, char* szLogin, char* szPass, bool bIsEditor, string& strWhy);
		int	Connect(int iConnectionID, char* szLogin, char* szPass, bool bIsEditor);
		void	Disconnect(int iConnectionID);
		int	GetClientObjectID();
		int	GetConnectionID() 			{	return m_iServerConnection;	}		///< Return our Connection Num on the Server.
		int	GetMaxPlayers() 				{	return m_kiMaxPlayers.GetInt();			}
		
		vector<Entity*>	GetClientEntitys();

		void HandleEditCommand(NetPacket* pkNetPacket);						
		void RouteEditCommand(NetPacket* pkNetPacket);						
		void SetEnableLogin(bool bLogin) { m_bClientLoginState = bLogin; }
	
		void StartProfile(int iGaa1, int iGaa2, int iGaa3);
		void EndProfile(int iGaa1, int iGaa2, int iGaa3);
		

		friend class ZSSNetWork;
		friend class Camera;
};





#endif




