#ifndef _CONTROLBOX_H
#define _CONTROLBOX_H

#include "dialogbox.h"

class ControlBox : public DlgBox  
{
public:
	ControlBox(GuiBuilder* pkGuiBuilder);
	virtual ~ControlBox();

	bool Create(int x, int y, int w, int h, ZGuiWndProc pkWndProc);
	bool OnOpen(int x=-1, int y=-1);
	bool OnClose(bool bSave);

};

#endif // #ifndef _CONTROLBOX_H
