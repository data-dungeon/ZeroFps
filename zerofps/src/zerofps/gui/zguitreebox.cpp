// zguitreebox.cpp: implementation of the ZGuiTreebox class.
//
//////////////////////////////////////////////////////////////////////

#include "../render/zguirenderer.h"
#include "zguiscrollbar.h"
#include "zguicheckbox.h"
#include "zguitreebox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiTreebox::ZGuiTreebox(Rect kArea, ZGuiWnd* pkParent, bool bVisible, int iID) 
	: ZGuiWnd(kArea, pkParent, bVisible, iID)
{
	m_pkVertScrollbar = NULL;
	m_pkHorzScrollbar = NULL;
	m_pkCloseBnSkin = new ZGuiSkin(0,255,0,0,0,0,0);
	m_pkOpenBnSkin = new ZGuiSkin(255,0,0,0,0,0,0);
	m_iID = 33212;
	RemoveWindowFlag(WF_TOPWINDOW); // kan inte användas som mainwindow
}

ZGuiTreebox::~ZGuiTreebox()
{

}

ZGuiTreebox::Branch* ZGuiTreebox::AddItem(ZGuiTreebox::Branch* pkParent, 
										  char* szText, int iSkin)
{
	const int BUTTON_SIZE = 16;
	const int ITEM_WIDTH = 100;

	Branch* pkNewItem = NULL;

	int x, y;

	if(pkParent == NULL)
	{
		x = y = 0;
	}
	else
	{
		x = pkParent->pkOpenCloseButton->GetWndRect().Left;
		y = pkParent->pkOpenCloseButton->GetWndRect().Top;
	}

	pkNewItem = new Branch;
	pkNewItem->pkParent = pkParent;
	pkNewItem->bOpen = false;
	Rect rcButton = Rect(x,y,x+BUTTON_SIZE,y+BUTTON_SIZE);
	pkNewItem->pkOpenCloseButton = new ZGuiCheckbox(
		rcButton, this, true, m_iID++);
	pkNewItem->pkOpenCloseButton->SetMoveArea(
		pkNewItem->pkOpenCloseButton->GetScreenRect());
	pkNewItem->pkOpenCloseButton->SetButtonCheckedSkin(m_pkCloseBnSkin);
	pkNewItem->pkOpenCloseButton->SetButtonUncheckedSkin(m_pkOpenBnSkin);
	pkNewItem->pkOpenCloseButton->SetText(szText);
	m_akBranshes.push_back(pkNewItem);

	return pkNewItem;
}

bool ZGuiTreebox::Render( ZGuiRender* pkRenderer )
{
	if(!IsVisible())
		return true;

	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect()); 
	pkRenderer->RenderBorder(GetScreenRect()); 

	if(m_pkVertScrollbar && m_pkVertScrollbar->IsVisible())
		m_pkVertScrollbar->Render( pkRenderer );
	if(m_pkHorzScrollbar && m_pkHorzScrollbar->IsVisible())
		m_pkHorzScrollbar->Render( pkRenderer );

	for( vector<Branch*>::iterator it = m_akBranshes.begin(); 
		 it != m_akBranshes.end(); it++)
		 {
			(*it)->pkOpenCloseButton->Render( pkRenderer );

			if((*it)->bOpen)
			{
				for( vector<ZGuiCheckbox*>::iterator it2 = (*it)->akLeafs.begin();
					 it2 != (*it)->akLeafs.end(); it2++)
					 {
						(*it2)->Render(pkRenderer);
					 }
			}
		 }

	return true;
}
