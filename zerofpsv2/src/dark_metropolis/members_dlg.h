#pragma once
#include "gamedlg.h"

class CMembersDlg : public CGameDlg
{
public:
	CMembersDlg();
	~CMembersDlg();

	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName);

	enum WINDOW_MODE // varifrån fönstret öppnades
	{
		// Klickat på folder knappen för att equipa föremål från högkvarteret.
		HQ_EQUIP_MEMBERS,	

		// Klickat på telephone button to browse members to fire and 
		// available agents for hiring
		HQ_BROWSE_MEMBERS_AND_AGENTS_AVAILABLE_FOR_HIRING,	

		// Har klickat på agentens ikon i ingamepanelen för att 
		// kolla på info om ens agent, equipa andra vapen etc.
		IN_GAME	
	};

	void SetWindowMode(WINDOW_MODE eType);
	const WINDOW_MODE GetWindowMode() { return m_eWidowMode; }

private:

	WINDOW_MODE m_eWidowMode;

	void SwitchCharacter(bool bNext);
	void SetCharacterStats(Entity* pkCharacterObject);

	int m_iCurrentCharacterPage;

	vector<Entity*> m_kMembersInField;

	// void AddSlot(int 
};
