// inventorydlg.cpp: implementation of the InventoryDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "inventorydlg.h"
#include "../zerofpsv2/basic/zfsystem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int InventoryDlg::s_iSlotZCounter = 2121212;

InventoryDlg::InventoryDlg()// : ZFSubSystem("InventoryDlg") 
{
	m_pkGui = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));
	m_pkResMan = static_cast<ZGuiResourceManager*>(g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));
	m_pkTexMan = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	m_kClickOffset = Point(0,0);
}

InventoryDlg::~InventoryDlg()
{

}

void InventoryDlg::AddSlot(char *szPic, char *szPicA, Point sqr, bool bDragItem)
{
	int x = 264+sqr.x*48;
	int y = 16+sqr.y*48;

	int size = bDragItem ? 64 : 32;

	int id = GetID(sqr);

	ZGuiLabel* pkNewLabel = new ZGuiLabel(Rect(x,y,x+size,y+size), 
		m_pkResMan->Wnd("BackPackWnd"), true, id);

	string strPath = "data/textures/gui/items/";

	string strFileName = strPath; strFileName += szPic;
	int tex = m_pkTexMan->Load(strFileName.c_str(), 0);

	strFileName = strPath; strFileName += szPicA;
	int texa = m_pkTexMan->Load(strFileName.c_str(), 0);

	ZGuiSkin* pkSkin = new ZGuiSkin( tex, texa, false);

	pkNewLabel->SetSkin(pkSkin); 

	Slot kNewSlot;
	
	strcpy(kNewSlot.szPic[0], szPic);
	strcpy(kNewSlot.szPic[1], szPicA);

	kNewSlot.sqr = sqr;
	kNewSlot.pkLabel = pkNewLabel;

	kNewSlot.pkLabel->SetZValue(s_iSlotZCounter++);
	kNewSlot.pkLabel->GetParent()->SortChilds();

	if(bDragItem == false)
		m_kSlots.push_back(kNewSlot);
	else
		m_kDragSlots.push_back(kNewSlot);

	RegisterSlot(kNewSlot);
}

void InventoryDlg::OnClick(int x, int y, bool bMouseDown)
{
	Point sqr = MousePosToSqr(x,y);

	Slot* pkSlot = FindSlot(sqr);

	char szPic[50], szPicA[50];

	if(pkSlot)
	{
		if(bMouseDown)
		{
			strcpy(szPic, pkSlot->szPic[0]);
			strcpy(szPicA, pkSlot->szPic[1]);

			RemoveSlot(sqr, true, false);

			AddSlot(szPic, szPicA, sqr, true);

			m_kClickOffset.x = x-(264+sqr.x*48); 
			m_kClickOffset.y = y-(16+sqr.y*48);
		}
	}
	else
	{
		if( bMouseDown == false )
		{
			if(!m_kDragSlots.empty())
			{
				Slot kSlotToAdd = m_kDragSlots.front();

				if(sqr == Point(-1,-1))
					sqr = kSlotToAdd.sqr;

				strcpy(szPic, kSlotToAdd.szPic[0]);
				strcpy(szPicA, kSlotToAdd.szPic[1]);

				RemoveSlot(kSlotToAdd.sqr, true, true);

				AddSlot(szPic, szPicA, sqr );
			}
		}
	}
}

InventoryDlg::Slot* InventoryDlg::FindSlot(Point sqr)
{
	if(sqr == Point(-1,-1))
		return NULL;

	for(unsigned int i=0; i<m_kSlots.size(); i++)
	{
		if(m_kSlots[i].sqr == sqr)
			return &m_kSlots[i];
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

void InventoryDlg::RemoveSlot(Point sqr, bool bUnregister, bool bDragItem)
{
	if(bDragItem == false)
	{
		for(itSlot it = m_kSlots.begin(); it != m_kSlots.end(); it++)
		{
			if((*it).sqr == sqr)
			{
				if(bUnregister)
					m_pkGui->UnregisterWindow((*it).pkLabel);

				m_kSlots.erase(it);
				return;
			}		
		}
	}
	else
	{
		for(itSlot it = m_kDragSlots.begin(); it != m_kDragSlots.end(); it++)
		{
			if((*it).sqr == sqr)
			{
				if(bUnregister)
					m_pkGui->UnregisterWindow((*it).pkLabel);

				m_kDragSlots.erase(it);
				return;
			}		
		}
	}
}

void InventoryDlg::RegisterSlot(Slot slot)
{
	int id = GetID(slot.sqr);

	char szSlotName[50];
	sprintf(szSlotName, "ItemSlotLabel%i", id);
	m_pkGui->RegisterWindow(slot.pkLabel, szSlotName);
}

int InventoryDlg::GetID(Point sqr)
{
	static int counter = 1000; counter++;
	return counter;
}

void InventoryDlg::OnMouseMove(int x, int y, bool bMouseDown)
{
	int xdif=0, ydif=0;
	for(itSlot it = m_kDragSlots.begin(); it != m_kDragSlots.end(); it++)
	{
		ZGuiLabel* pkLabel = (*it).pkLabel;
		pkLabel->SetPos(x+xdif-m_kClickOffset.x,y+ydif-m_kClickOffset.y,true,true);
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
		if((*itChild)->GetID() == iID)
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
	static int s_oldPos = iPos;

	int offset = s_oldPos-iPos;

	s_oldPos = iPos;

	if(offset == 0)
		return;

	for(itSlot it = m_kSlots.begin(); it != m_kSlots.end(); it++)
	{
		Slot slot = (*it);

		Rect rc = slot.pkLabel->GetScreenRect(); 

		int y = rc.Top+offset*48;

		slot.pkLabel->SetPos(rc.Left, y, true, true);

		(*it).sqr.y = (y-16) / 48;

		if(y < 0 || y > 192)
			slot.pkLabel->Hide();
		else
			slot.pkLabel->Show();
	}	
}
