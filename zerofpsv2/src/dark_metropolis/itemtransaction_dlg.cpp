#include "dark_metropolis.h"
#include "itemtransaction_dlg.h"
#include "handleagents_dlg.h"

CItemTransactionDlg::CItemTransactionDlg() : CGameDlg(
	"ItemTransactionWnd", &g_kDM)
{
	m_iActiveContainer = 0;
	m_iSelFocusCharItemIndex = -1;
	m_iSelStockroomItemIndex = -1;
}

CItemTransactionDlg::~CItemTransactionDlg() 
{
}

void CItemTransactionDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	if(strClickName == "ItemTransactionCloseBn")
	{
	//	m_pkGui->KillWndCapture(); 
		ShowWnd("ItemTransactionWnd", false/*, true*/);
		ShowWnd("ItemRemoveWnd", false/*, true*/);
		ShowWnd("ItemAddWnd", false/*, true*/);

		if(GetWnd("AgentsWnd") && GetWnd("AgentsWnd")->IsVisible())
		{
			m_pkGui->KillWndCapture(); 
			m_pkGui->SetCaptureToWnd(GetWnd("AgentsWnd"));
		}
		else
		if(GetWnd("HQWnd")->IsVisible())
		{
			m_pkGui->KillWndCapture(); 
			m_pkGui->SetCaptureToWnd(GetWnd("HQWnd"));
		}

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 

		return;
	}
	else
	if(strClickName == "RemoveItemBn")
	{
		if(m_iSelFocusCharItemIndex != -1)
		{
			MoveItemToStockroom(m_vkFocusCharItems[m_iSelFocusCharItemIndex]);
			InitDlg();
		}
	}
	else
	if(strClickName == "AddItemBn")
	{
		if(m_iSelStockroomItemIndex != -1)
		{
			MoveItemFromStockroom(m_vkStockroomItems[m_iSelStockroomItemIndex]);
			InitDlg();
		}
	}
	else
	if(strClickName.find("FocusCharItemBn") != string::npos)
	{
		m_iSelFocusCharItemIndex = -1;
		ZGuiCheckbox* pkButton = ((ZGuiCheckbox*)GetWnd((char*)strClickName.c_str()));

		for(int i=0; i<m_vkFocusCharItems.size(); i++)
		{
			if(m_vkFocusCharItems[i].m_pkMoveButton != pkButton)
				m_vkFocusCharItems[i].m_pkMoveButton->UncheckButton();
			else
			{
				m_iSelFocusCharItemIndex = i;
				m_vkFocusCharItems[i].m_pkMoveButton->CheckButton();
			}
		}

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName.find("StockRoomItemBn") != string::npos)
	{
		m_iSelStockroomItemIndex = -1;
		ZGuiCheckbox* pkButton = ((ZGuiCheckbox*)GetWnd((char*)strClickName.c_str()));

		for(int i=0; i<m_vkStockroomItems.size(); i++)
		{
			if(m_vkStockroomItems[i].m_pkMoveButton != pkButton)
				m_vkStockroomItems[i].m_pkMoveButton->UncheckButton();
			else
			{
				m_iSelStockroomItemIndex = i;
				m_vkStockroomItems[i].m_pkMoveButton->CheckButton();
			}
		}

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
}

void CItemTransactionDlg::OnSelectCB(ZGuiCombobox* pkCombobox, int iItemIndex)
{
	if(pkCombobox == NULL)
		return;

	string strListbox = pkCombobox->GetName();
	
	if(strListbox == "ActiveContatinerList")
	{
		string strListItem;
		strListItem = pkCombobox->GetListbox()->GetItem(iItemIndex)->GetText();

		if(strListItem == "Backpack")
		{
			m_iActiveContainer = BACKPACK;
			printf("switching to BACKPACK container\n");
		}
		else
		if(strListItem == "Armor")
		{
			m_iActiveContainer = ARMOR;
			printf("switching to ARMOR container\n");
		}
		else
		if(strListItem == "Cybernetics")
		{
			m_iActiveContainer = CYBERNETICS;
			printf("switching to CYBERNETICS container\n");
		}
		else
		if(strListItem == "Quickitem")
		{
			m_iActiveContainer = QUICKITEM;
			printf("switching to QUICKITEM container\n");
		}
		else
		if(strListItem == "Weapon")
		{
			m_iActiveContainer = WEAPON;
			printf("switching to WEAPON container\n");
		}
		else
			return;

		InitDlg();
	}
}

