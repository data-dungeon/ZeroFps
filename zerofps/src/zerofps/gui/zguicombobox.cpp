// Combobox.cpp: implementation of the Combobox class.
//
//////////////////////////////////////////////////////////////////////

#include "zguicombobox.h"
#include "zguilabel.h"
#include "zguilistbox.h"
#include "zgui.h"
#include "../render/zguirenderer.h"
#include <iostream>
#include <cstdio>
#include <typeinfo>

const int COMBOBOX_LABEL_ID = 800;
const int LISTBOX_ID = 801;

///////////////////////////////////////////////////////////////////////////////
// Name: Construction/Destruction
// Description: 
//
ZGuiCombobox::ZGuiCombobox(Rect kRectangle,ZGuiWnd* pkParent,
						   bool bVisible,int iID,int iItemHeight,
						   ZGuiSkin* pkSkinItem,ZGuiSkin* pkSkinItemSelected,
						   ZGuiSkin* pkSkinItemHighLight,ZGuiSkin* pkTopItemSkin) :
	ZGuiControl(kRectangle,pkParent,bVisible,iID)
{
	m_bIsMenu = false;
	Rect rc = GetWndRect();
	rc.Left = 0;
	rc.Right = rc.Left + kRectangle.Width();
	rc.Top = 0;
	rc.Bottom = iItemHeight;

	// Resiza comboboxen,annars fångar ZGuiCombobox 
	// upp Notify medelanden,vilket är obra (det 
	// sköter listboxen själv).
	ZGuiWnd::Resize(kRectangle.Width(),iItemHeight); 

	m_pkLabel = new ZGuiLabel(rc,this,true,COMBOBOX_LABEL_ID);
	m_pkLabel->SetSkin(pkTopItemSkin);

	m_unNumVisibleRows = 4; 
	rc.Top = 20;
	rc.Bottom = rc.Top+iItemHeight*m_unNumVisibleRows;

	m_pkListbox = new ZGuiListbox(rc,this,true,LISTBOX_ID,iItemHeight,
		pkSkinItem,pkSkinItemSelected,pkSkinItemHighLight);

	m_pkListbox->Hide();
	Resize(m_pkLabel->GetScreenRect().Width(),
		m_pkLabel->GetScreenRect().Height());
	m_pkListbox->GetItemArea().Top -= 20;

	RemoveWindowFlag(WF_CANHAVEFOCUS);
}

