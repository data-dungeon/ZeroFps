// zguifileopendlg.cpp: implementation of the FileOpenDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "fileopendlg.h"
#include "../zerofps/gui/zgui.h"
#include "../zerofps/gui/zguiresourcemanager.h"
#include "../zerofps/basic/zfbasicfs.h"
#include "../zerofps/engine/input.h"
//#include "resource_id.h"

FileOpenDlg::FileOpenDlg(ZGui* pkGui,ZFBasicFS* pkBasicFS,
						 ZGuiWndProc pkMainWndProc,unsigned long flags,
						 string szSearchPath)
{
	m_szSearchPath.reserve(1024);
	m_szCurrentDir.reserve(128);
	m_szCurrentFile.reserve(128);
	m_szSearchPath=pkBasicFS->GetCWD();

	m_pkGui=pkGui;
	m_pkBasicFS=pkBasicFS;
	m_oMainWndProc=pkMainWndProc;
	m_pkResMan=pkGui->GetResMan();
	
	m_vkBitParams.reset();

	if( ((flags & DIRECTORIES_ONLY)==DIRECTORIES_ONLY) )
	{
		m_vkBitParams.set(DIRECTORIES_ONLY);
	}

	if( ((flags & SAVE_FILES)==SAVE_FILES) )
		m_vkBitParams.set(SAVE_FILES);

	if( ((flags & DISALLOW_DIR_CHANGE)==DISALLOW_DIR_CHANGE) )
	{
		m_vkBitParams.set(DISALLOW_DIR_CHANGE);
		m_szSearchPath=szSearchPath; // search path is only valid if flag DISALLOW_DIR_CHANGE is true...
	}
}

FileOpenDlg::~FileOpenDlg()
{

}

bool FileOpenDlg::DlgProc( ZGuiWnd* pkWindow,unsigned int uiMessage,
						   int iNumberOfParams,void *pkParams )
{
	int iControllID;

	switch(uiMessage)
	{
		case ZGM_COMMAND:
			iControllID=((int*)pkParams)[0];

			switch(iControllID)
			{
			case ID_FILEPATH_WND_CLOSE:
			case ID_FILEPATH_CANCEL_BN:
				{
					m_pkGui->ShowMainWindow(m_pkResMan->Wnd("FOOpenFileWnd"),false);
					return m_oMainWndProc(pkWindow,uiMessage,iNumberOfParams,pkParams);
				}
				break;

			case ID_FILEPATH_OPEN_BN:
				{
					if(m_pkResMan->Wnd("FOFilePathSelFileEB")->GetText())
						m_szCurrentFile=m_pkResMan->Wnd("FOFilePathSelFileEB")->GetText();
					m_pkGui->ShowMainWindow(m_pkResMan->Wnd("FOOpenFileWnd"),false);
					return m_oMainWndProc(pkWindow,uiMessage,iNumberOfParams,pkParams);
				}
				break;
			}
			break;

		case ZGM_SELECTLISTITEM:
		{
			int iIDListBox=((int*)pkParams)[0];
			int iIDListItem=((int*)pkParams)[1];

			ZGuiWnd* pkWnd=m_pkResMan->Wnd("FOFilePathLB");

			if(pkWnd !=NULL)
			{			
				ZGuiListbox* pkListbox=(ZGuiListbox*) pkWnd;
				ZGuiListitem* pkItem=pkListbox->GetItem(iIDListItem);

				if(pkItem !=NULL)
				{
					string szFileName=pkItem->GetText();

					bool bFillPathlist=false;

					if(szFileName==string("..") && !m_vkBitParams.test(DISALLOW_DIR_CHANGE))
					{
						printf("Switching dir\n");

						int new_path_length=m_szSearchPath.find_last_of("/"); 

						if(new_path_length !=string::npos)
						{
							m_szSearchPath.resize(new_path_length); 
							bFillPathlist=true;
						}
					}
					else
					if(szFileName.find(".")==string::npos)
					{
						vector<string> vkDirectories;
						m_pkBasicFS->ListDir(&vkDirectories,m_szSearchPath.c_str(),true);

						bool bFoundDir=false;

						for(unsigned int i=0; i<vkDirectories.size(); i++)
						{
							if(vkDirectories[i]==szFileName)
							{
								m_szSearchPath.append("/");
								m_szSearchPath.append(szFileName);
								bFillPathlist=true;
								bFoundDir=true;
								break;
							}
						}
					}
					else
					{
						ZGuiWnd* pkWnd=m_pkGui->GetWindow(ID_FILEPATH_WND_FILE_EB);

						if(pkWnd)
						{
							ZGuiTextbox* pkTextbox=(ZGuiTextbox*) pkWnd;
							pkTextbox->SetText((char*)szFileName.c_str()); 
						}
					}

					if(bFillPathlist && !m_vkBitParams.test(DISALLOW_DIR_CHANGE))
					{
						int size=m_szSearchPath.length();  
						int pos=m_szSearchPath.find_last_of("/");
						if(string::npos !=pos)
							m_szCurrentDir=m_szSearchPath.substr(pos+1,size-pos);
						FillPathList(pkListbox,m_szSearchPath);
					}
					else
					{
						ZGuiWnd* pkWnd=m_pkGui->GetWindow(ID_FILEPATH_WND_FILE_EB);

						if(pkWnd && !szFileName.empty())
						{
							ZGuiTextbox* pkTextbox=(ZGuiTextbox*) pkWnd;
							pkTextbox->SetText((char*)szFileName.c_str()); 
						}							
					}
				}
			}

		}
		break;
	default:
		return m_oMainWndProc(pkWindow,uiMessage,iNumberOfParams,pkParams);
	}
	return true;
}

