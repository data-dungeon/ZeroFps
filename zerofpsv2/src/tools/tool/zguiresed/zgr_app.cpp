#include "zgr_app.h"
#include "../../../zerofpsv2/engine/inputhandle.h"

void ZGuiResEd::OnIdle()
{
	ManageGuiFocus();

	if(m_bTestGUI && m_iTask != 11)
		return;

	if(m_iTask == TASK_LOAD_SCRIPT)
	{
		map<string, ZGuiWnd*> kWnds;
		m_pkGuiMan->GetWindows(kWnds);
		int size = kWnds.size();

		if(NewGUI(size > 1))
			if(LoadGUI(m_strNewFileToLoad.c_str()))
			{
				SetTitle(string("ZeroFps GUI Editor - ") + m_strNewFileToLoad);
				m_strOpenFile = m_strNewFileToLoad;
			}

		m_iTask = TASK_DO_NOTHING;
	}
	if(m_iTask == TASK_CREATE_NEW_WINDOW) 
	{ 
		CreateNewWindow(); 
		m_iTask = TASK_DO_NOTHING; 
	}
	if(m_iTask == TASK_DELETE_SEL_WINDOW) 
	{ 
		bool bConfirm = m_pkFocusWnd && 
			GetWndType(m_pkFocusWnd) == Wnd && m_pkFocusWnd->GetNumChildrens() != 0;
		DeleteSelWindow(bConfirm); 
		m_iTask = TASK_DO_NOTHING; 
	}
	if(m_iTask == TASK_NEW_GUI) 
	{
		map<string, ZGuiWnd*> kWnds;
		m_pkGuiMan->GetWindows(kWnds);
		int size = kWnds.size();

		NewGUI(size>1); 
		m_iTask = TASK_DO_NOTHING; 
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
			if(char* szName = GetSelItem("GuiEd_ScriptList"))
			{
				unsigned char r,g,b;
				m_pkFocusWnd->GetTextColor(r,g,b); 
				SetFont(m_pkFocusWnd->GetName(), szName, r, g, b);				
			}			
		}	
		ShowWnd("GuiEd_SelectFileWnd", false);
		m_iTask = TASK_DO_NOTHING;
	}
	if(m_iTask == TASK_SAVE_SCRIPT) 
	{ 
		if(SaveScript(m_strNewFileToLoad.c_str(), m_bConfirmSaveScript))
		{
			UpdateScriptList();
			char msg[512];
			sprintf(msg, "Script file: \r\n\"%s\"\r\nsaved successfully!\r\n", m_strNewFileToLoad.c_str());
			MsgBox(msg, "ZGuiEd", MSGBOX_OK);
			m_strNewFileToLoad = "";
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
			string strSelSkin;
			
			char* szItem;
			if((szItem=GetSelItem("GuiEd_SkinElementsList"))!=NULL)
				strSelSkin = szItem;

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
	if(m_bEditorMode == false) HandleInput();
}

void ZGuiResEd::RenderInterface()		
{
	static ZGuiWnd* pkSelectFileWnd = NULL;

	if(pkSelectFileWnd == NULL)
		pkSelectFileWnd = GetWnd("GuiEd_SelectFileWnd");

	if(m_pkFocusWnd && !m_bTestGUI && !pkSelectFileWnd->IsVisible() )
	{
		Rect rc = m_pkFocusWnd->GetScreenRect();
		m_pkGui->SetLineColor(255,0,255);
		m_pkGui->DrawLine(Point(rc.Left,rc.Bottom), Point(rc.Left,rc.Top));
		m_pkGui->DrawLine(Point(rc.Left,rc.Top), Point(rc.Right,rc.Top));
		m_pkGui->DrawLine(Point(rc.Right,rc.Top), Point(rc.Right,rc.Bottom));
		m_pkGui->DrawLine(Point(rc.Right,rc.Bottom), Point(rc.Left,rc.Bottom));
	}
}

void ZGuiResEd::DeleteSelWindow(bool bConfirm)
{
	if(m_pkFocusWnd)
	{
		if(bConfirm)
		{
			MsgBox("Are u sure?", "Delete Wnd", MSGBOX_YESNO);
			return;
		}

		//string id = m_pkFocusWnd->GetName();
		//set<string>::iterator it = m_kFreemovementWnds.find(id);
		//if(it != m_kFreemovementWnds.end())
		//	m_kFreemovementWnds.erase(it);

		map<string,SPECIAL_WND_INFO>::iterator itWndInfo;
		itWndInfo = m_kSpecialWndInfo.find(m_pkFocusWnd->GetName());
		if(itWndInfo != m_kSpecialWndInfo.end())
		{
			m_kSpecialWndInfo.erase(itWndInfo);
		}

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

void ZGuiResEd::CreateNewWindow(ZGuiWnd* pkCloneTarget)
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
		
		if(char* selitem = GetSelItem("GuiEd_WidgetTypeList"))
			strcpy(szWnd, selitem);
		else
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
		CheckButton("GuiEd_ForceCaptureToSel", false);
		
		FilterWnd();

		UpdateInfo();

		if(pkCloneTarget)
		{
			CloneSkins(m_pkFocusWnd, pkCloneTarget);
			m_pkFocusWnd->SetFont(pkCloneTarget->GetFont());
			unsigned char r,g,b;
			pkCloneTarget->GetTextColor(r,g,b);
			m_pkFocusWnd->SetTextColor(r,g,b);
		}

		if(UpdateSkinList())
			SelNewSkin(0);

		AddSampleCtrlItem(m_pkFocusWnd);

		ShowSpecialControls();
	}
}

void ZGuiResEd::CheckMovement()
{
	if(m_pkFocusWnd == NULL)
		return;

	bool bFreeMovement = false;

	map<string,SPECIAL_WND_INFO>::iterator itWndInfo = 
		m_kSpecialWndInfo.find(m_pkFocusWnd->GetName());
	if(itWndInfo != m_kSpecialWndInfo.end())
	{
		if(itWndInfo->second.bFreemovement)
			bFreeMovement = true;
	}

	Rect rc = m_pkFocusWnd->GetScreenRect();

	if(bFreeMovement == false)
	{
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
				//string id = (*it)->GetName();
				//if(m_kFreemovementWnds.find(id) == m_kFreemovementWnds.end())
				map<string,SPECIAL_WND_INFO>::iterator itWndInfo = 
					m_kSpecialWndInfo.find((*it)->GetName());
				if(itWndInfo == m_kSpecialWndInfo.end())
				{
					Rect rcChild = (*it)->GetScreenRect();
					if(rcChild.Right > iRight)
						iRight = rcChild.Right;
					if(rcChild.Bottom > iBottom)
						iBottom = rcChild.Bottom;
				}
			}

			if(rc.Right < iRight)
				m_pkFocusWnd->Resize(iRight-rc.Left, -1);

			if(rc.Bottom < iBottom)
				m_pkFocusWnd->Resize(-1, iBottom-rc.Top); 
		}

		rcParent = (m_pkFocusWnd->GetParent()) ? 
			m_pkFocusWnd->GetParent()->GetScreenRect() : GetWnd("GuiEd_DesignBorder")->GetScreenRect();
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
	}

	rc = (m_pkFocusWnd->GetParent()) ? m_pkFocusWnd->GetWndRect() : m_pkFocusWnd->GetScreenRect();

	SetTextInt("GuiEd_WndPosX", rc.Left);
	SetTextInt("GuiEd_WndPosY", rc.Top);
	SetTextInt("GuiEd_WndWidth", rc.Width());
	SetTextInt("GuiEd_WndHeight", rc.Height());
}

