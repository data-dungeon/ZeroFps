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

#include "../zerofps/engine/zgui/zgui.h"
#include <map>
#include <string>
using namespace std;

class GuiBuilder  
{
public:

	enum CtrlType
	{
		WINDOW,
		LABEL,
		TEXTBOX,
		BUTTON,
		CHECKBOX,
		RADIOBUTTON,
		COMBOBOX,
		LISTBOX,
		SCROLLBAR,
	};

	typedef bool (*ZGuiWndProc)(ZGuiWnd*, unsigned int, int, void*);

	bool CreateNewType(CtrlType eType, ZGuiWndProc oWndProc);
	
	ZGuiButton* CreateCancelButton(ZGuiWnd* pkParent, int iID, 
		bool bOKButtonExist);
	ZGuiButton* CreateOKButton(ZGuiWnd* pkParent, int iID, int x=-1, int y=-1);
	ZGuiButton* CreateCloseButton(ZGuiWnd* pkParent, int iID);
	ZGuiScrollbar* CreateScrollbar(ZGuiWnd* pkParent, int iID, int x, int y, 
		int w, int h);
	ZGuiCombobox* CreateCombobox(ZGuiWnd* pkParent, int iID, int x, int y, 
		int w, int h, bool bMenu);
	ZGuiListbox* CreateListbox(ZGuiWnd* pkParent, int iID, int x, int y, int w,
		int h);
	ZGuiCheckbox* CreateCheckbox(ZGuiWnd* pkParent, int iID, int x, int y, 
		int w, int h, char *pkName);
	ZGuiTextbox* CreateTextbox(ZGuiWnd* pkParent, int iID, int x, int y, 
		int w, int h, bool bMulitLine, char* szSkin="white");
	ZGuiLabel* CreateLabel(ZGuiWnd* pkParent, int iID, int x, int y, int w, 
		int h, char* szText, char* szSkin="");
	ZGuiWnd* CreateMainWindow(int iMainWndID, int iID, int x, int y, int w, 
		int h, ZGuiWndProc oWndProc, char* szSkin="gray" );
	ZGuiRadiobutton* CreateRadioButton(ZGuiWnd* pkParent, int iID, int GroupID,
		int x, int y, int w, int h, char *szName);
	ZGuiButton* CreateButton(ZGuiWnd* pkParent, int iID, int x, int y, 
		int w, int h, char *szName);

	bool Register(ZGuiWnd *pkWnd, char* strName);
	bool Register(ZGuiSkin *pkSkin, char* strName);
	bool Register(ZGuiFont *pkFont, char* strName);

	ZGuiWnd* GetWnd(char* strName);
	ZGuiSkin* GetSkin(char* strName);
	int GetTexture(char* strName);
	
	bool InitSkins();

	GuiBuilder(ZGui* pkGui, TextureManager* pkTexMan, 
		ZGuiResourceManager* pkGuiMan, Rect rcScreen);

	virtual ~GuiBuilder();


private:
	map<string, ZGuiSkin*> m_kSkinMap;
	map<string, int> m_kTextureMap;

	ZGuiResourceManager* m_pkGuiMan;
	TextureManager* m_pkTexMan;

	CtrlType m_kSelectedCtrType;
	int m_iLastID;

	int m_iNewCtrPosX;
	int m_iNewCtrPosY;

	ZGuiWnd* m_pkLastWndCreated;

public:
	Rect m_rcScreen;
	ZGui* m_pkGui;

};

#endif // #ifndef _GUIBUILDER_H
