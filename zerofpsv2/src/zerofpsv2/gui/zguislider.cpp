// zguislider.cpp: implementation of the ZGuiSlider class.
//
//////////////////////////////////////////////////////////////////////

#include "zgui.h"
#include "zguibutton.h"
#include "zguilabel.h"
#include "zguislider.h"

static int ID_SLIDER = 433;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiSlider::ZGuiSlider(Rect kRect, ZGuiWnd* pkParent, bool bVisible, int iID, 
					   int iMin, int iMax, unsigned long ulStyle)
					   : ZGuiWnd(kRect, pkParent, bVisible, iID)
{
//	ZGuiButton* m_pkButton = NULL;
//	ZGuiLabel* m_pkLabel  = NULL;
	m_uiCtrlStyle = ulStyle;
	m_iMin = iMin;
	m_iMax = iMax;
	m_iPos = 0;

	CreateInternalControls();
}

ZGuiSlider::~ZGuiSlider()
{

}

void ZGuiSlider::CreateInternalControls()
{
	int iHeight = GetScreenRect().Height();
	int iWidth  = GetScreenRect().Width();

//	int iSliderHeight = iHeight*5;
//	int iSliderWidth  = iHeight*2;

	Rect rcBkArea, rcSlider, rcSliderMoveArea = GetScreenRect();

	if(GetCtrlStyle(SCF_HORZ)) // horizontal
	{
		if(GetCtrlStyle(SCF_SLIDER_CENTER)) // centrerad
		{
			int iBkAreaHeight = iHeight / 3;
			rcBkArea.Left = 0;
			rcBkArea.Right = iWidth;
			rcBkArea.Top = 0;//iHeight/2 - iBkAreaHeight/2;
			rcBkArea.Bottom = iHeight;//iHeight/2 + iBkAreaHeight/2;

			rcSlider.Left = 0;
			rcSlider.Right = iHeight;//iHeight/2;
			rcSlider.Top = 0;
			rcSlider.Bottom = iHeight;

			rcSliderMoveArea.Left += 20;
			rcSliderMoveArea.Right -= 20;
		}
		else
		if(GetCtrlStyle(SCF_SLIDER_BOTTOM)) // markören är under labeln
		{
			int iBkAreaHeight = iHeight / 3;
			rcBkArea.Left = 0;
			rcBkArea.Right = iWidth;
			rcBkArea.Top = 0;
			rcBkArea.Bottom = iBkAreaHeight;

			rcSlider.Left = 0;
			rcSlider.Right = iHeight-iBkAreaHeight;
			rcSlider.Top = iBkAreaHeight;
			rcSlider.Bottom = iHeight;

			rcSliderMoveArea.Top += iBkAreaHeight;
		}
	}

	m_pkButton = new ZGuiButton(rcSlider,this,true,ID_SLIDER);
	m_pkLabel = new ZGuiLabel(rcBkArea,this,true,0);
	
	m_pkButton->SetMoveArea(rcSliderMoveArea);

	m_pkButton->m_bUseAlhpaTest = false; // låt inte markören få alpha test...
}

void ZGuiSlider::SetPos(int iPos, bool bRedraw, bool bUpdateBuddy)
{
	m_iPos = iPos;

	ZGui* pkGui = GetGUI();
	if(pkGui)
	{
		callbackfunc cb = pkGui->GetActiveCallBackFunc();
		if(cb)
		{
			ZGuiWnd* pkActivMainWnd = GetParent(); //pkGui->GetActiveMainWnd();

			if(pkActivMainWnd)
			{	
				if(bRedraw == true)
				{
					int* piParams = new int[2];
					piParams[0] = GetID();
					piParams[1] = m_iPos;
					cb(pkActivMainWnd, ZGM_SCN_SETPOS, 2, piParams);
					delete[] piParams;
				}

				// Update buddy windows.
				if(bUpdateBuddy==true)
				{
					char szText[25];
					sprintf(szText,"%i", m_iPos);
					for(vector<ZGuiWnd*>::iterator it=m_akBuddys.begin(); 
						it != m_akBuddys.end(); it++)
						(*it)->SetText(szText);
				}

				if(/*bRedraw == true*/1)
				{
					int iWidth  = GetScreenRect().Width();
//					int iBnHeight = m_pkButton->GetScreenRect().Height();
					int iBnWidth  = m_pkButton->GetScreenRect().Width();

					float fLength = (float) (iWidth-iBnWidth);
					float fProcentOffLength = (float) m_iPos / (float) (m_iMax-m_iMin);
					m_pkButton->SetPos((int) (fProcentOffLength * fLength),0);
				}
			}
		}
	}
}

