#include "dark_metropolis.h"
#include "mission_dlg.h"

CMissionDlg::CMissionDlg() : CGameDlg("MissionWnd", &g_kDM)
{
	m_iScrollbarPos = 0;
}

CMissionDlg::~CMissionDlg() 
{
}

void CMissionDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	if(strClickName == "BriefingBn")
	{
		if(!m_strSelMission.empty())
		{
			ShowWnd("MissionWnd", false/*, true*/);
			LoadDlg("data/script/gui/dm_briefing.lua");
			m_pkGui->KillWndCapture();
			m_pkGui->SetCaptureToWnd(GetWnd("BriefingWnd"));
			GetGameDlg(BRIEFING_DLG)->InitDlg();
		}
	}
	else
	if(strClickName == "MissionCancelBn")
	{
		ShowWnd("MissionWnd", false/*, true*/);
		m_pkGui->KillWndCapture();
		m_pkGui->SetCaptureToWnd(GetWnd("HQWnd"));
	}
	else
	if(strClickName.find("_Eb") != string::npos)
	{
		m_strSelMission.clear(); // töm

		GetWnd("Mission_A_Eb")->GetSkin()->m_afBkColor[0] = 1; 
		GetWnd("Mission_A_Eb")->GetSkin()->m_afBkColor[1] = 1; 
		GetWnd("Mission_A_Eb")->GetSkin()->m_afBkColor[2] = 1; 
		GetWnd("Mission_B_Eb")->GetSkin()->m_afBkColor[0] = 1; 
		GetWnd("Mission_B_Eb")->GetSkin()->m_afBkColor[1] = 1; 
		GetWnd("Mission_B_Eb")->GetSkin()->m_afBkColor[2] = 1; 
		GetWnd("Mission_C_Eb")->GetSkin()->m_afBkColor[0] = 1; 
		GetWnd("Mission_C_Eb")->GetSkin()->m_afBkColor[1] = 1; 
		GetWnd("Mission_C_Eb")->GetSkin()->m_afBkColor[2] = 1; 

		if(strClickName == "Mission_A_Eb")
		{
			GetWnd("Mission_A_Eb")->GetSkin()->m_afBkColor[0] = 0.22; 
			GetWnd("Mission_A_Eb")->GetSkin()->m_afBkColor[1] = 0.52; 
			GetWnd("Mission_A_Eb")->GetSkin()->m_afBkColor[2] = 0.12; 
		}
		else
		if(strClickName == "Mission_B_Eb")
		{
			GetWnd("Mission_B_Eb")->GetSkin()->m_afBkColor[0] = 0.22; 
			GetWnd("Mission_B_Eb")->GetSkin()->m_afBkColor[1] = 0.52; 
			GetWnd("Mission_B_Eb")->GetSkin()->m_afBkColor[2] = 0.12; 
		}
		else
		if(strClickName == "Mission_C_Eb")
		{
			GetWnd("Mission_C_Eb")->GetSkin()->m_afBkColor[0] = 0.22; 
			GetWnd("Mission_C_Eb")->GetSkin()->m_afBkColor[1] = 0.52; 
			GetWnd("Mission_C_Eb")->GetSkin()->m_afBkColor[2] = 0.12; 
		}


		Entity* pkHQ = GetDMObject(HQ);

		if(pkHQ)
		{
			P_DMMission* pkMissionProperty = (P_DMMission*) 
				pkHQ->GetProperty("P_DMMission");

			vector<DMMissionInfo> vkInfo;
			pkMissionProperty->GetPossibleMissions(1, vkInfo); 

			char* szShortText = GetWnd((char*)strClickName.c_str())->GetText();

			for(int i=0; i<vkInfo.size(); i++)
			{
				if(szShortText == vkInfo[i].m_strInfoTextShort)
				{
					m_strSelMission = vkInfo[i].m_strName;
					break;
				}
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

		((ZGuiScrollbar*) GetWnd("MissionScrollbar"))->SetScrollInfo(
			0, STEPS, fPageSize, m_iScrollbarPos); 

		m_bInitialized = true;
	}

	//
	// Hämta information om alla utillgänliga uppdrag
	//

	UpdateMessageboxes(m_iScrollbarPos, 1);

	m_pkGui->SetCaptureToWnd(GetWnd("MissionWnd"));

	return true;
}


void CMissionDlg::UpdateMessageboxes(int iVectorOffset, int iLevel)
{
	Entity* pkHQ = GetDMObject(HQ);

	if(pkHQ)
	{
		P_DMMission* pkMissionProperty = (P_DMMission*) 
			pkHQ->GetProperty("P_DMMission");

		vector<DMMissionInfo> vkInfo;
		pkMissionProperty->GetPossibleMissions(iLevel, vkInfo); 

		int start_index = iVectorOffset - vkInfo.size();

		if(start_index < 0)
			start_index = 0;
		if(start_index > vkInfo.size()-1)
			start_index = vkInfo.size()-1;
	
		if(vkInfo.size() > start_index)
		{
			((ZGuiTextbox*)GetWnd("Mission_A_Eb"))->ToggleMultiLine(true);
			((ZGuiTextbox*)GetWnd("Mission_A_Eb"))->SetReadOnly(true);
			SetText("Mission_A_Eb", (char*) 
				vkInfo[start_index].m_strInfoTextShort.c_str() );
		}

		if(vkInfo.size() > start_index+1)
		{
			((ZGuiTextbox*)GetWnd("Mission_B_Eb"))->ToggleMultiLine(true);
			((ZGuiTextbox*)GetWnd("Mission_B_Eb"))->SetReadOnly(true);
			SetText("Mission_B_Eb", (char*) 
				vkInfo[start_index+1].m_strInfoTextShort.c_str() );
		}

		if(vkInfo.size() > start_index+2)
		{
			((ZGuiTextbox*)GetWnd("Mission_C_Eb"))->ToggleMultiLine(true);
			((ZGuiTextbox*)GetWnd("Mission_C_Eb"))->SetReadOnly(true);
			SetText("Mission_C_Eb", (char*) 
				vkInfo[start_index+2].m_strInfoTextShort.c_str() );
		}
	}
}

void CMissionDlg::OnScroll(int iID, int iPos, ZGuiWnd *pkMain)
{
	m_iScrollbarPos = iPos;
	UpdateMessageboxes(m_iScrollbarPos, 1);
}