// zguifileopendlg.cpp: implementation of the FileOpenDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "fileopendlg.h"
#include "../zerofps/engine/zgui/zgui.h"
#include "../zerofps/basic/zfbasicfs.h"
#include "resource_id.h"

// Window proc wrappers
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern ZeroEdit Editor;

static bool OPENFILEPROC( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams ){
	return Editor.m_pkGui->m_pkFileDlgbox->DlgProc(pkWindow, uiMessage, iNumberOfParams, pkParams); }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FileOpenDlg::FileOpenDlg(Gui* pkGui, ZFBasicFS* pkBasicFS, callback cb, unsigned long flags)
{
	m_vkBitParams.reset();

	if( ((flags & DIRECTORIES_ONLY) == DIRECTORIES_ONLY) )
		m_vkBitParams.set(DIRECTORIES_ONLY);

	if( ((flags & SAVE_FILES) == SAVE_FILES) )
		m_vkBitParams.set(SAVE_FILES);

	m_szSearchPath.reserve(1024);
	m_szCurrentDir.reserve(128);
	m_szCurrentFile.reserve(128);
	m_szSearchPath = pkBasicFS->GetCWD();

	m_oGuiCallback = cb;
	m_pkGui = pkGui;
	m_pkZGui = m_pkGui->GetGUI(); 
	m_pkBasicFS = pkBasicFS;

	m_pkWindow = Create(100,100,500,500);
}

FileOpenDlg::~FileOpenDlg()
{
	m_pkZGui->RemoveMainWindow(ID_FILEPATH_WND_MAIN);
}

bool FileOpenDlg::DlgProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams )
{
	int iControllID;

	switch(uiMessage)
	{
		case ZGM_COMMAND:
			iControllID = ((int*)pkParams)[0];

			switch(iControllID)
			{
			case ID_FILEPATH_WND_CLOSE:
			case ID_FILEPATH_CANCEL_BN:
				{
					m_pkZGui->ShowMainWindow(ID_FILEPATH_WND_MAIN, false);
					printf("hej\n");
				}
				break;

			case ID_FILEPATH_OPEN_BN:
				{
					int* pkParams = new int[1];
					pkParams[0] = ID_FILEPATH_OPEN_BN;
					m_oGuiCallback(m_pkWindow, ZGM_COMMAND, 1, pkParams);
					delete[] pkParams;
				}
				break;
			}
			break;

		case ZGM_SELECTLISTITEM:
		{
			int iIDListBox = ((int*)pkParams)[0];
			int iIDListItem = ((int*)pkParams)[1];

			ZGuiWnd* pkWnd = m_pkZGui->GetWindow(iIDListBox);

			if(pkWnd != NULL)
			{			
				ZGuiListbox* pkListbox = (ZGuiListbox*) pkWnd;
				ZGuiListitem* pkItem = pkListbox->GetItem(iIDListItem);

				if(pkItem != NULL)
				{
					string szFileName = pkItem->GetText();

					bool bFillPathlist = false;

					if(szFileName == string("..") )
					{
						int new_path_length = m_szSearchPath.find_last_of("\\"); 

						if(new_path_length != string::npos)
						{
							m_szSearchPath.resize(new_path_length); 
							bFillPathlist = true;
						}
					}
					else
					if(szFileName.find(".") == string::npos)
					{
						vector<string> vkDirectories;
						m_pkBasicFS->ListDir(&vkDirectories, m_szSearchPath.c_str(), true);

						for(unsigned int i=0; i<vkDirectories.size(); i++)
						{
							if(vkDirectories[i] == szFileName)
							{
								m_szSearchPath.append("\\");
								m_szSearchPath.append(szFileName);
								bFillPathlist = true;
								break;
							}
						}
					}
					else
					{
						ZGuiWnd* pkWnd = m_pkZGui->GetWindow(ID_FILEPATH_WND_FILE_EB);

						if(pkWnd)
						{
							ZGuiTextbox* pkTextbox = (ZGuiTextbox*) pkWnd;
							pkTextbox->SetText((char*)szFileName.c_str()); 
						}
					}

					if(bFillPathlist)
					{
						int size = m_szSearchPath.length();  
						int pos = m_szSearchPath.find_last_of("\\");
						if(string::npos != pos)
							m_szCurrentDir = m_szSearchPath.substr(pos+1, size-pos);
						FillPathList(pkListbox, m_szSearchPath);
					}
				}
			}

		}
		break;
	}
	return true;
}

