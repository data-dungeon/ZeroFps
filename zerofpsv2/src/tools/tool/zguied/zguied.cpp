#include "zguied.h"
#include "resource.h"

void ZGuiEd::OnIdle()
{
	if(m_bTestGUI && m_iTask != 11)
		return;

	if(m_strNewFileToLoad != "")
	{
		map<string, ZGuiWnd*> kWnds;
		m_pkGuiMan->GetWindows(kWnds);
		int size = kWnds.size();

		if(OnNew(size > 1))
		{
			OnLoadGUI(m_strNewFileToLoad.c_str());
		}
		m_strNewFileToLoad = "";
	}

	if(m_iTask == TASK_CREATE_NEW_WINDOW) 
	{ 
		CreateNewWindow(); m_iTask = TASK_DO_NOTHING; 
	}
	if(m_iTask == TASK_DELETE_SEL_WINDOW) 
	{ 
		bool bConfirm = m_pkFocusWnd && 
			GetWndType(m_pkFocusWnd) == Wnd && m_pkFocusWnd->GetNumChildrens() != 0;
		DeleteSelWindow(bConfirm); m_iTask = TASK_DO_NOTHING; 
	}
	if(m_iTask == TASK_NEW_GUI) 
	{
		map<string, ZGuiWnd*> kWnds;
		m_pkGuiMan->GetWindows(kWnds);
		int size = kWnds.size();

		OnNew(size>1); m_iTask = TASK_DO_NOTHING; 
	}
	if(m_iTask == TASK_BRING_WND_TO_FRONT) 
	{ 
		if(m_pkFocusWnd) 
		{
			m_pkFocusWnd->m_iZValue = m_iZValueCounter++;
			m_pkGui->PlaceWndFrontBack(m_pkFocusWnd,true); 
			UpdateInfo();
		}
		m_iTask = TASK_DO_NOTHING; 
	}
	if(m_iTask == TASK_UPDATE_TEXTURE_LIST) { if(UpdateTextureList()) m_iTask = TASK_DO_NOTHING; }
	if(m_iTask == TASK_SET_TEXTURE) { SetTexture(true); m_iTask = TASK_DO_NOTHING; }
	if(m_iTask == TASK_REMOVE_TEXTURE) { SetTexture(false); m_iTask = TASK_DO_NOTHING; }
	if(m_iTask == TASK_RESIZE_WND_TO_IMAGE) { ResizeWndToImage(); m_iTask = TASK_DO_NOTHING; }
	if(m_iTask == TASK_SET_FONT) 
	{
		if(m_pkFocusWnd)
		{		
			int sel = SendDlgItemMessage(g_kFontDlg, IDC_FONT_LIST, LB_GETCURSEL, 0, 0);
			if(sel != LB_ERR)
			{
				char szName[200];
				if(SendDlgItemMessage(g_kFontDlg, IDC_FONT_LIST, LB_GETTEXT, sel, 
					(LPARAM) (LPCSTR) szName) != LB_ERR)
				{
					SetFont(m_pkFocusWnd->GetName(), szName, 						
						GetDlgItemInt(g_kFontDlg, IDC_FONT_COLOR_R_EB, NULL, FALSE), 				
						GetDlgItemInt(g_kFontDlg, IDC_FONT_COLOR_G_EB, NULL, FALSE),			
						GetDlgItemInt(g_kFontDlg, IDC_FONT_COLOR_B_EB, NULL, FALSE));				
				}
			}			
		}	
		ShowWindow(GetParent(g_kFontDlg), SW_HIDE);
		m_iTask = TASK_DO_NOTHING;
	}
	if(m_iTask == TASK_SAVE_SCRIPT) 
	{ 
		if(!IsWindowVisible(GetCtrl(IDC_SELECT_SCRIPT_FOLDER_LIST, 1)))
		{			
			ShowWindow(GetCtrl(IDC_NEWCNTRL_CB, 1), SW_HIDE);
			ShowWindow(GetCtrl(IDC_CREATE_WND_BN, 1), SW_HIDE);
			ShowWindow(GetCtrl(IDC_DELETE_WND_BN, 1), SW_HIDE);
			ShowWindow(GetCtrl(IDC_ONLY_OF_WND_TYPE_CB, 1), SW_HIDE);
			ShowWindow(GetCtrl(IDC_SELECT_SCRIPT_FOLDER_LIST, 1), SW_SHOW);

			SendDlgItemMessage(g_kDlgBoxRight, IDC_SELECT_SCRIPT_FOLDER_LIST, LB_RESETCONTENT, 0, 0);
			SendDlgItemMessage(g_kDlgBoxRight, IDC_SELECT_SCRIPT_FOLDER_LIST, LB_ADDSTRING, 0, 
					(LPARAM) (LPCSTR) "../datafiles/mistlands/script/gui/" );
			SendDlgItemMessage(g_kDlgBoxRight, IDC_SELECT_SCRIPT_FOLDER_LIST, LB_ADDSTRING, 0, 
					(LPARAM) (LPCSTR) "../datafiles/sysdata/script/gui/" );
			SendDlgItemMessage(g_kDlgBoxRight, IDC_SELECT_SCRIPT_FOLDER_LIST, LB_ADDSTRING, 0, 
					(LPARAM) (LPCSTR) "../datafiles/dm/script/gui/" );
		}
		else
		{
			int sel = SendDlgItemMessage(g_kDlgBoxRight, IDC_SELECT_SCRIPT_FOLDER_LIST, LB_GETCURSEL, 0, 0);
			if(sel != LB_ERR)
			{
				char szWndName[200];
				if(SendDlgItemMessage(g_kDlgBoxRight, IDC_SELECT_SCRIPT_FOLDER_LIST, LB_GETTEXT, sel, 
					(LPARAM) (LPCSTR) szWndName) != LB_ERR)
				{
					char input_text[100];
					SendDlgItemMessage(g_kDlgBoxRight, IDC_FILELIST_CB, WM_GETTEXT, 99, (LPARAM) (LPTSTR) input_text);

					string strFileName = string(szWndName) + string(input_text);
				
					if(SaveScript(strFileName.c_str()))
						UpdateScriptList();
				}
			}

			ShowWindow(GetCtrl(IDC_NEWCNTRL_CB, 1), SW_SHOW);
			ShowWindow(GetCtrl(IDC_CREATE_WND_BN, 1), SW_SHOW);
			ShowWindow(GetCtrl(IDC_DELETE_WND_BN, 1), SW_SHOW);
			ShowWindow(GetCtrl(IDC_ONLY_OF_WND_TYPE_CB, 1), SW_SHOW);
			ShowWindow(GetCtrl(IDC_SELECT_SCRIPT_FOLDER_LIST, 1), SW_HIDE);
		}

		m_iTask = TASK_DO_NOTHING;
	}
	if(m_iTask == TASK_TEST_GUI)
	{
		OnTestGUI();
		m_iTask = TASK_DO_NOTHING;
	}
	if(m_iTask == TASK_COPY_WND)
	{
		CopyWnd();
		m_iTask = TASK_DO_NOTHING;
	}
	if(m_iTask == TASK_PASTE_WND)
	{
		PasteWnd();
		m_iTask = TASK_DO_NOTHING;
	}

	// Hantera input
	HandleInput();
}

