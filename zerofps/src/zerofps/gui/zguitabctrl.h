// zguitabctrl.h: interface for the ZGuiTabCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZGUITABCTRL_H__B5B9295C_AB42_4D6F_A1AA_6F2C473C0EE5__INCLUDED_)
#define AFX_ZGUITABCTRL_H__B5B9295C_AB42_4D6F_A1AA_6F2C473C0EE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gui_x.h"
#include "zguiwindow.h"
#include "zguibutton.h"

class GUI_API ZGuiTabCtrl : public ZGuiWnd 
{
public:
	ZGuiTabCtrl(Rect kRect, ZGuiWnd* pkParent, bool bVisible, int iID);
	virtual ~ZGuiTabCtrl();

	bool InsertPage(unsigned int iIndex, char* szTabText, unsigned int iTabSkinIndex);
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

protected:
	bool Notify(ZGuiWnd* pkWnd, int iCode);

private:
	void CreateInternalControls();
	bool SetButtonSkin(ZGuiButton* pkButton, unsigned int index);
	void ShowPage(unsigned int index, bool bVisible);
	bool MoveTabs(bool bLeft);
	
	list<ZGuiWnd*> m_kPageList;
	list<ZGuiButton*> m_kTabList;
	list<ZGuiSkin*> m_kTabSkinList;

	ZGuiButton* m_pkNextTabBn, *m_pkPrevTabBn;
	ZGuiSkin* m_pkNextEndSkin, *m_pkPrevEndSkin;

	unsigned int m_uiCurrentPage;
	static unsigned int s_uiPageIDCounter;
	static unsigned int s_uiTabIDCounter;

	const unsigned int MARG;
	const unsigned int TAB_HEIGHT;
};

#endif // !defined(AFX_ZGUITABCTRL_H__B5B9295C_AB42_4D6F_A1AA_6F2C473C0EE5__INCLUDED_)
