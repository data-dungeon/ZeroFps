// guibuilder.h

#ifndef _GUIBUILDER_H
#define _GUIBUILDER_H

#include <map>
#include <string>
using namespace std;

#include "../zerofpsv2/gui/zgui.h"

class TextureManager;
class ZFScript;

#define EB_IS_MULTILINE   0x1

enum GuiType
{
	Wnd,				// 0
	Button,			// 1
	Checkbox,		// 2
	Combobox,		// 3
	Label,			// 4
	Listbox,			// 5
	Radiobutton,	// 6
	Scrollbar,		// 7
	Slider,			// 8
	TabControl,		// 9
	Textbox,			// 10
	Treebox,			// 11
	GuiType_Error
};

class GuiApp
{
public:
	bool IsButtonChecked(int iWndID);
	float GetTextFloat(int iWndID, bool* pkSuccess);
	int GetTextInt(int iWndID, bool* pkSuccess);
	char* GetText(int iWndID);

	void SetTextFloat(int iWndID, float fNumber, bool bResize=false);
	void SetTextInt(int iWndID, int iNumber, bool bResize=false);
	void SetText(int iWndID, char* szText, bool bResize=false);
	ZGuiWnd* GetTabPage(int iTabCtrlID, int iPage);
	void AddTabPage(int iTabCtrlID, char* szName);
	void AddTreeItem(int iTreeboxID, const char* szID, const char* szIDParent, char* szText,
		unsigned char iNodeSkinNormal, unsigned char iNodeSkinSelected);
	void AddListItem(int iListboxID, char* szText, bool bCombobox=false);
	bool CreateNewRadiobuttonGroup(const char *szName, int id);
	void InitializeGui(ZGui* pkGui, TextureManager* pkTexMan, ZFScript* pkScript);
	bool CreateWnd(GuiType eType, char* szResourceName, char* szText,
		int iID, int parentID, int x, int y, int w, int h, unsigned long uiFlags);
	bool GuiApp::CreateWnd(GuiType eType, char* szResourceName, char* szText, int iID, 
		ZGuiWnd* pkParent, int x, int y, int w, int h, unsigned long uiFlags);

	GuiApp(ZGui::callback oMainWndProc);
	~GuiApp();

private:
	char* GetTexName(ZFScript* pkScript, char* szName);
	GuiType GetType(ZGuiWnd* pkWnd);
	int GetTexID(char* szFile);
	void InitTextures(ZFScript* pkScript);
	ZGuiWnd* GetWnd(int iID);
	ZGuiSkin* GetSkin(string strName);

	ZGui* m_pkGui;
	TextureManager* m_pkTexMan;
	map<string, ZGuiSkin*> m_kSkins;
	map<int, ZGuiWnd*> m_kWindows;
	
	ZGui::callback m_oMainWndProc;
	ZGuiRadiobutton* m_pkLastRadiobutton;

	char* m_szLastRadioBGroup;
	int m_iLastRadioBGroup;
	char m_szTexName[250];
};

#endif // #ifndef _GUIBUILDER_H
