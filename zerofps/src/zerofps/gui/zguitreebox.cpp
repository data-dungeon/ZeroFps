// zguitreebox.cpp: implementation of the ZGuiTreebox class.
//
//////////////////////////////////////////////////////////////////////

#include "../render/zguirenderer.h"
#include "zguiscrollbar.h"
#include "zguicheckbox.h"
#include "zguitreebox.h"

const int BUTTON_SIZE = 16;
const int VERT_ROW_SPACE = 1;
const int HORZ_ROW_SPACE = 1;
const int VERT_SCROLLBAR_ID = 622;
const int HORZ_SCROLLBAR_ID = 623;
const int SCROLLBAR_WIDTH = 16;

#define print_node(x) { if(x && x->pkButton) printf("%s\n", x->pkButton->GetText()); \
	else printf("NULL\n"); }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiTreebox::ZGuiTreebox(Rect kArea, ZGuiWnd* pkParent, bool bVisible, int iID) 
	: ZGuiWnd(kArea, pkParent, bVisible, iID)
{
	m_pkVertScrollbar = NULL;
	m_pkHorzScrollbar = NULL;
	m_iStartrow = 0;

	m_iID = 33212;
	RemoveWindowFlag(WF_TOPWINDOW); // kan inte användas som mainwindow

	CreateInternalControls();
	m_iItemHeight = 0;
	m_iItemWidth = 0;
}

ZGuiTreebox::~ZGuiTreebox()
{

}

ZGuiTreeboxNode* ZGuiTreebox::AddItem(ZGuiTreeboxNode* pkParent, 
									  char* szText, unsigned char ucSkinIndex,
									  unsigned char ucSkinIndexSelected)
{
	ZGuiTreeboxNode* pkNewBranch = CreateNode(pkParent, szText, ucSkinIndex,
		ucSkinIndexSelected);
	return pkNewBranch;
}

ZGuiTreeboxNode* ZGuiTreebox::CreateNode( ZGuiTreeboxNode* pkParent, char* szText, 
											unsigned char ucSkinIndex,
											unsigned char ucSkinIndexSelected)
{
	static int s_iRootLevel = 0;
	int x = 0, y = s_iRootLevel * (BUTTON_SIZE+VERT_ROW_SPACE);

	ZGuiTreeboxNode* pkNewItem = new ZGuiTreeboxNode;
	pkNewItem->uiRootLevel = s_iRootLevel;
	
	if(pkParent == NULL)
	{
		s_iRootLevel++;
	}
	else
	{
		int iNumItems = pkParent->kChilds.size();

		x = pkParent->pkButton->GetWndRect().Right + HORZ_ROW_SPACE;
		y = 0; 

		pkParent->kChilds.push_back(pkNewItem); // add this child to parent list.

		// Nu när vi har laggt till ett barn till föräldern så kan vi
		// byta skin på föräldern.
		pkParent->pkButton->SetButtonCheckedSkin(GetItemSkin(pkParent->ucSkinIndexSelected)); 
	}

	pkNewItem->pkNext = NULL;
	pkNewItem->pkParent = pkParent;
	pkNewItem->bIsOpen = (pkParent == NULL) ? true : false;
	pkNewItem->bChildListIsOpen = false;
	pkNewItem->ucSkinIndex = ucSkinIndex;
	pkNewItem->ucSkinIndexSelected = ucSkinIndexSelected;

	Rect rcButton = Rect(x,y,x+BUTTON_SIZE,y+BUTTON_SIZE);
	ZGuiCheckbox* pkButton = (pkNewItem->pkButton = 
		new ZGuiCheckbox(rcButton, this, (pkParent == NULL), m_iID++));
	
	pkButton->SetMoveArea(pkNewItem->pkButton->GetScreenRect());
	pkButton->SetText(szText);

	// Sätt normal skin på både checked/unchecked eftersom noden ännu inte
	// har några barn (byts ut när ett barn skapas med denna nod som förälder).
	pkButton->SetButtonUncheckedSkin(GetItemSkin(pkNewItem->ucSkinIndex));
	pkButton->SetButtonCheckedSkin(GetItemSkin(pkNewItem->ucSkinIndex));

	// Leta igenom alla barn till denna förälder och hitta den nod
	// som är närmast före men som inte har några egna barn. 
	// Sätt denna nods next pekare till att den nya noden.
	if(pkParent && pkParent->kChilds.size() > 1)
	{
		for(itChild it = pkParent->kChilds.begin();
			it != pkParent->kChilds.end(); it++)
			{
				itChild next = it; 
				next++;

				if(next != pkParent->kChilds.end())
				{
					if((*it)->pkNext == NULL)
					{
						(*it)->pkNext = (*next);
					}
				}
			}
	}

	m_kNodeList.push_back(pkNewItem);

	return pkNewItem;
}

