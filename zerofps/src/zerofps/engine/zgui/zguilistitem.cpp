// zguilistitem.cpp: implementation of the ZGuiListitem class.
//
//////////////////////////////////////////////////////////////////////

#include "zguilistitem.h"
#include "zguibutton.h"
#include "../../basic/rect.h"
#include "zguiwindow.h"
#include "zgui.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiListitem::ZGuiListitem(ZGuiWnd* pkParent, char* strText, unsigned int iID, 
						   ZGuiSkin* pkBkSkin, ZGuiSkin* pkSelected, 
						   ZGuiSkin* pkHighLigtSkin, ZGuiSkin* pkTextSkin,
						   int iFontMaskTexture)
{	
	int w = pkParent->GetScreenRect().Width()-20;
	m_pkButton = new ZGuiButton(Rect(0,0,w,20),pkParent,true, iID);
	m_pkButton->SetButtonDownSkin(pkSelected);
	m_pkButton->SetButtonUpSkin(pkBkSkin);
	m_pkButton->SetButtonHighLightSkin(pkHighLigtSkin);
	m_pkButton->SetTextSkin(pkTextSkin, iFontMaskTexture);
	m_pkButton->SetText(strText);  
	m_iID = iID;
}

ZGuiListitem::~ZGuiListitem()
{
	if(m_pkButton)
		delete m_pkButton;
}

void ZGuiListitem::SetText(char* strText)
{
	if(m_pkButton)
		m_pkButton->SetText(strText);
}

void ZGuiListitem::SetPos(int x, int y)
{
	if(m_pkButton)
	{
		m_pkButton->SetPos(x, y, false, true); 
	}
}

void ZGuiListitem::Resize(int w, int h)
{
	if(m_pkButton)
	{
		w = m_pkButton->GetParent()->GetWndRect().Width()-20;
		m_pkButton->Resize(w, h); 
		m_pkButton->SetMoveArea(m_pkButton->GetScreenRect()); 
	}
}

void ZGuiListitem::Deselect()
{
	if(m_pkButton)
		m_pkButton->SetSkin(m_pkButton->GetButtonUpSkin());
}

void ZGuiListitem::Select()
{
	if(m_bMenuItem == false)
	{
		if(m_pkButton)
			m_pkButton->SetSkin(m_pkButton->GetButtonDownSkin());
	}
}

unsigned int ZGuiListitem::GetID()
{
	return m_iID;
}

void ZGuiListitem::Move(int dx, int dy)
{
	if(m_pkButton)
	{
		Rect rc = m_pkButton->GetWndRect().Move(dx, dy);
		m_pkButton->SetPos(rc.Left, rc.Top, false, true);
		rc = m_pkButton->GetScreenRect();
		m_pkButton->SetMoveArea(rc,true);
	}
}

char* ZGuiListitem::GetText() 
{ 
	if(m_pkButton)
		return m_pkButton->GetText(); 
	else
		return NULL;
}






