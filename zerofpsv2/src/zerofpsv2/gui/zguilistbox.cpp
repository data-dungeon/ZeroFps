// Listbox.cpp: implementation of the Listbox class.
//
//////////////////////////////////////////////////////////////////////

#include "zguilistbox.h"
#include "../render/zguirenderer.h"
#include "../basic/zfassert.h"
#include "zguiscrollbar.h"
#include "zguibutton.h"
#include "zgui.h"
#include "zguiresourcemanager.h"
#include <typeinfo>
#include "../basic/keys.h"

// Static internal IDs for the scrollbars
const int VERT_SCROLLBAR_ID = 620;
const int HORZ_SCROLLBAR_ID = 621;
const int FONT_SIZE = 12;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiListbox::ZGuiListbox(Rect kRectangle, ZGuiWnd* pkParent, bool bVisible, 
						 int iID, int iItemHeight, 
						 ZGuiSkin* pkSkinItem, 
						 ZGuiSkin* pkSkinItemSelected, 
						 ZGuiSkin* pkSkinItemHighLight) :
	ZGuiWnd(kRectangle, pkParent, bVisible, iID)
{
	m_iScrollbarWidth = 20;
	m_bIsMenu = false;
	m_bEnabled = true;

	m_pkSelectedItem = NULL;
	m_pkSkinBnUp = pkSkinItem;
	m_pkSkinBnDown = pkSkinItemSelected;
	m_pkSkinBnHLight = pkSkinItemHighLight;

	m_unItemHeight = iItemHeight;

	int w = GetWndRect().Width();
	int h = GetWndRect().Height();
	
	ZGuiWnd::Resize(w,h);

	CreateInternalControls();

	m_kItemArea = GetWndRect();

	//RemoveWindowFlag(WF_CANHAVEFOCUS); // fönster har focus by default
	SetWindowFlag(WF_CANHAVEFOCUS); // textboxar har focus by default
	RemoveWindowFlag(WF_TOPWINDOW); // kan inte användas som mainwindow
}

ZGuiListbox::~ZGuiListbox()
{
	for( itItemList it = m_pkItemList.begin(); it != m_pkItemList.end(); it++)
	{
		delete (*it);
		(*it) = NULL;
	}

	m_pkItemList.clear(); 

	m_pkSelectedItem = NULL;

	delete m_pkScrollbarVertical;
	m_pkScrollbarVertical = NULL;
}

bool ZGuiListbox::Render( ZGuiRender* pkRenderer )
{
	if(!IsVisible())
		return true;

	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect()); 
	pkRenderer->RenderBorder(GetScreenRect()); 

	if(m_pkScrollbarVertical->IsVisible())
		m_pkScrollbarVertical->Render( pkRenderer );

	if(m_pkSelectedItem)
	{
		if(m_bIsMenu == false)
			m_pkSelectedItem->Select();
	}

	for( itItemList it = m_pkItemList.begin(); it != m_pkItemList.end(); it++)
		(*it)->GetButton()->Render( pkRenderer );

	return true;
}

void ZGuiListbox::SetItemNormalSkin(ZGuiSkin* pkSkin)
{
	m_pkSkinBnUp = pkSkin;
}

void ZGuiListbox::SetItemSelectedSkin(ZGuiSkin* pkSkin)
{
	m_pkSkinBnDown = pkSkin;
}

void ZGuiListbox::SetItemHighLightSkin(ZGuiSkin* pkSkin)
{
	m_pkSkinBnHLight = pkSkin;
}

void ZGuiListbox::SetScrollbarSkin(ZGuiSkin* pkSkinScrollArea, ZGuiSkin* pkSkinThumbButton, 
								   ZGuiSkin* pkSkinThumbButtonHighLight,
									ZGuiSkin* pkSkinTopBnUp, ZGuiSkin* pkSkinTopBnDown,
									ZGuiSkin* pkSkinBottomBnUp, ZGuiSkin* pkSkinBottomBnDown)
{
	m_pkScrollbarVertical->SetSkin(pkSkinScrollArea);
	m_pkScrollbarVertical->SetThumbButtonSkins(pkSkinThumbButton, pkSkinThumbButtonHighLight);

	m_pkScrollbarVertical->SetScrollButtonUpSkins(pkSkinTopBnUp, pkSkinTopBnDown);
	m_pkScrollbarVertical->SetScrollButtonDownSkins(pkSkinBottomBnUp, pkSkinBottomBnDown);
}

