
#ifndef _ZGUITEST_H_
#define _ZGUITEST_H_

#ifndef _DONT_MAIN
	#define _MAINAPPLICATION_
	#define _DONT_MAIN
#endif

#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <list>
#include "../zerofpsv2/engine/application.h"
#include "../zerofpsv2/gui/zgui.h"

class ZGuiTest :public Application, public ZGuiApp 
{
	public:
		ZGuiTest(char* aName,int iWidth,int iHeight,int iDepth);

		void OnInit(void);
		void OnIdle(void);
		void OnHud(void);
		void OnSystem() {};
		void OnServerStart(void) {};
		void OnClientStart(void) {};

		bool StartUp() {return true;};
		bool ShutDown() {return true;};
		bool IsValid() {return true;};

		bool OnPreConnect(IPaddress kRemoteIp, char* szLogin, char* szPass) { return true; }
		void OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass) {};
		void OnServerClientPart(ZFClient* pkClient,int iConID) {};

	private:
		void CreateUI();

		Camera*				m_pkCamera;
};

#endif // #ifndef _ZGUITEST_H_