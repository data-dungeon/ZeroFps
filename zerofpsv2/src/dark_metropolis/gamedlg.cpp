
#include "dark_metropolis.h"
#include "gamedlg.h"

CGameDlg::CGameDlg(string strMainWndName, DarkMetropolis* pkDM)
{
	m_pkDM = pkDM;
	m_pkGui = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));
	m_bInitialized = false;
	m_strMainWndName = strMainWndName;
}

CGameDlg::~CGameDlg()
{
}

int CGameDlg::GetTexID(char* szName)
{
	return m_pkDM->pkTexMan->Load(szName, 0);
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
	switch(eDmObject)
	{
	case HQ:
		vector<Entity*> kObjects;	
		m_pkDM->m_pkObjectMan->GetZoneObject()->GetAllObjects(&kObjects,false,true);
		for(unsigned int i=0;i<kObjects.size();i++)
			if((P_DMHQ*)kObjects[i]->GetProperty("P_DMHQ"))
				iID = kObjects[i]->iNetWorkID;
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
	bool bSuccess = m_pkDM->LoadGuiFromScript(m_pkDM->m_pkScript, szScriptName); 

	if(bSuccess)
	{
		return true;
	}

	return false;
}

void CGameDlg::GetAllAgentsInField(vector<Entity*>& kList)
{
	vector<Entity*> kObjects;	
	m_pkDM->m_pkObjectMan->GetZoneObject()->GetAllObjects(&kObjects,false,true);
	for(unsigned int i=0;i<kObjects.size();i++)
		if((P_DMHQ*)kObjects[i]->GetProperty("P_DMCharacter"))
			kList.push_back(kObjects[i]);	
}