bool FileOpenDlg::Create(int x,int y,int w,int h,ZGuiWndProc pkDlgProc)
{	
	ZGuiWnd* pkMainWindow = m_pkResMan->Wnd("FOOpenFileWnd");

	if( pkMainWindow )
	{
		m_pkGui->ShowMainWindow(m_pkResMan->Wnd("FOOpenFileWnd"),true);
		FillPathList((ZGuiListbox*)m_pkResMan->Wnd("FOFilePathLB"),
			m_szSearchPath);

		ZGuiWnd* pkButton = m_pkResMan->Wnd("FOFileOpenBN");
		ZGuiWnd* pkLabel = m_pkResMan->Wnd("FOFileOpenTitle");

		if(m_vkBitParams.test(SAVE_FILES))
		{
			if(pkButton) pkButton->SetText("Save");
			if(pkLabel) pkLabel->SetText("Save");
		}
		else
		{
			if(pkButton) pkButton->SetText("Open");
			if(pkLabel) pkLabel->SetText("Open");
		}

		return true;
	}

	pkMainWindow=new ZGuiWnd(Rect(x,y,x+w,y+h),NULL,true,ID_FILEPATH_WND);
	pkMainWindow->SetSkin(m_pkResMan->Skin(string("DEF_WND_SKIN")));
	pkMainWindow->SetMoveArea(Rect(0,0,1024,768));
	pkMainWindow->SetWindowFlag(WF_CLOSEABLE);

	ZGuiListbox* pkListbox=CreateListbox(pkMainWindow,ID_FILEPATH_WND_LB,0,21,
		w,h-80);
	m_pkGui->RegisterWindow(pkListbox,"FOFilePathLB");

	FillPathList(pkListbox,m_szSearchPath);

	char strTitle[50];
	char strBnName[50];

	if(m_vkBitParams.test(SAVE_FILES))
	{
		strcpy(strBnName,"Save");
		strcpy(strTitle,"Save");
	}
	else
	{
		strcpy(strBnName,"Open");
		strcpy(strTitle,"Open");
	}

	ZGuiWnd* pkCloseBn=CreateButton(pkMainWindow,ID_FILEPATH_WND_CLOSE,w-20,0,
		20,20,"x");
	pkCloseBn->SetWindowFlag(WF_CENTER_TEXT);

	ZGuiWnd* pkOKBn=CreateButton(pkMainWindow,ID_FILEPATH_OPEN_BN,w-80,h-44,
		80,20,strBnName);
	m_pkGui->RegisterWindow(pkOKBn,"FOFileOpenBN");
	m_pkResMan->Wnd("FOFileOpenBN")->SetWindowFlag(WF_CENTER_TEXT);
	ZGuiWnd* pkCancelBn=CreateButton(pkMainWindow,ID_FILEPATH_CANCEL_BN,
		w-80,h-22,80,20,"Cancel");
	pkCancelBn->SetWindowFlag(WF_CENTER_TEXT);

	ZGuiLabel* pkLabel=CreateLabel(pkMainWindow,0,0,0,w,22,strTitle);
	pkLabel->SetSkin(m_pkResMan->Skin(string("DEF_TITLEBAR_SKIN")));
	m_pkGui->RegisterWindow(pkLabel,"FOFileOpenTitle");

	CreateLabel(pkMainWindow,ID_FILEPATH_WND_LABEL_PATH,0,h-44,w-80,20,
		NULL)->SetText((char*)m_szCurrentDir.c_str());

	ZGuiWnd* pkTextbox=CreateTextbox(pkMainWindow,ID_FILEPATH_WND_FILE_EB,0,
		h-22,w-200,20);
	m_pkGui->RegisterWindow(pkTextbox,"FOFilePathSelFileEB");

	m_pkGui->AddMainWindow(ID_FILEPATH_WND_MAIN,pkMainWindow,"FOOpenFileWnd",
		pkDlgProc,true);

	m_pkGui->AddKeyCommand(KEY_ESCAPE,pkMainWindow,pkCancelBn);
	m_pkGui->AddKeyCommand(KEY_ESCAPE,pkMainWindow,pkCloseBn);
	m_pkGui->AddKeyCommand(KEY_RETURN,pkMainWindow,pkOKBn);

	return true;
}

