#include "dark_metropolis.h"
#include "mission_dlg.h"
#include <algorithm>

CMissionDlg::CMissionDlg() : CGameDlg("MissionWnd", &g_kDM)
{
	m_iScrollbarPos = 0;
	m_strSelMission = "";
}

CMissionDlg::~CMissionDlg() 
{
}

void CMissionDlg::OnKeyPress(int iKey, ZGuiWnd* pkCurrentMainWnd)
{
	if(iKey == KEY_ESCAPE)
	{
		OnCommand(pkCurrentMainWnd, "MissionCancelBn", false);
	}	
}

void CMissionDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName,
									 bool bRMouseBnClick)
{
	P_DMMission* pkMissionProperty = (P_DMMission*) 
		GetDMObject(GAME_INFO)->GetProperty("P_DMMission");

	if(strClickName == "BriefingBn")
	{
		if(!m_strSelMission.empty())
		{
			ShowWnd("MissionWnd", false/*, true*/);
			LoadDlg("data/script/gui/dm_briefing.lua");
			m_pkGui->KillWndCapture();
			m_pkGui->SetCaptureToWnd(GetWnd("BriefingWnd"));
			GetGameDlg(BRIEFING_DLG)->InitDlg();

			m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
				m_pkAudioSys->GetListnerPos()); 
		}
	}
	else
	if(strClickName == "MissionCancelBn")
	{
		ShowWnd("MissionWnd", false/*, true*/);
		m_pkGui->KillWndCapture();
		m_pkGui->SetCaptureToWnd(GetWnd("HQWnd"));

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName.find("_Eb") != string::npos && 
		pkMissionProperty->GetCurrentMission() == NULL )
	{
		m_strSelMission.clear(); // töm

		GetWnd("Mission_A_Eb")->GetSkin()->m_afBkColor[0] = 0.75f; 
		GetWnd("Mission_A_Eb")->GetSkin()->m_afBkColor[1] = 0.75f; 
		GetWnd("Mission_A_Eb")->GetSkin()->m_afBkColor[2] = 0.75f; 
		GetWnd("Mission_B_Eb")->GetSkin()->m_afBkColor[0] = 0.75f; 
		GetWnd("Mission_B_Eb")->GetSkin()->m_afBkColor[1] = 0.75f; 
		GetWnd("Mission_B_Eb")->GetSkin()->m_afBkColor[2] = 0.75f; 
		GetWnd("Mission_C_Eb")->GetSkin()->m_afBkColor[0] = 0.75f; 
		GetWnd("Mission_C_Eb")->GetSkin()->m_afBkColor[1] = 0.75f; 
		GetWnd("Mission_C_Eb")->GetSkin()->m_afBkColor[2] = 0.75f; 

		if(strClickName == "Mission_A_Eb")
		{
			GetWnd("Mission_A_Eb")->GetSkin()->m_afBkColor[0] = 1.0f; 	
			GetWnd("Mission_A_Eb")->GetSkin()->m_afBkColor[1] = 1.0f; 	
			GetWnd("Mission_A_Eb")->GetSkin()->m_afBkColor[2] = 1.0f; 	
		}
		else
		if(strClickName == "Mission_B_Eb")
		{
			GetWnd("Mission_B_Eb")->GetSkin()->m_afBkColor[0] = 1.0f; 	
			GetWnd("Mission_B_Eb")->GetSkin()->m_afBkColor[1] = 1.0f; 	
			GetWnd("Mission_B_Eb")->GetSkin()->m_afBkColor[2] = 1.0f; 	
		}
		else
		if(strClickName == "Mission_C_Eb")
		{
			GetWnd("Mission_C_Eb")->GetSkin()->m_afBkColor[0] = 1.0f; 	
			GetWnd("Mission_C_Eb")->GetSkin()->m_afBkColor[1] = 1.0f; 	
			GetWnd("Mission_C_Eb")->GetSkin()->m_afBkColor[2] = 1.0f; 	
		}

		float reputation = ((P_DMGameInfo*)
			GetDMObject(GAME_INFO)->GetProperty("P_DMGameInfo"))->m_fReputation;

		vector<DMMissionInfo> vkInfo;
		pkMissionProperty->GetPossibleMissions(reputation, vkInfo); 

		char* szShortText = GetWnd((char*)strClickName.c_str())->GetText();

		for(unsigned int i=0; i<vkInfo.size(); i++)
		{
			if(szShortText == vkInfo[i].m_strInfoTextShort)
			{
				m_strSelMission = vkInfo[i].m_strName;

				m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
					m_pkAudioSys->GetListnerPos()); 
				break;
			}
		}
	}
	
}