void ZGuiTreebox::OpenChilds(vector<ZGuiTreeboxNode*> kChilds, bool bOpen)
{
	int iNumChilds = (int) kChilds.size(); 
	for(int i=0; i<iNumChilds; i++)
	{
		int x = kChilds[i]->pkParent->pkButton->GetScreenRect().Right + HORZ_ROW_SPACE;
		int y = kChilds[i]->pkParent->pkButton->GetScreenRect().Bottom + VERT_ROW_SPACE;

		y += (bOpen) ? i * (BUTTON_SIZE+VERT_ROW_SPACE) : -i * (BUTTON_SIZE+VERT_ROW_SPACE);

		kChilds[i]->pkButton->SetPos(x,y,true,true);
		kChilds[i]->pkButton->SetMoveArea(Rect(x,y,x+BUTTON_SIZE,y+BUTTON_SIZE));
		
		if(bOpen)
		{
			kChilds[i]->bIsOpen = true;
			kChilds[i]->pkButton->Show();
			kChilds[i]->bChildListIsOpen = true;
		}
		else
		{
			// Stäng ner alla barn på samma gren.
			if(!kChilds[i]->kChilds.empty() && kChilds[i]->bChildListIsOpen)
				OpenChilds(kChilds[i]->kChilds, false);

			kChilds[i]->bIsOpen = false;
			kChilds[i]->pkButton->Hide();
			kChilds[i]->bChildListIsOpen = false;
			kChilds[i]->pkButton->UncheckButton();
		}
	}
}

void ZGuiTreebox::OpenNode(ZGuiTreeboxNode *pkClickNode, bool bOpen)
{
	if(bOpen)
	{
		pkClickNode->bChildListIsOpen = true;
	}
	else
	{
		pkClickNode->bChildListIsOpen = false;
	}

	m_iItemHeight = 0;
	OpenChilds(pkClickNode->kChilds, bOpen);

	int childs = pkClickNode->kChilds.size();
	if(bOpen == false && !pkClickNode->kChilds.empty())
	{
		ZGuiTreeboxNode* next = pkClickNode->pkNext;
		if(next)
		{
			int height_diff = next->pkButton->GetScreenRect().Top -
				pkClickNode->pkButton->GetScreenRect().Bottom;

			childs = height_diff / (BUTTON_SIZE+VERT_ROW_SPACE);
		}
	}

	int max = -10000000, min =  10000000;

	for(itNode it=m_kNodeList.begin(); it!=m_kNodeList.end(); it++)
	{
		Rect rc = (*it)->pkButton->GetScreenRect();
		if((*it) != pkClickNode)
		{
			if(rc.Top > pkClickNode->pkButton->GetScreenRect().Top)
			{
				bool bMoveItem = true;
				for(itChild c=pkClickNode->kChilds.begin();
					c!=pkClickNode->kChilds.end(); c++)
					{
						if((*it) == (*c))
						{
							bMoveItem = false;
							break;
						}
					}

				if(bMoveItem)
					MoveNode((*it), bOpen ? childs : -childs, false);
			}
		}

		rc = (*it)->pkButton->GetScreenRect();  // måste kolla arean igenom 
												// eftersom den kan ha ändrats.
		if((*it)->bIsOpen)
		{
			if(rc.Bottom > max)
				max = rc.Bottom;
			if(rc.Top < min)
				min = rc.Top;

			if(rc.Bottom>GetScreenRect().Bottom || rc.Top<GetScreenRect().Top)
				(*it)->pkButton->Hide();
			else
				(*it)->pkButton->Show();
		}
	}

	if(!(max == -10000000 && min == 10000000))
		m_iItemHeight = abs(max-min);
	else
		m_iItemHeight = BUTTON_SIZE+VERT_ROW_SPACE;

	// Change range of vertical scrollbar
	ChangeScrollbarRange(m_iItemWidth,m_iItemHeight);
}

