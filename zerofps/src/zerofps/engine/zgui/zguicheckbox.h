// Check box.h: interface for the Checkbox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHECKBOX_H__03DB5F8B_D30C_489B_8732_689D12D1B549__INCLUDED_)
#define AFX_CHECKBOX_H__03DB5F8B_D30C_489B_8732_689D12D1B549__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\engine_x.h"
#include "..\..\basic\rect.h"
#include "zguicontrol.h"

class ZGuiRender;
class ZGuiLabel;

class ENGINE_API ZGuiCheckbox : public ZGuiControl
{
public:
	ZGuiCheckbox(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, int iID=0);
	virtual ~ZGuiCheckbox();

	void SetButtonCheckedSkin(ZGuiSkin* pkSkin, int iMaskTexture);
	void SetButtonUncheckedSkin(ZGuiSkin* pkSkin, int iMaskTexture);
	bool Notify(ZGuiWnd* pkWnd, int iCode);
	bool Render( ZGuiRender* renderer );

	ZGuiSkin* GetCheckedSkin() { return m_pkSkinButtonChecked; }
	ZGuiSkin* GetUncheckedSkin() { return m_pkSkinButtonUnchecked; }
	int GetTexUnchecked() { return m_iMaskTexUnchecked; }
	int GetTexChecked() { return m_iMaskTexChecked; }

	void CheckButton();
	void UncheckButton();
	void SetTextSkin(ZGuiSkin* kSkin, int iMaskTexture = -1);
	void SetText(char* strText);

protected:
	ZGuiSkin* m_pkSkinButtonChecked, *m_pkSkinButtonUnchecked;
	ZGuiLabel* m_pkLabel;
	int m_iMaskTexUnchecked, m_iMaskTexChecked;
	bool m_bChecked;
};

#endif // !defined(AFX_CHECKBOX_H__03DB5F8B_D30C_489B_8732_689D12D1B549__INCLUDED_)