void ZGuiEd::RenderInterface()		
{
	if(m_pkFocusWnd && !m_bTestGUI)
	{
		Rect rc = m_pkFocusWnd->GetScreenRect();
		m_pkGui->SetLineColor(255,0,255);
		m_pkGui->DrawLine(Point(rc.Left,rc.Bottom), Point(rc.Left,rc.Top));
		m_pkGui->DrawLine(Point(rc.Left,rc.Top), Point(rc.Right,rc.Top));
		m_pkGui->DrawLine(Point(rc.Right,rc.Top), Point(rc.Right,rc.Bottom));
		m_pkGui->DrawLine(Point(rc.Right,rc.Bottom), Point(rc.Left,rc.Bottom));
	}
}

ZGuiWnd* ZGuiEd::GetWndFromPoint(int x, int y)
{
	ZGuiWnd* pkFind = NULL;
	vector<ZGuiWnd*> vkCandidates;
	map<string, ZGuiWnd*> kWindows;

	GuiType eType = GuiType_Error;
	bool bFilter = IsDlgButtonChecked(g_kDlgBoxRight, IDC_ONLY_OF_WND_TYPE_CB);

	int sel = SendDlgItemMessage(g_kDlgBoxRight, IDC_NEWCNTRL_CB, CB_GETCURSEL, 0, 0);
	if(sel != CB_ERR)
	{
		char szWndTypeText[100];
		SendDlgItemMessage(g_kDlgBoxRight, IDC_NEWCNTRL_CB, CB_GETLBTEXT, sel, (LPARAM) (LPTSTR) szWndTypeText);
		eType = FormatWndType(szWndTypeText);
	}

	m_pkGuiMan->GetWindows(kWindows);
	for( map<string, ZGuiWnd*>::iterator it = kWindows.begin(); it != kWindows.end(); it++)
	{
		ZGuiWnd* pkWnd = it->second;
		if(pkWnd->GetScreenRect().Inside(x,y) && pkWnd->IsVisible())
		{
			if(bFilter == false || GetWndType(pkWnd) == eType)
			{
				if(!pkWnd->GetParent() || pkWnd->GetParent()->IsVisible()) 
					vkCandidates.push_back(pkWnd);
			}
		}
	}

	if(!vkCandidates.empty())
	{
		pkFind = (*vkCandidates.begin());

		for(vector<ZGuiWnd*>::iterator it2 = vkCandidates.begin(); it2 != vkCandidates.end(); it2++)
		{
			if((*it2)->m_iZValue > pkFind->m_iZValue)
			{
				if(bFilter == false)
					pkFind = (*it2);
				else
				if(eType == GetWndType((*it2)))
					pkFind = (*it2);
			}
		}
	}

	return pkFind;
}

