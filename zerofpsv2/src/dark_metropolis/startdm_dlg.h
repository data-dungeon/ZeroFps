#pragma once
#include "gamedlg.h"

class CStartDMDlg : public CGameDlg
{
public:
	CStartDMDlg();
	~CStartDMDlg();

	void OnKeyPress(int iKey, ZGuiWnd* pkCurrentMainWnd);
	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName, bool bRMouseBnClick);
	void OnClick(int x, int y, bool bMouseDown, bool bLeftButton, ZGuiWnd *pkMain);
	void Update(float fFrameTime);

private:

	ZeroFps* m_pkFps;

	void PlayIntroScreen();
	bool m_bPlayIntro;
	float m_fFadeOffset;

	static char* Labels[];
	int m_iNumPictures;

	struct INTRO_CRAP
	{
		int oka;
		float FADE_TIME;
		bool first;
		ZGuiWnd* pkCurrent;
	};

	INTRO_CRAP m_kApa;

	vector<int> m_vkPlayedPictures;

	void CancelIntro();
	int GetNextPicture();
	
};
