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

class InventoryDlg
{
public:
	InventoryDlg();
	~InventoryDlg();

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
	
	void OpenContainerWnd(int id, char slots_x, char slots_y);
	void CloseContainerWnd();

	int m_iItemUnderCursor; // används av inventoryt för att avgöra vilket item som finns under 
									// cursorn efter att ha klickat på ett object i världen.

	int m_iActiveContainerID;
	
private:

	enum InventoryDropTarget
	{
		DropTarget_Inventory,
		DropTarget_Container,
		DropTarget_Ground,
	};

	struct ITEM_SLOT
	{	
		ZGuiWnd* pkWnd;
		int iItemID;
		bool bIsContainer;
	};

	struct MOVE_SLOT
	{
		int m_iIndex; // index in item list (m_vkInventoryItemList or m_vkContainerItemList)
		bool bIsInventoryItem;
	};

	void OnDropItem(int mx, int my);
	void CreateContainerGrid(char slots_horz, char slots_vert);
	int TestForCollision(int iTestSlot, bool bInventory); // returns -1 if no collision or index to list eitherwise
	int TestForCollision(Point test_slot, Point test_size, bool bInventory); // returns -1 if no collision or index to list eitherwise
	Point SlotFromScreenPos(int x, int y, bool bInventory);
	Point SlotFromWnd(ZGuiWnd* pkWnd, bool bInventory);
	Point SlotSizeFromWnd(ZGuiWnd* pkWnd);
	pair<int, bool> GetItemFromScreenPos(int x, int y); // Returns a index and a bool, 
																		 // if the bool is true then its a index 
																		 // into m_vkInventoryItemList.
	void PickUpFromGround(bool bLeftButtonPressed, int mx, int my);
	void PickUpFromGrid(int iSlotIndex, bool bInventory, int mx, int my);
	void OpenContainerItem(bool bOpen, int iSlotIndex, bool bInventory);
	int GetInventoryContainerID();
	InventoryDropTarget GetDropTargetFromScreenPos(int x, int y);
	void SetSelectionBorder(int iIndex, bool bInventory, bool bRemove);

	TextureManager* m_pkTexMan;
	
	vector<ITEM_SLOT> m_vkInventoryItemList;
	vector<ITEM_SLOT> m_vkContainerItemList;

	ZGuiWnd* m_pkInventoryWnd;
	ZGuiWnd* m_pkContainerWnd;

	MOVE_SLOT m_kMoveSlot; // index of m_vkInventoryItemList or m_vkContainerItemList
	int m_iSelItemID; // ITEM_SLOT::iItemID (aka MLContainerInfo::m_iItemID)
	int m_iHighestZ;

	float m_fPickUpTimer;

	int m_iSlotsHorzContainer;
	int m_iSlotsVertContainer;

	ZGuiSkin** m_ppCursorSkin;

	Point m_kCursorRangeDiff;
	
	Point m_kItemWndPosBeforeMove; // fönster koordinater
	
	const int ICON_WIDTH;
	const int ICON_HEIGHT;

	const int SLOTTS_HORZ_INVENTORY;
	const int SLOTTS_VERT_INVENTORY;

	const Point UPPER_LEFT_INVENTORY;
	const Point UPPER_LEFT_CONTAINER;

	const float BD_R;
	const float BD_G;
	const float BD_B;
};

#endif