void ZGuiEd::DeleteSelWindow(bool bConfirm)
{
	if(m_pkFocusWnd)
	{
		if(bConfirm && MessageBox(GetParent(g_kDlgBoxRight), "Are u sure?", "Delete?", MB_YESNO) == IDNO)
			return;

		if(m_pkCopyWnd == m_pkFocusWnd)
			m_pkCopyWnd = NULL;

		m_pkGui->UnregisterWindow(m_pkFocusWnd);
		m_pkFocusWnd = NULL;
		SetWindowText(GetCtrl(IDC_WINDOW_NAMEID_EB, 0), "");
		SetWindowText(GetCtrl(IDC_PARENT_WINDOW_NAMEID, 0), "");
		SetWindowText(GetCtrl(IDC_TEXTLABEL_EB, 0), "");
		SetWindowText(GetCtrl(IDC_POSX_EB, 0), "");
		SetWindowText(GetCtrl(IDC_POSY_EB, 0), "");
		SetWindowText(GetCtrl(IDC_WIDTH_EB, 0), "");
		SetWindowText(GetCtrl(IDC_HEIGHT_EB, 0), "");
		SetWindowText(GetCtrl(IDC_WINDOWTYPE_EB, 0), "");
		SetWindowText(GetCtrl(IDC_ZVALUE_EB, 0), "");		

		FilterWnd();
		UpdateSkinList();
		UpdatePreviewImage("");
		OnSelNewSkin(0);
	}
}

void ZGuiEd::CreateNewWindow(ZGuiWnd* pkCloneTarget)
{
	char szWnd[100];
	string text;

	int w, h, x = 0, y = 0;
	GuiType eType;

	if(pkCloneTarget)
	{
		eType = GetWndType(pkCloneTarget);
		x = pkCloneTarget->GetWndRect().Left;
		y = pkCloneTarget->GetWndRect().Top;
		w = pkCloneTarget->GetWndRect().Width();
		h = pkCloneTarget->GetWndRect().Height();
		strcpy(szWnd, FormatWndType(eType).c_str());
		text = pkCloneTarget->GetText(); 
	}
	else
	{
		int iCurrSel = SendMessage(GetCtrl(IDC_NEWCNTRL_CB, 1), CB_GETCURSEL, 0, 0);
		if(iCurrSel == CB_ERR)
			return;
	
		if(SendMessage(GetCtrl(IDC_NEWCNTRL_CB, 1), CB_GETLBTEXT, iCurrSel, (LPARAM) (LPTSTR) szWnd) == CB_ERR)
			return;

		string strText = szWnd;
		if(strText == "Wnd") {eType = Wnd; w=200; h=200;}
		if(strText == "Button") {eType = Button; w=40; h=16;}
		if(strText == "Checkbox") {eType = Checkbox; w=16; h=16;}
		if(strText == "Combobox") {eType = Combobox; w=118; h=16;}
		if(strText == "Label") {eType = Label; w=64; h=16;}
		if(strText == "Listbox") {eType = Listbox; w=128; h=128;}
		if(strText == "Radiobutton") {eType = Radiobutton; w=16; h=16;}
		if(strText == "Scrollbar") {eType = Scrollbar; w=16; h=200;}
		if(strText == "Slider") {eType = Slider; w=50; h=16;}
		if(strText == "TabControl") {eType = TabControl; w=200; h=200;}
		if(strText == "Textbox") {eType = Textbox; w=100; h=16;}
		if(strText == "Treebox") {eType = Treebox; w=100; h=100;}
		if(strText == "Menu") {eType = Menu; w=200; h=16;}
		if(strText == "Progressbar") {eType = Progressbar; w=100; h=16;}

		text = strText;
	}

	char name[50], parent[50];

	while(1)
	{
		sprintf(name, "%s%i", szWnd, m_iZValueCounter);
		if(GetWnd(name) == NULL)
			break;
		else
			m_iZValueCounter++;
	}

	if(eType == Wnd)
	{
		if(m_pkFocusWnd && GetWndType(m_pkFocusWnd) == TabControl)
			strcpy(parent, m_pkFocusWnd->GetName());
		else
			strcpy(parent, "");
	}
	else
	{
		if(m_pkFocusWnd && GetWndType(m_pkFocusWnd) == Wnd)
			strcpy(parent, m_pkFocusWnd->GetName());
		else
		{
			if(m_pkFocusWnd && m_pkFocusWnd->GetParent() && GetWndType(m_pkFocusWnd->GetParent()) == Wnd)
				strcpy(parent, m_pkFocusWnd->GetParent()->GetName());
			else
				strcpy(parent, "GuiMainWnd");
		}
	}

	if(pkCloneTarget)
	{
		string strCloneTargetParent = "";
		if(pkCloneTarget->GetParent())
			strCloneTargetParent = pkCloneTarget->GetParent()->GetName();

		if(strCloneTargetParent == parent)
		{
			x += 10;
			y += 10;
		}
	}

	ZGuiWnd* pkNewWnd = CreateWnd(eType, name, parent, (char*)text.c_str(), x, y, w, h, 0);
	if(pkNewWnd)
	{
		pkNewWnd->SetZValue(m_iZValueCounter++);

		m_pkGui->PlaceWndFrontBack(pkNewWnd, true);
		CheckMovement();

		pkNewWnd->SetZValue(m_iZValueCounter);

		m_pkFocusWnd = pkNewWnd;
		m_bForceCaptureToSel = false; // remove capture
		CheckDlgButton(g_kDlgBoxRight, IDC_FORCE_CAPTURE_TO_SEL_CB, BST_UNCHECKED);
		
		FilterWnd();

		UpdateInfo();

		if(pkCloneTarget)
		{
			CloneSkins(m_pkFocusWnd, pkCloneTarget);
		}

		if(UpdateSkinList())
			OnSelNewSkin(0);

		AddSampleCtrlItem(m_pkFocusWnd);
	}
}

