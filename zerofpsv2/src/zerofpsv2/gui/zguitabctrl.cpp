// zguitabctrl.cpp: implementation of the ZGuiTabCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "zguitabctrl.h"
#include <stdio.h>
#include "../basic/zguifont.h"
#include "../basic/zguiskin.h"
#include "zgui.h"
#include "zguiresourcemanager.h"
#include "../basic/globals.h"

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////

unsigned int ZGuiTabCtrl::s_uiPageIDCounter = 2213;
unsigned int ZGuiTabCtrl::s_uiTabIDCounter = 1213;

static int ID_TABCTRL_NEXT = 333;
static int ID_TABCTRL_PREV = 334;

ZGuiTabCtrl::ZGuiTabCtrl(Rect kRect, ZGuiWnd* pkParent, bool bVisible, int iID) 
	: ZGuiWnd(kRect, pkParent, bVisible, iID)
{
	m_uiMarg = 2;
	m_uiTabHeight = 20;
	m_bEnabled = true;
	m_uiCurrentPage = 0;
	m_pkNextTabBn = NULL;
	m_pkPrevTabBn = NULL;
	m_pkNextEndSkin = NULL;
	m_pkPrevEndSkin = NULL;
	CreateInternalControls();
	SetWindowFlag(WF_TOPWINDOW);
}

ZGuiTabCtrl::~ZGuiTabCtrl()
{

}

void ZGuiTabCtrl::CreateInternalControls()
{
	int iParentWidth = GetScreenRect().Width();
	const int c_iButtonSize  = 16;

	int y = m_uiTabHeight+5-c_iButtonSize;

	m_pkNextTabBn = new ZGuiButton( 
		Rect(iParentWidth-c_iButtonSize-m_uiMarg, y, 
			iParentWidth-m_uiMarg, y+c_iButtonSize), 
		this, true, ID_TABCTRL_NEXT);

	m_pkPrevTabBn = new ZGuiButton( 
		Rect(iParentWidth-c_iButtonSize-m_uiMarg-c_iButtonSize-m_uiMarg, y, 
			iParentWidth-m_uiMarg-c_iButtonSize-m_uiMarg, y+c_iButtonSize), 
		this, true, ID_TABCTRL_PREV);	
}

bool ZGuiTabCtrl::InsertPage(char* szResWndName, unsigned int uiIndex, 
									  char* szTabText, unsigned int iTabSkinIndex)
{
	unsigned int iNumTabs = m_kPageList.size();

	if( uiIndex > iNumTabs )
	{
		printf("Failed to insert page in tab control! Bad index.\n\n\n");
		return false;
	}

	int x = m_uiMarg;
	int y = m_uiTabHeight+5+m_uiMarg;

	Rect rcPage(x,y,x+GetScreenRect().Width()-x-m_uiMarg,
		y+GetScreenRect().Height()-y-m_uiMarg);

	unsigned int uiTabWidth = 40;
	unsigned int uiTabOffset = iNumTabs * uiTabWidth;

	if(m_pkFont)
	{
		uiTabWidth = m_pkFont->GetLength(szTabText)+5;
		uiTabOffset = 0;

		unsigned int counter=0;
		for(list<ZGuiButton*>::iterator itButton = m_kTabList.begin(); 
			itButton != m_kTabList.end(); itButton++)
			{
				if(uiIndex > counter)
					uiTabOffset += (m_pkFont->GetLength((*itButton)->GetText())+5);
				counter++;
			}
	}

	Rect rcTab(uiTabOffset,5,uiTabOffset+uiTabWidth,5+m_uiTabHeight);
	rcTab = rcTab.Move(m_uiMarg,m_uiMarg);

	ZGuiWnd* pkNewPage = NULL;
	ZGuiButton* pkNewTab = NULL;
	
	// Create and insert page
	if(uiIndex == m_kPageList.size())
	{
		m_kPageList.push_back(new ZGuiWnd(rcPage, 
			this, true, s_uiPageIDCounter++));
		m_kTabList.push_back(new ZGuiButton(rcTab, 
			this, true, s_uiTabIDCounter++));
		
		pkNewPage = m_kPageList.back();
		pkNewTab = m_kTabList.back();

		pkNewPage->SetText(szTabText); 
	}
	else
	{
		unsigned int counter=0;
		for(list<ZGuiWnd*>::iterator itPage = m_kPageList.begin(); 
			itPage != m_kPageList.end(); itPage++)
		{
			if(counter == uiIndex)
			{
				pkNewPage = (*m_kPageList.insert(itPage, new ZGuiWnd(
					rcPage, this, true, s_uiPageIDCounter++)));
				break;
			}
			counter++;
		}

		counter=0;
		for(list<ZGuiButton*>::iterator itButton = m_kTabList.begin(); 
			itButton != m_kTabList.end(); itButton++)
		{
			if(counter == uiIndex)
			{
				pkNewTab = (*m_kTabList.insert(itButton, new ZGuiButton(
					rcTab, this, true, s_uiTabIDCounter++)));
				break;
			}
			counter++;
		}
	}

	if(pkNewTab)
	{
		SetButtonSkin(pkNewTab, iTabSkinIndex);
		pkNewTab->SetMoveArea(pkNewTab->GetScreenRect());
		pkNewTab->SetText(szTabText);

		int Min = GetScreenRect().Left+m_uiMarg;
		int Max = m_pkPrevTabBn->GetScreenRect().Left-m_uiMarg;

		if( pkNewTab->GetScreenRect().Left < Min || 
			pkNewTab->GetScreenRect().Right > Max)
			pkNewTab->Hide();
		else
			pkNewTab->Show();
	}

	if(pkNewPage)
	{
		pkNewPage->SetMoveArea(pkNewPage->GetScreenRect());
		pkNewPage->SetSkin(new ZGuiSkin(214,211,206, 92,92,92, 0));
		pkNewPage->RemoveWindowFlag(WF_CANHAVEFOCUS); 

		ZGui* pkGui = GetGUI();

		if(pkGui)
		{
			pkGui->RegisterWindow(pkNewPage, szResWndName);
		}
		else
		{
			printf("Failed to register window for tabpage\n");
		}
	}

	return true;
}

