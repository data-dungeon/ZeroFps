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
