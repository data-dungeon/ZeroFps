// dark_metropolis.h

#ifndef _DARK_METROPOLIS_H_
#define _DARK_METROPOLIS_H_

#include <iostream>
#include <cmath>
#include <list>

#include "../zerofpsv2/engine/application.h"
#include "../zerofpsv2/gui/zgui.h"

bool GUIPROC(ZGuiWnd* win, unsigned int msg, int numparms, void *params );

/**	\brief	Daz DaaaaarkMetropolis
		\ingroup DarkMetropolis
*/

class DarkMetropolis : public Application, public ZGuiApp 
{
	private:
		Camera*	m_pkCamera;
		void Input();

	public:
		DarkMetropolis(char* aName,int iWidth,int iHeight,int iDepth);
		
		// Application, network and system stuff
		void OnInit();
		void OnIdle();
		void OnHud();
		void OnSystem();
		bool OnPreConnect(IPaddress kRemoteIp, char* szLogin, char* szPass) { return true; }
		void OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass);
		void OnServerClientPart(ZFClient* pkClient,int iConID);
		void OnServerStart();
		void OnClientStart();

		bool StartUp();
		bool ShutDown();
		bool IsValid();

		void RenderInterface() { }

		// Gui
		void GUI_OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd *pkMainWnd);
		void GUI_OnClick(int x, int y, bool bMouseDown, bool bLeftButton, ZGuiWnd *pkMain);
		void GUI_OnDoubleClick(int x, int y, bool bLeftButton, ZGuiWnd *pkMain);
		void GUI_OnMouseMove(int x, int y, bool bMouseDown, ZGuiWnd *pkMain);
		void GUI_OnScroll(int iID, int iPos, ZGuiWnd *pkMain);
		void GUI_OnSelectCB(int ListBoxID, int iItemIndex, ZGuiWnd *pkMain);
		void GUI_OnKeyPress(int iKey, ZGuiWnd *pkWnd);
};

extern DarkMetropolis g_kDM;

#endif