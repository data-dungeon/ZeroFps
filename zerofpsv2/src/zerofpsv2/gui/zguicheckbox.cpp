// Check box.cpp: implementation of the Checkbox class.
//
///////////////////////////////////////////////////////////////////////////////

#include "zguicheckbox.h"
#include "../render/zguirenderer.h"
#include "zguilabel.h"
#include "../basic/zguifont.h"
#include "zguiradiobutton.h"
#include "zgui.h"
#include "zguiresourcemanager.h"
#include <typeinfo>

///////////////////////////////////////////////////////////////////////////////
// Name: Construction/Destruction
// Description: 
//
ZGuiCheckbox::ZGuiCheckbox(Rect kRectangle, ZGuiWnd* pkParent, 
						   bool bVisible, int iID) :
	ZGuiWnd(kRectangle, pkParent, bVisible, iID)
{
	m_bChecked = false;
	m_bEnabled = true;

	m_pkLabel = new ZGuiLabel(Rect(0, 0, kRectangle.Width(), 
		kRectangle.Height()), this, true, 0);
	m_pkLabel->RemoveWindowFlag(WF_CANHAVEFOCUS);
	m_pkLabel->Enable();
	m_pkLabel->Move(kRectangle.Width()+2,0);
   m_pkLabel->Disable(); 
	RemoveWindowFlag(WF_TOPWINDOW); // kan inte användas som mainwindow
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
	//if(m_pkFont)
	//{
	//	m_pkLabel->SetFont(m_pkFont);
	//	//pkRenderer->SetFont(m_pkFont);
	//}

	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->SetClipperArea(m_kClipperArea);  
	pkRenderer->RenderQuad(GetScreenRect());

	if(m_pkSkin)
		pkRenderer->RenderBorder(GetScreenRect().Contract(m_pkSkin->m_unBorderSize));

	m_pkLabel->Render(pkRenderer); 
	
	return false;
}

void ZGuiCheckbox::SetFont(ZGuiFont* pkFont)
{
	m_pkFont = pkFont;
	m_pkLabel->SetFont(m_pkFont);
	m_pkLabel->Resize(-1, m_pkFont->m_iRowHeight+1);
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

		ZGui* pkGUI = GetGUI();

		if(m_pkParent && pkGUI) // Lade till 9 nov 2004 för att controllers på en tabctrl inte får msg annars.
		{
			int* pkParams = new int[2];
			pkParams[0] = GetID(); // control id
			pkParams[1] = false; // control id
			pkGUI->GetActiveCallBackFunc()(m_pkParent, ZGM_COMMAND,2,pkParams);
			delete[] pkParams;
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
//	ZGui* pkGui = GetGUI();
	if(!m_pkFont && m_pkResMan)
		m_pkFont = m_pkResMan->Font("defguifont");
	
	m_pkLabel->SetText(szText, true);
	m_pkLabel->Enable();
}

void ZGuiCheckbox::SetTextColor(unsigned char ucR, unsigned char ucG, 
										unsigned char ucB)
{
	m_pkLabel->SetTextColor(ucR, ucG, ucB);
	m_afTextColor[0] = ucR;
	m_afTextColor[1] = ucG;
	m_afTextColor[2] = ucB;
}

void ZGuiCheckbox::GetTextColor(unsigned char& rucR, 
										  unsigned char& rucG, unsigned char& rucB)
{
	m_pkLabel->GetTextColor(rucR, rucG, rucB);
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
	pkSkinDesc.push_back(SKIN_DESC((ZGuiSkin**)&m_pkSkinBnUp,
		string("Checkbox: Button up")));
	pkSkinDesc.push_back(SKIN_DESC((ZGuiSkin**)&m_pkSkinBnDown,
		string("Checkbox: Button down")));

	int iStart = pkSkinDesc.size(); 
	m_pkLabel->GetWndSkinsDesc(pkSkinDesc);
	for(unsigned int i=iStart; i<pkSkinDesc.size(); i++)
		pkSkinDesc[i].second.insert(0, "Checkbox: ");
}

void ZGuiCheckbox::Resize(int iWidth, int iHeight, bool bChangeMoveArea)
{
	if(iWidth == -1) iWidth = GetScreenRect().Width();
	if(iHeight == -1) iHeight = GetScreenRect().Height();

//	iHeight = GetScreenRect().Height(); // dont allow vertcal resize
//	iWidth = GetScreenRect().Width(); // dont allow horizontal resize

	ZGuiWnd::Resize(iWidth, iHeight, bChangeMoveArea);

	int font_height = 8;
	if(m_pkFont)
		font_height = m_pkFont->m_iRowHeight;

	int x = iWidth + 2;
	int y = iHeight/2-font_height/2;
	m_pkLabel->SetPos(x,y,false,true); 
}

void ZGuiCheckbox::CopyNonUniqueData(const ZGuiWnd* pkSrc)
{
	if(pkSrc && typeid(*pkSrc)==typeid(ZGuiCheckbox))
	{
		m_pkLabel->CopyNonUniqueData( ((ZGuiCheckbox*)pkSrc)->m_pkLabel );
	}

	ZGuiWnd::CopyNonUniqueData(pkSrc);
}

//bool ZGuiCheckbox::Rescale(int iOldWidth, int iOldHeight, int iNewWidth, int iNewHeight)
//{
//	ZGuiWnd::Rescale(iOldWidth, iOldHeight, iNewWidth, iNewHeight);
//	m_pkLabel->Rescale(iOldWidth, iOldHeight, iNewWidth, iNewHeight);
//	
//	return true;
//}














