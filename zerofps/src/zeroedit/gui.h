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
class EditPropertyDlg;
typedef bool (*ZGuiCallBack)(ZGuiWnd*, unsigned int, int, void*);

class Gui
{
private:
	
	enum SEARCH_TASK
	{
		MAP,
		LOAD_TEMPLATE,
		SAVE_TEMPLATE
	};

	struct MENU_INFO
	{
		ZGuiCombobox* cb;
		int iIndex;
		char* szCommando;
	};

	SEARCH_TASK m_kSearchTask;

	void RunMenuCommand(ZGuiCombobox* pkCombox, int iIndex);
	bool CreateMenu(ZFIni* pkIni, char* szFileName);
	bool Register(ZGuiFont* pkFont, char* strName);
	bool Register(ZGuiSkin* pkSkin, char* strName);
	bool Register(ZGuiWnd* pkWnd, char* strName);
	int CreateRadiobuttons(ZGuiWnd* pkParent, vector<string>& strNames, 
		char* strGroupName, int start_id, int x, int y, int size); // returns height
	ZGuiTextbox* CreateTextbox(ZGuiWnd* pkParent, int iID, int x, int y, int w,
		int h, bool bMulitLine=false);
	ZGuiCombobox* CreateCombobox(ZGuiWnd* pkParent, int iID, int x, int y, int w,
		int h, bool bMenu);
	ZGuiListbox* CreateListbox(ZGuiWnd* pkParent, int iID, int x, int y, int w,
		int h);
	ZGuiButton* CreateButton(ZGuiWnd* pkParent, int iID, int pos_x, int pos_y,
		int w, int h, char* pkName);
	ZGuiLabel* CreateLabel(ZGuiWnd* pkParent, int iID, int x, int y, int w,
		int h, char* strText);
	ZGui* GetGUI() { return m_pkEdit->pkGui; }
	
	ZeroEdit* m_pkEdit;
	
	map<string, ZGuiSkin*> m_kSkinMap;
	map<string, int> m_kTextureMap;
	ZGuiSkin* GetSkin(char* strName);
	int GetTexture(char* strName);

	int m_iScreenCX;
	int m_iScreenCY;
	bool m_bGuiHaveFocus;

	unsigned int m_uiNumMenuItems;
	MENU_INFO* m_pkMenuInfo;
	
	Rect GetScreenRect() { return Rect(0,0,m_pkEdit->m_iWidth, m_pkEdit->m_iHeight); }

	void AddItemToList(ZGuiWnd *pkWnd, bool bCombobox, const char *item, int id, bool bSelect);
	void AddItemsToList(ZGuiWnd* pkWnd, bool bCombobox, char** items, int Number, bool bSelectLast=true);
	ZGuiWnd* CreateEditPropertyDialog(int x, int y, int Widht, int Height);
	bool CreateWindows();
	bool InitSkins();

	ZGuiWnd* Get(char* strName);

public:
	void CreateTestWnd();
	void UpdatePropertybox();
	void ClosePropertybox();
	bool WndProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams );
	bool MenuProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams );

	void CaptureInput(bool bCapture);
	bool HaveFocus();

	Gui(ZeroEdit* pkEdit);
	virtual ~Gui();

	FileOpenDlg* m_pkFileDlgbox;
	EditPropertyDlg* m_pkEditPropDlgBox;

	friend class FileOpenDlg;
	friend class EditPropertyDlg;
};

#endif // !defined(AFX_GUI_H__6ADF8B0C_5888_4E90_95E1_C2E517786ECA__INCLUDED_)
