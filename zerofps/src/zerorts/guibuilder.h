// guibuilder.h: interface for the GuiBuilder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUIBUILDER_H__327897B3_8FAB_46F1_98E1_10831A36016E__INCLUDED_)
#define AFX_GUIBUILDER_H__327897B3_8FAB_46F1_98E1_10831A36016E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../zerofps/gui/zgui.h"

class TextureManager;
class Input;

#include <map>
#include <string>
using namespace std;

#include "../zerofps/gui/zgui.h"

class GuiBuilder  
{
public:
	GuiBuilder(TextureManager* pkTexMan, ZGui* pkGui);
	virtual ~GuiBuilder();

	map<string, ZGuiSkin*> m_kSkinMap;
	ZGuiSkin* GetSkin(char* strName);
	ZGuiWnd* Get(char* strName);

	ZGuiLabel* CreateLabel(ZGuiWnd* pkParent, int iID, int x, 
		int y, int w, int h, char* strText, char* szRegName=NULL);

private:
	bool InitSkins();
	TextureManager* m_pkTexMan;
	ZGui* m_pkGui;
};

#endif // !defined(AFX_GUIBUILDER_H__327897B3_8FAB_46F1_98E1_10831A36016E__INCLUDED_)
