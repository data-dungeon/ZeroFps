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

enum FileOpenFlags
{
	DIRECTORIES_ONLY=1,
	SAVE_FILES=2,
	DISALLOW_DIR_CHANGE=4,
	NUMBER_OF_FLAGS=6
};

class FileOpenDlg  
{
public:
	typedef bool (*callback)(ZGuiWnd* pkWnd, unsigned int uiMessage, int iNumParams, void *pParams);

	FileOpenDlg(Gui* pkGui, ZFBasicFS* pkBasicFS, callback cb, 
		unsigned long flags=NUMBER_OF_FLAGS, string szSearchPath = ""); // search path is only valid if flag DISALLOW_DIR_CHANGE is true...

	virtual ~FileOpenDlg();

	bool DlgProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams );

	string m_szSearchPath;
	string m_szCurrentDir;
	string m_szCurrentFile;

	

private:
	bool FillPathList(ZGuiListbox* pkListbox, string strDir);
	ZGuiWnd* Create(int x, int y, int w, int h);
	ZGuiWnd* m_pkWindow;
	Gui* m_pkGui;
	ZGui* m_pkZGui;
	ZFBasicFS* m_pkBasicFS;
	callback m_oGuiCallback;
	
	bitset<16> m_vkBitParams;
};

#endif // !defined(AFX_ZGUIFILEOPENDLG_H__5889E38C_500F_4A0F_979B_1CA085A4E0BC__INCLUDED_)


