// userpanel.h: interface for the UserPanel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERPANEL_H__A8A3CF47_3B60_4C3E_9EB6_42353E9A8A2E__INCLUDED_)
#define AFX_USERPANEL_H__A8A3CF47_3B60_4C3E_9EB6_42353E9A8A2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../common/dialogbox.h"

#define _DONT_MAIN

class ZeroRTS;
class GuiBuilder;
struct UnitCommandInfo;
class Object;

const int CMD_BN_SIZE = 48;

const int MAX_NUM_CMD_BNS = 18;
const int MAX_NUM_UNIT_BNS = 15;

class UserPanel : public DlgBox
{
public:
	void UpdateGraphic();
	void OnSelectObjects(Object* pkObjectInFocus);
	bool PopLastButtonCommand(char* szCommand);
	
	bool Click();
	UserPanel(ZeroRTS* pkZeroRts, ZGuiWndProc oMainWndProc);
	virtual ~UserPanel();

	bool Create(int x, int y, char* szResourceFile, char* szDlgName);
	bool DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
		int iNumberOfParams,void *pkParams );

	map<int, string> m_kCmdIconNameMap;
	map<string, int> m_kCmdIconIDMap;

	void HideAllUnitButtons();
	
private:
	void UpdateCmdButtons();
	void UpdateUnitButtons();
	void HideAllCmdButtons();
	int GetNumVisibleCmdButtons();
	void OnClickMinimap(int x, int y);
	void OnClickCmdButton(int iCtrlID);
	ZGuiButton* SetCmdButtonIcon(int iIndex, int iIconIndex, bool bShow);
	bool OnOpen(int x, int y);
	bool OnClose(bool bSave);

	ZeroRTS* m_pkZeroRts;
	GuiBuilder* m_pkGuiBuilder;

	vector<ZGuiButton*> m_akCommandBns;
	vector<ZGuiButton*> m_akUnitBns;

	map<ZGuiButton*, UnitCommandInfo*> m_kClickInfoMap;
	UnitCommandInfo* m_pkLastCmd; // command when the user presses a button.

	float m_fOriginalHealthbarSize;
};

#endif // !defined(AFX_USERPANEL_H__A8A3CF47_3B60_4C3E_9EB6_42353E9A8A2E__INCLUDED_)
