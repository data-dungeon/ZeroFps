
#include "mistclient.h"
#include "gui_inventory.h"
#include "gui_equipwnd.h"
#include "../mcommon/p_characterproperty.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/engine/inputhandle.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../mcommon/p_item.h"

extern MistClient	g_kMistClient;

void GuiMsgEquipmentDlg( string strMainWnd, string strController, 
								unsigned int msg, int numparms, void *params )
{
	if(msg == ZGM_COMMAND)
	{
		if(strMainWnd == "EquipWnd")
			g_kMistClient.m_pkEquipmentDlg->OnCommand(strController);
	}
	else
	if(msg == ZGM_MOUSEMOVE)
	{
		int pressed = ((int*)params)[0], x = ((int*)params)[1], y = ((int*)params)[2];
		g_kMistClient.m_pkEquipmentDlg->OnMouseMove(pressed < 1 ? false : true, x, y);
	}
	else
	if(msg == ZGM_KEYPRESS)
	{
		int iKey = ((int*)params)[0];
		switch(iKey)
		{		
		case KEY_RETURN:
			if( strController	==	"SayTextbox" )
			{
				char*	text = g_kMistClient.GetText("SayTextbox");
				if(text != NULL && strlen(text) > 0)
				{
					g_kMistClient.Say(text);
					g_kMistClient.SetText("SayTextbox",	"");
				}

				ZGuiWnd::m_pkFocusWnd->KillFocus();
				g_kMistClient.m_pkGui->SetFocus(g_kMistClient.GetWnd("GuiMainWnd"), false);				
				g_kMistClient.SetGuiCapture(false);
				g_kMistClient.m_pkGui->m_bHandledMouse = false;
			}
			else
				g_kMistClient.ToogleChatWnd(true, true);
			break;
		
		case KEY_ESCAPE:
			if( strController	==	"SayTextbox" )
			{
				((ZGuiTextbox*)g_kMistClient.GetWnd(strController))->KillFocus();
				if(ZGuiWnd::m_pkFocusWnd)
					ZGuiWnd::m_pkFocusWnd->KillFocus();
				g_kMistClient.SetGuiCapture(false);				
				g_kMistClient.m_pkGui->m_bHandledMouse = false;
			}

			g_kMistClient.m_pkGui->m_bForceGUICapture = false;
			break;	
		}
	}
}

EquipmentDlg::EquipmentDlg()
{
	m_pkMainWnd = NULL;
	m_pkMoveSlot = NULL;

	m_pkTexMan = g_kMistClient.m_pkTexMan;
	m_bSkillWndOpen = false;
	m_bStatsWndOpen = false;
}

EquipmentDlg::~EquipmentDlg()
{
	if(m_pkMainWnd)
		g_kMistClient.m_pkGui->UnregisterWindow(m_pkMainWnd);
}

