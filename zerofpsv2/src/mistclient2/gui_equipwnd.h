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

class EquipmentDlg
{
public:
	EquipmentDlg();
	~EquipmentDlg();

	void Open();
	void Close();
	void Update(int iContainerID, int iContainerType, vector<MLContainerInfo>& vkItemList);

	void OnCommand(string strController);
	void OnMouseMove(bool bLeftButtonPressed, int x, int y);
	bool IsVisible() 
	{ 
		if(m_pkMainWnd == NULL) 
			return false; 
		return m_pkMainWnd->IsVisible(); 
	}
	
	int GetSlotType(int mx, int my);
	int GetSlotContainerID(int mx, int my);
	void HighlightSlot(int iContainerType);
		
private:

	struct EQUIPMENT_SLOT
	{	
		EQUIPMENT_SLOT() {m_eSlotType = m_iItemID = m_iStackSize = m_iSlotWidthMax = m_iSlotHeightMax = m_iContainerID = -1; m_pkWnd = NULL; };
		EQUIPMENT_SLOT(ContainerTypes eType, ZGuiWnd* pkWnd, int iContainerID, int iSlotsX, int iSlotsY, int iItemID, int iStackSize)
		{
			m_eSlotType = eType;
			m_pkWnd = pkWnd;
			m_iItemID = iItemID;
			m_iStackSize = iStackSize;
			m_iSlotHeightMax = iSlotsY;
			m_iSlotWidthMax = iSlotsX;
			m_iSlotsW = m_iSlotsH = -1;
			m_iContainerID = iContainerID;

			m_rcOriginal = m_pkWnd->GetScreenRect();
		};

		Rect m_rcOriginal;
		ZGuiWnd* m_pkWnd;
		int m_eSlotType;
		int m_iItemID;
		int m_iContainerID;
		int m_iStackSize;
		int m_iSlotsW, m_iSlotsH;
		int m_iSlotWidthMax;
		int m_iSlotHeightMax;
	};

	void RescaleSlotIcon(EQUIPMENT_SLOT& r_kSlot, int iSlotsX, int iSlotsY);
	void GetEquipmentSlotFromContainerType(int iContainerType, EquipmentDlg::EQUIPMENT_SLOT*& ppSlot);
	EQUIPMENT_SLOT* GetSlot(int mx, int my);
	void PickUpFromSlot(EQUIPMENT_SLOT* pkSlot, int mx, int my);
	void OnDropItem(int mx, int my);

	Point m_kCursorRangeDiff;
	Point m_kItemWndPosBeforeMove; // fönster koordinater
	
	TextureManager* m_pkTexMan;
	
	ZGuiWnd* m_pkMainWnd;	

	bool m_bSkillWndOpen;
	bool m_bStatsWndOpen;

	EQUIPMENT_SLOT m_kHead;
	EQUIPMENT_SLOT m_kGloves;
	EQUIPMENT_SLOT m_kAmulett;
	EQUIPMENT_SLOT m_kCloak;
	EQUIPMENT_SLOT m_kRightHand;
	EQUIPMENT_SLOT m_kBody;
	EQUIPMENT_SLOT m_kLeftHand;
	EQUIPMENT_SLOT m_kBracers;
	EQUIPMENT_SLOT m_kBelt;
	EQUIPMENT_SLOT m_kRing1;
	EQUIPMENT_SLOT m_kRing2;
	EQUIPMENT_SLOT m_kRing3;
	EQUIPMENT_SLOT m_kRing4;
	EQUIPMENT_SLOT m_kBoots;

	EQUIPMENT_SLOT* m_pkMoveSlot; 
	//EQUIPMENT_SLOT* m_pkHighLightslot; 
	vector<EQUIPMENT_SLOT*> m_vkHighLightslots;
};

#endif // _GUI_EQUIPMENTDLG_H_
