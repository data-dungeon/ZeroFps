// Check box.h: interface for the Checkbox class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _GUI_ZGUICHECKBOX_H_
#define _GUI_ZGUICHECKBOX_H_

#include "gui_x.h"
#include "../basic/rect.h"
#include "zguiwindow.h"

class ZGuiRender;
class ZGuiLabel;

class GUI_API ZGuiCheckbox : public ZGuiWnd
{
public:
	ZGuiCheckbox(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, int iID=0);
	virtual ~ZGuiCheckbox();

	void CopyNonUniqueData(const ZGuiWnd* pkSrc);
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
	bool m_bChecked;
};

#endif



