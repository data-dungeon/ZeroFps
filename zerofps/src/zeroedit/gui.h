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

class FileOpenDlg;
typedef bool (*ZGuiCallBack)(ZGuiWnd*, unsigned int, int, void*);

class Gui
{
private:
	bool OnCloseEditProperty(bool bSave);
	void OnOpenEditProperty();
	bool Register(ZGuiFont* pkFont, char* strName);
	bool Register(ZGuiSkin* pkSkin, char* strName);
	bool Register(ZGuiWnd* pkWnd, char* strName);
	ZGuiTextbox* CreateTextbox(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h, bool bMulitLine=false);
	ZGuiCombobox* CreateCombobox(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h, bool bMenu);
	ZGuiListbox* CreateListbox(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h);
	ZGuiButton* CreateButton(ZGuiWnd* pkParent, int iID, int pos_x, int pos_y, int w, int h, char* pkName);
	ZGuiLabel* CreateLabel(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h, char* strText);
	ZGui* GetGUI() { return m_pkEdit->pkGui; }
	
	ZGuiWnd* m_pkMainWnd;
	ZeroEdit* m_pkEdit;
	
	map<string, ZGuiSkin*> m_kSkinMap;
	map<string, int> m_kTextureMap;
	ZGuiSkin* GetSkin(char* strName);
	int GetTexture(char* strName);
	bool m_bMenuActive;
	int m_iScreenCX;
	int m_iScreenCY;
	
	Rect GetScreenRect() { return Rect(0,0,m_pkEdit->m_iWidth, m_pkEdit->m_iHeight); }

	void AddItemToList(ZGuiWnd *pkWnd, bool bCombobox, const char *item, int id);
	void AddItemsToList(ZGuiWnd* pkWnd, bool bCombobox, char** items, int Number);
	ZGuiWnd* CreateEditPropertyDialog(int x, int y, int Widht, int Height);
	bool CreateWindows();
	bool InitSkins();

	ZGuiWnd* Get(string szName);

public:

	bool PropertyProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams );
	bool WndProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams );
	bool IsMenuActive() { return m_bMenuActive; }

	Gui(ZeroEdit* pkEdit);
	virtual ~Gui();

	FileOpenDlg* m_pkFileDlgbox;

	friend class FileOpenDlg;

};

#endif // !defined(AFX_GUI_H__6ADF8B0C_5888_4E90_95E1_C2E517786ECA__INCLUDED_)
