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
		int m_iState;
		void HandleArgs(int iNrOfArgs, char** paArgs);

	public:
		Application* m_pkApp;		
		CmdSystem* m_pkCmd;
		TextureManager* m_pkTexMan;
		Input* m_pkInput;
		Primitives* m_pkPrims;
		Console* m_pkConsole;	
	
		ZeroFps(void);
		
		void SetApp(void);
		void Init(int iNrOfArgs, char** paArgs);	
		void MainLoop(void);
		
		void InitDisplay(int iWidth,int iHeight,int iDepth);		
		void Swap(void);
		
		
};





#endif




