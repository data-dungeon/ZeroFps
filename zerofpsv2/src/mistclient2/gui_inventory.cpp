
#include "mistclient.h"
#include "gui_inventory.h"
#include "../mcommon/p_characterproperty.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/engine/inputhandle.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "gui_equipwnd.h"

extern MistClient	g_kMistClient;

void GuiMsgInventoryDlg( string strMainWnd, string strController, 
								unsigned int msg, int numparms, void *params )
{
	if(msg == ZGM_COMMAND)
	{
		//if(strMainWnd == "InventoryWnd" || strMainWnd == "ContainerWnd" || strMainWnd == "SplitStockWnd")
			g_kMistClient.m_pkInventoryDlg->OnCommand(strController);
	}
	else
	if(msg == ZGM_MOUSEMOVE)
	{
		int pressed = ((int*)params)[0], x = ((int*)params)[1], y = ((int*)params)[2];
		g_kMistClient.m_pkInventoryDlg->OnMouseMove(pressed < 1 ? false : true, x, y);
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
			if( strController == "SplitStockSplitEb")
				g_kMistClient.m_pkInventoryDlg->CloseSplitStockWnd(true);
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
			else
			if( strController == "SplitStockSplitEb")
				g_kMistClient.m_pkInventoryDlg->CloseSplitStockWnd(false);

			g_kMistClient.m_pkGui->m_bForceGUICapture = false;
			break;	
		}
	}
	else 
	if(msg == ZGM_EN_CHANGE)
	{
		if( strController == "SplitStockSplitEb")
		{
			if(g_kMistClient.m_pkInventoryDlg->m_pkSplitStockWnd && 
				g_kMistClient.m_pkInventoryDlg->m_pkSplitStockWnd->IsVisible())
			{
				printf("Got textbox message form %s\n", strController.c_str());		
				
				bool bSuccesss;
				int iCount = g_kMistClient.GetTextInt("SplitStockSplitEb", &bSuccesss);
				if(bSuccesss)
				{	
					g_kMistClient.SetTextInt("SplitStockMaxEb", 
						g_kMistClient.m_pkInventoryDlg->m_iSplitShareMax-iCount);  
				}				
			}
		}		
	}
	else
	if(msg == ZGM_LBUTTONDBLCLK)
	{	
		int x = ((int*)params)[0], y = ((int*)params)[1];
		g_kMistClient.m_pkInventoryDlg->OnDoubleClick(x,y);		
	}

}

InventoryDlg::InventoryDlg() : SLOTTS_HORZ_INVENTORY(6), 
										 SLOTTS_VERT_INVENTORY(9), 
										 BD_R(1), BD_G(1), BD_B(1),
										 ICON_WIDTH(32), // in pixels
										 ICON_HEIGHT(32), // in pixels
										 UPPERLEFT_INVENTORY(39,193), // in pixels
										 UPPERLEFT_CONTAINER( 0,  0) // in pixels
{
	m_bDoubleClicked = false;
	m_pkInventoryWnd = NULL;
	m_pkContainerWnd = NULL;
	m_pkSplitStockWnd = NULL;

	m_pkTexMan = g_kMistClient.m_pkTexMan;
	m_kMoveSlot.m_iIndex = -1;
	m_kSplitSlot.m_iIndex =-1;
	m_iSelItemID = -1;
	m_iHighestZ = 1000;
	m_iItemUnderCursor = -1;
	m_iActiveContainerID = -1;
	m_fPickUpTimer = 0;

	m_iSplitShareMax = 0;

	m_kItemUnderInspection.second = -1;


}

InventoryDlg::~InventoryDlg()
{
	g_kMistClient.m_pkGui->UnregisterWindow(m_pkContainerWnd);
	g_kMistClient.m_pkGui->UnregisterWindow(m_pkInventoryWnd);
}

void InventoryDlg::Open()
{	
	// load inventory
	if(m_pkInventoryWnd == NULL)
	{
		//if(!g_kMistClient.LoadGuiFromScript(g_kMistClient.m_kGuiScrips[GSF_INVENTORY].c_str()))
		//{
		//	printf("Error loading inventory script!\n");
		//	return;
		//}

		m_pkInventoryWnd = g_kMistClient.GetWnd("InventoryWnd");
		m_pkContainerWnd = g_kMistClient.GetWnd("ContainerWnd");
		m_pkSplitStockWnd = g_kMistClient.GetWnd("SplitStockWnd");

		m_pkContainerWnd->GetSkin()->m_unBorderSize = (int)(32.0f*g_kMistClient.GetScaleX()); 

		CreateContainerGrid();
	}

	m_ppCursorSkin = g_kMistClient.m_pkGui->GetCursorSkin();

	// visa inventory fönstret
	m_pkInventoryWnd->Show();
	g_kMistClient.m_pkGui->SetFocus(m_pkInventoryWnd, false);	


}

void InventoryDlg::Close()
{
	// Hide inventory window
	if(m_pkInventoryWnd)
		m_pkInventoryWnd->Hide();

	// Close contatiner window.
	CloseContainerWnd();

	// Close splitstock wnd
	CloseSplitStockWnd();

	// Must set focus on mainwnd to recive SPACE intput for chatbox...
	g_kMistClient.m_pkGui->SetFocus(g_kMistClient.GetWnd("GuiMainWnd"), false);	

	m_iItemUnderCursor = -1;

	g_kMistClient.m_pkGui->GetToolTip()->CloseAll();
}

void InventoryDlg::CloseContainerWnd()
{
	if(m_pkContainerWnd)
	{
		m_pkContainerWnd->Hide();
		g_kMistClient.m_pkGui->SetFocus(m_pkInventoryWnd, false);
		m_iActiveContainerID = -1;
	}
}

void InventoryDlg::OnCommand(string strController)
{
	if(strController == "InventoryCloseButton")
		Close();
	else
	if(strController == "ContainerCloseButton")
		CloseContainerWnd();
	else
	if(strController == "SplitStockOKBn")
		CloseSplitStockWnd(true);
	else
	if(strController == "SplitStockCancelBn")
		CloseSplitStockWnd(false);
	else
	if(strController == "InfoWndCloseButton")
	{
		ITEM_INFO kEmpty;
		OpenItemInfoWnd(false, kEmpty);
	}
}

