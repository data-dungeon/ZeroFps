// zguiresourcemanager.h: interface for the ZGuiResourceManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZGUIRESOURCEMANAGER_H__522A0AC6_0317_4003_94C3_FE568ABA9E36__INCLUDED_)
#define AFX_ZGUIRESOURCEMANAGER_H__522A0AC6_0317_4003_94C3_FE568ABA9E36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../basic/basic.pkg"
#include "../engine_x.h"
#include <string>
#include <map>
using namespace std;

class ZGuiWnd;
class ZGuiFont;
class ZGuiSkin;

class ENGINE_API ZGuiResourceManager : public ZFObject
{
public:
	ZGuiResourceManager();
	~ZGuiResourceManager();

	ZGuiWnd* Wnd(string szName);
	ZGuiFont* Font(string szName);
	ZGuiSkin* Skin(string szName);

	ZGuiWnd* Add(string szName, ZGuiWnd* pkWnd);
	ZGuiFont* Add(string szName, ZGuiFont* pkWnd);
	ZGuiSkin* Add(string szName, ZGuiSkin* pkWnd);
	
private:
	map<string, ZGuiWnd*> m_kWindows;
	map<string, ZGuiSkin*> m_kSkins;
	map<string, ZGuiFont*> m_kFonts;

};

#endif // !defined(AFX_ZGUIRESOURCEMANAGER_H__522A0AC6_0317_4003_94C3_FE568ABA9E36__INCLUDED_)
