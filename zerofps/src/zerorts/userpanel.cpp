// userpanel.cpp: implementation of the UserPanel class.
//
//////////////////////////////////////////////////////////////////////

#include "userpanel.h"
#include "zerorts.h"
#include "resource_id.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UserPanel::UserPanel(ZeroRTS* pkZeroRts, ZGuiWndProc oMainWndProc) 
	: DlgBox( pkZeroRts->pkGui, pkZeroRts->pkInput, oMainWndProc)
{
	m_pkZeroRts = pkZeroRts;
	m_pkGuiBuilder = pkZeroRts->m_pkGuiBuilder;
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

	ZGuiWnd* pkMinimap = new ZGuiWnd(Rect(10,9,10+172,10+172),m_pkDlgBox,true,751489);
	ZGuiSkin* pkSkin = new ZGuiSkin(255,255,255, 0,0,0, 0, 0);
	pkSkin->m_iBkTexID = m_pkZeroRts->pkTexMan->Load("../data/textures/minimap.bmp", 0);
	pkMinimap->SetSkin(pkSkin);
	pkMinimap->SetZValue(121212);

	int id = ID_CMDBN1;
	Rect rc(m_pkZeroRts->m_iWidth-48-10,10,0,0);
	rc.Right = rc.Left + 48; rc.Bottom = rc.Top + 48;

	char* strNames[] = 
	{
		"attack_bnu", "attack_bnd", "attack_bnf",
		"build_bnu", "build_bnd", "build_bnf",
		"move_bnu", "move_bnd", "move_bnf",
		"stop_bnu", "stop_bnd", "stop_bnf",
		"repair_bnu", "repair_bnd", "repair_bnf",
		"guard_bnu", "guard_bnd", "guard_bnf",
	};

	for(int i=0; i<(sizeof(strNames) / sizeof(strNames[1]))/3; i++)
	{
		ZGuiButton* pkButton = new ZGuiButton(rc,m_pkDlgBox,true,id++);
		pkButton->SetMoveArea(pkButton->GetScreenRect());
		pkButton->SetButtonUpSkin(m_pkGuiBuilder->GetSkin(strNames[i*3]));
		pkButton->SetButtonDownSkin(m_pkGuiBuilder->GetSkin(strNames[i*3+1]));
		pkButton->SetButtonHighLightSkin(m_pkGuiBuilder->GetSkin(strNames[i*3+2]));
		m_akCommandBns.push_back(pkButton);
		rc = rc.Move(-50,0);
	}

	m_pkGui->AddMainWindow(15424+1, m_pkDlgBox, "UserPanelDlg", m_oMainWndProc, false);

	m_pkDlgBox->SortChilds(); 



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



