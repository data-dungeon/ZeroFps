#ifndef _ENGINE_ZEROFPS_H_
#define _ENGINE_ZEROFPS_H_

#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <SDL/SDL.h>
#include "engine.pkg"
#include "../ogl/zfpsgl.h"
#include "network.h"
#include "zfresourcedb.h"
#include "mad_core.h"
#include "../physics_engine/physics_engine.pkg"
#include "fh.h"


#define ZF_VERSION_NUM		"ZF 0.0"


using namespace std;

class Basic;
class Render;
class ZGui;
class ZGuiResourceManager;
class GLGuiRender;
class ZFScript;
class Application;
class NetPacket;

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

#define	ZFGP_ENDOFPACKET	128

class DevStringPage
{
private:
public:
	bool			m_bVisible;
	string			m_kName;
	vector<string>	m_akDevString;
};

/* Information about a connectet client*/
class ENGINE_API ZFClient
{
public:
	float			m_fConnectTime;	
	string		m_strName;		// Name of player.
	Object*		m_pkObject;		// Object used for client.
};

/// Main class for the ZeroFps engine. 
class ENGINE_API ZeroFps : public ZFObject {
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
		
		SDL_Surface* m_pkScreen;		
		bool		m_bDevPagesVisible;
		
		float 		m_fLastFrameTime;
		float 		m_fAvrageFpsTime;
		int			m_iAvrageFrameCount;
		DebugGraph	m_kFpsGraph;

		

		int		m_iWidth,m_iHeight,m_iDepth;
		int		m_iFullScreen;
		bool  	m_bCapture;							// True if we should capture this frame

		float 	m_fSystemUpdateFps;
		float 	m_fSystemUpdateTime;
		float 	m_fGameTime;
		float 	m_fGameFrameTime;
		float		m_fEngineTime;						// Time since engine start.
		
		vector<string>	AppArguments;		
		
		Camera *m_pkCamera;
		Camera *m_pkConsoleCamera;
		string m_kCurentDir;

		void RunCommand(int cmdid, const CmdArgument* kCommand);
		void HandleArgs(int iNrOfArgs, char** paArgs);		
		void Run_EngineShell();
		void Run_Server();
		void Run_Client();
		void Draw_EngineShell();		
		void Update_System();

	public:
		/*
			All Engine Systems in ZeroFps. Listed in the same order they are created in
			ZeroFps();
		*/
		FileIo*					m_pkFile;					///< ?? file io ?? Vim
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
		AudioManager*			m_pkAudioMan;				///< audio manager (sfx,music)		
		ObjectManager*			m_pkObjectMan;				///< In Game Object System.
		SoundBufferManager*  m_pkSBM;						///< Sound
		OpenAlSystem*			m_pkOpenAlSystem;			///< Sound
		NetWork*					m_pkNetWork;				///< NetWork Interface.
		GLGuiRender*			m_pkGuiRenderer;			///< Gui - Zeb
		ZGuiResourceManager*	m_pkGuiMan;					///< Gui - Zeb
		ZGui*						m_pkGui;						///< Gui - Zeb
		ZFIni*					m_pkIni;						///< Ini Files - Zeb
		LevelManager*			m_pkLevelMan;				///< 
		PhysicsEngine*			m_pkPhysEngine;			///< 
		ZFResourceDB*			m_pkResourceDB;			///< Resouce Handler.
		ZFScript*				m_pkScript;					///< script
		Physics_Engine*		m_pkPhysics_Engine;		///< PhysicsEngine
		Application*			m_pkApp;						///< Application object.
			

		int		m_iState;									//	curent game state see enum enginestates
		float		m_fFps;										//	curent FPS
		float		m_fAvrageFps;
		float		m_fFrameTime;									//	frametime in MS
		
		bool		m_bServerMode;
		bool		m_bClientMode;
		//bool	m_bDrawDevList;
		bool		m_bGuiMode, m_bGuiTakeControl;
		bool		m_bRunWorldSim;

		
		int		m_iMadDraw;									//	Flags for what part's of mad's that should be draw.
		float		m_fMadLod;										//	If not 0 then force this LOD % on every mad.

		vector<ZFClient>		m_kClient;

		ZeroFps(void);		
		~ZeroFps();		
		void SetApp(void);
		bool Init(int iNrOfArgs, char** paArgs);	
		void MainLoop(void);		
		void InitDisplay(int iWidth,int iHeight,int iDepth);		
		void SetDisplay(int iWidth,int iHeight,int iDepth);
		void SetDisplay();
		void Swap(void);											//	swap gl buffers
		
		void ToggleFullScreen(void);
		void ToggleGui(void);
		
		inline float GetTicks() {return float((SDL_GetTicks()/1000.0));};
		inline float GetFrameTime() {return float((m_fFrameTime/1000.0));};
		inline float GetGameTime() {return m_fGameTime;};
		inline float GetGameFrameTime() {return m_fGameFrameTime;};
		inline float GetLastGameUpdateTime(){return m_fSystemUpdateTime;};
		inline float GetEngineTime() { return m_fEngineTime; }

		void SetCamera(Camera* pkCamera);	
		void UpdateCamera();
		inline Camera *GetCam() {return m_pkCamera;};		
		int	NumberOfArgs(void);										// Return num of arg to app.
		string GetArg(int iArgIndex);
	
		vector<DevStringPage>	m_DevStringPage;					
		DevStringPage*	DevPrint_FindPage(const char* szName);		
		void DrawDevStrings();
		void DevPrintf(const char* szName, const char *fmt, ...);
		void DevPrint_Show(bool bVisible);

		void HandleNetworkPacket(NetPacket* pkNetPacket);
		
		void RegisterPropertys();
		void QuitEngine();

		// Statistik Data
		int	m_iNumOfMadRender;

		void GetEngineCredits(vector<string>& kCreditsStrings);
		 
		vector<Camera*>	m_kRenderTarget;
		
		void SetRenderTarget(Camera* pkCamera);
		void RemoveRenderTarget(Camera* pkCamera);
		
		int GetWidth(){return m_iWidth;};
		int GetHeight(){return m_iHeight;};		
		int GetDepth(){return m_iDepth;};		


		// Called by network.
		/* PreConnect is called when a connection is about to be made. It's after a Connect message
			for a server Node that accepts connections. Never called on a client node. Return false
			to deny connection. Put reason if any into szWhy256. */
		bool PreConnect(IPaddress kRemoteIp, char* szWhy256);
		/* Connect is called when a connection have been made. It is called after PreConnect on server if
			PreConnect returns true. It is called on Clients when they recive connect_yes from server.*/
		void Connect(int iConnectionID);
		/*	Called when a connection is closed down by the other side. */
		void Disconnect(int iConnectionID);


		// ZeroRTS - 
		/* Returns ID of of object Client use to send data to server. Returns -1 if object is unknown at the moment.
			Keep asking :).*/ 
		int	m_iRTSClientObject;
		int GetClientObjectID();
};





#endif




