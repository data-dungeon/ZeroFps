// zguiradiobutton.cpp: implementation of the ZGuiRadiobutton class.
//
//////////////////////////////////////////////////////////////////////

#include "zguiradiobutton.h"
#include "../render/zguirenderer.h"
#include "zgui.h"
#include "zguiresourcemanager.h"
#include <typeinfo>

ZGuiRadiobutton* ZGuiRadiobutton::m_pkLastbutton = NULL;

///////////////////////////////////////////////////////////////////////////////
// Name: Construction/Destruction
// Description:
//
ZGuiRadiobutton::ZGuiRadiobutton(Rect kRectangle, ZGuiWnd* pkParent, int iID, 
								 int iGroupID, char* szGroupName, 
								 ZGuiRadiobutton* pkPrev, bool bVisible) :
	ZGuiWnd(kRectangle, pkParent, bVisible, iID)
{
	m_pkPrev = NULL;
	m_pkNext = NULL;
	m_iGroupID = -1;
	if(szGroupName != NULL)
		strcpy(m_szGroupName, szGroupName);
	m_pkCheckbox = new ZGuiCheckbox(Rect(0,0,kRectangle.Height(),
		kRectangle.Height()), this, true, iID);
	m_pkCheckbox->Disable();
	RemoveWindowFlag(WF_CANHAVEFOCUS);
	RemoveWindowFlag(WF_TOPWINDOW); // kan inte användas som mainwindow

	ConnectToGroup(iGroupID, m_pkLastbutton);
	m_pkLastbutton = this;

	m_bUseAlhpaTest = false;
}