bool ZGuiTreebox::Notify(ZGuiWnd* pkWnd, int iCode)
{
	if(iCode == NCODE_CLICK_UP)
	{
		for(itNode it=m_kNodeList.begin(); it!=m_kNodeList.end(); it++)
		{
			if((*it)->pkButton == pkWnd)
			{				
				// Öppna/stäng noden som har klickats.
				bool bShow = ((ZGuiCheckbox*) pkWnd)->IsChecked(); 
				OpenNode((*it), bShow);
			}
		}
	}
	else
	if(iCode == NCODE_MOVE)
	{
		if(pkWnd->GetID() == VERT_SCROLLBAR_ID)
		{
			ScrollRows(true);
		}		
	}
	return true;
}

void ZGuiTreebox::MoveNode(ZGuiTreeboxNode* pkNode, int steps, bool bRecursive)
{
	int x = pkNode->pkButton->GetScreenRect().Left;
	int y = pkNode->pkButton->GetScreenRect().Top;

	y += steps * (VERT_ROW_SPACE+BUTTON_SIZE);
	pkNode->pkButton->SetPos(x, y, true, true);
	pkNode->pkButton->SetMoveArea(Rect(x,y,x+BUTTON_SIZE,y+BUTTON_SIZE));

	if(bRecursive)
	{
		if(!pkNode->kChilds.empty() && pkNode->bChildListIsOpen) 
			MoveNode(pkNode->kChilds.front(), steps);
		
		if(pkNode->pkNext)
			MoveNode(pkNode->pkNext, steps);
	}
}

void ZGuiTreebox::CreateInternalControls()
{
	Rect rc = GetWndRect();
	int x = rc.Width()-SCROLLBAR_WIDTH;
	int y = 0;
	int w = SCROLLBAR_WIDTH;
	int h = rc.Height()-SCROLLBAR_WIDTH;
	m_pkVertScrollbar = new ZGuiScrollbar(Rect(x,y,x+w,y+h),
		this,true,VERT_SCROLLBAR_ID); 
	m_pkVertScrollbar->SetScrollInfo(0,100,1.0f,0); 

	x = 0;
	y = rc.Height()-SCROLLBAR_WIDTH;
	w = rc.Width()-SCROLLBAR_WIDTH;
	h = SCROLLBAR_WIDTH;
	m_pkHorzScrollbar = new ZGuiScrollbar(Rect(x,y,x+w,y+h),
		this,true,HORZ_SCROLLBAR_ID); 
	m_pkHorzScrollbar->SetScrollInfo(0,1,1.0f,0); 
}

void ZGuiTreebox::ChangeScrollbarRange(int width, int height)
{
	float fPageSize;

	// Change range of vertical scrollbar
	int iRows = (int) ((float) height / (BUTTON_SIZE+VERT_ROW_SPACE));
	fPageSize = (float) m_pkVertScrollbar->GetScreenRect().Height() / height; 
	if(fPageSize > 1.0f)
		fPageSize = 1.0f;

	m_pkVertScrollbar->SetScrollInfo(0,iRows,fPageSize,0);

}

