// zguiresourcemanager.h: interface for the ZGuiResourceManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZGUIRESOURCEMANAGER_H__522A0AC6_0317_4003_94C3_FE568ABA9E36__INCLUDED_)
#define AFX_ZGUIRESOURCEMANAGER_H__522A0AC6_0317_4003_94C3_FE568ABA9E36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../basic/basic.pkg"
#include "gui_x.h"
#include <string>
#include <map>
using namespace std;

class ZGuiWnd;
class ZGuiFont;
class ZGuiSkin;
class ZGui;

class GUI_API ZGuiResourceManager : public ZFObject
{
public:
	ZGuiResourceManager();
	~ZGuiResourceManager();

	ZGuiWnd* Wnd(string strName);
	ZGuiFont* Font(string strName);
	ZGuiSkin* Skin(string strName);

	ZGuiFont* Add(string strName, ZGuiFont* pkFont);
	ZGuiSkin* Add(string strName, ZGuiSkin* pkSkin);

	bool RemoveFont(string strName); // does not delete it, just erase it from list
	bool RemoveSkin(string strName); // does not delete it, just erase it from list

	void GetWindows( map<string, ZGuiWnd*>& kWindows )	{ kWindows=m_kWindows;	}
	void GetSkins  ( map<string, ZGuiSkin*>& kSkins  )	{ kSkins=m_kSkins;		}
	void GetFonts  ( map<string, ZGuiFont*>& kFonts  )	{ kFonts=m_kFonts;		}
	
private:
	map<string, ZGuiWnd*> m_kWindows;
	map<string, ZGuiSkin*> m_kSkins;
	map<string, ZGuiFont*> m_kFonts;

	ZGuiWnd* Add(string strName, ZGuiWnd* pkWnd);
	bool RemoveWnd(string strName); // does not delete it, just erase it from list
	friend class ZGui;

};

#endif // !defined(AFX_ZGUIRESOURCEMANAGER_H__522A0AC6_0317_4003_94C3_FE568ABA9E36__INCLUDED_)
