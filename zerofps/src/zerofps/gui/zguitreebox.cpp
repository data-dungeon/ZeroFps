// zguitreebox.cpp: implementation of the ZGuiTreebox class.
//
//////////////////////////////////////////////////////////////////////

#include "../render/zguirenderer.h"
#include "zguiscrollbar.h"
#include "zguicheckbox.h"
#include "zguitreebox.h"
#include "zgui.h"

const int BUTTON_SIZE = 16;
const int VERT_ROW_SPACE = 1;
const int HORZ_ROW_SPACE = 1;
const int VERT_SCROLLBAR_ID = 622;
const int HORZ_SCROLLBAR_ID = 623;
const int SCROLLBAR_WIDTH = 16;

#define print_node(x) { if(x && x->pkButton) printf("%s\n", x->pkButton->GetText()); \
	else printf("NULL\n"); }

#define node_have_childs(n) (n && !n->kChilds.empty())
#define first_child(n) n->kChilds.front()
#define next_node(n) n->pkNext
#define node_parent(n) n->pkParent

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiTreebox::ZGuiTreebox(Rect kArea, ZGuiWnd* pkParent, bool bVisible, int iID) 
	: ZGuiWnd(kArea, pkParent, bVisible, iID)
{
	m_pkSelectedNode = NULL;
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
									  unsigned char ucSkinIndexSelected,
									  const char* szIDName)
{
	ZGuiTreeboxNode* pkNewBranch = CreateNode(pkParent, szText, ucSkinIndex,
		ucSkinIndexSelected);

	if(szIDName != NULL)
	{
		m_kNodeMap.insert( map<string, ZGuiTreeboxNode*>::value_type(
			string(szIDName), pkNewBranch));
	}
	return pkNewBranch;
}

ZGuiTreeboxNode* ZGuiTreebox::AddItem(string szParent,
									  char* szText, unsigned char ucSkinIndex,
									  unsigned char ucSkinIndexSelected,
									  const char* szIDName)
{
	ZGuiTreeboxNode* pkNewBranch = NULL;
	ZGuiTreeboxNode* pkParent = Node(szParent.c_str());

	if(pkParent)
	{
		pkNewBranch = CreateNode(pkParent, szText, ucSkinIndex,
			ucSkinIndexSelected);

		if(szIDName != NULL)
		{
			m_kNodeMap.insert( map<string, ZGuiTreeboxNode*>::value_type(
				string(szIDName), pkNewBranch));
		}
	}
	return pkNewBranch;
}

