// inventorybox.cpp: implementation of the ItemBox class.
//
//////////////////////////////////////////////////////////////////////

#include "itembox.h"
#include "gui_resource_files/inventary_id.h"
#include "../common/container.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ItemBox::ItemBox(ZGui* pkGui, ZGuiWndProc oMainWndProc, TextureManager* pkTexMan) 
	: DlgBox(pkGui, oMainWndProc), m_ciSlotSize(48)
{
	m_pkContainer = NULL;
	m_pkTexMan = pkTexMan;
	m_pkClickButton = NULL;
	Create(0,0,0,0,oMainWndProc);
}

ItemBox::~ItemBox()
{

}

bool ItemBox::DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
					   int iNumberOfParams,void *pkParams ) 
{
	switch(uiMessage)
	{
	// Command Messages
	case ZGM_COMMAND:
		switch(((int*)pkParams)[0]) // control id
		{
		case InventCloseBN:
			OnClose(false);
			break;
		}
		break;

	case ZGM_LBUTTONDOWN:
		{
			// Check wich button that should move.
			map<slot_pos, ZGuiButton*>>::iterator it;
			for(it = m_kSlotsTable.begin(); it != m_kSlotsTable.end(); it++)
			{
				if(pkWnd == it->second)
				{
					m_pkClickButton = (ZGuiButton*) pkWnd;
					break;
				}
			}
		}
		break;

	case ZGM_LBUTTONUP:

		if(m_pkClickButton != NULL)
		{
			// Check wich button that should be removed.
			Object* pkMoveObject = NULL;
			map<slot_pos, ZGuiButton*>>::iterator it;
			for(it = m_kSlotsTable.begin(); it != m_kSlotsTable.end(); it++)
			{
				if(pkWnd == it->second)
				{
					cout << "put" << endl;

					if(pkMoveObject)
					{
						cout << "removed object named "<< pkMoveObject->GetName() << endl;

						m_pkContainer->RemoveItem(pkMoveObject);
						break;
						/*m_pkContainer->AddItem(pkMoveObject, it->first.first, 
							it->first.second);*/
					}
				}
				
				if(m_pkClickButton == it->second)
				{
					pkMoveObject = m_pkContainer->GetItem(it->first.first, 
						it->first.second);

					if(pkMoveObject)
						cout << pkMoveObject->GetName() << endl;
				}
			}

			m_pkClickButton = NULL;

			//m_pkClickButton->Hide();
			//m_kSlotsTable.find(
			//m_pkContainer->
		}
		break;
	}

	return true;
}

bool ItemBox::Create(int x, int y, int w, int h, ZGuiWndProc pkWndProc)
{
	if(m_pkGui->LoadDialog("../src/game/gui_resource_files/inventary_rc.txt", 
		"InventoryWnd", pkWndProc) == false)
		return false;

	m_pkDlgBox = m_pkGuiMan->Wnd("InventoryWnd");

	if(m_pkDlgBox == NULL)
		return false;

	list<ZGuiWnd*> pkChilds;
	m_pkDlgBox->GetChildrens(pkChilds);

	// InvetSlotBN
	int top_x = 10000, top_y = 10000;

	list<ZGuiWnd*>::iterator it;
	for(it = pkChilds.begin(); it != pkChilds.end(); it++)
	{
		// Skip if "InvetSlotBN" does not appear in name.
		if(strstr((*it)->GetName(), "InvetSlotBN") == NULL)
			continue;

		Rect rc = (*it)->GetWndRect();

		if(rc.Left < top_x)
			top_x = rc.Left;

		if(rc.Top < top_y)
			top_y = rc.Top;

		// insert all buttons in a dictionary.
		m_kSlotsTable.insert(map<slot_pos,ZGuiButton*>::value_type( 
			slot_pos(rc.Left, rc.Top), (ZGuiButton*)(*it)));

		//printf("(%i, %i), %i\n", rc.Left, rc.Top, (ZGuiButton*)(*it));
	}

	map<slot_pos, ZGuiButton*>>::iterator it2;
	for(it2 = m_kSlotsTable.begin(); it2 != m_kSlotsTable.end(); it2++)
	{
		int cell_x = it2->first.first / m_ciSlotSize;
		int cell_y = it2->first.second / m_ciSlotSize;
		ZGuiButton* pkButton = it2->second;

		m_kSlotsTable.erase(it2);

		m_kSlotsTable.insert(map<slot_pos,ZGuiButton*>::value_type( 
			slot_pos(cell_x, cell_y), pkButton));
	}

	m_pkGui->ShowMainWindow(m_pkDlgBox, false);
	return true;
}

bool ItemBox::OnOpen(int x, int y)
{
	m_pkDlgBox->SetPos(x,y,true,true);
	m_pkGui->ShowMainWindow(m_pkDlgBox, true);
	return true;
}

bool ItemBox::OnClose(bool bSave)
{
	m_pkGui->ShowMainWindow(m_pkDlgBox, false);
	return true;
}

void ItemBox::Update()
{
	if(m_pkContainer == NULL)
	{
		printf("No container!\n");
		return;
	}

	int sx, sy;
	m_pkContainer->GetSize(sx,sy);

	int iNumItems = m_pkContainer->GetNrOfItems();
	char szTexpath[512];

	// Loopa igenom alla items.
	for(int i=0; i<iNumItems; i++)
	{
		GuiData kData = m_pkContainer->GetGuiData(i);

		ZGuiButton* pkItemButton = NULL;

		// Dölj alla knappar som den nya knappen kommer att täcka
		// och registrera den nya knappen.
		for(int y=0; y<kData.iSizeY; y++)
			for(int x=0; x<kData.iSizeX; x++)
			{
				map<slot_pos,ZGuiButton*>::iterator res =
					m_kSlotsTable.find( slot_pos(kData.iPosX+x, kData.iPosY+y) );

				if(res != m_kSlotsTable.end())
				{
					if(!(y==0&&x==0))
						res->second->Hide();
					else
						pkItemButton = res->second;
				}
			}	

		// Ändra storlek och byt textur på knappen.
		if(pkItemButton)
		{
			pkItemButton->Resize(kData.iSizeX*m_ciSlotSize, kData.iSizeY*m_ciSlotSize);
			
			ZGuiSkin* pkSkin = pkItemButton->GetSkin();
			strcpy(szTexpath, "file:../data/textures/");
			strcat(szTexpath, kData.sTextureName.c_str());
			pkSkin->m_iBkTexID = m_pkTexMan->Load(szTexpath, 0);
		}
	}
}

void ItemBox::SetContainer(Container* pkContainer)
{
	m_pkContainer = pkContainer;
}
