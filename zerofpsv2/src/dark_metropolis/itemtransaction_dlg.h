#pragma once
#include "gamedlg.h"

class CItemTransactionDlg : public CGameDlg
{
public:
	CItemTransactionDlg();
	~CItemTransactionDlg();

	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName, bool bRMouseBnClick);
	bool InitDlg();

	enum VIEW_MODE { store_room, shop };
	VIEW_MODE m_eViewMode; 
	
private:

	struct CONTAINER_INFO
	{
		DMContainer* pkContainer;
		Rect m_rcItemContainer;
	};

	struct ITEM_MOVE_INFO
	{
		CONTAINER_INFO m_kFromContainer;
		ZGuiCheckbox* m_pkMoveButton;
		int* m_pMoveObject;
		Rect m_rcBeforeMove;
	};
	
	enum eActiveContainer
	{
		BACKPACK = 0,
		ARMOR = 1,
		CYBERNETICS = 2,
		QUICKITEM = 3,
		WEAPON = 4
	};

	void MoveItemToStockroom(ITEM_MOVE_INFO kItem);
	void MoveItemToShop(ITEM_MOVE_INFO kItem);

	void MoveItemFromInventory(ITEM_MOVE_INFO kItem);

	char* GetGetContainerName(int iContainer);

	void PrintItemInfo(ITEM_MOVE_INFO* pkItem, bool bCharacterItem);

	bool Buy(int iItemIndex);
	bool Sell(int iItemIndex);
	
	int m_iActiveContainer;
	int m_iSelFocusCharItemIndex;
	int m_iSelInventoryItemIndex;
	
	vector<ITEM_MOVE_INFO> m_vkFocusCharItems;
	vector<ITEM_MOVE_INFO> m_vkInventoryItems;

};
