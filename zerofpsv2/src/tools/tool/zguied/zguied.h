
#ifndef _ZGUIED_H_
#define _ZGUIED_H_

#include "../../../zerofpsv2/engine/application.h"
#include "../../../zerofpsv2/gui/zgui.h"
#include "../../../zerofpsv2/gui/zguiresourcemanager.h"
#include "../../../zerofpsv2/basic/zfbasicfs.h"

#define PREVIEW_SIZE 128

// Remove the console window
#ifdef WIN32
	#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#endif 

class ZGuiEd : public Application, public ZGuiApp
{
public:
	ZGuiEd(char* aName,int iWidth,int iHeight,int iDepth);

	void OnInit(void);
	void OnIdle(void);
	void RenderInterface();

	

	bool StartUp()		{return true;}
	bool ShutDown()	{return true;}
	bool IsValid()		{return true;}
	
	void OnCommand(int wmId, int wmEvent);

private:

	enum TaskList
	{
		TASK_DO_NOTHING				= 0,
		TASK_CREATE_NEW_WINDOW		= 1,		
		TASK_DELETE_SEL_WINDOW		= 2,
		TASK_NEW_GUI					= 3,
		TASK_BRING_WND_TO_FRONT		= 4,
		TASK_UPDATE_TEXTURE_LIST	= 5,
		TASK_SET_TEXTURE				= 6,
		TASK_REMOVE_TEXTURE			= 7,
		TASK_RESIZE_WND_TO_IMAGE	= 8,
		TASK_SET_FONT					= 9,
		TASK_SAVE_SCRIPT				= 10,
		TASK_TEST_GUI					= 11,
		TASK_COPY_WND					= 12,
		TASK_PASTE_WND					= 13,
	};

	int m_iTask; // 0 do nothing, 1 = create new window, 2 = delete sel window

	FILE* m_pkSaveFile;
	Camera* m_pkCamera;
	ZFBasicFS* m_pkBasicFS;
	ZGuiWnd* m_pkFocusWnd, *m_pkCopyWnd;
	int m_iZValueCounter;
	bool m_bResize;
	bool m_bTestGUI;
	bool m_bForceCaptureToSel;
	float	m_fDelayTime;

	vector< pair<ZGuiSkin, string> > m_kSkinTable;

	string m_strNewFileToLoad;
	string m_strCurrTexDir;

	ZGuiWnd* GetWndFromPoint(int x, int y);
	GuiType FormatWndType(string strText);
	string FormatWndType(GuiType eType);
	void OnMouseClick(bool bLeft, int x, int y);
	void OnMouseMove(bool bLeft, int x, int y);
	bool OnLoadGUI(const char* szFile);
	bool OnNew(bool bConfirm);
	void CreateNewWindow(ZGuiWnd* pkCloneTarget=NULL);
	void DeleteSelWindow(bool bConfirm);
	bool DelayCommand(float delay=0.3f);
	void CheckMovement();
	void FilterWnd();
	void UpdateInfo();
	void UpdatePreviewImage(const char* szFileName);
	bool UpdateTextureList();
	bool UpdateSkinList();
	void OnSelNewSkin(int iIndex=-1);
	void SetTexture(bool bSet);
	bool GetSelSkin(ZGuiSkin**& ppkSkin);
	void UpdateWndSkins();
	void OnTestGUI();
	void ResizeWndToImage();
	void OnOpenSelectFont();
	bool SaveScript(const char* szFileName, bool bCreateBackUp=true);
	bool CreateBackup(const char* szFileName);
	bool WriteSkins();
	bool WriteWindows();
	void AddFilesInFolderToListbox(const char* szFolderName);
	string GetTexNameFromID(int id, bool bFullName=true);
	void PrintSkins(ZGuiWnd* pkWnd);
	void UpdateScriptList();
	void AddSampleCtrlItem(ZGuiWnd* pkWnd);
	void RedrawCtrlSkin();
	void CopyWnd();
	void PasteWnd();
	bool CloneSkins(ZGuiWnd* pkDst, ZGuiWnd* pkSrc);
	void HandleInput();

	Point m_kCursorRangeDiff;
};

extern HWND GetCtrl(int iID, bool bRightPanel);
extern bool TextboxFocus();
extern string GetSelItemText(int iID, bool bRightPanel);
extern HWND g_kDlgBoxRight;
extern HWND g_kDlgBoxBottom;
extern HINSTANCE hInstance;
extern HBITMAP preview_bitmap;
extern HWND g_kFontDlg;


#endif // #ifndef _ZGUIED_H_