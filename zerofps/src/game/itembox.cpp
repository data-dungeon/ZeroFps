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
	m_pkMoveObject = NULL;
	m_pkContainer = NULL;
	m_pkTexMan = pkTexMan;
	//m_pkClickButton = NULL;
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
					m_pkMoveObject = m_pkContainer->GetItem(it->first.first, 
						it->first.second);
					m_pkMoveItem = it;
					break;
				}
			}
		}
		break;

	case ZGM_LBUTTONUP:

		if(m_pkMoveObject)
		{
			int mx = ((int*)pkParams)[0], my = ((int*)pkParams)[1];

			int x=-1,y=-1;
			map<slot_pos, ZGuiButton*>>::iterator it;
			for(it = m_kSlotsTable.begin(); it != m_kSlotsTable.end(); it++)
			{
				if(it->second->GetScreenRect().Inside(mx,my))
				{
					x = it->first.first;
					y = it->first.second;
					break;
				}
			}

			if( !(x==-1&&y==-1) )
			{
				int tx = m_pkMoveItem->first.first;
				int ty = m_pkMoveItem->first.second;
				int sx = tx+m_pkMoveItem->second->GetScreenRect().Width() / m_ciSlotSize;
				int sy = ty+m_pkMoveItem->second->GetScreenRect().Height() / m_ciSlotSize;

				for(int y=ty; y<sy; y++)
					for(int x=tx; x<sx; x++)
					{
						map<slot_pos,ZGuiButton*>::iterator res =
							m_kSlotsTable.find(slot_pos(x,y));

						if(res != m_kSlotsTable.end())
						{
							res->second->Show();
						}
					}

				cout << "removed object named "<< m_pkMoveObject->GetName() << endl;
				m_pkContainer->RemoveItem(m_pkMoveObject);

				m_pkMoveItem->second->Resize(m_ciSlotSize, m_ciSlotSize);
				m_pkMoveItem->second->GetSkin()->m_iBkTexID = -1;

				m_pkContainer->AddItem(m_pkMoveObject, x, y);
			}

			m_pkMoveObject = NULL;
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
		ZGuiButton* pkItemButton = NULL;

		GuiData kData = m_pkContainer->GetGuiData(i);  

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
