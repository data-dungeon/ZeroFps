// zguiscrollbar.h: interface for the ZGuiScrollbar class.

#ifndef _GUI_ZGUISCROLLBAR_H_
#define _GUI_ZGUISCROLLBAR_H_

#include "gui_x.h"
#include "../basic/rect.h"
#include "zguiwindow.h"

class ZGuiButton;

const int SCROLLUP_ID = 521;
const int SCROLLDOWN_ID = 522;

/** \brief	A Gui Scrollbar
	 \ingroup Gui
*/
class GUI_API ZGuiScrollbar : public ZGuiWnd
{
public:
	
	int GetArrowButtonHeight();
	int GetArrowButtonWidth();

	void SetScrollPos(unsigned int pos);
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

	void SetScrollButtonUpSkins(ZGuiSkin* pkSkinUp, ZGuiSkin* pkSkinDown);
	void SetScrollButtonDownSkins(ZGuiSkin* pkSkinUp, ZGuiSkin* pkSkinDown);

	//bool Rescale(int iOldWidth, int iOldHeight, int iNewWidth, int iNewHeight);
	
private:

	ZGuiButton* m_pkThumbButton;
	ZGuiButton* m_pkArrowBnUp;
	ZGuiButton* m_pkArrowBnDown;
	bool m_bHorzintal;
	bool m_bAutoHideScrollbar;
	unsigned int m_nMax, m_nMin, m_nPos, m_usThumbSize;
	float m_fPageSize;
	int m_iScrollPosXBefore, m_iScrollPosYBefore;
	
};

#endif 