void ZGuiListbox::CreateInternalControls()
{
	Rect rc = GetWndRect();
	int x = rc.Width()-m_iScrollbarWidth;
//	int y = 0;
	int w = m_iScrollbarWidth;
//	int h = rc.Height();

	m_pkScrollbarVertical = new ZGuiScrollbar(Rect(x,0,x+w,GetScreenRect().Height()),
		this,true,VERT_SCROLLBAR_ID); 

	m_pkScrollbarVertical->SetScrollInfo(0,0,1.0f,0); 
}

ZGuiListitem* ZGuiListbox::AddItem(char* strText, unsigned int iIndex, bool bSelect)
{
	ZGuiListitem* pkNewItem;
	pkNewItem = new ZGuiListitem(this, strText, iIndex, 
		m_pkSkinBnUp, m_pkSkinBnDown, m_pkSkinBnHLight);
	pkNewItem->GetButton()->SetGUI(GetGUI());

	if(!m_bEnabled)
		pkNewItem->GetButton()->Disable();

	m_pkItemList.push_back(pkNewItem);

	int iElements = m_pkItemList.size();
	
	if(iElements > 0)
	{
		int y = (iElements - 1) * m_unItemHeight; 
		m_pkItemList.back()->SetPos(0, y);
		m_pkItemList.back()->Resize(m_kItemArea.Width(), m_unItemHeight);
	}

	UpdateList();

//	ZGui* pkGui = GetGUI();
	if(m_pkFont == NULL && m_pkResMan)
		m_pkFont = m_pkResMan->Font("defguifont");

   SetFont(m_pkFont);

	int iWidth = GetScreenRect().Width();
	int iHeight = GetScreenRect().Height();
	int iNewWidth = m_pkFont->GetLength(strText) + 25;

	if(iNewWidth > iWidth && m_bIsMenu == true)
	{
		ZGuiWnd::Resize( iNewWidth, iHeight);
		
		// Also resize combobox if that is the parent
		ZGuiWnd* pkParent = GetParent();
		if(pkParent && typeid(*pkParent)==typeid(ZGuiCombobox))
			pkParent->Resize( iNewWidth, iHeight);

		list<ZGuiListitem*>::iterator it;
		for( it = m_pkItemList.begin();
			 it != m_pkItemList.end(); it++)
			 {
				(*it)->Resize( iNewWidth, m_unItemHeight);
			 }

		m_kItemArea.Right = m_kItemArea.Left + iWidth;
	}

	if(bSelect)
	{
		SelItem(strText);
		//m_pkSelectedItem = pkNewItem;
	}

	return pkNewItem;
}

bool ZGuiListbox::RemoveItem(ZGuiListitem* pkItemToRemove, bool bSelPrev)
{
	// Innehåller listan bara ett (1) alternativ?
	if(m_pkItemList.size() == 1)
	{
		RemoveAllItems();
		return true;
	}	

	bool bHaveFoundItem = false;
	int index_to_select = -1;

	vector<pair<string, int> > temp; // don´t forget the space after '>'!
	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
		 {
			 if((*it) != pkItemToRemove)
			 {
				 temp.push_back(pair<string, int>(string((*it)->GetText()),(*it)->GetIndex()));
			
				 if(bHaveFoundItem && index_to_select == -1)
					index_to_select = (*it)->GetIndex(); 
			 }
			 else
			 {
				 bHaveFoundItem = true;
			 }
		 }

	RemoveAllItems();

	if(bHaveFoundItem && index_to_select == -1)
		index_to_select = temp.back().second;

	for(unsigned int i=0; i<temp.size(); i++)
	{
		bool bSelect = bSelPrev && temp[i].second == index_to_select;
		AddItem((char*)temp[i].first.c_str(), i/*temp[i].second*/, bSelect); 
	}

	UpdateList();

	return true;
}

bool ZGuiListbox::RemoveAllItems()
{
	if(m_pkItemList.empty())
		return true;

	if(m_pkSelectedItem)
		ResetStaticClickWnds(m_pkSelectedItem->GetButton());

	ZGuiWnd::m_pkPrevWndClicked = NULL;

	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
		 {
			ZGuiListitem* pkListItem = (*it);
			delete pkListItem;
		 }

	m_pkItemList.clear();

	m_pkSelectedItem = NULL;
	UpdateList();

	return true;
}

bool g_bInv;

