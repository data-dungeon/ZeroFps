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

void CBriefingDlg::OnKeyPress(int iKey, ZGuiWnd* pkCurrentMainWnd)
{
	if(iKey == KEY_ESCAPE)
	{
		OnCommand(pkCurrentMainWnd, "BriefingCancelBn", false);
	}	
}

void CBriefingDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName, bool bRMouseBnClick)
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
	//		P_DMMission* pkMissionProperty = (P_DMMission*) 
	//			pkHQ->GetProperty("P_DMMission");

			P_DMMission* pkMissionProperty = (P_DMMission*) 
				GetDMObject(GAME_INFO)->GetProperty("P_DMMission");

			float reputation = ((P_DMGameInfo*)
				GetDMObject(GAME_INFO)->GetProperty("P_DMGameInfo"))->m_fReputation;

			vector<DMMissionInfo> vkInfo;
			pkMissionProperty->GetPossibleMissions(reputation, vkInfo); 

			for(unsigned int i=0; i<vkInfo.size(); i++)
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

					m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
						m_pkAudioSys->GetListnerPos()); 

					break;
				}
			}
		}
	}
	else
	if(strClickName == "BriefingCancelMissionBn")
	{
		P_DMMission* pkMissionProperty = (P_DMMission*) 
			GetDMObject(GAME_INFO)->GetProperty("P_DMMission");
		pkMissionProperty->CancelCurrent(); 

		((CMissionDlg*)GetGameDlg(MISSION_DLG))->m_strSelMission = "";

		pkMainWnd->Hide();
		m_pkGui->KillWndCapture();
		m_pkGui->SetCaptureToWnd(GetWnd("HQWnd"));

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "BriefingCancelBn")
	{
		pkMainWnd->Hide();
		m_pkGui->KillWndCapture();
		m_pkGui->SetCaptureToWnd(GetWnd("HQWnd"));
		((ZGuiTextbox*)GetWnd("MissionDetailEb"))->SetText(""); 

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
}

bool CBriefingDlg::InitDlg()
{
	string strSelMission = ((CMissionDlg*)GetGameDlg(MISSION_DLG))->m_strSelMission;

	P_DMMission* pkMissionProperty = (P_DMMission*) 
		GetDMObject(GAME_INFO)->GetProperty("P_DMMission");

	ShowWnd("BriefingCancelMissionBn", false);
	ShowWnd("BriefingAcceptBn", true);

	if(pkMissionProperty->GetCurrentMission() != NULL)
	{
		ShowWnd("BriefingAcceptBn", false);

		if(pkMissionProperty->GetCurrentMission()->m_strName == strSelMission)
		{
			ShowWnd("BriefingCancelMissionBn", true);
		}
	}

	float reputation = ((P_DMGameInfo*)
		GetDMObject(GAME_INFO)->GetProperty("P_DMGameInfo"))->m_fReputation;

	vector<DMMissionInfo> vkInfo;
	pkMissionProperty->GetPossibleMissions(reputation, vkInfo); 

	for(unsigned int i=0; i<vkInfo.size(); i++)
	{
		if(vkInfo[i].m_strName == strSelMission)
		{
			((ZGuiTextbox*)GetWnd("MissionDetailEb"))->ToggleMultiLine(true);
			((ZGuiTextbox*)GetWnd("MissionDetailEb"))->SetReadOnly(true);
			SetText("MissionDetailEb", (char*) vkInfo[i].m_strInfoTextLong.c_str());		
			break;
		}
	}


	return true;
}