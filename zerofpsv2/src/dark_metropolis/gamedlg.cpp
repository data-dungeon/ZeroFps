
#include "dark_metropolis.h"
#include "gamedlg.h"
#include <typeinfo>

CGameDlg::CGameDlg(string strMainWndName, DarkMetropolis* pkDM)
{
	m_pkDM = pkDM;
	m_pkGui = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));
	m_bInitialized = false;
	m_strMainWndName = strMainWndName;

	m_pkAudioSys = pkDM->m_pkAudioSys;
}

CGameDlg::~CGameDlg()
{
}

int CGameDlg::GetTexID(char* szName)
{
	return m_pkDM->m_pkTexMan->Load(szName, 0);
}

void CGameDlg::ShowWnd(char* szWndResName, bool bShow, bool bToogleCapture) 
{ 
	m_pkDM->ShowWnd(szWndResName, bShow); 

	if(bToogleCapture)
	{
		if(bShow)
			m_pkGui->SetCaptureToWnd(GetWnd(szWndResName));
		else
			m_pkGui->KillWndCapture();
	}
}

Entity* CGameDlg::GetDMObject(DM_OBJECT eDmObject) 
{ 
	int iID = -1;
	unsigned int i;

	vector<Entity*> kObjects;	

	switch(eDmObject)
	{
	case HQ:
		
		m_pkDM->m_pkObjectMan->GetZoneObject()->GetAllEntitys(&kObjects,false);
		for(i=0;i<kObjects.size();i++)
		{
			P_DMHQ* pkHQProperty = (P_DMHQ*)kObjects[i]->GetProperty("P_DMHQ");

			if(pkHQProperty)
			{
				if(pkHQProperty->GetActive())
				{
					iID = kObjects[i]->GetEntityID();
					break;
				}
			}
		}
		break;

		// // gameinfo entity
	case GAME_INFO:
		return m_pkDM->m_pkGameInfoEntity;
		break;
	}

	if(iID != -1)
		return m_pkDM->m_pkObjectMan->GetObjectByNetWorkID(iID);

	return NULL;
}

CGameDlg* CGameDlg::GetGameDlg(GAME_DLG eDlgType)
{
	switch(eDlgType)
	{
	case MISSION_DLG:
		return m_pkDM->m_pkMissionDlg;
		break;
	case GAMEPLAY_DLG:
		return m_pkDM->m_pkGamePlayDlg;
		break;
	case HANDLE_AGENTS_DLG:
		return m_pkDM->m_pkHandleAgents;
		break;
	case HQ_DLG:
		return m_pkDM->m_pkHQDlg;
		break;
	case MEMBERS_DLG:
		return m_pkDM->m_pkMembersDlg;
		break;
	case NEWGAME_DLG:
		return m_pkDM->m_pkNewGameDlg;
		break;
	case START_DLG:
		return m_pkDM->m_pkStartDMDlg;
		break;
	case BRIEFING_DLG:
		return m_pkDM->m_pkBriefingDlg;
		break;
	case ITEMTRANSACTION_DLG:
		return m_pkDM->m_pkItemTransactionDlg;
		break;
	}

	return NULL;
}

bool CGameDlg::LoadDlg(char* szScriptName) 
{ 
	bool bSuccess = m_pkDM->LoadGuiFromScript(/*m_pkDM->m_pkScript,*/ szScriptName); 

	if(bSuccess)
	{
		return true;
	}

	return false;
}

void CGameDlg::GetAllAgentsInField(vector<Entity*>& kList)
{
	//vector<Entity*> kObjects;	
	//m_pkDM->m_pkObjectMan->GetZoneObject()->GetAllEntitys(&kObjects,false);
	//for(unsigned int i=0;i<kObjects.size();i++)
	//	if((P_DMCharacter*)kObjects[i]->GetProperty("P_DMCharacter"))
	//	{
	//		// check if member belongs to players team
	//		if ( ((P_DMCharacter*)kObjects[i]->GetProperty("P_DMCharacter"))->m_iTeam == 0 )
	//		{
	//			kList.push_back(kObjects[i]);	
	//		}
	//	}

	m_pkDM->UpdateAgentsOnField();

	for(unsigned int i=0;i<m_pkDM->m_kAgentsOnField.size();i++)
	{
		kList.push_back(
			m_pkDM->m_pkObjectMan->GetObjectByNetWorkID(
				m_pkDM->m_kAgentsOnField[i]));	
	}

}

void CGameDlg::SetNumber(char* szWndName, float fNumber)
{ 
	char szText[50];
	sprintf(szText, "%f", fNumber);
	m_pkDM->SetText(szWndName, szText);
}

void CGameDlg::SetNumber(char* szWndName, int iNumber)
{ 
	char szText[50];
	sprintf(szText, "%i", iNumber);
	m_pkDM->SetText(szWndName, szText);
}