bool ZGuiListbox::Notify(ZGuiWnd* pkWnd, int iCode)
{
	if(!m_bEnabled)
		return true;

	if(pkWnd->GetID() == VERT_SCROLLBAR_ID)
	{
		if(iCode == NCODE_CLICK_DOWN)
			g_bInv =true;
		else
			g_bInv =false;

		ScrollItems(m_pkScrollbarVertical);
		return true;
	}

	if(m_bEnabled && iCode == NCODE_CLICK_DOWN)
	{
		list<ZGuiListitem*>::iterator it;
		for( it = m_pkItemList.begin();
			 it != m_pkItemList.end(); it++)
			 {
				 if(pkWnd == (*it)->GetButton())
				 {
					// Change skin on previus selected button
					if(m_pkSelectedItem)
						m_pkSelectedItem->Deselect();

					// Select new item
					m_pkSelectedItem = (*it);

					SendSelItemMsg();

					break;
				 }
			 }
	}

	if(m_pkParent)
		m_pkParent->Notify(this, iCode);

	return true;
}

/*void ZGuiListbox::ScrollItems(ZGuiScrollbar* pkScrollbar)
{
	if(!m_pkItemList.empty())
		m_unItemHeight = (*m_pkItemList.begin())->GetButton()->GetScreenRect().Height();  

	int change = pkScrollbar->m_iScrollChange*(int)m_unItemHeight;

	// Move all items
	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin(); it != m_pkItemList.end(); it++)
		 {
			ZGuiListitem* pkItem = (*it);

			if(g_bInv) // fulhack för att unvika att scrollbaren flytar items fel
				pkItem->Move(0,-change);
			else
				pkItem->Move(0,change);

			Rect rc = pkItem->GetButton()->GetScreenRect();

			if( rc.Top >= GetScreenRect().Top && rc.Bottom <= GetScreenRect().Bottom)  
				pkItem->GetButton()->Show();
			else
				pkItem->GetButton()->Hide();
		 }	

	// Reset parameter
	pkScrollbar->m_iScrollChange = 0;
}*/

void ZGuiListbox::ScrollItems(ZGuiScrollbar* pkScrollbar)
{
	if(!m_pkItemList.empty())
		m_unItemHeight = (*m_pkItemList.begin())->GetButton()->GetScreenRect().Height();  

//	int change = pkScrollbar->m_iScrollChange*(int)m_unItemHeight;
	int scroll_pos = pkScrollbar->GetPos(); 

	// Move all items
	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin(); it != m_pkItemList.end(); it++)
	 {
		ZGuiListitem* pkItem = (*it);

		Rect rc = pkItem->GetButton()->GetWndRect();
		rc.Top = (pkItem->GetIndex() * m_unItemHeight) - (scroll_pos * m_unItemHeight);
		pkItem->SetPos(rc.Left, rc.Top); 

		rc = pkItem->GetButton()->GetScreenRect();
		if( rc.Top >= GetScreenRect().Top && rc.Bottom <= GetScreenRect().Bottom)  
			pkItem->GetButton()->Show();
		else
			pkItem->GetButton()->Hide();
	 }	

	// Reset parameter
	pkScrollbar->m_iScrollChange = 0;
}

ZGuiListitem* ZGuiListbox::GetSelItem()
{
	return m_pkSelectedItem;
}

ZGuiListitem* ZGuiListbox::GetItem(unsigned int iIndex)
{
	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
		 {
			 if((*it)->GetIndex() == iIndex)
				 return (*it);
		 }

	return NULL;
}

void ZGuiListbox::IsMenu(bool bMenu)
{
	m_bIsMenu = bMenu;

	if(bMenu == true)
		m_pkScrollbarVertical->Hide();

	// must de-activte autohide because we dont
	// want the scrollbar to show it self if the
	// listbox is a menu.
	m_pkScrollbarVertical->SetAutoHide(!bMenu); 

	UpdateList();
}

