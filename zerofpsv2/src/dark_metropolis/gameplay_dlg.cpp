#include "gameplay_dlg.h"

CGamePlayDlg::CGamePlayDlg() : CGameDlg(&g_kDM)
{

}

CGamePlayDlg::~CGamePlayDlg()
{
}

void CGamePlayDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	if(strClickName == "GamPlayMenuBn")
	{
		ShowWnd("GamePlayScreen", false);
		LoadDlg("data/script/gui/dm_start.lua");
		pkMainWnd->Hide();
	}	
}
