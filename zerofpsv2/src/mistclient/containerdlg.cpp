#include "inventorydlg.h"
#include "containerdlg.h"
#include "../mcommon/p_clientcontrol.h"
#include "../mcommon/p_item.h"
#include "../mcommon/p_container.h"
#include "../zerofpsv2/basic/zfsystem.h"
#include "../zerofpsv2/engine/entitymanager.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//ContainerDlg
//////////////////////////////////////////////////////////////////////////////////////////////////////////

ContainerDlg::ContainerDlg(ZGuiApp* pkApp)
{
	m_pkInventoryDlg = NULL;
	m_pkContainer = NULL;
	m_pkApp = pkApp;
	Create();
}

ContainerDlg::~ContainerDlg()
{

}

void ContainerDlg::SetInventoryPointer(InventoryDlg* pkInventory)
{
	m_pkInventoryDlg = pkInventory;
}

void ContainerDlg::Create()
{
	m_pkGui = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));
	m_pkTexMan = static_cast<TextureManager*>(
		g_ZFObjSys.GetObjectPtr("TextureManager"));

	int screen_w = 800; //m_pkApp->GetWidth(); 

	//
	// Create main wnd
	//
	m_pkApp->CreateWnd( Wnd, "ContainerDlg", "MainWnd", "", screen_w-(256-27), 288, 256-27, 256, 0); 
	m_pkDlgWnd = m_pkApp->GetWnd("ContainerDlg");

	ZGuiSkin* pkSkin = new ZGuiSkin( 
		m_pkTexMan->Load("data/textures/gui/container_wnd.bmp", 0), 0 );
	m_pkDlgWnd->SetSkin(pkSkin); 

	//
	// Create label
	//
	m_pkApp->CreateWnd( Label, "ContainerWndLabel", 
		"ContainerDlg", "Container", 12, 8, 100, 20, 0); 

	//
	// Create scrollbar
	//
	int x_pos = SLOT_SIZE*CONTAINER_ITEM_COLS+8;
	int y_pos = 30;
	m_pkApp->CreateWnd(Scrollbar, "ContainerScrollbar", "ContainerDlg", "", 
		x_pos, y_pos-2, 16, SLOT_SIZE*(CONTAINER_ITEM_ROWS-1)+2, 0);
	((ZGuiScrollbar*)m_pkApp->GetWnd(
		"ContainerScrollbar"))->SetScrollInfo(0,100,0.10f,0);

	// 
	// Create "take all button"
	//
	m_pkApp->CreateWnd( Button, "ContainerWndTakeAllBn", "ContainerDlg", 
		"Take all", 12, 256-30, 100, 20, 0);

	// 
	// Create close button
	//
	m_pkApp->CreateWnd( Button, "ContainerWndCloseBn", "ContainerDlg", 
		"Close", 117, 256-30, 100, 20, 0);

	//
	// Create item buttons
	//
	ZGuiSkin* pkButtonSkinUp = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/itembn_u.bmp", 0),0);

	ZGuiSkin* pkButtonSkinDown = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/itembn_d.bmp", 0),0);

	m_rcClipperArea = Rect(12, 30-2, 12+SLOT_SIZE*4, 30+SLOT_SIZE*4);
	m_rcClipperArea += m_pkDlgWnd->GetScreenRect(); 

	for(int y=0; y<CONTAINER_ITEM_ROWS; y++)
		for(int x=0; x<CONTAINER_ITEM_COLS; x++)
		{
			char szName[50];
			sprintf(szName, "ContainerDlgItemSlot%i", y*10+x);
			m_pkApp->CreateWnd(Button, szName, "ContainerDlg", "", 
				8+x*(SLOT_SIZE), 30+y*(SLOT_SIZE), SLOT_SIZE, SLOT_SIZE, 0);

			ZGuiButton* pkButton = (ZGuiButton*)m_pkApp->GetWnd(szName);
			pkButton->SetButtonUpSkin(pkButtonSkinUp);
			pkButton->SetButtonHighLightSkin(pkButtonSkinUp);
			pkButton->SetButtonDownSkin(pkButtonSkinDown);
			pkButton->m_bUseAlhpaTest = false;

			pkButton->SetClipperArea(m_rcClipperArea); 
			pkButton->m_bUseClipper = true; 

			m_pkContatinerButtons[y][x] = pkButton;
		}

	for(int i=0; i<70; i++)
	{
		int nr = rand() % 10;

		switch(nr)
		{
			case 0: AddItem("bread.bmp", NULL);		 break;
			case 1: AddItem("bottle.bmp", NULL);	 break;
			case 2: AddItem("candle.bmp", NULL);	 break;
			case 3: AddItem("dagger.bmp", NULL);	 break;
			case 4: AddItem("gembag1.bmp", NULL);	 break;
			case 5: AddItem("meat.bmp", NULL);		 break;
			case 6: AddItem("mugg.bmp", NULL);		 break;
			case 7: AddItem("potion.bmp", NULL);	 break;
			case 8: AddItem("spellbook.bmp", NULL); break;
			case 9: AddItem("apple.bmp", NULL);		 break;
		}
	}

	// Hide window from start
	m_pkDlgWnd->Hide();
}