ZGuiTreeboxNode* ZGuiTreebox::CreateNode(ZGuiTreeboxNode* pkParent, char* szText, 
										 unsigned char ucSkinIndex,
										 unsigned char ucSkinIndexSelected)
{
	if(m_kNodeList.empty() == false && pkParent == NULL)
	{
		printf("Failed to create node! Node must have a parent.\n");
		return NULL;
	}

	int x = 0, y = 0;
	ZGuiTreeboxNode* pkNewItem = new ZGuiTreeboxNode;
	
	if(pkParent != NULL)
	{
		int iNumItems = pkParent->kChilds.size();

		x = pkParent->pkButton->GetWndRect().Right + HORZ_ROW_SPACE;
		y = 0; 

		pkParent->kChilds.push_back(pkNewItem); // add this child to parent list.

		// Nu när vi har laggt till ett barn till föräldern så kan vi
		// byta skin på föräldern.
		pkParent->pkButton->SetButtonCheckedSkin(GetItemSkin(pkParent->ucSkinIndexSelected)); 
	}
	else
	{
		x = y = -BUTTON_SIZE;
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
		for(itNode it = pkParent->kChilds.begin();
			it != pkParent->kChilds.end(); it++)
			{
				itNode next = it; 
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

	// Öppna noden om dess parent är root noden (så att den syns från början).
	if(pkParent == Root())
		OpenNode(pkParent, true);

	return pkNewItem;
}

void ZGuiTreebox::OpenChilds(list<ZGuiTreeboxNode*> kChilds, bool bOpen)
{
	int iCounter=0;
	int iNumChilds = (int) kChilds.size(); 
	for(itNode n=kChilds.begin(); n!=kChilds.end(); n++)  
	{
		int x = (*n)->pkParent->pkButton->GetScreenRect().Right + HORZ_ROW_SPACE;
		int y = (*n)->pkParent->pkButton->GetScreenRect().Bottom + VERT_ROW_SPACE;

		y += (bOpen) ? iCounter * (BUTTON_SIZE+VERT_ROW_SPACE) : 
			-iCounter * (BUTTON_SIZE+VERT_ROW_SPACE);

		(*n)->pkButton->SetPos(x,y,true,true);
		(*n)->pkButton->SetMoveArea(Rect(x,y,x+BUTTON_SIZE,y+BUTTON_SIZE));
		
		if(bOpen)
		{
			(*n)->bIsOpen = true;
			(*n)->pkButton->Show();
			(*n)->bChildListIsOpen = true;
		}
		else
		{
			// Stäng ner alla barn på samma gren.
			if(!(*n)->kChilds.empty() && (*n)->bChildListIsOpen)
				OpenChilds((*n)->kChilds, false);

			(*n)->bIsOpen = false;
			(*n)->pkButton->Hide();
			(*n)->bChildListIsOpen = false;
			(*n)->pkButton->UncheckButton();
		}

		iCounter++;
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
				for(itNode c=pkClickNode->kChilds.begin();
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

	min += BUTTON_SIZE; // räkna inte med root noden.

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
				m_pkSelectedNode = (*it);
				SetSelColor(m_pkSelectedNode);
				break;
			}
		}

		// Set focus on selected row if clicked outside 
		// the nodetree (but inside the controll window).
		if(m_pkSelectedNode)
		{
			ZGui* pkGui = GetGUI();
			if(pkGui)
				pkGui->SetFocus(m_pkSelectedNode->pkButton);
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

	// Create root node.
	ZGuiTreeboxNode* pkRoot = AddItem(NULL, "Root", 1, 1, NULL);
	pkRoot->bIsOpen = true;
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

void ZGuiTreebox::PrintChilds(list<ZGuiTreeboxNode*> kList)
{
	for(itNode it=kList.begin(); it!=kList.end(); it++)
		PrintNode((*it));
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
	printf("------------ZGuiTreebox::PrintHierarchy()------------\n");
	PrintNode(m_kNodeList.front());
	printf("-----------------------------------------------------\n");
}

void ZGuiTreebox::SetSelColor(ZGuiTreeboxNode* pkNode)
{

}

ZGuiTreeboxNode* ZGuiTreebox::Root()
{
	return m_kNodeList.front();
}

ZGuiTreeboxNode* ZGuiTreebox::Node(string strName)
{
	map<string, ZGuiTreeboxNode* >::iterator it;
	it = m_kNodeMap.find(strName);
	if(it != m_kNodeMap.end())
		return it->second;
	return NULL;
}

bool ZGuiTreebox::DeleteNode(string strName)
{
	map<string, ZGuiTreeboxNode* >::iterator it;
	it = m_kNodeMap.find(strName);
	if(it != m_kNodeMap.end())
	{
		ZGuiTreeboxNode* pkNode = it->second;
		if(DeleteNode(pkNode,false))
		{
			m_kNodeMap.erase(it);
			return true;
		}
	}
	else
	{
		printf("ZGuiTreebox::DeleteNode, Failed to find node!\n");
	}

	return false;
}

ZGuiTreeboxNode* ZGuiTreebox::RemoveNode(ZGuiTreeboxNode *pkNode)
{
	for(itNode n=m_kNodeList.begin(); n!=m_kNodeList.end(); n++)
	{
		// Find prev child
		if((*n)->pkNext == pkNode)
		{
			// Remove child from parent
			for(itNode p=pkNode->pkParent->kChilds.begin();
				p!=pkNode->pkParent->kChilds.end(); p++)
				if((*p) == pkNode)
				{
					pkNode->pkParent->kChilds.erase(p); 
					break;
				}

			// Set child next to remove child next (re-chain)
			(*n)->pkNext = pkNode->pkNext;

			for(itNode r=m_kNodeList.begin(); r!=m_kNodeList.end(); r++)
			{
				if((*r) == pkNode)
				{
					m_kNodeList.erase(r);
					break;
				}
			}

			delete pkNode->pkButton; pkNode->pkButton = NULL;
			delete pkNode; pkNode = NULL;

			printf("Node removed!\n");

			break;
		}
	}

	// Noden har inga childs, (är den sista noden på en gren eller
	// en tom föräldernod)
	for(itNode p=m_kNodeList.begin(); p!=m_kNodeList.end(); p++)
	{
		if((*p) == pkNode)
		{
			// Är det den sista noden?
			if((*p)->pkParent->kChilds.size() == 1)
			{
				(*p)->pkParent->kChilds.clear();
			}
			else
			{
				// ... eller finns det fler?
				// Remove child from parent
				for(itNode r=pkNode->pkParent->kChilds.begin();
					r!=pkNode->pkParent->kChilds.end(); r++)
					{
						if((*r)->pkNext == pkNode)
						{
							(*r)->pkNext = pkNode->pkNext;
						}
						
						if((*r) == pkNode)
						{
							pkNode->pkParent->kChilds.erase(r); 
							break;
						}
					}
			}

			m_kNodeList.erase(p);

			delete pkNode->pkButton; pkNode->pkButton = NULL;
			delete pkNode; pkNode = NULL;
			printf("Node removed!\n");

			break;
		}
	}

	return NULL;
}

ZGuiTreeboxNode* ZGuiTreebox::FindLastChild(ZGuiTreeboxNode *pkFrom)
{
	ZGuiTreeboxNode* s = pkFrom;
	
	while(1)
	{
		if(s->kChilds.empty() == false)
		{
			s = s->kChilds.back();
		}
		else
		{
			return s;
		}
	}

}

bool ZGuiTreebox::DeleteNode(ZGuiTreeboxNode* pkNode, bool bRemoveFromMap)
{
	ZGuiTreeboxNode* pkParent = pkNode->pkParent;

	if(pkNode == NULL)
		return false;

	if(pkParent == NULL)
	{
		printf("ZGuiTreebox::DeleteNode, Can't delete the root node!\n");
		return false;
	}

	ZGuiTreeboxNode* last_child;

	printf("-------- DeleteNode START --------\n");

	// Remove branch
	while(last_child != pkNode)
	{
		last_child = FindLastChild(pkNode);

		printf("Found node %s, tring to delete...", 
			last_child->pkButton->GetText());

		RemoveNode(last_child);
	}

	printf("-------- DeleteNode END ----------\n");

	// Öppna stäng noden så att nodträdet uppdateras.
	if(pkParent->bIsOpen)
	{
		OpenNode(pkParent, false);
		OpenNode(pkParent, true);
	}

	return true;
}
