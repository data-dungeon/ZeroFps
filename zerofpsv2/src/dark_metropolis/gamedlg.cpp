#include "gamedlg.h"

CGameDlg::CGameDlg(DarkMetropolis* pkDM)
{
	m_pkDM = pkDM;
}

CGameDlg::~CGameDlg()
{
}

int CGameDlg::GetTexID(char* szName)
{
	return m_pkDM->pkTexMan->Load("data/textures/gui/dm/start_base1.bmp", 0);
}