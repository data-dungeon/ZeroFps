#pragma once

#include "dark_metropolis.h"

class CGameDlg
{
public:
	CGameDlg(DarkMetropolis* pkDM);
	~CGameDlg(void);

	virtual void OnCommand(ZGuiWnd *pkMainWnd, string strClickName) {};
	//virtual void OnClick(int x, int y, bool bMouseDown, bool bLeftButton, ZGuiWnd *pkMain);
	//virtual void OnDoubleClick(int x, int y, bool bLeftButton, ZGuiWnd *pkMain);
	//virtual void OnMouseMove(int x, int y, bool bMouseDown, ZGuiWnd *pkMain);
	//virtual void OnScroll(int iID, int iPos, ZGuiWnd *pkMain);
	//virtual void OnSelectCB(int ListBoxID, int iItemIndex, ZGuiWnd *pkMain);
	//virtual void OnKeyPress(int iKey, ZGuiWnd *pkWnd);
	//virtual void OnSelectLB(int iID, int iIndex, ZGuiWnd* pkMainWnd);

	void ShowWnd(char* szWndResName, bool bShow) { m_pkDM->ShowWnd(szWndResName, bShow); }
	bool IsWndVisible(char* szWndResName) { return m_pkDM->IsWndVisible(szWndResName); } 
	bool LoadDlg(char* szScriptName) { return m_pkDM->LoadGuiFromScript(m_pkDM->m_pkScript, szScriptName); }
	void ClearListbox(char* szName) { m_pkDM->ClearListbox(szName); }
	void AddListItem(char* szListbox, char* szItem) { m_pkDM->AddListItem(szListbox, szItem); }
	void SetText(char* szWndName, char* szText) { m_pkDM->SetText(szWndName, szText); }
	void SelListItem(char* szWndName, char* szText) { m_pkDM->SelListItem(szWndName, szText); }
	ZGuiWnd* GetWnd(char* szName) { return m_pkDM->GetWnd(szName); }
	ZGuiWnd* CreateWnd(GuiType eType, char* szWndName, char* szParentName, 
		char* szLabel, int x, int y, int w, int h, unsigned long uiFlags) {
		m_pkDM->CreateWnd(eType, szWndName, szParentName, szLabel, x, y, w, h, uiFlags);
	}

	int GetTexID(char* szName);

	P_DMGameInfo* GetGameInfo() { return m_pkDM->m_pkGameInfoProperty; }

	DarkMetropolis* m_pkDM;
};
