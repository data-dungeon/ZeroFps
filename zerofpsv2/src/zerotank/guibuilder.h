// guibuilder.h

#ifndef _GUIBUILDER_H
#define _GUIBUILDER_H

#include <map>
#include <string>
using namespace std;

#include "../zerofpsv2/gui/zgui.h"

class TextureManager;

enum GuiType
{
	Wnd,
	Button,
	Checkbox,
	Combobox,
	Label,
	Listbox,
	Radiobutton,
	Scrollbar,
	Slider,
	TabControl,
	Textbox,
	Treebox,
};

class GuiBuilder  
{
public:
	bool CreateNewRadiobuttonGroup(const char *szName, int id);
	void Initialize();
	bool Create(GuiType eType, char* szResourceName, char* szText,
		int iID, int parentID, int x, int y, int w, int h, unsigned long uiFlags);

	GuiBuilder(ZGui* pkGui, TextureManager* pkTexMan, ZGui::callback oMainWndProc);
	~GuiBuilder();

private:
	int GetTexID(char* szFile);
	void InitTextures();
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
};

#endif // #ifndef _GUIBUILDER_H
