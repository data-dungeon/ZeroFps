// dark_metropolis.h

#ifndef _DARK_METROPOLIS_H_
#define _DARK_METROPOLIS_H_

#include <iostream>
#include <cmath>
#include <list>

bool GUIPROC(ZGuiWnd* win, unsigned int msg, int numparms, void *params );

/**	\brief	Daz DaaaaarkMetropolis
		\ingroup DarkMetropolis
*/

class DarkMetropolis : public Application, public ZGuiApp 
{
	private:
		Camera*	m_pkCamera;

	public:
		DarkMetropolis(char* aName,int iWidth,int iHeight,int iDepth);
		
		// Application
		void OnInit();
		void OnIdle();
		void OnHud();
		void OnSystem();

		// System
		bool StartUp();
		bool ShutDown();
		bool IsValid();

		bool OnPreConnect(IPaddress kRemoteIp, char* szLogin, char* szPass) { return true; }
		void OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass);
		void OnServerClientPart(ZFClient* pkClient,int iConID);

		void OnServerStart();
		void OnClientStart();
		void RenderInterface() { }

		// Gui
		void OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd *pkMainWnd);
		void OnClick(int x, int y, bool bMouseDown, bool bLeftButton, ZGuiWnd *pkMain);
		void OnDoubleClick(int x, int y, bool bLeftButton, ZGuiWnd *pkMain);
		void OnMouseMove(int x, int y, bool bMouseDown, ZGuiWnd *pkMain);
		void OnScroll(int iID, int iPos, ZGuiWnd *pkMain);
		void OnSelectCB(int ListBoxID, int iItemIndex, ZGuiWnd *pkMain);
		void OnKeyPress(int iKey, ZGuiWnd *pkWnd);
};

extern DarkMetropolis g_kDM;

#endif