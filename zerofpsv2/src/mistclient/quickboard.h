// quickboard.h: interface for the QuickBoard class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _QUICKBOARD_H_
#define _QUICKBOARD_H

#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../zerofpsv2/render/texturemanager.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"

const int MAX_NUM_QUICK_ITEMS = 7;

class QuickBoard  
{
public:

	enum ActionType
	{
		Spell,
		Item,
		Skill
	};

	struct QuickSlot
	{
		char szActionName[50];
		ZGuiButton* pkButton;
		ZGuiLabel* pkLabel;
		int iPos;
	};

	void Update();
	void OnCommand(ZGuiWnd* pkWndClicked, bool bRightMBnClicked);
	void AddSlot(char *szIcon, char* szIconAlpha);
	void RemoveSlot(int iIndex);
	void Init();
	QuickBoard(ZGuiApp* pkApp);
	~QuickBoard();

private:

	vector<QuickSlot*> m_vkQuickSlots;

	ZGuiApp* m_pkApp;
	ZGui* m_pkGui;
	ZGuiResourceManager* m_pkResMan;
	TextureManager* m_pkTexMan;
	ZFAudioSystem* m_pkAudioSys;
	ZGuiWnd* m_pkDialog;
};

#endif // #ifndef _QUICKBOARD_H_
