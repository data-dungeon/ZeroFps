// inventorybox.h: interface for the InventoryBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INVENTORYBOX_H__CBE34F86_D2E8_4A84_8078_6E89373F86A7__INCLUDED_)
#define AFX_INVENTORYBOX_H__CBE34F86_D2E8_4A84_8078_6E89373F86A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dialogbox.h"

class InventoryBox : public DlgBox 
{
public:
	InventoryBox(ZGui* pkGui, ZGuiWndProc oMainWndProc);
	virtual ~InventoryBox();

	bool DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
		int iNumberOfParams,void *pkParams );

	bool Create(int x, int y, int w, int h, ZGuiWndProc pkWndProc);
	bool OnClose(bool bSave);
	bool OnOpen(int x, int y);

};

#endif // !defined(AFX_INVENTORYBOX_H__CBE34F86_D2E8_4A84_8078_6E89373F86A7__INCLUDED_)
