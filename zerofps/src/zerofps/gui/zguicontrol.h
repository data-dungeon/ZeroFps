// GuiControl.h: interface for the GuiControl class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _GUI_ZGUICONTROL_H_
#define _GUI_ZGUICONTROL_H_

#include "gui_x.h"
#include "zguiwindow.h"

class ZGuiRender;

// Interface for a control (like a editobox)
class GUI_API ZGuiControl : public ZGuiWnd
{
public:
	ZGuiControl(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, int iID=0);
	virtual ~ZGuiControl();

	virtual bool Render( ZGuiRender* pkRender ); // render border if it exist
	virtual bool Notify(ZGuiWnd* pkWnd, int iCode) {return false;}

	//virtual bool IsControl() { return true; }

};

#endif 





