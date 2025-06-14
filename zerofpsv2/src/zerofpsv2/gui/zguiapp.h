// zguiapp.h

#ifndef _ZGUIAPP_H
#define _ZGUIAPP_H

#pragma warning( disable : 4512) // 'class' : assignment operator could not be generated 

#include "gui_x.h"
#include <map>
#include <string>
using namespace std;

#include "zgui.h"
#include "../script/zfscript.h"

class TextureManager;
class ZSSScriptSystem;
class ZGuiResourceManager;
class ZFResourceHandle;
class ZFScript;

// Flags for CreateWnd
#define EB_IS_MULTILINE			 1
#define READ_ONLY					 2
#define CREATE_WND_HIDDEN		 4
#define MENU_IS_POPUP			 8
#define CREATE_NEW_GROUP		16 // The radiobutton is the leader for a new group.

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
	Progressbar,	// 13
	GuiType_Error
};

enum WndAlignent
{
	TopLeft,     // 0. F�nstret flyttas ej.
	TopRight,    // 1. F�nstret flyttas s� att avst�ndet till h�gersidan av sk�rmen/parent f�nstret 
					 // blir den samma som innan.
	BottomLeft,  // 2. F�nstret flyttas s� att avst�ndet till undersidan av sk�rmen/parent f�nstret 
					 // blir den samma som innan.
	BottomRight, // 3. F�nstret flyttas s� att avst�ndet till undersidan och h�gersidan 
					 // av sk�rmen/parent f�nstret blir den samma som innan.
	CenterHorz,  // 4. F�nstret flyttas s� att det hamnar i centrum p� sk�rmen/parent f�nstret horisontellt.
	CenterVert,  // 5. F�nstret flyttas s� att det hamnar i centrum  p� sk�rmen/parent f�nstret vertilkalt.
	Center,      // 6. F�nstret flyttas s� att det hamnar i centrum p� sk�rmen/parent f�nstret 
					 // horisontellt och vertilkalt.
};

enum WndResizeType
{
	eNone,				// 0. F�nstret skall inte skalas om.
	ResizeWidth,	// 1
	ResizeHeight,	// 2
	Resize,			// 3
	//MaxSize,      // 1. Skala om s� att f�nstret blir lika stort som sk�rmen/parent f�nstret.
	//MaxWidth,     // 2. Skala om s� att f�nstret blir lika brett som sk�rmen/parent f�nstret.
	//MaxHeight,    // 3. Skala om s� att f�nstret blir lika h�gt som sk�rmen/parent f�nstret.
};

enum GuiScaleMode
{
	AUTO_SCALE		= 0,
	MANUALLY_SCALE = 1,
	DISABLE_SCALE	= -1
};

enum MsgBoxType
{
	MSGBOX_OK = 0,
	MSGBOX_YESNO = 1,
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
		ZSSScriptSystem* pkScriptSys;
	};

	int LoadGuiTextureByRes(string strFile); ///< Use this function instead of LoadTexture in texturemanager to load correct image based on resolution.
	bool BuildFileTree(char* szTreeBoxName, char* szRootPath, char* szExtension=NULL);
	bool CreateMenu(char* szFileName, char* szName, bool bPopup);
	int  GetWndID(char* szResName); // returns -1 if no window exist and can be used to check if a window exist from script.
	void ClearListbox(char* szName);
	void AddListItem(char* szListboxResName, char* szText, bool bSelect=false);
