// Button.h: interface for the Button class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _GUI_ZGUIBUTTON_H_
#define _GUI_ZGUIBUTTON_H_


#include "gui_x.h"
#include "../basic/rect.h"
#include "zguicontrol.h"

class ZGuiRender;

class GUI_API ZGuiButton : public ZGuiControl 
{
public:
	ZGuiButton(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, int iID=0);
	virtual ~ZGuiButton();

	void SetButtonUpSkin(ZGuiSkin* kSkin);
	void SetButtonDownSkin(ZGuiSkin* kSkin);
	void SetButtonHighLightSkin(ZGuiSkin* kSkin);
	bool Render( ZGuiRender* renderer );
	ZGuiSkin* GetButtonUpSkin() { return m_pkSkinBnUp; }
	ZGuiSkin* GetButtonDownSkin() { return m_pkSkinBnDown; }
	void GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const;

protected:
	bool Notify(ZGuiWnd* pkWnd, int iCode);
	ZGuiSkin* m_pkSkinBnUp, *m_pkSkinBnDown, *m_pkSkinBnHLight;
};

#endif 


