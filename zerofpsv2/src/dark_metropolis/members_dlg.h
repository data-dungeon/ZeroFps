#pragma once
#include "gamedlg.h"

class CMembersDlg : public CGameDlg
{
public:
	CMembersDlg();
	~CMembersDlg();

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
	void UpdateCamera();

protected:
	void OnMouseMove(int x, int y, bool bMouseDown, ZGuiWnd *pkMain);
	void OnClick(int x, int y, bool bMouseDown, bool bLeftButton, ZGuiWnd *pkMain);
	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName);

private:

	Camera* m_pkModellCamera;
	Entity*	m_pkCameraObject;
	ZeroFps* m_pkFps;

	WINDOW_MODE m_eWidowMode;

	void SwitchCharacter(bool bNext);
	void SetCharacterStats(Entity* pkCharacterObject);
	void OnEquip(int iItemID, DMContainer* pkDestContainer);

	int m_iCurrentCharacterPage;
	int m_iActiveCharacteInBase;
	
	vector<Entity*> m_kMembersInField;

	vector<ZGuiButton*> m_vkItemButtons;
	ZGuiButton* m_pkMoveButton;
	int* m_pMoveObject;

	int m_iCursorRangeDiffX, m_iCursorRangeDiffY;

	Rect m_rcItemContainer;
	Rect m_rcArmorContainer;
	Rect m_rcCyberneticsContainer;
	Rect m_rcWeaponContainer;
	Rect m_rcQuickItemContainer;

	struct CONTAINER_INFO
	{
		DMContainer* pkContainer;
		Rect m_rcItemContainer;
	};

	struct ITEM_MOVE_INFO
	{
		CONTAINER_INFO m_kFromContainer;
		ZGuiButton* m_pkMoveButton;
		int* m_pMoveObject;
		Rect m_rcBeforeMove;
	};

	ITEM_MOVE_INFO* m_pkMoveInfo;
	ITEM_MOVE_INFO* m_pkSelectInfo;

	bool GetContainer(int x, int y, CONTAINER_INFO& kContainer, int agent_obj_id);
	bool GetItemPosFromCursor(int x, int y, int& itempos_x, 
		int& itempos_y, int& slotpos_x, int& slotpos_y);
	void DropItem(ITEM_MOVE_INFO* pkObject);
	void UpdateInventory(Entity* pkCharacterObject);
	void UpdateLevelbar(Entity* pkCharacterObject);
	void CreateCamera();
};
