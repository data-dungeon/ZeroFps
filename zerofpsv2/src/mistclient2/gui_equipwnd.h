#ifndef _GUI_EQUIPMENTDLG_H_
#define _GUI_EQUIPMENTDLG_H_

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

enum EquipmentSlot
{
	EqS_Helm				= 0,
	EqS_Gloves			= 1,
	EqS_Amulett			= 2,
	EqS_Cloak			= 3,
	EqS_WeaponHand		= 4,
	EqS_Body				= 5,
	EqS_ShieldHand		= 6,
	EqS_Bracer1			= 7,
	EqS_Bracer2			= 8,
	EqS_Belt				= 9,
	EqS_Ring1			= 10,
	EqS_Ring2			= 11,
	EqS_Ring3			= 12,
	EqS_Ring4			= 13,
	EqS_Boots			= 14,

	EqS_None				= 99
};

class EquipmentDlg
{
public:
	EquipmentDlg();
	~EquipmentDlg();

	void Open();
	void Close();
	void Update(vector<MLContainerInfo>& vkItemList);

	void OnCommand(string strController);
	void OnMouseMove(bool bLeftButtonPressed, int x, int y);
	bool IsVisible() 
	{ 
		if(m_pkEquipmentWnd == NULL) 
			return false; 
		return m_pkEquipmentWnd->IsVisible(); 
	}
	void OnDrop(int mx, int my);
	EquipmentSlot GetSlot(int mx, int my, bool bScreenPos);
	Rect GetRect(EquipmentSlot eSlot, bool bScreenPos);
		
private:

	struct EQUIPMENT_SLOT
	{	
		EquipmentSlot eSlotType;
		ZGuiWnd* pkWnd;
		int iItemID;
		int iStackSize;
	};

	struct SPECIAL_SLOT
	{
		int m_iIndex; // index in item list
	};

	vector<EQUIPMENT_SLOT> m_vkEquipmentItemList;

	SPECIAL_SLOT m_kMoveSlot; 

	Point m_kCursorRangeDiff;
	Point m_kItemWndPosBeforeMove; // fönster koordinater
	
	TextureManager* m_pkTexMan;
	
	ZGuiWnd* m_pkEquipmentWnd;	

	bool m_bSkillWndOpen;
	bool m_bStatsWndOpen;

	// Kollisionsrektanglar för olika itemslots i fönsterkoordinater.
	const Rect m_rcHelm;
	const Rect m_rcGloves;
	const Rect m_rcAmulett;
	const Rect m_rcCloak;
	const Rect m_rcWeaponHand;
	const Rect m_rcBody;
	const Rect m_rcShieldHand;
	const Rect m_rcBracer1;
	const Rect m_rcBracer2;
	const Rect m_rcBelt;
	const Rect m_rcRing1;
	const Rect m_rcRing2;
	const Rect m_rcRing3;
	const Rect m_rcRing4;
	const Rect m_rcBoots;
};

#endif // _GUI_EQUIPMENTDLG_H_
