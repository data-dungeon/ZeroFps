// inventorybox.cpp: implementation of the ItemBox class.
//
//////////////////////////////////////////////////////////////////////

#include "itembox.h"
#include "../common/container.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int ItemBox::s_iButtonSlotIDCounter = 10000;
int ItemBox::s_iStaticGridIDCounter = 100000;
ZGuiSkin* ItemBox::s_kStaticGridSkinNormal = NULL;
ZGuiSkin* ItemBox::s_kStaticGridSkinUnused = NULL;

ItemBox::ItemBox(ZGui* pkGui, ZGuiWndProc oMainWndProc, TextureManager* pkTexMan,
				 int iCols, int iRows, int iSlotSize, int iTopX, int iTopY) 
	:	DlgBox(pkGui, oMainWndProc), m_ciSlotSize(iSlotSize), m_ciTopX(iTopX), 
		m_ciTopY(iTopY), m_ciCols(iCols), m_ciRows(iRows)
{
	m_iGridIDCounterStart = s_iStaticGridIDCounter;
	m_pkMoveItem = NULL;
	m_pkContainer = NULL;
	m_pkTexMan = pkTexMan;
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
/*	case ZGM_COMMAND:
		switch(((int*)pkParams)[0]) // control id
		{
		case InventCloseBN:
			OnClose(false);
			break;
		}
		break;*/

	case ZGM_LBUTTONDOWN:
		unsigned int i;
		for(i=0; i<m_akSlots.size(); i++)
		{
			if(pkWnd == m_akSlots[i].first)
			{
				m_pkMoveItem = &m_akSlots[i];
				break;
			}
		}
		break;

	case ZGM_LBUTTONUP:
		{
			if(m_pkMoveItem == NULL)
				return false;

			bool bObjectMoved = false;

			slot_pos kItemCell = GetSlot(((int*)pkParams)[0], ((int*)pkParams)[1]);
			if(kItemCell.first != -1) // was the button released inside the grid?
			{
				Object* pkObject = m_pkContainer->GetItem(
					m_pkMoveItem->second.first, m_pkMoveItem->second.second);

				if(pkObject)
				{
					int prev_x = m_pkMoveItem->second.first;
					int prev_y = m_pkMoveItem->second.second;

					// Remove button and item.
					RemoveSlot(prev_x, prev_y);
					m_pkContainer->RemoveItem(pkObject);
					
					if( m_pkContainer->AddItem(pkObject, kItemCell.first, 
						kItemCell.second) == false)
					{
						if( m_pkContainer->AddItem(pkObject, prev_x, prev_y) == false)
							printf("Item box: Failed to move object in container\n");
						else
							bObjectMoved = true;
					}
					else
						bObjectMoved = true;
				} 
			}
			
			// If move have failed, reset button pos.
			if(bObjectMoved == false)
			{
				int px = m_ciTopX + m_pkMoveItem->second.first * m_ciSlotSize;
				int py = m_ciTopY + m_pkMoveItem->second.second * m_ciSlotSize;
				m_pkMoveItem->first->SetPos(px, py, false, true);
			}

			m_pkMoveItem = NULL;
		}
		break;

	case ZGM_MOUSEMOVE:
		if(m_pkMoveItem)
		{
			int mx = ((int*)pkParams)[1]; 
			int my = ((int*)pkParams)[2]; 
			m_pkMoveItem->first->SetPos(mx, my,true,true);
		}
		break;
	}

	return true;
}

bool ItemBox::Create(int x, int y, char* szResourceFile, char* szDlgName)
{
	if(m_pkGui->LoadDialog(szResourceFile, szDlgName, m_oMainWndProc) == false)
		return false;

	m_pkDlgBox = m_pkGuiMan->Wnd(szDlgName);

	if(m_pkDlgBox == NULL)
		return false;

	if(!(x==-1 && y==-1))
		m_pkDlgBox->SetPos(x,y,true,true);

	CreateStaticGrid();

	m_pkGui->ShowMainWindow(m_pkDlgBox, false);
	return true;
}

