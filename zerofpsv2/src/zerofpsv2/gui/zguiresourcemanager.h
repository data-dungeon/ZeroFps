// zguiresourcemanager.h: interface for the ZGuiResourceManager class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _GUI_ZGUIRESMANGER_H_
#define _GUI_ZGUIRESMANGER_H_

#include "gui_x.h"
#include <string>
#include <map>
#include "../basic/zfobjectmanger.h"

using namespace std;

class ZGuiWnd;
class ZGuiFont;
class ZGuiSkin;
class ZGui;

class GUI_API ZGuiResourceManager : public ZFSubSystem
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

#endif
