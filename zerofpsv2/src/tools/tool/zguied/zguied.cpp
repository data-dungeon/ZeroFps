#include "zguied.h"
#include "resource.h"

void ZGuiEd::OnIdle()
{
	if(!IsWindowVisible(GetDlgItem(g_kDlgBoxRight, IDC_CLOSE_BUTTON)))
		if(m_pkRender->GetFullscreen() || 
			GetDeviceCaps(GetDC(NULL), HORZRES) <= 1024 || 
			GetDeviceCaps(GetDC(NULL), VERTRES) <= 768)
		{
			ShowWindow(GetDlgItem(g_kDlgBoxRight, IDC_CLOSE_BUTTON), SW_SHOW);
			ShowWindow(GetDlgItem(g_kDlgBoxRight, IDC_MINIMIZE_BUTTON), SW_SHOW);			
		}

	if(m_bTestGUI && m_iTask != 11)
		return;

	if(m_strNewFileToLoad != "")
	{
		map<string, ZGuiWnd*> kWnds;
		m_pkGuiMan->GetWindows(kWnds);
		int size = kWnds.size();

		if(NewGUI(size > 1))
			LoadGUI(m_strNewFileToLoad.c_str());

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

		NewGUI(size>1); m_iTask = TASK_DO_NOTHING; 
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
		TestGUI();
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
	if(m_iTask == TASK_COPY_ALL_SKINS)
	{
		m_vCopySkinDesc.clear();
		m_pkFocusWnd->GetWndSkinsDesc(m_vCopySkinDesc);
	
		m_pkCopySkin = NULL;
		m_iTask = TASK_DO_NOTHING;
	}
	if(m_iTask == TASK_COPY_SKIN)
	{	
		ZGuiSkin** pkSkin;
		if(GetSelSkin(pkSkin))
			m_pkCopySkin = *pkSkin;

		m_iTask = TASK_DO_NOTHING;
	}
	if(m_iTask == TASK_PASTE_SKIN)
	{
		if(m_pkCopySkin)
		{
			string strSelSkin = GetSelItemText(IDC_SKINELEMENTS_LIST, false);

			if(strSelSkin != "")
			{
				vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
				m_pkFocusWnd->GetWndSkinsDesc(vkSkinDesc);
				for(int i=0; i<vkSkinDesc.size(); i++)
				{
					if(vkSkinDesc[i].second == strSelSkin)
					{
						*(*vkSkinDesc[i].first) = m_pkCopySkin;
						RedrawCtrlSkin();
						break;
					}
				}
			}
		}
		else
		{
			vector<ZGuiWnd::SKIN_DESC> vkPasteWndSkinDesc;
			m_pkFocusWnd->GetWndSkinsDesc(vkPasteWndSkinDesc);

			for(int i=0; i<vkPasteWndSkinDesc.size(); i++)
			{
				string strPasteWndSkinName = vkPasteWndSkinDesc[i].second;

				for(int j=0; j<m_vCopySkinDesc.size(); j++)
				{
					string strCpyWndSkinName = m_vCopySkinDesc[j].second;

					if(strPasteWndSkinName.find(strCpyWndSkinName) != string::npos)
						*(*vkPasteWndSkinDesc[i].first) = *(*m_vCopySkinDesc[j].first);
				}
			}
		}

		RedrawCtrlSkin();
		

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
	pair<ZGuiWnd*, int> kFind(NULL,0);
	vector<pair<ZGuiWnd*, float> > vkCandidates;
	map<string, ZGuiWnd*> kWindows;

	GuiType eFilterType = GuiType_Error;
	bool bFilter = IsDlgButtonChecked(g_kDlgBoxRight, IDC_ONLY_OF_WND_TYPE_CB);

	int sel = SendDlgItemMessage(g_kDlgBoxRight, IDC_NEWCNTRL_CB, CB_GETCURSEL, 0, 0);
	if(sel != CB_ERR)
	{
		char szWndTypeText[100];
		SendDlgItemMessage(g_kDlgBoxRight, IDC_NEWCNTRL_CB, CB_GETLBTEXT, sel, (LPARAM) (LPTSTR) szWndTypeText);
		eFilterType = FormatWndType(szWndTypeText);
	}

	m_pkGuiMan->GetWindows(kWindows);
	for( map<string, ZGuiWnd*>::iterator it = kWindows.begin(); it != kWindows.end(); it++)
	{
		ZGuiWnd* pkWnd = it->second;
		if(pkWnd->GetScreenRect().Inside(x,y) && pkWnd->IsVisible())
		{
			GuiType eType = GetWndType(pkWnd);
			if(bFilter == false || eType == eFilterType)
			{
				if(!pkWnd->GetParent() || pkWnd->GetParent()->IsVisible())
				{
					int value = pkWnd->m_iZValue;

					Vector2 dist = Vector2(x,y) - Vector2(pkWnd->GetScreenRect().Left,pkWnd->GetScreenRect().Top);					
					float length = dist.Length();

					value -= length;

					vkCandidates.push_back(pair<ZGuiWnd*, int> (pkWnd, value));
				}
			}
		}
	}

	if(!vkCandidates.empty())
	{
		kFind = (*vkCandidates.begin());

		for(vector<pair<ZGuiWnd*, float> >::iterator it2 = vkCandidates.begin(); it2 != vkCandidates.end(); it2++)
			if((*it2).second > kFind.second)
			{
				if(bFilter == false)
					kFind = (*it2);
				else
					if(eFilterType == GetWndType((*it2).first))
						kFind = (*it2);
			}
	}

	return kFind.first;
}

void ZGuiEd::DeleteSelWindow(bool bConfirm)
{
	if(m_pkFocusWnd)
	{
		if(bConfirm && MessageBox(GetParent(g_kDlgBoxRight), "Are u sure?", "Delete?", MB_YESNO) == IDNO)
			return;

		if(m_pkCopyWnd == m_pkFocusWnd)
			m_pkCopyWnd = NULL;
		
		vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
		m_pkFocusWnd->GetWndSkinsDesc(vkSkinDesc);
		
		bool bSameSkinDesc = true;
		for(int i=0; i<vkSkinDesc.size(); i++)
		{
			if(m_vCopySkinDesc.size() == vkSkinDesc.size())
			{
				if(m_vCopySkinDesc[i].second != vkSkinDesc[i].second)
				{
					bSameSkinDesc = false;
					break;
				}
			}

			if(m_pkCopySkin == (*vkSkinDesc[i].first))
			{
				printf("NULLing skin\n");
				m_pkCopySkin = NULL;
			}
		}

		if(bSameSkinDesc)
			m_vCopySkinDesc.clear();
			
		if(m_pkFocusWnd->GetParent() && GetWndType(m_pkFocusWnd->GetParent()) == TabControl)
		{
			ZGuiTabCtrl* pkTabCtrl = (ZGuiTabCtrl*) m_pkFocusWnd->GetParent();
			unsigned int current_page = pkTabCtrl->GetCurrentPage(); 
			pkTabCtrl->DeletePage(current_page);
		}
		else
		{
			m_pkGui->UnregisterWindow(m_pkFocusWnd);
		}

		m_pkFocusWnd = NULL;

		UpdateInfo();
		FilterWnd();
		UpdateSkinList();
		UpdatePreviewImage("");
		SelNewSkin(0);
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
		text = strText;

		if(strText == "Wnd") {eType = Wnd; w=200; h=200; text=""; }
		if(strText == "Button") {eType = Button; w=40; h=16;}
		if(strText == "Checkbox") {eType = Checkbox; w=16; h=16;}
		if(strText == "Combobox") {eType = Combobox; w=118; h=16; text="";}
		if(strText == "Label") {eType = Label; w=64; h=16;}
		if(strText == "Listbox") {eType = Listbox; w=128; h=128; text="";}
		if(strText == "Radiobutton") {eType = Radiobutton; w=16; h=16;}
		if(strText == "Scrollbar") {eType = Scrollbar; w=16; h=200; text="";}
		if(strText == "Slider") {eType = Slider; w=50; h=16; text="";}
		if(strText == "TabControl") {eType = TabControl; w=200; h=200; text="";}
		if(strText == "Textbox") {eType = Textbox; w=100; h=16;}
		if(strText == "Treebox") {eType = Treebox; w=100; h=100; text="";}
		if(strText == "Menu") {eType = Menu; w=200; h=16; text="";}
		if(strText == "Progressbar") {eType = Progressbar; w=100; h=16; text="";}

		if(eType == Wnd && m_pkFocusWnd && m_pkFocusWnd->GetParent() && 
			GetWndType(m_pkFocusWnd->GetParent()) == TabControl)
			m_pkFocusWnd = m_pkFocusWnd->GetParent();
	}

	char name[150], parent[150];

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
		{
			strcpy(parent, "");
		}
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

	if(m_pkFocusWnd && GetWndType(m_pkFocusWnd) == TabControl)
		text = name;

	ZGuiWnd* pkNewWnd = CreateWnd(eType, name, parent, (char*)text.c_str(), x, y, w, h, 0);
	if(pkNewWnd)
	{
		pkNewWnd->SetZValue(m_iZValueCounter++);

		m_pkGui->PlaceWndFrontBack(pkNewWnd, true);
		

		pkNewWnd->SetZValue(m_iZValueCounter);

		m_pkFocusWnd = pkNewWnd;
		
		CheckMovement();

		m_bForceCaptureToSel = false; // remove capture
		CheckDlgButton(g_kDlgBoxRight, IDC_FORCE_CAPTURE_TO_SEL_CB, BST_UNCHECKED);
		
		FilterWnd();

		UpdateInfo();

		if(pkCloneTarget)
			CloneSkins(m_pkFocusWnd, pkCloneTarget);

		if(UpdateSkinList())
			SelNewSkin(0);

		AddSampleCtrlItem(m_pkFocusWnd);

		ShowSpecialControls();

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

	// Resize window if any childs are not inside window
	if(GetWndType(m_pkFocusWnd) == Wnd)
	{
		Rect rc = m_pkFocusWnd->GetScreenRect();

		list<ZGuiWnd*> kChilds;
		m_pkFocusWnd->GetChildrens(kChilds);

		int iBottom=0, iRight=0;
		
		for(list<ZGuiWnd*>::iterator it=kChilds.begin(); it!=kChilds.end(); it++)
		{
			Rect rcChild = (*it)->GetScreenRect();
			if(rcChild.Right > iRight)
				iRight = rcChild.Right;
			if(rcChild.Bottom > iBottom)
				iBottom = rcChild.Bottom;
		}

		if(rc.Right < iRight)
			m_pkFocusWnd->Resize(iRight-rc.Left, -1);

		if(rc.Bottom < iBottom)
			m_pkFocusWnd->Resize(-1, iBottom-rc.Top); 
	}

	rcParent = (m_pkFocusWnd->GetParent()) ? m_pkFocusWnd->GetParent()->GetScreenRect() : Rect(0,0,800,600);
	rc = m_pkFocusWnd->GetScreenRect();

	if(m_bResize == false)
	{
		if(rc.Right > rcParent.Right)
			rc.Left = rcParent.Right-rc.Width();

		if(rc.Bottom > rcParent.Bottom)
			rc.Top = rcParent.Bottom-rc.Height();

		if(rc.Left < rcParent.Left) rc.Left = rcParent.Left;
		if(rc.Top < rcParent.Top) rc.Top = rcParent.Top;

		m_pkFocusWnd->SetPos(rc.Left > 0 ? rc.Left : 0, rc.Top > 0 ? rc.Top : 0, true, true); 
	}
	else
	{
		if(rc.Right > rcParent.Right)
		{
			int w = rcParent.Right - rc.Left;
			m_pkFocusWnd->Resize(w, -1, true); 
		}

		if(rc.Bottom > rcParent.Bottom)
		{
			int h = rcParent.Bottom-rc.Top;
			m_pkFocusWnd->Resize(-1, h, true); 
		}
	}

	rc = (m_pkFocusWnd->GetParent()) ? m_pkFocusWnd->GetWndRect() : m_pkFocusWnd->GetScreenRect();

	char text[50];
	sprintf(text, "%i", rc.Left); SetWindowText(GetCtrl(IDC_POSX_EB, 0), text);
	sprintf(text, "%i", rc.Top); SetWindowText(GetCtrl(IDC_POSY_EB, 0), text);
	sprintf(text, "%i", rc.Width()); SetWindowText(GetCtrl(IDC_WIDTH_EB, 0), text);
	sprintf(text, "%i", rc.Height()); SetWindowText(GetCtrl(IDC_HEIGHT_EB, 0), text);
}

bool ZGuiEd::LoadGUI(const char* szFile)
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

bool ZGuiEd::NewGUI(bool bConfirm)
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
		if(removelist[k] != pkMainWnd)
			m_pkGui->UnregisterWindow(removelist[k]);

	ZGuiWnd::m_pkPrevWndUnderCursor = NULL;
	ZGuiWnd::m_pkPrevWndClicked = NULL;
	ZGuiWnd::m_pkFocusWnd = NULL;
	ZGuiWnd::m_pkWndClicked = NULL; 
	ZGuiWnd::m_pkWndUnderCursor = NULL;

	m_pkFocusWnd = NULL;
	m_pkCopyWnd = NULL;
	m_pkCopySkin = NULL;
	m_vCopySkinDesc.clear();

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
	SelNewSkin(0);

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
	ShowSpecialControls();

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
		SendDlgItemMessage(g_kDlgBoxRight, IDC_WINDOW_LIST, LB_SETCURSEL, -1, 0);
		SendDlgItemMessage(g_kDlgBoxRight, IDC_TEXTURE_LIST, LB_SETCURSEL, -1, 0);
		SendDlgItemMessage(g_kDlgBoxBottom, IDC_SKINELEMENTS_LIST, LB_SETCURSEL, -1, 0);		
		CheckDlgButton(g_kDlgBoxBottom, IDC_FREE_MOVEMENT_CB, BST_UNCHECKED);

		CheckDlgButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_TOPLEFT, BST_UNCHECKED);
		CheckDlgButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_CENTERVERT, BST_UNCHECKED);
		CheckDlgButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_TOPRIGHT, BST_UNCHECKED);
		CheckDlgButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_CENTER, BST_UNCHECKED);
		CheckDlgButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_BOTTOMLEFT, BST_UNCHECKED);
		CheckDlgButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_CENTERHORZ, BST_UNCHECKED);
		CheckDlgButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_BOTTOMRIGHT, BST_UNCHECKED);

		CheckDlgButton(g_kDlgBoxBottom, IDC_RESIZETYPE_DONT, BST_UNCHECKED);
		CheckDlgButton(g_kDlgBoxBottom, IDC_RESIZETYPE_HEIGHT, BST_UNCHECKED);
		CheckDlgButton(g_kDlgBoxBottom, IDC_RESIZETYPE_WIDTH, BST_UNCHECKED);
		CheckDlgButton(g_kDlgBoxBottom, IDC_RESIZETYPE_BOTH, BST_UNCHECKED);

	/*	CheckDlgButton(g_kDlgBoxBottom, IDC_SKINTYPE_HORZBORDER_RB, BST_UNCHECKED);
		CheckDlgButton(g_kDlgBoxBottom, IDC_SKINTYPE_VERTBORDER_RB, BST_UNCHECKED);
		CheckDlgButton(g_kDlgBoxBottom, IDC_SKINTYPE_BACKGROUND_RB, BST_UNCHECKED);
		CheckDlgButton(g_kDlgBoxBottom, IDC_SKINTYPE_CORNERBORDER_RB, BST_UNCHECKED);*/

		UpdateSkinList();
		return;
	}

	SetWindowText(GetCtrl(IDC_WINDOW_NAMEID_EB, 0), m_pkFocusWnd->GetName());
	SetWindowText(GetCtrl(IDC_TEXTLABEL_EB, 0), m_pkFocusWnd->GetText());

	char text[25];
	Rect rc;

	if(m_pkFocusWnd->GetParent())
	{
		rc = m_pkFocusWnd->GetWndRect();
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
	if( GetWndType(m_pkFocusWnd) == Wnd && m_pkFocusWnd->GetParent() != NULL)
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

	if(m_pkFocusWnd->m_iWndAlignment == TopLeft)
		CheckRadioButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_TOPLEFT, IDC_WNDALIGNMENT_BOTTOMRIGHT, IDC_WNDALIGNMENT_TOPLEFT);
	if(m_pkFocusWnd->m_iWndAlignment == TopRight)
		CheckRadioButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_TOPLEFT, IDC_WNDALIGNMENT_BOTTOMRIGHT, IDC_WNDALIGNMENT_TOPRIGHT);
	if(m_pkFocusWnd->m_iWndAlignment == BottomLeft)
		CheckRadioButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_TOPLEFT, IDC_WNDALIGNMENT_BOTTOMRIGHT, IDC_WNDALIGNMENT_BOTTOMLEFT);		
	if(m_pkFocusWnd->m_iWndAlignment == BottomRight)
		CheckRadioButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_TOPLEFT, IDC_WNDALIGNMENT_BOTTOMRIGHT, IDC_WNDALIGNMENT_BOTTOMRIGHT);
	if(m_pkFocusWnd->m_iWndAlignment == CenterHorz)
		CheckRadioButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_TOPLEFT, IDC_WNDALIGNMENT_BOTTOMRIGHT, IDC_WNDALIGNMENT_CENTERHORZ);
	if(m_pkFocusWnd->m_iWndAlignment == CenterVert)
		CheckRadioButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_TOPLEFT, IDC_WNDALIGNMENT_BOTTOMRIGHT, IDC_WNDALIGNMENT_CENTERVERT);
	if(m_pkFocusWnd->m_iWndAlignment == Center)
		CheckRadioButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_TOPLEFT, IDC_WNDALIGNMENT_BOTTOMRIGHT, IDC_WNDALIGNMENT_CENTER);

	if(m_pkFocusWnd->m_iResizeType == eNone)
		CheckRadioButton(g_kDlgBoxBottom, IDC_RESIZETYPE_DONT, IDC_RESIZETYPE_BOTH, IDC_RESIZETYPE_DONT);
	if(m_pkFocusWnd->m_iResizeType == ResizeWidth)
		CheckRadioButton(g_kDlgBoxBottom, IDC_RESIZETYPE_DONT, IDC_RESIZETYPE_BOTH, IDC_RESIZETYPE_WIDTH);
	if(m_pkFocusWnd->m_iResizeType == ResizeHeight)
		CheckRadioButton(g_kDlgBoxBottom, IDC_RESIZETYPE_DONT, IDC_RESIZETYPE_BOTH, IDC_RESIZETYPE_HEIGHT);
	if(m_pkFocusWnd->m_iResizeType == Resize)
		CheckRadioButton(g_kDlgBoxBottom, IDC_RESIZETYPE_DONT, IDC_RESIZETYPE_BOTH, IDC_RESIZETYPE_BOTH);

	UpdateSkinList();
}

