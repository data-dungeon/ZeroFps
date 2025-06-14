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
#include "editpropertydlg.h"
#include "workpaneldlg.h"

class FileOpenDlg;
//class WorkPanelDlg;
class DlgBox;

typedef bool (*ZGuiCallBack)(ZGuiWnd*, unsigned int, int, void*);

enum SEARCH_TASK
{
	LOAD_MAP,
	SAVE_MAP,
	LOAD_TEMPLATE,
	SAVE_TEMPLATE,
	NONE
};

class Gui
{
private:
	ZGuiSlider* CreateSlider(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h, 
		int flags, int min, int max, int pos, char* szResName, char* szBkSkin);
	bool IsButtonChecked(char* szName, char* szGroupName=NULL);
	bool CreateWorkPanel();
	void OpenPropertyDlg();
	
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
		int h, bool bMulitLine=false, char* szText=NULL, 
		char* szRegName=NULL, bool bNumberOnly=false);
	ZGuiCombobox* CreateCombobox(ZGuiWnd* pkParent, int iID, int x, int y, int w,
		int h, bool bMenu);
	ZGuiListbox* CreateListbox(ZGuiWnd* pkParent, int iID, int x, int y, int w,
		int h);
	ZGuiCheckbox* CreateCheckbox(ZGuiWnd* pkParent, int iID, int x, int y, int w, 
		int h, bool bChecked, char *pkName, char* pkResName);
	ZGuiButton* CreateButton(ZGuiWnd* pkParent, int iID, int pos_x, int pos_y,
		int w, int h, char* pkName);
	ZGuiLabel* CreateLabel(ZGuiWnd* pkParent, int iID, int x, int y, int w,
		int h, char* strText);
	ZGui* GetGUI() { return m_pkEdit->pkGui; }
	ZGuiTabCtrl* CreateTabbedDialog(char* szName, int iWndID, int iMainWndID, 
		int x, int y, int w, int h, vector<string> kNames, ZGuiCallBack pkProc);
	ZGuiTreebox* CreateTreebox(ZGuiWnd* pkParent, int iID, int x, int y, int w,
		int h, vector<ZGuiSkin*> akSkins, 
		Vector3 kBkColorRGB = Vector3(255.0f,255.0f,255.0f), 
		char* szRegName=NULL);	
	ZeroEdit* m_pkEdit;
	ZGui* m_pkGui;
	
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

	bool CheckRadioButton(char* szGroup, char* szName);
	void AddItemToList(ZGuiWnd *pkWnd, bool bCombobox, const char *item, int id, bool bSelect);
	void AddItemsToList(ZGuiWnd* pkWnd, bool bCombobox, char** items, int Number, bool bSelectLast=true);
	ZGuiWnd* CreateEditPropertyDialog(int x, int y, int Widht, int Height);
	bool CreateWindows();
	bool InitSkins();

	ZGuiWnd* Get(char* strName);

	map<string, DlgBox*> m_kDialogs;

public:
	bool OpenFileDlg(SEARCH_TASK eTask); 
	DlgBox* GetDlg(string strName);
	bool OpenDlg(string strResName);
	ZGuiWnd* GetMenu();
	void CloseMenu();
	void CreateTestWnd();
	void UpdatePropertybox();
	void ClosePropertybox();
	void OpenPropertybox();
	bool WndProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams );
	bool MenuProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams );
	bool WorkPanelProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams );

	void CaptureInput(bool bCapture);
	bool HaveFocus();

	Gui(ZeroEdit* pkEdit);
	virtual ~Gui();

//	ZGuiTabCtrl* m_pkWorkPanel;
	FileOpenDlg* m_pkFileDlgbox;

	friend class FileOpenDlg;
	friend class EditPropertyDlg;
	friend class WorkPanelDlg;
};

#endif // !defined(AFX_GUI_H__6ADF8B0C_5888_4E90_95E1_C2E517786ECA__INCLUDED_)
