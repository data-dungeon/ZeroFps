// inventorydlg.h: interface for the InventoryDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INVENTORYDLG_H__E2EC300C_10CF_4D5C_B948_2FD7FCAB2686__INCLUDED_)
#define AFX_INVENTORYDLG_H__E2EC300C_10CF_4D5C_B948_2FD7FCAB2686__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../zerofpsv2/basic/zfsubsystem.h"
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

class InventoryDlg// : public ZFSubSystem  
{
public:
	
	typedef pair<pair<string, string>,ItemStats*> itItem;

	bool AddItems(vector<pair<pair<string,string>,ItemStats*> >&vkItems);
	bool AddItem(const char* szPic, const char* szPicA, ItemStats* pkItemStats);
	void OnScroll(int iID, int iPos);
	void OnCommand(int iID);
	void OnMouseMove(int x, int y, bool bMouseDown);
	
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
		ZGuiLabel* m_pkLabel;
		
		char m_szPic[2][75];
		SlotType m_eType;
		Point m_kSqr;

		int m_iContainer;
		int m_iContainerID; // only if this slot is a container
	};

	Slot* FindSlot(int mouse_x, int mouse_y);
	void OnClick(int x, int y, bool bMouseDown, bool bLeftButton);
	void OnDClick(int x, int y, bool bLeftButton);
	
	InventoryDlg(ZGuiWnd* pkDlgWnd);
	~InventoryDlg();

	

private:
	void SwitchContainer(int iNewContainer);
	void AddSlot(const char *szPic, const char *szPicA, Point sqr, 
		SlotType eType, ItemStats* pkItemStats, int iContainer);
	bool RemoveSlot(/*Point sqr,*/ Slot* pkSlot); //bool bDragItem=false);
	bool GetFreeSlotPos(Point& refSqr);
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
	int m_iPrevScrollPos;
	int m_iCurrentContainer;
	int m_iPrevContainer;

	Point m_kClickOffset;



};

#endif // !defined(AFX_INVENTORYDLG_H__E2EC300C_10CF_4D5C_B948_2FD7FCAB2686__INCLUDED_)
