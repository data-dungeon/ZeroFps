// zguiapp.h

#ifndef _ZGUIAPP_H
#define _ZGUIAPP_H

#include "gui_x.h"
#include <map>
#include <string>
using namespace std;

#include "zgui.h"
#include "../script/zfscript.h"

class TextureManager;
class ZFScriptSystem;
class ZGuiResourceManager;
class ZFResourceHandle;
class ZFScript;

#define EB_IS_MULTILINE			0x1
#define READ_ONLY					0x2
#define CREATE_WND_HIDDEN		0x3

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
	Menu,				// 12
	GuiType_Error
};

enum WndAlignent
{
	TopLeft,     // 0. Fönstret flyttas ej.
	TopRight,    // 1. Fönstret flyttas så att avståndet till högersidan av skärmen/parent fönstret 
					 // blir den samma som innan.
	BottomLeft,  // 2. Fönstret flyttas så att avståndet till undersidan av skärmen/parent fönstret 
					 // blir den samma som innan.
	BottomRight, // 3. Fönstret flyttas så att avståndet till undersidan och högersidan 
					 // av skärmen/parent fönstret blir den samma som innan.
	CenterHorz,  // 4. Fönstret flyttas så att det hamnar i centrum på skärmen/parent fönstret horisontellt.
	CenterVert,  // 5. Fönstret flyttas så att det hamnar i centrum  på skärmen/parent fönstret vertilkalt.
	Center,      // 6. Fönstret flyttas så att det hamnar i centrum på skärmen/parent fönstret 
					 // horisontellt och vertilkalt.
};

enum WndResizeType
{
	eNone,				// 0. Fönstret skall inte skalas om.
	ResizeWidth,	// 1
	ResizeHeight,	// 2
	Resize,			// 3
	//MaxSize,      // 1. Skala om så att fönstret blir lika stort som skärmen/parent fönstret.
	//MaxWidth,     // 2. Skala om så att fönstret blir lika brett som skärmen/parent fönstret.
	//MaxHeight,    // 3. Skala om så att fönstret blir lika högt som skärmen/parent fönstret.
};

/** \brief	ZGuiApp
	 \ingroup Gui
*/
class GUI_API ZGuiApp
{
public:

	struct THREAD_INFO
	{
		char* szFileName;
		ZFScriptSystem* pkScriptSys;
	};

	bool BuildFileTree(char* szTreeBoxName, char* szRootPath, char* szExtension=NULL);
	bool CreateMenu(char* szFileName);
	int  GetWndID(char* szResName); // returns -1 if no window exist and can be used to check if a window exist from script.
	void ClearListbox(char* szName);
	void AddListItem(char* szListboxResName, char* szText);
//	void AddListItem(int iListboxID, char* szText);
	bool IsWndVisible(char* szResName);
	void ResizeWnd(char* szResName, int w, int h);
	bool ChangeSkin(lua_State* pkState, char* szID, char* szSkinName, char* szSkinType);

	bool LoadGuiFromScript(char* szFileName); ///< Open GUI enviroment from script.

	ZGuiSkin* AddSkinFromScript(char* szName, ZGuiSkin* pkSkin=NULL);
	ZGuiSkin* AddSkinFromScript2(char *szName, lua_State* pkResHandle, ZGuiSkin* pkSkin);

	bool IsButtonChecked(char* szWnd);
	float GetTextFloat(char* szWnd, bool* pkSuccess);
	int GetTextInt(char* szWnd, bool* pkSuccess);
	char* GetText(char* szWnd);
	char* GetSelItem(char* szWnd);
	bool SelListItem(char* szWnd, char* szItem);

	void SetTextFloat(char* szWndResName, float fNumber, bool bResize=false);
	void SetTextInt(char* szWndResName, int iNumber, bool bResize=false);
	void SetText(char* szWndResName, char* szText, bool bResize=false);

	bool ShowWnd(char* szWndResName, bool bShow);

	ZGuiWnd* GetTabPage(char* szTabBox, int iPage);
	void AddTabPage(char* szTabboxID, char* szPageResName, char* szName);
	void AddTreeItem(char* szTreeboxID, const char* szID, const char* szIDParent, char* szText,
		unsigned char iNodeSkinNormal, unsigned char iNodeSkinSelected);
	bool CreateNewRadiobuttonGroup(const char *szName, int id);
	
	void InitGui(ZFScriptSystem* pkScriptSys, char* szFontTexture, 
		char* szScriptFile, char* szMenuFile, bool bUseHardwareMouse);

	ZGuiWnd* CreateWnd(GuiType eType, char* szWndName, char* szParentName, 
		char* szLabel, int x, int y, int w, int h, unsigned long uiFlags,
		WndAlignent eAlignment=TopLeft, WndResizeType eResizeType=eNone);
	ZGuiWnd* CreateWnd(GuiType eType, char* szResourceName, char* szText, 
		ZGuiWnd* pkParent, int x, int y, int w, int h, unsigned long uiFlags,
		WndAlignent eAlignment=TopLeft, WndResizeType eResizeType=eNone);

	void CloseWindow(char* szResName);
	ZGuiWnd* GetWnd(int iID);
	ZGuiWnd* GetWnd(string strName);
	ZGuiSkin* GetSkin(string strName);
	GuiType GetWndType(ZGuiWnd* pkWnd);

	typedef bool (*callback)(ZGuiWnd* pkWnd, unsigned int uiMessage, int iNumParams, void *pParams);

	int GetWidth();
	int GetHeight();

	ZFScript* GetGuiScript();

	ZGuiApp(callback oMainWndProc);
	~ZGuiApp();

	ZFResourceHandle* m_pkScriptResHandle;

	bool m_bGuiHaveFocus;

private:

	struct MENU_INFO
	{
		ZGuiCombobox* cb;
		int iIndex;
		char* szCommando;
	};

	int m_uiNumMenuItems;
	MENU_INFO* m_pkMenuInfo;

	unsigned int m_uiWindowIDCounter;

	int GetTexID(char* szFile);
	void InitDefaultSkins();

	ZGui* m_pkGuiSys;
	ZGuiResourceManager* m_pkResMan;
	ZGuiRender* m_pkRenderer;		// Pointer to the gui render object
	TextureManager* m_pkTextureMan;
	ZFScriptSystem* m_pkScriptSystem;
	map<string, ZGuiSkin*> m_kSkins;
	map<int, ZGuiWnd*> m_kWindows;
	
	callback m_oMainWndProc;
	ZGuiRadiobutton* m_pkLastRadiobutton;

	char* m_szLastRadioBGroup;
	int m_iLastRadioBGroup;
	char m_szTexName[250];

	struct SORT_FILES : public binary_function<string, string, bool> {
		bool operator()(string x, string y);
	} SortFiles;

};

#endif // #ifndef _ZGUIAPP_H
