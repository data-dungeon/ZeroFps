// inventorydlg.cpp: implementation of the InventoryDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "inventorydlg.h"
//#include "../zerofpsv2/basic/zfsystem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int InventoryDlg::s_iSlotZCounter = 2121212;

InventoryDlg::InventoryDlg(ZGuiWnd* pkDlgWnd)// : ZFSubSystem("InventoryDlg") 
{
	m_pkGui = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));
	m_pkResMan = static_cast<ZGuiResourceManager*>(g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));
	m_pkTexMan = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	m_pkAudioSys = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
	m_kClickOffset = Point(0,0);
	m_iCurrentScrollPos = 0;
	m_pkDlgWnd = pkDlgWnd;
	m_pkSelectedSlot = NULL;

	m_pkSelectionLabel = new ZGuiLabel(Rect(264,16,264+32,16+32),m_pkDlgWnd,true,23443);
	m_pkSelectionLabel->SetSkin(new ZGuiSkin(-1,-1,-1,-1,  -1,-1,-1,-1, 0,0,0, 255,0,0, 2, 0, 1));
	m_pkSelectionLabel->Hide();
	m_iCurrentContainer = MAIN_CONTAINER;

	m_kContainerStack.push(m_iCurrentContainer); 

	static_cast<ZGuiScrollbar*>(m_pkResMan->Wnd(
		"SlotListScrollbar"))->SetScrollInfo(0,100,0.15f,0);
}

InventoryDlg::~InventoryDlg()
{
	
}

bool InventoryDlg::AddItem(const char *szPic, const char *szPicA, ItemStats* pkItemStats)
{
	Point sqr;

	if(!GetFreeSlotPos(sqr, MAIN_CONTAINER))
	{
		printf("Failed to find free slot!\n");
		return false;
	}
			
	AddSlot(szPic, szPicA, sqr, CONTAINTER_SLOTS, pkItemStats, MAIN_CONTAINER);

	ScrollItems(m_iCurrentScrollPos+1);
	ScrollItems(m_iCurrentScrollPos-1);
	
	return true;
}

bool InventoryDlg::AddItems(vector<pair<pair<string, string>,ItemStats*> >&vkItems)
{
	vector<itItem>::iterator it = vkItems.begin(); 

	for( ; it != vkItems.end(); it++)
	{
		Point sqr;

		if(!GetFreeSlotPos(sqr, MAIN_CONTAINER))
		{
			printf("Failed to find free slot!\n");
			return false;
		}

		AddSlot((*it).first.first.c_str(), (*it).first.second.c_str(), 
			sqr, CONTAINTER_SLOTS, (*it).second, MAIN_CONTAINER);
	}

	ScrollItems(m_iCurrentScrollPos+1);
	ScrollItems(m_iCurrentScrollPos-1);
	
	return true;
}


