// zguifileopendlg.h: interface for the ZGuiFileOpenDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZGUIFILEOPENDLG_H__5889E38C_500F_4A0F_979B_1CA085A4E0BC__INCLUDED_)
#define AFX_ZGUIFILEOPENDLG_H__5889E38C_500F_4A0F_979B_1CA085A4E0BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <bitset>
using namespace std;

class ZGui;
class ZGuiWnd;
class ZFBasicFS;
class ZGuiListbox;
class ZGuiTextbox;
class ZGuiLabel;
class ZGuiButton;
class ZGuiResourceManager;

enum FileOpenFlags
{
	DIRECTORIES_ONLY=1,
	SAVE_FILES=2,
	DISALLOW_DIR_CHANGE=4,
	NUMBER_OF_FLAGS=6
};

enum FileOpenResID
{
	ID_FILEPATH_WND_CLOSE,
	ID_FILEPATH_CANCEL_BN,
	ID_FILEPATH_WND_MAIN=1827167263,
	ID_FILEPATH_OPEN_BN,
	ID_FILEPATH_WND_FILE_EB,
	ID_FILEPATH_WND,
	ID_FILEPATH_WND_LB,
	ID_FILEPATH_WND_LABEL_PATH,
};

class FileOpenDlg  
{
public:
	bool GetFlag(FileOpenFlags eFlag);
	typedef bool (*ZGuiWndProc)(ZGuiWnd*, unsigned int, int, void*);

	FileOpenDlg(ZGui* pkGui, ZFBasicFS* pkBasicFS, ZGuiWndProc cb, 
		unsigned long flags=NUMBER_OF_FLAGS, string szSearchPath = ""); // search path is only valid if flag DISALLOW_DIR_CHANGE is true...

	virtual ~FileOpenDlg();

	bool DlgProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams );
	bool Create(int x, int y, int w, int h, ZGuiWndProc pkDlgProc);

	string m_szSearchPath;
	string m_szCurrentDir;
	string m_szCurrentFile;

private:
	ZGuiTextbox* CreateTextbox(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h, bool bMulitLine=false);
	ZGuiLabel* CreateLabel(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h, char* strText);
	ZGuiListbox* CreateListbox(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h);
	ZGuiButton* CreateButton(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h, char *pkName);
	bool FillPathList(ZGuiListbox* pkListbox, string strDir);

	ZGuiWnd* m_pkWindow;
	ZGui* m_pkGui;
	ZGuiResourceManager* m_pkResMan;
	ZFBasicFS* m_pkBasicFS;
	ZGuiWndProc m_oMainWndProc;
	
	bitset<16> m_vkBitParams;
};

#endif // !defined(AFX_ZGUIFILEOPENDLG_H__5889E38C_500F_4A0F_979B_1CA085A4E0BC__INCLUDED_)


