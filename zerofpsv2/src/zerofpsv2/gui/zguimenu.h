// ZGuiMenu.h: interface for the ZGuiMenu class.

#ifndef _GUI_ZGUIMENU_H_
#define _GUI_ZGUIMENU_H_

#include "gui_x.h"
#include "../basic/rect.h"
#include "zguiwindow.h"

class ZGuiRender;

/** \brief	A Menu in the gui.
	 \ingroup Gui
*/
class GUI_API ZGuiMenu : public ZGuiWnd 
{
public:
	ZGuiMenu(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, int iID=0);
	virtual ~ZGuiMenu();

	bool Render( ZGuiRender* renderer );
	void GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const;

protected:
	bool Notify(ZGuiWnd* pkWnd, int iCode);

};

#endif 


