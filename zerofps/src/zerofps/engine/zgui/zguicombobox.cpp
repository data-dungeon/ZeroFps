// Combobox.cpp: implementation of the Combobox class.
//
//////////////////////////////////////////////////////////////////////

#include "zguicombobox.h"
#include "zguilabel.h"
#include "zguilistbox.h"

const int COMBOBOX_LABEL_ID = 800;
const int LISTBOX_ID = 801;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiCombobox::ZGuiCombobox(Rect kRectangle, ZGuiWnd* pkParent, bool bVisible, int iID, int iItemHeight,
						 ZGuiSkin* pkTextSkin, int iFontTextureMask, ZGuiSkin* pkSkinItem, 
						 ZGuiSkin* pkSkinItemSelected, ZGuiSkin* pkSkinItemHighLight,
						 ZGuiSkin* pkTopItemSkin, int iTopItemTextureMask) :
	ZGuiControl(kRectangle, pkParent, bVisible, iID)
{
	Rect rc = GetWndRect();
	rc.Top = 0;
	rc.Bottom = iItemHeight;
	
	m_pkLabel = new ZGuiLabel(rc,this,true,COMBOBOX_LABEL_ID);
	m_pkLabel->SetText("Hello!");
	m_pkLabel->SetSkin(pkTopItemSkin, iTopItemTextureMask);
	m_pkLabel->SetTextSkin(pkTextSkin,iFontTextureMask); 

	rc.Top = iItemHeight;
	rc.Bottom = rc.Top+iItemHeight*5;

	m_pkListbox = new ZGuiListbox(rc,this,true,LISTBOX_ID,iItemHeight,
		pkTextSkin, iFontTextureMask, pkSkinItem, pkSkinItemSelected, pkSkinItemHighLight);
	//m_pkListbox->SetSkin(pkSkinItem);
	//pkListbox->SetScrollbarSkin(bk2, bn1_upp, bn1_hlight);

/*	rc = GetScreenRect(); 
	rc.Top +=20;
	m_pkListbox->SetMoveArea(rc,true);*/

	m_pkListbox->Hide();

	CreateInternalControls();
}

ZGuiCombobox::~ZGuiCombobox()
{

}

bool ZGuiCombobox::Render( ZGuiRender* pkRenderer )
{
	m_pkLabel->Render(pkRenderer);
	m_pkListbox->Render(pkRenderer);
	return true;
}

void ZGuiCombobox::CreateInternalControls()
{
/*	Rect rc = GetWndRect();
	int x = 0;
	int y = 0;
	int w = rc.Width()-20;
	int h = 20;
	m_pkLabel = new ZGuiLabel(rc,this,true,COMBOBOX_LABEL_ID);*/

/*	Rect rc = GetWndRect();
	int x = rc.Width()-20;
	int y = 0;
	int w = 20;
	int h = rc.Height()-20;

	m_pkScrollbarVertical = new ZGuiScrollbar(Rect(x,y,x+w,y+h),
		this,true,VERT_SCROLLBAR_ID);

	x = 0;
	y = rc.Height()-20;
	w = rc.Width()-20;
	h = 20;

	m_pkScrollbarHorizontal = new ZGuiScrollbar(Rect(x,y,x+w,y+h),
		this,true,HORZ_SCROLLBAR_ID);*/
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
 
		if(pkSelItem)
			m_pkLabel->SetText(pkSelItem->GetText());

		m_pkListbox->IsVisible() ? m_pkListbox->Hide() : m_pkListbox->Show();
	}

	return true;
}

bool ZGuiCombobox::AddItem(char* strText, unsigned int iID)
{
	return m_pkListbox->AddItem(strText, iID);
}