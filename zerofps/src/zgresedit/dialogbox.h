#ifndef _DLGBOX_H
#define _DLGBOX_H

#include "resource_id.h"
#include "guibuilder.h"

#pragma warning( disable : 4786 )

class DlgBox  
{
public:
	int Height();
	int Width();
	
	typedef bool (*ZGuiWndProc)(ZGuiWnd*, unsigned int, int, void*);
	
	DlgBox(GuiBuilder* pkGuiBuilder, ZGuiWndProc oMainWndProc);
	virtual ~DlgBox();

	virtual bool Create(int x, int y, int w, int h, ZGuiWndProc pkWndProc) = 0;

	virtual bool DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
		int iNumberOfParams,void *pkParams ) = 0;

protected:

	virtual bool OnOpen(int x=-1, int y=-1) = 0;
	virtual bool OnClose(bool bSave) = 0;

	GuiBuilder* m_pkGuiBuilder;	
	ZGui* m_pkGui;
	ZGuiWnd* m_pkDlgBox;
	ZGuiWndProc m_oMainWndProc;
};

#endif // #ifndef _DLGBOX_H