//	void AddListItem(int iListboxID, char* szText);
	bool IsWndVisible(char* szResName);
	void ResizeWnd(char* szResName, int w, int h);
	bool ChangeSkin(lua_State* pkState, char* szID, char* szSkinName, char* szSkinType);

	bool LoadGuiFromScript(const char* szFileName); ///< Open GUI enviroment from script.

	ZGuiSkin* AddSkinFromScript(char* szName, ZGuiSkin* pkSkin=NULL);
	ZGuiSkin* AddSkinFromScript2(char *szName, lua_State* pkResHandle, ZGuiSkin* pkSkin);

	bool IsButtonChecked(char* szWnd);
	void CheckButton(const char* szWnd, bool bCheck);
	float GetTextFloat(char* szWnd, bool* pkSuccess);
	int GetTextInt(char* szWnd, bool* pkSuccess);
	char* GetText(char* szWnd);
	char* GetSelItem(char* szWnd);
	bool SelListItem(char* szWnd, char* szItem);
	bool SelListItemByIndex(char* szWnd, int iIndex);

	void SetTextFloat(char* szWndResName, float fNumber, bool bResize=false);
	void SetTextInt(char* szWndResName, int iNumber, bool bResize=false);
	void SetText(char* szWndResName, char* szText, bool bResize=false);

	bool ShowWnd(char* szWndResName, bool bShow, bool bPlaceFront=false, bool bSetCapture=false);

	ZGuiWnd* GetTabPage(char* szTabBox, int iPage);
	void AddTabPage(char* szTabboxID, char* szPageResName, char* szName);
	void AddTreeItem(char* szTreeboxID, const char* szID, const char* szIDParent, char* szText,
		unsigned char iNodeSkinNormal, unsigned char iNodeSkinSelected);
	bool CreateNewRadiobuttonGroup(const char *szName, int id=-1);
	
	void InitGui(ZSSScriptSystem* pkScriptSys, char* szFontTexture, 
		char* szScriptFile, char* szMenuFile, bool bUseHardwareMouse, int iScaleMode=-1);

	ZGuiWnd* CreateWnd(GuiType eType, char* szWndName, char* szParentName, 
		char* szLabel, int x, int y, int w, int h, int iFlags,
		WndAlignent eAlignment=TopLeft, WndResizeType eResizeType=eNone);

	ZGuiWnd* CreateWnd(GuiType eType, char* szResourceName, char* szText, 
		ZGuiWnd* pkParent, int x, int y, int w, int h, int iFlags,
		WndAlignent eAlignment=TopLeft, WndResizeType eResizeType=eNone);

	void CloseWindow(char* szResName);
	ZGuiWnd* GetWnd(int iID);
	ZGuiWnd* GetWnd(string strName);
	ZGuiSkin* GetSkin(string strName);
	GuiType GetWndType(ZGuiWnd* pkWnd);

	bool SetFont(string strWnd, string strFont, int r=0, int g=0, int b=0, int glyph=0);

	void MsgBox(char* text, char* caption, int type);

	typedef bool (*callback)(ZGuiWnd* pkWnd, unsigned int uiMessage, int iNumParams, void *pParams);

	int GetWidth();
	int GetHeight();

	ZFScript* GetGuiScript();

	ZGuiApp(callback oMainWndProc);
	~ZGuiApp();

	ZFResourceHandle* m_pkScriptResHandle;

	bool m_bGuiHaveFocus;

   int m_iScaleMode; ///< the settings to determin the scaling operation of the gui
							//  if design resolution and screen resolution are not the same.
						   //  0 = automaticly (scale), 1 = manually (user scaling), -1 = none
							
	Point m_kDesignResolution; ///< width and height of the screen that the gui for this app is made for.

	float GetScaleX();
	float GetScaleY();

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
	void FindGuiTextureByResolution();

	ZGui* m_pkGuiSys;
	ZGuiResourceManager* m_pkResMan;
	ZGuiRender* m_pkRenderer;		// Pointer to the gui render object
	TextureManager* m_pkTextureMan;
	ZSSScriptSystem* m_pkScriptSystem;
	map<string, ZGuiSkin*> m_kSkins;
	map<int, ZGuiWnd*> m_kWindows;

	// 1: namn p� bilden (tex "icons/sword.tga") 
   // 2: hela filnmanmet baserat p� uppl�sning (tex "data/textures/gui/800x600/icons/sword.tga")
	map<string, string> m_kGuiImagePaths; 
		
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