void ZGuiListbox::UpdateList()
{
	int iElements = m_pkItemList.size();

	if(iElements > 0)
		m_unItemHeight = (*m_pkItemList.begin())->GetButton()->GetScreenRect().Height();
	
	// Får alla elementen plats? Nehe, hur många för mycket är det då?
	int iElementSize = m_unItemHeight * iElements;
	int iListboxSize = GetScreenRect().Height() - m_unItemHeight; // lägg på lite extra för att slippa avrundingsfel...
	if(iElementSize <= 0) iElementSize = 1; // don´t devide by zero
	float fThumbSize = (float) iListboxSize / (float) iElementSize;

	m_pkScrollbarVertical->SetScrollInfo(0,iElements,fThumbSize,0);
	
	if(fThumbSize >= 0.99f || iElements == 0)
	{
		m_pkScrollbarVertical->Hide();
	}
	else
	{
		if(m_bIsMenu == false) // don't show scrollbar if menu
			m_pkScrollbarVertical->Show();
	}

	// Resize all items
	int iNewWidth = GetScreenRect().Width();
	if(m_pkItemList.size()*m_unItemHeight > (unsigned int) GetScreenRect().Height())
		iNewWidth -= m_iScrollbarWidth;

	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
			(*it)->Resize( iNewWidth, m_unItemHeight);

	ScrollItems(m_pkScrollbarVertical);
}

void ZGuiListbox::Resize(int Width, int Height, bool bChangeMoveArea)
{
	if(Width == -1) Width = GetScreenRect().Width();
	if(Height == -1) Height = GetScreenRect().Height();

	Rect rc = GetWndRect();
	rc.Bottom = rc.Top + Height;
	rc.Right = rc.Left + Width;

	if(m_pkScrollbarVertical)
	{
		if(m_pkScrollbarVertical->IsVisible())
			rc.Right -= m_pkScrollbarVertical->GetScreenRect().Width();	
	}

	ZGuiWnd::Resize(Width, Height); 

	m_kItemArea = rc;
	
	if(m_pkScrollbarVertical)
	{
		int iScrollbarWidth = m_pkScrollbarVertical->GetScreenRect().Width();
		m_pkScrollbarVertical->SetPos(Width - iScrollbarWidth, 0, false, true);

		m_pkScrollbarVertical->Resize(iScrollbarWidth, 
			m_kItemArea.Height());
	}

	UpdateList();
}

void ZGuiListbox::SelNone()
{
	if(m_pkSelectedItem)
		m_pkSelectedItem->Deselect();

	m_pkSelectedItem = NULL;
}

void ZGuiListbox::SetZValue(int iValue)
{
	ZGuiWnd::SetZValue(iValue);
	m_pkScrollbarVertical->SetZValue(iValue-1);
}

int ZGuiListbox::GetItemCount()
{
	return m_pkItemList.size();
}

int ZGuiListbox::Find(char* strString)
{
	int counter=0;

	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
		 {
			 if(strcmp(strString, (*it)->GetText()) == 0)
				 return counter;

			 counter++;
		 }

	return -1;
}

bool ZGuiListbox::SelItem(int iIndex)
{
	if(iIndex < 0 || iIndex > (int) m_pkItemList.size()-1)
		return false;

	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
		 {
			 if((*it)->GetIndex() == (unsigned int) iIndex)
			 {
				 if(m_pkSelectedItem)
					m_pkSelectedItem->Deselect();

				 m_pkSelectedItem = (*it);

				 if(m_bIsMenu == false)
					m_pkSelectedItem->Select();

				 SendSelItemMsg();

				 //ZGuiWnd::m_pkFocusWnd = m_pkSelectedItem->GetButton();
				 break;
			 }
		 }

	UpdateList();

	return true;
}

bool ZGuiListbox::SelItem(const char* szText)
{
	if(szText == NULL)
		return false;

	bool bSuccess = false;

	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
		 {
			 if(strcmp((*it)->GetText(), szText) == 0)
			 {
				 if(m_pkSelectedItem)
					m_pkSelectedItem->Deselect();

				 m_pkSelectedItem = (*it);

				 if(m_bIsMenu == false)
					m_pkSelectedItem->Select();

					SendSelItemMsg();

				 bSuccess = true;
				 break;
			 }
		 }

	UpdateList();

	return bSuccess;
}

void ZGuiListbox::GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const
{
	pkSkinDesc.push_back( SKIN_DESC((ZGuiSkin**)&m_pkSkin, string("Listbox")) );
	pkSkinDesc.push_back( SKIN_DESC((ZGuiSkin**)&m_pkSkinBnUp, string("Listbox: Button up")) );
	pkSkinDesc.push_back( SKIN_DESC((ZGuiSkin**)&m_pkSkinBnDown, string("Listbox: Button down")) );
	pkSkinDesc.push_back( SKIN_DESC((ZGuiSkin**)&m_pkSkinBnHLight, string("Listbox: Button focus")) );

	int iStart = pkSkinDesc.size(); 
	m_pkScrollbarVertical->GetWndSkinsDesc(pkSkinDesc);
	for(unsigned int i=iStart; i<pkSkinDesc.size(); i++)
		pkSkinDesc[i].second.insert(0, "Listbox: ");
}

