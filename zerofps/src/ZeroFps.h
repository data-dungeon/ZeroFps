#ifndef _ZEROFPS_H_
#define _ZEROFPS_H_

#include <iostream>
#include <cstdlib>
#include "Globals.h"
#include "Vector3.h"
#include "Application.h"
#include "CmdSystem.h"

class Application;


enum enginestates {
	state_normal,
	state_exit,
	state_console,
	state_pause
};




class ZeroFps {
	private:
		Application* m_pkApp;		
		CmdSystem* m_pkCmdSystem;
		
		int m_iState;
		int blub;		
		void HandleArgs(int iNrOfArgs, char** paArgs);

	public:
		void SetApp(void);
		void Init(int iNrOfArgs, char** paArgs);	
		void MainLoop(void);
		
		void Error(char* aText);
		
};





#endif




