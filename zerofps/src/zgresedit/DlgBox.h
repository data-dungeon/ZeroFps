#ifndef _DLGBOX_H
#define _DLGBOX_H

#include "resource_id.h"
#include "GuiBuilder.h"

#pragma warning( disable : 4786 )

class DlgBox  
{
public:
	DlgBox(GuiBuilder* pkGuiBuilder);
	virtual ~DlgBox();

	typedef bool (*ZGuiWndProc)(ZGuiWnd*, unsigned int, int, void*);

	virtual bool Create(int x, int y, int w, int h, ZGuiWndProc pkWndProc) = 0;
	virtual bool OnOpen(int x=-1, int y=-1) = 0;
	virtual bool OnClose(bool bSave) = 0;

protected:
	GuiBuilder* m_pkGuiBuilder;	
	ZGui* m_pkGui;
	ZGuiWnd* m_pkDlgBox;
};

#endif // #ifndef _DLGBOX_H
