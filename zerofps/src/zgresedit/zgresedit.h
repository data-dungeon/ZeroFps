#ifndef _ZGRESEDIT_H
#define _ZGRESEDIT_H

#ifndef _DONT_MAIN
	#define _MAINAPPLICATION_
	#define _DONT_MAIN
#endif

#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "../zerofps/engine/application.h"
#include <cstdlib>

class GuiBuilder;
class PropertyBox;
class ControlBox;
class SkinBox;

class ZGResEdit :public Application  
{
public:
	
	bool WinProc( ZGuiWnd* pkWindow, unsigned int uiMessage, 
		int iNumberOfParams, void *pkParams ) ;
	void OnServerStart() {}
	void OnClientStart() {}
	void OnHud() {}
	void OnIdle();
	void OnInit();

	ZGResEdit(char* aName,int iWidth,int iHeight,int iDepth);
	virtual ~ZGResEdit();

	typedef bool (*ZGuiWndProc)(ZGuiWnd*, unsigned int, int, void*);
	typedef list<ZGuiWnd*>::iterator WINit;

private:
	//void CloseControlPropertyBox(bool bSave);
	//void OpenControlPropertyBox(ZGuiWnd* pkWnd, int x, int  y);
	bool IsGuiWnd(ZGuiWnd* pkWnd);

	enum MouseState
	{
		RESIZING_HORZ,
		RESIZING_VERT,
		RESIZING_BOTH,
		MOVING,
		IDLE,
	} m_kMouseState;

	void MoveWnd(ZGuiWnd* pkWnd, int x, int y);
	void ResizeWnd(ZGuiWnd* pkWnd, int x, int y);
	//ZGuiWnd* CreateControlPropertyBox(int x, int y, int w, int h);
	//ZGuiWnd* CreateControlBox(int x, int y, int w, int h);
	void SetCursor(char* szFileNameBitmap, char* szFileNameBitmapAlpha);
	bool Create();

	Camera* m_pkCamera;
	GuiBuilder* m_pkGuiBuilder;
	PropertyBox* m_pkPropertyBox;
	ControlBox* m_pkControlBox;
	SkinBox* m_pkSkinBox;
	
	int m_iClickPosX, m_iClickPosY;
	int m_pnCursorRangeDiffX, m_pnCursorRangeDiffY;
	int m_iResizewnd_old_x, m_iResizewnd_old_y;
};

#endif // #ifndef _ZGRESEDIT_H
