// zgresedit.h: interface for the ZGResEdit class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _ZGRESEDIT_H_
#define _ZGRESEDIT_H_

#ifndef _DONT_MAIN
	#define _MAINAPPLICATION_
	#define _DONT_MAIN
#endif

#include <iostream>
#include <cmath>
//#include <GL/glut.h>
#include <list>
#include "../zerofpsv2/engine/application.h"
#include "../zerofpsv2/gui/zgui.h"
#include "serialization.h"

class Scene;

const int RADIO_GROUP_COUNTER_START = 100;

class ZGResEdit : public Application, public ZGuiApp 
{
	public:
		
		void OnClickListbox(int iListBoxID, int iListboxIndex, ZGuiWnd* pkMain);
		void OnClickTreeItem(char *szTreeBox, char *szParentNodeText, char *szClickNodeText, bool bHaveChilds);
		void OnSelectCB(int ListBoxID, int iItemIndex, ZGuiWnd *pkMain);
		void OnKeyDown(int iKey);
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

		ZGResEdit(char* aName,int iWidth,int iHeight,int iDepth);

	private:
		void UpdateSkinList(ZGuiWnd* pkFocusWnd);
		void UpdatePropertyData();
		void ExecuteCommand();
		void MoveWndToTop(ZGuiWnd *pkWnd);
		void UpdateViewWnd();
		void DrawSelectionRect(ZGuiWnd* pkWnd);
		void MoveWnd(int x, int y);
		void ResizeWnd(int x, int y);
		void EnableWnds(bool bEnable);
		void ClipLine(Line kLine, const vector<Rect> rects, vector<Line>& out);
		void OpenDefPropWnd(string strWndType);
		void AddStandardElements(ZGuiWnd* pkWnd);
		bool ClickInsideWnd(ZGuiWnd* pkWnd, int x, int y);
		void OnSelectWnd(ZGuiWnd* pkWnd);
		void DeleteWnd(ZGuiWnd* pkWnd);
		
		void Resize(ZGuiWnd* pkWnd, int w, int h);
		void SetPos(ZGuiWnd* pkWnd, int x, int y);
		
		ZGuiWnd* GetWndFromPoint(int x, int y);
		void OnMouseClick(bool bReleased, int x, int y);
		void OnReleaseButton(int mx, int my);
		
		Camera*	m_pkCamera;
		Scene*	m_pkScene;

		ZGuiWnd* m_pkFocusWnd;
		ZGuiWnd* m_pkResizeWnd;
		ZGuiWnd* m_pkMainWnd;
		ZGuiWnd* m_pkMoveWnd;

		bool m_bHaveAskedForDefProp;
		int m_iRadiogroupCounter;
		bool m_bDisableAlphatest;

		int m_iXPosBeforeMove, m_iYPosBeforeMove;

		enum ResizeType { RightSide, BottomSide, LeftSide, TopSide, None } m_eCurrentResizeType;
		enum EditMode { MOVE, RESIZE, VIEW } m_eEditMode;

		int m_iXPos, m_iYPos;
		int m_iWidth, m_iHeight;
		bool m_bSaveFile, m_bOverwriteWarning;

		bool m_bLeftButtonPressed;
		int m_iHighestZValue;

		Point m_kSelStart, m_kClickPos;

		struct SORT_MOST_PARENTS : public binary_function<pair<ZGuiWnd*, int>, pair<ZGuiWnd*, int>, bool> {
			bool operator()(pair<ZGuiWnd*, int> x, pair<ZGuiWnd*, int> y) { 
				return (x.second > y.second);
			};
		} SortMostParents;

		struct SORT_ZVALUE : public binary_function<pair<ZGuiWnd*, int>, pair<ZGuiWnd*, int>, bool> {
			bool operator()(pair<ZGuiWnd*, int> x, pair<ZGuiWnd*, int> y) {
				return x.first->m_iZValue > y.first->m_iZValue; 
			};
		} SortZValue;
};

#endif // #ifndef _ZGRESEDIT_H_
