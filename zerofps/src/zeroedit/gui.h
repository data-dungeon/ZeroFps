// Gui.h: interface for the Gui class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUI_H__6ADF8B0C_5888_4E90_95E1_C2E517786ECA__INCLUDED_)
#define AFX_GUI_H__6ADF8B0C_5888_4E90_95E1_C2E517786ECA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//class ZeroEdit;

#define _DONT_MAIN

#include "zeroedit.h"

typedef bool (*ZGuiCallBack)(ZGuiWnd*, unsigned int, int, void*);

class Gui
{
private:
	ZGuiTextbox* CreateTextbox(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h, bool bMulitLine=false);
	ZGuiCombobox* CreateCombobox(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h, bool bMenu);
	ZGuiListbox* CreateListbox(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h);
	ZGuiButton* CreateButton(ZGuiWnd* pkParent, int iID, int pos_x, int pos_y, int w, int h, char* pkName);
	ZGuiLabel* CreateLabel(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h, char* strText);
	
	ZeroEdit* m_pkEdit;
	map<string, ZGuiSkin*> m_kSkinMap;
	map<string, int> m_kTextureMap;
	ZGuiSkin* GetSkin(char* strName);
	int GetTexture(char* strName);
	string m_szSearchBoxPath;
	bool m_bMenuActive;
	ZGuiCallBack m_pkWndProc;
	int m_iScreenCX;
	int m_iScreenCY;
	
	void AddItemToList(ZGuiWnd *pkWnd, bool bCombobox, const char *item, int id);
	void AddItemsToList(ZGuiWnd* pkWnd, bool bCombobox, char** items, int Number);
	ZGuiWnd* CreatePropertyDialog(int x, int y, int Widht, int Height);
	ZGuiWnd* CreateFileOpenDlgbox(int x, int y, int Widht, int Height);
	bool FillPathList(ZGuiListbox* pkListbox, string pkDir);
	bool CreateWindows(/*ZGuiCallBack*/);
	bool InitSkins();

public:
	
	bool ZGWinProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams );
	bool IsMenuActive() { return m_bMenuActive; }

	Gui(ZeroEdit* pkEdit, ZGuiCallBack cb);
	virtual ~Gui();



};

#endif // !defined(AFX_GUI_H__6ADF8B0C_5888_4E90_95E1_C2E517786ECA__INCLUDED_)
