// zguiscrollbar.h: interface for the ZGuiScrollbar class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _GUI_ZGUISCROLLBAR_H_
#define _GUI_ZGUISCROLLBAR_H_

#include "gui_x.h"
#include "../basic/rect.h"
#include "zguicontrol.h"

class ZGuiButton;

class GUI_API ZGuiScrollbar : public ZGuiControl
{
public:
	bool AutoHideOn();
	void SetAutoHide(bool bActivate);
	void ToogleHorizontal();
	void GetScrollInfo(unsigned int& min, unsigned int& max, unsigned int& pos);
	void SetScrollInfo(unsigned int min, unsigned int max, float page_size, 
		unsigned int pos); // page size är i procent och skall vara ett värde 
						   // mellan 0-1
	ZGuiScrollbar(Rect kRectangle, ZGuiWnd* pkParent, bool bVisible, int iID);
	virtual ~ZGuiScrollbar();

	void SetZValue(int iValue);
	void CreateInternalControls();
	bool Render( ZGuiRender* renderer );
	void SetThumbButtonSkins(ZGuiSkin* pkSkinNormal, ZGuiSkin* pkSkinHighLight);
	bool Notify(ZGuiWnd* pkWnd, int iCode);
	int GetPos() { return m_nPos; }
	void GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const; // overloaded
	void Resize(int Width, int Height, bool bChangeMoveArea=true); // overloaded
	int m_iScrollChange;

	ZGuiButton* GetButton() { return m_pkThumbButton; }
	void CopyNonUniqueData(const ZGuiWnd* pkSrc);

private:
	ZGuiButton* m_pkThumbButton;
	bool m_bHorzintal;
	bool m_bAutoHideScrollbar;
	unsigned int m_nMax, m_nMin, m_nPos, m_usThumbSize;
	
};

#endif 



