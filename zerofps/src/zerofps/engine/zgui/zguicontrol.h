// GuiControl.h: interface for the GuiControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUICONTROL_H__850E9039_B50F_4D02_AB7E_866310E576E2__INCLUDED_)
#define AFX_GUICONTROL_H__850E9039_B50F_4D02_AB7E_866310E576E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\engine_x.h"
#include "zguiwindow.h"

class ZGuiRender;

// Interface for a control (like a editobox)
class ENGINE_API ZGuiControl : public ZGuiWnd
{
public:
	ZGuiControl(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, int iID=0);
	virtual ~ZGuiControl();

	virtual bool Render( ZGuiRender* pkRender ); // render border if it exist
	virtual bool Notify(ZGuiWnd* pkWnd, int iCode) {return false;}

	//virtual bool IsControl() { return true; }

};

#endif // !defined(AFX_GUICONTROL_H__850E9039_B50F_4D02_AB7E_866310E576E2__INCLUDED_)