void InventoryDlg::OnMouseMove(bool bLeftButtonPressed, int mx, int my)
{
	if(m_pkSplitStockWnd && g_kMistClient.m_pkGui->GetWndCapture() == m_pkSplitStockWnd)
		return;

	static bool s_bRightMouseButtonPressed = false;

	if(m_iItemUnderCursor) // the application have found a item 		
		PickUpFromGround(bLeftButtonPressed, mx, my); // test if its time to place it under cursor.

	int iCursorBordeSize = 0;
	
	if(m_pkInventoryWnd->IsVisible()) // kolla så att inventory är synligt
	{
		for(int i=0; i<m_vkInventoryItemList.size(); i++)
		{		
			if(m_vkInventoryItemList[i].pkWnd->GetScreenRect().Inside(mx, my)) // cursor is inside the 
			{																						 // rectangle of the slot.
				iCursorBordeSize = 2;
				SetSelectionBorder(i, true, false);

				if(bLeftButtonPressed)
				{												
					if(m_kMoveSlot.m_iIndex == -1)
						PickUpFromGrid(i, true, mx, my); // try to find item under cursor and set as move item.
				}
				else
				{
					if(m_bDoubleClicked == false)
					{
						if(m_kMoveSlot.m_iIndex != -1) // if we have a move slot...
							OnDropItem(mx, my); // place it somewhere.
					}

					m_kMoveSlot.m_iIndex = -1; // mark that we no longer have a moveslot.
				}	
				
				// Open or close container in grid.
				if(g_kMistClient.m_pkGui->m_bMouseRightPressed && s_bRightMouseButtonPressed == false)
				{						
					s_bRightMouseButtonPressed = true;

					if(m_vkInventoryItemList[i].bIsContainer && 
						m_iActiveContainerID == m_vkInventoryItemList[i].iItemID)	
					{
						OpenContainerItem(false, i, true);												
					}
					else
					{
						m_kItemUnderInspection.first = i;
						m_kItemUnderInspection.second = true;

						g_kMistClient.RequestItemInfo(m_vkInventoryItemList[i].iItemID);
						//OpenItemInfoWnd(true);
					}
					//if(m_vkInventoryItemList[i].iStackSize > 1)
					//{
					//	m_kSplitSlot.m_iIndex = i;
					//	m_kSplitSlot.bIsInventoryItem = true;
					//	m_kSplitSlotTarget.m_iIndex = -1;
					//	OpenSplitStockWnd();
					//}
				}
				else if(!g_kMistClient.m_pkGui->m_bMouseRightPressed)
					s_bRightMouseButtonPressed = false;

			}
			else // cursor is not inside the slots rectangle.
			{		
				// remove selection border.
				if( m_vkInventoryItemList[i].iItemID != m_iSelItemID)
					SetSelectionBorder(i, true, true);
			}
		}
	}

	if(m_pkContainerWnd->IsVisible()) // kolla så att containerfönstret är synligt
	{
		for(int i=0; i<m_vkContainerItemList.size(); i++)
		{
			if(m_vkContainerItemList[i].pkWnd->GetScreenRect().Inside(mx, my))
			{
				iCursorBordeSize = 2;
				SetSelectionBorder(i, false, false);

				if(bLeftButtonPressed)
				{				
					if(m_kMoveSlot.m_iIndex == -1) // inget slot under cursorn, plocka upp.
						PickUpFromGrid(i,false,mx,my); 
				}
				else
				{
					if(m_kMoveSlot.m_iIndex != -1)
						OnDropItem(mx, my);

					m_kMoveSlot.m_iIndex = -1;
				}

				// Open or close container in grid.
				if(g_kMistClient.m_pkGui->m_bMouseRightPressed && s_bRightMouseButtonPressed == false)
				{	
					s_bRightMouseButtonPressed = true;

					//if(m_vkContainerItemList[i].bIsContainer)
					//{
					//	bool bOpen = true;
					//	if(m_iActiveContainerID == m_vkContainerItemList[i].iItemID) // klickat på samma container
					//		bOpen = false;															 // som redan är öppnad?

					//	OpenContainerItem(bOpen, i, false);		
					//}
					//else
					//if(m_vkContainerItemList[i].iStackSize > 1)
					//{
					//	m_kSplitSlot.m_iIndex = i;
					//	m_kSplitSlot.bIsInventoryItem = false;
					//	m_kSplitSlotTarget.m_iIndex = -1;
					//	OpenSplitStockWnd();
					//}

					if(m_vkContainerItemList[i].bIsContainer && 
						m_iActiveContainerID == m_vkContainerItemList[i].iItemID)	
					{
						OpenContainerItem(false, i, false);												
					}
					else
					{
						g_kMistClient.RequestItemInfo(m_vkContainerItemList[i].iItemID);
						m_kItemUnderInspection.first = i;
						m_kItemUnderInspection.second = false;
						//OpenItemInfoWnd(true);
					}
				}
				else if(!g_kMistClient.m_pkGui->m_bMouseRightPressed)
					s_bRightMouseButtonPressed = false;
			}
			else
			{
				if( m_vkContainerItemList[i].iItemID != m_iSelItemID)
					SetSelectionBorder(i, false, true);
			}		
		}
	}

	if(m_kMoveSlot.m_iIndex != -1)
	{
		if(m_kMoveSlot.bIsInventoryItem == true)
			m_vkInventoryItemList[m_kMoveSlot.m_iIndex].pkWnd->SetPos(mx,my, true, true);
		else
			m_vkContainerItemList[m_kMoveSlot.m_iIndex].pkWnd->SetPos(mx,my, true, true);

		(*m_ppCursorSkin)->m_unBorderSize = iCursorBordeSize;
	}
	
	(*m_ppCursorSkin)->m_unBorderSize = 0;


	if(bLeftButtonPressed)
	{
		if(g_kMistClient.m_pkEquipmentDlg->GetSlotContainerID(mx, my) != -1)
			(*m_ppCursorSkin)->m_unBorderSize = 1;
	}

	if(m_kMoveSlot.m_iIndex != -1)
		if( (m_pkInventoryWnd->IsVisible() && m_pkInventoryWnd->GetScreenRect().Inside(mx,my) ||
		    (m_pkContainerWnd->IsVisible() && m_pkContainerWnd->GetScreenRect().Inside(mx,my) )) ) 
			(*m_ppCursorSkin)->m_unBorderSize = 1;

	if(m_bDoubleClicked == true)
		m_bDoubleClicked = false;
	
}

