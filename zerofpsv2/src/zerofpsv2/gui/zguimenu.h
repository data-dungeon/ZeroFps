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
	int iIconIndex;
	bool bChecked;
	ZGuiButton* pkButton;
	ZGuiMenuItem* pkParent; // NULL if no parent
	bool bOpenSubMenu;
	bool bUseCheckMark;
	int iCheckMarkGroup;
};

/** \brief	A Menu in the gui.
	 \ingroup Gui
*/
class GUI_API ZGuiMenu : public ZGuiWnd 
{
public:
	ZGuiMenu(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, 
		int iID=0, bool bPopup=false);

	virtual ~ZGuiMenu();

	bool Render( ZGuiRender* renderer );
	void GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const;

	bool AddItem(const char* szText, const char* szNameID, const char* szParentID,
		bool bOpenSubMenu=false);
	
	ZGuiMenuItem* GetItem(const char* szNameID);
	void HideAll();
	void ClearAll();

	void ResizeMenu();

	bool IsOpen();
	bool IsMenuItem(ZGuiWnd* pkButton);
	bool HooverItem(int x, int y);

	char* GetItemName(int iID);

	void UseCheckMark(char* szItemNameID, bool bUse);
	void SetCheckMark(char* szItemNameID, bool bSet);
	void SetItemIcon(char* szItemNameID, char* szIconFile);
	void SetItemText(char* szItemNameID, char* szName);
	void SetCheckMarkGroup(char* szItemNameID, int iGroup);

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

	int s_iMenuIDCounter;

	bool m_bNeedToResize;

	bool m_bIsOpen;
	bool m_bPopup;

	vector<ZGuiSkin*> m_vkMenuIcons;
};

#endif 