bool CItemTransactionDlg::InitDlg()
{
	int x, y, w, h;
	unsigned int i, c;
	const int CELL_SIZE = 31;
	char szName[50];
	P_DMCharacter* pkCharProperty=NULL;
	Entity* pkHQ = GetDMObject(HQ);

	if(pkHQ == NULL)
		return false;
	
	DMContainer* pkStookroom = 
		((P_DMHQ*)pkHQ->GetProperty("P_DMHQ"))->m_pkStockroom;

	int iAgentID = ((CHandleAgents*)GetGameDlg(HANDLE_AGENTS_DLG))->GetSelAgent();
	
	if(iAgentID == -1)
		return false;

	if((pkCharProperty = (P_DMCharacter*) 
			GetObject(iAgentID)->GetProperty("P_DMCharacter")) == NULL)
	{
		return false;
	}

	for(i=0; i<m_vkFocusCharItems.size(); i++)
		m_vkFocusCharItems[i].m_pkMoveButton->Hide();

	for(i=0; i<m_vkStockroomItems.size(); i++)
		m_vkStockroomItems[i].m_pkMoveButton->Hide();
	
	m_vkFocusCharItems.clear();
	m_vkStockroomItems.clear();

	int item_counter1=0;
	int item_counter2=0;

	DMContainer* akContainerObjects[] = 
	{
		pkCharProperty->m_pkBackPack,		// 0) Backpack
		pkCharProperty->m_pkBody,			// 1) Armor
		pkCharProperty->m_pkImplants,		// 2) Cybernectics
		pkCharProperty->m_pkBelt,			// 3) Quickitem
		pkCharProperty->m_pkHand,			// 4) Weapon

		pkStookroom								// 5) Stockroom

	};

	for( c=0; c<6; c++)
	{
		DMContainer* pkContainer = akContainerObjects[c];
		
		Rect kRect = Rect(383,43,0,0);

		if(c == 5)
		{
			kRect = Rect(43,43,0,0);
			pkContainer->Print();
		}

		if(pkContainer && (m_iActiveContainer == c || c == 5))
		{
			vector<ContainerInfo> kItemList;
			pkContainer->GetItemList(&kItemList);

			for(unsigned int i=0; i<kItemList.size(); i++)
			{
				x = kRect.Left + kItemList[i].m_iItemX*CELL_SIZE;
				y = kRect.Top + kItemList[i].m_iItemY*CELL_SIZE;
				w = kItemList[i].m_iItemW*CELL_SIZE;
				h = kItemList[i].m_iItemH*CELL_SIZE;

				if(c!=5)
					sprintf(szName, "FocusCharItemBn%i", item_counter1++);
				else
					sprintf(szName, "StockRoomItemBn%i", item_counter2++);
					
				ITEM_MOVE_INFO kMoveInfo;

				ZGuiCheckbox* pkButton;
				ZGuiSkin* pkSkinUp, *pkSkinDown;
				
				if((pkButton=(ZGuiCheckbox*)GetWnd(szName)) == NULL)
				{
					pkSkinUp = new ZGuiSkin();
					pkSkinDown = new ZGuiSkin();
				}
				else
				{
					pkSkinUp = pkButton->GetUncheckedSkin();
					pkSkinDown = pkButton->GetCheckedSkin();
				}

				pkButton = (ZGuiCheckbox*) CreateWnd(Checkbox, szName, 
					"ItemTransactionWnd", "", x, y, w, h, 0);

				pkButton->ZGuiWnd::Resize(w,h,true);
				pkButton->SetPos(x,y,false,true);
				pkButton->SetMoveArea(pkButton->GetScreenRect()); 

				string strIcon = "data/textures/gui/dm/items/" + kItemList[i].m_strIcon;

				pkSkinUp->m_iBkTexID = GetTexID((char*)strIcon.c_str());
				pkSkinDown->m_iBkTexID = GetTexID((char*)strIcon.c_str());
				pkSkinDown->m_unBorderSize = 2;
				pkSkinDown->m_afBorderColor[0] = 
				pkSkinDown->m_afBorderColor[1] = 
				pkSkinDown->m_afBorderColor[2] = 0;

				pkButton->SetButtonCheckedSkin(pkSkinDown);
				pkButton->SetButtonUncheckedSkin(pkSkinUp);

				pkButton->m_bUseAlhpaTest = false;
				pkButton->Show();

				// Move icon to top and set movearea to parent window
				static int s_okaZ = 10000;
				pkButton->m_iZValue = s_okaZ++;

				GetWnd("ActiveContatinerList")->m_iZValue = s_okaZ++;

				GetWnd("ItemTransactionWnd")->SortChilds(); 

				kMoveInfo.m_pkMoveButton = pkButton;
				kMoveInfo.m_kFromContainer.pkContainer = pkContainer;
				kMoveInfo.m_pMoveObject = pkContainer->GetItem(
					kItemList[i].m_iItemX, kItemList[i].m_iItemY);
				
				if(c != 5)
					m_vkFocusCharItems.push_back(kMoveInfo);
				else
					m_vkStockroomItems.push_back(kMoveInfo);
			}
		}
	}

	if(GetWnd("ActiveContatinerList") != NULL)
	{
		((ZGuiCombobox*)
			GetWnd("ActiveContatinerList"))->GetListbox()->SelItem(
				m_iActiveContainer);

		char* szText =
			((ZGuiCombobox*)
			GetWnd("ActiveContatinerList"))->GetListbox()->GetSelItem()->GetText(); 

		((ZGuiCombobox*)
			GetWnd("ActiveContatinerList"))->SetLabelText( szText);
	}

	return true;
}