void ZGuiSlider::SetRange(int iMin, int iMax)
{
	m_iMin = iMin;
	m_iMax = iMax;
}

int ZGuiSlider::GetPos()
{
	return m_iPos;
}

void ZGuiSlider::GetRange(int& riMin, int& riMax)
{
	riMin = m_iMin;
	riMax = m_iMax;
}

void ZGuiSlider::SetSliderSkin(ZGuiSkin* pkSkin)
{
	m_pkButton->SetButtonHighLightSkin(pkSkin);
	m_pkButton->SetButtonDownSkin(pkSkin);
	m_pkButton->SetButtonUpSkin(pkSkin);
}

void ZGuiSlider::SetBkSkin(ZGuiSkin* pkSkin)
{
	m_pkLabel->SetSkin(pkSkin);
}

ZGuiSkin* ZGuiSlider::GetBkSkin()
{
	return m_pkLabel->GetSkin();
}

bool ZGuiSlider::ProcessKBInput(int nKey)
{
	return true;
}

void ZGuiSlider::SetCtrlStyle(unsigned long ulNewStyle) // replaces the previus
{
	m_uiCtrlStyle = ulNewStyle;
}

bool ZGuiSlider::GetCtrlStyle(unsigned long ulValue)
{
	return ((m_uiCtrlStyle & ulValue) == ulValue) ? true : false;
}

bool ZGuiSlider::Notify(ZGuiWnd* pkWnd, int iCode)
{
	if(iCode == NCODE_MOVE)
	{
		if(pkWnd->GetID() == (unsigned int) ID_SLIDER)
		{
			Rect rcArea = GetScreenRect();
			Rect rcButton = m_pkButton->GetScreenRect();

			int width = rcArea.Width(), size = rcButton.Width() + m_iMin;
			int pos = rcButton.Left -  rcArea.Left;

			int prev_pos = m_iPos;
			int new_pos = (int) ( (float) (m_iMax-m_iMin-m_iMin) * (float) 
				( (float) pos/(width-size) ) );

			if(prev_pos != new_pos)
			{
				SetPos(m_iMin+new_pos, true);
			}
		}
	}
	return true;
}

ZGuiButton* ZGuiSlider::GetButton()
{ 
	return m_pkButton; 
}

void ZGuiSlider::AddBuddyWindow(ZGuiWnd *pkWnd)
{
	m_akBuddys.push_back(pkWnd);
}

void ZGuiSlider::RemoveBuddyWindow(ZGuiWnd *pkWnd)
{
	for(vector<ZGuiWnd*>::iterator it=m_akBuddys.begin(); 
		it != m_akBuddys.end(); it++)
		if((*it) == pkWnd)
		{
			m_akBuddys.erase(it); 
			return;
		}
}

void ZGuiSlider::RemoveAllBuddys()
{
	m_akBuddys.clear(); 
}

//bool ZGuiSlider::Rescale(int iOldWidth, int iOldHeight, int iNewWidth, int iNewHeight)
//{
//	ZGuiWnd::Rescale(iOldWidth, iOldHeight, iNewWidth, iNewHeight);
//
//	m_pkButton->Rescale(iOldWidth, iOldHeight, iNewWidth, iNewHeight);	
//	m_pkLabel->Rescale(iOldWidth, iOldHeight, iNewWidth, iNewHeight);	
//	return true;
//}

void ZGuiSlider::GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const
{
	int iStart = pkSkinDesc.size(); 
	m_pkLabel->GetWndSkinsDesc(pkSkinDesc);
	m_pkButton->GetWndSkinsDesc(pkSkinDesc);
	for(unsigned int i=iStart; i<pkSkinDesc.size(); i++)
		pkSkinDesc[i].second.insert(0, "Slider: ");
}