bool ZGuiResEd::LoadGUI(const char* szFile)
{
	map<string, ZGuiWnd*> kWndsBefore;
	m_pkGuiMan->GetWindows(kWndsBefore);

	if(!LoadGuiFromScript((char*)szFile))
		return false;				
	
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
			ZGuiWnd* pkParent = addlist[k]->GetParent();

			addlist[k]->m_iZValue = m_iZValueCounter++;
			AddSampleCtrlItem(addlist[k]);			

			SPECIAL_WND_INFO info;			
			info.bHiddenFromStart = (addlist[k]->IsVisible(false) == false);		
			info.bFreemovement = false;

			if(pkParent == NULL)
			{
				if(addlist[k]->GetMoveArea() != addlist[k]->GetScreenRect())
					info.bFreemovement = true;
			}
			else
			{
				Rect rc = addlist[k]->GetScreenRect();
				Rect rcParent = pkParent->GetScreenRect();

				if(rc.Left < rcParent.Left || rc.Right > rcParent.Right ||
					rc.Top < rcParent.Top || rc.Bottom > rcParent.Bottom)
					info.bFreemovement = true;
			}
			
			m_kSpecialWndInfo.insert(
				map<string,SPECIAL_WND_INFO>::value_type(
					addlist[k]->GetName(), info)); 

			if(pkParent && GetWndType(pkParent) != TabControl)
				addlist[k]->Show();
		}
	}

	FilterWnd();

	SwitchDesignRes(m_kDesignResolution.x, m_kDesignResolution.y, true);

	return true;
}

