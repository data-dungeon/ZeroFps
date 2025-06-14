// zguislider.h: interface for the ZGuiSlider class.

#ifndef _GUI_ZGUISLIDER_H
#define _GUI_ZGUISLIDER_H

#include "gui_x.h"
#include "zguiwindow.h"

class ZGuiButton;
class ZGuiLabel;
class ZGuiSkin;

#define SCF_HORZ				1
#define SCF_VERT				2
#define SCF_SLIDER_CENTER	4
#define SCF_SLIDER_BOTTOM	8
#define SCF_SLIDER_TOP		16

/** \brief	A slider control (lets a user select from a range of values by moving a slider).
	 \ingroup Gui
*/
class GUI_API ZGuiSlider : public ZGuiWnd
{
public:
	ZGuiSlider(Rect kRect, ZGuiWnd* pkParent, bool bVisible, int iID,
		int iMin=0, int iMax=10, unsigned long ulStyle=SCF_HORZ|SCF_SLIDER_CENTER);
	virtual ~ZGuiSlider();

	void AddBuddyWindow(ZGuiWnd* pkWnd);
	void RemoveBuddyWindow(ZGuiWnd* pkWnd);
	void RemoveAllBuddys();

	ZGuiButton* GetButton();
	void SetPos(int iPos, bool bRedraw, bool bUpdateBuddy=true);
	void SetRange(int iMin, int iMax);
	void SetCtrlStyle(unsigned long ulNewStyle); // replaces the previus
	void SetSliderSkin(ZGuiSkin* pkSkin);
	void SetBkSkin(ZGuiSkin* pkSkin);
	ZGuiSkin* GetBkSkin();
	
	int  GetPos();
	void GetRange(int& riMin, int& riMax);
	bool GetCtrlStyle(unsigned long ulValue);
	
	bool ProcessKBInput(int nKey);

	void GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const; // overloaded

	void Resize(int iWidth, int iHeight, bool bChangeMoveArea);
	
protected:
	bool Notify(ZGuiWnd* pkWnd, int iCode);

private:
	void CreateInternalControls();

	ZGuiButton* m_pkButton;
	ZGuiLabel* m_pkLabel;
	int m_iMax, m_iMin, m_iPos;
	unsigned long m_uiCtrlStyle;
	vector<ZGuiWnd*> m_akBuddys;

};

#endif // #ifndef _GUI_ZGUISLIDER_H
