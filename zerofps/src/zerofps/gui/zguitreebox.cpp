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
										unsigned char ucSkinNormal,
										unsigned char ucSkinClosed,
										unsigned char ucSkinOpen)
{
	Node* pkNewBranch = CreateNode(pkParent, szText, ucSkinNormal,
		ucSkinClosed, ucSkinOpen);

	return pkNewBranch;
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
		for(list<Node*>::iterator it = m_kNodeList.begin(); 
			it != m_kNodeList.end(); it++)
			{
				if((*it)->pkButton == pkWnd)
				{
					bool bShow = ((ZGuiCheckbox*) pkWnd)->IsChecked();
					ShowNode((*it)->m_kChilds, bShow); // Hide/Show childrens
					//GetNumOpenChilds((*it), bShow);
					OpenNode((*it), bShow);

					if((*it)->m_kChilds.empty())
					{
						(*it)->pkButton->SetButtonCheckedSkin(GetItemSkin((*it)->ucSkinIndexOpen));
						(*it)->pkButton->SetButtonUncheckedSkin(GetItemSkin((*it)->ucSkinIndexOpen));
					}
					else
					{
						if(bShow)
							(*it)->pkButton->SetSkin(GetItemSkin((*it)->ucSkinIndexClosed));
						else
							(*it)->pkButton->SetSkin(GetItemSkin((*it)->ucSkinIndexOpen));
					}
				}
			}
	}
	return true;
}

void ZGuiTreebox::CreateInternalControls()
{
	
}

ZGuiTreebox::Node* ZGuiTreebox::CreateNode( Node* pkParent, char* szText, 
											unsigned char uiSkinNormal,
											unsigned char uiSkinClosed,
											unsigned char uiSkinOpen)
{
	const int ITEM_WIDTH = 100;

	Node* pkNewItem = new Node;

	int x, y;
	
	if(pkParent == NULL)
	{
		static int root_top = 0;
		x = 0; y = root_top;
		root_top += (BUTTON_SIZE+VERT_ROW_SPACE);
	}
	else
	{
		int iNumItems = pkParent->m_kChilds.size();

		x = pkParent->pkButton->GetWndRect().Right + HORZ_ROW_SPACE;
		y = pkParent->pkButton->GetWndRect().Bottom + VERT_ROW_SPACE +
			iNumItems*(BUTTON_SIZE+VERT_ROW_SPACE);

		pkParent->m_kChilds.push_back(pkNewItem); // add this child to parent list.
	}

	bool bHaveChilds = false;

	pkNewItem->pkPrev = pkParent;
	pkNewItem->pkNext = NULL;
	pkNewItem->bChildListIsOpen = false;

	pkNewItem->ucSkinIndexNormal = uiSkinNormal;
	pkNewItem->ucSkinIndexClosed = uiSkinClosed;
	pkNewItem->ucSkinIndexOpen = uiSkinOpen;

	Rect rcButton = Rect(x,y,x+BUTTON_SIZE,y+BUTTON_SIZE);
	ZGuiCheckbox* pkButton = (pkNewItem->pkButton = 
		new ZGuiCheckbox(rcButton, this, (pkParent == NULL), m_iID++));
	
	pkButton->SetMoveArea(pkNewItem->pkButton->GetScreenRect());
	pkButton->SetText(szText);

	if(bHaveChilds)
	{
		pkButton->SetButtonCheckedSkin(GetItemSkin(pkNewItem->ucSkinIndexOpen));
		pkButton->SetButtonUncheckedSkin(GetItemSkin(pkNewItem->ucSkinIndexClosed));
	}
	else
	{
		pkButton->SetButtonCheckedSkin(GetItemSkin(pkNewItem->ucSkinIndexClosed));
		pkButton->SetButtonUncheckedSkin(GetItemSkin(pkNewItem->ucSkinIndexOpen));
	}

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

void ZGuiTreebox::OpenNode(ZGuiTreebox::Node *pkNode, bool bOpen)
{
	pkNode->bChildListIsOpen = bOpen;

	Node* pkParent = pkNode->pkPrev;
	if(pkParent != NULL)
	{
		int iSize = (pkNode->m_kChilds.size()) * (BUTTON_SIZE+VERT_ROW_SPACE);
		int iTop = pkNode->pkButton->GetScreenRect().Top;

		ZGuiCheckbox* pkButton;
		for(unsigned int i=0; i<pkParent->m_kChilds.size(); i++)
		{
			pkButton = pkParent->m_kChilds[i]->pkButton;
			if(pkButton->GetScreenRect().Top > iTop)
			{
				int x = pkButton->GetScreenRect().Left, y=0; 

				if(bOpen == true) 
					y = pkButton->GetScreenRect().Top + iSize;
				else
					y = pkButton->GetScreenRect().Top - iSize;

				pkButton->SetPos(x, y, true, true);
				pkButton->SetMoveArea(Rect(x,y,x+BUTTON_SIZE,y+BUTTON_SIZE));
			}
		}
	}
}
