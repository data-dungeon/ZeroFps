// guibuilder.h: interface for the GuiBuilder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUIBUILDER_H__327897B3_8FAB_46F1_98E1_10831A36016E__INCLUDED_)
#define AFX_GUIBUILDER_H__327897B3_8FAB_46F1_98E1_10831A36016E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class TextureManager;
#include <map>
#include <string>
using namespace std;

#include "../zerofps/gui/zgui.h"

class GuiBuilder  
{
public:
	GuiBuilder(TextureManager* pkTexMan);
	virtual ~GuiBuilder();

	map<string, ZGuiSkin*> m_kSkinMap;
	ZGuiSkin* GetSkin(char* strName);

private:
	bool InitSkins();
	TextureManager* m_pkTexMan;
};

#endif // !defined(AFX_GUIBUILDER_H__327897B3_8FAB_46F1_98E1_10831A36016E__INCLUDED_)