bool ItemBox::OnOpen(int x, int y)
{
	int sx,sy;
	m_pkContainer->GetSize(sx,sy);
	PaintStaticSlots(sx,sy);

	if(!(x==-1 && y==-1))
		m_pkDlgBox->SetPos(x,y,true,true);

	m_pkGui->ShowMainWindow(m_pkDlgBox, true);

	CenterCursor();

	return true;
}

bool ItemBox::OnClose(bool bSave)
{
	m_pkGui->ShowMainWindow(m_pkDlgBox, false);
	m_pkGui->ShowCursor(false);
	return true;
}

void ItemBox::Update()
{
	if(m_pkContainer == NULL)
	{
		printf("No container!\n");
		return;
	}

	unsigned int iNumItems = m_pkContainer->GetNrOfItems();

	// Kolla om några nya knappar behövs läggas till.
	if(iNumItems > m_akSlots.size())
	{
		for(unsigned int i=0; i<iNumItems; i++)
		{
			GuiData kData = m_pkContainer->GetGuiData(i);  
			if(!ButtonSlotExist(kData.iPosX, kData.iPosY))
				AddSlot(&kData);
		}
	}
	else
	// Kolla om några nya knappar behövs ta borts.
	if(m_akSlots.size() > iNumItems)
	{
		vector<slot_pos> kRemoveVector;
		vector<slot>::iterator it;
		vector<slot_pos>::iterator it2;

		for(it = m_akSlots.begin(); it != m_akSlots.end(); it++)
			if(m_pkContainer->GetItem(it->second.first,it->second.second)==NULL)
				kRemoveVector.push_back(it->second);

		for(it2 = kRemoveVector.begin(); it2 != kRemoveVector.end(); it2++)
			RemoveSlot(it2->first, it2->second);
	}
}

void ItemBox::SetContainer(Container* pkContainer)
{
	m_pkContainer = pkContainer;
}

void ItemBox::AddSlot(GuiData* pkData)
{
	Rect rc(pkData->iPosX*m_ciSlotSize, pkData->iPosY*m_ciSlotSize,
		pkData->iPosX*m_ciSlotSize + pkData->iSizeX*m_ciSlotSize, 
		pkData->iPosY*m_ciSlotSize + pkData->iSizeY*m_ciSlotSize);

	rc = rc.Move(m_ciTopX, m_ciTopY);

	ZGuiButton* pkButton = new ZGuiButton(rc, m_pkDlgBox, true, 
		s_iButtonSlotIDCounter++);

	ZGuiSkin* pkSkin = new ZGuiSkin();
	char szTexpath[512];
	strcpy(szTexpath, "file:../data/textures/");
	strcat(szTexpath, pkData->sTextureName.c_str());
	pkSkin->m_iBkTexID = m_pkTexMan->Load(szTexpath, 0);
	pkButton->SetButtonUpSkin(pkSkin);
	pkButton->SetButtonDownSkin(pkSkin);
	pkButton->SetButtonHighLightSkin(pkSkin);
	
	char* name = GetSlotName(s_iButtonSlotIDCounter);
	m_pkGui->RegisterWindow(pkButton, name);
	delete[] name;

	// Must sort all window again so that the the static grid picture
	// doesn´t cover the new button.
	pkButton->SetZValue(s_iButtonSlotIDCounter);
	m_pkDlgBox->SortChilds();

	// Add the slot to the vector
	m_akSlots.push_back(slot(pkButton, slot_pos(pkData->iPosX, pkData->iPosY))); 
}

void ItemBox::RemoveSlot(int grid_x, int grid_y)
{
	for(vector<slot>::iterator it = m_akSlots.begin();
		it != m_akSlots.end(); it++)
		{
			if( it->second.first == grid_x && 
				it->second.second == grid_y)
			{
				m_pkGui->UnregisterWindow(it->first);
				m_akSlots.erase(it);
				break;
			}
		}
}

