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
	
	if(pkParent == NULL)
	{
		s_iRootLevel++;
	}
	else
	{
		int iNumItems = pkParent->m_kChilds.size();

		x = pkParent->pkButton->GetWndRect().Right + HORZ_ROW_SPACE;
		y = pkParent->pkButton->GetWndRect().Bottom + VERT_ROW_SPACE +
			iNumItems*(BUTTON_SIZE+VERT_ROW_SPACE);

		pkParent->m_kChilds.push_back(pkNewItem); // add this child to parent list.

		// Nu när vi har laggt till ett barn till föräldern så kan vi
		// byta skin på föräldern.
		pkParent->pkButton->SetButtonCheckedSkin(GetItemSkin(pkParent->ucSkinIndexSelected)); 
	}

	pkNewItem->pkPrev = pkParent;
	pkNewItem->pkNext = NULL;
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

	if(!m_kNodeList.empty())
		m_kNodeList.back()->pkNext = pkNewItem;

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

void ZGuiTreebox::OpenNode(Node *pkClickNode, bool bOpen)
{
	for(unsigned int i=0; i<pkClickNode->m_kChilds.size(); i++)
	{
		if(bOpen) 
		{
			 pkClickNode->m_kChilds[i]->pkButton->Show();
		}
		else
		{
			 pkClickNode->m_kChilds[i]->pkButton->Hide();

			// Close child trees in a recursiv way. 
			if(!pkClickNode->m_kChilds[i]->m_kChilds.empty() &&
				pkClickNode->m_kChilds[i]->bChildListIsOpen)
			{
				pkClickNode->m_kChilds[i]->pkButton->UncheckButton(); 
				OpenNode(pkClickNode->m_kChilds[i], false);
			}
		}

		int offset = i*(BUTTON_SIZE+VERT_ROW_SPACE);
		MoveNode(pkClickNode->m_kChilds[i], (bOpen) ? offset : -offset);
	}

	pkClickNode->bChildListIsOpen = bOpen;
}

void ZGuiTreebox::ShowNode(vector<Node*> itList, bool bShow)
{
	for(unsigned int i=0; i<itList.size(); i++)
	{
		if(bShow)
			itList[i]->pkButton->Show();
		else
			itList[i]->pkButton->Hide();

		if( itList[i]->m_kChilds.empty() == false )
			if( itList[i]->bChildListIsOpen == true)
				ShowNode(itList[i]->m_kChilds, bShow);
	}
}

static void GetNumOpenChilds(ZGuiTreebox::Node* pkNode, bool bShow)
{	
	ZGuiTreebox::Node* pkFirst = pkNode;
	ZGuiTreebox::Node* pkParent = pkNode;

	while(pkFirst != NULL)
	{
		pkFirst = pkFirst->pkNext;

		if(pkFirst && pkFirst->pkPrev == pkParent)
		{
			if(bShow)
				pkFirst->pkButton->Show();
			else
				pkFirst->pkButton->Hide();
		}		
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
				OpenNode((*it), bShow);
			}
		}
	}
	return true;
}

void ZGuiTreebox::CreateInternalControls()
{
	
}

void ZGuiTreebox::MoveNode(Node* pkNode, int offset)
{
	int x = 0, y = 0;
	Node* pkParent = pkNode->pkPrev;
	
	if(pkParent)
	{
		x = pkParent->pkButton->GetScreenRect().Right + HORZ_ROW_SPACE;
		y = pkParent->pkButton->GetScreenRect().Bottom + VERT_ROW_SPACE;
	}

	y += offset;
	pkNode->pkButton->SetPos(x, y, true, true);
	pkNode->pkButton->SetMoveArea(Rect(x,y,x+BUTTON_SIZE,y+BUTTON_SIZE));
}
