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
class FileOpenDlg;

class ZGResEdit :public Application  
{
public:
	
	bool WinProc(ZGuiWnd* pkWnd,unsigned int uiMessage,
				 int iNumberOfParams,void *pkParams ) ;
	void OnServerStart() {}
	void OnClientStart() {}
	void OnHud();
	void OnIdle();
	void OnInit();

	ZGResEdit(char* aName,int iWidth,int iHeight,int iDepth);
	virtual ~ZGResEdit();

	typedef bool (*ZGuiWndProc)(const ZGuiWnd*,const unsigned int,
		const int,const void*);
	typedef list<ZGuiWnd*>::iterator WINit;

	SkinBox* m_pkSkinBox;
	PropertyBox* m_pkPropertyBox;
	ControlBox* m_pkControlBox;
	FileOpenDlg* m_pkFileOpenDlg;

	ZGuiWnd* m_pkRectWnd;

private:

	bool m_bResizeRectWnd;

	void SaveFile();
	void EnableClickWnd();
	void DisableClickWnd();

	enum MouseState
	{
		RESIZING_HORZ,
		RESIZING_VERT,
		RESIZING_BOTH,
		MOVING,
		IDLE,
	} m_kMouseState;

	bool IsGuiWnd(ZGuiWnd* pkWnd) const;
	void MoveWnd(ZGuiWnd* pkWnd,int x,int y) const;
	void ResizeWnd(ZGuiWnd* pkWnd,int x,int y) const;
	void SetCursor(const char* szFileNameBitmap,
		const char* szFileNameBitmapAlpha) const;
	bool Create();

	Camera* m_pkCamera;
	GuiBuilder* m_pkGuiBuilder;
	
	int m_iClickPosX,m_iClickPosY;
	int m_pnCursorRangeDiffX,m_pnCursorRangeDiffY;
	int m_iResizewnd_old_x,m_iResizewnd_old_y;
	bool m_bUseGrid;
	int m_iGridPrec;
	vector<ZGuiWnd*> m_pkMoveWnds;

	ZFFile* m_pkFile;
	ZFIni* m_pkINI;
};

#endif // #ifndef _ZGRESEDIT_H
