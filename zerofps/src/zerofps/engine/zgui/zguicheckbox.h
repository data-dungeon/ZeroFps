// Check box.h: interface for the Checkbox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHECKBOX_H__03DB5F8B_D30C_489B_8732_689D12D1B549__INCLUDED_)
#define AFX_CHECKBOX_H__03DB5F8B_D30C_489B_8732_689D12D1B549__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../engine_x.h"
#include "../../basic/rect.h"
#include "zguicontrol.h"

class ZGuiRender;
class ZGuiLabel;

class ENGINE_API ZGuiCheckbox : public ZGuiControl
{
public:
	ZGuiCheckbox(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, int iID=0);
	virtual ~ZGuiCheckbox();

	void SetButtonCheckedSkin(ZGuiSkin* pkSkin);
	void SetButtonUncheckedSkin(ZGuiSkin* pkSkin);
	void SetLabelSkin(ZGuiSkin* pkSkin);
	void Resize(int Width, int Height, bool bChangeMoveArea=true); // overloaded

	bool Notify(ZGuiWnd* pkWnd, int iCode);
	bool Render( ZGuiRender* renderer );

	ZGuiSkin* GetCheckedSkin() { return m_pkSkinBnDown; }
	ZGuiSkin* GetUncheckedSkin() { return m_pkSkinBnUp; }
	char* GetText();// overloaded
	void GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const; // overloaded

	void CheckButton();
	void UncheckButton();
	void SetText(char* strText, bool bResizeWnd=false); // overloaded

	bool IsChecked();

protected:
	ZGuiSkin* m_pkSkinBnDown, *m_pkSkinBnUp;
	ZGuiLabel* m_pkLabel;
	//int m_iMaskTexUnchecked, m_iMaskTexChecked;
	bool m_bChecked;
};

#endif // !defined(AFX_CHECKBOX_H__03DB5F8B_D30C_489B_8732_689D12D1B549__INCLUDED_)



