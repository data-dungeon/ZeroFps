#pragma once
#include "gamedlg.h"

class CMembersDlg : public CGameDlg
{
public:
	CMembersDlg();
	~CMembersDlg();

	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName);

	enum WINDOW_MODE // varifr�n f�nstret �ppnades
	{
		// Klickat p� folder knappen f�r att equipa f�rem�l fr�n h�gkvarteret.
		HQ_EQUIP_MEMBERS,	

		// Klickat p� telephone button to browse members to fire and 
		// available agents for hiring
		HQ_BROWSE_MEMBERS_AND_AGENTS_AVAILABLE_FOR_HIRING,	

		// Har klickat p� agentens ikon i ingamepanelen f�r att 
		// kolla p� info om ens agent, equipa andra vapen etc.
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