void InventoryDlg::OnClick(int x, int y, bool bMouseDown, bool bLeftButton)
{
	Point sqr = MousePosToSqr(x,y);

	Slot* pkSlot = FindSlot(x,y);

	char szPic[50], szPicA[50];

	//
	// Left click
	//
	if(bLeftButton)
	{
		if(pkSlot)
		{
			// ta tag i ett föremål?
			if(bMouseDown)
			{
				strcpy(szPic, pkSlot->m_szPic[0]);
				strcpy(szPicA, pkSlot->m_szPic[1]);
				ItemStats* pkStats = pkSlot->m_pkItemStats;
				SlotType prev_type = pkSlot->m_eType;

				if(RemoveSlot(pkSlot))
				{
					if(prev_type == SPECIAL_SLOTS)
					{
						sqr.x = -x;
						sqr.y = -y;					
					}

					AddSlot(szPic, szPicA, sqr, UNDER_MOUSE, pkStats, m_iCurrentContainer);

					m_pkAudioSys->StartSound("/data/sound/open_window.wav",
						m_pkAudioSys->GetListnerPos(),m_pkAudioSys->GetListnerDir(),false);

					m_kClickOffset.x = 32;//x-pkSlot->m_pkLabel->GetScreenRect().Left; //(264+sqr.x*48); 
					m_kClickOffset.y = 32;//y-pkSlot->m_pkLabel->GetScreenRect().Top;  //(16+sqr.y*48);

					m_pkGui->SetCaptureToWnd( m_pkDlgWnd ); // set capture
				}
			}
		}
		else // släppa tillbaks ett föremål
		if(!m_kDragSlots.empty() && bMouseDown == false)
		{
			if(Rect(448,200,448+48,200+48).Inside(x,y))
			{
				DropItems();
				m_pkGui->KillWndCapture(); // remove capture
				return;
			}

			Slot kSlotToAdd = m_kDragSlots.back();

			// det går inte att placera en container i sig själv
			if(kSlotToAdd.m_iContainerID == m_iCurrentContainer)
				sqr = Point(-1,-1);

			if(sqr == Point(-1,-1))
			{
				sqr = kSlotToAdd.m_kSqr; // välj den gamla positionen (ruta eller pixel)
										 // om slot:en inte fanns.
			}
		
			strcpy(szPic, kSlotToAdd.m_szPic[0]);
			strcpy(szPicA, kSlotToAdd.m_szPic[1]);
			ItemStats* pkStats = kSlotToAdd.m_pkItemStats;
			
			if(RemoveSlot(&kSlotToAdd))
			{
				SlotType type = CONTAINTER_SLOTS;
				Point test = MousePosToSpecialSqrPos(x,y,pkStats->GetEquipmentCategory());
				if( test != Point(-1,-1) )
				{
					type = SPECIAL_SLOTS;
			
					if(!SlotExist(test.x,test.y))
						sqr = test;
				}
				else
				{
					if(sqr.x > 4)
					{
						type = SPECIAL_SLOTS;
					}
				}

				int sx, sy;
				if(sqr.x < 5) // container item
				{
					sx = 264 + sqr.x * 48;
					sy = 16 + sqr.y * 48;
				}
				else
				{
					sx = sqr.x;
					sy = sqr.y;
				}

				if(SlotExist(sx, sy))
				{
					GetFreeSlotPos(sqr, m_iCurrentContainer);
					type = CONTAINTER_SLOTS;					
				}
		
				AddSlot(szPic, szPicA, sqr, type, pkStats, m_iCurrentContainer);

				if(m_kDragSlots.empty())
					m_pkGui->KillWndCapture(); // remove capture

				m_pkAudioSys->StartSound("/data/sound/close_window.wav",
					m_pkAudioSys->GetListnerPos(),m_pkAudioSys->GetListnerDir(),false);			
			}
		}
	}
	//
	// Right click
	//
	else
	{
		if(pkSlot && bMouseDown)
		{
			Rect rcClickLabel = pkSlot->m_pkLabel->GetScreenRect();
			m_pkSelectionLabel->SetPos(rcClickLabel.Left, rcClickLabel.Top, true, true);  
			m_pkSelectionLabel->Show();
			m_pkSelectedSlot = pkSlot;
		}
	}
}

void InventoryDlg::OnDClick(int x, int y, bool bLeftButton)
{
	if(bLeftButton == false)
	{
		Slot* pkSlot = FindSlot(x,y);

		if(pkSlot)
		{
			int new_container = pkSlot->m_pkItemStats->GetContainerID();

			if(new_container != -1)
			{
				SwitchContainer(new_container);

				m_pkSelectionLabel->Hide();
				m_pkSelectedSlot = NULL;

				m_kContainerStack.push(new_container);

				m_pkAudioSys->StartSound("/data/sound/WoodenPanelClose.wav",
					m_pkAudioSys->GetListnerPos(),m_pkAudioSys->GetListnerDir(),false);					
			}
		}
	}
}

InventoryDlg::Slot* InventoryDlg::FindSlot(int mouse_x, int mouse_y)
{
	// Leta efter container slots

	Point sqr = MousePosToSqr(mouse_x, mouse_y);

	unsigned int i;

	for(i=0; i<m_kItemSlots.size(); i++)
	{
		if(m_kItemSlots[i].m_kSqr == sqr)
		{
			if(	m_kItemSlots[i].m_eType != SPECIAL_SLOTS)
			{
				if(m_kItemSlots[i].m_iContainer != m_iCurrentContainer)
					continue;
			}

			if(m_kItemSlots[i].m_pkLabel->IsVisible())
				return &m_kItemSlots[i];
		}
	}

	// Leta efter special slots

	for(i=0; i<m_kItemSlots.size(); i++)
	{
		if(m_kItemSlots[i].m_pkLabel->IsVisible())
		{
			if(m_kItemSlots[i].m_pkLabel->GetScreenRect().Inside(mouse_x, mouse_y))
			{
				if(	m_kItemSlots[i].m_eType != SPECIAL_SLOTS)
				{
					if(m_kItemSlots[i].m_iContainer != m_iCurrentContainer)
						continue;
				}
					
				return &m_kItemSlots[i];
			}
		}
	}

	return NULL;
}

