#ifndef _GUIBUILDER_H
#define _GUIBUILDER_H

class ZGui;
class ZGuiTextbox;
class ZGuiRadiobutton;
class ZGuiButton;
class ZGuiCheckbox;
class ZGuiLabel;
class ZGuiListbox;
class ZGuiSkin;
class ZGuiScrollbar;
class ZGuiCombobox;
class TextureManager;
class ZGuiResourceManager;
class ZGuiFont;

#pragma warning( disable : 4786 )

#include "selectwnd.h"
#include "../zerofps/engine/zgui/zgui.h"
#include <map>
#include <string>
using namespace std;

enum CtrlType
{
	WINDOW,					// 1
	LABEL,					// 2
	TEXTBOX,				// 3
	BUTTON,					// 4
	CHECKBOX,				// 5
	RADIOBUTTON,			// 6
	RADIOBUTTON_NEW_GROUP,	// 7
	COMBOBOX,				// 8
	LISTBOX,				// 9
	SCROLLBAR,				// 10
	NONE
};

class GuiBuilder  
{
public:
	typedef bool (*ZGuiWndProc)(ZGuiWnd*, unsigned int, int, void*);
	
	ZGuiButton* CreateCancelButton(ZGuiWnd* pkParent, int iID, char* szRegName,
		bool bOKButtonExist=false,int TabOrderNr=-1);
	ZGuiButton* CreateOKButton(ZGuiWnd* pkParent, int iID, char* szRegName,
		bool bExistbuttonExist=false, int TabOrderNr=-1);
	ZGuiButton* CreateCloseButton(ZGuiWnd* pkParent, int iID,char* szRegName,
		int TabOrderNr=-1);
	ZGuiScrollbar* CreateScrollbar(ZGuiWnd* pkParent, int iID, char* szRegName,
		int x, int y, int w, int h, int TabOrderNr=-1);
	ZGuiCombobox* CreateCombobox(ZGuiWnd* pkParent, int iID, char* szRegName, 
		int x, int y, int w, int h, bool bMenu, int TabOrderNr=-1);
	ZGuiListbox* CreateListbox(ZGuiWnd* pkParent, int iID, char* szRegName,
		int x, int y, int w, int h, int TabOrderNr=-1);
	ZGuiCheckbox* CreateCheckbox(ZGuiWnd* pkParent, int iID, char* szRegName, 
		int x, int y, int w, int h, char *pkName, bool bCheck=false, int TabOrderNr=-1);
	ZGuiTextbox* CreateTextbox(ZGuiWnd* pkParent, int iID, char* szRegName, int x, 
		int y, int w, int h, bool bMulitLine, char* szSkin="white", int TabOrderNr=-1);
	ZGuiLabel* CreateLabel(ZGuiWnd* pkParent, int iID, char* szRegName, int x, 
		int y, int w, int h, char* szText, char* szSkin="Label", int TabOrderNr=-1);
	ZGuiWnd* CreateMainWindow(int iMainWndID, int iID, char* szRegName, int x, int y, 
		int w, int h, ZGuiWndProc oWndProc, char* szSkin="gray", int TabOrderNr=-1);
	ZGuiRadiobutton* CreateRadioButton(ZGuiWnd* pkParent, int iID, char* szRegName,
		int GroupID, char* szGroupName, bool bCreateNewGroup, int x, int y, int w, int h, char *szName, 
		bool bCheck=false, int TabOrderNr=-1);
	ZGuiButton* CreateButton(ZGuiWnd* pkParent, int iID, char* szRegName, int x, int y, 
		int w, int h, char *szName, int TabOrderNr=-1);

	bool IsResNameLegalForWnd(const ZGuiWnd* pkWndToCheck, const char* szResName);
	bool IsGroupNameLegal(ZGuiRadiobutton* pkRadibuttonToCheck, 
		const char* szGroupName);

	ZGuiWnd* GetWnd(const char* szName);
	ZGuiSkin* GetSkin(const char* szName, bool bAllocateNew=false);
	int GetTexture(const char* szName);
	CtrlType GetWndType(ZGuiWnd* pkWnd);
	
	bool InitSkins();

	GuiBuilder(ZGui* pkGui, TextureManager* pkTexMan, 
		ZGuiResourceManager* pkGuiMan, Rect rcScreen);

	~GuiBuilder();

private:
	bool Register(ZGuiWnd *pkWnd, char* szName);
	bool Register(ZGuiSkin *pkSkin, char* szName);
	bool Register(ZGuiFont *pkFont, char* szName);

	map<string, ZGuiSkin*> m_kSkinMap;
	map<string, int> m_kTextureMap;

	ZGuiResourceManager* m_pkGuiMan;
	TextureManager* m_pkTexMan;
	
public:
	bool RenameWnd(ZGuiWnd* pkWndToRename, const char* szNewName);
	void SetScrollPos(const char*szScrollbar, float fPos);
	void SetColor(float afColor[3], const unsigned char r, const unsigned char g,
		const unsigned char b);
	char* GetTypeName(ZGuiWnd *pkWnd);
	CtrlType GetType(char* szTypeName);
	bool IsButtonChecked(const char* szCheckBox);
	int GetTextInt(const char *szWindow);
	bool SetTextString(const char *szWindow, char* szString);
	bool SetTextInt(const char* szWindow, int iValue);
	bool GenUniqueWndName(ZGuiWnd* pkWnd, char szName[50]);
	Rect m_rcScreen;
	ZGui* m_pkGui;
	bool m_bAllocateNewSkins;
	
	static int m_iLastID;
	static int m_iLastRadioGroup;
	static char m_szLastGroupName[50];
	static int m_iLastTabNr;
};

#endif // #ifndef _GUIBUILDER_H
