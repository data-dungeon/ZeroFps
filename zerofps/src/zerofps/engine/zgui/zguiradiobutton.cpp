// zguiradiobutton.cpp: implementation of the ZGuiRadiobutton class.
//
//////////////////////////////////////////////////////////////////////

#include "zguiradiobutton.h"
#include "../../render/zguirenderer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiRadiobutton* ZGuiRadiobutton::m_pkLastbutton = NULL;

ZGuiRadiobutton::ZGuiRadiobutton(Rect kRectangle, ZGuiWnd* pkParent, int iID, int iGroupID, 
								 ZGuiRadiobutton* pkPrev, bool bVisible) :
	ZGuiControl(kRectangle, pkParent, bVisible, iID)
{
	m_pkPrev = NULL;
	m_pkNext = NULL;
	m_iGroupID = -1;
	m_pkCheckbox = new ZGuiCheckbox(Rect(0,0,kRectangle.Width(),
		kRectangle.Height()), this, true, iID);
	m_pkCheckbox->RemoveWindowFlag(WF_CANHAVEFOCUS);
	SetWindowFlag(WF_CANHAVEFOCUS);
	ConnectToGroup(iGroupID, m_pkLastbutton);
	m_pkLastbutton = this;

	list<ZGuiWnd*> pkChilds;
	m_pkCheckbox->GetChildrens(pkChilds); 

	(*pkChilds.begin())->SetZValue(m_iZValue+1);
}

ZGuiRadiobutton::~ZGuiRadiobutton()
{
	if(m_pkCheckbox)
	{
		ResetStaticClickWnds(m_pkCheckbox);
		delete m_pkCheckbox;
		m_pkCheckbox = NULL;
	}
}

bool ZGuiRadiobutton::Render( ZGuiRender* pkRenderer )
{
	if(m_pkFont)
		pkRenderer->SetFont(m_pkFont);

	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect(),(m_iBkMaskTexture > 0)); 
	pkRenderer->RenderBorder(GetScreenRect()); 
	m_pkCheckbox->Render(pkRenderer);
	return false;
}

void ZGuiRadiobutton::SetButtonSelectedSkin(ZGuiSkin* pkSkin, int iMaskTex)
{
	m_pkCheckbox->SetButtonCheckedSkin(pkSkin, iMaskTex);
}

void ZGuiRadiobutton::SetButtonUnselectedSkin(ZGuiSkin* pkSkin, int iMaskTex)
{
	m_pkCheckbox->SetButtonUncheckedSkin(pkSkin, iMaskTex);
}

bool ZGuiRadiobutton::Notify(ZGuiWnd* pkWnd, int iCode)
{
	if(iCode == NCODE_CLICK_UP && m_iGroupID != -1)
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

	return true;
}

void ZGuiRadiobutton::SetText(char* strText)
{
	m_pkCheckbox->SetText(strText);

	// Ändra storlek på radiobutton kontrollen.
	// (så att man även träffar knappen när man klickar på texten)
	Resize(strlen(strText)*12+25, GetScreenRect().Height());
}

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




