
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
		void OnClickTreeItem(char *szTreeBox, char *szParentNodeText, char *szClickNodeText, bool bHaveChilds);
		void OnSelectCB(int ListBoxID, int iItemIndex, ZGuiWnd *pkMain);
		void OnMouseClick(bool bReleased, int x, int y);
		void OnKeyDown(int iKey, ZGuiWnd* pkWnd);
		void OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd *pkMainWnd);

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

		ZGuiTest(char* aName,int iWidth,int iHeight,int iDepth);

	private:
		void OnSelectWnd(ZGuiWnd* pkWnd);
		void DeleteWnd(ZGuiWnd* pkWnd);
		
		bool BuildFileTree(char* szTreeBoxName, char* szRootPath);

		
		void Resize(ZGuiWnd* pkWnd, int w, int h);
		void SetPos(ZGuiWnd* pkWnd, int x, int y);
		
		bool IsResWnd(ZGuiWnd* pkWnd);
		ZGuiWnd* GetWndFromPoint(int x, int y);
		void CreateUI();
		
		Camera*	m_pkCamera;

		ZGuiWnd* m_pkToolbar;
		ZGuiWnd* m_pkEditbar;

		ZGuiWnd* m_pkFocusWnd;
		ZGuiWnd* m_pkResizeWnd;
		ZGuiWnd* m_pkMainWnd;

		struct MOVE_INFO
		{
			ZGuiWnd* pkWnd;
			int m_iXPosBeforeMove, m_iYPosBeforeMove;
		};

		vector<MOVE_INFO> m_vkMoveWnds;

		enum ResizeType { RightSide, BottomSide, LeftSide, TopSide } m_eCurrentResizeType;
		enum EditMode { MOVE, RESIZE } m_eEditMode;

		int m_iXPos, m_iYPos;
		int m_iWidth, m_iHeight;

		int m_iXPosBeforeResize, m_iYPosBeforeResize;

		Point m_kSelStart;

		Point m_kClickPos;

			struct SORT_FILES : public binary_function<string, string, bool> {
				bool operator()(string x, string y) { 
					return (x.find(".") == string::npos);
				};
			} SortFiles;
};

#endif // #ifndef _ZGUITEST_H_