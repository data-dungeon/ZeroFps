// DlgBox.cpp: implementation of the DlgBox class.
//
//////////////////////////////////////////////////////////////////////

#include "dialogbox.h"
#include "../zerofps/engine/input.h"

std::vector<DlgBox*> DlgBox::s_kTopDlgQueue;

int DlgBox::prev_x = 0;
int DlgBox::prev_y = 0;

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
	/*
	m_pkInput->MouseXY(prev_x, prev_y);

	int mx = m_pkDlgBox->GetScreenRect().Left + m_pkDlgBox->GetScreenRect().Width() / 2;
	int my = m_pkDlgBox->GetScreenRect().Top + m_pkDlgBox->GetScreenRect().Height() / 2;
	m_pkInput->SetCursorInputPos(mx,my);
	*/
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

	//CenterCursor();

	s_kTopDlgQueue.push_back(this);
	
	return OnOpen(x,y);
}

bool DlgBox::Close(bool bSave)
{
	m_pkGui->ShowMainWindow(m_pkDlgBox, false);
	KillFocus();

	if(s_kTopDlgQueue.size() > 0)
	{
		for(std::vector<DlgBox*>::iterator it = s_kTopDlgQueue.begin(); 
			it != s_kTopDlgQueue.end(); it++)
		{
			if((*it) == this)
			{
				s_kTopDlgQueue.erase(it);
				break;
			}
		}
	}

	return OnClose(bSave);
}

void DlgBox::CloseFocusDlg()
{
	if(!s_kTopDlgQueue.empty())
		s_kTopDlgQueue.back()->Close(false);
}

int DlgBox::GetNumVisibleDialogs()
{
	return s_kTopDlgQueue.size();
}
