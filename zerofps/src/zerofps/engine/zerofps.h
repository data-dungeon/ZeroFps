#ifndef _ZEROFPS_H_
#define _ZEROFPS_H_

#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include "engine.pkg"
#include <string>
#include <vector>
#include <SDL/SDL.h>
#include "../ogl/zfpsgl.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include "network.h"
//#include "propertyfactory.h"
//#include "zgui/zgui.h"

using namespace std;

class Application;
class NetPacket;

enum enginestates 
{
	state_normal,
	state_exit,
	state_console,
	state_pause
};

		
//	extern PFNGLFOGCOORDFEXTPROC glFogCoordfEXT;		

#define	ZFGP_OBJECTSTATE	1
#define	ZFGP_CLIENTSTATE	2
#define	ZFGP_CLIENTCMD		3
#define	ZFGP_PRINT			4
#define	ZFGP_ENDOFPACKET	128

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
		};
		
		SDL_Surface* m_pkScreen;		

		float m_fLastFrameTime;
		int m_iWidth,m_iHeight,m_iDepth;
		int m_iFullScreen;
		
		vector<string>	AppArguments;		
		
		Camera *m_pkTempCamera;
		Camera *m_pkCamera;
		Camera *m_pkConsoleCamera;
		Camera *m_pkDefaultCamera;
	
		string m_kCurentDir;

		void RunCommand(int cmdid, const CmdArgument* kCommand);
		void HandleArgs(int iNrOfArgs, char** paArgs);		

	public:
		Application* m_pkApp;						//application
		CmdSystem* m_pkCmd;							//realtime variable handler
		ZFBasicFS* m_pkBasicFS;					//basic filesystem funktions
		AudioManager* m_pkAudioMan;			//audio manager (sfx,music)		
		TextureManager* m_pkTexMan;			//texture manager
		PropertyFactory* m_pkPropertyFactory;		
		Input* m_pkInput;								//keyboard mouse input
		Render* m_pkRender;					//graphic primitives
		Console* m_pkConsole;						//console handler
		FileIo* m_pkFile;								//file io
		Light* m_pkLight;
		ObjectManager* m_pkObjectMan;
		CollisionManager*  m_pkCollisionMan;
		SoundBufferManager* m_pkSBM;		
		OpenAlSystem* m_pkOpenAlSystem;
		NetWork* m_pkNetWork;
		Frustum* m_pkFrustum;
		ZGui* m_pkGui;
		GLGuiRender* m_pkGuiRenderer;
		ZFIni* m_pkIni;
		LevelManager* m_pkLevelMan;

		vector<Core>		akCoreModells;
				
		
		int LoadMAD(const char* filename);
		void ClearMAD(void);
		int GetMADIndex(const char* filename);
		Core* GetMADPtr(const char* filename);

		int m_iState;										//curent game state see enum enginestates
		int m_iFps;											//curent FPS
		float m_fFrameTime;							//frametime in MS
		
		bool m_bServerMode;
		bool m_bClientMode;
		bool m_bConsoleMode;
		
		
		ZeroFps(void);		
		~ZeroFps();		
		void SetApp(void);
		void Init(int iNrOfArgs, char** paArgs);	
		void MainLoop(void);		
		void InitDisplay(int iWidth,int iHeight,int iDepth);		
		void SetDisplay(int iWidth,int iHeight,int iDepth);
		void SetDisplay();
		void Swap(void);								//swap gl buffers
		
		void ToggleFullScreen(void);
		
		inline float GetTicks(void) {return float((SDL_GetTicks()/1000.0));};
		inline float GetFrameTime() {return float((m_fFrameTime/1000.0));};
		inline float GetGameTime(void) {return float((SDL_GetTicks()/1000.0));};
		
		void SetCamera(Camera* pkCamera);
	
		inline Camera *GetCam() {return m_pkCamera;};		
		inline Camera *GetDefaultCam() {return m_pkDefaultCamera;};		
		inline Camera *GetConsoleCam() {return m_pkConsoleCamera;};				

		int	NumberOfArgs(void);	// Return num of arg to app.
		string GetArg(int iArgIndex);
	
		vector<string>	akDevString;
		void DrawDevStrings();
		void DevPrintf(const char *fmt, ...);

		void HandleNetworkPacket(NetPacket* pkNetPacket);
		
		void RegisterPropertys();
		void QuitEngine();

};





#endif




