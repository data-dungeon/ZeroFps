// Label.h: interface for the Label class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LABEL_H__EE074D36_08D4_4FF6_92EB_70F5A4E756A4__INCLUDED_)
#define AFX_LABEL_H__EE074D36_08D4_4FF6_92EB_70F5A4E756A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gui_x.h"
#include "zguicontrol.h"

class ZGuiRender;

class GUI_API ZGuiLabel : public ZGuiControl 
{
public:
	ZGuiLabel(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, int iID=0);
	virtual ~ZGuiLabel();

	virtual bool Render( ZGuiRender* renderer );
	bool Notify(ZGuiWnd* pkWnd, int iCode);
};

#endif // !defined(AFX_LABEL_H__EE074D36_08D4_4FF6_92EB_70F5A4E756A4__INCLUDED_)






