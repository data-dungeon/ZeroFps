#ifndef _GUI_INVENTORY_H_
#define _GUI_INVENTORY_H_

#include "../zerofpsv2/basic/vector2.h"
#include "../zerofpsv2/basic/zguiskin.h"
#include "../mcommon/mlcontainer.h"

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
	bool IsVisible() { if(m_pkMainWnd == NULL) return false; return m_pkMainWnd && m_pkMainWnd->IsVisible(); }
	MLContainer* GetContainer();

private:

	struct ITEM_SLOT
	{
		ZGuiWnd* pkWnd;
		int iItemID;
	};

	void OnDropItem();

	TextureManager* m_pkTexMan;

	vector<ITEM_SLOT> m_vkItemList;

	ZGuiWnd* m_pkMainWnd;
	int m_iMoveSlot; // index of m_vkItemList
	int m_iSelItemID; // ITEM_SLOT::iItemID (aka MLContainerInfo::m_iItemID)
	int m_iHighestZ;

	Point m_kPosBeforeMove;
	
	bool m_bGuiCaptureBeforOpen;

	const int ICON_WIDTH;
	const int ICON_HEIGHT;

	const int SLOTTS_HORZ;
	const int SLOTTS_VERT;

	const Point UPPER_LEFT;





	
};

#endif