void ZGuiEd::TestGUI()
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
			//AddSampleCtrlItem(it->second);
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

		m_bGuiHaveFocus = false;
		m_pkGui->m_bHandledMouse = false;
	}
}

void ZGuiEd::OpenSelectFontDlg()
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

// Lägg till några item till listboxar så att knapparna skall synas
// (annars ser man inte dom när ändringar görs).
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
		
		AddTreeItem((char*)pkWnd->GetName(), "SampleIDTreebox", NULL, "Sample treebox Sample treebox Sample treebox Sample treebox", 1, 2);
		AddTreeItem((char*)pkWnd->GetName(), "SampleIDTreebox1", NULL, "Sample treebox Sample treebox Sample treebox Sample treebox", 2, 1);

		for(int i=0; i<20; i++)
		{
			char szNameID[50];
			sprintf(szNameID, "SampleIDTreebox%i", i+3);
			AddTreeItem((char*)pkWnd->GetName(), szNameID, NULL, "Sample treebox Sample treebox Sample treebox Sample treebox", 0, 1);
		}
		
		break;
	}
}

void ZGuiEd::CopyWnd()
{
	if(m_pkFocusWnd == NULL)
		return;

	m_pkCopyWnd = m_pkFocusWnd;

}

void ZGuiEd::CutWnd()
{
	//if(m_pkFocusWnd == NULL)
	//	return;

	//ZGuiWnd* pkCutWnd = m_pkFocusWnd;

	//map<string, ZGuiWnd*> kWndsBefore;
	//m_pkGuiMan->GetWindows(kWndsBefore);

	//CopyWnd();
	//PasteWnd();

	//DeleteSelWindow(false);
}

