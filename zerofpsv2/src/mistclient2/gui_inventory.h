#ifndef _GUI_INVENTORY_H_
#define _GUI_INVENTORY_H_

#include "../zerofpsv2/basic/vector2.h"
#include "../zerofpsv2/basic/zguiskin.h"
#include "../mcommon/p_container.h"

class TextureManager;
class ZGuiWnd;
class ZGuiLabel;
class Entity;

#include <vector>
#include <string>
using namespace std;

enum InventoryDropTarget
{
	DropTarget_Inventory,
	DropTarget_Container,
	DropTarget_Ground,
	DropTarget_EquipmentSlot,
};

struct ITEM_INFO 
{
	string	strInfo;
	string	strImage;
	int		m_iValue;
	float		m_fWeight;
};

class InventoryDlg
{
public:
	InventoryDlg();
	~InventoryDlg();

	void OnDoubleClick(int iMouseX, int iMouseY);

	void UpdateInventory(vector<MLContainerInfo>& vkItemList);
	void UpdateContainer(vector<MLContainerInfo>& vkItemList);

	void Open();
	void Close();
	void OnCommand(string strController);
	void OnMouseMove(bool bLeftButtonPressed, int x, int y);
	bool IsVisible() 
	{ 
		if(m_pkInventoryWnd == NULL) 
			return false; 
		return m_pkInventoryWnd->IsVisible(); 
	}
	
	void OpenItemInfoWnd(bool bOpen, ITEM_INFO kInfo);
	int GetInventoryContainerID();
	int GetActiveContainerID();
	InventoryDropTarget GetDropTargetFromScreenPos(int x, int y);
	void OpenContainerWnd(int id, char slots_x, char slots_y);
	void CloseContainerWnd();
	void CloseSplitStockWnd(bool bExecuteSplit=false);
	Point SlotFromScreenPos(int x, int y, bool bInventory);

	int m_iItemUnderCursor; // används av inventoryt för att avgöra vilket item som finns under 
									// cursorn efter att ha klickat på ett object i världen.

	int m_iActiveContainerID;

	ZGuiWnd* m_pkSplitStockWnd;
	int m_iSplitShareMax;

private:

	struct ITEM_SLOT
	{	
		ZGuiWnd* pkWnd;
		int iItemID;
		bool bIsContainer;
		int iItemType;
		int iStackSize;
		string strName;
	};

	struct SPECIAL_SLOT
	{
		int m_iIndex; // index in item list (m_vkInventoryItemList or m_vkContainerItemList)
		bool bIsInventoryItem;
	};

	void OnDropItem(int mx, int my);
	void CreateContainerGrid();
	void RebuidContainerGrid(char slots_horz, char slots_vert);
	int TestForCollision(int iTestSlot, bool bInventory); // returns -1 if no collision or index to list eitherwise
	int TestForCollision(Point test_slot, Point test_size, bool bInventory); // returns -1 if no collision or index to list eitherwise
	
	Point SlotFromWnd(ZGuiWnd* pkWnd, bool bInventory);
	Point SlotSizeFromWnd(ZGuiWnd* pkWnd);
	pair<int, bool> GetItemFromScreenPos(int x, int y); // Returns a index and a bool, 
																		 // if the bool is true then its a index 
																		 // into m_vkInventoryItemList.
	void PickUpFromGround(bool bLeftButtonPressed, int mx, int my);
	void PickUpFromGrid(int iSlotIndex, bool bInventory, int mx, int my);
	void OpenContainerItem(bool bOpen, int iSlotIndex, bool bInventory);
	
	void SetSelectionBorder(int iIndex, bool bInventory, bool bRemove);
	void OpenSplitStockWnd();
	
	TextureManager* m_pkTexMan;
	
	vector<ITEM_SLOT> m_vkInventoryItemList;
	vector<ITEM_SLOT> m_vkContainerItemList;

	ZGuiWnd* m_pkInventoryWnd;
	ZGuiWnd* m_pkContainerWnd;

	SPECIAL_SLOT m_kMoveSlot; 
	SPECIAL_SLOT m_kSplitSlot; 
	SPECIAL_SLOT m_kSplitSlotTarget; 

	pair<int, bool> m_kItemUnderInspection; // 1:st arg: index into that array (or -1 if no item under inspection).
														 // 2:nd arg: 1 = m_vkInventoryItemList, 0: m_vkContainerItemList.

	int m_iSelItemID; // ITEM_SLOT::iItemID (aka MLContainerInfo::m_iItemID)
	int m_iHighestZ;

	bool m_bDoubleClicked;

	float m_fPickUpTimer;

	int m_iSlotsHorzContainer;
	int m_iSlotsVertContainer;

	ZGuiSkin** m_ppCursorSkin;
	vector<ZGuiWnd*> m_vkContainerGridSlots;

	Point m_kCursorRangeDiff;
	
	Point m_kItemWndPosBeforeMove; // fönster koordinater
	
	const int ICON_WIDTH; // in pixels
	const int ICON_HEIGHT; // in pixels

	const Point UPPERLEFT_INVENTORY; // in pixels
	const Point UPPERLEFT_CONTAINER; // in pixels

	const int SLOTTS_HORZ_INVENTORY; 
	const int SLOTTS_VERT_INVENTORY; 

	const float BD_R;
	const float BD_G;
	const float BD_B;
};

#endif
