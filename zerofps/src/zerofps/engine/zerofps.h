#ifndef _ZEROFPS_H_
#define _ZEROFPS_H_

#include <iostream>
#include <cstdlib>
#include <SDL/SDL.h>
#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include "engine.pkg"
#include "../ogl/zfpsgl.h"
#include "propertyfactory.h"
#include <string>
#include <vector>


using namespace std;

class Application;

enum enginestates 
{
	state_normal,
	state_exit,
	state_console,
	state_pause
};

		
	extern PFNGLFOGCOORDFEXTPROC glFogCoordfEXT;		

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
		
		inline unsigned int GetTicks(void) {return SDL_GetTicks();};
		inline float GetFrameTime() {return m_fFrameTime;};
		
	
		void SetCamera(Camera* pkCamera);
	
		inline Camera *GetCam() {return m_pkCamera;};		
		inline Camera *GetDefaultCam() {return m_pkDefaultCamera;};		
		inline Camera *GetConsoleCam() {return m_pkConsoleCamera;};				

		PropertyFactory	m_kPropertyFactory;

		int	NumberOfArgs(void);	// Return num of arg to app.
		string GetArg(int iArgIndex);
};





#endif




