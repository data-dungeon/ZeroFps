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

	void Update(vector<MLContainerInfo>& vkContainerList);

	void Open();
	void Close();
	void OnCommand(string strController);
	void OnMouseMove(bool bLeftButtonPressed, int x, int y);
	bool IsVisible() { if(m_pkInventoryWnd == NULL) return false; return m_pkInventoryWnd && m_pkInventoryWnd->IsVisible(); }
//	MLContainer* GetContainer();
	void OpenContainerWnd();
	void CloseContainerWnd();

	int m_iItemUnderCursor; // används av inventoryt för att avgöra vilket item som finns under 
									// cursorn efter att ha klickat på ett object i världen.
	
private:

	struct ITEM_SLOT
	{
		ZGuiWnd* pkWnd;
		int iItemID;
	};

	void OnDropItem();
	void CreateContainerGrid(int slots_horz, int slots_vert);
	bool TestForCollision(int iTestSlot);
	Point SlotFromScreenPos(int x, int y);
	Point SlotFromWnd(ZGuiWnd* pkWnd);
	Point SlotSizeFromWnd(ZGuiWnd* pkWnd);

	TextureManager* m_pkTexMan;

	vector<ITEM_SLOT> m_vkInventoryItemList;
	vector<ITEM_SLOT> m_vkContainerItemList;

	ZGuiWnd* m_pkInventoryWnd;
	ZGuiWnd* m_pkContainerWnd;

	int m_iMoveSlot; // index of m_vkItemList
	int m_iSelItemID; // ITEM_SLOT::iItemID (aka MLContainerInfo::m_iItemID)
	int m_iHighestZ;

	float m_fPickUpTimer;

	Point m_kPosBeforeMove;
	
	bool m_bGuiCaptureBeforOpen;

	const int ICON_WIDTH;
	const int ICON_HEIGHT;

	const int SLOTTS_HORZ;
	const int SLOTTS_VERT;

	const Point UPPER_LEFT;
};

#endif
