// itemboxbox.h: interface for the itembox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMBOX_H__CBE34F86_D2E8_4A84_8078_6E89373F86A7__INCLUDED_)
#define AFX_ITEMBOX_H__CBE34F86_D2E8_4A84_8078_6E89373F86A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dialogbox.h"

class Container;
class TextureManager;

class ItemBox : public DlgBox 
{
public:
	void SetContainer(Container* pkContainer);
	void Update();
	ItemBox(ZGui* pkGui, ZGuiWndProc oMainWndProc, TextureManager* pkTexMan);
	virtual ~ItemBox();

	bool DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
		int iNumberOfParams,void *pkParams );

	bool Create(int x, int y, int w, int h, ZGuiWndProc pkWndProc);
	bool OnClose(bool bSave);
	bool OnOpen(int x, int y);

private:

	typedef pair<int,int> slot_pos;

	map< slot_pos, ZGuiButton* > m_kSlotsTable;

	const int m_ciSlotSize;
	TextureManager* m_pkTexMan;
	ZGuiButton* m_pkClickButton;
	Container* m_pkContainer;

};

#endif // !defined(AFX_ITEMBOX_H__CBE34F86_D2E8_4A84_8078_6E89373F86A7__INCLUDED_)