void ZGuiEd::CheckMovement()
{
	if(m_pkFocusWnd == NULL)
		return;

	Rect rc = m_pkFocusWnd->GetScreenRect();
	Rect rcParent = Rect(0,0,800,600);

	if(m_pkFocusWnd->GetParent())
		rcParent = m_pkFocusWnd->GetParent()->GetScreenRect();

	if(rc.Left < rcParent.Left) {rc.Left = rcParent.Left; }
	if(rc.Top < rcParent.Top) { rc.Top = rcParent.Top; } 

	if(m_bResize)
	{
		if(rc.Right > rcParent.Right)		{ m_pkFocusWnd->Resize(rcParent.Right-rc.Left, -1); }
		if(rc.Bottom > rcParent.Bottom)	{ m_pkFocusWnd->Resize(-1, rcParent.Bottom-rc.Top); }
	}
	else
	{
		if(rc.Right > rcParent.Right)		{ rc.Left = rcParent.Right - rc.Width();	}
		if(rc.Bottom > rcParent.Bottom)	{ rc.Top = rcParent.Bottom - rc.Height();	}
	}

	if(m_bResize)
	{
		if(rc.Width() > rcParent.Width())
		{
			rc.Left = rcParent.Left;
			m_pkFocusWnd->Resize(rcParent.Width(), -1);
		}

		if(rc.Height() > rcParent.Height())
		{
			rc.Top = rcParent.Top;
			m_pkFocusWnd->Resize(-1, rcParent.Height());
		}
	}

	m_pkFocusWnd->SetPos(rc.Left, rc.Top, true, true);

	if(m_pkFocusWnd->GetParent())
		rc = m_pkFocusWnd->GetWndRect(); 

	char text[50];
	sprintf(text, "%i", rc.Left); SetWindowText(GetCtrl(IDC_POSX_EB, 0), text);
	sprintf(text, "%i", rc.Top); SetWindowText(GetCtrl(IDC_POSY_EB, 0), text);
	sprintf(text, "%i", rc.Width()); SetWindowText(GetCtrl(IDC_WIDTH_EB, 0), text);
	sprintf(text, "%i", rc.Height()); SetWindowText(GetCtrl(IDC_HEIGHT_EB, 0), text);
}

bool ZGuiEd::OnLoadGUI(const char* szFile)
{
	map<string, ZGuiWnd*> kWndsBefore;
	m_pkGuiMan->GetWindows(kWndsBefore);

	if(!LoadGuiFromScript((char*)szFile))
		return false;				

	string strRealName = m_pkZFVFileSystem->GetRealName(string(szFile));
	SetTitle(string("ZeroFps GUI Editor - ") + strRealName);

	map<string, ZGuiWnd*> kWndsAfter;
	m_pkGuiMan->GetWindows(kWndsAfter);

	vector<ZGuiWnd*> vkCandidates;

	for( map<string, ZGuiWnd*>::iterator it = kWndsAfter.begin(); it != kWndsAfter.end(); it++)
	{
		bool bNewWnd = true;

		for( map<string, ZGuiWnd*>::iterator it2 = kWndsBefore.begin(); it2 != kWndsBefore.end(); it2++)
		{
			if(it2->first == it->first)
			{
				bNewWnd = false;
				break;
			}
		}

		if(bNewWnd)
		{
			vkCandidates.push_back(it->second);
		}
	}

	vector<ZGuiWnd*> addlist;
	addlist.push_back(GetWnd("GuiMainWnd"));
	vkCandidates.push_back(GetWnd("GuiMainWnd"));

	while(1)
	{
		for(int i=0; i<vkCandidates.size(); i++)
		{
			ZGuiWnd* pkParent = vkCandidates[i]->GetParent();

			if(pkParent == NULL)
			{
				bool already_in_list = false;
				for(int k=0; k<addlist.size(); k++)
					if(addlist[k] == vkCandidates[i])
					{
						already_in_list = true;
						break;
					}

					if(!already_in_list)
						addlist.push_back(vkCandidates[i]);
			}
			else
			{
				for(int j=0; j<addlist.size(); j++)
					if(pkParent == addlist[j]) 
					{
						bool already_in_list = false;
						for(int k=0; k<addlist.size(); k++)
							if(addlist[k] == vkCandidates[i])
							{
								already_in_list = true;
								break;
							}

							if(!already_in_list)
								addlist.push_back(vkCandidates[i]);

							break;
					}
			}
		}

		if(addlist.size() == vkCandidates.size())
			break;
	}

	for(int k=0; k<addlist.size(); k++)
	{
		if(addlist[k] != GetWnd("GuiMainWnd"))
		{
			addlist[k]->m_iZValue = m_iZValueCounter++;
			AddSampleCtrlItem(addlist[k]);			
		}
	}

	FilterWnd();

	return true;
}

