// Combobox.h: interface for the Combobox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMBOBOX_H__942928D7_8B6B_421F_B821_742984902F7D__INCLUDED_)
#define AFX_COMBOBOX_H__942928D7_8B6B_421F_B821_742984902F7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../engine_x.h"
#include "../../basic/rect.h"
#include "zguicontrol.h"

class ZGuiRender;
class ZGuiLabel;
class ZGuiListbox;
class ZGuiListitem;

class ENGINE_API ZGuiCombobox : public ZGuiControl
{
public:
	ZGuiCombobox(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, int iID=0, int iItemHeight=20, 
		ZGuiSkin* pkTextSkin=NULL, int iTextureMask=-1, ZGuiSkin* pkSkinItem=NULL, ZGuiSkin* pkSkinItemSelected=NULL, 
		ZGuiSkin *pkSkinItemHighLight=NULL, ZGuiSkin* pkTopItemSkin=NULL, int iTopItemTextureMask=-1);
	virtual ~ZGuiCombobox();

	virtual bool Render( ZGuiRender* renderer );
	void SetScrollbarSkin(ZGuiSkin* pkSkinScrollArea, ZGuiSkin* pkSkinThumbButton,
		ZGuiSkin* pkSkinThumbButtonHighLight);
	void SetSkin(ZGuiSkin* pkSkin); // ersätter ZGuiWnd::SetSkin...
	bool Notify(ZGuiWnd* pkWnd, int iCode);
	bool AddItem(char* strText, unsigned int iID);
	void SetLabelText(char* strText);
	void SetLabelWidth(int iWidth);
	void DisableLabelText(bool bDisable=true);
	void SetNumVisibleRows(unsigned short iNumVisibleRows);

	ZGuiListitem* GetItem(int iID);

	ZGuiListbox* GetListbox() { return m_pkListbox; }

private:
	ZGuiLabel* m_pkLabel;
	ZGuiListbox* m_pkListbox;
//	ZGuiSkin m_kDropDownIcon;
	bool m_pkLabelIsDisabled;
	unsigned short m_unNumVisibleRows;
};

#endif // !defined(AFX_COMBOBOX_H__942928D7_8B6B_421F_B821_742984902F7D__INCLUDED_)