void InventoryDlg::PickUpFromGround(bool bLeftButtonPressed, int mx, int my)
{
	if(m_kMoveSlot.m_iIndex != -1) // return if we already have a moveslot.
		return;

	if(bLeftButtonPressed)
	{		
		const float WAIT_TIME_PICKUP = 0.25f; // seconds to wait before item appear under cursor.

		if(g_kMistClient.m_pkZeroFps->GetEngineTime() - m_fPickUpTimer > WAIT_TIME_PICKUP)
		{
			for(int i=0; i<m_vkInventoryItemList.size(); i++)
				if(m_vkInventoryItemList[i].iItemID == m_iItemUnderCursor)
				{
					CloseSplitStockWnd();

					m_kMoveSlot.bIsInventoryItem = true;
					m_kMoveSlot.m_iIndex = i;
				
					Point size = SlotSizeFromWnd(m_vkInventoryItemList[i].pkWnd);
					int id = m_vkInventoryItemList[i].pkWnd->GetSkin()->m_iBkTexID;

					m_kCursorRangeDiff = Point(0,0);
					g_kMistClient.m_pkGui->SetCursor( mx, my, id, -1, size.x*32, size.y*32);
					break;
				}

			m_iItemUnderCursor = -1;
		}
	}
	else
	{
		for(int i=0; i<m_vkInventoryItemList.size(); i++)
			if(m_vkInventoryItemList[i].iItemID == m_iItemUnderCursor)
			{
				m_vkInventoryItemList[i].pkWnd->Show();
				break;
			}

		m_iItemUnderCursor = -1;
	}
}

void InventoryDlg::PickUpFromGrid(int iSlotIndex, bool bInventory, int mx, int my)
{
	vector<ITEM_SLOT>* pkVector;
	if(bInventory)
		pkVector = &m_vkInventoryItemList;
	else
		pkVector = &m_vkContainerItemList;

	m_kCursorRangeDiff.x = mx-(*pkVector)[iSlotIndex].pkWnd->GetScreenRect().Left; 
	m_kCursorRangeDiff.y = my-(*pkVector)[iSlotIndex].pkWnd->GetScreenRect().Top;

	m_kItemWndPosBeforeMove.x = (*pkVector)[iSlotIndex].pkWnd->GetWndRect().Left;
	m_kItemWndPosBeforeMove.y = (*pkVector)[iSlotIndex].pkWnd->GetWndRect().Top;

	g_kMistClient.m_pkEquipmentDlg->HighlightSlot( (*pkVector)[iSlotIndex].iItemType );

	m_kMoveSlot.bIsInventoryItem = bInventory;
	m_kMoveSlot.m_iIndex = iSlotIndex; // select new item

	(*pkVector)[iSlotIndex].pkWnd->Hide();

	Point size = SlotSizeFromWnd((*pkVector)[iSlotIndex].pkWnd);
	int id = (*pkVector)[iSlotIndex].pkWnd->GetSkin()->m_iBkTexID;

	int x = mx - m_kCursorRangeDiff.x, y = my - m_kCursorRangeDiff.y;
	g_kMistClient.m_pkGui->SetCursor( x, y, id, -1, 
		g_kMistClient.GetScaleX()*(float)(size.x*ICON_WIDTH), 
		g_kMistClient.GetScaleY()*(float)(size.y*ICON_HEIGHT));	
	g_kMistClient.m_pkInputHandle->SetCursorInputPos(x,y);  

	printf("Item id = %i\n", (*pkVector)[iSlotIndex].iItemID);
}

void InventoryDlg::OpenContainerItem(bool bOpen, int iSlotIndex, bool bInventory)
{
	vector<ITEM_SLOT>* pkVector;
	if(bInventory)
		pkVector = &m_vkInventoryItemList;
	else
		pkVector = &m_vkContainerItemList;

	if(bOpen)
	{
		g_kMistClient.SendRequestContainer((*pkVector)[iSlotIndex].iItemID);
		m_iSelItemID = (*pkVector)[iSlotIndex].iItemID;
		(*pkVector)[iSlotIndex].pkWnd->GetSkin()->m_unBorderSize = 2;
	}
	else
	{
		CloseContainerWnd();
		if(m_iSelItemID == (*pkVector)[iSlotIndex].iItemID)
		{
			m_iSelItemID = -1;
			(*pkVector)[iSlotIndex].pkWnd->GetSkin()->m_unBorderSize = 0;
		}
	}
}

void InventoryDlg::OnDoubleClick(int iMouseX, int iMouseY)
{
	m_bDoubleClicked = true;

	int temp = m_kMoveSlot.m_iIndex;
	m_kMoveSlot.m_iIndex = -1;

	pair<int, bool> kTargetSlot = GetItemFromScreenPos(iMouseX, iMouseY);

	bool bTargetIsInventoryItem = (kTargetSlot.second == true) ? true : false;

	if(kTargetSlot.first == -1)
	{
		m_kMoveSlot.m_iIndex = temp;
		return;
	}

	vector<ITEM_SLOT>* pkVector;

	if(bTargetIsInventoryItem)
	{
		pkVector = &m_vkInventoryItemList;
	}
	else
	{
		pkVector = &m_vkContainerItemList;
	}
		
	if((*pkVector)[kTargetSlot.first].bIsContainer)
	{
		OpenContainerItem(true, kTargetSlot.first, 
			bTargetIsInventoryItem);
	}

	m_kMoveSlot.m_iIndex = temp;

	if(m_kMoveSlot.m_iIndex != -1)
	{
		// Show icon again and move back before split.
		(*pkVector)[m_kMoveSlot.m_iIndex].pkWnd->Show();
		(*pkVector)[m_kMoveSlot.m_iIndex].pkWnd->SetPos(m_kItemWndPosBeforeMove.x, 
			m_kItemWndPosBeforeMove.y, false, true);

		// Show normal cursor again.
		float w = g_kMistClient.GetScaleX()*64.0f, h = g_kMistClient.GetScaleY()*64.0f ;
		g_kMistClient.m_pkGui->SetCursor( (int)iMouseX, 
			(int)iMouseY, 
			g_kMistClient.LoadGuiTextureByRes("cursor_sword.tga"), -1, w, h);

		m_kMoveSlot.m_iIndex = -1;
	}

}

