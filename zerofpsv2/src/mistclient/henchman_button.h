#ifndef _HENCHMAN_BUTTON_H_
#define _HENCHMAN_BUTTON_H_

#include "../zerofpsv2/gui/zgui.h"

class HenchmanButton
{
	ZGuiCheckbox* m_pkClickIcon;
	ZGuiLabel* m_pkLifeBar;

	int m_iListPos; ///> position in list

public:
	void Check(bool bCheck);
	string GetName();
	HenchmanButton(ZGuiApp* pkApp, int iIconID, int pos);
	~HenchmanButton();

	static int s_iHenchemanAlphaTex;
};

#endif // #ifndef _HENCHMAN_BUTTON_H_















