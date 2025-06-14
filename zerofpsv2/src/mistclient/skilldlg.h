// skilldlg.h: interface for the SkillDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKILLDLG_H__3F43BAD0_998F_4EE0_9C84_5C41014FA075__INCLUDED_)
#define AFX_SKILLDLG_H__3F43BAD0_998F_4EE0_9C84_5C41014FA075__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../mcommon/p_charstats.h"
#include "../mcommon/rulesystem/character/characterstats.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../zerofpsv2/render/texturemanager.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"

class QuickBoard;

const int SKILL_ROWS = 8;
const int SKILL_COLS = 5;

class SkillDlg  
{
public:
	bool IsVisible();
	SkillDlg(ZGuiApp* pkApp, QuickBoard* pkQuickBoard);
	~SkillDlg();

	void Update();
	void Init();
	void OnCommand(ZGuiWnd* pkWndClicked);
	void ToogleOpen();
	void OnScroll(int iID, int iPos);
	void SetCharacterProperty(CharacterProperty* pkCharProp);

private:

	struct SkillSlot
	{
		ZGuiLabel* pkLabel;
		string strName;
	};

	SkillSlot* GetSkillFromCursorPos(int x, int y);
	void GetFreeSlotPos(int& x, int &y);
	void AddSlot(string strName);
	ZGuiApp* m_pkApp;
	ZGui* m_pkGui;
	ZGuiResourceManager* m_pkResMan;
	TextureManager* m_pkTexMan;
	ZFAudioSystem* m_pkAudioSys;
	ZGuiWnd* m_pkDialog;
	QuickBoard* m_pkQuickBoard;
	CharacterProperty* m_pkCharProperty;

	ZGuiButton* m_pkSkillButtons[SKILL_ROWS][SKILL_COLS];

	map<string, SkillSlot*> m_vkSkillSlots;

	string GetWndByID(int iID);
	int m_iTopRow;

	Rect m_rcClipperArea;
};

#endif // !defined(AFX_SKILLDLG_H__3F43BAD0_998F_4EE0_9C84_5C41014FA075__INCLUDED_)
