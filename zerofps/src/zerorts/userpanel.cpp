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
	m_pkDlgBox = new ZGuiWnd(Rect(0,m_pkZeroRts->m_iHeight-192,
		m_pkZeroRts->m_iWidth,m_pkZeroRts->m_iHeight), NULL,false,15424);

	ZGuiSkin* pkMainWndSkin = new ZGuiSkin(0,0,0, 0,0,0, 0, 0);
	pkMainWndSkin->m_iBkTexID = 
		m_pkZeroRts->pkTexMan->Load("../data/textures/player_panel_rts.bmp", 0);
	pkMainWndSkin->m_iBkTexAlphaID = 
		m_pkZeroRts->pkTexMan->Load("../data/textures/player_panel_rts_a.bmp", 0);
	m_pkDlgBox->SetSkin(pkMainWndSkin);

	ZGuiWnd* pkMinimap = new ZGuiWnd(Rect(10,9,10+172,10+172),m_pkDlgBox,true,751489);
	pkMinimap->SetSkin(m_pkGuiBuilder->GetSkin("minimap") );
	pkMinimap->SetZValue(121212);
	m_pkGui->RegisterWindow(pkMinimap, "MinMapWnd");

	Rect rc(m_pkZeroRts->m_iWidth-48-10,10,0,0);
	rc.Right = rc.Left + 48; rc.Bottom = rc.Top + 48;

	char* aSkinNames[] = 
	{
		"attack_bnu", "attack_bnd", "attack_bnf",
		"build_bnu", "build_bnd", "build_bnf",
		"move_bnu", "move_bnd", "move_bnf",
		"stop_bnu", "stop_bnd", "stop_bnf",
		"repair_bnu", "repair_bnd", "repair_bnf",
		"guard_bnu", "guard_bnd", "guard_bnf",
	};

	char* aDescNames[] = 
	{
		"Attack",
		"Build",
		"Move",
		"Stop",
		"Repair",
		"Guard",
	};

	int aIDNumbers[] =
	{
		ID_ATTACK_BN,
		ID_BUILD_BN,
		ID_MOVE_BN,
		ID_STOP_BN,
		ID_REPAIR_BN,
		ID_GUARD_BN,
	};

	for(int i=0; i<(sizeof(aSkinNames) / sizeof(aSkinNames[1]))/3; i++)
	{
		if(i%6==0 && i!=0)
		{
			rc = Rect(m_pkZeroRts->m_iWidth-48-10,10,0,0);
			rc.Right = rc.Left + 48; rc.Bottom = rc.Top + 48;
			rc = rc.Move(0,(i/6)*50);
		}

		ZGuiButton* pkButton = new ZGuiButton(rc,m_pkDlgBox,true,aIDNumbers[i]);
		pkButton->SetMoveArea(pkButton->GetScreenRect());
		pkButton->SetButtonUpSkin(m_pkGuiBuilder->GetSkin(aSkinNames[i*3]));
		pkButton->SetButtonDownSkin(m_pkGuiBuilder->GetSkin(aSkinNames[i*3+1]));
		pkButton->SetButtonHighLightSkin(m_pkGuiBuilder->GetSkin(aSkinNames[i*3+2]));
		m_akCommandBns.push_back(pkButton);
		m_akCmdButtonText.insert(map< ZGuiButton*, string >::value_type(
			pkButton, string(aDescNames[i]))); 
		rc = rc.Move(-50,0);
	}

	ZGuiLabel* pkLabel = m_pkGuiBuilder->CreateLabel(m_pkDlgBox, 123456, 
		m_pkZeroRts->m_iWidth-310, 160, 300, 20, "", "CmdButtonTipLB");
	pkLabel->SetZValue(m_akCommandBns.back()->m_iZValue+1);

	m_pkGui->AddMainWindow(15424+1, m_pkDlgBox, "UserPanelDlg", 
		m_oMainWndProc, false);

	m_pkDlgBox->SortChilds();

	return true;
}

bool UserPanel::DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
						 int iNumberOfParams,void *pkParams )
{
	switch(uiMessage)
	{
	case ZGM_LBUTTONDOWN:
	case ZGM_RBUTTONDOWN:

		int x, y;
		x = ((int*)pkParams)[0];
		y = ((int*)pkParams)[1];
		ZGuiWnd* pkWnd = m_pkDlgBox->Find(x,y);

		m_pkGuiBuilder->Get("CmdButtonTipLB")->SetText("");

		for(unsigned int i=0; i<m_akCommandBns.size(); i++)
		{
			if(m_akCommandBns[i] == pkWnd)
			{
				map<ZGuiButton*,string>::iterator r;
				r = m_akCmdButtonText.find((ZGuiButton*)pkWnd);
				if(r != m_akCmdButtonText.end())
					m_pkGuiBuilder->Get("CmdButtonTipLB")->SetText(
						(char*)r->second.c_str());
				break;
			}
		}
		break;
	}
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


bool UserPanel::Click()
{
	int x,y;
	m_pkInput->MouseXY(x,y);

	if(m_pkDlgBox->GetScreenRect().Inside(x,y))
	{
		if( m_pkInput->Pressed(MOUSELEFT) || 
			m_pkInput->Pressed(MOUSERIGHT) )
		{
			return true;
		}
	}

	return false;
}






