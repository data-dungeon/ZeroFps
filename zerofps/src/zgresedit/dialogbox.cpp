// DlgBox.cpp: implementation of the DlgBox class.
//
//////////////////////////////////////////////////////////////////////

#include "dialogbox.h"

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//
DlgBox::DlgBox(GuiBuilder* pkGuiBuilder, ZGuiWndProc oMainWndProc)
{
	m_pkGuiBuilder = pkGuiBuilder;
	m_pkGui = pkGuiBuilder->m_pkGui;
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
