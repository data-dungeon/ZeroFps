// Check box.cpp: implementation of the Checkbox class.
//
///////////////////////////////////////////////////////////////////////////////

#include "zguicheckbox.h"
#include "../../render/zguirenderer.h"
#include "zguilabel.h"
#include "../../basic/zguifont.h"
#include "zguiradiobutton.h"
#include "zgui.h"

///////////////////////////////////////////////////////////////////////////////
// Name: Construction/Destruction
// Description: 
//
ZGuiCheckbox::ZGuiCheckbox(Rect kRectangle, ZGuiWnd* pkParent, 
						   bool bVisible, int iID) :
	ZGuiControl(kRectangle, pkParent, bVisible, iID)
{
	m_bChecked = false;
	m_bEnabled = true;

	m_pkLabel = new ZGuiLabel(Rect(0, 0, kRectangle.Width(), 
		kRectangle.Height()), this, true, 0);
	m_pkLabel->RemoveWindowFlag(WF_CANHAVEFOCUS);
	m_pkLabel->Enable();
	m_pkLabel->Move(20,0);
}

ZGuiCheckbox::~ZGuiCheckbox()
{
	if(m_pkLabel)
	{
		ResetStaticClickWnds(m_pkLabel);
		delete m_pkLabel;
		m_pkLabel = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Name: Render
// Description: 
//
bool ZGuiCheckbox::Render( ZGuiRender* pkRenderer )
{
	if(m_pkFont)
		pkRenderer->SetFont(m_pkFont);

	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect()); 

	m_pkLabel->Render(pkRenderer); 
	return false;
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetButtonCheckedSkin
// Description: 
//
void ZGuiCheckbox::SetButtonCheckedSkin(ZGuiSkin* pkSkin)
{
	m_pkSkinBnDown = pkSkin;
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetButtonUncheckedSkin
// Description: 
//
void ZGuiCheckbox::SetButtonUncheckedSkin(ZGuiSkin* pkSkin)
{
	m_pkSkin = m_pkSkinBnUp = pkSkin;
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetButtonUncheckedSkin
// Description: 
//
void ZGuiCheckbox::SetLabelSkin(ZGuiSkin* pkSkin)
{
	m_pkLabel->SetSkin(pkSkin);
}

///////////////////////////////////////////////////////////////////////////////
// Name: Notify
// Description: 
//
bool ZGuiCheckbox::Notify(ZGuiWnd* pkWnd, int iCode)
{
	if(iCode == NCODE_CLICK_UP)
	{
		m_bChecked = !m_bChecked;

		if(m_bChecked == false)
		{
			m_pkSkin = m_pkSkinBnUp;
		}
		else
		{
			m_pkSkin = m_pkSkinBnDown;
		}
	}

	m_pkParent->Notify(this, iCode);

	return false;
}

///////////////////////////////////////////////////////////////////////////////
// Name: CheckButton
// Description: 
//
void ZGuiCheckbox::CheckButton()
{
	m_bChecked = true;
	m_pkSkin = m_pkSkinBnDown;
}
 
///////////////////////////////////////////////////////////////////////////////
// Name: UncheckButton
// Description: 
//
void ZGuiCheckbox::UncheckButton()
{
	m_bChecked = false;
	m_pkSkin = m_pkSkinBnUp;
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetText
// Description: 
//
void ZGuiCheckbox::SetText(char* szText, bool bResizeWnd)
{
	ZGui* pkGui = GetGUI();
	if(!m_pkFont && pkGui)
		m_pkFont = pkGui->GetBitmapFont(ZG_DEFAULT_GUI_FONT);
	
	m_pkLabel->SetText(szText, true);
	m_pkLabel->Enable();
}

///////////////////////////////////////////////////////////////////////////////
// Name: GetText
// Description: 
//
char* ZGuiCheckbox::GetText() 
{ 
	return m_pkLabel->GetText(); 
} 

///////////////////////////////////////////////////////////////////////////////
// Name: IsChecked
// Description: 
//
bool ZGuiCheckbox::IsChecked()
{
	return m_bChecked;
}

///////////////////////////////////////////////////////////////////////////////
// Name: GetWndSkinsDesc
// Description: 
//
void ZGuiCheckbox::GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const
{
	pkSkinDesc.push_back( SKIN_DESC(m_pkSkinBnUp, string("Checkbox: Button up")) );
	pkSkinDesc.push_back( SKIN_DESC(m_pkSkinBnDown, 
		string("Checkbox: Button down")) );

	int iStart = pkSkinDesc.size(); 
	m_pkLabel->GetWndSkinsDesc(pkSkinDesc);
	for(unsigned int i=iStart; i<pkSkinDesc.size(); i++)
		pkSkinDesc[i].second.insert(0, "Checkbox: ");
}

void ZGuiCheckbox::Resize(int iWidth, int iHeight, bool bChangeMoveArea)
{
	iHeight = GetScreenRect().Height(); // dont allow vertcal resize
	iWidth = GetScreenRect().Width(); // dont allow horizontal resize

	ZGuiWnd::Resize(iWidth, iHeight, bChangeMoveArea);
}