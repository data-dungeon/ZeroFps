// zguitooltip.h: interface for the ZGuiToolTip class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _GUI_ZGUITOOLTIP_H
#define _GUI_ZGUITOOLTIP_H

#include "gui_x.h"
#include "zguiwindow.h"
#include "zguilabel.h"

#include <vector>
using namespace std;

/** \brief	A Gui Tooltip control (a small pop-up window that displays a single line of text describing the purpose of a tool).
	 \ingroup Gui
*/
class GUI_API ZGuiToolTip  
{
public:
	void SetSkin(ZGuiSkin kSkin);
	ZGuiToolTip(float fDisplayTime=3.0f);
	virtual ~ZGuiToolTip();

	void Update(int mouse_x, int mouse_y, bool bMouseClick, float fGameTime);
	void AddToolTip(/*ZGuiWnd* pkWnd*/string strWnd, string strText, float fLifeTimeInSec=0.75f);
	void RemoveToolTip(/*ZGuiWnd* pkWnd*/string strWnd);
	void CloseAll();

	ZGuiWnd* GetWnd() { return m_pkToolTipWnd; }

private:

	struct TOOLTIP_INFO
	{
		//ZGuiWnd* pkWnd;
		string strWnd;
		string strText;
		float fLifeTimeInSec;
	};

	ZGuiResourceManager* m_pkResMan;

	vector<TOOLTIP_INFO> m_vkWindowList;

	ZGui* m_pkGui;
	ZGuiWnd* m_pkToolTipWnd;

	ZGuiWnd* m_pkLastToolTipWnd;
	float m_fToolTipDisplayTime;

	int m_iPrevCursorX, m_iPrevCursorY;
	float m_fDisplayTime;	///< how long it will take (seconds) before the tooltip hides

};

#endif // #ifndef _GUI_ZGUITOOLTIP_H