void InventoryDlg::OpenContainerWnd(int id, char slots_x, char slots_y)
{
	m_iSelItemID = id;

	m_pkContainerWnd = g_kMistClient.GetWnd("ContainerWnd");
	m_pkContainerWnd->Show();

	m_iSlotsHorzContainer = slots_x;
	m_iSlotsVertContainer = slots_y;
	m_iActiveContainerID = id;

	RebuidContainerGrid(slots_x, slots_y);

	m_pkContainerWnd->SetZValue(22);
	
	if(m_pkSplitStockWnd == NULL || !m_pkSplitStockWnd->IsVisible()) // dont place on top if stockwnd is visible
	{
		g_kMistClient.m_pkGui->PlaceWndFrontBack(m_pkContainerWnd, true); 
		g_kMistClient.m_pkGui->SetFocus(m_pkContainerWnd, false);
	}
}

void InventoryDlg::CreateContainerGrid()
{
	const int MAX_SLOTS_X = 8;
	const int MAX_SLOTS_Y = 8;
	const int iNumOfLabelsNeeded = MAX_SLOTS_X*MAX_SLOTS_Y;

	char szSlotBkLabelName[50];
	ZGuiWnd* pkLabel;

	static ZGuiSkin* pkSlotBkSkin = NULL;
	
	if(pkSlotBkSkin == NULL)
	{
		pkSlotBkSkin = new ZGuiSkin();
		pkSlotBkSkin->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("inventory/slots_1x1.tga");
		pkSlotBkSkin->m_bTileBkSkin = false;
		pkSlotBkSkin->m_afBorderColor[0] = 237.0f / 255.0f;
		pkSlotBkSkin->m_afBorderColor[1] = 190.0f / 255.0f;
		pkSlotBkSkin->m_afBorderColor[2] = 107.0f / 255.0f;
		pkSlotBkSkin->m_unBorderSize = 1;
	}

	for(int i=0; i<iNumOfLabelsNeeded; i++)
	{
		sprintf(szSlotBkLabelName, "CSlotBkLabel_%i", i);
		pkLabel = g_kMistClient.CreateWnd(Label, szSlotBkLabelName, "ContainerWnd", 
			"", 0, 0, ICON_WIDTH, ICON_HEIGHT, 0, TopLeft, eNone);
		pkLabel->SetSkin(pkSlotBkSkin);
		m_vkContainerGridSlots.push_back(pkLabel);
	}	
}

// Skapa bakgrundsbilden genom att klistra samman massa labels som har skapats från skript.
void InventoryDlg::RebuidContainerGrid(char slots_horz, char slots_vert)
{
	int max_width = 0;
	int current_slot_x=0, current_slot_y=0;
	int dx = 0, dy = 0;
	int iSlotWidth = m_vkContainerGridSlots[0]->GetScreenRect().Width();
	int iSlotHeight = m_vkContainerGridSlots[0]->GetScreenRect().Height();
	Rect rcInventory = m_pkInventoryWnd->GetScreenRect();

	for(int i=0; i<m_vkContainerGridSlots.size(); i++)
	{
		m_vkContainerGridSlots[i]->Hide();
	}

	for(int i=0; i<m_vkContainerGridSlots.size(); i++)
	{
		ZGuiWnd* pkWnd = m_vkContainerGridSlots[i];

		pkWnd->SetPos(dx, dy, false, true);

		current_slot_x += 1;
		dx += iSlotWidth;
		
		if(current_slot_y >= slots_vert)
		{			
			//pkWnd->Hide();		
			break;
		}
		else
		{
			pkWnd->Show();
			pkWnd->SetZValue(1);
		}

		if(current_slot_x >= slots_horz)
		{
			max_width = dx;
			current_slot_x = 0;
			current_slot_y += 1;
			dx = 0;
			dy += iSlotHeight;
		}		
	}

	m_pkContainerWnd->Resize(max_width, dy);

	int top = m_pkInventoryWnd->GetScreenRect().Top + 
		(int) ((float) UPPERLEFT_INVENTORY.y * g_kMistClient.GetScaleY()); 
	
	m_pkContainerWnd->SetPos(rcInventory.Left - max_width, top, true, true);

//	g_kMistClient.GetWnd("ContainerCloseButton")->SetPos(max_width, -16, false, true);
}

