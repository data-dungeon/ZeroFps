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
		char* szWndToHide[] =
		{
			"GamePlayChar1Wnd", "GamePlayChar2Wnd", 
			"GamePlayChar3Wnd", "GamePlayChar4Wnd", 
			"GamePlayChar5Wnd", "GamePlayPanelWnd",
			"GamePlayInfoWnd", 
		};

		for(int i=0; i<sizeof(szWndToHide)/sizeof(szWndToHide[1]); i++)
			ShowWnd(szWndToHide[i], false);	

		LoadDlg("data/script/gui/dm_ingame.lua");
		pkMainWnd->Hide();	
	}
}