void EquipmentDlg::Open()
{	
	// load inventory
	if(m_pkMainWnd == NULL)
	{
		m_pkMainWnd = g_kMistClient.GetWnd("EquipWnd");		

		m_kHead = EQUIPMENT_SLOT(eHead, g_kMistClient.GetWnd("EQSlotHead"), -1, 2, 2, -1, -1);
		m_kGloves = EQUIPMENT_SLOT(eGloves, g_kMistClient.GetWnd("EQSlotGloves"), -1, 2, 2, -1, -1);
		m_kAmulett = EQUIPMENT_SLOT(eNecklace, g_kMistClient.GetWnd("EQSlotAmulett"), -1, 1, 1, -1, -1);
		m_kCloak = EQUIPMENT_SLOT(eCape, g_kMistClient.GetWnd("EQSlotCloak"), -1, 2, 2, -1, -1);
		m_kRightHand = EQUIPMENT_SLOT(eRightHand, g_kMistClient.GetWnd("EQSlotRightHand"), -1, 2, 4, -1, -1);
		m_kBody = EQUIPMENT_SLOT(eBody, g_kMistClient.GetWnd("EQSlotBody"), -1, 3, 4, -1, -1);
		m_kLeftHand = EQUIPMENT_SLOT(eLeftHand, g_kMistClient.GetWnd("EQSlotLeftHand"), -1, 2, 4, -1, -1);
		m_kBracers = EQUIPMENT_SLOT(eBracers, g_kMistClient.GetWnd("EQSlotBracers"), -1, 1, 2, -1, -1);
		m_kBelt = EQUIPMENT_SLOT(eBelt, g_kMistClient.GetWnd("EQSlotBelt"), -1, 2, 1, -1, -1);
		m_kRing1 = EQUIPMENT_SLOT(eRing1, g_kMistClient.GetWnd("EQSlotRing1"), -1, 1, 1, -1, -1);
		m_kRing2 = EQUIPMENT_SLOT(eRing2, g_kMistClient.GetWnd("EQSlotRing2"), -1, 1, 1, -1, -1);
		m_kRing3 = EQUIPMENT_SLOT(eRing3, g_kMistClient.GetWnd("EQSlotRing3"), -1, 1, 1, -1, -1);
		m_kRing4 = EQUIPMENT_SLOT(eRing4, g_kMistClient.GetWnd("EQSlotRing4"), -1, 1, 1, -1, -1);
		m_kBoots = EQUIPMENT_SLOT(eFeets, g_kMistClient.GetWnd("EQSlotBoots"), -1, 2, 2, -1, -1);
	}

	// visa inventory fönstret
	m_pkMainWnd->Show();
	g_kMistClient.m_pkGui->PlaceWndFrontBack(g_kMistClient.GetWnd("BuffWnd"),false);
	g_kMistClient.m_pkGui->PlaceWndFrontBack(m_pkMainWnd,true);
	g_kMistClient.m_pkGui->SetFocus(m_pkMainWnd, false);	

	if(m_bSkillWndOpen)
		g_kMistClient.GetWnd("SkillWnd")->Show();

	if(m_bStatsWndOpen)
		g_kMistClient.GetWnd("StatsWnd")->Show();
}

void EquipmentDlg::Close()
{
	// Hide inventory window
	if(m_pkMainWnd)
		m_pkMainWnd->Hide();

	// Must set focus on mainwnd to recive SPACE intput for chatbox...
	g_kMistClient.m_pkGui->SetFocus(g_kMistClient.GetWnd("GuiMainWnd"), false);	

	g_kMistClient.GetWnd("SkillWnd")->Hide();
	g_kMistClient.GetWnd("StatsWnd")->Hide();

}

void EquipmentDlg::OnCommand(string strController)
{
	if(strController == "OpenSkillsBn")
	{
		if(g_kMistClient.GetWnd("SkillWnd")->IsVisible()==false)
		{
			m_bSkillWndOpen = true;
			m_bStatsWndOpen = false;
			g_kMistClient.GetWnd("SkillWnd")->Show();
			g_kMistClient.GetWnd("StatsWnd")->Hide();
		}
		else
		{
			m_bSkillWndOpen = false;
			g_kMistClient.GetWnd("SkillWnd")->Hide();
			g_kMistClient.GetWnd("StatsWnd")->Hide();
		}
	}

	if(strController == "OpenStatsBn")
	{
		if(g_kMistClient.GetWnd("StatsWnd")->IsVisible()==false)
		{
			m_bStatsWndOpen = true;
			m_bSkillWndOpen = false;
			g_kMistClient.GetWnd("StatsWnd")->Show();
			g_kMistClient.GetWnd("SkillWnd")->Hide();
		}
		else
		{
			m_bStatsWndOpen = false;
			g_kMistClient.GetWnd("SkillWnd")->Hide();
			g_kMistClient.GetWnd("StatsWnd")->Hide();
		}
	}

}

