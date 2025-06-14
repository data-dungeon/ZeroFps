#pragma once
#include "gamedlg.h"

class CMissionDlg : public CGameDlg
{
public:
	CMissionDlg();
	~CMissionDlg();

	void OnKeyPress(int iKey, ZGuiWnd* pkCurrentMainWnd);
	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName, bool bRMouseBnClick);
	bool InitDlg();

	string m_strSelMission;



protected:
	
	void OnScroll(int iID, int iPos, ZGuiWnd *pkMain);
	
private:
	int m_iScrollbarPos;
	void UpdateMessageboxes(int iVectorOffset);
};
