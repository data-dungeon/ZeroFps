// workpaneldlg.h: interface for the WorkPanelDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORKPANELDLG_H__DFE20F04_8243_480E_AED1_73229A96CB5D__INCLUDED_)
#define AFX_WORKPANELDLG_H__DFE20F04_8243_480E_AED1_73229A96CB5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../common/dialogbox.h"

class Gui;
class ZeroEdit;

class WorkPanelDlg : public DlgBox
{
public:
	WorkPanelDlg(Gui* pkGui, Input* pkInput, ZGuiWndProc oMainWndProc);
	virtual ~WorkPanelDlg();

	void UpdateMapTexList();
	bool Create(int x, int y, char* szResourceFile, char* szDlgName);
	bool DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage, int iNumberOfParams,void *pkParams );

protected:

	bool OnOpen(int x=-1, int y=-1);
	bool OnClose(bool bSave);

private:
	ZGuiTabCtrl* WorkPanel();
	Gui* m_pkGuiBuilder;
	ZeroEdit* m_pkEdit;
	bool m_bLockMaskSliders;
	int m_iWidth, m_iHeight;

	void MoveColorSliders(char* szLeader);
};

#endif // !defined(AFX_WORKPANELDLG_H__DFE20F04_8243_480E_AED1_73229A96CB5D__INCLUDED_)