bool CMissionDlg::InitDlg()
{
	if(m_bInitialized == false)
	{
		const int STEPS = 5; // rymmer 15 uppdrag... borde räcka :)

		ZGuiScrollbar* pkScrollbar =  (ZGuiScrollbar*) GetWnd("MissionScrollbar");
		int bn_size = (float) pkScrollbar->GetArrowButtonHeight();
		float fPageSize = 0.20f; 
		if(fPageSize > 1.0f)
			fPageSize = 1.0f;

		ZGuiSkin* pkSkin1 = new ZGuiSkin(GetTexID("data/textures/gui/thumbbn_u.bmp"),0);	// thumb button		
		ZGuiSkin* pkSkin2 = new ZGuiSkin(GetTexID("data/textures/gui/thumbbn_f.bmp"),0);	// thumb button focus

		((ZGuiScrollbar*) GetWnd("MissionScrollbar"))->SetThumbButtonSkins(pkSkin1, pkSkin2);

		((ZGuiScrollbar*) GetWnd("MissionScrollbar"))->SetScrollInfo(
			0, STEPS, fPageSize, m_iScrollbarPos); 

		m_bInitialized = true;
	}

	//
	// Hämta information om alla utillgänliga uppdrag
	//

	UpdateMessageboxes(m_iScrollbarPos);

	m_pkGui->SetCaptureToWnd(GetWnd("MissionWnd"));

	return true;
}


void CMissionDlg::UpdateMessageboxes(int iVectorOffset)
{
	P_DMMission* pkMissionProperty = (P_DMMission*) 
		GetDMObject(GAME_INFO)->GetProperty("P_DMMission");

	Entity* pkGameInfoObj = GetDMObject(GAME_INFO);
	P_DMGameInfo* pkGameInfo = (P_DMGameInfo*) pkGameInfoObj->GetProperty("P_DMGameInfo");

	float reputation = pkGameInfo->m_fReputation;

	vector<DMMissionInfo> vkInfo;
	pkMissionProperty->GetPossibleMissions(reputation, vkInfo); 

	unsigned int iStart_index = iVectorOffset;// - vkInfo.size();

	if(iStart_index  < 0)
		iStart_index  = 0;
	if(iStart_index  > vkInfo.size()-1)
		iStart_index  = vkInfo.size()-1;

	char* test[] = {
		"Mission_A_Eb", "Mission_B_Eb", "Mission_C_Eb",
	};

	for(int i=0; i<3; i++)
	{
		if(vkInfo.size() > iStart_index+i)
		{
			((ZGuiTextbox*)GetWnd(test[i]))->ToggleMultiLine(true);
			((ZGuiTextbox*)GetWnd(test[i]))->SetReadOnly(true);
			SetText(test[i], (char*) 
				vkInfo[iStart_index+i].m_strInfoTextShort.c_str() );
		}
		else
		{
			SetText(test[i], "");
		}

		if(!vkInfo.empty() && m_strSelMission == vkInfo[iStart_index+i].m_strName)
		{
			GetWnd(test[i])->GetSkin()->m_afBkColor[0] = 1.0f; 
			GetWnd(test[i])->GetSkin()->m_afBkColor[1] = 1.0f; 
			GetWnd(test[i])->GetSkin()->m_afBkColor[2] = 1.0f; 	
		}
		else
		{
			GetWnd(test[i])->GetSkin()->m_afBkColor[0] = 0.75f; 
			GetWnd(test[i])->GetSkin()->m_afBkColor[1] = 0.75f; 
			GetWnd(test[i])->GetSkin()->m_afBkColor[2] = 0.75f; 
		}
	}
}

void CMissionDlg::OnScroll(int iID, int iPos, ZGuiWnd *pkMain)
{
	int prev_pos = m_iScrollbarPos;

	m_iScrollbarPos = iPos;
	UpdateMessageboxes(m_iScrollbarPos);

	if(abs(prev_pos - m_iScrollbarPos) == 1)
	{
		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
}