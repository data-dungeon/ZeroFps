// quickboard.h: interface for the QuickBoard class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _QUICKBOARD_H_
#define _QUICKBOARD_H

#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../zerofpsv2/render/texturemanager.h"

const int MAX_NUM_QUICK_ITEMS = 7;

class QuickBoard  
{
public:
	void Update();
	void OnCommand(ZGuiWnd* pkWndClicked);
	void AddQuickItem(char* szItemName);
	void Init();
	QuickBoard(ZGuiApp* pkApp);
	~QuickBoard();

private:

	struct QuickItem
	{
		char szActionName[50];
		ZGuiButton* pkButton;
		ZGuiLabel* pkLabel;
		int iPos;
	};

	vector<QuickItem*> m_vkQuickItems;

	ZGuiApp* m_pkApp;
	ZGui* m_pkGui;
	ZGuiResourceManager* m_pkResMan;
	TextureManager* m_pkTexMan;
	ZGuiWnd* m_pkDialog;
};

#endif // #ifndef _QUICKBOARD_H_
