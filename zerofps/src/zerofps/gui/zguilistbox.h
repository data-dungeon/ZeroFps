// Listbox.h: interface for the Listbox class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _GUI_ZGUILISTBOX_H_
#define _GUI_ZGUILISTBOX_H_

#include "gui_x.h"
#include "../basic/rect.h"
#include "zguilistitem.h"
#include "zguiwindow.h"

class ZGuiRender;
class ZGuiScrollbar;

class GUI_API ZGuiListbox : public ZGuiWnd
{
public:
	bool SelItem(int iIndex);
	int GetItemCount();
	void SelNone();
	ZGuiListbox(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, 
		int iID=0, int iItemHeight=20, ZGuiSkin* pkSkinItem=NULL, 
		ZGuiSkin* pkSkinItemSelected=NULL, ZGuiSkin *pkSkinItemHighLight=NULL);
	virtual ~ZGuiListbox();
	void GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const; // overloaded
	int Find(char* strString);
	void Resize(int Width, int Height);
	bool RemoveAllItems();
	bool RemoveItem(ZGuiListitem* pkItemToRemove, bool bSelPrev);
	void IsMenu(bool bMenu=true);
	ZGuiListitem* GetItem(unsigned int iIndex);
	ZGuiListitem* AddItem(char* strText, unsigned int iIndex, bool bSelect); 
	void SetScrollbarSkin(ZGuiSkin* pkSkinScrollArea, 
		ZGuiSkin* pkSkinThumbButton, ZGuiSkin* pkSkinThumbButtonHighLight);
	void SetItemNormalSkin(ZGuiSkin* pkSkin);
	void SetItemSelectedSkin(ZGuiSkin* pkSkin);
	void SetItemHighLightSkin(ZGuiSkin* pkSkin);
	bool Notify(ZGuiWnd* pkWnd, int iCode);
	bool Render( ZGuiRender* renderer );

	void SetZValue(int iValue); // överlagrad
	unsigned short GetItemHeight() { return m_unItemHeight; }
	ZGuiListitem* GetSelItem();
	ZGuiScrollbar* GetVScrollbar() { return m_pkScrollbarVertical; }

	typedef list<ZGuiListitem*>::iterator itItemList;

	Rect& GetItemArea() { return m_kItemArea; }

	ZGuiScrollbar* GetScrollbar() { return m_pkScrollbarVertical; }
	void CopyNonUniqueData(const ZGuiWnd* pkSrc);

private:
	
	void UpdateList();
	void ScrollItems(ZGuiScrollbar* pkScrollbar);
	void CreateInternalControls();

	ZGuiSkin* m_pkSkinBnUp, *m_pkSkinBnDown, *m_pkSkinBnHLight;
	ZGuiScrollbar* m_pkScrollbarVertical;
	list<ZGuiListitem*> m_pkItemList;
	unsigned short m_unItemHeight;
	unsigned short m_unOriginalHeight;
	Rect m_kItemArea;
	ZGuiListitem* m_pkSelectedItem;
	bool m_bIsMenu;
	int m_iScrollbarWidth;
};

#endif 