void InventoryDlg::UpdateInventory(vector<MLContainerInfo>& vkItemList)
{
	m_iHighestZ = 1000;
	m_kMoveSlot.m_iIndex = -1;

	// Remove all slots.
	for(int i=0; i<m_vkInventoryItemList.size(); i++)
	{
		ZGuiWnd* pkWnd = m_vkInventoryItemList[i].pkWnd;
		
		if(pkWnd->GetSkin())
			delete pkWnd->GetSkin();
		
		g_kMistClient.m_pkGui->GetToolTip()->RemoveToolTip(pkWnd);	//krashar utan denna *gråta*
		g_kMistClient.m_pkGui->UnregisterWindow( pkWnd );
	}

	m_vkInventoryItemList.clear();

	// Rebuild slotlist.
	int x,y,w,h;
	char szItemName[128];
	for(int i=0; i<vkItemList.size(); i++)
	{
		sprintf(szItemName, "InventoryItemLabel%i", i);
		x = UPPERLEFT_INVENTORY.x + vkItemList[i].m_cItemX * ICON_WIDTH + vkItemList[i].m_cItemX;
		y = UPPERLEFT_INVENTORY.y + vkItemList[i].m_cItemY * ICON_HEIGHT + vkItemList[i].m_cItemY;
		w = vkItemList[i].m_cItemW * (ICON_WIDTH) + vkItemList[i].m_cItemW-1;
		h = vkItemList[i].m_cItemH * (ICON_HEIGHT) + vkItemList[i].m_cItemH-1;
		
		char text[20] = "";
		if(vkItemList[i].m_iStackSize > 1)
			sprintf(text, "%i", vkItemList[i].m_iStackSize);

		ZGuiWnd* pkNewSlot = g_kMistClient.CreateWnd(Label, 
			szItemName, text, m_pkInventoryWnd, x, y, w, h, 0);
		pkNewSlot->Show();

		g_kMistClient.m_pkGui->GetToolTip()->AddToolTip(/*pkNewSlot*/szItemName, vkItemList[i].m_strName, 50.0f);
		
		g_kMistClient.SetFont(szItemName, "small7", 255, 255, 255, 0);

		((ZGuiLabel*) pkNewSlot)->m_eTextAlignment = ZGLA_BottomRight;

//		 If left mouse button is pressed, hide the icon and set it to be visible under cursor.
		if(g_kMistClient.m_pkGui->m_bMouseLeftPressed)
		{
			if(g_kMistClient.m_iPickedEntityID == vkItemList[i].m_iItemID)
			{
				m_iItemUnderCursor = vkItemList[i].m_iItemID;			
				m_fPickUpTimer = g_kMistClient.m_pkZeroFps->GetEngineTime();
				pkNewSlot->Hide();
			}
		}

		pkNewSlot->SetSkin(new ZGuiSkin());
		pkNewSlot->GetSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("items/" + vkItemList[i].m_strIcon) ;
		pkNewSlot->GetSkin()->m_bTileBkSkin = 0;

		ITEM_SLOT kNewSlot;
		kNewSlot.pkWnd = pkNewSlot;
		kNewSlot.iItemID = vkItemList[i].m_iItemID;
		kNewSlot.bIsContainer = vkItemList[i].m_bIsContainer;
		kNewSlot.iItemType = vkItemList[i].m_cItemType;
		kNewSlot.iStackSize = vkItemList[i].m_iStackSize;
		kNewSlot.strName = vkItemList[i].m_strName;
		m_vkInventoryItemList.push_back(kNewSlot);

		/*SetSelectionBorder(i, true, !(kNewSlot.iItemID == m_iActiveContainerID && 
			m_pkContainerWnd->IsVisible()));*/

		SetSelectionBorder(i, true, true);
	}	
}

void InventoryDlg::UpdateContainer(vector<MLContainerInfo>& vkItemList)
{
	// Remove all slots.
	for(int i=0; i<m_vkContainerItemList.size(); i++)
	{
		ZGuiWnd* pkWnd = m_vkContainerItemList[i].pkWnd;
		delete pkWnd->GetSkin();
		g_kMistClient.m_pkGui->GetToolTip()->RemoveToolTip(pkWnd->GetName());
		g_kMistClient.m_pkGui->UnregisterWindow( pkWnd );
	}

	m_vkContainerItemList.clear();

	// Rebuild slotlist.
	int x,y,w,h;
	char szItemName[128];
	for(int i=0; i<vkItemList.size(); i++)
	{
		sprintf(szItemName, "ContainerItemLabel%i", i);
		x = UPPERLEFT_CONTAINER.x + vkItemList[i].m_cItemX * ICON_WIDTH;
		y = UPPERLEFT_CONTAINER.y + vkItemList[i].m_cItemY * ICON_HEIGHT;
		w = vkItemList[i].m_cItemW * ICON_WIDTH ;
		h = vkItemList[i].m_cItemH * ICON_HEIGHT;

		char text[20] = "";
		if(vkItemList[i].m_iStackSize > 1)
			sprintf(text, "%i", vkItemList[i].m_iStackSize);

		ZGuiWnd* pkNewSlot = g_kMistClient.CreateWnd(Label, szItemName, text, 
			m_pkContainerWnd, x, y, w, h, 0);
		pkNewSlot->Show();
		pkNewSlot->SetZValue(12121);
		g_kMistClient.SetFont(szItemName, "small7", 255, 255, 255, 0);
		((ZGuiLabel*) pkNewSlot)->m_eTextAlignment = ZGLA_BottomRight;

		g_kMistClient.m_pkGui->GetToolTip()->AddToolTip(/*pkNewSlot*/szItemName, vkItemList[i].m_strName, 50.0f);

		pkNewSlot->SetSkin(new ZGuiSkin());
		pkNewSlot->GetSkin()->m_bTileBkSkin = 0;
		pkNewSlot->GetSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("items/" + vkItemList[i].m_strIcon);

		ITEM_SLOT kNewSlot;
		kNewSlot.pkWnd = pkNewSlot;
		kNewSlot.iItemID = vkItemList[i].m_iItemID;
		kNewSlot.bIsContainer = vkItemList[i].m_bIsContainer;
		kNewSlot.iStackSize = vkItemList[i].m_iStackSize;
		kNewSlot.iItemType = vkItemList[i].m_cItemType;
		kNewSlot.strName = vkItemList[i].m_strName;
		m_vkContainerItemList.push_back(kNewSlot);

		//SetSelectionBorder(i, false, !(kNewSlot.iItemID == m_iActiveContainerID && 
		//	m_pkContainerWnd->IsVisible()));

		SetSelectionBorder(i, false, true);
	}

	m_pkContainerWnd->SortChilds();
}