void CGameDlg::SetButtonIcon(ZGuiWnd* pkButton, string strIconNameUp, 
									  bool bSetAlphaTex, bool bUseBorder)
{
	bool bIsCheckbox = false;

	const type_info& t = typeid(*pkButton);
	if(t==typeid(ZGuiCheckbox))
		bIsCheckbox = true;

	string strIconNameDown="data/textures/notex.bmp";
	string strIconNameA   ="data/textures/notex.bmp";

	int alpha_tex = -1;

	int pos;
	if((pos=strIconNameUp.find(".bmp")) != string::npos)
	{
		// Down tex
		if(!bUseBorder)
		{
			string temp(strIconNameUp);
			temp.erase(pos,strIconNameUp.length());
			temp.insert(temp.length(), "_b.bmp");
			strIconNameDown = temp;
		}

		// Alpha tex
		if(bSetAlphaTex)
		{
			string temp2(strIconNameUp);
			temp2.erase(pos,strIconNameUp.length());
			temp2.insert(temp2.length(), "_a.bmp");
			strIconNameA = temp2;
			alpha_tex = GetTexID((char*)strIconNameA.c_str());
		}
	}

	if(bIsCheckbox == false)
	{
		((ZGuiButton*)pkButton)->GetSkin()->m_iBkTexID = 
			GetTexID((char*)strIconNameUp.c_str());
		((ZGuiButton*)pkButton)->GetSkin()->m_iBkTexAlphaID = alpha_tex;

		((ZGuiButton*)pkButton)->GetButtonUpSkin()->m_iBkTexID = 
			GetTexID((char*)strIconNameUp.c_str());
		((ZGuiButton*)pkButton)->GetButtonUpSkin()->m_iBkTexAlphaID = alpha_tex;

		((ZGuiButton*)pkButton)->GetButtonHighLightSkin()->m_iBkTexID = 
			GetTexID((char*)strIconNameUp.c_str());
		((ZGuiButton*)pkButton)->GetButtonHighLightSkin()->m_iBkTexAlphaID = alpha_tex;
	}
	else
	{
		((ZGuiCheckbox*)pkButton)->GetSkin()->m_iBkTexID = 
			GetTexID((char*)strIconNameUp.c_str());
		((ZGuiCheckbox*)pkButton)->GetSkin()->m_iBkTexAlphaID = alpha_tex;

		((ZGuiCheckbox*)pkButton)->GetUncheckedSkin()->m_iBkTexID = 
			GetTexID((char*)strIconNameUp.c_str());
		((ZGuiCheckbox*)pkButton)->GetUncheckedSkin()->m_iBkTexAlphaID = alpha_tex;

		((ZGuiCheckbox*)pkButton)->GetCheckedSkin()->m_iBkTexID = 
			GetTexID((char*)strIconNameUp.c_str());
		((ZGuiCheckbox*)pkButton)->GetCheckedSkin()->m_iBkTexAlphaID = alpha_tex;		
	}

	if(!bUseBorder)
	{
		if(bIsCheckbox == false)
		{
			((ZGuiButton*)pkButton)->GetButtonDownSkin()->m_iBkTexID = 
				GetTexID((char*)strIconNameDown.c_str());
			((ZGuiButton*)pkButton)->GetButtonDownSkin()->m_iBkTexAlphaID = alpha_tex;
		}
		else
		{
			((ZGuiCheckbox*)pkButton)->GetUncheckedSkin()->m_iBkTexID = 
				GetTexID((char*)strIconNameDown.c_str());
			((ZGuiCheckbox*)pkButton)->GetCheckedSkin()->m_iBkTexAlphaID = alpha_tex;

			((ZGuiCheckbox*)pkButton)->SetButtonCheckedSkin(
				((ZGuiCheckbox*)pkButton)->GetCheckedSkin());
			((ZGuiCheckbox*)pkButton)->SetButtonUncheckedSkin(
				((ZGuiCheckbox*)pkButton)->GetUncheckedSkin());
		}
	}
	else
	{
		if(bIsCheckbox == false)
		{
			((ZGuiButton*)pkButton)->GetButtonDownSkin()->m_iBkTexID = 
				GetTexID((char*)strIconNameUp.c_str());
			((ZGuiButton*)pkButton)->GetButtonDownSkin()->m_iBkTexAlphaID = alpha_tex;
			((ZGuiButton*)pkButton)->GetButtonDownSkin()->m_afBorderColor[0] = 0;
			((ZGuiButton*)pkButton)->GetButtonDownSkin()->m_afBorderColor[1] = 0.5f;
			((ZGuiButton*)pkButton)->GetButtonDownSkin()->m_afBorderColor[2] = 0;
			((ZGuiButton*)pkButton)->GetButtonDownSkin()->m_unBorderSize = 2;
		}
		else
		{
			((ZGuiCheckbox*)pkButton)->GetCheckedSkin()->m_iBkTexID = 
				GetTexID((char*)strIconNameUp.c_str());
			((ZGuiCheckbox*)pkButton)->GetCheckedSkin()->m_iBkTexAlphaID = alpha_tex;
			((ZGuiCheckbox*)pkButton)->GetCheckedSkin()->m_afBorderColor[0] = 0;
			((ZGuiCheckbox*)pkButton)->GetCheckedSkin()->m_afBorderColor[1] = 0.5f;
			((ZGuiCheckbox*)pkButton)->GetCheckedSkin()->m_afBorderColor[2] = 0;
			((ZGuiCheckbox*)pkButton)->GetCheckedSkin()->m_unBorderSize = 2;

			((ZGuiCheckbox*)pkButton)->SetButtonCheckedSkin(
				((ZGuiCheckbox*)pkButton)->GetCheckedSkin());
			((ZGuiCheckbox*)pkButton)->SetButtonUncheckedSkin(
				((ZGuiCheckbox*)pkButton)->GetUncheckedSkin());
		}

	}
}