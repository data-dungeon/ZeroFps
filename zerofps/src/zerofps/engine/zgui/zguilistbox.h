// Listbox.h: interface for the Listbox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTBOX_H__F667D216_3A7B_4606_907F_E48FFD386301__INCLUDED_)
#define AFX_LISTBOX_H__F667D216_3A7B_4606_907F_E48FFD386301__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../engine_x.h"
#include "../../basic/rect.h"
#include "zguicontrol.h"
#include "zguilistitem.h"

class ZGuiRender;
class ZGuiScrollbar;

class ENGINE_API ZGuiListbox : public ZGuiControl
{
public:
	ZGuiListbox(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, int iID=0, int iItemHeight=20, 
		ZGuiSkin* pkTextSkin=NULL, int iTextureMask=-1, ZGuiSkin* pkSkinItem=NULL, ZGuiSkin* pkSkinItemSelected=NULL, 
		ZGuiSkin *pkSkinItemHighLight=NULL);
	virtual ~ZGuiListbox();

	void Resize(int Width, int Height);
	void UpdateList();
	bool RemoveAllItems();
	bool RemoveItem(unsigned int iID);
	void DisableSelItem(bool bDisable=true);
	ZGuiListitem* GetItem(int iID);
	bool AddItem(char* strText, unsigned int iID); 
	void SetScrollbarSkin(ZGuiSkin* pkSkinScrollArea, ZGuiSkin* pkSkinThumbButton, 
		ZGuiSkin* pkSkinThumbButtonHighLight);
	void SetItemNormalSkin(ZGuiSkin* pkSkin);
	void SetItemSelectedSkin(ZGuiSkin* pkSkin);
	void SetItemHighLightSkin(ZGuiSkin* pkSkin);
	bool Notify(ZGuiWnd* pkWnd, int iCode);
	bool Render( ZGuiRender* renderer );
	unsigned short GetItemHeight() { return m_unItemHeight; }
	ZGuiListitem* GetSelItem();
	ZGuiScrollbar* GetVScrollbar() { return m_pkScrollbarVertical; }

	typedef list<ZGuiListitem*>::iterator itItemList;

	Rect& GetItemArea() { return m_kItemArea; }

private:
	
	void ScrollItems(ZGuiScrollbar* pkScrollbar);
	void CreateInternalControls();

	ZGuiSkin* m_pkSkinItem, *m_pkSkinItemSelected, *m_pkSkinItemHighLight, *m_pkSkinItemFont;
	ZGuiScrollbar* m_pkScrollbarVertical, *m_pkScrollbarHorizontal;
	list<ZGuiListitem*> m_pkItemList;
	unsigned short m_unItemHeight;
	Rect m_kItemArea;
	ZGuiListitem* m_pkSelectedItem;
	bool m_bSelitemDisabled;
	int m_iScrollbarWidth;
};

#endif // !defined(AFX_LISTBOX_H__F667D216_3A7B_4606_907F_E48FFD386301__INCLUDED_)