ZGuiWnd* FileOpenDlg::Create(int x, int y, int w, int h)
{
	ZGuiWnd* pkMainWindow = m_pkZGui->GetMainWindow(ID_FILEPATH_WND_MAIN);

	if( pkMainWindow )
	{
		m_pkZGui->ShowMainWindow(ID_FILEPATH_WND_MAIN, true);
		FillPathList((ZGuiListbox*)m_pkGui->Get("FilePathLB"), m_szSearchPath);

		if(m_vkBitParams.test(SAVE_FILES))
		{
			m_pkGui->Get("FileOpenBN")->SetText("Save");
			m_pkGui->Get("FileOpenLabel")->SetText("Save");
		}
		else
		{
			m_pkGui->Get("FileOpenBN")->SetText("Open");
			m_pkGui->Get("FileOpenLabel")->SetText("Open");
		}

		return pkMainWindow;
	}

	pkMainWindow = new ZGuiWnd(Rect(x,y,x+w,y+h),NULL,true,ID_FILEPATH_WND);
	pkMainWindow->SetSkin(m_pkGui->GetSkin("main"), -1, m_pkGui->GetTexture("border_corner_a"));
	pkMainWindow->SetMoveArea(m_pkGui->GetScreenRect());
	pkMainWindow->SetWindowFlag(WF_CLOSEABLE);

	ZGuiListbox* pkListbox = m_pkGui->CreateListbox(pkMainWindow, ID_FILEPATH_WND_LB, 0, 21, w, h-80);

	m_pkGui->Register( pkListbox, "FilePathLB");

	FillPathList(pkListbox, m_szSearchPath);

	char strTitle[50];
	char strBnName[50];

	if(m_vkBitParams.test(SAVE_FILES))
	{
		strcpy(strBnName, "Save");
		strcpy(strTitle, "Save");
	}
	else
	{
		strcpy(strBnName, "Open");
		strcpy(strTitle, "Open");
	}

	m_pkGui->CreateButton(pkMainWindow, ID_FILEPATH_WND_CLOSE, w-20, 0, 20, 20, "x")->SetWindowFlag(WF_CENTER_TEXT);
	m_pkGui->Register(m_pkGui->CreateButton(pkMainWindow, ID_FILEPATH_OPEN_BN, w-80, h-44, 80, 20, strBnName), "FileOpenBN" );
	m_pkGui->Get("FileOpenBN")->SetWindowFlag(WF_CENTER_TEXT);
	m_pkGui->CreateButton(pkMainWindow, ID_FILEPATH_CANCEL_BN, w-80, h-22, 80, 20, "Cancel")->SetWindowFlag(WF_CENTER_TEXT);

	ZGuiLabel* pkLabel = m_pkGui->CreateLabel(pkMainWindow, 0, 0, 0, w, 22, strTitle);
	pkLabel->SetSkin(m_pkGui->GetSkin("titlebar"));
	m_pkGui->Register(pkLabel, "FileOpenLabel");
	m_pkGui->CreateLabel(pkMainWindow, ID_FILEPATH_WND_LABEL_PATH, 0, h-44, w-80, 20, NULL)->SetText((char*)m_szCurrentDir.c_str());
	m_pkGui->CreateTextbox(pkMainWindow, ID_FILEPATH_WND_FILE_EB, 0, h-22, w-200, 20);

	m_pkZGui->AddMainWindow(ID_FILEPATH_WND_MAIN, pkMainWindow, OPENFILEPROC, true);
	return pkMainWindow;
}

bool FileOpenDlg::FillPathList(ZGuiListbox* pkListbox, string strDir)
{
	ZGuiWnd* pkWnd = m_pkZGui->GetWindow(ID_FILEPATH_WND_LABEL_PATH);
	if(pkWnd != NULL)
	{
		const int MAX_LENGTH = 35;

		ZGuiLabel* pkLabel = (ZGuiLabel*) pkWnd;
		
		int length = m_szSearchPath.length(); 
		int start = length - MAX_LENGTH;
		
		string szLabelText;

		if(start < 0)
			start = 0;
		else
			szLabelText = "...";

		szLabelText = m_szSearchPath;
		szLabelText += start;
		
		pkLabel->SetText((char*)m_szSearchPath.c_str());
	}

	vector<string> vkFiles;
	if(m_vkBitParams.test(DIRECTORIES_ONLY))
		m_pkBasicFS->ListDir(&vkFiles, strDir.c_str(), true );
	else
		m_pkBasicFS->ListDir(&vkFiles, strDir.c_str() );

	pkListbox->RemoveAllItems();

	char name[450];
	for( unsigned int i=0; i<vkFiles.size(); i++)
	{
		sprintf(name, "%s", vkFiles[i].c_str());	
		pkListbox->AddItem(name, i); 
	}

	printf("Nr Items: %i\n", pkListbox->GetItemCount());

	return true;
}
