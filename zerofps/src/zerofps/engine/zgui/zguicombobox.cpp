// Combobox.cpp: implementation of the Combobox class.
//
//////////////////////////////////////////////////////////////////////

#include "zguicombobox.h"
#include "zguilabel.h"
#include "zguilistbox.h"
#include "zgui.h"
#include "../../render/zguirenderer.h"
#include <iostream>
#include <cstdio>

const int COMBOBOX_LABEL_ID = 800;
const int LISTBOX_ID = 801;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiCombobox::ZGuiCombobox(Rect kRectangle, ZGuiWnd* pkParent, bool bVisible, int iID, int iItemHeight,
						 ZGuiSkin* pkSkinItem, ZGuiSkin* pkSkinItemSelected, ZGuiSkin* pkSkinItemHighLight,
						 ZGuiSkin* pkTopItemSkin, int iTopItemTextureMask) :
	ZGuiControl(kRectangle, pkParent, bVisible, iID)
{
	m_bIsMenu = false;
	Rect rc = GetWndRect();
	rc.Left = 0;
	rc.Right = rc.Left + kRectangle.Width();
	rc.Top = 0;
	rc.Bottom = iItemHeight;
	
	m_pkLabel = new ZGuiLabel(rc,this,true,COMBOBOX_LABEL_ID);
	m_pkLabel->SetSkin(pkTopItemSkin, iTopItemTextureMask);

	m_unNumVisibleRows = 7;
	rc.Top = 20;
	rc.Bottom = rc.Top+iItemHeight*m_unNumVisibleRows;

	m_pkListbox = new ZGuiListbox(rc,this,true,LISTBOX_ID,iItemHeight,
		pkSkinItem, pkSkinItemSelected, pkSkinItemHighLight);

	m_pkListbox->GetScrollbar()->Move(0,-20);
	int w = m_pkListbox->GetScrollbar()->GetScreenRect().Width();
	int h = m_pkListbox->GetScrollbar()->GetScreenRect().Height();
	m_pkListbox->GetScrollbar()->Resize(w,h+20*2);

	m_pkListbox->Hide();
	Resize(m_pkLabel->GetScreenRect().Width(),
		m_pkLabel->GetScreenRect().Height());
	m_pkListbox->GetItemArea().Top -= 20;
}

void ZGuiCombobox::SetZValue(int iValue)
{
	ZGuiWnd::SetZValue(iValue);
	m_pkListbox->SetZValue(iValue-1);
}

ZGuiCombobox::~ZGuiCombobox()
{

}

bool ZGuiCombobox::Render( ZGuiRender* pkRenderer )
{
	if(m_pkFont)
		pkRenderer->SetFont(m_pkFont);

	m_pkLabel->Render(pkRenderer);
	m_pkListbox->Render(pkRenderer);
	return true;
}

void ZGuiCombobox::SetScrollbarSkin(ZGuiSkin* pkSkinScrollArea, ZGuiSkin* pkSkinThumbButton, 
								    ZGuiSkin* pkSkinThumbButtonHighLight)
{
	m_pkListbox->SetScrollbarSkin(pkSkinScrollArea, pkSkinThumbButton, pkSkinThumbButtonHighLight);
}

void ZGuiCombobox::SetSkin(ZGuiSkin* pkSkin)
{
	m_pkLabel->SetSkin(pkSkin);
	m_pkListbox->SetSkin(pkSkin);
}

bool ZGuiCombobox::Notify(ZGuiWnd* pkWnd, int iCode)
{
	if(iCode == NCODE_CLICK_DOWN)
	{
		ZGuiListitem* pkSelItem = m_pkListbox->GetSelItem();
 
		if(pkSelItem && m_bIsMenu == false)
			m_pkLabel->SetText(pkSelItem->GetText());

		if(m_pkListbox->IsVisible())
		{
			m_pkListbox->Hide();
	
			Resize(m_pkLabel->GetScreenRect().Width(),
				m_pkLabel->GetScreenRect().Height());

			// Send a message to the main winproc...
			int* piParams = new int[2];
			piParams[0] = GetID(); // Listbox ID
			if(pkSelItem != NULL)
			{
				piParams[1] = pkSelItem->GetID(); // list item ID
				GetGUI()->GetActiveCallBackFunc()(
					GetGUI()->GetActiveMainWnd(), ZGM_CBN_SELENDOK, 2, piParams);
			}
			delete[] piParams;
		}
		else
		{
			m_pkListbox->Show();

			Resize(GetScreenRect().Width(),
				m_pkLabel->GetScreenRect().Height()+
				m_pkListbox->GetScreenRect().Height());
		}
	}
	else
	if(iCode == NCODE_RELEASE)
	{
		if(m_pkListbox->IsVisible() )
		{
			if(pkWnd != m_pkListbox->GetVScrollbar()->GetButton())
			{
				if(m_bIsMenu)
				{
					m_pkListbox->SelNone();
				}
				else
				{
					ZGuiListitem* pkSelItem = m_pkListbox->GetSelItem();
 
					if(pkSelItem && m_bIsMenu == false)
						m_pkLabel->SetText(pkSelItem->GetText());
				}

				m_pkListbox->Hide();
		
				Resize(m_pkLabel->GetScreenRect().Width(),
					m_pkLabel->GetScreenRect().Height());
			}
		}
	}

	return true;
}

bool ZGuiCombobox::AddItem(char* strText, unsigned int iID)
{
	m_pkListbox->AddItem(strText, iID);
	return true;
}

void ZGuiCombobox::SetLabelText(char* strText)
{ 
	m_pkLabel->SetText(strText); 
}

void ZGuiCombobox::SetLabelWidth(int iWidth)
{ 
	m_pkLabel->Resize(iWidth, m_pkLabel->GetScreenRect().Height());
}

void ZGuiCombobox::IsMenu(bool bIsMenu)
{
	m_bIsMenu = bIsMenu; 
	m_pkListbox->IsMenu(bIsMenu);
}

void ZGuiCombobox::SetNumVisibleRows(unsigned short iNumVisibleRows)
{ 
	m_unNumVisibleRows = iNumVisibleRows;
	
	Rect rc = m_pkListbox->GetScreenRect();
	int iNewHeight = (m_unNumVisibleRows+1)*m_pkListbox->GetItemHeight();
	m_pkListbox->Resize(rc.Width(), iNewHeight);

	m_pkListbox->GetItemArea().Top -= m_pkListbox->GetItemHeight();
	m_pkListbox->GetItemArea().Bottom -= m_pkListbox->GetItemHeight()*2;

	Resize(m_pkLabel->GetScreenRect().Width(),
			m_pkLabel->GetScreenRect().Height());
}

bool ZGuiCombobox::RemoveAllItems()
{
	m_pkListbox->RemoveAllItems();
	m_pkLabel->SetText(" ");
	return true;
}







