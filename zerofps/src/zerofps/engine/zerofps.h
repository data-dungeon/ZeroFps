#ifndef _ZEROFPS_H_
#define _ZEROFPS_H_

#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include <string>
#include <vector>
#include <SDL/SDL.h>
#include "../ogl/zfpsgl.h"
#include <iostream>
#include <cstdlib>
#include "engine.pkg"
#include "propertyfactory.h"
#include <cstdio>
#include <cstdarg>
#include "network.h"

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
		SDL_Surface* m_pkScreen;		
		void HandleArgs(int iNrOfArgs, char** paArgs);

		float m_fLastFrameTime;

		int m_iWidth,m_iHeight,m_iDepth;
		int m_iFullScreen;
		
		Camera *m_pkTempCamera;
		Camera *m_pkCamera;
		Camera *m_pkConsoleCamera;
		Camera *m_pkDefaultCamera;
	
		vector<string>	AppArguments;



		enum FuncId_e
			{
			FID_SETDISPLAY,
			FID_QUIT,
			FID_SLIST,			// List all servers
			FID_CONNECT,		// Connect to server.
			FID_SERVER,			// Start a Server.
			};

		void RunCommand(int cmdid, const CmdArgument* kCommand);
		

	public:
		Application* m_pkApp;						//application
		CmdSystem* m_pkCmd;							//realtime variable handler
		AudioManager* m_pkAudioMan;			//audio manager (sfx,music)		
		TextureManager* m_pkTexMan;			//texture manager
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


		vector<Core>		akCoreModells;
				
		
		int LoadMAD(const char* filename);
		void ClearMAD(void);
		int GetMADIndex(const char* filename);
		Core* GetMADPtr(const char* filename);

		int m_iState;										//curent game state see enum enginestates
		int m_iFps;											//curent FPS
		float m_fFrameTime;							//frametime in MS
		
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
		
		inline float GetTicks(void) {return (SDL_GetTicks()/1000.0);};
		inline float GetFrameTime() {return (m_fFrameTime/1000.0);};
		inline float GetGameTime(void) {return (SDL_GetTicks()/1000.0);};
		
	
		void SetCamera(Camera* pkCamera);
	
		inline Camera *GetCam() {return m_pkCamera;};		
		inline Camera *GetDefaultCam() {return m_pkDefaultCamera;};		
		inline Camera *GetConsoleCam() {return m_pkConsoleCamera;};				

		PropertyFactory	m_kPropertyFactory;

		int	NumberOfArgs(void);	// Return num of arg to app.
		string GetArg(int iArgIndex);
	
		vector<string>	akDevString;
		void DrawDevStrings();
		void DevPrintf(const char *fmt, ...);

		void HandleNetworkPacket(NetPacket* pkNetPacket);

};





#endif