void ZGuiEd::PasteWnd()
{
	if(m_pkCopyWnd == NULL)
		return;

	ZGuiWnd* pkMainWnd = GetWnd("GuiMainWnd");

	ZGuiWnd* kNewSelWnd = NULL;
	vector<ZGuiWnd*> kWnds, kAddList;

	map<string, ZGuiWnd*> kAllWndMap;
	m_pkGuiMan->GetWindows(kAllWndMap);
	for( map<string, ZGuiWnd*>::iterator it = kAllWndMap.begin(); it != kAllWndMap.end(); it++)
		kWnds.push_back(it->second);

	// Skapa mainwindow fönstret
	if(GetWndType(m_pkCopyWnd) == Wnd)
	{
		CreateNewWindow(m_pkCopyWnd);	
		kAddList.push_back(m_pkCopyWnd);
		kNewSelWnd = m_pkFocusWnd;
	}

	// Skapa alla eventuella tabcontrols och dess sidor
	for(int i=0; i<kWnds.size(); i++)
		if(GetWndType(kWnds[i]) == TabControl && kWnds[i]->GetParent() == m_pkCopyWnd)
		{
			CreateNewWindow(kWnds[i]); // skapa tab controllen
			kAddList.push_back(kWnds[i]);

			ZGuiTabCtrl* pkTabControl = (ZGuiTabCtrl*) m_pkFocusWnd;
			
			for(int j=0; j<((ZGuiTabCtrl*)kWnds[i])->GetNumPages(); j++)
			{								
				ZGuiWnd* pkPage = ((ZGuiTabCtrl*)kWnds[i])->GetPage(j);

				CreateNewWindow(pkPage); // skapa sidan
				kAddList.push_back(pkPage);

				// Sätt fokus på sidan
				pkTabControl->SetCurrentPage(j);
				m_pkFocusWnd = pkTabControl->GetPage(j);

				// Skapa alla knappar och annat på sidorna
				list<ZGuiWnd*> kChilds; pkPage->GetChildrens(kChilds);
				for(list<ZGuiWnd*>::iterator it = kChilds.begin(); it!=kChilds.end(); it++)	
				{
					CreateNewWindow(*it);		
					kAddList.push_back(*it);
				}

				m_pkFocusWnd = pkTabControl;
			}

			pkTabControl->SetCurrentPage(0);
		}

	for(int i=0; i<kWnds.size(); i++)
	{
		if(kWnds[i]->GetParent() == m_pkCopyWnd || kWnds[i] == m_pkCopyWnd)
		{
			if(!AlreadyInList(kAddList, kWnds[i]))
				CreateNewWindow(kWnds[i]);
		}
	}

	if(kNewSelWnd != NULL)
		m_pkFocusWnd = kNewSelWnd;
}

void ZGuiEd::ShowSpecialControls()
{
	int x = 5, y = 141;
	if(m_pkFocusWnd != NULL && GetWndType(m_pkFocusWnd) == Textbox)
	{
		ShowWindow(GetDlgItem(g_kDlgBoxBottom, IDC_MULTILINE_CB), SW_SHOW);
		SetWindowPos(GetDlgItem(g_kDlgBoxBottom, IDC_MULTILINE_CB), NULL,
			x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		x += (GetWindowSize(IDC_MULTILINE_CB, false, true) + 5);
		CheckDlgButton(g_kDlgBoxBottom, IDC_MULTILINE_CB, 
			((ZGuiTextbox*) m_pkFocusWnd)->IsMultiLine()); 
	}
	else
		ShowWindow(GetDlgItem(g_kDlgBoxBottom, IDC_MULTILINE_CB), SW_HIDE);


}