void ZGuiTreebox::ScrollRows(bool bVertically)
{	
	static int PREV_VERT_SCROLLROW = -1000;

	if(bVertically)
	{
		m_iStartrow = m_pkVertScrollbar->GetPos();
		if(m_iStartrow < 0)
			m_iStartrow = 0;

		int row_size = 12;

		if(m_pkFont)
			row_size = m_pkFont->m_cCharCellSize;

		// Reset parameter
		m_pkVertScrollbar->m_iScrollChange = 0;
	}

	if(PREV_VERT_SCROLLROW != m_iStartrow)
	{
		int offset = m_iStartrow;
		if(PREV_VERT_SCROLLROW != -1000)
			offset -= PREV_VERT_SCROLLROW;

		for(itNode it=m_kNodeList.begin(); it!=m_kNodeList.end(); it++)
		{
			(*it)->pkButton->Move(0, -(offset*(BUTTON_SIZE+VERT_ROW_SPACE)), 
				true, true);

			(*it)->pkButton->SetMoveArea((*it)->pkButton->GetScreenRect());

			Rect rc = (*it)->pkButton->GetScreenRect();
			if((*it)->bIsOpen)
			{
				if(rc.Bottom>GetScreenRect().Bottom || rc.Top<GetScreenRect().Top)
					(*it)->pkButton->Hide();
				else
					(*it)->pkButton->Show();
			}
		}

		PREV_VERT_SCROLLROW = m_iStartrow;
	}
}

bool ZGuiTreebox::InsertBranchSkin(unsigned int uiIndex, ZGuiSkin* pkSkin)
{
	if(uiIndex == m_kItemSkinList.size())
	{
		m_kItemSkinList.push_back(pkSkin);
		return true;
	}

	unsigned int counter = 0;
	list<ZGuiSkin*>::iterator itSkin = m_kItemSkinList.begin();
	for( ; itSkin != m_kItemSkinList.end(); itSkin++)
	{
		if(counter == uiIndex)
		{
			m_kItemSkinList.insert(itSkin,pkSkin);
			return true;
		}
	}

	printf("Failed to insert branch skin in tree controll!. Bad index.\n\n\n");
	return false;
}

void ZGuiTreebox::SetScrollbarSkin(ZGuiSkin* pkSkinScrollArea, ZGuiSkin* pkSkinThumbButton, 
								   ZGuiSkin* pkSkinThumbButtonHighLight)
{
	m_pkVertScrollbar->SetSkin(pkSkinScrollArea);
	m_pkVertScrollbar->SetThumbButtonSkins(pkSkinThumbButton, pkSkinThumbButtonHighLight);

	m_pkHorzScrollbar->SetSkin(pkSkinScrollArea);
	m_pkHorzScrollbar->SetThumbButtonSkins(pkSkinThumbButton, pkSkinThumbButtonHighLight);
}

unsigned int ZGuiTreebox::GetNumItemSkins()
{
	return m_kItemSkinList.size();
}

ZGuiSkin* ZGuiTreebox::GetItemSkin(unsigned int uiIndex)
{
	unsigned int uiSkins = GetNumItemSkins();
	list<ZGuiSkin*>::iterator itSkin = m_kItemSkinList.begin();

	for(unsigned int i=0; i<uiSkins; i++)
	{
		if(i == uiIndex)
			return (*itSkin);
		itSkin++;
	}

	return NULL;
}

void ZGuiTreebox::PrintChilds(vector<ZGuiTreeboxNode*> kList)
{
	for(unsigned int i=0; i<kList.size(); i++)
		PrintNode(kList[i]);
}

void ZGuiTreebox::PrintNode(ZGuiTreeboxNode* pkNode)
{
	printf("%s", pkNode->pkButton->GetText()); 

	if(pkNode->pkParent)
		printf(", parent:%s", pkNode->pkParent->pkButton->GetText()); 
	else
		printf(", parent:NULL"); 

	if(pkNode->pkNext)
		printf(", next:%s\n", pkNode->pkNext->pkButton->GetText()); 
	else
		printf(", next:NULL\n"); 

	if(pkNode->kChilds.empty() == false)
		PrintChilds(pkNode->kChilds);
}

void ZGuiTreebox::PrintHierarchy()
{
	printf("-----------------------------------------------------\n");
	PrintNode(m_kNodeList.front());
}