bool ZGuiEd::OnNew(bool bConfirm)
{
	if(bConfirm && MessageBox(GetParent(g_kDlgBoxRight), "Are u sure?", "New?", MB_YESNO) == IDNO)
		return false;

	SetTitle(string("ZeroFps GUI Editor"));

	m_iZValueCounter = 1; // reset zvalue counter

	ZGuiWnd* pkMainWnd = GetWnd("GuiMainWnd");

	vector<ZGuiWnd*> removelist;
	map<string, ZGuiWnd*> kWnds;

	m_pkGuiMan->GetWindows(kWnds);

	removelist.push_back(GetWnd("GuiMainWnd"));

	while(1)
	{
		for(map<string, ZGuiWnd*>::iterator it = kWnds.begin(); it != kWnds.end(); it++)
		{
			ZGuiWnd* pkWnd = it->second;
			ZGuiWnd* pkParent = it->second->GetParent();

			if(pkParent == NULL)
			{
				bool already_in_list = false;
				for(int k=0; k<removelist.size(); k++)
					if(removelist[k] == pkWnd)
					{
						already_in_list = true;
						break;
					}

					if(!already_in_list)
						removelist.push_back(pkWnd);
			}
			else
			{
				for(int j=0; j<removelist.size(); j++)
					if(pkParent == removelist[j]) 
					{
						bool already_in_list = false;
						for(int k=0; k<removelist.size(); k++)
							if(removelist[k] == pkWnd)
							{
								already_in_list = true;
								break;
							}

							if(!already_in_list)
								removelist.push_back(pkWnd);

							break;
					}
			}
		}

		if(removelist.size() == kWnds.size())
			break;
	}

	for(int k=removelist.size()-1; k>0; k--)
	{
		if(removelist[k] != GetWnd("GuiMainWnd"))
		{
			if(removelist[k] != pkMainWnd)
			{
				m_pkGui->UnregisterWindow(removelist[k]);
			}
		}
	}

	ZGuiWnd::m_pkPrevWndUnderCursor = NULL;
	ZGuiWnd::m_pkPrevWndClicked = NULL;
	ZGuiWnd::m_pkFocusWnd = NULL;
	ZGuiWnd::m_pkWndClicked = NULL; 
	ZGuiWnd::m_pkWndUnderCursor = NULL;

	m_pkFocusWnd = NULL;
	m_pkCopyWnd = NULL;
	SetWindowText(GetCtrl(IDC_WINDOW_NAMEID_EB, 0), "");
	SetWindowText(GetCtrl(IDC_PARENT_WINDOW_NAMEID, 0), "");
	SetWindowText(GetCtrl(IDC_TEXTLABEL_EB, 0), "");
	SetWindowText(GetCtrl(IDC_POSX_EB, 0), "");
	SetWindowText(GetCtrl(IDC_POSY_EB, 0), "");
	SetWindowText(GetCtrl(IDC_WIDTH_EB, 0), "");
	SetWindowText(GetCtrl(IDC_HEIGHT_EB, 0), "");
	SetWindowText(GetCtrl(IDC_ZVALUE_EB, 0), "");		

	SendDlgItemMessage(g_kDlgBoxRight, IDC_WINDOW_LIST, LB_RESETCONTENT,  0, 0 );

	UpdateInfo();
	OnSelNewSkin(0);

	return true;
}

GuiType ZGuiEd::FormatWndType(string strText)
{
	GuiType eType = GuiType_Error;

	if(strText == "Wnd") {eType = Wnd; }
	if(strText == "Button") {eType = Button;}
	if(strText == "Checkbox") {eType = Checkbox;}
	if(strText == "Combobox") {eType = Combobox;}
	if(strText == "Label") {eType = Label;}
	if(strText == "Listbox") {eType = Listbox;}
	if(strText == "Radiobutton") {eType = Radiobutton;}
	if(strText == "Scrollbar") {eType = Scrollbar;}
	if(strText == "Slider") {eType = Slider;}
	if(strText == "TabControl") {eType = TabControl;}
	if(strText == "Textbox") {eType = Textbox;}
	if(strText == "Treebox") {eType = Treebox;}
	if(strText == "Menu") {eType = Menu;}
	if(strText == "Progressbar") {eType = Progressbar;}

	return eType;
}

