// Listbox.cpp: implementation of the Listbox class.
//
//////////////////////////////////////////////////////////////////////

#include "zguilistbox.h"
#include "../../render/zguirenderer.h"
#include "zguiscrollbar.h"
#include "zguibutton.h"
#include "zgui.h"

// Static internal IDs for the scrollbars
const int VERT_SCROLLBAR_ID = 620;
const int HORZ_SCROLLBAR_ID = 621;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiListbox::ZGuiListbox(Rect kRectangle, ZGuiWnd* pkParent, bool bVisible, int iID, int iItemHeight,
						 ZGuiSkin* pkTextSkin, int iTextureMask, ZGuiSkin* pkSkinItem, 
						 ZGuiSkin* pkSkinItemSelected, ZGuiSkin* pkSkinItemHighLight) :
	ZGuiControl(kRectangle, pkParent, bVisible, iID)
{
	m_unTopItem = 0;
	kRectangle.Bottom = 50;

	m_pkSelectedItem = NULL;
	m_pkSkinItemFont = pkTextSkin;
	m_pkSkinItem = pkSkinItem;
	m_pkSkinItemSelected = pkSkinItemSelected;
	m_pkSkinItemHighLight = pkSkinItemHighLight;

	SetTextSkin(pkTextSkin, iTextureMask);

	m_unItemHeight = iItemHeight;

	int w = GetWndRect().Width();
	int h = GetWndRect().Height();// h=m_unItemHeight*10;//GetWndRect().Height() % m_unItemHeight;
	Resize(w,h);

	CreateInternalControls();

	m_kItemArea = GetWndRect();

	if(m_pkScrollbarVertical != NULL)
		m_kItemArea.Right -= m_pkScrollbarVertical->GetWndRect().Width(); 
/*	if(m_pkScrollbarHorizontal != NULL)
		m_kItemArea.Bottom -= m_pkScrollbarHorizontal->GetWndRect().Height(); */
}

ZGuiListbox::~ZGuiListbox()
{

}

bool ZGuiListbox::Render( ZGuiRender* pkRenderer )
{
	if(!IsVisible())
		return true;

	if(m_iBkMaskTexture > 0)
		pkRenderer->SetMaskTexture(m_iBkMaskTexture);

	if(m_iTextMaskTexture > 0)
		pkRenderer->SetMaskTexture(m_iTextMaskTexture);

	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect(),(m_iBkMaskTexture > 0)); 
	pkRenderer->RenderBorder(GetScreenRect(),(m_iBkMaskTexture > 0)); 

	m_pkScrollbarHorizontal->Render( pkRenderer );
	m_pkScrollbarVertical->Render( pkRenderer );

	if(m_pkSelectedItem)
		m_pkSelectedItem->Select();

	// Render childrens
	for( WINit w = m_kChildList.begin();
		 w != m_kChildList.end(); w++)
		 {
			 Rect item = (*w)->GetWndRect();

			 if(item.Top + m_unItemHeight < m_kItemArea.Height() &&
				 item.Top >= m_kItemArea.Top-20)
				(*w)->Render( pkRenderer );
		 }

	return true;
}

void ZGuiListbox::SetItemNormalSkin(ZGuiSkin* pkSkin)
{
	m_pkSkinItem = pkSkin;
}

void ZGuiListbox::SetItemSelectedSkin(ZGuiSkin* pkSkin)
{
	m_pkSkinItemSelected = pkSkin;
}

void ZGuiListbox::SetItemHighLightSkin(ZGuiSkin* pkSkin)
{
	m_pkSkinItemHighLight = pkSkin;
}

void ZGuiListbox::SetScrollbarSkin(ZGuiSkin* pkSkinScrollArea, ZGuiSkin* pkSkinThumbButton, 
								   ZGuiSkin* pkSkinThumbButtonHighLight)
{
	m_pkScrollbarVertical->SetSkin(pkSkinScrollArea);
	m_pkScrollbarVertical->SetThumbButtonSkins(pkSkinThumbButton, pkSkinThumbButtonHighLight);

	m_pkScrollbarHorizontal->SetSkin(pkSkinScrollArea);
	m_pkScrollbarHorizontal->SetThumbButtonSkins(pkSkinThumbButton, pkSkinThumbButtonHighLight);
}

