
#include "mistclient.h"
#include "gui_inventory.h"
#include "../mcommon/p_characterproperty.h"

extern MistClient	g_kMistClient;

void GuiMsgInventoryDlg( string strMainWnd, string strController, 
								unsigned int msg, int numparms, void *params )
{
	if(msg == ZGM_COMMAND)
	{
		if(strMainWnd == "InventoryWnd")
		{
			g_kMistClient.m_pkInventoryDlg->OnCommand(strController);
		}
	}

	if(msg == ZGM_MOUSEMOVE)
	{
		int pressed = ((int*)params)[0];
		int x = ((int*)params)[1];
		int y = ((int*)params)[2];
		g_kMistClient.m_pkInventoryDlg->OnMouseMove(pressed < 1 ? false : true, x, y);
	}

	if(msg == ZGM_KEYPRESS)
	{
		if( ((int*)params)[0] == KEY_P)
		{
			if(g_kMistClient.m_pkInventoryDlg->IsVisible())
				g_kMistClient.m_pkInventoryDlg->Close(); 
			else
				g_kMistClient.m_pkInventoryDlg->Open(); 
		}
	}
}

InventoryDlg::InventoryDlg() : ICON_WIDTH(32), ICON_HEIGHT(32), UPPER_LEFT(27,87),
										 SLOTTS_HORZ(6), SLOTTS_VERT(12)
{
	m_pkMainWnd = NULL;
	m_pkTexMan = g_kMistClient.m_pkTexMan;
	m_iMoveSlot = -1;
	m_iSelItemID = -1;
	m_iHighestZ = 1000;
}

InventoryDlg::~InventoryDlg()
{
	g_kMistClient.m_pkGui->UnregisterWindow(m_pkMainWnd);
}

MLContainer* InventoryDlg::GetContainer()
{
	Entity* pkPlayer = g_kMistClient.m_pkEntityManager->GetEntityByID(g_kMistClient.m_iCharacterID);

	if(pkPlayer)
	{
		P_CharacterProperty* pkCharacterProperty = 
			(P_CharacterProperty*) pkPlayer->GetProperty("P_CharacterProperty");

		if(pkCharacterProperty)
		{
			return pkCharacterProperty->m_pkInventory;
		}
	}

	return NULL;
}

void InventoryDlg::Open()
{	
	//send iventory request to server
	g_kMistClient.SendRequestIventory();			

	m_bGuiCaptureBeforOpen = g_kMistClient.m_bGuiCapture; // rembember privius gui capture mode

	// load inventory
	if(m_pkMainWnd == NULL)
	{
		if(!g_kMistClient.LoadGuiFromScript("data/script/gui/inventory.lua"))
		{
			printf("Error loading inventory script!\n");
			return;
		}

		m_pkMainWnd = g_kMistClient.GetWnd("InventoryWnd");
	}

	m_pkMainWnd->Show();

	g_kMistClient.GetWnd("OpenInventoryBn")->Hide();
	g_kMistClient.PositionActionButtons();

	ZGuiWnd::m_pkFocusWnd = m_pkMainWnd;
}

void InventoryDlg::Close()
{
	m_pkMainWnd->Hide();

	// Must set focus on mainwnd to recive SPACE intput for chatbox...
	g_kMistClient.m_pkGui->SetFocus(g_kMistClient.GetWnd("GuiMainWnd"), false);	

	g_kMistClient.GetWnd("OpenInventoryBn")->Show();
	g_kMistClient.PositionActionButtons();

}

void InventoryDlg::OnCommand(string strController)
{
	if(strController == "InventoryCloseButton")
		Close();
}

void InventoryDlg::OnMouseMove(bool bLeftButtonPressed, int mx, int my)
{
	if(g_kMistClient.m_pkGui->m_bMouseRightPressed)
	{
		m_iSelItemID = -1;
	}

	for(int i=0; i<m_vkItemList.size(); i++)
	{		
		if(m_vkItemList[i].pkWnd->GetScreenRect().Inside(mx, my))
		{
			if(m_iMoveSlot == -1)
				m_vkItemList[i].pkWnd->GetSkin()->m_unBorderSize = 2;

			if(bLeftButtonPressed)
			{
				if(m_iMoveSlot == -1)
				{
					m_kPosBeforeMove.x = m_vkItemList[i].pkWnd->GetWndRect().Left;
					m_kPosBeforeMove.y = m_vkItemList[i].pkWnd->GetWndRect().Top;

					m_iMoveSlot = i; // select new item
					m_vkItemList[i].pkWnd->m_iZValue = m_iHighestZ++;
					m_pkMainWnd->SortChilds(); 
				}
			}
			else
			{
				if(m_iMoveSlot != -1)
					OnDropItem();

				m_iMoveSlot = -1;
			}	

			if(g_kMistClient.m_pkGui->m_bMouseRightPressed)
			{	
				m_iSelItemID = m_vkItemList[i].iItemID;
			}
		}
		else
		{
			if( m_vkItemList[i].iItemID != m_iSelItemID)
				m_vkItemList[i].pkWnd->GetSkin()->m_unBorderSize = 0;
		}
	}

	if(m_iMoveSlot != -1)
	{
		m_vkItemList[m_iMoveSlot].pkWnd->SetPos(mx,my, true, true);
	}
}

