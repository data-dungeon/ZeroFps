
#ifndef _CONTAINERDLG_H_
#define _CONTAINERDLG_H_

#include "../zerofpsv2/basic/zfsubsystem.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/gui/zguilabel.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../zerofpsv2/render/texturemanager.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"
#include "../mcommon/rulesystem/item/itemstats.h"

class InventoryDlg;

const int CONTAINER_ITEM_ROWS = 5;
const int CONTAINER_ITEM_COLS = 4;

const int MAX_NUM_ITEMS = 76;

class P_Container;

class ContainerDlg
{
public:
	void Update();
	void OnCommand(int iID);
	void OnScroll(int iID, int iPos);
	bool IsOpen() { if(m_pkDlgWnd == NULL) return false; return m_pkDlgWnd->IsVisible(); }
	void ToggleOpen(bool bOpen);
	void SetContainer(P_Container* pkContainer);
	void Create();
	void SetInventoryPointer(InventoryDlg* pkInventory);

	ContainerDlg(ZGuiApp* pkApp);
	~ContainerDlg();

private:
	Entity* GetItemFromCursorPos(int x, int y);
	bool MoveItemToInventory(Entity* pkObject);
	void RemoveAllItems();
	void RemoveItem(Entity* pkObject);
	bool AddItem(char* szPic, Entity* pkObject);
	void TakeAll();
	ZGuiWnd* m_pkDlgWnd;

	ZGui* m_pkGui;
	ZGuiApp* m_pkApp;
	TextureManager* m_pkTexMan;
	InventoryDlg* m_pkInventoryDlg;

	ZGuiButton* m_pkContatinerButtons[CONTAINER_ITEM_ROWS][CONTAINER_ITEM_COLS];

	vector<pair<ZGuiLabel*, Entity*> > m_vkItems;
	set<Entity*> m_kContainerObjects;

	string GetWndByID(int iID);

	Rect m_rcClipperArea;

	P_Container* m_pkContainer;
	
};

#endif // _CONTAINERDLG_H_