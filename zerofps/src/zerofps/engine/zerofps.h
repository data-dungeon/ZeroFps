#ifndef _ZEROFPS_H_
#define _ZEROFPS_H_

#include <iostream>
#include <cstdlib>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL.h>

#include "engine.pkg"
#include "../base/base.pkg"
#include "../render/render.pkg"

using namespace std;

class Application;

enum enginestates {
	state_normal,
	state_exit,
	state_console,
	state_pause
};

class ZeroFps {
	private:		
		SDL_Surface* m_pkScreen;		
		void HandleArgs(int iNrOfArgs, char** paArgs);

		float m_fLastFrameTime;

	public:
		Application* m_pkApp;						//application
		CmdSystem* m_pkCmd;							//realtime variable handler
		TextureManager* m_pkTexMan;			//texture manager
		ModellManager* m_pkModMan;			//texture manager		
		Input* m_pkInput;								//keyboard mouse input
		Primitives* m_pkPrims;					//graphic primitives
		Console* m_pkConsole;						//console handler
		FileIo* m_pkFile;								//file io
	
		int m_iState;										//curent game state see enum enginestates
		int m_iFps;											//curent FPS
		float m_fFrameTime;							//frametime in MS
			
		ZeroFps(void);
		
		void SetApp(void);
		void Init(int iNrOfArgs, char** paArgs);	
		void MainLoop(void);
		
		void InitDisplay(int iWidth,int iHeight,int iDepth);		
		void Swap(void);								//swap gl buffers
		
		inline unsigned int GetTicks(void) {return SDL_GetTicks();};
		
};





#endif