Point InventoryDlg::MousePosToSqr(int x, int y)
{
	int top_x, top_y;

	top_x = 264;
	top_y = 16;

	int sqr_x = (x - top_x) / 48;
	int sqr_y = (y - top_y) / 48;

	if(x < top_x || y < top_y || sqr_x > 4 || sqr_y > 3)
		return Point(-1,-1);

	return Point(sqr_x, sqr_y);
}

Point InventoryDlg::MousePosToSpecialSqrPos(int x, int y, EquipmentCategory eCategory)
{
	switch(eCategory) 
	{
	case Armor:
		if(Rect(16,16,16+32,16+32).Inside(x,y)) return Point(16,16);
		break;
	case Weapon:
		if(Rect(208,64,208+32,64+32).Inside(x,y)) return Point(208,64);
		if(Rect(16,64,208+32,64+32).Inside(x,y)) return Point(16,64);
		break;
	case Cloak:
		if(Rect(64,16,64+32,16+32).Inside(x,y)) return Point(64,16);
		break;
	case Helm:
		if(Rect(112,16,112+32,16+32).Inside(x,y)) return Point(112,16);
		break;
	case Amulett:
		if(Rect(160,16,160+32,16+32).Inside(x,y)) return Point(160,16);
		break;
	case Shield:
		if(Rect(208,16,208+32,16+32).Inside(x,y)) return Point(208,16);
		break;
	case Bracer:
		if(Rect(16,112,16+32,112+32).Inside(x,y)) return Point(16,112);
		break;
	case Glove:
		if(Rect(208,112,208+32,112+32).Inside(x,y)) return Point(208,112);
		break;
	case Ring:
		if(Rect(16,160,16+32,160+32).Inside(x,y)) return Point(16,160);
		if(Rect(208,160,208+32,160+32).Inside(x,y)) return Point(208,160);
		break;
	case Boots:
		if(Rect(16,208,16+32,208+32).Inside(x,y)) return Point(16,208);
		break;
	case Belt:
		if(Rect(208,208,208+32,208+32).Inside(x,y)) return Point(208,208);
		break;
	}
	
	return Point(-1, -1);
}

bool InventoryDlg::RemoveSlot(Slot* pkSlot)
{
	Point sqr = pkSlot->m_kSqr;
	
	bool bDragItem = pkSlot->m_eType == UNDER_MOUSE;

	if(bDragItem == false)
	{
		for(itSlot it = m_kItemSlots.begin(); it != m_kItemSlots.end(); it++)
		{
			Slot s = (*it);

			if( s.m_iContainer == m_iCurrentContainer || s.m_eType == SPECIAL_SLOTS)
			{
				if(s.m_kSqr == sqr)
				{
					if(m_pkSelectedSlot)
					{
						if(s.m_pkLabel->GetScreenRect() == 
							m_pkSelectionLabel->GetScreenRect() )
						{
							m_pkSelectionLabel->Hide();
							m_pkSelectedSlot = NULL;
						}
					}

					m_pkGui->UnregisterWindow(s.m_pkLabel);
					m_kItemSlots.erase(it);
					return true;
				}
			}
		}
	}
	else
	{
		for(itSlot it = m_kDragSlots.begin(); it != m_kDragSlots.end(); it++)
		{
			if((*it).m_kSqr == sqr)
			{
				m_pkGui->UnregisterWindow((*it).m_pkLabel);
				m_kDragSlots.erase(it);
				return true;
			}		
		}
	}

	return false;
}

void InventoryDlg::RegisterSlot(InventoryDlg::Slot slot)
{
	int id = GenerateID(slot.m_kSqr);

	char szSlotName[50];
	sprintf(szSlotName, "ItemSlotLabel%i", id);
	m_pkGui->RegisterWindow(slot.m_pkLabel, szSlotName);
}

int InventoryDlg::GenerateID(Point sqr)
{
	static int counter = 1000; counter++;
	return counter;
}

