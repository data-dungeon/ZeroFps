#include "dark_metropolis.h"
#include "itemtransaction_dlg.h"
#include "handleagents_dlg.h"
#include "../mcommon/p_dmshop.h"

CItemTransactionDlg::CItemTransactionDlg() : CGameDlg(
	"ItemTransactionWnd", &g_kDM)
{
	m_iActiveContainer = 0;
	m_iSelFocusCharItemIndex = -1;
	m_iSelInventoryItemIndex = -1;
	m_eViewMode = shop;
}

CItemTransactionDlg::~CItemTransactionDlg() 
{
}

void CItemTransactionDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	if(strClickName == "ItemRemoveTransactionPrevPageBn")
	{
		if(m_iActiveContainer > 0)
		{
			m_iActiveContainer--;
			InitDlg();
		}

		SetText("ItemInfoLabel", GetGetContainerName(m_iActiveContainer));
	}
	else
	if(strClickName == "ItemRemoveTransactionNextPageBn")
	{
		if(m_iActiveContainer < 4)
		{
			m_iActiveContainer++;
			InitDlg();
		}

		SetText("ItemInfoLabel", GetGetContainerName(m_iActiveContainer));
	}
	else
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

		P_DMGameInfo* pkGameInfo = (P_DMGameInfo*)
			GetDMObject(GAME_INFO)->GetProperty("P_DMGameInfo");

		char szText[50];
		sprintf(szText, "Money: %i", pkGameInfo->m_iMoney);
		SetText("AgentsMoneyLabel", szText);

		return;
	}
	else
	if(strClickName == "RemoveItemBn")
	{
		if(m_iSelFocusCharItemIndex != -1)
		{
			if(m_eViewMode == store_room)
				MoveItemToStockroom(m_vkFocusCharItems[m_iSelFocusCharItemIndex]);
			else
			{
				// Sälja något till affären
				if(Sell(m_iSelFocusCharItemIndex) == true)
					MoveItemToShop(m_vkFocusCharItems[m_iSelFocusCharItemIndex]);
			}

			InitDlg();
		}
	}
	else
	if(strClickName == "AddItemBn")
	{
		if(m_iSelInventoryItemIndex != -1)
		{
			// Köpa något från affären
			if(m_eViewMode == shop)
			{
				if(Buy(m_iSelInventoryItemIndex) == false)
					return;
			}

			MoveItemFromInventory(m_vkInventoryItems[m_iSelInventoryItemIndex]);
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
				PrintItemInfo(&m_vkFocusCharItems[i], true);
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
		m_iSelInventoryItemIndex = -1;
		ZGuiCheckbox* pkButton = ((ZGuiCheckbox*)GetWnd((char*)strClickName.c_str()));

		for(int i=0; i<m_vkInventoryItems.size(); i++)
		{
			if(m_vkInventoryItems[i].m_pkMoveButton != pkButton)
			{
				m_vkInventoryItems[i].m_pkMoveButton->UncheckButton();
			}
			else
			{
				PrintItemInfo(&m_vkInventoryItems[i], false);
				m_iSelInventoryItemIndex = i;
				m_vkInventoryItems[i].m_pkMoveButton->CheckButton();
			}
		}

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
}

bool CItemTransactionDlg::InitDlg()
{
	PrintItemInfo(NULL, false);

	int x, y, w, h;
	unsigned int i, c;
	const int CELL_SIZE = 31;
	char szName[50];
	P_DMCharacter* pkCharProperty=NULL;
	Entity* pkHQ = GetDMObject(HQ);

	if(pkHQ == NULL)
		return false;
	
	DMContainer* pkStockroomOrShop;
	
	if(m_eViewMode == shop)
		pkStockroomOrShop = ((P_DMShop*)pkHQ->GetProperty("P_DMShop"))->m_pkItems;
	else
		pkStockroomOrShop = ((P_DMHQ*)pkHQ->GetProperty("P_DMHQ"))->m_pkStockroom;

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

	for(i=0; i<m_vkInventoryItems.size(); i++)
		m_vkInventoryItems[i].m_pkMoveButton->Hide();
	
	m_vkFocusCharItems.clear();
	m_vkInventoryItems.clear();

	int item_counter1=0;
	int item_counter2=0;

	DMContainer* akContainerObjects[] = 
	{
		pkCharProperty->m_pkBackPack,		// 0) Backpack
		pkCharProperty->m_pkBody,			// 1) Armor
		pkCharProperty->m_pkImplants,		// 2) Cybernectics
		pkCharProperty->m_pkBelt,			// 3) Quickitem
		pkCharProperty->m_pkHand,			// 4) Weapon

		pkStockroomOrShop								// 5) Stockroom / shop

	};

	for( c=0; c<6; c++)
	{
		DMContainer* pkContainer = akContainerObjects[c];
		
		Rect kRect = Rect(383,43,0,0);

		if(c == 5)
		{
			kRect = Rect(43,43,0,0);
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

				SetButtonIcon(pkButton, strIcon, true, true);

				//pkSkinUp->m_iBkTexID = GetTexID((char*)strIcon.c_str());
				//pkSkinDown->m_iBkTexID = GetTexID((char*)strIcon.c_str());
				//pkSkinDown->m_unBorderSize = 2;
				//pkSkinDown->m_afBorderColor[0] = 
				//pkSkinDown->m_afBorderColor[1] = 
				//pkSkinDown->m_afBorderColor[2] = 0;

				//pkButton->SetButtonCheckedSkin(pkSkinDown);
				//pkButton->SetButtonUncheckedSkin(pkSkinUp);

				pkButton->m_bUseAlhpaTest = false;
				pkButton->Show();

				// Move icon to top and set movearea to parent window
				static int s_okaZ = 10000;
				pkButton->m_iZValue = s_okaZ++;
				GetWnd("ItemInfoLabel")->m_iZValue = s_okaZ++;


				GetWnd("ItemAddTransactionNextPageBn")->m_iZValue = s_okaZ++;
				GetWnd("ItemAddTransactionPrevPageBn")->m_iZValue = s_okaZ++;
				GetWnd("ItemRemoveTransactionNextPageBn")->m_iZValue = s_okaZ++;
				GetWnd("ItemRemoveTransactionPrevPageBn")->m_iZValue = s_okaZ++;

				

				GetWnd("ItemTransactionWnd")->SortChilds(); 

				kMoveInfo.m_pkMoveButton = pkButton;
				kMoveInfo.m_kFromContainer.pkContainer = pkContainer;
				kMoveInfo.m_pMoveObject = pkContainer->GetItem(
					kItemList[i].m_iItemX, kItemList[i].m_iItemY);
				
				if(c != 5)
					m_vkFocusCharItems.push_back(kMoveInfo);
				else
					m_vkInventoryItems.push_back(kMoveInfo);
			}
		}
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

void CItemTransactionDlg::MoveItemFromInventory(ITEM_MOVE_INFO kItem)
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


void CItemTransactionDlg::MoveItemToShop(ITEM_MOVE_INFO kItem) // ie.Sell item
{
	Entity* pkHQ = GetDMObject(HQ);

	DMContainer* pkShop = 
		((P_DMShop*)pkHQ->GetProperty("P_DMShop"))->m_pkItems;

	if(kItem.m_kFromContainer.pkContainer->MoveItem(
		*kItem.m_pMoveObject, pkShop))
	{
		printf("Moving item id %i from container id % to container id %i\n",
			*kItem.m_pMoveObject, kItem.m_kFromContainer.pkContainer->GetOwnerID(), 
			pkShop->GetOwnerID());
	}
	else
	{
		printf("Failed to move item id %i from container id % to container id %i\n",
			*kItem.m_pMoveObject, kItem.m_kFromContainer.pkContainer->GetOwnerID(), 
			pkShop->GetOwnerID());
	}
}


bool CItemTransactionDlg::Buy(int iItemIndex)
{
	int iPrice=0;
	bool bCanAfford = false;

	P_DMGameInfo* pkGameInfo = (P_DMGameInfo*)
		GetDMObject(GAME_INFO)->GetProperty("P_DMGameInfo");

	P_DMShop* pkShop = (P_DMShop*) GetDMObject(HQ)->GetProperty("P_DMShop");

	int item = *m_vkInventoryItems[iItemIndex].m_pMoveObject;

	DMContainer* pkContainer = 
		m_vkInventoryItems[iItemIndex].m_kFromContainer.pkContainer;

	vector<ContainerInfo> kItemList;
	pkContainer->GetItemList(&kItemList);

	for(unsigned int i=0; i<kItemList.size(); i++)
	{
		int x = kItemList[i].m_iItemX;
		int y = kItemList[i].m_iItemY;

		int id = *pkContainer->GetItem(x, y); 
		if(id)
		{
			if(id == item)
			{
				iPrice = pkShop->GetSellPrice(id);
				if(iPrice < pkGameInfo->m_iMoney)
				{
					pkGameInfo->m_iMoney -= iPrice;
					bCanAfford = true;
				}
				break;
			}
		}
	}

	if(bCanAfford == false)
	{
		printf("Can't afford to buy that! (price: %i, you have: %i)\n",
			iPrice, pkGameInfo->m_iMoney);

		return false;
	}

	return true;
}

bool CItemTransactionDlg::Sell(int iItemIndex)
{
	// Fråga affären vad den vill ge för ett föremål av den typen.
	// Affären tittar bland alla sina föremål och kollar om det finns
	// ett liknande föremål, ser på värdet på föremålet som skickas in
	// och kommer fram till ett pris.

	P_DMShop* pkShop = (P_DMShop*) GetDMObject(HQ)->GetProperty("P_DMShop");

	int iPrice = pkShop->GetBuyPrice(*m_vkFocusCharItems[iItemIndex].m_pMoveObject);

	P_DMGameInfo* pkGameInfo = (P_DMGameInfo*)
		GetDMObject(GAME_INFO)->GetProperty("P_DMGameInfo");

	pkGameInfo->m_iMoney += iPrice;

	if(iPrice != -1)
	{
		printf("The shop buy the item! (price: %i, you have: %i)\n",
			iPrice, pkGameInfo->m_iMoney);

		return true;
	}
	else
	{
		printf("The shop will not buy that item! (id: %i)\n", iItemIndex);
	}

	return false;
}

void CItemTransactionDlg::PrintItemInfo(ITEM_MOVE_INFO* pkMoveItem, bool bCharacterItem)
{
	P_DMItem* pkItem = NULL;
	P_DMShop* pkShop = NULL;
	P_DMCharacter* pkChar = NULL;

	char szText[150];
	strcpy(szText, "");

	if(pkMoveItem != NULL)
	{
		if(pkMoveItem->m_pMoveObject)
		{
			int iAgentID = ((CHandleAgents*)GetGameDlg(HANDLE_AGENTS_DLG))->GetSelAgent();
			int iObjectID = *pkMoveItem->m_pMoveObject;

			pkItem = (P_DMItem*) GetObject(iObjectID)->GetProperty("P_DMItem");
			pkChar = (P_DMCharacter*) GetObject(iAgentID)->GetProperty("P_DMCharacter");
			pkShop = (P_DMShop*) GetDMObject(HQ)->GetProperty("P_DMShop");
			
			P_DMGameInfo* pkGameInfo = (P_DMGameInfo*)
				GetDMObject(GAME_INFO)->GetProperty("P_DMGameInfo");

			if(pkItem)
			{
				int iValue = -1;
				
				if(bCharacterItem == false) // klickat i stockroom/shop
				{
					if(m_eViewMode == shop)
					{
						iValue = pkShop->GetSellPrice(iObjectID);
						sprintf(szText, "Buy %s, price: %i (%i money left)", 
							pkItem->GetName().c_str(), iValue, pkGameInfo->m_iMoney );
					}
					else
					{
						sprintf(szText, "Move %s to character %s", 
							pkItem->GetName().c_str(),
							pkChar->GetStats()->m_strName.c_str());
					}
				}
				else // klickat i current character inventory
				{
					if(m_eViewMode == shop)
					{
						iValue = pkShop->GetBuyPrice(iObjectID);
						sprintf(szText, "Sell %s in %s, price: %i (%i money left)", 
							pkItem->GetName().c_str(), 
							GetGetContainerName(m_iActiveContainer), iValue,
							pkGameInfo->m_iMoney);
					}
					else
					{
						sprintf(szText, "Move %s from % s to stockroom", 
							pkItem->GetName().c_str(), 
							GetGetContainerName(m_iActiveContainer) );
					}
				}
			}
		}
	}

	SetText("ItemInfoLabel", szText);
}

char* CItemTransactionDlg::GetGetContainerName(int iContainer)
{
	switch(iContainer)
	{
	case BACKPACK: return "backpack";
	case ARMOR: return "armor";
	case CYBERNETICS: return "cybernetics";
	case QUICKITEM: return "belt";
	case WEAPON: return "weapon";
	default:
		return NULL;
	}
}