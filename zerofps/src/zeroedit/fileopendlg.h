// zguifileopendlg.h: interface for the ZGuiFileOpenDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZGUIFILEOPENDLG_H__5889E38C_500F_4A0F_979B_1CA085A4E0BC__INCLUDED_)
#define AFX_ZGUIFILEOPENDLG_H__5889E38C_500F_4A0F_979B_1CA085A4E0BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gui.h"

#include <string>
#include <bitset>
using namespace std;

class ZFBasicFS;

enum Flags
{
	NORMAL,
	DIRECTORIES_ONLY,
	DISALLOW_DIR_CHANGE,
	NUMBER_OF_FLAGS
};

class FileOpenDlg  
{
public:

	typedef bool (*callback)(ZGuiWnd* pkWnd, unsigned int uiMessage, int iNumParams, void *pParams);

	FileOpenDlg(Gui* pkGui, ZFBasicFS* pkBasicFS, callback cb, 
				bitset<NUMBER_OF_FLAGS> flags = NORMAL);
	virtual ~FileOpenDlg();

	bool DlgProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams );

	string m_szSearchPath;
	string m_szCurrentDir;
	string m_szCurrentFile;

private:
	bool FillPathList(ZGuiListbox* pkListbox, string strDir);
	ZGuiWnd* Create(int x, int y, int w, int h);

	Gui* m_pkGui;
	ZGui* m_pkZGui;
	ZFBasicFS* m_pkBasicFS;
	bool m_bListDirOnly;
	bool m_bNoChangeDir;
	callback m_oGuiCallback;
	ZGuiWnd* m_pkWindow;

	bitset<NUMBER_OF_FLAGS> m_vkBitParams;
};

#endif // !defined(AFX_ZGUIFILEOPENDLG_H__5889E38C_500F_4A0F_979B_1CA085A4E0BC__INCLUDED_)


