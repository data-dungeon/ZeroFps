

#include "mistclient.h"
#include "gui_inventory.h"

extern MistClient	g_kMistClient;

void GuiMsgInventoryDlg( string strMainWnd, string strController, unsigned int msg, int numparms, void *params )
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
	m_pkSelectedWnd = NULL;
}

InventoryDlg::~InventoryDlg()
{
	g_kMistClient.m_pkGui->UnregisterWindow(m_pkMainWnd);
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
	for(int i=0; i<m_vkItemList.size(); i++)
	{		
		if(m_vkItemList[i].pkWnd->GetScreenRect().Inside(mx, my))
		{
			m_vkItemList[i].pkWnd->GetSkin()->m_unBorderSize = 2;

			if(bLeftButtonPressed)
			{
				m_vkItemList[i].pkWnd->GetSkin()->m_afBorderColor[0] = 1;
				m_vkItemList[i].pkWnd->GetSkin()->m_afBorderColor[1] = 0;
				m_vkItemList[i].pkWnd->GetSkin()->m_afBorderColor[2] = 0;

				if(m_pkSelectedWnd == NULL)
					m_pkSelectedWnd = m_vkItemList[i].pkWnd; // set new selection window
			}
			else
			{
				m_vkItemList[i].pkWnd->GetSkin()->m_afBorderColor[0] = 1;
				m_vkItemList[i].pkWnd->GetSkin()->m_afBorderColor[1] = 1;
				m_vkItemList[i].pkWnd->GetSkin()->m_afBorderColor[2] = 1;

				if(m_pkSelectedWnd)
				{
					OnDropItem();
				}

				m_pkSelectedWnd = NULL;
			}

		}
		else
		{
			m_vkItemList[i].pkWnd->GetSkin()->m_unBorderSize = 0;
		}
	}

	if(m_pkSelectedWnd)
	{
		m_pkSelectedWnd->SetPos(mx,my, true, true);
	}
}

void InventoryDlg::Update(vector<MLContainerInfo>& vkItemList)
{
	printf("size of item list = %i\n", vkItemList.size());

	m_pkSelectedWnd = NULL;

	// Ta bort alla gamla items
	for(int i=0; i<m_vkItemList.size(); i++)
	{
		ZGuiWnd* pkWnd = m_vkItemList[i].pkWnd;
		delete pkWnd->GetSkin();
		g_kMistClient.m_pkGui->UnregisterWindow( pkWnd );
	}

	m_vkItemList.clear();

	int x,y,w,h;
	char szItemName[128];
	for(int i=0; i<vkItemList.size(); i++)
	{
		sprintf(szItemName, "InventoryItemLabel%i", i);
		x = UPPER_LEFT.x + vkItemList[i].m_cItemX * ICON_WIDTH + vkItemList[i].m_cItemX;
		y = UPPER_LEFT.y + vkItemList[i].m_cItemY * ICON_HEIGHT + vkItemList[i].m_cItemY;
		w = vkItemList[i].m_cItemW * ICON_WIDTH;
		h = vkItemList[i].m_cItemH * ICON_HEIGHT;

		ZGuiWnd* pkNewSlot = g_kMistClient.CreateWnd(Label, szItemName, "InventoryWnd", "", x, y, w, h, 0);
		pkNewSlot->Show();

		pkNewSlot->SetSkin(new ZGuiSkin());
		pkNewSlot->GetSkin()->m_iBkTexID = m_pkTexMan->Load(
			string(string("data/textures/gui/items/") + vkItemList[i].m_strIcon).c_str(), 0) ;	

		ITEM_SLOT kNewSlot;
		kNewSlot.pkWnd = pkNewSlot;
		m_vkItemList.push_back(kNewSlot);
	}
}

void InventoryDlg::OnDropItem()
{
	Rect rcMain = m_pkMainWnd->GetWndRect();
	Rect rc = m_pkSelectedWnd->GetWndRect();

	if(rc.Left < UPPER_LEFT.x)
		rc.Left = UPPER_LEFT.x;

	if(rc.Top < UPPER_LEFT.y)
		rc.Top = UPPER_LEFT.y;

	int slot_w = rc.Width() / ICON_WIDTH;
	int slot_h = rc.Height() / ICON_HEIGHT;

	int slot_x = (rc.Left - UPPER_LEFT.x) / ICON_WIDTH;
	int slot_y = (rc.Top  - UPPER_LEFT.y) / ICON_HEIGHT;

	if(slot_x > SLOTTS_HORZ-slot_w) slot_x = SLOTTS_HORZ-slot_w;
	if(slot_y > SLOTTS_VERT-slot_h) slot_y = SLOTTS_VERT-slot_h;

	int x = UPPER_LEFT.x + slot_x * ICON_WIDTH + slot_x;
	int y = UPPER_LEFT.y + slot_y * ICON_HEIGHT + slot_y;

	m_pkSelectedWnd->SetPos(x, y, false, true);
}