void InventoryDlg::OnMouseMove(int x, int y, bool bMouseDown)
{
	for(itSlot it = m_kDragSlots.begin(); it != m_kDragSlots.end(); it++)
	{
		ZGuiLabel* pkLabel = (*it).m_pkLabel;
		pkLabel->SetPos(x-m_kClickOffset.x,y-m_kClickOffset.y,true,true);
	}
}

void InventoryDlg::OnCommand(int iID)
{
	string strScrollbarName = GetWndByID(iID);

	if(strScrollbarName == "BackPackBackButton")
	{
		if(m_kContainerStack.size() > 1)
		{
			m_kContainerStack.pop();

			int new_container = m_kContainerStack.top();

			SwitchContainer(new_container);		

			m_pkAudioSys->StartSound("/data/sound/PickUpRusty.wav",
				m_pkAudioSys->GetListnerPos(),m_pkAudioSys->GetListnerDir(),false);
		}
	}
}

void InventoryDlg::OnScroll(int iID, int iPos)
{
	string strScrollbarName = GetWndByID(iID);

	if(strScrollbarName == "SlotListScrollbar")
	{
		ScrollItems(iPos);
	}
}

void InventoryDlg::ScrollItems(int iPos)
{
	int offset = m_iCurrentScrollPos-iPos;

	m_iCurrentScrollPos = iPos;

	if(offset == 0)
		return;

	for(itSlot it = m_kItemSlots.begin(); it != m_kItemSlots.end(); it++)
	{
		Slot slot = (*it);

		if(slot.m_iContainer == m_iCurrentContainer)
		{
			if(slot.m_eType != SPECIAL_SLOTS)
			{
				Rect rc = slot.m_pkLabel->GetScreenRect(); 

				int y = rc.Top+offset*48;

				slot.m_pkLabel->SetPos(rc.Left, y, true, true);

				(*it).m_kSqr.y = (y-16) / 48;

				if(y < 0 || y > 192)
					slot.m_pkLabel->Hide();
				else
					slot.m_pkLabel->Show();
			}
		}
	}	

	if(m_pkSelectedSlot && m_pkSelectedSlot->m_eType != SPECIAL_SLOTS)
	{
		if(m_pkSelectedSlot->m_iContainer == m_iCurrentContainer)
		{
			Rect rc = m_pkSelectionLabel->GetScreenRect(); 
			int y = rc.Top+offset*48;
			m_pkSelectionLabel->SetPos(rc.Left, y, true, true);
			if(y < 0 || y > 192) m_pkSelectionLabel->Hide();
			else m_pkSelectionLabel->Show();
		}
	}
}

bool InventoryDlg::GetFreeSlotPos(Point& refSqr, int iContainer)
{
	for(int y=0; y<100; y++)
		for(int x=0; x<5; x++)
		{
			Point t = Point(x,y);
			bool bTaken = false;

			itSlot it;
			for( it = m_kItemSlots.begin(); it != m_kItemSlots.end(); it++)
				if((*it).m_kSqr == t && (*it).m_iContainer == iContainer)
				{
					bTaken = true;
					break;
				}

			if(bTaken == false)
			{
				refSqr = t;
				return true;
			}
		}

	return false;
}

