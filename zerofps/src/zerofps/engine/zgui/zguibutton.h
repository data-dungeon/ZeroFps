// Button.h: interface for the Button class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUTTON_H__1656E751_9931_4A8F_A55D_907CEEA8E99F__INCLUDED_)
#define AFX_BUTTON_H__1656E751_9931_4A8F_A55D_907CEEA8E99F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../engine_x.h"
#include "../../basic/rect.h"
#include "zguicontrol.h"

class ZGuiRender;

class ENGINE_API ZGuiButton : public ZGuiControl 
{
public:
	ZGuiButton(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, int iID=0);
	virtual ~ZGuiButton();

	void SetButtonUpSkin(ZGuiSkin* kSkin, int iMaskTexture=-1);
	void SetButtonDownSkin(ZGuiSkin* kSkin);
	void SetButtonHighLightSkin(ZGuiSkin* kSkin);
	bool Render( ZGuiRender* renderer );
	ZGuiSkin* GetButtonUpSkin() { return m_kSkinUp; }
	ZGuiSkin* GetButtonDownSkin() { return m_kSkinDown; }

protected:
	bool Notify(ZGuiWnd* pkWnd, int iCode);
	ZGuiSkin* m_kSkinUp, *m_kSkinDown, *m_kSkinHighLight;
	int m_iMaskTextureUp;
};

#endif // !defined(AFX_BUTTON_H__1656E751_9931_4A8F_A55D_907CEEA8E99F__INCLUDED_)