void ContainerDlg::SetContainer(P_Container* pkContainer)
{	
	if(pkContainer != m_pkContainer)
	{
		m_pkContainer = pkContainer;

		// clear all icons
		RemoveAllItems(); 
	}
}

void ContainerDlg::ToggleOpen(bool bOpen)
{
	if(bOpen)
	{
		m_pkDlgWnd->Show();
		m_pkDlgWnd->SetFocus();
		m_pkGui->SetFocus( m_pkDlgWnd);

		if(m_pkContainer)
		{
			m_pkContainer->RequestUpdateFromServer(); 

			printf("m_pkContainer->RequestUpdateFromServer()\n");
			printf("m_pkContainer->m_iCapacity = %i\n", m_pkContainer->m_iCapacity);
		}
		else
		{
			printf("Failed to open container! P_Container is NULL\n");
		}
	}
	else
	{
		m_pkDlgWnd->Hide();
		ZGuiWnd* pkPanel = m_pkApp->GetWnd("PanelBkWnd");
		pkPanel->SetFocus();
		m_pkGui->SetFocus(pkPanel);
	}
}

void ContainerDlg::OnScroll(int iID, int iPos)
{
	static int prev_pos = 0;
	int offset;

	if(prev_pos == iPos)
		return;
	else
	{
		offset = iPos-prev_pos;
		prev_pos = iPos;
	}

	int screen_h = 600; //m_pkApp->GetHeight();
	string strScrollbarName = GetWndByID(iID);

	if(strScrollbarName == "ContainerScrollbar")
	{
		Rect m_rcClipperArea(12, 30-2, 12+SLOT_SIZE*4, 30+SLOT_SIZE*4);
		m_rcClipperArea += m_pkDlgWnd->GetScreenRect(); 

		// Move buttons
		for(int y=0; y<CONTAINER_ITEM_ROWS; y++)
			for(int x=0; x<CONTAINER_ITEM_COLS; x++)
			{
				m_pkContatinerButtons[y][x]->Move(0, -offset*8, true, true);

				Rect rcButton = m_pkContatinerButtons[y][x]->GetScreenRect();

				// scroll down
				if(rcButton.Bottom <= m_rcClipperArea.Top)
					m_pkContatinerButtons[y][x]->Move(0, 
					48*CONTAINER_ITEM_ROWS , true, true);

				// scroll up
				if(rcButton.Top >= m_rcClipperArea.Bottom)
					m_pkContatinerButtons[y][x]->Move(0, 
					-48*CONTAINER_ITEM_ROWS, true, true);

				m_pkContatinerButtons[y][x]->SetMoveArea(
					m_pkContatinerButtons[y][x]->GetScreenRect(), true); 
			}

		// Move icon slots
		vector<pair<ZGuiLabel*, Entity*> >::iterator itIcons = m_vkItems.begin();
		for( ; itIcons != m_vkItems.end(); itIcons++)
		{
			ZGuiLabel* pkLabel = (*itIcons).first;
			pkLabel->Move(0, -offset*8, true, true);

			Rect rcButton = pkLabel->GetScreenRect();

			if(rcButton.Bottom <= m_rcClipperArea.Top || rcButton.Top >= m_rcClipperArea.Bottom)
				pkLabel->Hide();
			else
				pkLabel->Show();
		}

	}
}

string ContainerDlg::GetWndByID(int iID)
{
	ZGuiWnd* pkParent = m_pkDlgWnd;

	list<ZGuiWnd*> childs;
	pkParent->GetChildrens(childs);

	list<ZGuiWnd*>::iterator itChild = childs.begin() ;

	ZGuiWnd* pkClickWnd = NULL;
	
	for( ; itChild != childs.end(); itChild++)
	{
		if((*itChild)->GetID() == (unsigned int) iID)
		{
			pkClickWnd = *itChild;
			return string(pkClickWnd->GetName());
		}
	}

	return string("");
}

void ContainerDlg::OnCommand(int iID)
{
	string strCtrlName = GetWndByID(iID);

	if(strCtrlName == "ContainerWndCloseBn")
	{
		ToggleOpen(false);
	}
	else
	if(strCtrlName == "ContainerWndTakeAllBn")
	{
		TakeAll();
		ToggleOpen(false);
	}
	else
	{
		ZGuiWnd* pkWnd = m_pkApp->GetWnd( GetWndByID(iID) );

		Rect rc = pkWnd->GetScreenRect();
		Entity* pkObject = GetItemFromCursorPos(rc.Left+4, rc.Top+4);

		if(pkObject)
			MoveItemToInventory(pkObject);
	}
}

