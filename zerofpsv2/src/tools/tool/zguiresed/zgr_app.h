
#ifndef _ZGUIRESED_H
#define _ZGUIRESED_H

#include "../../../zerofpsv2/engine/application.h"
#include "../../../zerofpsv2/gui/zgui.h"
#include "../../../zerofpsv2/gui/zguiresourcemanager.h"
#include "../../../zerofpsv2/basic/zfbasicfs.h"
#include "../../../zerofpsv2/engine/camera.h"

// Remove the console window
#ifdef WIN32
//	#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#endif 

class ZGuiResEd : public Application, public ZGuiApp
{
public:
	ZGuiResEd(char* aName,int iWidth,int iHeight,int iDepth);

	void OnInit(void);
	void OnIdle(void);
	void RenderInterface();

	bool StartUp()		{return true;}
	bool ShutDown()	{return true;}
	bool IsValid()		{return true;}
	
	void OnCommand(string strCtrlID, int iCmdExtra);
	void OnTextboxInput(string strFocusWnd, int iKey);
	bool IsEditorWnd(ZGuiWnd* pkWnd);

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
		TASK_COPY_SKIN					= 14,
		TASK_COPY_ALL_SKINS			= 15,
		TASK_PASTE_SKIN				= 16,
		TASK_LOAD_SCRIPT				= 17
	};

	int m_iTask; // 0 do nothing, 1 = create new window, 2 = delete sel window

	FILE* m_pkSaveFile;
	Camera* m_pkCamera;
	ZFBasicFS* m_pkBasicFS;
	ZGuiWnd* m_pkFocusWnd, *m_pkCopyWnd;
	ZGuiSkin* m_pkCopySkin;
	vector<ZGuiWnd::SKIN_DESC> m_vCopySkinDesc;
	int m_iZValueCounter;
	bool m_bResize;
	enum ResizeDir { Left, Up } m_eResizeDir;
	bool m_bTestGUI, m_bEditorMode;
	bool m_bForceCaptureToSel;
	bool m_bFreeMovement;
	float	m_fDelayTime;
	bool m_bSelectScriptSave;
	unsigned char m_ucaPickColor[3];
	bool m_bPickedColor;
	bool m_bAlphaTextureMode;
	bool m_bSelectFont; // true = openfiledlg contains fonts
	bool m_bConfirmSaveScript;

	bool m_bMouseClick, m_bMouseDown;
	int m_iMouseX, m_iMouseY;

	int m_iDesignResX, m_iDesignResY;

	struct SPECIAL_WND_INFO
	{
		bool bHiddenFromStart;
		bool bFreemovement;
	};

	map<string,SPECIAL_WND_INFO> m_kSpecialWndInfo;
	//set<string> m_kFreemovementWnds;

	vector< pair<ZGuiSkin, string> > m_kSkinTable; // used while saving

	string m_strNewFileToLoad;
	string m_strOpenFile;
	string m_strCurrTexDir;
	
	ZGuiWnd* GetWndFromPoint(int x, int y);
	GuiType FormatWndType(string strText);
	string FormatWndType(GuiType eType);
	string FormatWndAlignent(ZGuiWnd* pkWnd);
	string FormatWndResizeType(ZGuiWnd* pkWnd);
	void MouseClick(bool bLeft, int x, int y);
	void MouseMove(bool bLeft, int x, int y);
	bool LoadGUI(const char* szFile);
	bool NewGUI(bool bConfirm);
	void CreateNewWindow(ZGuiWnd* pkCloneTarget=NULL);
	void DeleteSelWindow(bool bConfirm);
	bool DelayCommand(float delay=0.3f);
	void CheckMovement();
	void FilterWnd();
	void UpdateInfo();
	void UpdatePreviewImage(const char* szFileName);
	bool UpdateTextureList();
	bool UpdateSkinList();
	void SelNewSkin(int iIndex=-1);
	void SetTexture(bool bSet);
	bool GetSelSkin(ZGuiSkin**& ppkSkin);
	void UpdateWndSkins();
	void ToggleTestGUI();
	void ResizeWndToImage();
	void OpenSelectFontDlg();
	bool SaveScript(const char* szFileName, bool bConfirmOverwrite, bool bCreateBackUp=true);
	bool CreateBackup(const char* szFileName);
	bool WriteSkins();
	bool WriteWindows();
	bool WriteSpecialProperties();
	void AddFilesInFolderToListbox(const char* szFolderName);
	string GetTexNameFromID(int id, bool bFullName=true);
	void PrintSkins(ZGuiWnd* pkWnd);
	void UpdateScriptList();
	void AddSampleCtrlItem(ZGuiWnd* pkWnd);
	void RedrawCtrlSkin();
	void CopyWnd();
	void CutWnd();
	void PasteWnd();
	bool CloneSkins(ZGuiWnd* pkDst, ZGuiWnd* pkSrc);
	void HandleInput();
	bool AlreadyInList(vector<ZGuiWnd*>& kList, ZGuiWnd* kWindow);
	void ShowSpecialControls();
	bool SearchFiles(vector<string>& vkPathList, const char* szRootPath, 
		char* szExtension, bool bSearchForFolders);
	void RenameSelWnd(char* text);
	void OnChangeSkinColor();
	void SwitchDesignRes(int w, int h, bool rescale);
	void OnConfirmMsgBox(bool bAnswer);

	Point m_kCursorRangeDiff;

	friend bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params );
};

#endif // #ifndef _ZGUIRESED_H