bool ZGuiResEd::NewGUI(bool bConfirm)
{
//	if(bConfirm && MessageBox(GetParent(g_kDlgBoxRight), "Are u sure?", "New?", MB_YESNO) == IDNO)
//		return false;

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
		if(!IsEditorWnd(removelist[k]))
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
	m_kSpecialWndInfo.clear();
	//m_kFreemovementWnds.clear();

	SetText("GuiEd_WndIDName", "");
	SetText("GuiEd_WndParent", "");
	SetText("GuiEd_WndCaption", "");
	SetText("GuiEd_WndPosX", "");
	SetText("GuiEd_WndPosY", "");
	SetText("GuiEd_WndWidth", "");
	SetText("GuiEd_WndHeight", "");
	SetText("GuiEd_WndZValue", "");		

	CheckButton("GuiEd_SelBackgroundTexture", true);

	ClearListbox("GuiEd_WidgetList");

	UpdateInfo();
	SelNewSkin(0);

	return true;
}

GuiType ZGuiResEd::FormatWndType(string strText)
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

string ZGuiResEd::FormatWndType(GuiType eType)
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

string ZGuiResEd::FormatWndAlignent(ZGuiWnd* pkWnd)
{
	string strText = "";

	WndAlignent eAlignment = WndAlignent(pkWnd->m_iWndAlignment);

	if(eAlignment == TopLeft)		strText = "TopLeft";
	if(eAlignment == TopRight)		strText = "TopRight";
	if(eAlignment == BottomLeft)	strText = "BottomLeft";	
	if(eAlignment == BottomRight)	strText = "BottomRight";	
	if(eAlignment == CenterHorz)	strText = "CenterHorz";	
	if(eAlignment == CenterVert)	strText = "CenterVert";	
	if(eAlignment == Center)		strText = "Center";	

	return strText;
}

string ZGuiResEd::FormatWndResizeType(ZGuiWnd* pkWnd)
{
	string strText = "";

	WndResizeType eType = WndResizeType(pkWnd->m_iResizeType);

	if(eType == eNone)				strText = "eNone";
	if(eType == ResizeWidth)		strText = "ResizeWidth";
	if(eType == ResizeHeight)		strText = "ResizeHeight";	
	if(eType == Resize)				strText = "Resize";	

	return strText;
}
void ZGuiResEd::FilterWnd()
{
	bool bFilter = IsButtonChecked("GuiEd_FilterToType");

	map<string, ZGuiWnd*> kWindows;
	vector<string> vkWndNames;

	if(char* type = GetSelItem("GuiEd_WidgetTypeList"))
	{
		GuiType eType = FormatWndType(type);

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

		ClearListbox("GuiEd_WidgetList");
		
		for(int i=0; i<vkWndNames.size(); i++)
			if(!IsEditorWnd(GetWnd(vkWndNames[i])))
				AddListItem("GuiEd_WidgetList", (char*) vkWndNames[i].c_str());

		if(m_pkFocusWnd)
			SelListItem("GuiEd_WidgetList",(char*)m_pkFocusWnd->GetName());
		else
			SelListItem("GuiEd_WidgetList",NULL);
	}

}

