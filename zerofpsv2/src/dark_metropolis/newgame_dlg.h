#pragma once
#include "gamedlg.h"

class CNewGameDlg : public CGameDlg
{
public:
	CNewGameDlg();
	~CNewGameDlg();
	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName, bool bRMouseBnClick);

	void StartFade(float fGameTime);
	void EndFade();
	void UpdateFade(float fGameTime);
	bool InitDlg();

private:
	void AddContinueButton();
	float m_fFadeOffset;
	float m_fTimeCheck;
};
