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
#include "../physics_engine/physics_engine.h"
#include "fh.h"
#include "entity.h"
#include "camera.h"
#include "../basic/cmdsystem.h"
#include "../engine_systems/audio/zfaudiosystem.h"
#include "../basic/zfini.h"
#include "../engine_systems/physicsengine/physicsengine.h"
#include "../render/zshader.h"
#include "i_zerofps.h"
#include "../engine_systems/mad/mad_core.h"
#include "res_texture.h"
#include "../script/zfscript.h"
#include "../engine_systems/script_interfaces/si_std.h"
#include "astar.h"

#define ZF_VERSION_NUM		"ZF 0.01"


using namespace std;


class PhysicsEngine;
class Basic;
class Render;
class ZGui;
class ZGuiResourceManager;
class GLGuiRender;
class ZFScriptSystem;
class Application;
class NetPacket;
class PSystemManager;

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
#define	ZFGP_REQOWNOBJECT		6
#define	ZFGP_GIVEOWNOBJECT	7
#define	ZFGP_ZONELIST			8		// LIst of active zones sent to client.
#define  ZFGP_GETSTATICDATA	9
#define  ZFGP_STATICDATA		10

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
	string		m_strName;			// Name of player.
	Entity*		m_pkObject;			// Object used for client.
	set<int>	m_iActiveZones;		// Activated Zones.
};

/// Main class for the ZeroFps engine. 
class ENGINE_API ZeroFps : public I_ZeroFps {
	private:		
		enum FuncId_e
		{
			FID_SETDISPLAY,
			FID_QUIT,
			FID_SLIST,			// List all servers
			FID_CONNECT,		// Connect to server.
			FID_SERVER,			// Start a Server.
			FID_DIR,
			FID_CD,
			FID_LISTMAD,		// List all loaded mad's.
			FID_PRINTOBJECT,	// Print all objects to external console.

			FID_VERSION,		// Print Version info.
			FID_CREDITS,		// Print Credits to console.
			FID_ECHO,
		
			FID_GLDUMP,
		
			// DevStrings
			FID_DEV_SHOWPAGE,	
			FID_DEV_HIDEPAGE,

			FID_SCREENSHOOT,
		};
		

		bool		m_bDevPagesVisible;
		
		float 	m_fLastFrameTime;
		float 	m_fAvrageFpsTime;
		int		m_iAvrageFrameCount;
		
		float 	m_fSystemUpdateFps;
		float 	m_fSystemUpdateTime;
		float 	m_fGameTime;
		float 	m_fGameFrameTime;
		float		m_fEngineTime;						// Time since engine start.
		int		m_iRenderOn;
		
		int		m_iServerConnection;	// The Connection num we have on the server.		
		int		m_iRTSClientObject;
		
		int		m_iMaxPlayers;

		Camera *m_pkCamera;
		Camera *m_pkConsoleCamera;
		string m_kCurentDir;

		void RunCommand(int cmdid, const CmdArgument* kCommand);
		//void HandleArgs(int iNrOfArgs, char** paArgs);		
		void ConfigFileRun();
		void ConfigFileSave();
		void Run_EngineShell();
		void Run_Server();
		void Run_Client();
		void Draw_EngineShell();		
		void Update_System();

		

	public:
//		Object* CreateScriptObject(const char* szName);
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
		CmdSystem*				m_pkCmd;						///< Handles ZeroFps console variables
		EntityManager*			m_pkObjectMan;				///< In Game Object System.
		ZFAudioSystem*			m_pkAudioSystem;			///< Sound
		NetWork*					m_pkNetWork;				///< NetWork Interface.
		GLGuiRender*			m_pkGuiRenderer;			///< Gui - Zeb
		ZGuiResourceManager*	m_pkGuiMan;					///< Gui - Zeb
		ZGui*						m_pkGui;						///< Gui - Zeb
		ZFIni*					m_pkIni;						///< Ini Files - Zeb
		PhysicsEngine*			m_pkPhysEngine;			///< 
		ZFResourceDB*			m_pkResourceDB;			///< Resouce Handler.
		ZFScriptSystem*		m_pkScript;					///< script
		Physics_Engine*		m_pkPhysics_Engine;		///< PhysicsEngine
		Application*			m_pkApp;						///< Application object.
		ZShader*					m_pkZShader;				///< zerofps shader system
		OggMusic*				m_pkMusic;
		PSystemManager*		m_pkPSystemManager;		///< ParticleSystemManager - Zerom
		AStar*					m_pkAStar;

		int		m_iState;									//	curent game state see enum enginestates
		float		m_fFps;										//	curent FPS
		float		m_fAvrageFps;
		float		m_fFrameTime;								//	frametime in MS
		
		bool		m_bServerMode;
		bool		m_bClientMode;
		bool		m_bGuiMode, m_bGuiTakeControl;
		bool		m_bRunWorldSim;
		
		int		m_iMadDraw;									//	Flags for what part's of mad's that should be draw.
		float		m_fMadLod;									//	If not 0 then force this LOD % on every mad.

		vector<ZFClient>		m_kClient;					

		ZeroFps(void);		
		~ZeroFps();		
		void SetApp(void);
		bool Init(int iNrOfArgs, char** paArgs);	
		void MainLoop(void);		
		void Swap(void);											//	swap gl buffers
		
		void ToggleFullScreen(void);
		void ToggleGui(void);
		
		// Timer Functions.
		float GetTicks()					{	return float((SDL_GetTicks()/1000.0));};
		float GetFrameTime()				{	return float((m_fFrameTime/1000.0));};
		float GetGameTime()				{	return m_fGameTime;};
		float GetGameFrameTime()		{	return m_fGameFrameTime;};
		float GetLastGameUpdateTime()	{	return m_fSystemUpdateTime;};
		float GetEngineTime()			{	return m_fEngineTime; }

		void SetCamera(Camera* pkCamera);	
		void UpdateCamera();
		Camera *GetCam() {return m_pkCamera;};		
	
		vector<DevStringPage>	m_DevStringPage;					
		DevStringPage*	DevPrint_FindPage(const char* szName);		
		void DrawDevStrings();
		void DevPrintf(const char* szName, const char *fmt, ...);
		void DevPrint_Show(bool bVisible);

		void HandleNetworkPacket(NetPacket* pkNetPacket);
		
		void RegisterPropertys();
		void RegisterResources();
		
		void QuitEngine();

		// Statistik Data
		int	m_iNumOfMadRender;

		void GetEngineCredits(vector<string>& kCreditsStrings);
		 
		vector<Camera*>	m_kRenderTarget;
		
		void SetRenderTarget(Camera* pkCamera);
		void RemoveRenderTarget(Camera* pkCamera);
		
		int GetWidth(){return m_pkRender->GetWidth();}
		int GetHeight(){return m_pkRender->GetHeight();};		
		int GetDepth(){return m_pkRender->GetDepth();};		

		// Called by network.
		bool	PreConnect(IPaddress kRemoteIp, char* szWhy256);
		int	Connect(int iConnectionID);
		void	Disconnect(int iConnectionID);
		int	GetClientObjectID();
		int	GetConnectionID() {	return m_iServerConnection;	};		///< Return our Connection Num on the Server.
		
		bool StartUp();
		bool ShutDown();
		bool IsValid();


		friend class NetWork;
};





#endif




