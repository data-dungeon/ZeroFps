// spelldlg.h: interface for the SpellDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPELLDLG_H__0C1929AC_DAE0_4FC3_8711_E568BCADA733__INCLUDED_)
#define AFX_SPELLDLG_H__0C1929AC_DAE0_4FC3_8711_E568BCADA733__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../zerofpsv2/render/texturemanager.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"

using namespace std;
#include <string>


const int NUM_PAGES = 10;
const int NUM_SCHOOLS = 10;
const int SPELL_ROWS = 4;
const int SPELL_COLS = 5;

class QuickBoard;

class SpellDlg  
{
public:
	SpellDlg(ZGuiApp* pkApp, QuickBoard* pkQuickBoard);
	~SpellDlg();

	void Update();
	void Init();
	void OnCommand(ZGuiWnd* pkWndClicked);
	void ToogleOpen();

private:
	ZGuiApp* m_pkApp;
	ZGui* m_pkGui;
	ZGuiResourceManager* m_pkResMan;
	TextureManager* m_pkTexMan;
	ZFAudioSystem* m_pkAudioSys;
	ZGuiWnd* m_pkDialog;
	ZGuiButton* m_pkPageButton[10];
	ZGuiCheckbox* m_pkSchoolButtons[10];
	ZGuiButton* m_pkSpellButtons[SPELL_ROWS][SPELL_COLS];
	QuickBoard* m_pkQuickBoard;

	

	struct SpellSlot
	{
		int x, y;
		ZGuiLabel* pkLabel;
	};

	vector<SpellSlot*> m_vkSpells;

	SpellSlot* FindSlot(int x, int y);
	

	ZGuiCheckbox* m_pkAutoCloseSpellCheckBox;
	bool m_bAutoCloseWnd;
};

#endif // !defined(AFX_SPELLDLG_H__0C1929AC_DAE0_4FC3_8711_E568BCADA733__INCLUDED_)
