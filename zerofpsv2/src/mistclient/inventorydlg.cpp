// inventorydlg.cpp: implementation of the InventoryDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "inventorydlg.h"
#include "../zerofpsv2/basic/zfsystem.h"

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
}

InventoryDlg::~InventoryDlg()
{
	
}

bool InventoryDlg::AddItem(const char *szPic, const char *szPicA, ItemStats* pkItemStats)
{
	Point sqr;

	if(!GetFreeSlotPos(sqr))
	{
		printf("Failed to find free slot!\n");
		return false;
	}
			
	AddSlot(szPic, szPicA, sqr, CONTAINTER_SLOTS, pkItemStats);

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

		if(!GetFreeSlotPos(sqr))
		{
			printf("Failed to find free slot!\n");
			return false;
		}

		AddSlot((*it).first.first.c_str(), (*it).first.second.c_str(), 
			sqr, CONTAINTER_SLOTS, (*it).second);
	}

	ScrollItems(m_iCurrentScrollPos+1);
	ScrollItems(m_iCurrentScrollPos-1);
	
	return true;
}


void InventoryDlg::OnClick(int x, int y, bool bMouseDown)
{
	Point sqr = MousePosToSqr(x,y);

	Slot* pkSlot = FindSlot(x,y);

	char szPic[50], szPicA[50];

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

				AddSlot(szPic, szPicA, sqr, UNDER_MOUSE, pkStats);

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
		Slot kSlotToAdd = m_kDragSlots.back();

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
				GetFreeSlotPos(sqr);
				type = CONTAINTER_SLOTS;
				
			}
	
			AddSlot(szPic, szPicA, sqr, type, pkStats);

			if(m_kDragSlots.empty())
				m_pkGui->KillWndCapture(); // remove capture

			m_pkAudioSys->StartSound("/data/sound/close_window.wav",
				m_pkAudioSys->GetListnerPos(),m_pkAudioSys->GetListnerDir(),false);			
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
			return &m_kItemSlots[i];
	}

	// Leta efter special slots

	for(i=0; i<m_kItemSlots.size(); i++)
	{
		if(m_kItemSlots[i].m_pkLabel->IsVisible())
		{
			if(m_kItemSlots[i].m_pkLabel->GetScreenRect().Inside(mouse_x, mouse_y))
				return &m_kItemSlots[i];
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
			if((*it).m_kSqr == sqr)
			{
				m_pkGui->UnregisterWindow((*it).m_pkLabel);
				m_kItemSlots.erase(it);
				return true;
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
	int id = GetID(slot.m_kSqr);

	char szSlotName[50];
	sprintf(szSlotName, "ItemSlotLabel%i", id);
	m_pkGui->RegisterWindow(slot.m_pkLabel, szSlotName);
}

int InventoryDlg::GetID(Point sqr)
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
	
}

void InventoryDlg::OnScroll(int iID, int iPos)
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
			break;
		}
	}

	if(pkClickWnd != NULL)
	{
		string strScrollbarName = pkClickWnd->GetName();

		if(strScrollbarName == "SlotListScrollbar")
		{
			ScrollItems(iPos);
		}
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

bool InventoryDlg::GetFreeSlotPos(Point& refSqr)
{
	for(int y=0; y<100; y++)
		for(int x=0; x<5; x++)
		{
			Point t = Point(x,y);
			bool bTaken = false;

			itSlot it;
			for( it = m_kItemSlots.begin(); it != m_kItemSlots.end(); it++)
				if((*it).m_kSqr == t)
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
						   SlotType eType, ItemStats* pkItemStats)
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

	int id = GetID(sqr);

	ZGuiLabel* pkNewLabel = new ZGuiLabel(Rect(sx,sy,sx+size,sy+size), 
		m_pkResMan->Wnd("BackPackWnd"), true, id);

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

	switch(eType)
	{
	case UNDER_MOUSE:
		m_kDragSlots.push_back(kNewSlot);
		break;
	default:
		m_kItemSlots.push_back(kNewSlot);
		break;
	}

	RegisterSlot(kNewSlot);
}

bool InventoryDlg::SlotExist(int sx, int sy)
{
	itSlot it;
	for( it = m_kItemSlots.begin(); it != m_kItemSlots.end(); it++)
		if((*it).m_pkLabel->GetScreenRect().Inside(sx,sy))
			return true;
		
	return false;
}