#include "dark_metropolis.h"
#include "briefing_dlg.h"
#include "mission_dlg.h"
#include "../mcommon/si_dm.h"

CBriefingDlg::CBriefingDlg() : CGameDlg("BriefingWnd", &g_kDM)
{
}

CBriefingDlg::~CBriefingDlg()
{
}

void CBriefingDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	string strSelMission = ((CMissionDlg*)GetGameDlg(MISSION_DLG))->m_strSelMission;

	if(strClickName == "BriefingAcceptBn")
	{
		//
		// Starta ett uppdrag
		//

		Entity* pkHQ = GetDMObject(HQ);

		if(pkHQ)
		{
			P_DMMission* pkMissionProperty = (P_DMMission*) 
				pkHQ->GetProperty("P_DMMission");

			vector<DMMissionInfo> vkInfo;
			pkMissionProperty->GetPossibleMissions(1, vkInfo); 

			for(int i=0; i<vkInfo.size(); i++)
			{
				if(vkInfo[i].m_strName == strSelMission)
				{
					pkMissionProperty->SetCurrentMission(vkInfo[i].m_strScript);	

					ShowWnd("HQWnd", false, false);
					LoadDlg("data/script/gui/dm_gameplay_2.lua");
					GetGameDlg(GAMEPLAY_DLG)->InitDlg();
					pkMainWnd->Hide();	
					m_pkGui->KillWndCapture();
					m_pkDM->PauseGame(false); 

					break;
				}
			}
		}
	}
	else
	if(strClickName == "BriefingCancelBn")
	{
		pkMainWnd->Hide();
		m_pkGui->KillWndCapture();
		m_pkGui->SetCaptureToWnd(GetWnd("HQWnd"));
	}
}

bool CBriefingDlg::InitDlg()
{
	string strSelMission = ((CMissionDlg*)GetGameDlg(MISSION_DLG))->m_strSelMission;

	Entity* pkHQ = GetDMObject(HQ);

	if(pkHQ)
	{
		P_DMMission* pkMissionProperty = (P_DMMission*) 
			pkHQ->GetProperty("P_DMMission");

		vector<DMMissionInfo> vkInfo;
		pkMissionProperty->GetPossibleMissions(1, vkInfo); 

		for(int i=0; i<vkInfo.size(); i++)
		{
			if(vkInfo[i].m_strName == strSelMission)
			{
				((ZGuiTextbox*)GetWnd("MissionDetailEb"))->ToggleMultiLine(true);
				((ZGuiTextbox*)GetWnd("MissionDetailEb"))->SetReadOnly(true);
				SetText("MissionDetailEb", (char*) vkInfo[0].m_strInfoTextLong.c_str());		
				break;
			}
		}
	}

	return true;
}