bool ZGuiTabCtrl::DeletePage(unsigned int uiIndex)
{
	return true;
}

bool ZGuiTabCtrl::DeleteAllPages()
{
	return true;
}

bool ZGuiTabCtrl::InsertTabSkin(unsigned int uiIndex, ZGuiSkin* pkSkin)
{
	if(uiIndex == m_kTabSkinList.size())
	{
		m_kTabSkinList.push_back(pkSkin);
		return true;
	}

	unsigned int counter = 0;
	list<ZGuiSkin*>::iterator itSkin = m_kTabSkinList.begin();
	for( ; itSkin != m_kTabSkinList.end(); itSkin++)
	{
		if(counter == uiIndex)
		{
			m_kTabSkinList.insert(itSkin,pkSkin);
			return true;
		}
	}

	printf("Failed to insert tab skin in tab controll!. Bad index.\n\n\n");
	return false;
}

ZGuiWnd* ZGuiTabCtrl::GetPage(unsigned int uiIndex)
{
	unsigned int uiPages = GetNumPages();
	list<ZGuiWnd*>::iterator itPage = m_kPageList.begin();

	for(unsigned int i=0; i<uiPages; i++)
	{
		if(i == uiIndex)
			return (*itPage);

		itPage++;
	}

	return NULL;
}

bool ZGuiTabCtrl::Notify(ZGuiWnd* pkWnd, int iCode)
{
	if(iCode == NCODE_CLICK_DOWN)
	{
		if(pkWnd == m_pkNextTabBn)
		{
			if(!MoveTabs(true))
				pkWnd->Disable();
			else
				m_pkPrevTabBn->Enable();
			return true;
		}

		if(pkWnd == m_pkPrevTabBn)
		{
			if(!MoveTabs(false))
				pkWnd->Disable();
			else
				m_pkNextTabBn->Enable();
			return true;
		}

		int counter = 0;
		for(list<ZGuiButton*>::iterator itButton = m_kTabList.begin(); 
			itButton != m_kTabList.end(); itButton++)
		{
			if((*itButton) == pkWnd)
			{
				SetCurrentPage(counter);
				break;
			}
			counter++;
		}
	}

	return true;
}

unsigned int ZGuiTabCtrl::GetNumPages()
{
	return m_kTabList.size();
}

unsigned int ZGuiTabCtrl::GetCurrentPage()
{
	return m_uiCurrentPage;
}

