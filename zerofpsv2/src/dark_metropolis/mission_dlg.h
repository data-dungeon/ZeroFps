#pragma once
#include "gamedlg.h"

class CMissionDlg : public CGameDlg
{
public:
	CMissionDlg();
	~CMissionDlg();

	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName);

	string m_strSelMission;

protected:
	bool InitDlg();
	void OnScroll(int iID, int iPos, ZGuiWnd *pkMain);
	
private:
	int m_iScrollbarPos;
	void UpdateMessageboxes(int iVectorOffset);
};