void ZGuiResEd::UpdateInfo()
{
	ShowSpecialControls();

	if(m_pkFocusWnd == NULL)
	{
		SetText("GuiEd_WndIDName", "");
		SetText("GuiEd_WndParent", "");
		SetText("GuiEd_WndCaption", "");
		SetText("GuiEd_WndPosX", "");
		SetText("GuiEd_WndPosY", "");
		SetText("GuiEd_WndWidth", "");
		SetText("GuiEd_WndHeight", "");
		SetText("GuiEd_WndZValue", "");		
		SetText("GuiEd_WndType", "");		
		
		SelListItem("GuiEd_WidgetList",NULL);
		SelListItem("GuiEd_SkinElementsList", NULL);
		SelListItem("GuiEd_TextureList", NULL);
		
		CheckButton("GuiEd_Alignment1", true);
		CheckButton("GuiEd_Alignment2", true);
		CheckButton("GuiEd_Alignment3", true);
		CheckButton("GuiEd_Alignment4", true);
		CheckButton("GuiEd_Alignment5", true);
		CheckButton("GuiEd_Alignment6", true);
		CheckButton("GuiEd_Alignment7", true);

		//CheckDlgButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_TOPLEFT, BST_UNCHECKED);
		//CheckDlgButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_CENTERVERT, BST_UNCHECKED);
		//CheckDlgButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_TOPRIGHT, BST_UNCHECKED);
		//CheckDlgButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_CENTER, BST_UNCHECKED);
		//CheckDlgButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_BOTTOMLEFT, BST_UNCHECKED);
		//CheckDlgButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_CENTERHORZ, BST_UNCHECKED);
		//CheckDlgButton(g_kDlgBoxBottom, IDC_WNDALIGNMENT_BOTTOMRIGHT, BST_UNCHECKED);

		//CheckDlgButton(g_kDlgBoxBottom, IDC_RESIZETYPE_DONT, BST_UNCHECKED);
		//CheckDlgButton(g_kDlgBoxBottom, IDC_RESIZETYPE_HEIGHT, BST_UNCHECKED);
		//CheckDlgButton(g_kDlgBoxBottom, IDC_RESIZETYPE_WIDTH, BST_UNCHECKED);
		//CheckDlgButton(g_kDlgBoxBottom, IDC_RESIZETYPE_BOTH, BST_UNCHECKED);

		CheckButton("GuiEd_Freemovement", false);
		CheckButton("GuiEd_HiddenFromStart", false);

		UpdateSkinList();
		return;
	}

	SetText("GuiEd_WndIDName", (char*)m_pkFocusWnd->GetName());
	SetText("GuiEd_WndCaption", (char*)m_pkFocusWnd->GetText());

	char text[25];
	Rect rc;

	if(m_pkFocusWnd->GetParent())
	{
		rc = m_pkFocusWnd->GetWndRect();
		SetText("GuiEd_WndParent", (char*)m_pkFocusWnd->GetParent()->GetName());
	}
	else
	{
		rc = m_pkFocusWnd->GetScreenRect();
		SetText("GuiEd_WndParent", "");
	}

	sprintf(text, "%i", rc.Left); SetText("GuiEd_WndPosX", text);
	sprintf(text, "%i", rc.Top); SetText("GuiEd_WndPosY", text);
	sprintf(text, "%i", rc.Width()); SetText("GuiEd_WndWidth", text);
	sprintf(text, "%i", rc.Height()); SetText("GuiEd_WndHeight", text);
	sprintf(text, "%i", rc.Height()); SetText("GuiEd_WndHeight", text);
	SetTextInt("GuiEd_WndZValue", m_pkFocusWnd->m_iZValue);
	SetText("GuiEd_WndType", (char*)FormatWndType(GetWndType(m_pkFocusWnd)).c_str());

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
	CheckButton("GuiEd_ToggleShow", true);

	if(m_pkFocusWnd->GetMoveArea() == m_pkFocusWnd->GetScreenRect()) 
		CheckButton("GuiEd_Freemovement", false);
	else
		CheckButton("GuiEd_Freemovement", true);

	if(m_pkFocusWnd->m_iWndAlignment == TopLeft) CheckButton("GuiEd_Alignment1", true);
	if(m_pkFocusWnd->m_iWndAlignment == TopRight) CheckButton("GuiEd_Alignment2", true);
	if(m_pkFocusWnd->m_iWndAlignment == BottomRight) CheckButton("GuiEd_Alignment3", true);
	if(m_pkFocusWnd->m_iWndAlignment == BottomLeft) CheckButton("GuiEd_Alignment4", true);
	if(m_pkFocusWnd->m_iWndAlignment == CenterHorz) CheckButton("GuiEd_Alignment5", true);
	if(m_pkFocusWnd->m_iWndAlignment == CenterVert) CheckButton("GuiEd_Alignment6", true);
	if(m_pkFocusWnd->m_iWndAlignment == Center) CheckButton("GuiEd_Alignment7", true);

	//if(m_pkFocusWnd->m_iResizeType == eNone)
	//	CheckRadioButton(g_kDlgBoxBottom, IDC_RESIZETYPE_DONT, IDC_RESIZETYPE_BOTH, IDC_RESIZETYPE_DONT);
	//if(m_pkFocusWnd->m_iResizeType == ResizeWidth)
	//	CheckRadioButton(g_kDlgBoxBottom, IDC_RESIZETYPE_DONT, IDC_RESIZETYPE_BOTH, IDC_RESIZETYPE_WIDTH);
	//if(m_pkFocusWnd->m_iResizeType == ResizeHeight)
	//	CheckRadioButton(g_kDlgBoxBottom, IDC_RESIZETYPE_DONT, IDC_RESIZETYPE_BOTH, IDC_RESIZETYPE_HEIGHT);
	//if(m_pkFocusWnd->m_iResizeType == Resize)
	//	CheckRadioButton(g_kDlgBoxBottom, IDC_RESIZETYPE_DONT, IDC_RESIZETYPE_BOTH, IDC_RESIZETYPE_BOTH);

	map<string,SPECIAL_WND_INFO>::iterator itWndInfo;
	itWndInfo = m_kSpecialWndInfo.find(m_pkFocusWnd->GetName());
	if(itWndInfo != m_kSpecialWndInfo.end())
	{
		CheckButton("GuiEd_HiddenFromStart", itWndInfo->second.bHiddenFromStart);
		CheckButton("GuiEd_Freemovement", itWndInfo->second.bFreemovement);
	}
	else
	{
		CheckButton("GuiEd_HiddenFromStart", false);
		CheckButton("GuiEd_Freemovement", false);
	}

	
	

	UpdateSkinList();
}

