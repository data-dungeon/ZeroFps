// logbox.h: interface for the LogBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGBOX_H__DE7B8142_735D_463B_81E1_725A3BBA2A0C__INCLUDED_)
#define AFX_LOGBOX_H__DE7B8142_735D_463B_81E1_725A3BBA2A0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../common/dialogbox.h"

class LogProperty;
class TextureManager;

class LogBox : public DlgBox 
{
public:
	bool SetLogProperty(LogProperty* lp);
	LogBox(ZGui* pkGui, Input* pkInput, TextureManager* pkTexMan, ZGuiWndProc oMainWndProc);
	virtual ~LogBox();

	bool Create(int x, int y, char* szResourceFile, char* szDlgName);
	bool DlgProc( ZGuiWnd* pkWnd, unsigned int uiMessage,
		int iNumberOfParams, void *pkParams );

private:

	bool OnOpen(int x=-1, int y=-1);
	bool OnClose(bool bSave);

	LogProperty* m_pkLogProperty;
	TextureManager* m_pkTexMan;

};

#endif // !defined(AFX_LOGBOX_H__DE7B8142_735D_463B_81E1_725A3BBA2A0C__INCLUDED_)