string ZGuiEd::FormatWndType(GuiType eType)
{
	string strText = "";

	if(eType == Wnd)				strText = "Wnd";
	if(eType == Button)			strText = "Button";
	if(eType == Checkbox)		strText = "Checkbox";	
	if(eType == Combobox)		strText = "Combobox";	
	if(eType == Label)			strText = "Label";	
	if(eType == Listbox)			strText = "Listbox";		
	if(eType == Radiobutton)	strText = "Radiobutton";
	if(eType == Scrollbar)		strText = "Scrollbar";
	if(eType == Slider)			strText = "Slider";	
	if(eType == TabControl)		strText = "TabControl";
	if(eType == Textbox)			strText = "Textbox";	
	if(eType == Treebox)			strText = "Treebox";		
	if(eType == Menu)				strText = "Menu";		
	if(eType == Progressbar)	strText = "Progressbar";

	return strText;
}

void ZGuiEd::FilterWnd()
{
	int sel = SendDlgItemMessage(g_kDlgBoxRight, IDC_NEWCNTRL_CB, CB_GETCURSEL, 0, 0);
	if(sel != CB_ERR)
	{
		bool bFilter = IsDlgButtonChecked(g_kDlgBoxRight, IDC_ONLY_OF_WND_TYPE_CB);

		char szWndTypeText[100];
		map<string, ZGuiWnd*> kWindows;
		vector<string> vkWndNames;

		SendDlgItemMessage(g_kDlgBoxRight, IDC_NEWCNTRL_CB, CB_GETLBTEXT, 
			sel, (LPARAM) (LPTSTR) szWndTypeText);

		GuiType eType = FormatWndType(szWndTypeText);

		m_pkGuiMan->GetWindows(kWindows);
		for( map<string, ZGuiWnd*>::iterator it = kWindows.begin(); it != kWindows.end(); it++)
		{
			ZGuiWnd* pkWnd = GetWnd(it->first);
			if(pkWnd)
			{
				if(bFilter == false || (pkWnd && GetWndType(pkWnd) == eType))
					vkWndNames.push_back(it->first);
			}
		}

		SendDlgItemMessage(g_kDlgBoxRight, IDC_WINDOW_LIST, LB_RESETCONTENT, 0, 0);

		for(int i=0; i<vkWndNames.size(); i++)
		{
			if(vkWndNames[i] != "GuiMainWnd")
				SendDlgItemMessage(g_kDlgBoxRight, IDC_WINDOW_LIST, LB_ADDSTRING, 0, 
					(LPARAM) (LPCSTR) vkWndNames[i].c_str() );
		}

		if(m_pkFocusWnd)
			SendDlgItemMessage(g_kDlgBoxRight, IDC_WINDOW_LIST, LB_SELECTSTRING, -1, 
				(LPARAM) (LPCSTR) m_pkFocusWnd->GetName() );
		else
			SendDlgItemMessage(g_kDlgBoxRight, IDC_WINDOW_LIST, LB_SETCURSEL, -1, 0);
	}
}