void InventoryDlg::AddSlot(const char *szPic, const char *szPicA, Point sqr, 
						   SlotType eType, ItemStats* pkItemStats, int iContainer)
{
	int sx, sy;

	if(eType == SPECIAL_SLOTS)
	{
		sx = sqr.x;
		sy = sqr.y;
	}
	else
	{
		sx = 264+sqr.x*48;
		sy = 16+sqr.y*48;

		if((sqr.x < -1 && sqr.y < -1))
		{
			sqr = MousePosToSpecialSqrPos(-sqr.x, -sqr.y, pkItemStats->GetEquipmentCategory()); 
			sx = sqr.x; sy = sqr.y;
		}
	}

	int size = (eType == UNDER_MOUSE) ? 64 : 32;

	int id = GenerateID(sqr);

	ZGuiLabel* pkNewLabel = new ZGuiLabel(Rect(sx,sy,sx+size,sy+size), 
		m_pkResMan->Wnd("BackPackWnd"), iContainer == m_iCurrentContainer, id);

	string strPath = "data/textures/gui/items/";

	string strFileName = strPath; strFileName += szPic;
	int tex = m_pkTexMan->Load(strFileName.c_str(), 0);

	strFileName = strPath; strFileName += szPicA;
	int texa = m_pkTexMan->Load(strFileName.c_str(), 0);

	ZGuiSkin* pkSkin = new ZGuiSkin( tex, texa, false);

	pkNewLabel->SetSkin(pkSkin); 

	Slot kNewSlot;
	
	strcpy(kNewSlot.m_szPic[0], szPic);
	strcpy(kNewSlot.m_szPic[1], szPicA);

	kNewSlot.m_kSqr = sqr;
	kNewSlot.m_pkLabel = pkNewLabel;

	if(eType == UNDER_MOUSE)
	{
		kNewSlot.m_pkLabel->SetZValue(s_iSlotZCounter++);
		kNewSlot.m_pkLabel->GetParent()->SortChilds();
	}

	kNewSlot.m_eType = eType;
	kNewSlot.m_pkItemStats = pkItemStats;
	kNewSlot.m_iContainer = iContainer;
	kNewSlot.m_iContainerID = kNewSlot.m_pkItemStats->GetContainerID();

	switch(eType)
	{
	case UNDER_MOUSE:
		m_kDragSlots.push_back(kNewSlot);
		break;
	default:
		if(eType == CONTAINTER_SLOTS)
		{
			kNewSlot.m_kRealSqr = sqr;
			kNewSlot.m_kRealSqr.y += m_iCurrentScrollPos;
		}

		m_kItemSlots.push_back(kNewSlot);
		break;
	}

	RegisterSlot(kNewSlot);
}

bool InventoryDlg::SlotExist(int sx, int sy)
{
	itSlot it;
	for( it = m_kItemSlots.begin(); it != m_kItemSlots.end(); it++)
	{
		Slot slot = (*it);
		if(slot.m_pkLabel->GetScreenRect().Inside(sx,sy))
		{
			if(slot.m_iContainer == m_iCurrentContainer)
			{
				if(slot.m_pkLabel->IsVisible())
					return true;
			}
		}
	}
		
	return false;
}


void InventoryDlg::SwitchContainer(int iNewContainer)
{
	static_cast<ZGuiScrollbar*>(m_pkResMan->Wnd(
		"SlotListScrollbar"))->SetScrollInfo(0,100,0.15f,0);
	m_iCurrentScrollPos = 0;

	m_iCurrentContainer = iNewContainer;

	itSlot it;
	for( it = m_kItemSlots.begin(); it != m_kItemSlots.end(); it++)
	{
		Slot slot = (*it);

		if(slot.m_eType == CONTAINTER_SLOTS)
		{
			// Placer föremålen på rätt ställen i den nya gridden.
			if(slot.m_iContainer == iNewContainer)
			{
				int nx = 264 + slot.m_kRealSqr.x * 48;
				int ny = 16  + slot.m_kRealSqr.y * 48;

				slot.m_pkLabel->SetPos(nx, ny, true, true);
				
				if(ny < 0 || ny > 192)
					slot.m_pkLabel->Hide();
				else
					slot.m_pkLabel->Show();
			}
			else
				slot.m_pkLabel->Hide();
		}
	}
}

string InventoryDlg::GetWndByID(int iID)
{
	ZGuiWnd* pkParent = m_pkResMan->Wnd("BackPackWnd");

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

void InventoryDlg::DropItems()
{
	vector<itSlot> kRemoveList;

	itSlot it;
	for( it = m_kDragSlots.begin(); it != m_kDragSlots.end(); it++)
	{
		if((*it).m_iContainer != MAIN_CONTAINER)
		{
			int iNewContainer = (*it).m_iContainer - 1;

			Point sqr;
			if(GetFreeSlotPos(sqr, iNewContainer))
			{
				Slot s = (*it);

				AddSlot( s.m_szPic[0], s.m_szPic[1], sqr, CONTAINTER_SLOTS, 
					s.m_pkItemStats, iNewContainer);
			}
		}
		else
		{
			// TODO:
			// Meddela något system att föremålet har släppts på marken.
		}

		m_pkGui->UnregisterWindow((*it).m_pkLabel);

		kRemoveList.push_back(it);
	}

	for(unsigned int i=0; i<kRemoveList.size(); i++)
		m_kDragSlots.erase(kRemoveList[i]);
}