void InventoryDlg::Update(vector<MLContainerInfo>& vkItemList)
{
	m_iHighestZ = 1000;
	m_iMoveSlot = -1;

	// Remove all slots.
	for(int i=0; i<m_vkItemList.size(); i++)
	{
		ZGuiWnd* pkWnd = m_vkItemList[i].pkWnd;
		delete pkWnd->GetSkin();
		g_kMistClient.m_pkGui->UnregisterWindow( pkWnd );
	}

	m_vkItemList.clear();

	// Rebuild slotlist.
	int x,y,w,h;
	char szItemName[128];
	for(int i=0; i<vkItemList.size(); i++)
	{
		sprintf(szItemName, "InventoryItemLabel%i", i);
		x = UPPER_LEFT.x + vkItemList[i].m_cItemX * ICON_WIDTH + vkItemList[i].m_cItemX;
		y = UPPER_LEFT.y + vkItemList[i].m_cItemY * ICON_HEIGHT + vkItemList[i].m_cItemY;
		w = vkItemList[i].m_cItemW * ICON_WIDTH;
		h = vkItemList[i].m_cItemH * ICON_HEIGHT;

		ZGuiWnd* pkNewSlot = g_kMistClient.CreateWnd(Label, 
			szItemName, "", m_pkMainWnd, x, y, w, h, 0);
		pkNewSlot->Show();

		pkNewSlot->SetSkin(new ZGuiSkin());
		pkNewSlot->GetSkin()->m_iBkTexID = m_pkTexMan->Load(
			string(string("data/textures/gui/items/") + vkItemList[i].m_strIcon).c_str(), 0) ;	

		if(m_iSelItemID == vkItemList[i].m_iItemID)
			pkNewSlot->GetSkin()->m_unBorderSize = 2;

		ITEM_SLOT kNewSlot;
		kNewSlot.pkWnd = pkNewSlot;
		kNewSlot.iItemID = vkItemList[i].m_iItemID;
		m_vkItemList.push_back(kNewSlot);

	}
}

// When dropping a item, check for collision, reposition it and update the inventory.
void InventoryDlg::OnDropItem()
{
	Rect rcMain = m_vkItemList[m_iMoveSlot].pkWnd->GetWndRect();
	Rect rc = m_vkItemList[m_iMoveSlot].pkWnd->GetWndRect();

	if(rc.Left < UPPER_LEFT.x) rc.Left = UPPER_LEFT.x;
	if(rc.Top < UPPER_LEFT.y) rc.Top = UPPER_LEFT.y;

	int slot_w = rc.Width() / ICON_WIDTH;
	int slot_h = rc.Height() / ICON_HEIGHT;

	int slot_x = (rc.Left - UPPER_LEFT.x) / ICON_WIDTH;
	int slot_y = (rc.Top  - UPPER_LEFT.y) / ICON_HEIGHT;

	if(slot_x > SLOTTS_HORZ-slot_w) slot_x = SLOTTS_HORZ-slot_w;
	if(slot_y > SLOTTS_VERT-slot_h) slot_y = SLOTTS_VERT-slot_h;

	int x = UPPER_LEFT.x + slot_x * ICON_WIDTH + slot_x;
	int y = UPPER_LEFT.y + slot_y * ICON_HEIGHT + slot_y;

	bool bMoveOK = true;
	for(int i=0; i<m_vkItemList.size(); i++)
	{
		if(i!=m_iMoveSlot)
		{
			if(m_vkItemList[i].pkWnd->GetWndRect().Inside(x,y) ||
				m_vkItemList[i].pkWnd->GetWndRect().Inside(x+rc.Width()-1,y) ||
				m_vkItemList[i].pkWnd->GetWndRect().Inside(x,y+rc.Height()-1) ||
				m_vkItemList[i].pkWnd->GetWndRect().Inside(x+rc.Width()-1,y+rc.Height()-1))
			{
				// TODO: Check if collision slot is a container or ar stackable item.
				bMoveOK = false; 
				break;
			}
		}
	}

	if(bMoveOK)
	{
		m_vkItemList[m_iMoveSlot].pkWnd->SetPos(x, y, false, true);
		MLContainer* pkContainer = GetContainer();
		pkContainer->MoveItem(m_vkItemList[m_iMoveSlot].iItemID, 4, 8);
	}
	else
	{
		m_vkItemList[m_iMoveSlot].pkWnd->SetPos(m_kPosBeforeMove.x, 
			m_kPosBeforeMove.y, false, true);		
	}
}