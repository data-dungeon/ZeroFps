// inventorydlg.h: interface for the InventoryDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INVENTORYDLG_H__E2EC300C_10CF_4D5C_B948_2FD7FCAB2686__INCLUDED_)
#define AFX_INVENTORYDLG_H__E2EC300C_10CF_4D5C_B948_2FD7FCAB2686__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../zerofpsv2/basic/zfsubsystem.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/gui/zguilabel.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../zerofpsv2/render/texturemanager.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"
#include "../mcommon/rulesystem/item/itemstats.h"

#include <vector>
#include <string>
#include <stack>
using namespace std;

const int MAIN_CONTAINER = 0;
const int SLOT_SIZE = 48;

struct InvType
{
   ItemStats* m_pkItemStats;
   int m_iNetWorkID;
};

class InventoryDlg// : public ZFSubSystem  
{
public:
	typedef pair<pair<string, string>, InvType> itItem;

	bool EquipSpecialSlot(ItemStats *pkItemStats, int iNetworkID, EquipmentCategory eCategory);
	bool AddItems(vector<Entity*>&vkItems);
	bool AddItem(Entity* pkItemStats);
	void OnScroll(int iID, int iPos);
	void OnCommand(int iID);
	void OnMouseMove(int x, int y, bool bMouseDown);

	void SetCurrentContainer(int iContainerID);
	void SetMainContainer(int iContainerID);
	
	enum SlotType
	{
		CONTAINTER_SLOTS,
		UNDER_MOUSE,
		SPECIAL_SLOTS,
	};	

	class Slot
	{
	public:
		Slot() {};
		~Slot() {};

		ItemStats* m_pkItemStats;
      int m_iContainerID; // ID of the container, if it have any..else ID = -1 // Zerom

      int m_iNetWorkID;
		ZGuiLabel* m_pkLabel;
		
		char m_szPic[75];
		SlotType m_eType;
		Point m_kSqr;
		Point m_kRealSqr;
	};

	Slot* FindSlot(int mouse_x, int mouse_y);
	void OnClick(int x, int y, bool bMouseDown, bool bLeftButton);
	void OnDClick(int x, int y, bool bLeftButton);

   // if this vector contain objects, the objects is added to the inventory
   // the vector is checked for new items every frame
   vector<Entity*>* m_pkAddItemList;

   void Update();
	
	InventoryDlg(ZGuiWnd* pkDlgWnd);
	~InventoryDlg();

private:
	EquipmentCategory MousePosToCategory(int x, int y);
	void UpdateSkin(Slot slot);
	int GetPrevContainer();
	void DropItemsToContainer(int iContainer);
	void DropItems();
	void SwitchContainer(int iNewContainer);
	void AddSlot(const char *szPic, Point sqr, SlotType eType, 
		ItemStats* pkItemStats, int iContID, int iNetworkID, int iContainer);
	bool RemoveSlot(Slot* pkSlot);
	bool GetFreeSlotPos(Point& refSqr, int iContainer);
	bool SlotExist(int sx, int sy);
	static int GenerateID(Point sqr);

	string GetWndByID(int iID);
	void ScrollItems(int iPos);
	
	void RegisterSlot(Slot slot);
	Point MousePosToSqr(int x, int y);
	Point MousePosToSpecialSqrPos(int x, int y, EquipmentCategory eCategory);
	vector<Slot> m_kItemSlots;
	vector<Slot> m_kDragSlots;
	stack<int> m_kContainerStack;
	
	typedef vector<Slot>::iterator itSlot;

   ZeroFps* m_pkZeroFps;
   EntityManager* m_pkEntityMan;
	ZGui* m_pkGui;
	ZGuiResourceManager* m_pkResMan;
	TextureManager* m_pkTexMan;
	ZFAudioSystem* m_pkAudioSys;
	ZGuiWnd* m_pkDlgWnd;
	ZGuiLabel* m_pkSelectionLabel;
	ZGuiButton* m_pkBackButton;
	Slot* m_pkSelectedSlot;

	static int s_iSlotZCounter;
	
	int m_iCurrentScrollPos;
	int m_iCurrentContainer;
	int m_iMainContainer;

	Point m_kClickOffset;
};

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

	ContainerDlg(ZGuiApp* pkApp);
	~ContainerDlg();

private:
	void RemoveAllItems();
	bool AddItem(char* szPic, Entity* pkObject);
	void TakeAll();
	ZGuiWnd* m_pkDlgWnd;

	ZGui* m_pkGui;
	ZGuiApp* m_pkApp;
	TextureManager* m_pkTexMan;

	ZGuiButton* m_pkContatinerButtons[CONTAINER_ITEM_ROWS][CONTAINER_ITEM_COLS];

	vector<pair<ZGuiLabel*, Entity*> > m_vkItems;
	set<Entity*> m_kContainerObjects;

	string GetWndByID(int iID);

	Rect m_rcClipperArea;

	P_Container* m_pkContainer;
	
};

#endif // !defined(AFX_INVENTORYDLG_H__E2EC300C_10CF_4D5C_B948_2FD7FCAB2686__INCLUDED_)