void ZGuiTabCtrl::SetCurrentPage(unsigned int index)
{
	static int iPrevCurrentPage = -1;
	unsigned int uiPages = GetNumPages();

	if(index < uiPages)
		m_uiCurrentPage  = index;
	else
		return;

	list<ZGuiButton*>::iterator itButton = m_kTabList.begin();
	list<ZGuiWnd*>::iterator itPage = m_kPageList.begin();

	for(unsigned int i=0; i<uiPages; i++)
	{
		if(i == m_uiCurrentPage)
		{
			// Higher the selected tab.
			Rect rc = (*itButton)->GetScreenRect();
			(*itButton)->Resize(rc.Width(), rc.Height()+5);
			(*itButton)->SetPos(rc.Left, rc.Top-5, true, true);
			
			SetButtonSkin((*itButton), 1);
			(*itPage)->Show();

			int* piParams = new int[3];
			piParams[0] = m_uiCurrentPage;
			piParams[1] = iPrevCurrentPage;
			piParams[2] = (int) this;
			SendNotifyMessage(ZGM_TCN_SELCHANGE, 3, piParams);
			delete[] piParams;
		}
		else
		{
			SetButtonSkin((*itButton), 0);
			(*itPage)->Hide();
		}

		// Lower the previus selected tab back again.
		if(iPrevCurrentPage == (int) i)
		{
			Rect rc = (*itButton)->GetScreenRect();
			(*itButton)->Resize(rc.Width(), rc.Height()-5);
			(*itButton)->SetPos(rc.Left, rc.Top+5, true, true);
		}

		itButton++;
		itPage++;
	}

	// Move tab into focus

	if(IsVisible())
	{
		bool bMoveLeft = (iPrevCurrentPage < index);
		bool stop = false;

		while(1)
		{
			list<ZGuiButton*>::iterator itButton;

			// Find first visible button.
			unsigned int uiIndex=0;
			ZGuiButton* pkFirst = NULL;
			for(itButton = m_kTabList.begin(); itButton != m_kTabList.end(); itButton++)
			{
				if(uiIndex == index)
				{
					if((*itButton)->IsVisible())
					{
						stop = true;
						break;
					}
				}

				uiIndex++;
			}

			if(stop)
				break;
			else
				MoveTabs(bMoveLeft);
		}
	}
	

	iPrevCurrentPage = index;
}

void ZGuiTabCtrl::ShowPage(unsigned int index, bool bVisible)
{
	unsigned int uiPages = GetNumPages();
	list<ZGuiWnd*>::iterator itPage = m_kPageList.begin();

	for(unsigned int i=0; i<uiPages; i++)
	{
		if(i == index)
		{
			if(bVisible == true)
				(*itPage)->Show();
			else
				(*itPage)->Hide();
			break;
		}

		itPage++;
	}	
}

void ZGuiTabCtrl::SetNextButtonSkin(ZGuiSkin* pkUp, ZGuiSkin* pkDown, 
									ZGuiSkin* pkEnd)
{
	m_pkNextTabBn->SetButtonUpSkin(pkUp);
	m_pkNextTabBn->SetButtonDownSkin(pkDown);
	m_pkNextTabBn->SetButtonHighLightSkin(pkUp);
	m_pkNextEndSkin = pkEnd;
}

void ZGuiTabCtrl::SetPrevButtonSkin(ZGuiSkin* pkUp, ZGuiSkin* pkDown, 
									ZGuiSkin* pkEnd)
{
	m_pkPrevTabBn->SetButtonUpSkin(pkUp);
	m_pkPrevTabBn->SetButtonDownSkin(pkDown);
	m_pkPrevTabBn->SetButtonHighLightSkin(pkUp);
	m_pkPrevEndSkin = pkEnd;
}

bool ZGuiTabCtrl::SetButtonSkin(ZGuiButton* pkButton, unsigned int index)
{
	unsigned int counter = 0;
	list<ZGuiSkin*>::iterator itSkin = m_kTabSkinList.begin();
	for( ; itSkin != m_kTabSkinList.end(); itSkin++)
	{
		if(counter == index)
		{
			pkButton->SetButtonUpSkin(*itSkin);
			pkButton->SetButtonDownSkin(*itSkin);
			pkButton->SetButtonHighLightSkin(*itSkin);
			return true;
		}

		counter++;
	}

	printf("Failed to set button skin to tab control!\n\n\n");
	return false;
}

bool ZGuiTabCtrl::MoveTabs(bool bLeft)
{
	if(bLeft && !m_kTabList.empty() && m_kTabList.back()->IsVisible() )
	{
		return false;
	}

	list<ZGuiButton*>::iterator itButton;

	// Find first visible button.
	unsigned int uiIndex=0;
	ZGuiButton* pkFirst = NULL;
	for(itButton = m_kTabList.begin(); itButton != m_kTabList.end(); itButton++)
	{
		Rect rc = (*itButton)->GetWndRect();

		if( (bLeft && rc.Left == (int) m_uiMarg) || (!bLeft && rc.Right == (int) m_uiMarg) )
		{
			pkFirst = (*itButton);
			break;
		}
		uiIndex++;
	}

	if(pkFirst == NULL || (bLeft && uiIndex == GetNumPages()-1) )
		return false;

	int Min = GetScreenRect().Left+m_uiMarg;
	int Max = m_pkPrevTabBn->GetScreenRect().Left-m_uiMarg;

	// Move all button to the left at offset pkFirst.Width()
	int iOffset = pkFirst->GetScreenRect().Width();
	for(itButton = m_kTabList.begin(); itButton != m_kTabList.end(); itButton++)
	{
		(*itButton)->Move(bLeft ? -iOffset : iOffset,0,true,true); 
		(*itButton)->SetMoveArea((*itButton)->GetScreenRect(),true);
		if( (*itButton)->GetScreenRect().Left < Min || 
			(*itButton)->GetScreenRect().Right > Max)
			(*itButton)->Hide();
		else
			(*itButton)->Show();
	}

	return true;
}