void ZGuiEd::UpdateInfo()
{
	if(m_pkFocusWnd == NULL)
	{
		SetWindowText(GetCtrl(IDC_WINDOW_NAMEID_EB, 0), "");
		SetWindowText(GetCtrl(IDC_PARENT_WINDOW_NAMEID, 0), "");
		SetWindowText(GetCtrl(IDC_TEXTLABEL_EB, 0), "");
		SetWindowText(GetCtrl(IDC_POSX_EB, 0), "");
		SetWindowText(GetCtrl(IDC_POSY_EB, 0), "");
		SetWindowText(GetCtrl(IDC_WIDTH_EB, 0), "");
		SetWindowText(GetCtrl(IDC_HEIGHT_EB, 0), "");
		SetWindowText(GetCtrl(IDC_WINDOWTYPE_EB, 0), "");
		SetWindowText(GetCtrl(IDC_ZVALUE_EB, 0), "");		
		CheckDlgButton(g_kDlgBoxBottom, IDC_FREE_MOVEMENT_CB, BST_UNCHECKED);
		UpdateSkinList();
		return;
	}

	SetWindowText(GetCtrl(IDC_WINDOW_NAMEID_EB, 0), m_pkFocusWnd->GetName());
	SetWindowText(GetCtrl(IDC_TEXTLABEL_EB, 0), m_pkFocusWnd->GetText());

	char text[25];
	Rect rc;

	if(m_pkFocusWnd->GetParent())
	{
		rc = m_pkFocusWnd->GetScreenRect();
		SetWindowText(GetCtrl(IDC_PARENT_WINDOW_NAMEID, 0), m_pkFocusWnd->GetParent()->GetName());
	}
	else
	{
		rc = m_pkFocusWnd->GetScreenRect();
		SetWindowText(GetCtrl(IDC_PARENT_WINDOW_NAMEID, 0), "");
	}

	sprintf(text, "%i", rc.Left); SetWindowText(GetCtrl(IDC_POSX_EB, 0), text);
	sprintf(text, "%i", rc.Top); SetWindowText(GetCtrl(IDC_POSY_EB, 0), text);
	sprintf(text, "%i", rc.Width()); SetWindowText(GetCtrl(IDC_WIDTH_EB, 0), text);
	sprintf(text, "%i", rc.Height()); SetWindowText(GetCtrl(IDC_HEIGHT_EB, 0), text);
	SetDlgItemInt(g_kDlgBoxBottom, IDC_ZVALUE_EB, m_pkFocusWnd->m_iZValue, FALSE);
	SetWindowText(GetCtrl(IDC_WINDOWTYPE_EB, 0), FormatWndType(GetWndType(m_pkFocusWnd)).c_str());

	// if selection a tab page, show it
	if(GetWndType(m_pkFocusWnd) == Wnd && m_pkFocusWnd->GetParent() != NULL)
	{
		if(GetWndType(m_pkFocusWnd->GetParent()) == TabControl)
		{
			int num_pages = ((ZGuiTabCtrl*)m_pkFocusWnd->GetParent())->GetNumPages();

			for(int i=0; i<num_pages; i++)
			{
				if(((ZGuiTabCtrl*)m_pkFocusWnd->GetParent())->GetPage(i) == m_pkFocusWnd)
				{
					((ZGuiTabCtrl*)m_pkFocusWnd->GetParent())->SetCurrentPage(i);
					break;
				}
			}
		}
	}

	m_pkFocusWnd->Show();
	CheckDlgButton(g_kDlgBoxRight, IDC_VISIBLE_CHECK, BST_CHECKED);

	if(m_pkFocusWnd->GetMoveArea() == m_pkFocusWnd->GetScreenRect()) 
		CheckDlgButton(g_kDlgBoxBottom, IDC_FREE_MOVEMENT_CB, BST_UNCHECKED);
	else
		CheckDlgButton(g_kDlgBoxBottom, IDC_FREE_MOVEMENT_CB, BST_CHECKED);

	UpdateSkinList();
}

void ZGuiEd::OnTestGUI()
{
	static map<string, int> s_kZValueInfo;

	if(m_bTestGUI) // remebmer all previus z-values
	{
		s_kZValueInfo.clear();

		map<string, ZGuiWnd*> kWindows;
		m_pkGuiMan->GetWindows(kWindows);
		for( map<string, ZGuiWnd*>::iterator it = kWindows.begin(); it != kWindows.end(); it++)
		{
			s_kZValueInfo.insert(map<string,int>::value_type(it->first, it->second->m_iZValue)); 
			AddSampleCtrlItem(it->second);
		}
	}
	else
	{
		map<string, ZGuiWnd*> kWindows;
		m_pkGuiMan->GetWindows(kWindows);
		for( map<string, ZGuiWnd*>::iterator it = kWindows.begin(); it != kWindows.end(); it++)
		{
			map<string,int>::iterator itWndInfo;
			itWndInfo = s_kZValueInfo.find(it->first);
			if(itWndInfo != s_kZValueInfo.end())
			{
				it->second->SetZValue(itWndInfo->second);
			}
			AddSampleCtrlItem(it->second);
		}
	}
}

void ZGuiEd::OnOpenSelectFont()
{
	ShowWindow(GetParent(g_kFontDlg), SW_SHOW);
	ShowWindow(g_kFontDlg, SW_SHOW);

	ZFVFileSystem* m_pkFileSys = reinterpret_cast<ZFVFileSystem*>(
		g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));	

	vector<string> t;
	m_pkFileSys->ListDir(&t, "data/textures/gui/fonts/");

	SendDlgItemMessage(g_kFontDlg, IDC_FONT_LIST, LB_RESETCONTENT, 0, 0);

	for(int i=0; i<t.size(); i++) 
	{
		char *ext = strrchr( t[i].c_str(), '.');
		if(ext != NULL && strcmp(ext, ".fnt")==0)
		{			
			t[i].erase(t[i].find(".fnt"), 4);			
			SendDlgItemMessage(g_kFontDlg, IDC_FONT_LIST, LB_ADDSTRING,  
				0, (LPARAM) (LPCSTR) t[i].c_str() );
		}
	}

	string strFontName = "defguifont";
	unsigned char color[3] = {255,255,255};

	if(m_pkFocusWnd)
	{
		m_pkFocusWnd->GetTextColor(color[0],color[1],color[2]);
		strFontName = m_pkFocusWnd->GetFont()->m_szNameID;
	}

	SetDlgItemInt(g_kFontDlg, IDC_FONT_COLOR_R_EB, color[0], FALSE);
	SetDlgItemInt(g_kFontDlg, IDC_FONT_COLOR_G_EB, color[1], FALSE);
	SetDlgItemInt(g_kFontDlg, IDC_FONT_COLOR_B_EB, color[2], FALSE);

	SendDlgItemMessage(g_kFontDlg, IDC_FONT_LIST, LB_SELECTSTRING, -1, 
		(LPARAM) (LPCSTR) strFontName.c_str() );
}

