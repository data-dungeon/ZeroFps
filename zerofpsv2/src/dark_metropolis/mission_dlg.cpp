#include "dark_metropolis.h"
#include "mission_dlg.h"

CMissionDlg::CMissionDlg() : CGameDlg("MissionWnd", &g_kDM)
{
}

CMissionDlg::~CMissionDlg() 
{
}

void CMissionDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	if(strClickName == "BriefingBn")
	{
		ShowWnd("MissionWnd", false/*, true*/);
		LoadDlg("data/script/gui/dm_briefing.lua");
		m_pkGui->KillWndCapture();
		m_pkGui->SetCaptureToWnd(GetWnd("BriefingWnd"));
	}
	else
	if(strClickName == "MissionCancelBn")
	{
		ShowWnd("MissionWnd", false/*, true*/);
		m_pkGui->KillWndCapture();
		m_pkGui->SetCaptureToWnd(GetWnd("HQWnd"));
	}
}

bool CMissionDlg::InitDlg()
{
	string szText =
		string("March 7, 2004 - As you have noticed, we have gotten ourselves a real domain. At the moment we are having a break from Mistlands, ") +
		string("and are working on another game - called DarkMetropolis - for a three month long contest. We're using the same engine so a lot of the ") +
		string("progess will show up in Mistlands as well. For example we have re-implemented heightmaps in the engine, and the editor now supports multiple camera views.") +
		string("Added some new screenshots on it, and some not-so-new character walk-cycle movies.");

	((ZGuiTextbox*)GetWnd("Mission_A_Eb"))->ToggleMultiLine(true);
	((ZGuiTextbox*)GetWnd("Mission_A_Eb"))->SetReadOnly(true);
	SetText("Mission_A_Eb", (char*) szText.c_str() );

	((ZGuiTextbox*)GetWnd("Mission_B_Eb"))->ToggleMultiLine(true);
	((ZGuiTextbox*)GetWnd("Mission_B_Eb"))->SetReadOnly(true);
	SetText("Mission_B_Eb", (char*) szText.c_str() );

	((ZGuiTextbox*)GetWnd("Mission_C_Eb"))->ToggleMultiLine(true);
	((ZGuiTextbox*)GetWnd("Mission_C_Eb"))->SetReadOnly(true);
	SetText("Mission_C_Eb", (char*) szText.c_str() );

	m_pkGui->SetCaptureToWnd(GetWnd("MissionWnd"));

	return true;
}