void ZGuiTabCtrl::SetFont(ZGuiFont* pkFont)
{
	m_pkFont = pkFont;
}

bool ZGuiTabCtrl::SendNotifyMessage(int iType, int iParams, void *pMsg)
{
	ZGui* pkGui = GetGUI();
	if(pkGui)
	{
		callbackfunc cb = pkGui->GetActiveCallBackFunc();
		if(cb)
		{
			ZGuiWnd* pkActivMainWnd = pkGui->GetActiveMainWnd();

			if(pkActivMainWnd)
			{	
				cb(pkActivMainWnd, iType, iParams, pMsg);
				return true;
			}
		}
	}

	return false;
}

bool ZGuiTabCtrl::Rescale(int iOldWidth, int iOldHeight, int iNewWidth, int iNewHeight)
{
	ZGuiWnd::Rescale(iOldWidth, iOldHeight, iNewWidth, iNewHeight);

/*	list<ZGuiWnd*>::iterator it = m_kPageList.begin();
	for( ; it != m_kPageList.end(); it++)
		(*it)->Rescale( iOldWidth, iOldHeight, iNewWidth, iNewHeight );*/

	list<ZGuiButton*>::iterator it2 = m_kTabList.begin();
	for( ; it2 != m_kTabList.end(); it2++)
		(*it2)->Rescale( iOldWidth, iOldHeight, iNewWidth, iNewHeight );

	m_pkNextTabBn->Rescale( iOldWidth, iOldHeight, iNewWidth, iNewHeight );
	m_pkPrevTabBn->Rescale( iOldWidth, iOldHeight, iNewWidth, iNewHeight );

	m_uiMarg = (unsigned int) round2((float) m_uiMarg * (float) ((float)iNewWidth/(float)iOldWidth));
	m_uiTabHeight = (unsigned int) round2((float) m_uiTabHeight * (float) ((float)iNewHeight/(float)iOldHeight));

	return true;
}

void ZGuiTabCtrl::GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const
{
	pkSkinDesc.push_back( SKIN_DESC(&(ZGuiSkin*)m_pkSkin, string("Tabctrl")) );
	
	unsigned int i;
	int iStart = pkSkinDesc.size(); 
	m_pkNextTabBn->GetWndSkinsDesc(pkSkinDesc);
	for( i=iStart; i<pkSkinDesc.size(); i++)
		pkSkinDesc[i].second.insert(0, "Tabctrl: nexttab: ");	

	iStart = pkSkinDesc.size(); 
	m_pkPrevTabBn->GetWndSkinsDesc(pkSkinDesc);
	for( i=iStart; i<pkSkinDesc.size(); i++)
		pkSkinDesc[i].second.insert(0, "Tabctrl: prevtab: ");	
}

void ZGuiTabCtrl::Resize(int Width, int Height, bool bChangeMoveArea)
{
	int x, y;

	int iParentWidth = Width;
	const int c_iButtonSize  = 16;

	y = m_uiTabHeight+5-c_iButtonSize;
	x = iParentWidth-c_iButtonSize-m_uiMarg;
	m_pkNextTabBn->SetPos(x,y,false,true);

	x = iParentWidth-c_iButtonSize-m_uiMarg-c_iButtonSize-m_uiMarg;
	m_pkPrevTabBn->SetPos(x,y,false,true);


	x = m_uiMarg;
	y = m_uiTabHeight+5+m_uiMarg;

	Rect rcPage(x,y,x+GetScreenRect().Width()-x-m_uiMarg,
		y+GetScreenRect().Height()-y-m_uiMarg);

	list<ZGuiWnd*>::iterator it = m_kPageList.begin();
	for( ; it != m_kPageList.end(); it++)
	{
		ZGuiWnd* pkWnd = (*it);
		pkWnd->Resize(rcPage.Width(), rcPage.Height());
	}


	ZGuiWnd::Resize(Width, Height, bChangeMoveArea); 
}