// When dropping a item, check for collision, reposition it and update the inventory.
void InventoryDlg::OnDropItem(int mx, int my)
{
	g_kMistClient.m_pkEquipmentDlg->HighlightSlot( NULL );

	vector<ITEM_SLOT>* pkVector;
	int iTarget = -1, iSlotX = -1, iSlotY = -1, iItemID = -1;

	InventoryDropTarget eDropTarget = GetDropTargetFromScreenPos(mx, my);
	pair<int, bool> kTargetSlot = GetItemFromScreenPos(mx, my);

	bool bTargetIsInventoryItem = (kTargetSlot.second == true) ? true : false;

	Point slot = SlotFromScreenPos(mx, my, (eDropTarget == DropTarget_Inventory));
	iSlotX = slot.x;
	iSlotY = slot.y;

	pkVector = (m_kMoveSlot.bIsInventoryItem) ? &m_vkInventoryItemList : &m_vkContainerItemList;	
	iItemID = (*pkVector)[m_kMoveSlot.m_iIndex].iItemID;
	bool bIsSplitSlot = (*pkVector)[m_kMoveSlot.m_iIndex].iStackSize > 1;
	bool bTryExecuteSlplit = false, bEquip = false;

	ZGuiWnd* pkMoveWnd = (*pkVector)[m_kMoveSlot.m_iIndex].pkWnd;

	m_kSplitSlotTarget.m_iIndex = -1;

	if(kTargetSlot.first == -1) // no collision
	{					
		if(eDropTarget == DropTarget_Ground)
		{
			iTarget = iSlotX = iSlotY = -1;
			if(g_kMistClient.GetWnd("EquipWnd")->GetScreenRect().Inside(mx,my))
			{
				bEquip = true;

				pkMoveWnd->Show();
				pkMoveWnd->SetPos(m_kItemWndPosBeforeMove.x, 
					m_kItemWndPosBeforeMove.y, false, true);
			}
		}
		else
		if(eDropTarget == DropTarget_Inventory && !m_kMoveSlot.bIsInventoryItem)
			iTarget = GetInventoryContainerID();
		else
		if(eDropTarget == DropTarget_Container && m_kMoveSlot.bIsInventoryItem)
			iTarget = m_iActiveContainerID;
		else
			iTarget = -1;

		if(eDropTarget == DropTarget_EquipmentSlot)
		{
			int iTarget = g_kMistClient.m_pkEquipmentDlg->GetSlotContainerID(mx, my);
			if(iTarget != -1)
			{
				g_kMistClient.SendMoveItem(iItemID, iTarget, 0, 0, -1);
				bEquip = true;
			}
		}
	}
	else
	{
		if(eDropTarget == DropTarget_Ground)
			iTarget = iSlotX = iSlotY = -1;
		else		
		if(eDropTarget == DropTarget_Inventory)
		{
			pkVector = &m_vkInventoryItemList;
			iTarget = GetInventoryContainerID(); 
		}
		else
		{
			pkVector = &m_vkContainerItemList;
			iTarget = m_iActiveContainerID;
		}

		if((*pkVector)[kTargetSlot.first].bIsContainer)
		{
			iSlotX = iSlotY = -1;
			iTarget = (*pkVector)[kTargetSlot.first].iItemID;
		}
		else
		if(bIsSplitSlot)
		{
			if(g_kMistClient.m_pkInputHandle->Pressed(KEY_LCTRL))  
				bTryExecuteSlplit = true;
		
			m_kSplitSlot.m_iIndex = m_kMoveSlot.m_iIndex;
			m_kSplitSlot.bIsInventoryItem = m_kMoveSlot.bIsInventoryItem;

			m_kSplitSlotTarget.m_iIndex = kTargetSlot.first;
			m_kSplitSlotTarget.bIsInventoryItem = kTargetSlot.second;	
		}
	}

	if(bEquip == false)
	{
		if(bTryExecuteSlplit == false)
		{
			g_kMistClient.SendMoveItem(iItemID, iTarget, iSlotX, iSlotY);
		}
		else
		{		
			// Show icon again and move back before split.
			pkMoveWnd->Show();
			pkMoveWnd->SetPos(m_kItemWndPosBeforeMove.x, 
				m_kItemWndPosBeforeMove.y, false, true);

			// Open split window.
			OpenSplitStockWnd();
		}
	}

	// Show normal cursor again.
	float w = g_kMistClient.GetScaleX()*32.0f, h = g_kMistClient.GetScaleY()*32.0f ;
	g_kMistClient.m_pkGui->SetCursor( (int)mx+m_kCursorRangeDiff.x, (int)my+m_kCursorRangeDiff.y, 
		g_kMistClient.LoadGuiTextureByRes("cursor_sword.tga"), -1, w, h);

	g_kMistClient.m_pkInputHandle->SetCursorInputPos(mx+m_kCursorRangeDiff.x,my+m_kCursorRangeDiff.y);	

//	g_kMistClient.SendRequestContainer(GetInventoryContainerID());
}

int InventoryDlg::TestForCollision(int iTestSlot, bool bInventory)
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

	return TestForCollision(test_slot, test_size, bInventory);
}

int InventoryDlg::TestForCollision(Point test_slot, Point test_size, bool bInventory)
{
	vector< pair<Point, int> > kSlotsTaken; // slotpos, index

	if(bInventory)
	{
		for(int i=0; i<m_vkInventoryItemList.size(); i++)
		{
			Point kSlot = SlotFromWnd(m_vkInventoryItemList[i].pkWnd, true);
			Point kSlotSize = SlotSizeFromWnd(m_vkInventoryItemList[i].pkWnd);

			if(i != m_kMoveSlot.m_iIndex) 
			{
				for(int y=0; y<kSlotSize.y; y++)
					for(int x=0; x<kSlotSize.x; x++)
						kSlotsTaken.push_back(pair<Point,int>(Point(kSlot.x+x, kSlot.y+y), i));
			}
		}
	}
	else
	{
		for(int i=0; i<m_vkContainerItemList.size(); i++)
		{
			Point kSlot = SlotFromWnd(m_vkContainerItemList[i].pkWnd, false);
			Point kSlotSize = SlotSizeFromWnd(m_vkContainerItemList[i].pkWnd);

			if(i != m_kMoveSlot.m_iIndex)
			{
				for(int y=0; y<kSlotSize.y; y++)
					for(int x=0; x<kSlotSize.x; x++)
						kSlotsTaken.push_back(pair<Point,int>(Point(kSlot.x+x, kSlot.y+y), i));
			}
		}
	}

	for(int i=0; i<kSlotsTaken.size(); i++)
		for(int y=0; y<test_size.y; y++)
			for(int x=0; x<test_size.x; x++)
			{
				Point t(test_slot.x + x, test_slot.y + y);

				if(t == kSlotsTaken[i].first)
				{
					return kSlotsTaken[i].second;
				}
			}

	return -1;
}

Point InventoryDlg::SlotSizeFromWnd(ZGuiWnd* pkWnd)
{
	int fSlotWidth = (int)((float)(ICON_WIDTH)*g_kMistClient.GetScaleX());
	int fSlotHeight = (int)((float)(ICON_WIDTH)*g_kMistClient.GetScaleX());

	Rect rcTest = pkWnd->GetScreenRect();
	return Point(rcTest.Width()/fSlotWidth, rcTest.Height()/fSlotHeight);
}