void EquipmentDlg::OnMouseMove(bool bLeftButtonPressed, int mx, int my)
{
	static bool s_bLeftButtonUp = true;

	if(s_bLeftButtonUp == true && bLeftButtonPressed)
	{
		s_bLeftButtonUp = false;
		
		if(m_pkMoveSlot == NULL)
		{
			EQUIPMENT_SLOT* pkSlot = GetSlot(mx, my);
			if(pkSlot != NULL && pkSlot->m_iItemID != -1)
				PickUpFromSlot(pkSlot, mx, my);
		}
	}
	else
	if(bLeftButtonPressed == false)
	{
		s_bLeftButtonUp = true;

		if(m_pkMoveSlot != NULL)
		{
			OnDropItem(mx, my);
		}
	}	

	if(m_pkMainWnd)
	{
		if(m_pkMainWnd->IsVisible())
		{
			if(m_pkMoveSlot)
			{
				if(g_kMistClient.m_pkInventoryDlg->GetDropTargetFromScreenPos(mx, my) != DropTarget_Ground) 
					(*g_kMistClient.m_pkGui->GetCursorSkin())->m_unBorderSize = 1; 
				else
					(*g_kMistClient.m_pkGui->GetCursorSkin())->m_unBorderSize = 0; 
			}
		}
	}
}

void EquipmentDlg::HighlightSlot(int iItemType)
{
	if(m_pkMainWnd == NULL)
		return;

	if(!m_vkHighLightslots.empty()) 
	{
		for(int i=0; i<m_vkHighLightslots.size(); i++)
			m_vkHighLightslots[i]->m_pkWnd->GetSkin()->m_unBorderSize = 0;  
	}

	m_vkHighLightslots.clear(); 

	switch(iItemType)
	{
		case MLITEM_HEAD: m_vkHighLightslots.push_back(&m_kHead); break;
		case MLITEM_BODY: m_vkHighLightslots.push_back(&m_kBody); break;
		case MLITEM_HAND: 
			m_vkHighLightslots.push_back(&m_kRightHand); 
			m_vkHighLightslots.push_back(&m_kLeftHand); 
			break;
		case MLITEM_RING:
			m_vkHighLightslots.push_back(&m_kRing1); 
			m_vkHighLightslots.push_back(&m_kRing2); 
			m_vkHighLightslots.push_back(&m_kRing3); 
			m_vkHighLightslots.push_back(&m_kRing4);
			break;
		case MLITEM_NECKLACE: m_vkHighLightslots.push_back(&m_kAmulett); break;
		case MLITEM_BRACERS: m_vkHighLightslots.push_back(&m_kBracers); break;
		case MLITEM_GLOVES: m_vkHighLightslots.push_back(&m_kGloves); break;
		case MLITEM_BELT: m_vkHighLightslots.push_back(&m_kBelt); break;
		case MLITEM_FEETS: m_vkHighLightslots.push_back(&m_kBoots); break;
		case MLITEM_CAPE:  m_vkHighLightslots.push_back(&m_kCloak); break;
	}	

	for(int i=0; i<m_vkHighLightslots.size(); i++)
	{
		m_vkHighLightslots[i]->m_pkWnd->Show();
		//m_vkHighLightslots[i]->m_pkWnd->GetSkin()->m_bTransparent = true;
		m_vkHighLightslots[i]->m_pkWnd->GetSkin()->m_unBorderSize = 4;  
	}
}

EquipmentDlg::EQUIPMENT_SLOT* EquipmentDlg::GetSlot(int mx, int my)
{
	if(m_pkMainWnd == NULL)
		return NULL;
	
	if(m_kHead.m_rcOriginal.Inside(mx,my)) return &m_kHead;
	if(m_kGloves.m_rcOriginal.Inside(mx,my)) return &m_kGloves;
	if(m_kAmulett.m_rcOriginal.Inside(mx,my)) return &m_kAmulett;
	if(m_kCloak.m_rcOriginal.Inside(mx,my)) return &m_kCloak;
	if(m_kRightHand.m_rcOriginal.Inside(mx,my)) return &m_kRightHand;
	if(m_kBody.m_rcOriginal.Inside(mx,my)) return &m_kBody;
	if(m_kLeftHand.m_rcOriginal.Inside(mx,my)) return &m_kLeftHand;
	if(m_kBracers.m_rcOriginal.Inside(mx,my)) return &m_kBracers;
	if(m_kBelt.m_rcOriginal.Inside(mx,my)) return &m_kBelt;
	if(m_kRing1.m_rcOriginal.Inside(mx,my)) return &m_kRing1;
	if(m_kRing2.m_rcOriginal.Inside(mx,my)) return &m_kRing2;
	if(m_kRing3.m_rcOriginal.Inside(mx,my)) return &m_kRing3;
	if(m_kRing4.m_rcOriginal.Inside(mx,my)) return &m_kRing4;
	if(m_kBoots.m_rcOriginal.Inside(mx,my)) return &m_kBoots;

	return NULL;	
}

