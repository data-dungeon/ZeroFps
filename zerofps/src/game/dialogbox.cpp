// DlgBox.cpp: implementation of the DlgBox class.
//
//////////////////////////////////////////////////////////////////////

#include "dialogbox.h"
#include "../zerofps/engine/input.h"

stack<DlgBox*> DlgBox::s_pkOpenStack;

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//
DlgBox::DlgBox(ZGui* pkGui, Input* pkInput, ZGuiWndProc oMainWndProc)
{
	m_pkGui = pkGui;
	m_pkInput = pkInput;
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
	if(x < 0) x = 0; 
	if(y < 0) y = 0;

	m_pkDlgBox->SetPos(x,y,true,true);
}

void DlgBox::GetPos(int& x, int& y)
{
	x = m_pkDlgBox->GetScreenRect().Left;
	y = m_pkDlgBox->GetScreenRect().Top;
}

void DlgBox::CenterCursor()
{
	int mx = m_pkDlgBox->GetScreenRect().Left + m_pkDlgBox->GetScreenRect().Width() / 2;
	int my = m_pkDlgBox->GetScreenRect().Top + m_pkDlgBox->GetScreenRect().Height() / 2;
	m_pkInput->SetCursorInputPos(mx,my);
}

void DlgBox::KillFocus()
{
	ZGuiWnd::m_pkFocusWnd = NULL;
	m_pkDlgBox->KillFocus();
}

const ZGuiWnd* DlgBox::GetWnd()
{
	return m_pkDlgBox;
}

bool DlgBox::Open(int x, int y)
{
	m_pkGui->ShowMainWindow(m_pkDlgBox, true);
	
	if(!(x==-1 && y==-1))
		m_pkDlgBox->SetPos(x,y,true,true);

	CenterCursor();

	s_pkOpenStack.push(this);

	return OnOpen(x,y);
}

bool DlgBox::Close(bool bSave)
{
	m_pkGui->ShowMainWindow(m_pkDlgBox, false);
	KillFocus();

	if(s_pkOpenStack.empty() == false)
		s_pkOpenStack.pop();

	return OnClose(bSave);
}