void ZGuiListbox::CopyNonUniqueData(const ZGuiWnd* pkSrc)
{
	if(pkSrc && typeid(*pkSrc)==typeid(ZGuiListbox))
	{
		if(((ZGuiListbox*)pkSrc)->m_pkScrollbarVertical)
			m_pkScrollbarVertical->CopyNonUniqueData( 
				((ZGuiListbox*)pkSrc)->m_pkScrollbarVertical );
	}

	ZGuiWnd::CopyNonUniqueData(pkSrc);
}

void ZGuiListbox::SetEnable(bool bEnable)
{
	m_bEnabled = bEnable;

	list<ZGuiListitem*>::iterator it;
	for(  it = m_pkItemList.begin();
			it != m_pkItemList.end(); it++)

		 {
			 if(m_bEnabled)
				 (*it)->GetButton()->Enable();  
			 else
				 (*it)->GetButton()->Disable();  
		 }
}

//bool ZGuiListbox::Rescale(int iOldWidth, int iOldHeight, int iNewWidth, int iNewHeight)
//{
//	ZGuiWnd::Rescale(iOldWidth, iOldHeight, iNewWidth, iNewHeight);
//
//	for(itItemList it = m_pkItemList.begin(); it != m_pkItemList.end(); it++)
//		(*it)->GetButton()->Rescale(iOldWidth, iOldHeight, iNewWidth, iNewHeight);
//
//	m_pkScrollbarVertical->ZGuiScrollbar::Rescale(iOldWidth, iOldHeight, iNewWidth, iNewHeight); 
//
//	m_unItemHeight = (int) ((float) m_unItemHeight * (float) ((float)iNewHeight/(float)iOldHeight));
//	m_iScrollbarWidth = (int) ((float) m_iScrollbarWidth * (float) ((float)iNewWidth/(float)iOldWidth));
//
//	int iWidth = GetScreenRect().Width();
//
//	m_kItemArea = GetWndRect();
//	m_kItemArea = m_kItemArea.Left + iWidth;
//
//	UpdateList();
//
//	return true;
//}

void ZGuiListbox::SetResizeFlags(bool bHorz, bool bVert)
{
	m_bResizeHorz = bHorz;
	m_bResizeVert = bVert;

	list<ZGuiListitem*>::iterator it;
	for(  it = m_pkItemList.begin();
			it != m_pkItemList.end(); it++)
	 {
		(*it)->GetButton()->SetResizeFlags(bHorz, bVert);
	 }
}

void ZGuiListbox::SetFont(ZGuiFont* pkFont)
{
	m_pkFont = pkFont;

	list<ZGuiListitem*>::iterator it;
	for(  it = m_pkItemList.begin(); it != m_pkItemList.end(); it++)
   {
		(*it)->GetButton()->SetFont(pkFont);
	}
}




void ZGuiListbox::SendSelItemMsg()
{
	// Send a scroll message to the main winproc...
	int* piParams = new int[2];
	piParams[0] = GetID(); // Listbox ID
	piParams[1] = m_pkSelectedItem->GetIndex(); // list item index

	ZGui* pkGui = GetGUI();
	if(pkGui == NULL)
	{
		pkGui = GetParent()->GetGUI();
		ZFAssert(pkGui, "ZGuiListbox::Notify, GetGUI = NULL");
	}

	callbackfunc cb = pkGui->GetActiveCallBackFunc();
	ZFAssert(cb, "ZGuiListbox::Notify, GetActiveCallBackFunc = NULL");

	ZGuiWnd* pkActivMainWnd = GetParent() ; //pkGui->GetActiveMainWnd();
	ZFAssert(pkActivMainWnd, "ZGuiListbox::Notify, pkGui->GetActiveMainWnd() = NULL");

	cb(pkActivMainWnd, ZGM_SELECTLISTITEM, 2, piParams);
}


bool ZGuiListbox::ProcessKBInput(int iKey)
{
	if(iKey == KEY_DOWN)
	{
		int iIndex = m_pkSelectedItem == NULL ? 0 : m_pkSelectedItem->GetIndex()+1;
		return SelItem(iIndex);
	}
	else
	if(iKey == KEY_UP)
	{
		int iIndex = m_pkSelectedItem == NULL ? 0 : m_pkSelectedItem->GetIndex()-1;
		return SelItem(iIndex);
	}

	return true;
}