ZGuiRadiobutton::~ZGuiRadiobutton()
{
	// Reconnect previus.
	if(m_pkPrev)
	{
		if(m_pkNext)
			m_pkPrev->SetNext(m_pkNext);
		else
			m_pkPrev->SetNext(NULL);
	}
	
	// Reconect next.
	if(m_pkNext)
	{
		if(m_pkPrev)
			m_pkNext->SetPrev(m_pkPrev);
		else
			m_pkNext->SetPrev(NULL);
	}

	// Reset static window.
	if(m_pkLastbutton == this)
		m_pkLastbutton = NULL;

	if(m_pkCheckbox)
	{
		ResetStaticClickWnds(m_pkCheckbox);
		delete m_pkCheckbox;
		m_pkCheckbox = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Name: Render
// Description:
//
bool ZGuiRadiobutton::Render( ZGuiRender* pkRenderer )
{
	if(m_pkFont)
		pkRenderer->SetFont(m_pkFont);

	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect()); 
	pkRenderer->RenderBorder(GetScreenRect()); 
	m_pkCheckbox->Render(pkRenderer);
	return false;
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetButtonSelectedSkin
// Description:
//
void ZGuiRadiobutton::SetButtonSelectedSkin(ZGuiSkin* pkSkin)
{
	m_pkCheckbox->SetButtonCheckedSkin(pkSkin);
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetButtonUnselectedSkin
// Description:
//
void ZGuiRadiobutton::SetButtonUnselectedSkin(ZGuiSkin* pkSkin)
{
	m_pkCheckbox->SetButtonUncheckedSkin(pkSkin);
}

///////////////////////////////////////////////////////////////////////////////
// Name: Notify
// Description:
//
bool ZGuiRadiobutton::Notify(ZGuiWnd* pkWnd, int iCode)
{
	if(iCode == NCODE_CLICK_UP && m_iGroupID != -1)
	{
		Check();
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetText
// Description:
//
void ZGuiRadiobutton::SetText(char* strText, bool bResizeWnd)
{
	m_pkCheckbox->SetText(strText,true);

//	ZGui* pkGui = GetGUI();
	if(!m_pkFont && m_pkResMan)
		m_pkFont = m_pkResMan->Font("defguifont");

	int usTextLength = strlen(strText)*12+25;

	if(m_pkFont != NULL)
		usTextLength = m_pkFont->GetLength(strText);

//	int usFullsize = GetScreenRect().Width() + usTextLength;

	// Ändra storlek på radiobutton kontrollen.
	// (så att man även träffar knappen när man klickar på texten)
	Resize(usTextLength+20, GetScreenRect().Height());

	
}

///////////////////////////////////////////////////////////////////////////////
// Name: GetText
// Description:
//
char* ZGuiRadiobutton::GetText()
{
	return m_pkCheckbox->GetText(); 
}

///////////////////////////////////////////////////////////////////////////////
// Name: ConnectToGroup
// Description:
//
void ZGuiRadiobutton::ConnectToGroup(int iGroupID, ZGuiRadiobutton* pbNeigbour)
{
	// Ta bort knappen från den tidigare gruppen
	if( m_iGroupID != -1 )
	{		
		ZGuiRadiobutton* next = m_pkPrev->GetNext(); next = m_pkNext; 
		ZGuiRadiobutton* prev = m_pkNext->GetPrev(); prev = m_pkPrev; 
	}

	ZGuiRadiobutton* curr = pbNeigbour;

	if(curr != NULL)
	{
		// Leta upp det sista elementet
		while(curr->GetNext() != NULL)
		{
			curr = curr->GetNext();
		}

		curr->SetNext(this); 		
		SetPrev(curr);
	}

	m_iGroupID = iGroupID;
}

///////////////////////////////////////////////////////////////////////////////
// Name: GetWndSkinsDesc
// Description:
//
void ZGuiRadiobutton::GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const
{
	int iStart = pkSkinDesc.size(); 
	m_pkCheckbox->GetWndSkinsDesc(pkSkinDesc);
	for(unsigned int i=iStart; i<pkSkinDesc.size(); i++)
		pkSkinDesc[i].second.insert(0, "Radiobutton: ");
}

///////////////////////////////////////////////////////////////////////////////
// Name: Resize
// Description:
//
void ZGuiRadiobutton::Resize(int iWidth, int iHeight, bool bChangeMoveArea)
{
/*	iHeight = GetScreenRect().Height(); // dont allow vertcal resize
	iWidth = GetScreenRect().Width(); // dont allow vertcal resize*/

	ZGuiWnd::Resize(iWidth, iHeight, bChangeMoveArea);

/*	printf("%i, %i\n", iWidth, iHeight);*/
}

///////////////////////////////////////////////////////////////////////////////
// Name: CopyNonUniqueData
// Description:
//
void ZGuiRadiobutton::CopyNonUniqueData(const ZGuiWnd* pkSrc)
{
	if(pkSrc && typeid(*pkSrc)==typeid(ZGuiRadiobutton))
	{
		m_pkCheckbox->CopyNonUniqueData( ((ZGuiRadiobutton*)pkSrc)->m_pkCheckbox );
	}

	ZGuiWnd::CopyNonUniqueData(pkSrc);
}

void ZGuiRadiobutton::ChangeGroupName(char *szNewName)
{
	ZGuiRadiobutton* prev = m_pkPrev;
	while(prev != NULL)
	{
		if(prev->GetGroupID() == m_iGroupID)
			strcpy(prev->m_szGroupName, szNewName); 
		prev = prev->GetPrev();
	}

	ZGuiRadiobutton* next = m_pkNext;
	while(next != NULL)
	{
		if(next->GetGroupID() == m_iGroupID)
			strcpy(next->m_szGroupName, szNewName); 
		next = next->GetNext();
	}

	strcpy(m_szGroupName, szNewName);
}

//bool ZGuiRadiobutton::Rescale(int iOldWidth, int iOldHeight, int iNewWidth, int iNewHeight)
//{
//	ZGuiWnd::Rescale(iOldWidth, iOldHeight, iNewWidth, iNewHeight);
//
//	m_pkCheckbox->ZGuiCheckbox::Rescale(iOldWidth, iOldHeight, iNewWidth, iNewHeight);	
//	return true;
//}

void ZGuiRadiobutton::Check()
{
	m_pkCheckbox->CheckButton();

	ZGuiRadiobutton* prev = m_pkPrev;
	while(prev != NULL)
	{
		if(prev->GetGroupID() == m_iGroupID)
			prev->GetButton()->UncheckButton(); 
		prev = prev->GetPrev();
	}

	ZGuiRadiobutton* next = m_pkNext;
	while(next != NULL)
	{
		if(next->GetGroupID() == m_iGroupID)
			next->GetButton()->UncheckButton(); 
		next = next->GetNext();
	}

	m_pkCheckbox->CheckButton();

	SetFocus();
}

void ZGuiRadiobutton::SetTextColor(unsigned char ucR, unsigned char ucG, unsigned char ucB)
{
	m_pkCheckbox->SetTextColor(ucR, ucG, ucB);
	m_afTextColor[0] = ucR;
	m_afTextColor[1] = ucG;
	m_afTextColor[2] = ucB;
}
