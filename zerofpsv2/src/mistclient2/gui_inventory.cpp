
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

InventoryDlg::InventoryDlg() : ICON_WIDTH(32), ICON_HEIGHT(32), UPPER_LEFT_INVENTORY(27,87),
										 SLOTTS_HORZ_INVENTORY(6), SLOTTS_VERT_INVENTORY(12), 
										 UPPER_LEFT_CONTAINER(0,0), BD_R(1), BD_G(1), BD_B(1)
{
	m_pkInventoryWnd = NULL;
	m_pkContainerWnd = NULL;

	m_pkTexMan = g_kMistClient.m_pkTexMan;
	m_kMoveSlot.m_iIndex = -1;
	m_iSelItemID = -1;
	m_iHighestZ = 1000;
	m_iItemUnderCursor = -1;
	m_iActiveContainerID = -1;
	m_fPickUpTimer = 0;
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
		if(!g_kMistClient.LoadGuiFromScript("data/script/gui/inventory.lua"))
		{
			printf("Error loading inventory script!\n");
			return;
		}

		m_pkInventoryWnd = g_kMistClient.GetWnd("InventoryWnd");
		m_pkContainerWnd = g_kMistClient.GetWnd("ContainerWnd");
	}

	// visa inventory fönstret
	m_pkInventoryWnd->Show();

	// dölj actionikonen och regruppera dom andra
	g_kMistClient.GetWnd("OpenInventoryBn")->Hide();
	g_kMistClient.PositionActionButtons();

	ZGuiSkin* pkToolTipSkin = new ZGuiSkin();
	pkToolTipSkin->m_unBorderSize = 8;

	pkToolTipSkin->m_iBkTexID = m_pkTexMan->Load( "data/textures/gui/textbox.bmp", 0 );
	pkToolTipSkin->m_iVertBorderTexID = m_pkTexMan->Load( "data/textures/gui/wndborder_top.bmp", 0 );
	pkToolTipSkin->m_iHorzBorderTexID = m_pkTexMan->Load( "data/textures/gui/wndborder_left.bmp", 0 );
	pkToolTipSkin->m_iBorderCornerTexID = m_pkTexMan->Load( "data/textures/gui/wndborder_corner.bmp", 0 );
	  
	g_kMistClient.m_pkGui->GetToolTip()->GetWnd()->SetFont(
			g_kMistClient.m_pkGuiMan->Font("chatboxfont"));
	g_kMistClient.m_pkGui->GetToolTip()->GetWnd()->SetTextColor(255,255,255); 

	g_kMistClient.m_pkGui->GetToolTip()->SetSkin(pkToolTipSkin);

}

void InventoryDlg::Close()
{
	// Hide inventory window
	if(m_pkInventoryWnd)
		m_pkInventoryWnd->Hide();

	// Close contatiner window.
	CloseContainerWnd();

	// Must set focus on mainwnd to recive SPACE intput for chatbox...
	g_kMistClient.m_pkGui->SetFocus(g_kMistClient.GetWnd("GuiMainWnd"), false);	

	// Show the button that opens the inventory again.
	g_kMistClient.GetWnd("OpenInventoryBn")->Show();

	// Reposition action buttons.
	g_kMistClient.PositionActionButtons();
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
}

