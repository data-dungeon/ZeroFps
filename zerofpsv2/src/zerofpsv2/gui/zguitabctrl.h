// zguitabctrl.h: interface for the ZGuiTabCtrl class.

#ifndef _GUI_ZGUITABCTRL_H
#define _GUI_ZGUITABCTRL_H

#include "gui_x.h"
#include "zguiwindow.h"
#include "zguibutton.h"

/** \brief	A Gui tab control (a window with multiple pages that the user can select by the corresponding tab).
	 \ingroup Gui
*/
class GUI_API ZGuiTabCtrl : public ZGuiWnd 
{
public:
	void OnChangeWndText(ZGuiWnd* pkWnd);
	ZGuiTabCtrl(Rect kRect, ZGuiWnd* pkParent, bool bVisible, int iID);
	virtual ~ZGuiTabCtrl();

	bool InsertPage(char* szResWndName, unsigned int iIndex, 
		char* szTabText, unsigned int iTabSkinIndex);
	bool DeletePage(unsigned int iIndex);
	bool DeleteAllPages();
	bool InsertTabSkin(unsigned int iIndex, ZGuiSkin* pkSkin);

	ZGuiWnd* GetPage(unsigned int index);

	void SetCurrentPage(unsigned int index);
	unsigned int GetCurrentPage();
	unsigned int GetNumPages();

	void SetNextButtonSkin(ZGuiSkin* pkUp, ZGuiSkin* pkDown, ZGuiSkin* pkFocus);
	void SetPrevButtonSkin(ZGuiSkin* pkUp, ZGuiSkin* pkDown, ZGuiSkin* pkFocus);
	void SetFont(ZGuiFont* pkFont);

	//bool Rescale(int iOldWidth, int iOldHeight, int iNewWidth, int iNewHeight);

	void GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const; // overloaded

	void Resize(int Width, int Height, bool bChangeMoveArea=true); // overloaded

	ZGuiButton* GetTabFromPage(ZGuiWnd* pkPage);

   ZGuiButton* GetTabButton(bool bNextTabBn) { if(bNextTabBn) return m_pkNextTabBn; else return m_pkPrevTabBn; }

	void SetTabColor(float afColor[3], bool bSelTab);

protected:
	bool Notify(ZGuiWnd* pkWnd, int iCode);

private:
	bool SendNotifyMessageTabCtrl(int iType, int iParams, void* pMsg);
	void CreateInternalControls();
	bool SetButtonSkin(ZGuiButton* pkButton, unsigned int index);
	void ShowPage(unsigned int index, bool bVisible);
	bool MoveTabs(bool bLeft);
	
	list<ZGuiWnd*> m_kPageList;
	list<ZGuiButton*> m_kTabList;
	list<ZGuiSkin*> m_kTabSkinList;

	float m_afSelTabTextColor[3];
	float m_afUnSelTabTextColor[3];

	ZGuiButton* m_pkNextTabBn, *m_pkPrevTabBn;
	ZGuiSkin* m_pkNextEndSkin, *m_pkPrevEndSkin;

	unsigned int m_uiCurrentPage;
	static unsigned int s_uiPageIDCounter;
	static unsigned int s_uiTabIDCounter;

	unsigned int m_uiMarg;
	unsigned int m_uiTabHeight;

	int m_iPrevCurrentPage;
};

#endif // #ifndef _GUI_ZGUITABCTRL_H