Point InventoryDlg::SlotFromWnd(ZGuiWnd* pkWnd, bool bInventory)
{
	Rect rcTest = pkWnd->GetScreenRect();
	return SlotFromScreenPos(rcTest.Left, rcTest.Top, bInventory);
}

Point InventoryDlg::SlotFromScreenPos(int x, int y, bool bInventory)
{
	int slot_x, slot_y;

	if(bInventory)
	{
		slot_x = (x - m_pkInventoryWnd->GetScreenRect().Left - 
			(g_kMistClient.GetScaleX()*(float)UPPERLEFT_INVENTORY.x) ) / 
			(g_kMistClient.GetScaleX() * (float) ICON_WIDTH); 

		slot_y = (y - m_pkInventoryWnd->GetScreenRect().Top - 
			(g_kMistClient.GetScaleY()*(float)UPPERLEFT_INVENTORY.y) ) / 
			(g_kMistClient.GetScaleY() * (float) ICON_HEIGHT);
	}
	else
	{
		slot_x = (x - m_pkContainerWnd->GetScreenRect().Left - 
			(g_kMistClient.GetScaleX()*(float)UPPERLEFT_CONTAINER.x) ) / 
			(g_kMistClient.GetScaleX() * (float) ICON_WIDTH); 

		slot_y = (y - m_pkContainerWnd->GetScreenRect().Top - 
			(g_kMistClient.GetScaleY()*(float)UPPERLEFT_CONTAINER.y) ) / 
			(g_kMistClient.GetScaleY() * (float) ICON_HEIGHT);
	}

	return Point(slot_x, slot_y);
}

pair<int, bool> InventoryDlg::GetItemFromScreenPos(int x, int y)
{
	Rect rc;

	if(m_pkInventoryWnd->IsVisible())
	{
		for(int i=0; i<m_vkInventoryItemList.size(); i++)
		{
			rc = m_vkInventoryItemList[i].pkWnd->GetScreenRect();
			if(rc.Inside(x,y) && (i != m_kMoveSlot.m_iIndex || !m_kMoveSlot.bIsInventoryItem ) )
				return pair<int,bool>(i,true);
		}
	}

	if(m_pkContainerWnd->IsVisible())
	{
		for(int i=0; i<m_vkContainerItemList.size(); i++)
		{
			rc = m_vkContainerItemList[i].pkWnd->GetScreenRect();
			if(rc.Inside(x,y) && (i != m_kMoveSlot.m_iIndex || m_kMoveSlot.bIsInventoryItem ) )
				return pair<int,bool>(i,false);
		}
	}

	return pair<int,bool>(-1,0);
}

int InventoryDlg::GetInventoryContainerID()
{
	if(Entity* pkCharacter = g_kMistClient.m_pkEntityManager->GetEntityByID(g_kMistClient.m_iCharacterID))
		if(P_CharacterProperty* pkCharProp = (P_CharacterProperty*)pkCharacter->GetProperty("P_CharacterProperty"))
			return pkCharProp->m_iInventory;

	return -1;
}

int InventoryDlg::GetActiveContainerID()
{
	return m_iActiveContainerID;
}

InventoryDropTarget InventoryDlg::GetDropTargetFromScreenPos(int mx, int my)
{
	bool bDropInInventory = m_pkInventoryWnd && m_pkInventoryWnd->IsVisible() && 
		m_pkInventoryWnd->GetScreenRect().Inside(mx, my);

	bool bDropInContainer = m_pkContainerWnd && m_pkContainerWnd->IsVisible() && 
		m_pkContainerWnd->GetScreenRect().Inside(mx, my);

	int iSlotType = -1;
	
	if(g_kMistClient.m_pkEquipmentDlg)
		iSlotType = g_kMistClient.m_pkEquipmentDlg->GetSlotType(mx, my);

	bool bDropInEuipmentDlg = (iSlotType == -1) ? false : true;
	
	if(bDropInInventory == true && bDropInContainer == false)
		return DropTarget_Inventory;
	
	if(bDropInContainer == true && bDropInInventory == false)
		return DropTarget_Container;

	if(bDropInEuipmentDlg)
		return DropTarget_EquipmentSlot;

	return DropTarget_Ground;
}

void InventoryDlg::SetSelectionBorder(int iIndex, bool bInventory, bool bRemove)
{
	vector<ITEM_SLOT>* pkVector;
	if(bInventory)
		pkVector = &m_vkInventoryItemList;
	else
		pkVector = &m_vkContainerItemList;

	if((*pkVector).size() < iIndex)
		return;

	(*pkVector)[iIndex].pkWnd->GetSkin()->m_unBorderSize = bRemove ? 0 : 2;

	if((*pkVector)[iIndex].bIsContainer)
	{
		(*pkVector)[iIndex].pkWnd->GetSkin()->m_afBorderColor[0] = 0;
		(*pkVector)[iIndex].pkWnd->GetSkin()->m_afBorderColor[1] = 0;
		(*pkVector)[iIndex].pkWnd->GetSkin()->m_afBorderColor[2] = 1;
	}
	else
	{
		(*pkVector)[iIndex].pkWnd->GetSkin()->m_afBorderColor[0] = 1;
		(*pkVector)[iIndex].pkWnd->GetSkin()->m_afBorderColor[1] = 1;
		(*pkVector)[iIndex].pkWnd->GetSkin()->m_afBorderColor[2] = 1;
	}
}