void ZGuiResEd::TestGUI()
{
	m_bTestGUI = !m_bTestGUI;

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

		m_pkCamera->SetClearColor(Vector4(0.44f,0.14f,0.14f,0));
		GetWnd("GuiEd_EditWnd")->Hide();
		GetWnd("GuiEd_NewWnd")->Hide();
		GetWnd("GuiEd_SelectFileWnd")->Hide();

		m_pkGui->Activate(true); 
	}
	else
	{
		m_pkGui->Activate(false);

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

			if(!IsEditorWnd(it->second))
				AddSampleCtrlItem(it->second);
		}

		m_bGuiHaveFocus = false;
		m_pkGui->m_bHandledMouse = false;

		m_pkCamera->SetClearColor(Vector4(0.44f,0.44f,0.44f,0));
		GetWnd("GuiEd_EditWnd")->Show();
		GetWnd("GuiEd_NewWnd")->Show();
	}
}

void ZGuiResEd::OpenSelectFontDlg()
{
	m_bSelectFont = true;

	ShowWnd("GuiEd_SelectFileWnd", true, true, true);

	ZFVFileSystem* m_pkFileSys = reinterpret_cast<ZFVFileSystem*>(
		g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));	

	vector<string> t;
	m_pkFileSys->ListDir(&t, "data/textures/gui/fonts/");

	ClearListbox("GuiEd_ScriptList");

	for(int i=0; i<t.size(); i++) 
	{
		char *ext = strrchr( t[i].c_str(), '.');
		if(ext != NULL && strcmp(ext, ".fnt")==0)
		{			
			t[i].erase(t[i].find(".fnt"), 4);			
			AddListItem("GuiEd_ScriptList", (char*) t[i].c_str());
		}
	}

	string strFontName = "defguifont";

	if(m_pkFocusWnd)
		strFontName = m_pkFocusWnd->GetFont()->m_szNameID;

	SelListItem("GuiEd_ScriptList", (char*)strFontName.c_str());
}

void ZGuiResEd::UpdateScriptList()
{
	ClearListbox("GuiEd_ScriptList");

	ZFVFileSystem* m_pkFileSys = reinterpret_cast<ZFVFileSystem*>(
		g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));	

	vector<string> t;
	m_pkFileSys->ListDir(&t, "data/script/gui/");

	for(int i=0; i<t.size(); i++) 
	{
		if(t[i].find(".lua") != string::npos)
			AddListItem("GuiEd_ScriptList", (char*) t[i].c_str());
	}
}

// Lägg till några item till listboxar så att knapparna skall synas
// (annars ser man inte dom när ändringar görs).
void ZGuiResEd::AddSampleCtrlItem(ZGuiWnd* pkWnd)
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

void ZGuiResEd::CopyWnd()
{
	if(m_pkFocusWnd == NULL)
		return;

	m_pkCopyWnd = m_pkFocusWnd;

}

