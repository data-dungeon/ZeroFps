#ifndef _PROPERTYBOX_H
#define _PROPERTYBOX_H

#include "dialogbox.h"

class PropertyBox : public DlgBox  
{
public:
	void SetEditWnd(ZGuiWnd* pkWnd);
	PropertyBox(GuiBuilder* pkGuiBuilder);
	virtual ~PropertyBox();

	bool Create(int x, int y, int w, int h, ZGuiWndProc pkWndProc);
	bool OnOpen(int x=-1, int y=-1);
	bool OnClose(bool bSave);

private:
	ZGuiWnd* m_pkWndEdit;
};

#endif // #ifndef _PROPERTYBOX_H
