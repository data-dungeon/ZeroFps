// zguifileopendlg.cpp: implementation of the FileOpenDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "fileopendlg.h"
#include "../zerofps/engine/zgui/zgui.h"
#include "../zerofps/basic/zfbasicfs.h"
#include "resource_id.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern ZeroEdit Editor;

static bool OPENFILEPROC( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams ){
	return Editor.m_pkGui->m_pkFileDlgbox->DlgProc(pkWindow, uiMessage, iNumberOfParams, pkParams); }

FileOpenDlg::FileOpenDlg(Gui* pkGui, ZFBasicFS* pkBasicFS, string szPath, 
						 callback cb, bitset<NUMBER_OF_FLAGS> flags)
{
	m_vkBitParams = flags;

	m_szSearchPath.reserve(1024);
	m_szSearchPath.assign(szPath);

	m_szCurrentDir.reserve(128);
	m_szCurrentFile.reserve(128);

	m_szSearchPath = pkBasicFS->GetCWD();
	//printf("%s\n", m_szSearchPath.c_str());

	int size = m_szSearchPath.length();  
	int pos = m_szSearchPath.find_last_of("//");
	if(string::npos != pos)
	{
		//m_szCurrentDir.resize(m_szSearchPath.substr(pos+1, size-pos).size());
	}

	m_oGuiCallback = cb;
	m_bListDirOnly = false;
	m_bNoChangeDir = false;
	m_pkGui = pkGui;
	m_pkZGui = m_pkGui->GetGUI(); 
	m_pkBasicFS = pkBasicFS;
	m_pkWindow = Create(100,100,500,500);
}

FileOpenDlg::~FileOpenDlg()
{

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
				{
					m_pkZGui->ShowMainWindow(ID_FILEPATH_WND_MAIN, false);
					ZGuiWnd* pkWin = m_pkZGui->GetActiveMainWnd();
					if(pkWin)
					{
						printf("%i\n", pkWin->GetID());
					}
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

					if(szFileName == string("..") /*&& !m_vkBitParams.test(DISALLOW_DIR_CHANGE)*/ )
					{
						int new_path_length = m_szSearchPath.find_last_of(char(47)); 

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
								m_szSearchPath.append("//");
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
						int pos = m_szSearchPath.find_last_of("//");
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
	if( m_pkZGui->GetMainWindow(ID_FILEPATH_WND_MAIN))
	{
		m_pkZGui->ShowMainWindow(ID_FILEPATH_WND_MAIN, true);
		return false;
	}

	ZGuiWnd* pkMainWindow = new ZGuiWnd(Rect(x,y,x+w,y+h),NULL,true,ID_FILEPATH_WND);
	pkMainWindow->SetSkin(m_pkGui->GetSkin("blue"));
	pkMainWindow->SetMoveArea(m_pkGui->GetScreenRect());
	pkMainWindow->SetWindowFlag(WF_CLOSEABLE);

	ZGuiListbox* pkListbox = m_pkGui->CreateListbox(pkMainWindow, ID_FILEPATH_WND, 0, 21, w, h-80);
	FillPathList(pkListbox, m_szSearchPath);

	m_pkGui->CreateButton(pkMainWindow, ID_FILEPATH_WND_CLOSE, w-20, 0, 20, 20, "x")->SetWindowFlag(WF_CENTER_TEXT);
	m_pkGui->CreateButton(pkMainWindow, ID_FILEPATH_OPEN_BN, w-80, h-44, 80, 20, "Open")->SetWindowFlag(WF_CENTER_TEXT);
	m_pkGui->CreateButton(pkMainWindow, ID_FILEPATH_CLOSE_BN, w-80, h-22, 80, 20, "Cancel")->SetWindowFlag(WF_CENTER_TEXT);

	ZGuiLabel* pkLabel = m_pkGui->CreateLabel(pkMainWindow, 0, 0, 0, w, 22, "Open");
	pkLabel->SetSkin(m_pkGui->GetSkin("titlebar"));
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
/*		
		int length = m_szSearchPath.length(); 
		int start = length - MAX_LENGTH;
		
		string szLabelText;

		if(start < 0)
			start = 0;
		else
			szLabelText = "...";

		szLabelText = m_szSearchPath;
		szLabelText += start;*/
		
		pkLabel->SetText((char*)m_szSearchPath.c_str());
	}

	vector<string> vkFiles;
	m_pkBasicFS->ListDir(&vkFiles, strDir.c_str() );

	pkListbox->RemoveAllItems();

	if(vkFiles.size() > 0)
	{
		char name[450];
		for( unsigned int i=0; i<vkFiles.size(); i++)
		{
			sprintf(name, "%s", vkFiles[i]);
			pkListbox->AddItem(name, i); 
		}
	}
	else
	{
		printf("Dir have no files!\n");
		return false;
	}

	return true;
}






