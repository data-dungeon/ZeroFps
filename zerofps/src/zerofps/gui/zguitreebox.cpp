// zguitreebox.cpp: implementation of the ZGuiTreebox class.
//
//////////////////////////////////////////////////////////////////////

#include "../render/zguirenderer.h"
#include "zguiscrollbar.h"
#include "zguicheckbox.h"
#include "zguitreebox.h"

const int BUTTON_SIZE = 16;
const int VERT_ROW_SPACE = 2;
const int HORZ_ROW_SPACE = 2;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiTreebox::ZGuiTreebox(Rect kArea, ZGuiWnd* pkParent, bool bVisible, int iID) 
	: ZGuiWnd(kArea, pkParent, bVisible, iID)
{
	m_pkVertScrollbar = NULL;
	m_pkHorzScrollbar = NULL;

	m_iID = 33212;
	RemoveWindowFlag(WF_TOPWINDOW); // kan inte användas som mainwindow

	CreateInternalControls();
}

ZGuiTreebox::~ZGuiTreebox()
{

}

ZGuiTreebox::Node* ZGuiTreebox::AddItem(Node* pkParent, 
										char* szText, 
										unsigned char ucSkinIndex,
										unsigned char ucSkinIndexSelected)
{
	Node* pkNewBranch = CreateNode(pkParent, szText, ucSkinIndex,
		ucSkinIndexSelected);
	return pkNewBranch;
}

ZGuiTreebox::Node* ZGuiTreebox::CreateNode( Node* pkParent, char* szText, 
											unsigned char ucSkinIndex,
											unsigned char ucSkinIndexSelected)
{
	static int s_iRootLevel = 0;
	int x = 0, y = s_iRootLevel * (BUTTON_SIZE+VERT_ROW_SPACE);

	Node* pkNewItem = new Node;
	pkNewItem->uiRootLevel = s_iRootLevel;
	pkNewItem->uiChildIndex = 0;
	
	if(pkParent == NULL)
	{
		s_iRootLevel++;
	}
	else
	{
		int iNumItems = pkParent->kChilds.size();

		x = pkParent->pkButton->GetWndRect().Right + HORZ_ROW_SPACE;
		y = pkParent->pkButton->GetWndRect().Bottom + VERT_ROW_SPACE +
			iNumItems*(BUTTON_SIZE+VERT_ROW_SPACE);

		pkNewItem->uiChildIndex = pkParent->kChilds.size();

		pkParent->kChilds.push_back(pkNewItem); // add this child to parent list.

		// Nu när vi har laggt till ett barn till föräldern så kan vi
		// byta skin på föräldern.
		pkParent->pkButton->SetButtonCheckedSkin(GetItemSkin(pkParent->ucSkinIndexSelected)); 
	}

	pkNewItem->pkNext = NULL;
	pkNewItem->pkParent = pkParent;
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
						printf("Next item for %s is %s\n", 
							(*it)->pkButton->GetText(), 
							(*next)->pkButton->GetText() ); 
					}
				}
			}
	}

	m_kNodeList.push_back(pkNewItem);

	return pkNewItem;
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

void ZGuiTreebox::OpenChilds(vector<Node*> kChilds, bool bOpen)
{
	for(unsigned int i=0; i<kChilds.size(); i++)
		OpenNode(kChilds[i], bOpen);
}

void ZGuiTreebox::OpenNode(Node *pkClickNode, bool bOpen)
{
	if(bOpen)
	{
		pkClickNode->pkButton->Show();

		Node* pkParent = pkClickNode->pkParent;

		if(pkParent)
		{
			int x = pkParent->pkButton->GetScreenRect().Right + HORZ_ROW_SPACE;
			int y = pkParent->pkButton->GetScreenRect().Bottom + VERT_ROW_SPACE;
			y += pkClickNode->uiChildIndex * (BUTTON_SIZE+VERT_ROW_SPACE);

			pkClickNode->pkButton->SetPos(x,y,true,true);
			pkClickNode->pkButton->SetMoveArea(Rect(x,y,x+BUTTON_SIZE,y+BUTTON_SIZE));
		}
	}
	else
	{
		pkClickNode->pkButton->Hide();
		pkClickNode->pkButton->UncheckButton();

		if(pkClickNode->kChilds.empty() == false)
			OpenChilds(pkClickNode->kChilds, false);
	}
}

void ZGuiTreebox::ShowNode(vector<Node*> itList, bool bShow)
{
	for(unsigned int i=0; i<itList.size(); i++)
	{
		if(bShow)
			itList[i]->pkButton->Show();
		else
			itList[i]->pkButton->Hide();

		if( itList[i]->kChilds.empty() == false )
			if( itList[i]->bChildListIsOpen == true)
				ShowNode(itList[i]->kChilds, bShow);
	}
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

				Node* pkNode = (*it);
				if(pkNode->kChilds.empty() == false)
				{
					pkNode->bChildListIsOpen = bShow;
					OpenChilds(pkNode->kChilds, bShow);

					int childs = pkNode->kChilds.size(); 
					if(childs != 0)
					{
						if(pkNode->pkNext)
						{
							MoveNode(pkNode->pkNext, bShow ? childs : -childs);
						}
					}
				}
			}
		}
	}
	return true;
}

void ZGuiTreebox::CreateInternalControls()
{
	
}

void ZGuiTreebox::MoveNode(Node* pkNode, int steps)
{
	int x = pkNode->pkButton->GetScreenRect().Left;
	int y = pkNode->pkButton->GetScreenRect().Top;

	y += steps * (VERT_ROW_SPACE+BUTTON_SIZE);
	pkNode->pkButton->SetPos(x, y, true, true);
	pkNode->pkButton->SetMoveArea(Rect(x,y,x+BUTTON_SIZE,y+BUTTON_SIZE));

	if(pkNode->pkNext)
	{
		MoveNode(pkNode->pkNext, steps);

		if(pkNode->pkNext->bChildListIsOpen)
		{
			MoveNode(pkNode->pkNext->kChilds.front(), steps); 
		}
	}
}

void ZGuiTreebox::PrintChilds(vector<Node*> kList)
{
	for(unsigned int i=0; i<kList.size(); i++)
		PrintNode(kList[i]);
}

void ZGuiTreebox::PrintNode(Node* pkNode)
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