char* ItemBox::GetSlotName(int id)
{
	char* szApa = new char[50];
	sprintf(szApa, "ItemBoxItemSlot%iBN", id);
	return szApa;
}

bool ItemBox::ButtonSlotExist(int grid_x, int grid_y)
{
	for(vector<slot>::iterator it = m_akSlots.begin();
		it != m_akSlots.end(); it++)
		{
			if( it->second.first == grid_x && 
				it->second.second == grid_y)
				return true;
		}

	return false;
}

/// x och y är i skärm koordinater
pair<int,int> ItemBox::GetSlot(int x, int y)
{
	Rect test = m_pkDlgBox->GetScreenRect();
	test = test.Move(m_ciTopX,m_ciTopY);
	test.Right = test.Left + (m_ciCols * m_ciSlotSize);
	test.Bottom = test.Top + (m_ciRows * m_ciSlotSize);
	if(!test.Inside(x,y))
		return slot_pos(-1,-1);

	int wnd_x = m_ciTopX + x - m_pkDlgBox->GetScreenRect().Left;
	int wnd_y = m_ciTopY + y - m_pkDlgBox->GetScreenRect().Top;

	int col = wnd_x / m_ciSlotSize;
	int row = wnd_y / m_ciSlotSize;

	return slot_pos(col,row);
}

void ItemBox::PaintStaticSlots(int container_size_x, int container_size_y)
{
	char szSearchName[50];
	for(int y=0, counter=m_iGridIDCounterStart; y<m_ciRows; y++)
		for(int x=0; x<m_ciCols; x++, counter++)
		{
			sprintf(szSearchName, "ItemboxSlotGridST%i", counter);
			ZGuiWnd* pkWnd = m_pkGuiMan->Wnd(string(szSearchName));
			if(pkWnd != NULL )
			{
				slot_pos cell = GetSlot(pkWnd->GetScreenRect().Left, 
					pkWnd->GetScreenRect().Top);

				ZGuiSkin* pkSkin = pkWnd->GetSkin();
				
				if(cell.first>=container_size_x || cell.second>=container_size_y)
					pkSkin = s_kStaticGridSkinUnused;
				else
					pkSkin = s_kStaticGridSkinNormal;

				pkWnd->SetSkin(pkSkin);
			}
		}
}

void ItemBox::CreateStaticGrid()
{
	if(s_kStaticGridSkinNormal == NULL)
	{
		const float fStaticColor[] = {
			(1.0f/255)*132,(1.0f/255)*130,(1.0f/255)*132 };

		s_kStaticGridSkinNormal = new ZGuiSkin();
		s_kStaticGridSkinUnused = new ZGuiSkin();

		char szTexpath[512];
		strcpy(szTexpath, "file:../data/textures/itemslots.bmp");
		s_kStaticGridSkinNormal->m_iBkTexID = m_pkTexMan->Load(szTexpath, 0);
		s_kStaticGridSkinUnused->m_iBkTexID = m_pkTexMan->Load(szTexpath, 0);

		memcpy(s_kStaticGridSkinUnused->m_afBkColor, fStaticColor, 
			sizeof(float) * 3);
	}

	ZGuiLabel* pkLabel;
	char szName[50];
	Rect rc;

	for(int y=0; y<m_ciRows; y++)
		for(int x=0; x<m_ciCols; x++)
		{
			rc = Rect(x*m_ciSlotSize, y*m_ciSlotSize,
				x*m_ciSlotSize+m_ciSlotSize, y*m_ciSlotSize+m_ciSlotSize);
			rc = rc.Move(m_ciTopX, m_ciTopY);

			pkLabel = new ZGuiLabel(rc, m_pkDlgBox, true, s_iStaticGridIDCounter);
			pkLabel->SetSkin(s_kStaticGridSkinNormal);

			sprintf(szName, "ItemboxSlotGridST%i", s_iStaticGridIDCounter);
			m_pkGui->RegisterWindow(pkLabel, szName);

			s_iStaticGridIDCounter++;
		}
}