void InventoryDlg::OpenSplitStockWnd()
{
	Rect rcSlot;
	int max, split;
	
	if(m_kSplitSlot.bIsInventoryItem)
	{
		max = m_vkInventoryItemList[m_kSplitSlot.m_iIndex].iStackSize;
		split = m_vkInventoryItemList[m_kSplitSlot.m_iIndex].iStackSize/2;
		rcSlot = m_vkInventoryItemList[m_kSplitSlot.m_iIndex].pkWnd->GetScreenRect();
	}
	else
	{
		max = m_vkContainerItemList[m_kSplitSlot.m_iIndex].iStackSize;
		split = m_vkContainerItemList[m_kSplitSlot.m_iIndex].iStackSize/2;
		rcSlot = m_vkContainerItemList[m_kSplitSlot.m_iIndex].pkWnd->GetScreenRect();
	}

	m_iSplitShareMax = max;

	m_pkSplitStockWnd->Show();
	
	Rect rcWnd = m_pkSplitStockWnd->GetScreenRect();
	
	int x, y;
	x = rcSlot.Left - rcWnd.Width() / 2 + ICON_WIDTH/2;
	y = rcSlot.Bottom + 8;

	if(y + rcWnd.Height() > g_kMistClient.GetHeight())
		y = rcSlot.Top - rcWnd.Height() - 8;
	if(x + rcWnd.Width() > g_kMistClient.GetWidth())
		x = g_kMistClient.GetWidth() - rcWnd.Width() - 8;

	m_pkSplitStockWnd->SetPos(x, y, true, true); 

	g_kMistClient.m_pkGui->PlaceWndFrontBack(m_pkSplitStockWnd, true); 
	g_kMistClient.m_pkGui->SetFocus(m_pkSplitStockWnd, true);
	g_kMistClient.m_pkGui->SetCaptureToWnd(m_pkSplitStockWnd);

	g_kMistClient.SetTextInt("SplitStockMaxEb", max);
	g_kMistClient.SetTextInt("SplitStockSplitEb", split);
	g_kMistClient.GetWnd("SplitStockSplitEb")->Disable();
	g_kMistClient.GetWnd("SplitStockMaxEb")->Disable();

	ZGuiSlider* pkSlider = (ZGuiSlider*) g_kMistClient.GetWnd("SplitStockSlider");
	pkSlider->SetRange(0, max);
	pkSlider->SetPos(split, true, false);
	pkSlider->AddBuddyWindow(g_kMistClient.GetWnd("SplitStockSplitEb"));
}

void InventoryDlg::CloseSplitStockWnd(bool bExecuteSplit)
{
	if(m_pkSplitStockWnd)
		m_pkSplitStockWnd->Hide();

	bool bUpdate = false;

	if(bExecuteSplit && m_kSplitSlot.m_iIndex != -1)
	{
		int iItemID=-1, iTarget=-1, iSlotX=1, iSlotY=-1, iCount=0;

		bool bSuccesss;
		int iTemp = g_kMistClient.GetTextInt("SplitStockSplitEb", &bSuccesss);
		if(bSuccesss)
		{	
			iCount = iTemp;
		}

		if(m_kSplitSlot.bIsInventoryItem)
			iItemID = m_vkInventoryItemList[m_kSplitSlot.m_iIndex].iItemID;
		else
			iItemID = m_vkContainerItemList[m_kSplitSlot.m_iIndex].iItemID;

		if(m_kSplitSlotTarget.m_iIndex != -1)
		{
			Point slot;
			if(m_kSplitSlotTarget.bIsInventoryItem)
			{
				iTarget = GetInventoryContainerID(); 
				slot = SlotFromWnd(m_vkInventoryItemList[m_kSplitSlotTarget.m_iIndex].pkWnd, true);
			}
			else
			{
				iTarget = m_iActiveContainerID;
				slot = SlotFromWnd(m_vkContainerItemList[m_kSplitSlotTarget.m_iIndex].pkWnd, false);
			}

			iSlotX = slot.x;
			iSlotY = slot.y;			
		}

		if(iCount != 0)
		{
			printf("GUI: Trying to move %i item from %i to %i (at pos (%i,%i)\n", iCount, iItemID, iTarget, iSlotX, iSlotY);
			g_kMistClient.SendMoveItem(iItemID, iTarget, iSlotX, iSlotY, iCount);	
			bUpdate = false;
		}
		else
			bUpdate = true;
	}
	
	if(bUpdate)
	{
		if(m_kSplitSlot.bIsInventoryItem)
			g_kMistClient.RequestOpenInventory();
		else
		{
			if(m_iActiveContainerID)
				g_kMistClient.SendRequestContainer(m_iActiveContainerID);
		}
	}

	m_kSplitSlot.m_iIndex = -1;
	m_kSplitSlotTarget.m_iIndex = -1;
	
	if(ZGuiWnd::m_pkFocusWnd)
		ZGuiWnd::m_pkFocusWnd->KillFocus();

	g_kMistClient.m_pkGui->SetFocus(m_pkInventoryWnd, false);	
	g_kMistClient.m_pkGui->KillWndCapture(); 
}

void InventoryDlg::OpenItemInfoWnd(bool bOpen, ITEM_INFO kInfo)
{
	static bool s_bLoaded = false;

	if(s_bLoaded == false)
	{
		if(!g_kMistClient.LoadGuiFromScript(g_kMistClient.m_kGuiScrips[GSF_ITEMINFO].c_str()))
		{
			printf("Failed to load GUI script!\n");
			return;
		}

		((ZGuiTextbox*) g_kMistClient.GetWnd("ItemInfoTextbox"))->ToggleMultiLine(true);

		s_bLoaded = true;
	}

	if(!bOpen)
	{
		g_kMistClient.m_pkGui->PlaceWndFrontBack(g_kMistClient.GetWnd("ItemInfoWnd"), false); 
		g_kMistClient.GetWnd("ItemInfoWnd")->Hide();
	}
	else
	{
		char szNumber[50];

		g_kMistClient.ShowWnd("ItemInfoWnd", true, true, false);
		g_kMistClient.SetText("ItemInfoTextbox", (char*)kInfo.strInfo.c_str());

		if(m_kItemUnderInspection.first != -1)
		{
			if(m_kItemUnderInspection.second == true)
				g_kMistClient.SetText("ItemLabelTextbox", 
					(char*)m_vkInventoryItemList[m_kItemUnderInspection.first].strName.c_str());
			else
				g_kMistClient.SetText("ItemLabelTextbox", 
					(char*)m_vkContainerItemList[m_kItemUnderInspection.first].strName.c_str());

		}
		
		sprintf(szNumber, "%0.2f Kg", kInfo.m_fWeight);
		g_kMistClient.SetText("ItemValueWeight", szNumber);

		sprintf(szNumber, "%i GM", kInfo.m_iValue);
		g_kMistClient.SetText("ItemValueLabel", szNumber);

		g_kMistClient.GetWnd("ItemInfoImage")->GetSkin()->m_iBkTexID = 
			g_kMistClient.LoadGuiTextureByRes(string("item_images/") + kInfo.strImage);
	}
}