///////////////////////////////////////////////////////////////////////////////
// Name: Create
// Description: 
//
ZGuiCombobox::~ZGuiCombobox()
{
	delete m_pkLabel;
	m_pkLabel = NULL;
	delete m_pkListbox;
	m_pkListbox = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetZValue
// Description: 
//
void ZGuiCombobox::SetZValue(int iValue)
{
	ZGuiWnd::SetZValue(iValue);
	m_pkListbox->SetZValue(iValue-1);
}

///////////////////////////////////////////////////////////////////////////////
// Name: Render
// Description: 
//
bool ZGuiCombobox::Render( ZGuiRender* pkRenderer )
{
	if(m_pkFont)
		pkRenderer->SetFont(m_pkFont);

	m_pkLabel->Render(pkRenderer);
	m_pkListbox->Render(pkRenderer);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetScrollbarSkin
// Description: 
//
void ZGuiCombobox::SetScrollbarSkin(ZGuiSkin* pkSkinScrollArea,
									ZGuiSkin* pkSkinThumbButton,
								    ZGuiSkin* pkSkinThumbButtonHighLight)
{
	m_pkListbox->SetScrollbarSkin(pkSkinScrollArea,pkSkinThumbButton,
		pkSkinThumbButtonHighLight);
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetSkin
// Description: 
//
void ZGuiCombobox::SetSkin(ZGuiSkin* pkSkin)
{
	//m_pkLabel->SetSkin(pkSkin);
	//m_pkListbox->SetSkin(pkSkin);
	m_pkSkin = pkSkin;
}

///////////////////////////////////////////////////////////////////////////////
// Name: Notify
// Description: 
//
bool ZGuiCombobox::Notify(ZGuiWnd* pkWnd,int iCode)
{
	if(iCode == NCODE_CLICK_DOWN)
	{
		ZGuiListitem* pkSelItem = m_pkListbox->GetSelItem();
 
		if(pkSelItem && m_bIsMenu == false)
			m_pkLabel->SetText(pkSelItem->GetText());

		if(m_pkListbox->IsVisible())
		{
			if(m_bIsMenu)
				m_pkListbox->SelNone();
			
			m_pkListbox->Hide();
	
			Resize(m_pkLabel->GetScreenRect().Width(),
				m_pkLabel->GetScreenRect().Height());

			// Send a message to the main winproc...
			int* piParams = new int[2];
			piParams[0] = GetID(); // Listbox ID
			if(pkSelItem != NULL)
			{
				piParams[1] = pkSelItem->GetIndex(); // list item ID
				GetGUI()->GetActiveCallBackFunc()(
					GetGUI()->GetActiveMainWnd(),ZGM_CBN_SELENDOK,
					2,piParams);
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

///////////////////////////////////////////////////////////////////////////////
// Name: AddItem
// Description: 
//
bool ZGuiCombobox::AddItem(char* strText,int iIndex,bool bSelect)
{
	if(iIndex == -1)
		iIndex = m_pkListbox->GetItemCount()+1;

	bool bSelectItem = bSelect && !m_bIsMenu;

	m_pkListbox->AddItem(strText,iIndex,bSelectItem);

	if(m_bIsMenu)
	{
		Resize(m_pkListbox->GetScreenRect().Width(), 
			m_pkListbox->GetItemCount()*m_pkListbox->GetItemHeight()); 
	}

	if(bSelectItem)
	{
		if(m_pkListbox->GetSelItem())
			SetLabelText(m_pkListbox->GetSelItem()->GetText());
		else
			SetLabelText(" ");
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: RemoveSelItem
// Description: 
//
void ZGuiCombobox::RemoveSelItem(bool bSelectPrev)
{
	bool bSelectPrevItem = bSelectPrev && !m_bIsMenu;

	ZGuiListitem* pkSelItem = m_pkListbox->GetSelItem();
	m_pkListbox->RemoveItem(pkSelItem,bSelectPrevItem);
	if(m_pkListbox->GetSelItem())
		SetLabelText(m_pkListbox->GetSelItem()->GetText());
	else
		SetLabelText(" ");
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetLabelText
// Description: 
//
void ZGuiCombobox::SetLabelText(char* strText)
{ 
	m_pkLabel->SetText(strText,m_bIsMenu); 
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetLabelWidth
// Description: 
//
void ZGuiCombobox::SetLabelWidth(int iWidth)
{ 
	m_pkLabel->Resize(iWidth,m_pkLabel->GetScreenRect().Height());
}

///////////////////////////////////////////////////////////////////////////////
// Name: IsMenu
// Description: 
//
void ZGuiCombobox::IsMenu(bool bIsMenu)
{
	m_bIsMenu = bIsMenu;
	m_pkListbox->IsMenu(bIsMenu);
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetNumVisibleRows
// Description: 
//
void ZGuiCombobox::SetNumVisibleRows(unsigned short iNumVisibleRows)
{ 
	m_unNumVisibleRows = iNumVisibleRows;
	
	int iNewHeight = m_unNumVisibleRows*m_pkListbox->GetItemHeight();
	m_pkListbox->Resize(m_pkListbox->GetScreenRect().Width(),iNewHeight);

	ZGuiScrollbar* pkScrollbar = m_pkListbox->GetScrollbar();
	if(pkScrollbar)
		pkScrollbar->Resize(pkScrollbar->GetScreenRect().Width(),
			iNewHeight);
}

///////////////////////////////////////////////////////////////////////////////
// Name: RemoveAllItems
// Description: 
//
bool ZGuiCombobox::RemoveAllItems()
{
	m_pkListbox->RemoveAllItems();
	
	if(m_bIsMenu == false)
		m_pkLabel->SetText("");
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: GetWndSkinsDesc
// Description: 
//
void ZGuiCombobox::GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const
{
	pkSkinDesc.push_back( SKIN_DESC(m_pkSkin,string("Combobox")) );

	int iStart = pkSkinDesc.size(); 
	m_pkLabel->GetWndSkinsDesc(pkSkinDesc);
	m_pkListbox->GetWndSkinsDesc(pkSkinDesc);
	for(unsigned int i=iStart; i<pkSkinDesc.size(); i++)
		pkSkinDesc[i].second.insert(0,"Combobox: ");
}

///////////////////////////////////////////////////////////////////////////////
// Name: Resize
// Description: 
//
void ZGuiCombobox::Resize(int iWidth,int iHeight,bool bChangeMoveArea)
{
	iHeight = GetScreenRect().Height(); // dont allow vertcal resize

	if(m_bIsMenu == false) // The label of a menu shall not be resized.
		m_pkLabel->Resize(iWidth,m_pkListbox->GetItemHeight());

	ZGuiWnd::Resize(iWidth,iHeight,bChangeMoveArea);

	if(m_bIsMenu)
	{
		m_pkListbox->Resize(
			m_pkListbox->GetScreenRect().Width(),
			m_pkListbox->GetItemCount()*
			m_pkListbox->GetItemHeight());
	}
}

void ZGuiCombobox::CopyNonUniqueData(const ZGuiWnd* pkSrc)
{
	if(pkSrc && typeid(*pkSrc)==typeid(ZGuiCombobox))
	{
		m_pkLabel->CopyNonUniqueData( ((ZGuiCombobox*)pkSrc)->m_pkLabel );
		m_pkListbox->CopyNonUniqueData( ((ZGuiCombobox*)pkSrc)->m_pkListbox );
	}

	ZGuiWnd::CopyNonUniqueData(pkSrc);
}