void InventoryDlg::OnMouseMove(bool bLeftButtonPressed, int mx, int my)
{
	static bool s_bRightMouseButtonPressed = false;

	if(m_iItemUnderCursor) // the application have found a item 
		PickUpFromGround(bLeftButtonPressed, mx, my); // test if its time to place it under cursor.

	if(m_pkInventoryWnd->IsVisible()) // kolla så att inventory är synligt
	{
		for(int i=0; i<m_vkInventoryItemList.size(); i++)
		{		
			if(m_vkInventoryItemList[i].pkWnd->GetScreenRect().Inside(mx, my)) // cursor is inside the 
			{																						 // rectangle of the slot.
				// set selection border.
				if(m_kMoveSlot.m_iIndex == -1)
					m_vkInventoryItemList[i].pkWnd->GetSkin()->m_unBorderSize = 2;

				if(bLeftButtonPressed)
				{			
					if(m_kMoveSlot.m_iIndex == -1)
						PickUpFromGrid(i, true, mx, my); // try to find item under cursor and set as move item.
				}
				else
				{
					if(m_kMoveSlot.m_iIndex != -1) // if we have a move slot...
						OnDropItem(); // place it somewhere.

					m_kMoveSlot.m_iIndex = -1; // mark that we no longer have a moveslot.
				}	
				
				// Open or close container in grid.
				if(g_kMistClient.m_pkGui->m_bMouseRightPressed && s_bRightMouseButtonPressed == false)
				{	
					s_bRightMouseButtonPressed = true;

					bool bOpen = true;
					if(m_iActiveContainerID == m_vkInventoryItemList[i].iItemID) // klickat på samma container
						bOpen = false;															 // som redan är öppnad?

					OpenContainerItem(bOpen, i, true);				
				}
				else if(!g_kMistClient.m_pkGui->m_bMouseRightPressed)
					s_bRightMouseButtonPressed = false;

			}
			else // cursor is not inside the slots rectangle.
			{		
				// remove selection border.
				if( m_vkInventoryItemList[i].iItemID != m_iSelItemID)
					m_vkInventoryItemList[i].pkWnd->GetSkin()->m_unBorderSize = 0;
			}
		}
	}

	if(m_pkContainerWnd->IsVisible()) // kolla så att containerfönstret är synligt
	{
		for(int i=0; i<m_vkContainerItemList.size(); i++)
		{
			if(m_vkContainerItemList[i].pkWnd->GetScreenRect().Inside(mx, my))
			{
				if(m_kMoveSlot.m_iIndex == -1)
					m_vkContainerItemList[i].pkWnd->GetSkin()->m_unBorderSize = 2;

				if(bLeftButtonPressed)
				{				
					if(m_kMoveSlot.m_iIndex == -1) // inget slot under cursorn, plocka upp.
						PickUpFromGrid(i,false,mx,my); 
				}
				else
				{
					if(m_kMoveSlot.m_iIndex != -1)
						OnDropItem();

					m_kMoveSlot.m_iIndex = -1;
				}

				// Open or close container in grid.
				if(g_kMistClient.m_pkGui->m_bMouseRightPressed && s_bRightMouseButtonPressed == false)
				{	
					s_bRightMouseButtonPressed = true;

					bool bOpen = true;
					if(m_iActiveContainerID == m_vkContainerItemList[i].iItemID) // klickat på samma container
						bOpen = false;															 // som redan är öppnad?

					OpenContainerItem(bOpen, i, false);				
				}
				else if(!g_kMistClient.m_pkGui->m_bMouseRightPressed)
					s_bRightMouseButtonPressed = false;
			}
			else
			{
				m_vkContainerItemList[i].pkWnd->GetSkin()->m_unBorderSize = 0;
			}		
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

void InventoryDlg::PickUpFromGround(bool bLeftButtonPressed, int mx, int my)
{
	const float WAIT_TIME_PICKUP = 0.25f;

	float fTime = (float) SDL_GetTicks() / 1000.0f;

	if(bLeftButtonPressed)
	{
		if(fTime - m_fPickUpTimer > WAIT_TIME_PICKUP)
		{
			for(int i=0; i<m_vkInventoryItemList.size(); i++)
				if(m_vkInventoryItemList[i].iItemID == m_iItemUnderCursor)
				{
					if(bLeftButtonPressed)
					{
						m_kMoveSlot.bIsInventoryItem = true;
						m_kMoveSlot.m_iIndex = i;
					
						Point size = SlotSizeFromWnd(m_vkInventoryItemList[i].pkWnd);
						int id = m_vkInventoryItemList[i].pkWnd->GetSkin()->m_iBkTexID;

						m_kCursorRangeDiff = Point(0,0);
						g_kMistClient.m_pkGui->SetCursor( mx, my, id, -1, size.x*32, size.y*32);							
					}					
					break;
				}

			m_iItemUnderCursor = -1;
		}
	}
	else
	{
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

	m_kMoveSlot.bIsInventoryItem = bInventory;
	m_kMoveSlot.m_iIndex = iSlotIndex; // select new item

	(*pkVector)[iSlotIndex].pkWnd->Hide();

	Point size = SlotSizeFromWnd((*pkVector)[iSlotIndex].pkWnd);
	int id = (*pkVector)[iSlotIndex].pkWnd->GetSkin()->m_iBkTexID;

	int x = mx - m_kCursorRangeDiff.x, y = my - m_kCursorRangeDiff.y;
	g_kMistClient.m_pkGui->SetCursor( x, y, id, -1, size.x*32, size.y*32);	
	g_kMistClient.m_pkInputHandle->SetCursorInputPos(x,y);  
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

void InventoryDlg::OpenContainerWnd(int id, char slots_x, char slots_y)
{
	m_pkContainerWnd = g_kMistClient.GetWnd("ContainerWnd");
	m_pkContainerWnd->Show();

	m_iSlotsHorzContainer = slots_x;
	m_iSlotsVertContainer = slots_y;
	m_iActiveContainerID = id;

	CreateContainerGrid(slots_x, slots_y);

	m_pkContainerWnd->SetZValue(22);
	
	g_kMistClient.m_pkGui->PlaceWndFrontBack(m_pkContainerWnd, true); 
	g_kMistClient.m_pkGui->SetFocus(m_pkContainerWnd, false);
}

// Skapa bakgrundsbilden genom att klistra samman massa labels som har skapats från skript.
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
		
		if(pkWnd->GetSkin())
			delete pkWnd->GetSkin();
		
		g_kMistClient.m_pkGui->GetToolTip()->RemoveToolTip(pkWnd);
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

		char text[20] = "";
		if(vkItemList[i].m_iStackSize > 1)
			sprintf(text, "%i", vkItemList[i].m_iStackSize);

		ZGuiWnd* pkNewSlot = g_kMistClient.CreateWnd(Label, 
			szItemName, text, m_pkInventoryWnd, x, y, w, h, 0);
		pkNewSlot->Show();

		g_kMistClient.m_pkGui->GetToolTip()->AddToolTip(pkNewSlot, vkItemList[i].m_strName);
		
		g_kMistClient.SetFont(szItemName, "small7", 255, 255, 255, 0);

		((ZGuiLabel*) pkNewSlot)->m_eTextAlignment = ZGLA_BottomRight;

		if(g_kMistClient.m_pkGui->m_bMouseLeftPressed)
			if(m_iItemUnderCursor == vkItemList[i].m_iItemID)
			{
				float fTime = (float) SDL_GetTicks() / 1000.0f;
				m_fPickUpTimer = fTime;
				pkNewSlot->Hide();
			}

		pkNewSlot->SetSkin(new ZGuiSkin());
		pkNewSlot->GetSkin()->m_iBkTexID = m_pkTexMan->Load(
			string(string("data/textures/gui/items/") + vkItemList[i].m_strIcon).c_str(), 0) ;
		pkNewSlot->GetSkin()->m_bTileBkSkin = 0;
		pkNewSlot->GetSkin()->m_afBorderColor[0] = BD_R;
		pkNewSlot->GetSkin()->m_afBorderColor[1] = BD_G;
		pkNewSlot->GetSkin()->m_afBorderColor[2] = BD_B;

		if(m_iSelItemID == vkItemList[i].m_iItemID)
			pkNewSlot->GetSkin()->m_unBorderSize = 2;

		ITEM_SLOT kNewSlot;
		kNewSlot.pkWnd = pkNewSlot;
		kNewSlot.iItemID = vkItemList[i].m_iItemID;
		kNewSlot.bIsContainer = vkItemList[i].m_bIsContainer;
		m_vkInventoryItemList.push_back(kNewSlot);
	}	
}

void InventoryDlg::UpdateContainer(vector<MLContainerInfo>& vkItemList)
{
	// Remove all slots.
	for(int i=0; i<m_vkContainerItemList.size(); i++)
	{
		ZGuiWnd* pkWnd = m_vkContainerItemList[i].pkWnd;
		delete pkWnd->GetSkin();
		g_kMistClient.m_pkGui->GetToolTip()->RemoveToolTip(pkWnd);
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

		ZGuiWnd* pkNewSlot = g_kMistClient.CreateWnd(Label, szItemName, "", 
			m_pkContainerWnd, x, y, w, h, 0);
		pkNewSlot->Show();
		pkNewSlot->SetZValue(12121);
		g_kMistClient.SetFont(szItemName, "small7", 255, 255, 255, 0);
		((ZGuiLabel*) pkNewSlot)->m_eTextAlignment = ZGLA_BottomRight;

		g_kMistClient.m_pkGui->GetToolTip()->AddToolTip(pkNewSlot, vkItemList[i].m_strName);

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
		kNewSlot.bIsContainer = vkItemList[i].m_bIsContainer;
		m_vkContainerItemList.push_back(kNewSlot);
	}

	m_pkContainerWnd->SortChilds();
}

// When dropping a item, check for collision, reposition it and update the inventory.
void InventoryDlg::OnDropItem()
{
	float mx, my;
	g_kMistClient.m_pkInputHandle->MouseXY(mx,my);
	g_kMistClient.m_pkGui->SetCursor((int)mx+m_kCursorRangeDiff.x, (int)my+m_kCursorRangeDiff.y, 
		m_pkTexMan->Load("data/textures/gui/cursor.bmp", 0),
		m_pkTexMan->Load("data/textures/gui/cursor_a.bmp", 0), 32, 32);
	g_kMistClient.m_pkInputHandle->SetCursorInputPos(mx+m_kCursorRangeDiff.x,my+m_kCursorRangeDiff.y);

	Point upper_left;
	Rect rcMain, rc, rcScreenMain, rcDropWnd;

	int iSlotsHorz, iSlotsVert;

	if(m_kMoveSlot.bIsInventoryItem)
	{
		upper_left = UPPER_LEFT_INVENTORY;

		rcMain = m_vkInventoryItemList[m_kMoveSlot.m_iIndex].pkWnd->GetWndRect();
		rc = m_vkInventoryItemList[m_kMoveSlot.m_iIndex].pkWnd->GetWndRect();
		rcScreenMain = m_pkInventoryWnd->GetScreenRect();
		rcScreenMain.Left += UPPER_LEFT_INVENTORY.x;
		rcScreenMain.Top += UPPER_LEFT_INVENTORY.y;

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
		rcScreenMain.Left += UPPER_LEFT_CONTAINER.x;
		rcScreenMain.Top += UPPER_LEFT_CONTAINER.y;

		rcDropWnd = m_vkContainerItemList[m_kMoveSlot.m_iIndex].pkWnd->GetScreenRect();

		iSlotsHorz = m_iSlotsHorzContainer;
		iSlotsVert = m_iSlotsVertContainer;
	}

	bool bInsideGrid = rcScreenMain.Inside(rcDropWnd.Left, rcDropWnd.Top);
	if(bInsideGrid == false) bInsideGrid = rcScreenMain.Inside(rcDropWnd.Right, rcDropWnd.Top);
	if(bInsideGrid == false) bInsideGrid = rcScreenMain.Inside(rcDropWnd.Right, rcDropWnd.Bottom);
	if(bInsideGrid == false) bInsideGrid = rcScreenMain.Inside(rcDropWnd.Left, rcDropWnd.Bottom);

	if(bInsideGrid == false) // klickat utanför gridden eller i gridden för det andra fönstret.
	{
		if(m_kMoveSlot.bIsInventoryItem) // flytta ett inventory item till containern?
		{
			if(m_pkContainerWnd && m_pkContainerWnd->IsVisible() && 
				(m_pkContainerWnd->GetScreenRect().Inside(rcDropWnd.Left, rcDropWnd.Top) ||
				 m_pkContainerWnd->GetScreenRect().Inside(rcDropWnd.Right, rcDropWnd.Top) ||
				 m_pkContainerWnd->GetScreenRect().Inside(rcDropWnd.Right, rcDropWnd.Bottom) ||
				 m_pkContainerWnd->GetScreenRect().Inside(rcDropWnd.Left, rcDropWnd.Bottom) ) )
			{
				if(Entity* pkCharacter = g_kMistClient.m_pkEntityManager->GetEntityByID(g_kMistClient.m_iCharacterID))
					if(P_CharacterProperty* pkCharProp = (P_CharacterProperty*)pkCharacter->GetProperty("P_CharacterProperty"))
					{												
						Point p = SlotFromWnd(m_vkInventoryItemList[m_kMoveSlot.m_iIndex].pkWnd, false);
						Point s = SlotSizeFromWnd(m_vkInventoryItemList[m_kMoveSlot.m_iIndex].pkWnd);
						
						int container_id = m_iActiveContainerID;

						int collision_slot = TestForCollision(p, s, false);
						if(collision_slot > 0)
						{
							//p = Point(-1,-1); kommenterar bort för att kunna stapla grejer
							if(m_vkContainerItemList[collision_slot].bIsContainer)
							{
								container_id = m_vkContainerItemList[collision_slot].iItemID;	
								if(container_id != m_iActiveContainerID)			
								{
									g_kMistClient.SendRequestContainer(container_id);
								}
							}
						}

						g_kMistClient.SendMoveItem(
							m_vkInventoryItemList[m_kMoveSlot.m_iIndex].iItemID,container_id,p.x,p.y);
						m_vkInventoryItemList[m_kMoveSlot.m_iIndex].pkWnd->Hide();	
						m_kMoveSlot.m_iIndex = -1;

						//if(collision_slot > 0)
						//	g_kMistClient.RequestOpenInventory();
						
						return;
					}
			}
			else
			{				
				g_kMistClient.SendMoveItem(m_vkInventoryItemList[m_kMoveSlot.m_iIndex].iItemID, -1, -1, -1);

				for(int i=0; i<m_vkInventoryItemList.size(); i++)
					if(m_kMoveSlot.m_iIndex == i)
					{
						m_vkInventoryItemList[i].pkWnd->Hide();				
						m_kMoveSlot.m_iIndex = -1;
						break;
					}
			}
		}
		else // flyttar ett containeritem till inventory fönstret?
		{
			if(m_pkInventoryWnd->GetScreenRect().Inside(rcDropWnd.Left, rcDropWnd.Top) ||
				m_pkInventoryWnd->GetScreenRect().Inside(rcDropWnd.Right, rcDropWnd.Top) ||
				m_pkInventoryWnd->GetScreenRect().Inside(rcDropWnd.Right, rcDropWnd.Bottom) ||
				m_pkInventoryWnd->GetScreenRect().Inside(rcDropWnd.Left, rcDropWnd.Bottom))
			{
				if(Entity* pkCharacter = g_kMistClient.m_pkEntityManager->GetEntityByID(g_kMistClient.m_iCharacterID))
					if(P_CharacterProperty* pkCharProp = (P_CharacterProperty*)pkCharacter->GetProperty("P_CharacterProperty"))
					{
						Point p = SlotFromWnd(m_vkContainerItemList[m_kMoveSlot.m_iIndex].pkWnd, true);
						Point s = SlotSizeFromWnd(m_vkContainerItemList[m_kMoveSlot.m_iIndex].pkWnd);

						int container_id = pkCharProp->m_iInventory;
						
						int collision_slot = TestForCollision(p, s, true);
						if(collision_slot > 0)
						{
							//p = Point(-1,-1); kommenterar bort för att kunna stapla grejer
							if(m_vkInventoryItemList[collision_slot].bIsContainer)
							{
								container_id = m_vkInventoryItemList[collision_slot].iItemID;	
								if(container_id != m_iActiveContainerID)								
									g_kMistClient.SendRequestContainer(container_id);								
							}
						}

						g_kMistClient.SendMoveItem(
							m_vkContainerItemList[m_kMoveSlot.m_iIndex].iItemID, container_id, p.x, p.y);
						m_vkContainerItemList[m_kMoveSlot.m_iIndex].pkWnd->Hide();	
						m_kMoveSlot.m_iIndex = -1;

						//if(collision_slot > 0)
						//	g_kMistClient.SendRequestContainer(m_iActiveContainerID);
					
						return;
					}
			}
			else
			{
				g_kMistClient.SendMoveItem(m_vkContainerItemList[m_kMoveSlot.m_iIndex].iItemID, -1, -1, -1);

				for(int i=0; i<m_vkContainerItemList.size(); i++)
					if(m_kMoveSlot.m_iIndex == i)
					{
						m_vkContainerItemList[i].pkWnd->Hide();				
						m_kMoveSlot.m_iIndex = -1;
						break;
					}
			}
		}
	}
	else // Klickat innuti gridden (släppa i samma fönster)
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

		int iCollisionSlot = TestForCollision(m_kMoveSlot.m_iIndex, m_kMoveSlot.bIsInventoryItem);

		if(iCollisionSlot == -1)
		{
			if(m_kMoveSlot.bIsInventoryItem)
			{
				m_vkInventoryItemList[m_kMoveSlot.m_iIndex].pkWnd->Show();
				m_vkInventoryItemList[m_kMoveSlot.m_iIndex].pkWnd->SetPos(x, y, false, true);
				g_kMistClient.SendMoveItem(m_vkInventoryItemList[m_kMoveSlot.m_iIndex].iItemID, 
					-1, slot_x, slot_y);
				mx = x + m_kCursorRangeDiff.x + m_pkInventoryWnd->GetScreenRect().Left;
				my = y + m_kCursorRangeDiff.y + m_pkInventoryWnd->GetScreenRect().Top;
				g_kMistClient.m_pkInputHandle->SetCursorInputPos(mx, my);
			}
			else
			{
				m_vkContainerItemList[m_kMoveSlot.m_iIndex].pkWnd->Show();
				m_vkContainerItemList[m_kMoveSlot.m_iIndex].pkWnd->SetPos(x, y, false, true);
				g_kMistClient.SendMoveItem(m_vkContainerItemList[m_kMoveSlot.m_iIndex].iItemID, 
					-1, slot_x, slot_y);
				mx = x + m_kCursorRangeDiff.x + m_pkContainerWnd->GetScreenRect().Left;
				my = y + m_kCursorRangeDiff.y + m_pkContainerWnd->GetScreenRect().Top;
				g_kMistClient.m_pkInputHandle->SetCursorInputPos(mx, my);								
			}
			
			g_kMistClient.RequestOpenInventory();
		}
		else // har släppt ett item på ett annat item.
		{			
			int x = rcDropWnd.Left + rcDropWnd.Width()/2;
			int y = rcDropWnd.Top + rcDropWnd.Height()/2;

			pair<int,bool> iItemUnderCursor = GetItemFromScreenPos(x, y);

			if(iItemUnderCursor.second == true) // droped item to inventory wnd
			{				
				if(iItemUnderCursor.first != -1)	
					if(m_vkInventoryItemList[iItemUnderCursor.first].bIsContainer)		
					{
						for(int i=0; i<m_vkInventoryItemList.size(); i++)						
							if(i==iItemUnderCursor.first) 
							{
								g_kMistClient.SendMoveItem(
									m_vkInventoryItemList[m_kMoveSlot.m_iIndex].iItemID, 
									m_vkInventoryItemList[i].iItemID, -1, -1);

								if(m_vkInventoryItemList[i].iItemID != m_iActiveContainerID)								
									g_kMistClient.SendRequestContainer(m_vkInventoryItemList[i].iItemID);	
								break;
							}
					}		
					else
					{
						// TODO: Släppa item på annat item som går att stacka.
						g_kMistClient.SendMoveItem(m_vkInventoryItemList[m_kMoveSlot.m_iIndex].iItemID, 
							-1, slot_x, slot_y);
					}
			}
			else // droped item to container wnd
			{
				if(iItemUnderCursor.first != -1)	
					if(m_vkContainerItemList[iItemUnderCursor.first].bIsContainer)
					{
						for(int i=0; i<m_vkContainerItemList.size(); i++)						
							if(i==iItemUnderCursor.first) 
							{								
								g_kMistClient.SendMoveItem(
									m_vkContainerItemList[m_kMoveSlot.m_iIndex].iItemID, 
									m_vkContainerItemList[i].iItemID, -1, -1);

								//if(m_vkContainerItemList[i].iItemID != m_iActiveContainerID)								
								//	g_kMistClient.SendRequestContainer(m_vkContainerItemList[i].iItemID);	
								break;
							}
					}
					else
					{
						// TODO: Släppa item på annat item som går att stacka.
						g_kMistClient.SendMoveItem(m_vkContainerItemList[m_kMoveSlot.m_iIndex].iItemID, 
							-1, slot_x, slot_y);
					}

					//g_kMistClient.SendRequestContainer(m_iActiveContainerID);	
			}

			g_kMistClient.RequestOpenInventory();
		}
	}
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

pair<int, bool> InventoryDlg::GetItemFromScreenPos(int x, int y)
{
	Rect rc;

	for(int i=0; i<m_vkInventoryItemList.size(); i++)
	{
		rc = m_vkInventoryItemList[i].pkWnd->GetScreenRect();
		if(rc.Inside(x,y) && i != m_kMoveSlot.m_iIndex)
			return pair<int,bool>(i,true);
	}

	for(int i=0; i<m_vkContainerItemList.size(); i++)
	{
		rc = m_vkContainerItemList[i].pkWnd->GetScreenRect();
		if(rc.Inside(x,y) && i != m_kMoveSlot.m_iIndex)
			return pair<int,bool>(i,false);
	}

	return pair<int,bool>(-1,0);
}