#ifndef _DLGBOX_H
#define _DLGBOX_H

#include "../zerofps/gui/zgui.h"
#include "../zerofps/gui/zguiresourcemanager.h"

#pragma warning( disable : 4786 )

class Input;

#include <vector>
#include "common_x.h"

class COMMON_API DlgBox  
{
public:
	static int GetNumVisibleDialogs();
	static void CloseFocusDlg();
	const ZGuiWnd* GetWnd();
	void KillFocus();
	void SetPos(int x, int y);
	void GetPos(int& x, int& y);
	bool IsOpen();
	int Height();
	int Width();
	
	typedef bool (*ZGuiWndProc)(ZGuiWnd*, unsigned int, int, void*);
	
	DlgBox(ZGui* pkGui, Input* pkInput, ZGuiWndProc oMainWndProc);
	virtual ~DlgBox();

	virtual bool Create(int x, int y, char* szResourceFile, char* szDlgName)=0;
	virtual bool DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
		int iNumberOfParams,void *pkParams ) = 0;

	bool Open(int x=-1, int y=-1);
	bool Close(bool bSave);

	static int prev_x;
	static int prev_y;

protected:

	virtual bool OnOpen(int x=-1, int y=-1)=0;
	virtual bool OnClose(bool bSave)=0;
	void CenterCursor();

	ZGuiResourceManager* m_pkGuiMan;
	ZGui* m_pkGui;
	ZGuiWnd* m_pkDlgBox;
	ZGuiWndProc m_oMainWndProc;
	Input* m_pkInput;
	static std::vector<DlgBox*> s_kTopDlgQueue;

	
};

#endif // #ifndef _DLGBOX_H
