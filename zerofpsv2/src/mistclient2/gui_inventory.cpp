
#include "mistclient.h"
#include "gui_inventory.h"
#include "../mcommon/p_characterproperty.h"

extern MistClient	g_kMistClient;

void GuiMsgInventoryDlg( string strMainWnd, string strController, 
								unsigned int msg, int numparms, void *params )
{
	if(msg == ZGM_COMMAND)
	{
		if(strMainWnd == "InventoryWnd" || strMainWnd == "ContainerWnd")
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

const float BD_R = 1;
const float BD_G = 0;
const float BD_B = 0;

InventoryDlg::InventoryDlg() : ICON_WIDTH(32), ICON_HEIGHT(32), UPPER_LEFT_INVENTORY(27,87),
										 SLOTTS_HORZ_INVENTORY(6), SLOTTS_VERT_INVENTORY(12), UPPER_LEFT_CONTAINER(0,0)
{
	m_pkInventoryWnd = NULL;
	m_pkContainerWnd = NULL;

	m_pkTexMan = g_kMistClient.m_pkTexMan;
	m_kMoveSlot.m_iIndex = -1;
	m_iSelItemID = -1;
	m_iHighestZ = 1000;
	m_iItemUnderCursor = -1;
	m_iActiveContainerID = -1;
}

InventoryDlg::~InventoryDlg()
{
	g_kMistClient.m_pkGui->UnregisterWindow(m_pkInventoryWnd);
}

/*
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
*/

void InventoryDlg::Open()
{	
	m_bGuiCaptureBeforOpen = g_kMistClient.m_bGuiCapture; // rembember privius gui capture mode

	// load inventory
	if(m_pkInventoryWnd == NULL)
	{
		if(!g_kMistClient.LoadGuiFromScript("data/script/gui/inventory.lua"))
		{
			printf("Error loading inventory script!\n");
			return;
		}

		m_pkInventoryWnd = g_kMistClient.GetWnd("InventoryWnd");
	}

	m_pkInventoryWnd->Show();

	g_kMistClient.GetWnd("OpenInventoryBn")->Hide();
	g_kMistClient.PositionActionButtons();

	ZGuiWnd::m_pkFocusWnd = m_pkInventoryWnd;
}

void InventoryDlg::Close()
{
	m_pkInventoryWnd->Hide();
	CloseContainerWnd();

	// Must set focus on mainwnd to recive SPACE intput for chatbox...
	g_kMistClient.m_pkGui->SetFocus(g_kMistClient.GetWnd("GuiMainWnd"), false);	

	g_kMistClient.GetWnd("OpenInventoryBn")->Show();
	g_kMistClient.PositionActionButtons();

	

}

void InventoryDlg::OnCommand(string strController)
{
	if(strController == "InventoryCloseButton")
		Close();
	else
	if(strController == "ContainerCloseButton")
		CloseContainerWnd();
}

void InventoryDlg::OnMouseMove(bool bLeftButtonPressed, int mx, int my)
{
	static bool s_bRightMouseButtonPressed = false;

	if(m_iItemUnderCursor)
	{
		const float WAIT_TIME_PICKUP = 0.25f;

		float fTime = (float) SDL_GetTicks() / 1000.0f;

		if(fTime - m_fPickUpTimer > WAIT_TIME_PICKUP)
		{
			for(int i=0; i<m_vkInventoryItemList.size(); i++)
			{
				if(m_vkInventoryItemList[i].iItemID == m_iItemUnderCursor)
				{
					if(bLeftButtonPressed)
					{
						m_kMoveSlot.bIsInventoryItem = true;
						m_kMoveSlot.m_iIndex = i;
					}

					m_vkInventoryItemList[i].pkWnd->Show();
					m_vkInventoryItemList[i].pkWnd->m_iZValue = m_iHighestZ++;
					m_pkInventoryWnd->SortChilds(); 
					break;
				}
			}

			m_iItemUnderCursor = -1;
		}
	}

	//if(g_kMistClient.m_pkGui->m_bMouseRightPressed)
	//{
	//	m_iSelItemID = -1;
	//}

	for(int i=0; i<m_vkInventoryItemList.size(); i++)
	{		
		if(m_vkInventoryItemList[i].pkWnd->GetScreenRect().Inside(mx, my))
		{
			if(m_kMoveSlot.m_iIndex == -1)
				m_vkInventoryItemList[i].pkWnd->GetSkin()->m_unBorderSize = 1;

			if(bLeftButtonPressed)
			{
				if(m_kMoveSlot.m_iIndex == -1)
				{
					m_kPosBeforeMove.x = m_vkInventoryItemList[i].pkWnd->GetWndRect().Left;
					m_kPosBeforeMove.y = m_vkInventoryItemList[i].pkWnd->GetWndRect().Top;

					m_kMoveSlot.bIsInventoryItem = true;
					m_kMoveSlot.m_iIndex = i; // select new item
					m_vkInventoryItemList[i].pkWnd->m_iZValue = m_iHighestZ++;
					m_pkInventoryWnd->SortChilds(); 
				}
			}
			else
			{
				if(m_kMoveSlot.m_iIndex != -1)
					OnDropItem();

				m_kMoveSlot.m_iIndex = -1;
			}	
			
			if(g_kMistClient.m_pkGui->m_bMouseRightPressed && s_bRightMouseButtonPressed == false)
			{	
				if(m_iActiveContainerID == m_vkInventoryItemList[i].iItemID)
				{
					CloseContainerWnd();
					if(m_iSelItemID == m_vkInventoryItemList[i].iItemID)
					{
						m_iSelItemID = -1;
						m_vkInventoryItemList[i].pkWnd->GetSkin()->m_unBorderSize = 0;
					}
				}
				else
				{
					g_kMistClient.SendRequestContainer(m_vkInventoryItemList[i].iItemID);
					m_iSelItemID = m_vkInventoryItemList[i].iItemID;
					m_vkInventoryItemList[i].pkWnd->GetSkin()->m_unBorderSize = 1;
				}

				s_bRightMouseButtonPressed = true;
			}
			else if(!g_kMistClient.m_pkGui->m_bMouseRightPressed)
			{
				s_bRightMouseButtonPressed = false;
			}

		}
		else
		{
			if( m_vkInventoryItemList[i].iItemID != m_iSelItemID)
				m_vkInventoryItemList[i].pkWnd->GetSkin()->m_unBorderSize = 0;
		}
	}

	for(int i=0; i<m_vkContainerItemList.size(); i++)
	{
		if(m_vkContainerItemList[i].pkWnd->GetScreenRect().Inside(mx, my))
		{
			if(m_kMoveSlot.m_iIndex == -1)
				m_vkContainerItemList[i].pkWnd->GetSkin()->m_unBorderSize = 1;

			if(bLeftButtonPressed)
			{
				if(m_kMoveSlot.m_iIndex == -1)
				{
					m_kPosBeforeMove.x = m_vkContainerItemList[i].pkWnd->GetWndRect().Left;
					m_kPosBeforeMove.y = m_vkContainerItemList[i].pkWnd->GetWndRect().Top;

					m_kMoveSlot.bIsInventoryItem = false;
					m_kMoveSlot.m_iIndex = i; // select new item
					m_vkContainerItemList[i].pkWnd->m_iZValue = m_iHighestZ++;
					m_pkContainerWnd->SortChilds(); 
				}
			}
			else
			{
				if(m_kMoveSlot.m_iIndex != -1)
					OnDropItem();

				m_kMoveSlot.m_iIndex = -1;
			}	

			//if(g_kMistClient.m_pkGui->m_bMouseRightPressed)
			//{	
			//	m_iSelItemID = m_vkContainerItemList[i].iItemID;
			//}
		}
		else
		{
			//if( m_vkContainerItemList[i].iItemID != m_iSelItemID)
				m_vkContainerItemList[i].pkWnd->GetSkin()->m_unBorderSize = 0;
		}		
	}

	if(m_kMoveSlot.m_iIndex != -1)
	{
		if(m_kMoveSlot.bIsInventoryItem == true)
			m_vkInventoryItemList[m_kMoveSlot.m_iIndex].pkWnd->SetPos(mx,my, true, true);
		else
			m_vkContainerItemList[m_kMoveSlot.m_iIndex].pkWnd->SetPos(mx,my, true, true);
	}
}

void InventoryDlg::OpenContainerWnd(int id, char slots_x, char slots_y)
{
	m_pkContainerWnd = g_kMistClient.GetWnd("ContainerWnd");
	m_pkContainerWnd->Show();

	m_iSlotsHorzContainer = slots_x;
	m_iSlotsVertContainer = slots_y;
	m_iActiveContainerID = id;

	CreateContainerGrid(slots_x, slots_y);

//	m_pkContainerWnd->m_bIncludeBorder = true;
	m_pkContainerWnd->SetZValue(22);
	g_kMistClient.GetWnd("ContainerCloseButton")->SetZValue(44);
	
	g_kMistClient.m_pkGui->PlaceWndFrontBack(m_pkContainerWnd, true); 

	g_kMistClient.m_pkGui->SetFocus(m_pkContainerWnd, false);
}

void InventoryDlg::CreateContainerGrid(char slots_horz, char slots_vert)
{
	list<ZGuiWnd*> kChilds;
	m_pkContainerWnd->GetChildrens(kChilds);

	const int MAX_WIDTH = slots_horz*33+1;
	const int MAX_HEIGHT = slots_vert*33+1;

	m_pkContainerWnd->Resize(MAX_WIDTH, MAX_HEIGHT);

	int bdsize = m_pkContainerWnd->GetSkin()->m_unBorderSize; 
	
	Rect rcInventory = m_pkInventoryWnd->GetScreenRect();
	m_pkContainerWnd->SetPos(rcInventory.Left - MAX_WIDTH - bdsize + 16, 
		rcInventory.Bottom - MAX_HEIGHT - bdsize  /*no all is opaque*/, true, true);

	g_kMistClient.GetWnd("ContainerCloseButton")->SetPos(MAX_WIDTH, -16, false, true);
	g_kMistClient.GetWnd("ContainerCloseButton")->Show();

	int current_slot_x=0, current_slot_y=0;
	int dx=0, dy=0;
	
	for(list<ZGuiWnd*>::iterator it=kChilds.begin(); it!=kChilds.end(); it++)
	{
		string strName = (*it)->GetName();

		if(strName.find("CSlotBkLabel_") != string::npos)
		{	
			(*it)->SetPos(dx, dy, false, true);
			(*it)->Resize(99, 99);

			current_slot_x += 3;
			dx += 99;

			if(current_slot_y > slots_vert)
			{
				(*it)->Hide();				
			}
			else
			{
				(*it)->Show();
				(*it)->SetZValue(1);

				Rect rc = (*it)->GetWndRect();

				int w = rc.Width();
				int h = rc.Height();

				if(rc.Right > MAX_WIDTH)  w = MAX_WIDTH-rc.Left;
				if(rc.Bottom > MAX_HEIGHT) h = MAX_HEIGHT-rc.Top;

				(*it)->Resize(w, h);
			}

			if(current_slot_x > slots_horz)
			{
				current_slot_x = 0;
				current_slot_y += 3;
				dx = 0;
				dy += 99;
			}
		}
	}
}

void InventoryDlg::UpdateInventory(vector<MLContainerInfo>& vkItemList)
{
	m_iHighestZ = 1000;
	m_kMoveSlot.m_iIndex = -1;

	// Remove all slots.
	for(int i=0; i<m_vkInventoryItemList.size(); i++)
	{
		ZGuiWnd* pkWnd = m_vkInventoryItemList[i].pkWnd;
		delete pkWnd->GetSkin();
		g_kMistClient.m_pkGui->UnregisterWindow( pkWnd );
	}

	m_vkInventoryItemList.clear();

	// Rebuild slotlist.
	int x,y,w,h;
	char szItemName[128];
	for(int i=0; i<vkItemList.size(); i++)
	{
		sprintf(szItemName, "InventoryItemLabel%i", i);
		x = UPPER_LEFT_INVENTORY.x + vkItemList[i].m_cItemX * ICON_WIDTH + vkItemList[i].m_cItemX;
		y = UPPER_LEFT_INVENTORY.y + vkItemList[i].m_cItemY * ICON_HEIGHT + vkItemList[i].m_cItemY;
		w = vkItemList[i].m_cItemW * (ICON_WIDTH) + vkItemList[i].m_cItemW-1;
		h = vkItemList[i].m_cItemH * (ICON_HEIGHT) + vkItemList[i].m_cItemH-1;

		ZGuiWnd* pkNewSlot = g_kMistClient.CreateWnd(Label, 
			szItemName, "", m_pkInventoryWnd, x, y, w, h, 0);
		pkNewSlot->Show();

		if(g_kMistClient.m_pkGui->m_bMouseLeftPressed)
		{
			if(m_iItemUnderCursor == vkItemList[i].m_iItemID)
			{
				float fTime = (float) SDL_GetTicks() / 1000.0f;
				m_fPickUpTimer = fTime;
				pkNewSlot->Hide();
			}
		}

		pkNewSlot->SetSkin(new ZGuiSkin());
		pkNewSlot->GetSkin()->m_iBkTexID = m_pkTexMan->Load(
			string(string("data/textures/gui/items/") + vkItemList[i].m_strIcon).c_str(), 0) ;
		pkNewSlot->GetSkin()->m_bTileBkSkin = 0;
		pkNewSlot->GetSkin()->m_afBorderColor[0] = BD_R;
		pkNewSlot->GetSkin()->m_afBorderColor[1] = BD_G;
		pkNewSlot->GetSkin()->m_afBorderColor[2] = BD_B;

		if(m_iSelItemID == vkItemList[i].m_iItemID)
			pkNewSlot->GetSkin()->m_unBorderSize = 1;

		ITEM_SLOT kNewSlot;
		kNewSlot.pkWnd = pkNewSlot;
		kNewSlot.iItemID = vkItemList[i].m_iItemID;
		m_vkInventoryItemList.push_back(kNewSlot);
	}	
}

void InventoryDlg::UpdateContainer(vector<MLContainerInfo>& vkItemList)
{
	//m_iHighestZ = 1000;
	//m_iMoveSlot = -1;

	// Remove all slots.
	for(int i=0; i<m_vkContainerItemList.size(); i++)
	{
		ZGuiWnd* pkWnd = m_vkContainerItemList[i].pkWnd;
		delete pkWnd->GetSkin();
		g_kMistClient.m_pkGui->UnregisterWindow( pkWnd );
	}

	m_vkContainerItemList.clear();

	// Rebuild slotlist.
	int x,y,w,h;
	char szItemName[128];
	for(int i=0; i<vkItemList.size(); i++)
	{
		sprintf(szItemName, "ContainerItemLabel%i", i);
		x = 1+ UPPER_LEFT_CONTAINER.x + vkItemList[i].m_cItemX * ICON_WIDTH + vkItemList[i].m_cItemX;
		y = 1+ UPPER_LEFT_CONTAINER.y + vkItemList[i].m_cItemY * ICON_HEIGHT + vkItemList[i].m_cItemY;
		w = vkItemList[i].m_cItemW * (ICON_WIDTH) + vkItemList[i].m_cItemW-1;
		h = vkItemList[i].m_cItemH * (ICON_HEIGHT) + vkItemList[i].m_cItemH-1;

		ZGuiWnd* pkNewSlot = g_kMistClient.CreateWnd(Label, 
			szItemName, "", m_pkContainerWnd, x, y, w, h, 0);
		pkNewSlot->Show();
		pkNewSlot->SetZValue(12121);

		//if(g_kMistClient.m_pkGui->m_bMouseLeftPressed)
		//{
		//	if(m_iItemUnderCursor == vkItemList[i].m_iItemID)
		//	{
		//		float fTime = (float) SDL_GetTicks() / 1000.0f;
		//		m_fPickUpTimer = fTime;
		//		pkNewSlot->Hide();
		//	}
		//}

		pkNewSlot->SetSkin(new ZGuiSkin());
		pkNewSlot->GetSkin()->m_bTileBkSkin = 0;
		pkNewSlot->GetSkin()->m_iBkTexID = m_pkTexMan->Load(
			string(string("data/textures/gui/items/") + vkItemList[i].m_strIcon).c_str(), 0) ;	

		pkNewSlot->GetSkin()->m_afBorderColor[0] = BD_R;
		pkNewSlot->GetSkin()->m_afBorderColor[1] = BD_G;
		pkNewSlot->GetSkin()->m_afBorderColor[2] = BD_B;

		if(m_iSelItemID == vkItemList[i].m_iItemID)
			pkNewSlot->GetSkin()->m_unBorderSize = 2;

		ITEM_SLOT kNewSlot;
		kNewSlot.pkWnd = pkNewSlot;
		kNewSlot.iItemID = vkItemList[i].m_iItemID;
		m_vkContainerItemList.push_back(kNewSlot);
	}

	m_pkContainerWnd->SortChilds();
}

// When dropping a item, check for collision, reposition it and update the inventory.
void InventoryDlg::OnDropItem()
{
	Point upper_left;
	Rect rcMain, rc, rcScreenMain, rcDropWnd;

	int iSlotsHorz, iSlotsVert;

	if(m_kMoveSlot.bIsInventoryItem)
	{
		upper_left = UPPER_LEFT_INVENTORY;

		rcMain = m_vkInventoryItemList[m_kMoveSlot.m_iIndex].pkWnd->GetWndRect();
		rc = m_vkInventoryItemList[m_kMoveSlot.m_iIndex].pkWnd->GetWndRect();
		rcScreenMain = m_pkInventoryWnd->GetScreenRect();
		rcDropWnd = m_vkInventoryItemList[m_kMoveSlot.m_iIndex].pkWnd->GetScreenRect();

		iSlotsHorz = SLOTTS_HORZ_INVENTORY;
		iSlotsVert = SLOTTS_VERT_INVENTORY;
	}
	else
	{
		upper_left = UPPER_LEFT_CONTAINER;

		rcMain = m_vkContainerItemList[m_kMoveSlot.m_iIndex].pkWnd->GetWndRect();
		rc = m_vkContainerItemList[m_kMoveSlot.m_iIndex].pkWnd->GetWndRect();

		rcScreenMain = m_pkContainerWnd->GetScreenRect();
		rcDropWnd = m_vkContainerItemList[m_kMoveSlot.m_iIndex].pkWnd->GetScreenRect();

		iSlotsHorz = m_iSlotsHorzContainer;
		iSlotsVert = m_iSlotsVertContainer;
	}

	if(!rcScreenMain.Inside(rcDropWnd.Left, rcDropWnd.Top))
	{
		if(m_kMoveSlot.bIsInventoryItem)
		{
			if(m_pkContainerWnd && m_pkContainerWnd->IsVisible() && 
				m_pkContainerWnd->GetScreenRect().Inside(rcDropWnd.Left, rcDropWnd.Top))
			{
				if(Entity* pkCharacter = g_kMistClient.m_pkEntityManager->GetEntityByID(g_kMistClient.m_iCharacterID))
				{
					if(P_CharacterProperty* pkCharProp = (P_CharacterProperty*)pkCharacter->GetProperty("P_CharacterProperty"))
					{
						g_kMistClient.SendMoveItem(
							m_vkInventoryItemList[m_kMoveSlot.m_iIndex].iItemID,m_iActiveContainerID,-1,-1);
						m_vkInventoryItemList[m_kMoveSlot.m_iIndex].pkWnd->Hide();	
						m_kMoveSlot.m_iIndex = -1;
						return;
					}
				}
			}
			else
			{
				g_kMistClient.SendMoveItem(m_vkInventoryItemList[m_kMoveSlot.m_iIndex].iItemID, -1, -1, -1);

				for(int i=0; i<m_vkInventoryItemList.size(); i++)
				{
					if(m_kMoveSlot.m_iIndex == i)
					{
						m_vkInventoryItemList[i].pkWnd->Hide();				
						m_kMoveSlot.m_iIndex = -1;
						break;
					}
				}
			}
		}
		else
		{
			if(m_pkInventoryWnd->GetScreenRect().Inside(rcDropWnd.Left, rcDropWnd.Top))
			{
				if(Entity* pkCharacter = g_kMistClient.m_pkEntityManager->GetEntityByID(g_kMistClient.m_iCharacterID))
				{
					if(P_CharacterProperty* pkCharProp = (P_CharacterProperty*)pkCharacter->GetProperty("P_CharacterProperty"))
					{
						g_kMistClient.SendMoveItem(
							m_vkContainerItemList[m_kMoveSlot.m_iIndex].iItemID,pkCharProp->m_iInventory,-1,-1);
						m_vkContainerItemList[m_kMoveSlot.m_iIndex].pkWnd->Hide();	
						m_kMoveSlot.m_iIndex = -1;
						return;
					}
				}
			}
			else
			{
				g_kMistClient.SendMoveItem(m_vkContainerItemList[m_kMoveSlot.m_iIndex].iItemID, -1, -1, -1);

				for(int i=0; i<m_vkContainerItemList.size(); i++)
				{
					if(m_kMoveSlot.m_iIndex == i)
					{
						m_vkContainerItemList[i].pkWnd->Hide();				
						m_kMoveSlot.m_iIndex = -1;
						break;
					}
				}
			}
		}
	}
	else
	{
		if(rc.Left < upper_left.x) rc.Left = upper_left.x;
		if(rc.Top < upper_left.y) rc.Top = upper_left.y;

		int slot_w = rc.Width() / ICON_WIDTH;
		int slot_h = rc.Height() / ICON_HEIGHT;

		int slot_x = (rc.Left - upper_left.x) / ICON_WIDTH;
		int slot_y = (rc.Top  - upper_left.y) / ICON_HEIGHT;

		if(slot_x > iSlotsHorz-slot_w) slot_x = iSlotsHorz - slot_w;
		if(slot_y > iSlotsVert-slot_h) slot_y = iSlotsVert - slot_h;

		int x = upper_left.x + slot_x * ICON_WIDTH + slot_x;
		int y = upper_left.y + slot_y * ICON_HEIGHT + slot_y;

		bool bCollision = TestForCollision(m_kMoveSlot.m_iIndex, m_kMoveSlot.bIsInventoryItem);

		if(bCollision == false)
		{
			if(m_kMoveSlot.bIsInventoryItem)
			{
				m_vkInventoryItemList[m_kMoveSlot.m_iIndex].pkWnd->SetPos(x, y, false, true);
				g_kMistClient.SendMoveItem(m_vkInventoryItemList[m_kMoveSlot.m_iIndex].iItemID, 
					-1, slot_x, slot_y);
			}
			else
			{
				m_vkContainerItemList[m_kMoveSlot.m_iIndex].pkWnd->SetPos(x, y, false, true);
				g_kMistClient.SendMoveItem(m_vkContainerItemList[m_kMoveSlot.m_iIndex].iItemID, 
					-1, slot_x, slot_y);
			}
		}
		else
		{
			if(m_kMoveSlot.bIsInventoryItem)
			{
				m_vkInventoryItemList[m_kMoveSlot.m_iIndex].pkWnd->SetPos(m_kPosBeforeMove.x, 
					m_kPosBeforeMove.y, false, true);		
			}
			else
			{
				m_vkContainerItemList[m_kMoveSlot.m_iIndex].pkWnd->SetPos(m_kPosBeforeMove.x, 
					m_kPosBeforeMove.y, false, true);	
			}
		}
	}
}

void InventoryDlg::CloseContainerWnd()
{
	if(m_pkContainerWnd)
	{
		m_pkContainerWnd->Hide();
		g_kMistClient.GetWnd("ContainerCloseButton")->Hide();
		g_kMistClient.m_pkGui->SetFocus(m_pkInventoryWnd, false);
		m_iActiveContainerID = -1;
	}
}



bool InventoryDlg::TestForCollision(int iTestSlot, bool bInventory)
{
	Point test_slot, test_size;

	if(bInventory)
	{
		test_slot = SlotFromWnd(m_vkInventoryItemList[iTestSlot].pkWnd, true);
		test_size = SlotSizeFromWnd(m_vkInventoryItemList[iTestSlot].pkWnd);
	}
	else
	{
		test_slot = SlotFromWnd(m_vkContainerItemList[iTestSlot].pkWnd, false);
		test_size = SlotSizeFromWnd(m_vkContainerItemList[iTestSlot].pkWnd);
	}

	vector<Point> kSlotsTaken;

	if(bInventory)
	{
		for(int i=0; i<m_vkInventoryItemList.size(); i++)
		{
			if( i != iTestSlot)
			{
				Point kSlot = SlotFromWnd(m_vkInventoryItemList[i].pkWnd, true);
				Point kSlotSize = SlotSizeFromWnd(m_vkInventoryItemList[i].pkWnd);

				for(int y=0; y<kSlotSize.y; y++)
					for(int x=0; x<kSlotSize.x; x++)
						kSlotsTaken.push_back(Point(kSlot.x+x, kSlot.y+y));
			}
		}
	}
	else
	{
		for(int i=0; i<m_vkContainerItemList.size(); i++)
		{
			if( i != iTestSlot)
			{
				Point kSlot = SlotFromWnd(m_vkContainerItemList[i].pkWnd, false);
				Point kSlotSize = SlotSizeFromWnd(m_vkContainerItemList[i].pkWnd);

				for(int y=0; y<kSlotSize.y; y++)
					for(int x=0; x<kSlotSize.x; x++)
						kSlotsTaken.push_back(Point(kSlot.x+x, kSlot.y+y));
			}
		}
	}

	for(int i=0; i<kSlotsTaken.size(); i++)
	{
		for(int y=0; y<test_size.y; y++)
			for(int x=0; x<test_size.x; x++)
			{
				Point t(test_slot.x + x, test_slot.y + y);

				if(t == kSlotsTaken[i])
					return true;
			}
	}

	return false;

}

Point InventoryDlg::SlotFromWnd(ZGuiWnd* pkWnd, bool bInventory)
{
	Rect rcTest = pkWnd->GetScreenRect();
	return SlotFromScreenPos(rcTest.Left, rcTest.Top, bInventory);
}

Point InventoryDlg::SlotSizeFromWnd(ZGuiWnd* pkWnd)
{
	Rect rcTest = pkWnd->GetScreenRect();
	return Point(rcTest.Width()/ICON_WIDTH, rcTest.Height()/ICON_HEIGHT);
}

Point InventoryDlg::SlotFromScreenPos(int x, int y, bool bInventory)
{
	int slot_x, slot_y;

	if(bInventory)
	{
		slot_x = (x - m_pkInventoryWnd->GetScreenRect().Left - UPPER_LEFT_INVENTORY.x) / ICON_WIDTH; 
		slot_y = (y - m_pkInventoryWnd->GetScreenRect().Top - UPPER_LEFT_INVENTORY.y) / ICON_HEIGHT;
	}
	else
	{
		slot_x = (x - m_pkContainerWnd->GetScreenRect().Left - UPPER_LEFT_CONTAINER.x) / ICON_WIDTH; 
		slot_y = (y - m_pkContainerWnd->GetScreenRect().Top - UPPER_LEFT_CONTAINER.y) / ICON_HEIGHT;
	}

	return Point(slot_x, slot_y);
}