void ZGuiListbox::CreateInternalControls()
{
	Rect rc = GetWndRect();
	int x = rc.Width()-20;
	int y = 0;
	int w = 20;
	int h = rc.Height()-20;

	m_pkScrollbarVertical = new ZGuiScrollbar(Rect(x,y,x+w,y+h),
		this,true,VERT_SCROLLBAR_ID); 

	m_pkScrollbarVertical->SetScrollInfo(0,0,1.0f,0); 

	x = 0;
	y = rc.Height()-20;
	w = rc.Width()-20;
	h = 20;

	m_pkScrollbarHorizontal = new ZGuiScrollbar(Rect(x,y,x+w,y+h),
		this,true,HORZ_SCROLLBAR_ID);

	m_pkScrollbarHorizontal->SetScrollInfo(0,0,1.0f,0); 
}

bool ZGuiListbox::AddItem(char* strText, unsigned int iID)
{
	ZGuiListitem* pkNewItem = new ZGuiListitem(this, strText, iID, 
		m_pkSkinItem, m_pkSkinItemSelected, m_pkSkinItemHighLight, 
		m_pkSkinItemFont, m_iTextMaskTexture);

	m_pkItemList.push_back(pkNewItem);

	int iElements = m_pkItemList.size();
	int y = (iElements - 1) * m_unItemHeight; 
	m_pkItemList.back()->SetPos(0, y);
	m_pkItemList.back()->Resize(m_kItemArea.Width(), m_unItemHeight);

	// Får alla elementen plats? Nehe, hur många för mycket är det då?
	int iElementSize = m_unItemHeight * iElements;
	int iListboxSize = GetWndRect().Height() - m_unItemHeight; // lägg på lite extra för att slippa avrundingsfel...
	float fThumbSize = (float) iListboxSize / (float) iElementSize;

	m_pkScrollbarVertical->SetScrollInfo(0,iElements,fThumbSize,0); 
	return true;
}

bool ZGuiListbox::Notify(ZGuiWnd* pkWnd, int iCode)
{
	if(pkWnd->GetID() == VERT_SCROLLBAR_ID)
	{
		ScrollItems(m_pkScrollbarVertical);
		return true;
	}
	else
	if(pkWnd->GetID() == HORZ_SCROLLBAR_ID)
	{
		ScrollItems(m_pkScrollbarHorizontal);
		return true;
	}

	if(iCode == NCODE_CLICK_DOWN)
	{
		list<ZGuiListitem*>::iterator it;
		for( it = m_pkItemList.begin();
			 it != m_pkItemList.end(); it++)
			 {
				 if(pkWnd->GetID() == (*it)->GetID())
				 {
					// Change skin on previus selected button
					if(m_pkSelectedItem)
						m_pkSelectedItem->Deselect();

					// Select new item
					m_pkSelectedItem = (*it);

					// Send a scroll message to the main winproc...
					int* piParams = new int[2];
					piParams[0] = GetID(); // Listbox ID
					piParams[1] = m_pkSelectedItem->GetID(); // list item ID
					GetGUI()->GetActiveCallBackFunc()(
						GetGUI()->GetActiveMainWnd(), ZGM_SELECTLISTITEM, 2, piParams);
					delete[] piParams;

					break;
				 }
			 }
	}

	if(m_pkParent)
		m_pkParent->Notify(this, iCode);

	return true;
}

void ZGuiListbox::ScrollItems(ZGuiScrollbar* pkScrollbar)
{
	// Move all items
	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
		 {
			 (*it)->Move(0,pkScrollbar->m_iScrollChange*(int)m_unItemHeight);
		 }	

	// Reset parameter
	pkScrollbar->m_iScrollChange = 0;
}

ZGuiListitem* ZGuiListbox::GetSelItem()
{
	return m_pkSelectedItem;
}

ZGuiListitem* ZGuiListbox::GetItem(int iID)
{
	// Move all items
	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
		 {
			 if((*it)->GetID() == iID)
				 return (*it);
		 }

	return NULL;
}