bool FileOpenDlg::FillPathList(ZGuiListbox* pkListbox,string strDir)
{
	ZGuiWnd* pkWnd=m_pkGui->GetWindow(ID_FILEPATH_WND_LABEL_PATH);
	if(pkWnd !=NULL)
	{
		const int MAX_LENGTH=35;

		ZGuiLabel* pkLabel=(ZGuiLabel*) pkWnd;
		
		int length=m_szSearchPath.length(); 
		int start=length - MAX_LENGTH;
		
		string szLabelText;

		if(start < 0)
			start=0;
		else
			szLabelText="...";

		szLabelText=m_szSearchPath;
		szLabelText +=start;
		
		pkLabel->SetText((char*)m_szSearchPath.c_str());
	}

	vector<string> vkFiles;
	if(m_vkBitParams.test(DIRECTORIES_ONLY))
		m_pkBasicFS->ListDir(&vkFiles,strDir.c_str(),true );
	else
		m_pkBasicFS->ListDir(&vkFiles,strDir.c_str() );

	pkListbox->RemoveAllItems();

	char name[450];
	for( unsigned int i=0; i<vkFiles.size(); i++)
	{
		sprintf(name,"%s",vkFiles[i].c_str());	
		pkListbox->AddItem(name,i,false); 
	}

	return true;
}

ZGuiButton* FileOpenDlg::CreateButton(ZGuiWnd* pkParent,int iID,int x,int y,
									  int w,int h,char *pkName)
{
	ZGuiButton* pkButton=new ZGuiButton(Rect(x,y,x+w,y+h),pkParent,true,iID);
	pkButton->SetButtonHighLightSkin(m_pkResMan->Skin(string("DEF_BN_FOCUS_SKIN")));
	pkButton->SetButtonDownSkin(m_pkResMan->Skin(string("DEF_BN_DOWN_SKIN")));
	pkButton->SetButtonUpSkin(m_pkResMan->Skin(string("DEF_BN_UP_SKIN")));
	pkButton->SetText(pkName);
	pkButton->SetGUI(m_pkGui);
	return pkButton;
}

ZGuiListbox* FileOpenDlg::CreateListbox(ZGuiWnd* pkParent,int iID,int x,int y,
										int w,int h)
{
	ZGuiListbox* pkListbox=new ZGuiListbox(Rect(x,y,x+w,y+h),pkParent,true,iID,20,
		m_pkResMan->Skin(string("DEF_BN_UP_SKIN")),m_pkResMan->Skin(string("DEF_BN_DOWN_SKIN")),
		m_pkResMan->Skin(string("DEF_BN_FOCUS_SKIN")));
	pkListbox->SetSkin( m_pkResMan->Skin("DEF_LISTBOX_SKIN") );
	pkListbox->SetScrollbarSkin(m_pkResMan->Skin(string("DEF_SCROLLBAR_SKIN")),
		m_pkResMan->Skin(string("DEF_BN_FOCUS_SKIN")),m_pkResMan->Skin(string("DEF_BN_FOCUS_SKIN")));
	pkListbox->SetGUI(m_pkGui);
	return pkListbox;
}

ZGuiLabel* FileOpenDlg::CreateLabel(ZGuiWnd* pkParent,int iID,int x,int y,int w,
									int h,char* strText)
{
	ZGuiLabel* pkLabel=new ZGuiLabel(Rect(x,y,x+w,y+h),pkParent,true,iID);
	
	if(strText)
		pkLabel->SetText(strText);

	pkLabel->SetGUI(m_pkGui);

	return pkLabel;
}

ZGuiTextbox* FileOpenDlg::CreateTextbox(ZGuiWnd* pkParent,int iID,int x,int y,
										int w,int h,bool bMulitLine)
{
	ZGuiTextbox* pkTextbox=new ZGuiTextbox(Rect(x,y,x+w,y+h),pkParent,true,iID,
		bMulitLine);
	pkTextbox->SetSkin(m_pkResMan->Skin(string("DEF_TEXTBOX_SKIN")));
	pkTextbox->SetScrollbarSkin(m_pkResMan->Skin(string("DEF_SCROLLBAR_SKIN")),
		m_pkResMan->Skin(string("DEF_BN_FOCUS_SKIN")),m_pkResMan->Skin(string("DEF_BN_FOCUS_SKIN")));
	pkTextbox->SetGUI(m_pkGui);

	return pkTextbox;
}

bool FileOpenDlg::GetFlag(FileOpenFlags eFlag)
{
	return m_vkBitParams.test(eFlag);
}
