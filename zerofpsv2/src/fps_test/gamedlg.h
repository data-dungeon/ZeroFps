#pragma once



class CGameDlg
{
public:
	CGameDlg(string strMainWndName, DarkMetropolis* pkDM);
	~CGameDlg(void);

	virtual void OnSelectCB(ZGuiCombobox* pkCombobox, int iItemIndex) {};
	virtual void OnClick(int x, int y, bool bMouseDown, bool bLeftButton, ZGuiWnd *pkMain) {}
	virtual void OnCommand(ZGuiWnd *pkMainWnd, string strClickName, bool bRMouseBnClick) {};
	virtual void OnMouseMove(int x, int y, bool bMouseDown, ZGuiWnd *pkMain) {}
	virtual void OnScroll(int iID, int iPos, ZGuiWnd *pkMain) {};
	virtual bool InitDlg() {return true;}
	virtual void OpenDlg(){};
	virtual void OnKeyPress(int iKey, ZGuiWnd* pkCurrentMainWnd) {};

	bool IsInitialized() { return m_bInitialized; }

	//virtual void OnClick(int x, int y, bool bMouseDown, bool bLeftButton, ZGuiWnd *pkMain);
	//virtual void OnDoubleClick(int x, int y, bool bLeftButton, ZGuiWnd *pkMain);
	//virtual void OnScroll(int iID, int iPos, ZGuiWnd *pkMain);
	//virtual void OnSelectCB(int ListBoxID, int iItemIndex, ZGuiWnd *pkMain);
	//virtual void OnKeyPress(int iKey, ZGuiWnd *pkWnd);
	//virtual void OnSelectLB(int iID, int iIndex, ZGuiWnd* pkMainWnd);

protected:

	void SetButtonIcon(ZGuiWnd* pkButton, string strIconName, bool bSetAlphaTex, 
		bool bUseBorder=false);
	void ShowWnd(char* szWndResName, bool bShow, bool bToogleCapture=false);
	bool IsWndVisible(char* szWndResName) { return m_pkDM->IsWndVisible(szWndResName); } 
	bool LoadDlg(char* szScriptName);
	void ClearListbox(char* szName) { m_pkDM->ClearListbox(szName); }
	void AddListItem(char* szListbox, char* szItem) { m_pkDM->AddListItem(szListbox, szItem); }
	void SetText(char* szWndName, char* szText) { m_pkDM->SetText(szWndName, szText); }
	void SetNumber(char* szWndName, float fNumber);
	void SetNumber(char* szWndName, int iNumber);
	void SelListItem(char* szWndName, char* szText) { m_pkDM->SelListItem(szWndName, szText); }
	ZGuiWnd* GetWnd(char* szName) { return m_pkDM->GetWnd(szName); }
	ZGuiWnd* CreateWnd(GuiType eType, char* szWndName, char* szParentName, 
		char* szLabel, int x, int y, int w, int h, unsigned long uiFlags) {
		return m_pkDM->CreateWnd(eType, szWndName, szParentName, szLabel, x, y, w, h, uiFlags);
	}
	void SetCapture(char* szWnd);

	P_DMGameInfo* GetGameInfo() { return m_pkDM->m_pkGameInfoProperty; }
	
	enum DM_OBJECT
	{
		HQ,
		GAME_INFO
	};


	Entity* GetObject(int iID){return m_pkDM->m_pkEntityManager->GetEntityByID(iID);}
	Entity* GetDMObject(DM_OBJECT eDmObject);
	void GetAllAgentsInField(vector<Entity*>& kList);

	DarkMetropolis* m_pkDM;
	ZGui* m_pkGui;
	ZFAudioSystem* m_pkAudioSys;
	TextureManager* GetTexMan() { return m_pkDM->Application::m_pkTexMan; }

	int GetTexID(char* szName);
	
	enum GAME_DLG
	{
		MISSION_DLG,
		GAMEPLAY_DLG,
		HANDLE_AGENTS_DLG,
		HQ_DLG,
		MEMBERS_DLG,
		NEWGAME_DLG,
		START_DLG,
		BRIEFING_DLG,
		ITEMTRANSACTION_DLG
	};

	CGameDlg* GetGameDlg(GAME_DLG eDlgType);

	bool m_bInitialized;

	string m_strMainWndName;
};