void ZGuiResEd::CutWnd()
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

void ZGuiResEd::PasteWnd()
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

void ZGuiResEd::ShowSpecialControls()
{
/*	int x = 5, y = 141;
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

	if(m_pkFocusWnd != NULL &&
		GetWndType(m_pkFocusWnd) == Slider || 
		GetWndType(m_pkFocusWnd) == Scrollbar)
	{
		ShowWindow(GetDlgItem(g_kDlgBoxBottom, IDC_HORIZONTAL_CB), SW_SHOW);
		SetWindowPos(GetDlgItem(g_kDlgBoxBottom, IDC_HORIZONTAL_CB), NULL,
			x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		x += (GetWindowSize(IDC_HORIZONTAL_CB, false, true) + 5);
	}
	else
		ShowWindow(GetDlgItem(g_kDlgBoxBottom, IDC_HORIZONTAL_CB), SW_HIDE);

	if(m_pkFocusWnd && GetWndType(m_pkFocusWnd) == Slider)
		CheckDlgButton(g_kDlgBoxBottom, IDC_HORIZONTAL_CB, 
			((ZGuiSlider*) m_pkFocusWnd)->GetCtrlStyle(SCF_HORZ)); 
	else
	if(m_pkFocusWnd && GetWndType(m_pkFocusWnd) == Scrollbar)
		CheckDlgButton(g_kDlgBoxBottom, IDC_HORIZONTAL_CB, 
		((ZGuiScrollbar*) m_pkFocusWnd)->IsHorizontal()); 

	bool bIsProgessbar = (GetWndType(m_pkFocusWnd) == Progressbar);
	
	int show_state = bIsProgessbar ? SW_SHOW : SW_HIDE;

	ShowWindow(GetDlgItem(g_kDlgBoxBottom, IDC_PROGRESSBAR_DIR1_RB), show_state);
	ShowWindow(GetDlgItem(g_kDlgBoxBottom, IDC_PROGRESSBAR_DIR2_RB), show_state);
	ShowWindow(GetDlgItem(g_kDlgBoxBottom, IDC_PROGRESSBAR_DIR3_RB), show_state);
	ShowWindow(GetDlgItem(g_kDlgBoxBottom, IDC_PROGRESSBAR_DIR4_RB), show_state);
	ShowWindow(GetDlgItem(g_kDlgBoxBottom, IDC_PROGRESSBAR_DIR_GROUP_LABEL), show_state);
	ShowWindow(GetDlgItem(g_kDlgBoxBottom, IDC_PROGRESSBAR_TEXTORIENT_GROUP_LABEL), show_state);
	ShowWindow(GetDlgItem(g_kDlgBoxBottom, IDC_PBTEXTORIENT_RB1), show_state);
	ShowWindow(GetDlgItem(g_kDlgBoxBottom, IDC_PBTEXTORIENT_RB2), show_state);
	ShowWindow(GetDlgItem(g_kDlgBoxBottom, IDC_PBTEXTORIENT_RB3), show_state);
	ShowWindow(GetDlgItem(g_kDlgBoxBottom, IDC_PBTEXTORIENT_RB4), show_state);
	ShowWindow(GetDlgItem(g_kDlgBoxBottom, IDC_PBTEXTORIENT_RB5), show_state);
	ShowWindow(GetDlgItem(g_kDlgBoxBottom, IDC_PBTEXTORIENT_RB6), show_state);
	
	if(bIsProgessbar)
	{
		x += GetWindowSize(IDC_PROGRESSBAR_DIR_GROUP_LABEL, false, true);
		x += GetWindowSize(IDC_PROGRESSBAR_TEXTORIENT_GROUP_LABEL, false, true);

		if(((ZGuiProgressbar*) m_pkFocusWnd)->GetDir() == PBDIR_LEFT_TO_RIGHT)
			CheckRadioButton(g_kDlgBoxBottom, IDC_PROGRESSBAR_DIR1_RB, 
				IDC_PROGRESSBAR_DIR4_RB, IDC_PROGRESSBAR_DIR1_RB);
		if(((ZGuiProgressbar*) m_pkFocusWnd)->GetDir() == PBDIR_RIGHT_TO_LEFT)
			CheckRadioButton(g_kDlgBoxBottom, IDC_PROGRESSBAR_DIR1_RB, 
				IDC_PROGRESSBAR_DIR4_RB, IDC_PROGRESSBAR_DIR2_RB);
		if(((ZGuiProgressbar*) m_pkFocusWnd)->GetDir() == PBDIR_TOP_TO_BOTTOM)
			CheckRadioButton(g_kDlgBoxBottom, IDC_PROGRESSBAR_DIR1_RB, 
				IDC_PROGRESSBAR_DIR4_RB, IDC_PROGRESSBAR_DIR3_RB);
		if(((ZGuiProgressbar*) m_pkFocusWnd)->GetDir() == PBDIR_BOTTOM_TO_TOP)
			CheckRadioButton(g_kDlgBoxBottom, IDC_PROGRESSBAR_DIR1_RB, 
				IDC_PROGRESSBAR_DIR4_RB, IDC_PROGRESSBAR_DIR4_RB);

		if(((ZGuiProgressbar*) m_pkFocusWnd)->GetTextOrientation() == PBTEXTORIENT_CENTER)
			CheckRadioButton(g_kDlgBoxBottom, IDC_PBTEXTORIENT_RB1, 
				IDC_PBTEXTORIENT_RB6, IDC_PBTEXTORIENT_RB1);
		if(((ZGuiProgressbar*) m_pkFocusWnd)->GetTextOrientation() == PBTEXTORIENT_LEFT)
			CheckRadioButton(g_kDlgBoxBottom, IDC_PBTEXTORIENT_RB1, 
				IDC_PBTEXTORIENT_RB6, IDC_PBTEXTORIENT_RB2);
		if(((ZGuiProgressbar*) m_pkFocusWnd)->GetTextOrientation() == PBTEXTORIENT_OVER)
			CheckRadioButton(g_kDlgBoxBottom, IDC_PBTEXTORIENT_RB1, 
				IDC_PBTEXTORIENT_RB6, IDC_PBTEXTORIENT_RB3);
		if(((ZGuiProgressbar*) m_pkFocusWnd)->GetTextOrientation() == PBTEXTORIENT_RIGHT)
			CheckRadioButton(g_kDlgBoxBottom, IDC_PBTEXTORIENT_RB1, 
				IDC_PBTEXTORIENT_RB6, IDC_PBTEXTORIENT_RB4);
		if(((ZGuiProgressbar*) m_pkFocusWnd)->GetTextOrientation() == PBTEXTORIENT_UNDER)
			CheckRadioButton(g_kDlgBoxBottom, IDC_PBTEXTORIENT_RB1, 
				IDC_PBTEXTORIENT_RB6, IDC_PBTEXTORIENT_RB5);
		if(((ZGuiProgressbar*) m_pkFocusWnd)->m_bShowText == false)
			CheckRadioButton(g_kDlgBoxBottom, IDC_PBTEXTORIENT_RB1, 
				IDC_PBTEXTORIENT_RB6, IDC_PBTEXTORIENT_RB6);		
	}
*/
}


