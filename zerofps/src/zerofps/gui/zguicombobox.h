// Combobox.h: interface for the Combobox class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _GUI_ZGUICOMBOBOX_H_
#define _GUI_ZGUICOMBOBOX_H_

#include "gui_x.h"
#include "../basic/rect.h"
#include "zguicontrol.h"

class ZGuiRender;
class ZGuiLabel;
class ZGuiListbox;
class ZGuiListitem;

class GUI_API ZGuiCombobox : public ZGuiControl
{
public:
	ZGuiCombobox(Rect kRectangle, ZGuiWnd* pkParent=NULL, 
		bool bVisible=true, int iID=0, int iItemHeight=20, 
		ZGuiSkin* pkSkinItem=NULL, ZGuiSkin* pkSkinItemSelected=NULL, 
		ZGuiSkin *pkSkinItemHighLight=NULL, ZGuiSkin* pkTopItemSkin=NULL);

	virtual ~ZGuiCombobox();

	virtual bool Render( ZGuiRender* renderer );
	void SetScrollbarSkin(ZGuiSkin* pkSkinScrollArea, ZGuiSkin* pkSkinThumbButton,
		ZGuiSkin* pkSkinThumbButtonHighLight);
	void SetSkin(ZGuiSkin* pkSkin); // ersätter ZGuiWnd::SetSkin...
	bool Notify(ZGuiWnd* pkWnd, int iCode);
	bool AddItem(char* strText, int iIndex, bool bSelect=true); // if index is -1 it set index to getitemcount+1
	void RemoveSelItem(bool bSelectPrev=true);
	void SetLabelText(char* strText);
	void SetLabelWidth(int iWidth);
	void IsMenu(bool bIsMenu);
	const bool IsMenu() { return m_bIsMenu; }
	void SetNumVisibleRows(unsigned short iNumVisibleRows);
	bool RemoveAllItems(); // välj denna istället för den i listboxen annars rensas inte textboxen...
	void GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const; // overloaded
	void SetZValue(int iValue); // överlagrad
	void Resize(int Width, int Height, bool bChangeMoveArea=true); // overloaded
	ZGuiListbox* GetListbox() { return m_pkListbox; }
	const int GetNumVisibleRows() { return m_unNumVisibleRows; }
	void CopyNonUniqueData(const ZGuiWnd* pkSrc);

private:
	ZGuiLabel* m_pkLabel;
	ZGuiListbox* m_pkListbox;
//	ZGuiSkin m_kDropDownIcon;
	bool m_bIsMenu;
	unsigned short m_unNumVisibleRows;
};

#endif 



