// Label.h: interface for the Label class.

#ifndef _GUI_ZGUILABEL_H_
#define _GUI_ZGUILABEL_H_

#include "gui_x.h"
#include "zguiwindow.h"

class ZGuiRender;

/** \brief	A label in the gui.
	 \ingroup Gui
*/
class GUI_API ZGuiLabel : public ZGuiWnd 
{
public:
	ZGuiLabel(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, int iID=0);
	virtual ~ZGuiLabel();

	virtual bool Render( ZGuiRender* renderer );
	bool Notify(ZGuiWnd* pkWnd, int iCode);
};

#endif






