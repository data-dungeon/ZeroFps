#pragma once
#include "gamedlg.h"

class CNewGameDlg : public CGameDlg
{
public:
	CNewGameDlg();
	~CNewGameDlg();
	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName, bool bRMouseBnClick);
	void OnKeyPress(int iKey, ZGuiWnd* pkCurrentMainWnd);

	void StartFade(float fGameTime);
	void UpdateFade(float fGameTime);
	bool InitDlg();

	void SetFade(float fColor);

private:
	void AddContinueButton();
	float m_fFadeOffset;
	float m_fTimeCheck;
};