void ContainerDlg::TakeAll()
{
	//
	// Let the gui remove all labels and delete all skins
	//
	for(unsigned int i=0; i<m_vkItems.size(); i++)
	{
		m_pkInventoryDlg->AddItem( m_vkItems[i].second );

		ZGuiLabel* pkLabel = m_vkItems[i].first;
		ZGuiSkin* pkSkin = pkLabel->GetSkin();

		delete pkSkin;
		delete pkLabel;
	}

	//
	// Clear data structures
	//
	m_kContainerObjects.clear();
	m_vkItems.clear(); 
}

bool ContainerDlg::AddItem(char *szPic, Entity* pkObject)
{
	if(MAX_NUM_ITEMS < m_vkItems.size())
	{
		printf("To much item for container!\n");
		return false;
	}

	Rect rc;
	ZGuiSkin* pkSkin = new ZGuiSkin();
	int tex, texa;
	
	// Get texture id
	string strPath = "data/textures/gui/items/";
	string strFileName = strPath; strFileName += szPic;
	tex = m_pkTexMan->Load(strFileName.c_str(), 0);
	string strAlphaTex = strFileName;
	int pos = strAlphaTex.find_last_of(".");
	strAlphaTex.insert(pos, "_a", 2);
	texa = m_pkTexMan->Load(strAlphaTex.c_str(), 0);

	// Calculate rect
	int top_x = 18, top_y = 36;
	int row = m_vkItems.size() / CONTAINER_ITEM_COLS;
	int col = m_vkItems.size() - (row*CONTAINER_ITEM_COLS);

	rc.Left = top_x+col*48;
	rc.Top = top_y+row*48;

	rc.Right = rc.Left + 32;
	rc.Bottom = rc.Top + 32;

	ZGuiLabel* m_pkLabel = new ZGuiLabel(rc,m_pkDlgWnd,true,0);

	pkSkin->m_iBkTexID = tex;
	pkSkin->m_iBkTexAlphaID = texa;
	m_pkLabel->SetSkin(pkSkin);

	m_pkLabel->SetClipperArea(m_rcClipperArea); 
	m_pkLabel->m_bUseClipper = true; 

	m_pkLabel->SetZValue(12121212);
	m_pkDlgWnd->SortChilds();

	m_vkItems.push_back( pair<ZGuiLabel*, Entity*>(m_pkLabel, pkObject) );
	m_kContainerObjects.insert(pkObject);

	return true;
}

void ContainerDlg::Update()
{
	if(m_pkContainer)
	{
		vector<Entity*> vkObjects;
		m_pkContainer->GetAllItemsInContainer(&vkObjects); 

      cout << "Object form container:" << vkObjects.size() << endl;

		for(unsigned int i=0; i<vkObjects.size(); i++)
		{
			// om den inte redan finns, lägg till den
			if(m_kContainerObjects.find(vkObjects[i]) == m_kContainerObjects.end())
			{
				P_Item* pkItemProp = (P_Item*)vkObjects[i]->GetProperty ("P_Item"); 
				AddItem(pkItemProp->m_pkItemStats->m_szPic, vkObjects[i]);
			}
		}
	}
}

void ContainerDlg::RemoveAllItems()
{
	//
	// Let the gui remove all labels and delete all skins
	//
	for(unsigned int i=0; i<m_vkItems.size(); i++)
	{
		ZGuiLabel* pkLabel = m_vkItems[i].first;
		ZGuiSkin* pkSkin = pkLabel->GetSkin();

		delete pkSkin;
		delete pkLabel;
	}

	//
	// Clear data structures
	//
	m_kContainerObjects.clear();
	m_vkItems.clear(); 
}

void ContainerDlg::RemoveItem(Entity* pkObjectToRemove)
{
	vector< pair<ZGuiLabel*, Entity*> >::iterator itSlots = m_vkItems.begin(); 

	for( ; itSlots != m_vkItems.end(); itSlots++)
	{
		Entity* pkObject = (*itSlots).second;

		if(pkObject == pkObjectToRemove)
		{
			m_kContainerObjects.erase( pkObjectToRemove );

			ZGuiLabel* pkLabel = (*itSlots).first;
			ZGuiSkin* pkSkin = pkLabel->GetSkin();

			delete pkSkin;
			delete pkLabel;

			m_vkItems.erase( itSlots);
			break;
		}
	}
}

bool ContainerDlg::MoveItemToInventory(Entity *pkObject)
{
	if(m_pkInventoryDlg == NULL)
	{
		printf("Failed to move object to inventory, m_pkInventoryDlg = NULL\n");
		return false;
	}

	m_pkInventoryDlg->AddItem( pkObject );
	RemoveItem(pkObject);
	return true;
}

Entity* ContainerDlg::GetItemFromCursorPos(int x, int y)
{
	vector< pair<ZGuiLabel*, Entity*> >::iterator itItem = m_vkItems.begin();

	for( ; itItem != m_vkItems.end(); itItem++)
	{
		Rect rc = itItem->first->GetScreenRect();
		if(x >= rc.Left && x <= rc.Right && y >= rc.Top && y <= rc.Bottom)
			return itItem->second;
	}

	return NULL;
}