int EquipmentDlg::GetSlotType(int mx, int my)
{
	if(m_pkMainWnd == NULL)
		return -1;

	Rect rc(0,0,0,0);
		
	if(m_kHead.m_rcOriginal.Inside(mx,my)) return eHead;
	if(m_kGloves.m_rcOriginal.Inside(mx,my)) return eGloves;
	if(m_kAmulett.m_rcOriginal.Inside(mx,my)) return eNecklace;
	if(m_kCloak.m_rcOriginal.Inside(mx,my)) return eCape;
	if(m_kRightHand.m_rcOriginal.Inside(mx,my)) return eRightHand;
	if(m_kBody.m_rcOriginal.Inside(mx,my)) return eBody;
	if(m_kLeftHand.m_rcOriginal.Inside(mx,my)) return eLeftHand;
	if(m_kBracers.m_rcOriginal.Inside(mx,my)) return eBracers;
	if(m_kBelt.m_rcOriginal.Inside(mx,my)) return eBelt;
	if(m_kRing1.m_rcOriginal.Inside(mx,my)) return eRing1;
	if(m_kRing2.m_rcOriginal.Inside(mx,my)) return eRing2;
	if(m_kRing3.m_rcOriginal.Inside(mx,my)) return eRing3;
	if(m_kRing4.m_rcOriginal.Inside(mx,my)) return eRing4;
	if(m_kBoots.m_rcOriginal.Inside(mx,my)) return eFeets;

	return -1;	
}

int EquipmentDlg::GetSlotContainerID(int mx, int my)
{
	if(m_pkMainWnd == NULL)
		return -1;

	Rect rc(0,0,0,0);
		
	if(m_kHead.m_rcOriginal.Inside(mx,my)) return m_kHead.m_iContainerID;
	if(m_kGloves.m_rcOriginal.Inside(mx,my)) return m_kGloves.m_iContainerID;
	if(m_kAmulett.m_rcOriginal.Inside(mx,my)) return m_kAmulett.m_iContainerID;
	if(m_kCloak.m_rcOriginal.Inside(mx,my)) return m_kCloak.m_iContainerID;
	if(m_kRightHand.m_rcOriginal.Inside(mx,my)) return m_kRightHand.m_iContainerID;
	if(m_kBody.m_rcOriginal.Inside(mx,my)) return m_kBody.m_iContainerID;
	if(m_kLeftHand.m_rcOriginal.Inside(mx,my)) return m_kLeftHand.m_iContainerID;
	if(m_kBracers.m_rcOriginal.Inside(mx,my)) return m_kBracers.m_iContainerID;
	if(m_kBelt.m_rcOriginal.Inside(mx,my)) return m_kBelt.m_iContainerID;
	if(m_kRing1.m_rcOriginal.Inside(mx,my)) return m_kRing1.m_iContainerID;
	if(m_kRing2.m_rcOriginal.Inside(mx,my)) return m_kRing2.m_iContainerID;
	if(m_kRing3.m_rcOriginal.Inside(mx,my)) return m_kRing3.m_iContainerID;
	if(m_kRing4.m_rcOriginal.Inside(mx,my)) return m_kRing4.m_iContainerID;
	if(m_kBoots.m_rcOriginal.Inside(mx,my)) return m_kBoots.m_iContainerID;

	return -1;	
}


