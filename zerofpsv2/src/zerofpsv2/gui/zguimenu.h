// ZGuiMenu.h: interface for the ZGuiMenu class.

#ifndef _GUI_ZGUIMENU_H_
#define _GUI_ZGUIMENU_H_

#pragma warning( disable : 4512) // 'class' : assignment operator could not be generated 

#include <map>
#include "gui_x.h"
#include "../basic/rect.h"
#include "zguiwindow.h"
#include "zguilabel.h"
#include "zguibutton.h"

class ZGuiRender;

struct ZGuiMenuItem
{
	char* szNameID;
	ZGuiButton* pkButton;
	ZGuiMenuItem* pkParent; // NULL if no parent
	bool bOpenSubMenu;
};

/** \brief	A Menu in the gui.
	 \ingroup Gui
*/
class GUI_API ZGuiMenu : public ZGuiWnd 
{
public:
	ZGuiMenu(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, 
		int iID=0);

	virtual ~ZGuiMenu();

	bool Render( ZGuiRender* renderer );
	void GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const;

	bool AddItem(const char* szText, const char* szNameID, const char* szParentID,
		bool bOpenSubMenu=false);
	
	ZGuiMenuItem* GetItem(const char* szNameID);
	void HideAll();

	void ResizeMenu();

	bool IsOpen();
	bool IsMenuItem(ZGuiWnd* pkButton);
	bool HooverItem(int x, int y);

protected:
	bool Notify(ZGuiWnd* pkWnd, int iCode);

private:
	void CreateInternalControls();

	int GetNumChilds(char* szNameID);
	ZGuiMenuItem* GetRightParent();
	void OpenSubMenu(ZGuiMenuItem* pkSubMenu, bool bOpen);
	bool MenuIsOpen(ZGuiMenuItem* pkMenu);
	bool IsChildOf(ZGuiMenuItem* pkSubMenu);
	

	ZGuiLabel* m_pkLabel;

	vector<ZGuiMenuItem*> m_vkItems;
	map<ZGuiMenuItem*, bool> m_mkSubMenuStateMap;

	ZGuiSkin* m_pkItemSkinUp;
	ZGuiSkin* m_pkItemSkinFocus;
	ZGuiSkin* m_pkSkinDown;

	static int s_iMenuIDCounter;

	bool m_bNeedToResize;

	bool m_bIsOpen;

};

#endif 


