// userpanel.cpp: implementation of the UserPanel class.
//
//////////////////////////////////////////////////////////////////////

#include "userpanel.h"
#include "zerorts.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UserPanel::UserPanel(ZeroRTS* pkZeroRts, ZGuiWndProc oMainWndProc) 
	: DlgBox( pkZeroRts->pkGui, pkZeroRts->pkInput, oMainWndProc)
{
	m_pkZeroRts = pkZeroRts;
}

UserPanel::~UserPanel()
{

}

bool UserPanel::Create(int x, int y, char* szResourceFile, char* szDlgName)
{
	m_pkDlgBox = new ZGuiWnd(Rect(0,m_pkZeroRts->m_iHeight-192,m_pkZeroRts->m_iWidth,
		m_pkZeroRts->m_iHeight), NULL,false,15424);

	ZGuiSkin* pkMainWndSkin = new ZGuiSkin(0,0,0, 0,0,0, 0, 0);
	pkMainWndSkin->m_iBkTexID = m_pkZeroRts->pkTexMan->Load("../data/textures/player_panel_rts.bmp", 0);
	pkMainWndSkin->m_iBkTexAlphaID = m_pkZeroRts->pkTexMan->Load("../data/textures/player_panel_rts_a.bmp", 0);
	m_pkDlgBox->SetSkin(pkMainWndSkin);

	m_pkGui->AddMainWindow(15424+1, m_pkDlgBox, "UserPanelDlg", m_oMainWndProc, false);

	return true;
}

bool UserPanel::DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
						 int iNumberOfParams,void *pkParams )
{
	return true;
}

bool UserPanel::OnOpen(int x, int y)
{
	return true;
}

bool UserPanel::OnClose(bool bSave)
{
	return true;
}



