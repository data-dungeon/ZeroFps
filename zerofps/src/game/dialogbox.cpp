// DlgBox.cpp: implementation of the DlgBox class.
//
//////////////////////////////////////////////////////////////////////

#include "dialogbox.h"

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//
DlgBox::DlgBox(ZGui* pkGui, ZGuiWndProc oMainWndProc)
{
	m_pkGui = pkGui;
	m_pkGuiMan = m_pkGui->GetResMan();
	m_oMainWndProc = oMainWndProc;
}

DlgBox::~DlgBox()
{

}

int DlgBox::Width()
{
	return m_pkDlgBox->GetScreenRect().Width();
}

int DlgBox::Height()
{
	return m_pkDlgBox->GetScreenRect().Height();
}

bool DlgBox::IsOpen()
{
	if(m_pkDlgBox)
		return m_pkDlgBox->IsVisible();
	
	return false;
}

void DlgBox::SetPos(int x, int y)
{
	m_pkDlgBox->SetPos(x,y,true,true);
}

void DlgBox::CenterCursor()
{
	int mx = m_pkDlgBox->GetScreenRect().Left + m_pkDlgBox->GetScreenRect().Width() / 2;
	int my = m_pkDlgBox->GetScreenRect().Top + m_pkDlgBox->GetScreenRect().Height() / 2;
	m_pkGui->ShowCursor(true,mx,my);
}