void CItemTransactionDlg::MoveItemToStockroom(ITEM_MOVE_INFO kItem)
{
	Entity* pkHQ = GetDMObject(HQ);

	DMContainer* pkStookroom = 
		((P_DMHQ*)pkHQ->GetProperty("P_DMHQ"))->m_pkStockroom;

	if(kItem.m_kFromContainer.pkContainer->MoveItem(
		*kItem.m_pMoveObject, pkStookroom))
	{
		printf("Moving item id %i from container id % to container id %i\n",
			*kItem.m_pMoveObject, kItem.m_kFromContainer.pkContainer->GetOwnerID(), 
			pkStookroom->GetOwnerID());
	}
	else
	{
		printf("Failed to move item id %i from container id % to container id %i\n",
			*kItem.m_pMoveObject, kItem.m_kFromContainer.pkContainer->GetOwnerID(), 
			pkStookroom->GetOwnerID());
	}
}

void CItemTransactionDlg::MoveItemFromStockroom(ITEM_MOVE_INFO kItem)
{
	if(m_iActiveContainer == -1)
		return;

	P_DMCharacter* pkCharProperty=NULL;

	int iAgentID = ((CHandleAgents*)GetGameDlg(HANDLE_AGENTS_DLG))->GetSelAgent();
	
	if(iAgentID == -1)
		return;

	if((pkCharProperty = (P_DMCharacter*) 
			GetObject(iAgentID)->GetProperty("P_DMCharacter")) == NULL)
	{
		return;
	}

	DMContainer* akContainerObjects[] = 
	{
		pkCharProperty->m_pkBackPack,		// 0) Backpack
		pkCharProperty->m_pkBody,			// 1) Armor
		pkCharProperty->m_pkImplants,		// 2) Cybernectics
		pkCharProperty->m_pkBelt,			// 3) Quickitem
		pkCharProperty->m_pkHand,			// 4) Weapon
	};

	Entity* pkHQ = GetDMObject(HQ);

	DMContainer* pkContainer = akContainerObjects[m_iActiveContainer];

	if(kItem.m_kFromContainer.pkContainer->MoveItem(
		*kItem.m_pMoveObject, pkContainer))
	{
		printf("Moving item id %i from container id % to container id %i\n",
			*kItem.m_pMoveObject, kItem.m_kFromContainer.pkContainer->GetOwnerID(), 
			pkContainer->GetOwnerID());
	}
	else
	{
		printf("Failed to move item id %i from container id % to container id %i\n",
			*kItem.m_pMoveObject, kItem.m_kFromContainer.pkContainer->GetOwnerID(), 
			pkContainer->GetOwnerID());
	}
}