void ZGuiResEd::RenameSelWnd(char* text)
{
	bool bFreeMovement = false;
	SPECIAL_WND_INFO* pkInfo = NULL;
	
	// Copy special window info and remove from list
	// because the window have changed name and have a diffrent key now.
	map<string,SPECIAL_WND_INFO>::iterator it;
	it = m_kSpecialWndInfo.find(m_pkFocusWnd->GetName());
	if(it != m_kSpecialWndInfo.end())
	{
		pkInfo = new SPECIAL_WND_INFO;
		memcpy(pkInfo, &it->second, sizeof(SPECIAL_WND_INFO));
		m_kSpecialWndInfo.erase(it);
	}

	//string id = m_pkFocusWnd->GetName();
	//set<string>::iterator itFreeMove = m_kFreemovementWnds.find(id);
	//if(itFreeMove != m_kFreemovementWnds.end())
	//{
	//	bFreeMovement = true;
	//	m_kFreemovementWnds.erase(itFreeMove);
	//}

	m_pkGui->ChangeWndRegName(m_pkFocusWnd, text);
	FilterWnd();

	// Insert special window information.
	if(pkInfo != NULL)
	{
		m_kSpecialWndInfo.insert(
			map<string,SPECIAL_WND_INFO>::value_type(
				m_pkFocusWnd->GetName(), *pkInfo)); 
		delete pkInfo;
	}

	//if(bFreeMovement)
	//{
	//	id = m_pkFocusWnd->GetName();
	//	m_kFreemovementWnds.insert(id);
	//}

}