void ZGuiEd::UpdateScriptList()
{
	SendDlgItemMessage(g_kDlgBoxRight, IDC_FILELIST_CB, CB_RESETCONTENT, 0, 0);

	ZFVFileSystem* m_pkFileSys = reinterpret_cast<ZFVFileSystem*>(
		g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));	

	vector<string> t;
	m_pkFileSys->ListDir(&t, "data/script/gui/");

	for(int i=0; i<t.size(); i++) 
	{
		if(t[i].find(".lua") != string::npos)
			SendDlgItemMessage(g_kDlgBoxRight, IDC_FILELIST_CB, CB_ADDSTRING,  
				0, (LPARAM) (LPCSTR) t[i].c_str() );
	}
}

// Lägg till ett item till listboxar så att dessa skall synas
// (annars ser man inte dom ändringar man gör).
void ZGuiEd::AddSampleCtrlItem(ZGuiWnd* pkWnd)
{
	ZGuiListbox* pkListbox;
	ZGuiCombobox* pkCombobox;
	ZGuiTreebox* pkTreebox;

	int iItems, iScrollbarWidth; 

	switch(GetWndType(pkWnd))
	{
	case Listbox:
		pkListbox = (ZGuiListbox*) pkWnd; 
		pkListbox->RemoveAllItems(); 

		iScrollbarWidth = pkListbox->GetScrollbar()->GetScreenRect().Width();
		
		for(int i=0; i<20; i++)
			pkListbox->AddItem("Sample listbox", i, false);
		break;

	case Combobox:
		pkCombobox = (ZGuiCombobox*) pkWnd; 
		pkCombobox->RemoveAllItems(); 
	
		iItems = pkCombobox->GetNumVisibleRows() * 2;
		for(int i=0; i<iItems; i++)
			pkCombobox->AddItem("Sample combobox", i, false);

		break;

	case Treebox:
		pkTreebox = (ZGuiTreebox*) pkWnd;

		ClearListbox((char*)pkWnd->GetName());
		AddTreeItem((char*)pkWnd->GetName(), "SampleIDTreebox", NULL, "Sample treebox", 1, 2);
		AddTreeItem((char*)pkWnd->GetName(), "SampleIDTreebox1", NULL, "Sample treebox", 2, 1);
		AddTreeItem((char*)pkWnd->GetName(), "SampleIDTreebox2", NULL, "Sample treebox", 0, 1);
		AddTreeItem((char*)pkWnd->GetName(), "SampleIDTreebox2", "SampleIDTreebox", "Sample treebox", 0, 1);
		
		pkTreebox->GetScrollbar(true)->Show();
		pkTreebox->GetScrollbar(true)->GetArrowButton(true)->Show();
		pkTreebox->GetScrollbar(true)->GetArrowButton(false)->Show();
		pkTreebox->GetScrollbar(false)->GetArrowButton(true)->Show();
		pkTreebox->GetScrollbar(false)->GetArrowButton(false)->Show();
		pkTreebox->GetScrollbar(true)->GetButton()->Show();
		pkTreebox->GetScrollbar(true)->GetButton()->Resize(-1,16,false);
		pkTreebox->GetScrollbar(true)->GetButton()->Resize(16,-1,false);		
		break;
	}
}

void ZGuiEd::CopyWnd()
{
	if(m_pkFocusWnd == NULL)
		return;

	m_pkCopyWnd = m_pkFocusWnd;
}

void ZGuiEd::PasteWnd()
{
	if(m_pkCopyWnd)
	{
		vector<ZGuiWnd*> addlist;

		map<string, ZGuiWnd*> kWindows;
		m_pkGuiMan->GetWindows(kWindows);

		for( map<string, ZGuiWnd*>::iterator it = kWindows.begin(); it != kWindows.end(); it++)
		{
			ZGuiWnd* pkWnd = it->second;
			if(pkWnd->GetParent() && pkWnd->GetParent() == m_pkCopyWnd) // hittat ett barn
			{
				bool bAlreadyThere = false;

				for(int k=0; k<addlist.size(); k++)
					if(pkWnd == addlist[k])
					{
						bAlreadyThere = true;
						break;
					}

				if(!bAlreadyThere)
					addlist.push_back(pkWnd);				
			}
		}

		CreateNewWindow(m_pkCopyWnd);

		for(int i=0; i<addlist.size(); i++)
		{
			CreateNewWindow(addlist[i]);		
		}
	}
}