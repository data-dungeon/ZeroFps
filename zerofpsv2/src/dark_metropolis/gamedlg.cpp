#include "gamedlg.h"

CGameDlg::CGameDlg(DarkMetropolis* pkDM)
{
	m_pkDM = pkDM;
	m_pkGui = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));
}

CGameDlg::~CGameDlg()
{
}

int CGameDlg::GetTexID(char* szName)
{
	return m_pkDM->pkTexMan->Load("data/textures/gui/dm/start_base1.bmp", 0);
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