// Gui.h: interface for the Gui class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUI_H__6ADF8B0C_5888_4E90_95E1_C2E517786ECA__INCLUDED_)
#define AFX_GUI_H__6ADF8B0C_5888_4E90_95E1_C2E517786ECA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//class ZeroEdit;

#include "zeroedit.h"

class Gui
{
public:

	typedef bool (*ZGuiCallBack)(ZGuiWnd*, unsigned int, int, void*);

	Gui(ZeroEdit* pkEdit, ZGuiCallBack cb);
	virtual ~Gui();

	bool InitSkins();
	int  CreateFilePathBox(int iMainWindow, int iListBox, int x, int y, int Widht, int Height);
	bool FillPathList(ZGuiListbox* pkListbox, string pkDir);
	void ToogleMenu();
	bool CreateWindows(ZGuiCallBack);
	bool ZGWinProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams );
	bool IsMenuActive() { return m_bMenuActive; }

private:
	ZeroEdit* m_pkEdit;
	map<string, ZGuiSkin*> m_kSkinMap;
	ZGuiSkin* GetSkin(char* strName);
	string m_szSearchBoxPath;
	bool m_bMenuActive;

};

#endif // !defined(AFX_GUI_H__6ADF8B0C_5888_4E90_95E1_C2E517786ECA__INCLUDED_)