void EquipmentDlg::GetEquipmentSlotFromContainerType(int iContainerType, EquipmentDlg::EQUIPMENT_SLOT*& ppSlot)
{
	switch(iContainerType)
	{
		case eLeftHand: ppSlot = &m_kLeftHand; break;
		case eRightHand: ppSlot = &m_kRightHand; break;
		case eHead: ppSlot = &m_kHead; break;
		case eBody: ppSlot = &m_kBody; break;
		case eGloves: ppSlot = &m_kGloves; break;
		case eCape: ppSlot = &m_kCloak; break;
		case eNecklace: ppSlot = &m_kAmulett; break;
		case eBracers: ppSlot = &m_kBracers; break;
		case eBelt: ppSlot = &m_kBelt; break;
		case eFeets: ppSlot = &m_kBoots; break;
		case eRing1: ppSlot = &m_kRing1; break;
		case eRing2: ppSlot = &m_kRing2; break;
		case eRing3: ppSlot = &m_kRing3; break;
		case eRing4: ppSlot = &m_kRing4; break;
		default: ppSlot = NULL;
	}
}

void EquipmentDlg::Update(int iContainerID, int iContainerType, vector<MLContainerInfo>& vkItemList)
{
	EQUIPMENT_SLOT* pSlot;
	GetEquipmentSlotFromContainerType(iContainerType, pSlot);

	if(pSlot != NULL)
	{
		if(vkItemList.empty())
		{			
			(*pSlot).m_iItemID = -1;
			(*pSlot).m_iStackSize = -1;
			(*pSlot).m_iSlotsW = -1;
			(*pSlot).m_iSlotsH = -1;
			(*pSlot).m_pkWnd->GetSkin()->m_bTransparent = true;
			(*pSlot).m_pkWnd->Hide();
			(*pSlot).m_iContainerID = iContainerID;
		}
		else
		{
			(*pSlot).m_iItemType = vkItemList[0].m_cItemType;
			(*pSlot).m_iContainerID = iContainerID;
			(*pSlot).m_iItemID = vkItemList[0].m_iItemID;
			(*pSlot).m_iStackSize = vkItemList[0].m_iStackSize;
			(*pSlot).m_iSlotsW = vkItemList[0].m_cItemW;
			(*pSlot).m_iSlotsH = vkItemList[0].m_cItemH;

			(*pSlot).m_pkWnd->GetSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes( "items/" + vkItemList[0].m_strIcon);
			(*pSlot).m_pkWnd->GetSkin()->m_bTransparent = false; 
			(*pSlot).m_pkWnd->Show();
			RescaleSlotIcon((*pSlot), (*pSlot).m_iSlotsW, (*pSlot).m_iSlotsH);
		}
	}

}

void EquipmentDlg::RescaleSlotIcon(EQUIPMENT_SLOT& r_kSlot, int iSlotsX, int iSlotsY)
{
	Rect rc = r_kSlot.m_pkWnd->GetScreenRect();
	Rect rcIcon = r_kSlot.m_rcOriginal;

	if(iSlotsX > r_kSlot.m_iSlotWidthMax)
		iSlotsX = r_kSlot.m_iSlotWidthMax;

	if(iSlotsY > r_kSlot.m_iSlotHeightMax)
		iSlotsY = r_kSlot.m_iSlotHeightMax;

	float dx = iSlotsX * 32;
	dx *= g_kMistClient.GetScaleX(); 

	float dy = iSlotsY * 32;
	dy *= g_kMistClient.GetScaleY(); 

	int x = rcIcon.Right - rcIcon.Width() / 2 - (int) dx / 2;
	int y  = rcIcon.Bottom - rcIcon.Height() / 2 - (int) dy / 2;

	r_kSlot.m_pkWnd->SetPos(x, y, true, true);
	r_kSlot.m_pkWnd->Resize(dx, dy);
}

void EquipmentDlg::PickUpFromSlot(EQUIPMENT_SLOT* pkSlot, int mx, int my)
{
	if(pkSlot == NULL)
	{
		printf("Didn't find slot\n");
		return;
	}

	m_kCursorRangeDiff.x = mx-pkSlot->m_pkWnd->GetScreenRect().Left; 
	m_kCursorRangeDiff.y = my-pkSlot->m_pkWnd->GetScreenRect().Top;

	m_kItemWndPosBeforeMove.x = pkSlot->m_pkWnd->GetWndRect().Left;
	m_kItemWndPosBeforeMove.y = pkSlot->m_pkWnd->GetWndRect().Top;

	m_pkMoveSlot = pkSlot;
	m_pkMoveSlot->m_pkWnd->Hide();

	Point size(m_pkMoveSlot->m_iSlotsW, m_pkMoveSlot->m_iSlotsH);
	int id = m_pkMoveSlot->m_pkWnd->GetSkin()->m_iBkTexID;

	int x = mx - m_kCursorRangeDiff.x, y = my - m_kCursorRangeDiff.y;
	g_kMistClient.m_pkGui->SetCursor( x, y, id, -1, 
		g_kMistClient.GetScaleX()*(float)(size.x*32), 
		g_kMistClient.GetScaleY()*(float)(size.y*32));	
	g_kMistClient.m_pkInputHandle->SetCursorInputPos(x,y);  

	HighlightSlot(pkSlot->m_iItemType); 

	m_pkMoveSlot->m_pkWnd->GetSkin()->m_bTransparent = true;
	m_pkMoveSlot->m_pkWnd->GetSkin()->m_unBorderSize = 4;

}

void EquipmentDlg::OnDropItem(int mx, int my)
{
	if(m_pkMoveSlot != NULL && m_pkMoveSlot->m_iItemID >= 0)
	{
		InventoryDropTarget eDropTarget = g_kMistClient.m_pkInventoryDlg->GetDropTargetFromScreenPos(mx, my);

		Point slot(-1,-1);
		int iTarget = -1;
		if(eDropTarget == DropTarget_Ground)
		{
			iTarget = -1;
		}
		else
		if(eDropTarget == DropTarget_Inventory)
		{
			iTarget = g_kMistClient.m_pkInventoryDlg->GetInventoryContainerID();
			slot = g_kMistClient.m_pkInventoryDlg->SlotFromScreenPos(mx, my, true);
		}
		else
		if(eDropTarget == DropTarget_Container)
		{
			iTarget = g_kMistClient.m_pkInventoryDlg->GetActiveContainerID();
			slot = g_kMistClient.m_pkInventoryDlg->SlotFromScreenPos(mx, my, false);
		}
		else
		{
			EQUIPMENT_SLOT* pkTargetSlot = GetSlot(mx, my);

			if(pkTargetSlot)
			{
				iTarget = pkTargetSlot->m_iContainerID;
				slot.x = slot.y = -1;
			}
		}

		if(m_pkMoveSlot->m_iItemID >= 0)
			g_kMistClient.SendMoveItem(m_pkMoveSlot->m_iItemID, iTarget, slot.x, slot.y, -1);
		m_pkMoveSlot = NULL;
	}
	else
	{
		printf("Failed to drop equipment item, bad moveslot\n");
	}

	// Show normal cursor again.
	float w = g_kMistClient.GetScaleX()*64.0f, h = g_kMistClient.GetScaleY()*64.0f ;
	g_kMistClient.m_pkGui->SetCursor( (int)mx+m_kCursorRangeDiff.x, (int)my+m_kCursorRangeDiff.y, 
		g_kMistClient.LoadGuiTextureByRes("cursor_sword.tga"), -1, w, h);

	g_kMistClient.m_pkInputHandle->SetCursorInputPos(mx+m_kCursorRangeDiff.x,my+m_kCursorRangeDiff.y);	

	(*g_kMistClient.m_pkGui->GetCursorSkin())->m_unBorderSize = 0; 

	HighlightSlot(-1);
}