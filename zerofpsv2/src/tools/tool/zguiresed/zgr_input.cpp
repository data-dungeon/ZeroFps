#include "zgr_app.h"
#include "../../../zerofpsv2/engine/inputhandle.h"

#ifdef WIN32
	#include "shellapi.h" // needed for ShellExecute API call
#endif

void ZGuiResEd::HandleInput()
{
	static bool s_bLeftMouseButtonPressed = false;
	static bool s_bRightMouseButtonPressed = false;
	static bool s_bLShiftPressed = false;
	static bool s_bReturnPressed = false;
	static bool s_bArrowLeftPressed = false;
	static bool s_bArrowRightPressed = false;
	static bool s_bArrowUpPressed = false;
	static bool s_bArrowDownPressed = false;
	static bool s_bLCPressed = false;
	static bool s_bLVPressed = false;
	static bool s_bEscPressed = false;

	if(m_pkFocusWnd)
	{			
		if(m_pkInputHandle->Pressed(KEY_LEFT) && !DelayCommand()){
			Rect rc = m_pkFocusWnd->GetScreenRect(); rc.Left--;
			m_pkFocusWnd->SetPos(rc.Left > 0 ? rc.Left : 0, rc.Top, true, true); 
			CheckMovement();
		}
		if(m_pkInputHandle->Pressed(KEY_RIGHT) && !DelayCommand()) {
			Rect rc = m_pkFocusWnd->GetScreenRect();
			m_pkFocusWnd->SetPos(rc.Left+1, rc.Top, true, true); 
			CheckMovement();
		}
		if(m_pkInputHandle->Pressed(KEY_UP) && !DelayCommand()) {
			Rect rc = m_pkFocusWnd->GetScreenRect(); rc.Top--;
			m_pkFocusWnd->SetPos(rc.Left, rc.Top > 0 ? rc.Top : 0, true, true); 
			CheckMovement();
		}
		if(m_pkInputHandle->Pressed(KEY_DOWN) && !DelayCommand()) {
			Rect rc = m_pkFocusWnd->GetScreenRect();
			m_pkFocusWnd->SetPos(rc.Left, rc.Top+1, true, true); 
			CheckMovement();
		}
	}

	if(m_pkInputHandle->Pressed(KEY_DELETE) && !DelayCommand() ) {
		if(m_pkFocusWnd)
		{
			DeleteSelWindow(GetWndType(m_pkFocusWnd) == Wnd && m_pkFocusWnd->GetNumChildrens() != 0);
		}
	}

	if(m_pkInputHandle->Pressed(MOUSELEFT) && s_bLeftMouseButtonPressed == false)
	{
		int x,y;
		m_pkInputHandle->SDLMouseXY(x,y);

		MouseClick(true, x, y);
		
		if(m_pkInputHandle->Pressed(KEY_LCTRL))
		{
			CopyWnd();
			PasteWnd();
		}
		
		s_bLeftMouseButtonPressed = true;
	}
	else if(!m_pkInputHandle->Pressed(MOUSELEFT))
	{
		s_bLeftMouseButtonPressed = false;
	}

	if(m_pkInputHandle->Pressed(MOUSERIGHT) && s_bRightMouseButtonPressed == false)
	{
		int x,y;
		m_pkInputHandle->SDLMouseXY(x,y);
		MouseClick(false, x,y);
		s_bRightMouseButtonPressed = true;
	}
	else if(!m_pkInputHandle->Pressed(MOUSERIGHT))
		s_bRightMouseButtonPressed = false;

	if(m_pkInputHandle->Pressed(MOUSELEFT))
	{
		int x,y;
		m_pkInputHandle->SDLMouseXY(x,y);
		MouseMove(true, x,y);
	}

	if(m_pkInputHandle->Pressed(KEY_LSHIFT)/*(m_pkGui->m_iKeyPressed == KEY_LSHIFT)*/ && s_bLShiftPressed == false)
	{
		s_bLShiftPressed = true;
		if(m_bResize == false)
		{
			CheckButton("GuiEd_ResizeMode", true);
			m_bResize = true;
		}
	}
	else if(!/*(m_pkGui->m_iKeyPressed == KEY_LSHIFT)*/m_pkInputHandle->Pressed(KEY_LSHIFT))
	{
		if(m_bResize == true && s_bLShiftPressed)
		{
			s_bLShiftPressed = false;
			CheckButton("GuiEd_ResizeMode", false);
			m_bResize = false;
			
#ifdef WIN32
			SetCursor(LoadCursor(NULL, IDC_ARROW));
#endif

		}
	}

	if(m_pkInputHandle->Pressed(KEY_C) && s_bLCPressed == false)
	{
		s_bLCPressed = true;
		if(m_pkInputHandle->Pressed(KEY_LCTRL))						
			CopyWnd();			
	}
	else if(!m_pkInputHandle->Pressed(KEY_C))
		s_bLCPressed = false;

	if(m_pkInputHandle->Pressed(KEY_V) && s_bLVPressed == false)
	{
		s_bLVPressed = true;
		if(m_pkInputHandle->Pressed(KEY_LCTRL))
			PasteWnd();
	}
	else if(!m_pkInputHandle->Pressed(KEY_V))
		s_bLVPressed = false;

}

bool ZGuiResEd::DelayCommand(float delay)
{
	if(m_pkZeroFps->GetEngineTime() < m_fDelayTime)
		return true;
	m_fDelayTime = m_pkZeroFps->GetEngineTime() + delay;

	return false;
}

void ZGuiResEd::MouseClick(bool bLeft, int x, int y)
{
	if(bLeft)
	{
		ZGuiWnd* pkPickWnd;
		
		if(m_bForceCaptureToSel == false)
			pkPickWnd = GetWndFromPoint(x,y);
		else
			pkPickWnd = m_pkFocusWnd;

		if(pkPickWnd && pkPickWnd != GetWnd("GuiMainWnd"))
		{
			m_kCursorRangeDiff.x = x-pkPickWnd->GetScreenRect().Left; 
			m_kCursorRangeDiff.y = y-pkPickWnd->GetScreenRect().Top;
			m_pkFocusWnd = pkPickWnd;
			
			SelListItem("GuiEd_WidgetList", (char*)m_pkFocusWnd->GetName());

			if(GetWndType(m_pkFocusWnd) == Combobox)
			{
				ZGuiCombobox* pkCombobox = (ZGuiCombobox*) m_pkFocusWnd;
				pkCombobox->Notify(m_pkFocusWnd, 
					pkCombobox->GetListbox()->IsVisible() ? NCODE_RELEASE : NCODE_CLICK_DOWN );
			}

			Rect rc = pkPickWnd->GetScreenRect();
			int diff_right = abs(rc.Right-x);
			int diff_bottom = abs(rc.Bottom-y);

			if(m_bResize)
			{
				if(diff_right < diff_bottom)
				{
					m_eResizeDir = Left;

#ifdef WIN32
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
#endif					
				}
				else
				{
					m_eResizeDir = Up;
#ifdef WIN32
			SetCursor(LoadCursor(NULL, IDC_SIZENS));
#endif					
				}
			}
			else
			{
				
#ifdef WIN32
			SetCursor(LoadCursor(NULL, IDC_ARROW));
#endif
			}
		}
		else
		{
			m_pkFocusWnd = NULL;	
		}

		UpdateInfo();
		SelNewSkin(0);
	}
}

void ZGuiResEd::MouseMove(bool bLeft, int x, int y)
{
	if(IsButtonChecked("GuiEd_DisableMove") && m_bResize == false)
		return;

	if(m_pkFocusWnd)
	{		
		Rect rc = m_pkFocusWnd->GetScreenRect();

		bool bParentIsTabControl = m_pkFocusWnd->GetParent() && GetWndType(m_pkFocusWnd->GetParent());

		if(m_bResize == true)
		{	
			if(bParentIsTabControl)	
				m_pkFocusWnd = m_pkFocusWnd->GetParent();		

			if(m_eResizeDir == Left)
			{				   
				int w = x - rc.Left;
				m_pkFocusWnd->Resize(w > 0 ? w : 0, -1); 
			}

			if(m_eResizeDir == Up)
			{
				int h = y - rc.Top;
				m_pkFocusWnd->Resize(-1, h > 0 ? h : 0); 
			}
	
			CheckMovement();
		}
		else
		{		
			x -= m_kCursorRangeDiff.x;
			y -= m_kCursorRangeDiff.y;

			if(m_pkInputHandle->Pressed(KEY_Y))
				x = rc.Left;

			if(m_pkInputHandle->Pressed(KEY_X))
				y = rc.Top;

			if(!bParentIsTabControl)
			{
				map<string,SPECIAL_WND_INFO>::iterator itWndInfo = 
					m_kSpecialWndInfo.find(m_pkFocusWnd->GetName());
				if(itWndInfo != m_kSpecialWndInfo.end())
				{
					if(itWndInfo->second.bFreemovement)
						m_pkFocusWnd->SetPos(x, y, true, true);
					else
						m_pkFocusWnd->SetPos(x > 0 ? x : 0, y > 0 ? y : 0, true, true);
				}
				else
					m_pkFocusWnd->SetPos(x > 0 ? x : 0, y > 0 ? y : 0, true, true);
			}

			CheckMovement();
		}
	}

	if(m_pkFocusWnd)
	{
		Rect rc;

		if(m_pkFocusWnd->GetParent())
			rc = m_pkFocusWnd->GetWndRect();
		else
			rc = m_pkFocusWnd->GetScreenRect();

		SetTextInt("GuiEd_WndPosX", rc.Left);
		SetTextInt("GuiEd_WndPosY", rc.Top);
		SetTextInt("GuiEd_WndWidth", rc.Width());
		SetTextInt("GuiEd_WndHeight", rc.Height());

	}
}

void ZGuiResEd::OnCommand(string strCtrlID, int iCmdExtra)
{	
	if(m_bEditorMode == false)
		return;

	// Just skip all commands if in test mode
	if(m_bTestGUI)
		return;

	string strText;

	ZGuiSkin** ppkSkin;

	if(strCtrlID == "DefMsgBoxYesBn")
	{
		OnConfirmMsgBox(true);
		return;
	}

	if(strCtrlID == "DefMsgBoxNoBn")
	{
		OnConfirmMsgBox(false);
		return;
	}

	if(strCtrlID == "GuiEd_HiddenFromStart")
	{
		if(m_pkFocusWnd)
		{
			bool bHidden = IsButtonChecked("GuiEd_HiddenFromStart");

			map<string,SPECIAL_WND_INFO>::iterator itWndInfo;
			itWndInfo = m_kSpecialWndInfo.find(m_pkFocusWnd->GetName());
			if(itWndInfo != m_kSpecialWndInfo.end())
			{
				itWndInfo->second.bHiddenFromStart = bHidden;
			}
			else
			{
				SPECIAL_WND_INFO info;
				info.bHiddenFromStart = bHidden;
				m_kSpecialWndInfo.insert(
					map<string,SPECIAL_WND_INFO>::value_type(
						m_pkFocusWnd->GetName(), info)); 
			}
		}
	}

	//case IDC_MULTILINE_CB:
	//	if(m_pkFocusWnd)
	//	{
	//		bool bMultiLine = IsDlgButtonChecked(g_kDlgBoxBottom, IDC_MULTILINE_CB);
	//		((ZGuiTextbox*)m_pkFocusWnd)->ToggleMultiLine(bMultiLine);

	//		if(bMultiLine)
	//		{
	//			((ZGuiTextbox*)m_pkFocusWnd)->SetScrollbarSkin(GetSkin("DefSBrBkSkin"),
	//				GetSkin("DefSBrNSkin"), GetSkin("DefSBrFSkin"),
	//				GetSkin("DefSBrScrollUpSkin_u"), GetSkin("DefSBrScrollUpSkin_d"),
	//				GetSkin("DefSBrScrollDownSkin_u"), GetSkin("DefSBrScrollDownSkin_d"));
	//				}
	//		}
	//	return;

	/*case IDC_HORIZONTAL_CB:
		if(m_pkFocusWnd)
		{
			bool bHorizontal = IsDlgButtonChecked(g_kDlgBoxBottom, IDC_HORIZONTAL_CB);

			Rect rc = m_pkFocusWnd->GetScreenRect();
		
			if(bHorizontal)
			{
				if(GetWndType(m_pkFocusWnd) == Slider)
					((ZGuiSlider*)m_pkFocusWnd)->SetCtrlStyle(SCF_HORZ);										
			}
			else
			{
				if(GetWndType(m_pkFocusWnd) == Slider)
					((ZGuiSlider*)m_pkFocusWnd)->SetCtrlStyle(SCF_VERT);				
			}

			if(GetWndType(m_pkFocusWnd) == Scrollbar)
				((ZGuiScrollbar*)m_pkFocusWnd)->ToogleHorizontal();

			CheckMovement();
		}
		return;*/

	/*case IDC_PROGRESSBAR_DIR1_RB:
		((ZGuiProgressbar*)m_pkFocusWnd)->SetDir(PBDIR_LEFT_TO_RIGHT);
		return;

	case IDC_PROGRESSBAR_DIR2_RB:
		((ZGuiProgressbar*)m_pkFocusWnd)->SetDir(PBDIR_RIGHT_TO_LEFT);
		return;

	case IDC_PROGRESSBAR_DIR3_RB:
		((ZGuiProgressbar*)m_pkFocusWnd)->SetDir(PBDIR_TOP_TO_BOTTOM);
		return;

	case IDC_PROGRESSBAR_DIR4_RB:
		((ZGuiProgressbar*)m_pkFocusWnd)->SetDir(PBDIR_BOTTOM_TO_TOP);
		return;

	case IDC_PBTEXTORIENT_RB1:
		((ZGuiProgressbar*)m_pkFocusWnd)->m_bShowText = true;
		((ZGuiProgressbar*)m_pkFocusWnd)->SetTextOrientation(PBTEXTORIENT_CENTER);
		return;

	case IDC_PBTEXTORIENT_RB2:
		((ZGuiProgressbar*)m_pkFocusWnd)->m_bShowText = true;
		((ZGuiProgressbar*)m_pkFocusWnd)->SetTextOrientation(PBTEXTORIENT_LEFT);
		return;

	case IDC_PBTEXTORIENT_RB3:
		((ZGuiProgressbar*)m_pkFocusWnd)->m_bShowText = true;
		((ZGuiProgressbar*)m_pkFocusWnd)->SetTextOrientation(PBTEXTORIENT_OVER);
		return;

	case IDC_PBTEXTORIENT_RB4:
		((ZGuiProgressbar*)m_pkFocusWnd)->m_bShowText = true;
		((ZGuiProgressbar*)m_pkFocusWnd)->SetTextOrientation(PBTEXTORIENT_RIGHT);
		return;

	case IDC_PBTEXTORIENT_RB5:
		((ZGuiProgressbar*)m_pkFocusWnd)->m_bShowText = true;
		((ZGuiProgressbar*)m_pkFocusWnd)->SetTextOrientation(PBTEXTORIENT_UNDER);
		return;

	case IDC_PBTEXTORIENT_RB6:
		((ZGuiProgressbar*)m_pkFocusWnd)->m_bShowText = false;
		return;*/

	if(strCtrlID == "GuiEd_Close")
	{
		m_pkZeroFps->QuitEngine();
		return;
	}

	if(strCtrlID == "GuiEd_WidgetTypeList" || strCtrlID == "GuiEd_FilterToType")
	{
		FilterWnd();
		return; 
	}

	if(strCtrlID == "GuiEd_CopySelWidget")
	{
		m_iTask = TASK_COPY_WND;
		return; 
	}

	if(strCtrlID == "GuiEd_PasteSelWidget")
	{
		m_iTask = TASK_PASTE_WND;
		return; 
	}

	if(strCtrlID == "GuiEd_CopySelSkin")
	{
		m_iTask = TASK_COPY_SKIN;
		return; 
	}

	if(strCtrlID == "GuiEd_CopyAllSkins")
	{
		m_iTask = TASK_COPY_ALL_SKINS;
		return; 
	}
		
	if(strCtrlID == "GuiEd_PasteSkin")
	{
		m_iTask = TASK_PASTE_SKIN;
		return; 
	}

	/*if(strCtrlID == "_ALPHA_TEXTURE_CB {
		m_bAlphaTextureMode = IsDlgButtonChecked(g_kDlgBoxRight, iCtrlID);
		SelNewSkin();
		return; }*/

	// Select window from list.
	if(strCtrlID == "GuiEd_WidgetList") 
	{
		if(char* szItem = GetSelItem("GuiEd_WidgetList"))
		{
			ZGuiWnd* pkWnd = GetWnd(szItem);
			if(pkWnd)
			{
				m_pkFocusWnd = pkWnd;						
				UpdateInfo();
				SelNewSkin(0);
			}				
		}

		m_pkGui->SetFocus(GetWnd("GuiEd_WidgetList"), false); // set focus on listbox again		
		return; 
	}

	if(strCtrlID == "GuiEd_SelectedDesignMode")
	{
		MsgBox("Translate and scale windows to get same propositions in the new resolution?", 
			"Switch Design Resolution", MSGBOX_YESNO);
	}

	if(strCtrlID == "GuiEd_OpenTexture")
	{
		if(char* szTexName = GetSelItem("GuiEd_TextureList"))
		{
			string strName = m_strCurrTexDir + string("/") + string(szTexName);

			ZSSVFileSystem* m_pkFileSys = reinterpret_cast<ZSSVFileSystem*>(
					g_ZFObjSys.GetObjectPtr("ZSSVFileSystem"));	

			string strRealName = m_pkFileSys->GetFullPath(strName.c_str());

			#ifdef WIN32

				ShellExecute(GetFocus(), "open", 
					strRealName.c_str(), NULL, NULL, SW_SHOWDEFAULT);

			#endif	
		}
		return; 
	}

	if(strCtrlID == "GuiEd_TextureList")
	{
		if(char* szTexName = GetSelItem("GuiEd_TextureList"))
		{
			string strName = m_strCurrTexDir + string("/") + string(szTexName);

			ZSSVFileSystem* m_pkFileSys = reinterpret_cast<ZSSVFileSystem*>(
					g_ZFObjSys.GetObjectPtr("ZSSVFileSystem"));	

			string strRealName = m_pkFileSys->GetFullPath(strName.c_str());

			UpdatePreviewImage(strRealName.c_str());

			if(iCmdExtra == 1) // double click
			{
				if(strrchr(szTexName, '.') == NULL) // no point found in textitem, must be a folder...
				{
					m_strCurrTexDir += string("/") + string(szTexName); // change root path for images
					
					string strInfo = string( "Textures (root=)" ) + m_strCurrTexDir;
					SetText("GuiEd_TextureListLabel", (char*)strInfo.c_str());				
				}
				else
				if(strcmp(szTexName, "..") == 0) // go back
				{							
					int slash = m_strCurrTexDir.find_last_of('/'); 
					if(slash != string::npos)
					{								
						if( m_strCurrTexDir != "data/textures/gui" ) // dont allow browsing lower then gui texture folder
						{
							m_strCurrTexDir.erase(slash, m_strCurrTexDir.size()-slash);									
						
							string strInfo = string( "Textures (root=)" ) + m_strCurrTexDir;
							SetText("GuiEd_TextureListLabel", (char*)strInfo.c_str());							
						}
					}
				}
				else
				{
					m_iTask = TASK_SET_TEXTURE;
					return; // double clicked on a oridnary file
				}

				ClearListbox("GuiEd_TextureList");		
				AddFilesInFolderToListbox(m_strCurrTexDir.c_str());
			}
		}
		return;
	}


	if(strCtrlID == "GuiEd_SkinElementsList") 
	{
		SelNewSkin();
		return;
	}

	if(strCtrlID == "GuiEd_SelBackgroundTexture" ||
		strCtrlID == "GuiEd_SelBorderCorner" ||
		strCtrlID == "GuiEd_SelBorderHorz" ||
		strCtrlID == "GuiEd_SelBorderVert")
	{
		SelNewSkin();
		return; 
	}

	if(strCtrlID == "GuiEd_CreateNewWidget")
	{
		m_iTask = TASK_CREATE_NEW_WINDOW;
		return; 
	}

	if(strCtrlID == "GuiEd_DeleteSelWidget")
	{
		m_iTask = TASK_DELETE_SEL_WINDOW;
		return; 
	}

	if(strCtrlID == "GuiEd_NewScript")
	{
		MsgBox("Are you sure? Everything will be lost.", "Create New GUI", MSGBOX_YESNO);
		return; 
	}

	if(strCtrlID == "GuiEd_BringToFront")
	{
		m_iTask = TASK_BRING_WND_TO_FRONT;
		return; 
	}

	if(strCtrlID == "GuiEd_SetTexture")
	{
		m_iTask = TASK_SET_TEXTURE;
		return; 
	}

	if(strCtrlID == "GuiEd_RemoveTexture")
	{
		m_iTask = TASK_REMOVE_TEXTURE;
		return; 
	}

	if(strCtrlID == "GuiEd_Tile")
	{
		if(GetSelSkin(ppkSkin))
			(*ppkSkin)->m_bTileBkSkin = IsButtonChecked("GuiEd_Tile");
		return; 
	}

	if(strCtrlID == "GuiEd_Trans") 
	{
		if(GetSelSkin(ppkSkin))
			(*ppkSkin)->m_bTransparent = IsButtonChecked("GuiEd_Trans");
		return; 
	}

	if(strCtrlID == "GuiEd_ToggleShow")
	{
		if(m_pkFocusWnd)
		{
			bool bVisible = IsButtonChecked("GuiEd_ToggleShow");
			if(bVisible)
				m_pkFocusWnd->Show();
			else
				m_pkFocusWnd->Hide();

			m_pkFocusWnd = NULL;
			UpdateInfo();
			UpdatePreviewImage(NULL);
		}
		return; 
	}

	if(strCtrlID == "GuiEd_OpenScript" || strCtrlID == "GuiEd_SaveScript")
	{
		m_bSelectFont = false;

		ShowWnd("GuiEd_SelectFileWnd", true, true, true);

		static bool s_bFirstTime = true;
		if(s_bFirstTime)
		{
			s_bFirstTime = false;
			ZGuiWnd* pkScriptDlg = GetWnd("GuiEd_SelectFileWnd");
			Rect rc = pkScriptDlg->GetScreenRect();
			Rect rcCreateDlg = GetWnd("GuiEd_NewWnd")->GetScreenRect();

			int x = rcCreateDlg.Left-rc.Width();
			int y = rcCreateDlg.Top;

			if(x < 0) x = 0; else if(x > GetWidth()-rc.Width()) x = GetWidth()-rc.Width();
			if(y < 0) y = 0; else if(y > GetHeight()-rc.Height()) y = GetHeight()-rc.Height();

			GetWnd("GuiEd_SelectFileWnd")->SetPos(x,y,true,true); 
		}

		ZSSVFileSystem* m_pkFileSys = reinterpret_cast<ZSSVFileSystem*>(
				g_ZFObjSys.GetObjectPtr("ZSSVFileSystem"));	
		
		vector<string> kFiles;
		vector<string> ext;
		ext.push_back(".lua"); 

		bool bSave = (strCtrlID == "GuiEd_SaveScript");

		m_pkFileSys->ListDirRecursive(kFiles, string("../datafiles"), ext, bSave);

		//if(SearchFiles(kFiles, "..\\datafiles", ".lua", (strCtrlID == "GuiEd_SaveScript")))
		if(1)
		{
			printf("Num files found = %i\n", kFiles.size());

			ClearListbox("GuiEd_ScriptList");

			for(int i=0; i<kFiles.size(); i++) 
			{		
				bool bAdd = false;

				if(strCtrlID == "GuiEd_SaveScript")
				{
					if(kFiles[i].find("/script/gui") != string::npos && 
						kFiles[i].find("/CVS") == string::npos)
					bAdd = true;

					if(bAdd && kFiles[i].find(".") != string::npos)
						bAdd = false;
				}
				else
				{
					if(kFiles[i].find("/script/gui") != string::npos)
						bAdd = true;
				}

				if(bAdd)
					AddListItem("GuiEd_ScriptList", (char*) kFiles[i].c_str());
			}

			if(strCtrlID == "GuiEd_SaveScript")
			{
				int p = m_strOpenFile.find_last_of("\\");
				if(p == string::npos)
					p = m_strOpenFile.find_last_of("/");

				string strCurrentFile = m_strOpenFile;
				strCurrentFile.erase(0,p+1);

				SetText("GuiEd_ScriptFileName", (char*)strCurrentFile.c_str());
			}
		}

		if(strCtrlID == "GuiEd_SaveScript")
		{
			m_bSelectScriptSave = true;
			GetWnd("GuiEd_ScriptFileName")->Show();
			SetText("SelectScriptInfoLabel", "Please select a folder and a file name.");
			
		}
		else
		{
			m_bSelectScriptSave = false;			
			GetWnd("GuiEd_ScriptFileName")->Hide();
			SetText("SelectScriptInfoLabel", "Please select the file you want to load.");
		}

		return; 
	}

		
	if(strCtrlID == "GuiEd_SelectScriptCancel")
	{
		ShowWnd("GuiEd_SelectFileWnd", false);
		return; 
	}

	if(strCtrlID == "GuiEd_SelectScriptOK")
	{
		if(m_bSelectFont == true)
		{
			m_iTask = TASK_SET_FONT;
			return;
		}

		if(char* szSelItem = GetSelItem("GuiEd_ScriptList"))
		{
			char szText[200];

			strcpy(szText, szSelItem);

			for(int i=0; i<strlen(szText); i++)
				if(szText[i] == '\\')
					szText[i] = '/';

			string strFile = szText;
			strFile.erase(0,strFile.find("/script/"));
			strFile = string("data") + strFile;
			
			if(m_bSelectScriptSave)
			{
				string strFullName;
				char* szFileName;
				if((szFileName=GetText("GuiEd_ScriptFileName")))
				{
					strFullName = string("../datafiles/") + string(szText) + string("/") + string(szFileName);

					char *ext = strrchr( strFullName.c_str(), '.');
					if(strcmp(ext,".lua") != 0) 
					{
						strFullName += ".lua";
					}								

					m_strNewFileToLoad = strFullName;
					m_iTask = TASK_SAVE_SCRIPT;
					m_bConfirmSaveScript = true;
				}
			}
			else
			{
				m_iTask = TASK_LOAD_SCRIPT;
				m_strNewFileToLoad = strFile;
			}

			ShowWnd("GuiEd_SelectFileWnd", false);
		}

		return; 
	}

	if(strCtrlID == "GuiEd_ResizeMode")
	{
		m_bResize = !m_bResize;
		return; 
	}

	if(strCtrlID == "GuiEd_TestGui") 
	{			
		//m_bTestGUI = !m_bTestGUI;
		//m_pkGui->Activate(m_bTestGUI); 
		m_iTask = TASK_TEST_GUI;		

		//if(m_bTestGUI) // change clear color to notify we are in test mode.
		//{
		//	m_pkCamera->SetClearColor(Vector4(0.44f,0.14f,0.14f,0));
		//	GetWnd("GuiEd_EditWnd")->Hide();
		//	GetWnd("GuiEd_NewWnd")->Hide();
		//	GetWnd("GuiEd_SelectFileWnd")->Hide();
		//	m_bEditorMode
		//}
		//else
		//{
		//	m_pkCamera->SetClearColor(Vector4(0.44f,0.44f,0.44f,0));
		//	GetWnd("GuiEd_EditWnd")->Show();
		//	GetWnd("GuiEd_NewWnd")->Show();
		//	GetWnd("GuiEd_SelectFileWnd")->Show();
		//}

		return; 
	}

	if(strCtrlID == "GuiEd_Freemovement")
	{
		if(m_pkFocusWnd) 
		{
			bool bFreeMovement = IsButtonChecked("GuiEd_Freemovement");

			if(GetWndType(m_pkFocusWnd) == Wnd)
			{					
				if(bFreeMovement)
					m_pkFocusWnd->SetMoveArea(Rect(0,0,800,600), true);
				else
				{
					Rect rc = m_pkFocusWnd->GetScreenRect();
					m_pkFocusWnd->SetMoveArea(rc, true);					
				}
			}

			//string id = m_pkFocusWnd->GetName();
			//set<string> { {iterator it = m_kFreemovementWnds.find(id);

			//if(bFreeMovement)
			//{
			//	if(it == m_kFreemovementWnds.end())				
			//		m_kFreemovementWnds.insert(id);		
			//}
			//else
			//{				
			//	if(it != m_kFreemovementWnds.end())
			//		m_kFreemovementWnds.erase(it);
			//}

			map<string,SPECIAL_WND_INFO>::iterator itWndInfo;
			itWndInfo = m_kSpecialWndInfo.find(m_pkFocusWnd->GetName());
			if(itWndInfo != m_kSpecialWndInfo.end())
			{
				itWndInfo->second.bFreemovement = bFreeMovement;
			}
			else
			{
				SPECIAL_WND_INFO info;
				info.bFreemovement = bFreeMovement;
				m_kSpecialWndInfo.insert(
						map<string,SPECIAL_WND_INFO>::value_type(
							m_pkFocusWnd->GetName(), info)); 
			}

		}
		return; 
	}

	if(strCtrlID == "GuiEd_ForceCaptureToSel")
	{
		m_bForceCaptureToSel = !m_bForceCaptureToSel;
		return; 
	}
	
	if(strCtrlID == "GuiEd_ResizeToImg")
	{
		m_iTask = TASK_RESIZE_WND_TO_IMAGE;
		return; 
	}

	if(strCtrlID == "GuiEd_SelectFont")
	{
		m_bPickedColor = false;
		OpenSelectFontDlg();
		return; 
	}

	if(strCtrlID == "GuiEd_Alignment1") {
		if(m_pkFocusWnd) m_pkFocusWnd->m_iWndAlignment = TopLeft;
		return; }
	if(strCtrlID == "GuiEd_Alignment2") {
		if(m_pkFocusWnd) m_pkFocusWnd->m_iWndAlignment = TopRight;
		return; }
	if(strCtrlID == "GuiEd_Alignment3") {
		if(m_pkFocusWnd) m_pkFocusWnd->m_iWndAlignment = BottomRight;
		return; }
	if(strCtrlID == "GuiEd_Alignment4") {
		if(m_pkFocusWnd) m_pkFocusWnd->m_iWndAlignment = BottomLeft;
		return; }
	if(strCtrlID == "GuiEd_Alignment5") {
		if(m_pkFocusWnd) m_pkFocusWnd->m_iWndAlignment = CenterHorz;
		return; }
	if(strCtrlID == "GuiEd_Alignment6") {
		if(m_pkFocusWnd) m_pkFocusWnd->m_iWndAlignment = CenterVert;
		return; }
	if(strCtrlID == "GuiEd_Alignment7") {
		if(m_pkFocusWnd) m_pkFocusWnd->m_iWndAlignment = Center;
		return; }
	


	//if(strCtrlID == "_RESIZETYPE_DONT {
	//	if(m_pkFocusWnd) m_pkFocusWnd->m_iResizeType = Center;
	//	return; }
	//if(strCtrlID == "_RESIZETYPE_WIDTH {
	//	if(m_pkFocusWnd) m_pkFocusWnd->m_iResizeType = ResizeWidth;
	//	return; }
	//if(strCtrlID == "_RESIZETYPE_HEIGHT {
	//	if(m_pkFocusWnd) m_pkFocusWnd->m_iResizeType = ResizeHeight;
	//	return; }
	//if(strCtrlID == "_RESIZETYPE_BOTH {
	//	if(m_pkFocusWnd) m_pkFocusWnd->m_iResizeType = Resize;
	//	return; }
}

void ZGuiResEd::OnTextboxInput(string strFocusWnd, int iKey)
{
	ZGuiWnd* pkWnd = GetWnd(strFocusWnd);
	if(pkWnd == NULL)
		return;

	char* text = pkWnd->GetText(); 

	switch(iKey)
	{
	case KEY_RETURN:
		{
			if(strFocusWnd == "GuiEd_WndCaption")
				m_pkFocusWnd->SetText(text);
			else
			if(strFocusWnd == "GuiEd_WndIDName")
				RenameSelWnd(text);
			else
			if(strFocusWnd == "GuiEd_WndPosX")
			{
				bool bChild = m_pkFocusWnd->GetParent() != NULL;

				Rect rc;
				
				if(bChild)
					rc = m_pkFocusWnd->GetWndRect();
				else
					rc = m_pkFocusWnd->GetScreenRect();

				rc.Left = atoi(text);

				m_pkFocusWnd->SetPos(rc.Left > 0 ? rc.Left : 0, rc.Top > 0 ? rc.Top : 0, !bChild, true);
				CheckMovement();
			}
			else
			if(strFocusWnd == "GuiEd_WndPosY")
			{
				bool bChild = m_pkFocusWnd->GetParent() != NULL;

				Rect rc;
				
				if(bChild)
					rc = m_pkFocusWnd->GetWndRect();
				else
					rc = m_pkFocusWnd->GetScreenRect();

				rc.Top = atoi(text);

				m_pkFocusWnd->SetPos(rc.Left > 0 ? rc.Left : 0, rc.Top > 0 ? rc.Top : 0, !bChild, true);
				CheckMovement();
			}
			else
			if(strFocusWnd == "GuiEd_WndWidth")
			{
				m_pkFocusWnd->Resize(atoi(text),-1);
				CheckMovement();
			}
			else
			if(strFocusWnd == "GuiEd_WndHeight")
			{
				m_pkFocusWnd->Resize(-1, atoi(text));
				CheckMovement();
			}
			else
			if(strFocusWnd == "GuiEd_ColorR" ||
				strFocusWnd == "GuiEd_ColorG" ||
				strFocusWnd == "GuiEd_ColorB" )
			{
				OnChangeSkinColor();
			}
			else
			if(strFocusWnd == "GuiEd_BorderSize")
			{
				ZGuiSkin** ppkSkin;
				if(GetSelSkin(ppkSkin))
				{
					(*ppkSkin)->m_unBorderSize = atoi(text);
				}
			}
		}
		break;
	}
}

ZGuiWnd* ZGuiResEd::GetWndFromPoint(int x, int y)
{
	pair<ZGuiWnd*, int> kFind(NULL,0);
	vector<pair<ZGuiWnd*, float> > vkCandidates;
	map<string, ZGuiWnd*> kWindows;

	GuiType eFilterType = FormatWndType(GetSelItem("GuiEd_WidgetTypeList"));
	bool bFilter = IsButtonChecked("GuiEd_FilterToType");

	ZGuiWnd* pkMainWnd = GetWnd("GuiMainWnd");

	m_pkGuiMan->GetWindows(kWindows);
	for( map<string, ZGuiWnd*>::iterator it = kWindows.begin(); it != kWindows.end(); it++)
	{
		ZGuiWnd* pkWnd = it->second;

		if(pkWnd->GetScreenRect().Inside(x,y) && pkWnd->IsVisible() && IsEditorWnd(pkWnd) == false)
		{
			GuiType eType = GetWndType(pkWnd);
			if(bFilter == false || eType == eFilterType)
			{
				if(!pkWnd->GetParent() || pkWnd->GetParent()->IsVisible())
				{
					int value = pkWnd->m_iZValue;

					Vector2 dist = Vector2(x,y) - Vector2(pkWnd->GetScreenRect().Left, pkWnd->GetScreenRect().Top);					
					float length = dist.Length();

					if(pkWnd->GetParent())
					{
						Vector2 dist_to_parent = Vector2(x,y) - Vector2(
										pkWnd->GetParent()->GetScreenRect().Left, 
										pkWnd->GetParent()->GetScreenRect().Top);					

						if(dist_to_parent.Length() < length)
							length = dist_to_parent.Length();
					}
					else
					{
						length = 9999;
					}

					value -= int(length);

					if(pkWnd == pkMainWnd)  
						value -= 9999;

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

	if(kFind.first)
		printf("kFind.first = %s\n", kFind.first->GetName());

	return kFind.first;
}

void ZGuiResEd::SwitchDesignRes(int w, int h, bool rescale)
{
	int iBorderSize = (GetWidth()-w) > (GetHeight()-h) ? (GetWidth()-w) : (GetHeight()-h);

	if(iBorderSize < 0)
		iBorderSize = 0;

	GetWnd("GuiEd_DesignBorder")->GetSkin()->m_unBorderSize = iBorderSize;
	GetWnd("GuiEd_DesignBorder")->Resize(w, h); 

	if(rescale)
	{
		float fResModX =  (float )w  / (float) m_iDesignResX;
		float fResModY =  (float) h  / (float) m_iDesignResY;

		int iAntal = 0;

		map<string, ZGuiWnd*> kWindows;
		
		m_pkGuiMan->GetWindows(kWindows);
		for( map<string, ZGuiWnd*>::iterator it = kWindows.begin(); it != kWindows.end(); it++)
		{
			if(IsEditorWnd((*it).second) == false)
			{
				iAntal++;
			}
		}	

		kWindows.clear();
		m_pkGuiMan->GetWindows(kWindows);
		
		vector<ZGuiWnd*> kAddList;

		printf("----------------------------------------\n");

		for( map<string, ZGuiWnd*>::iterator it = kWindows.begin(); it != kWindows.end(); it++)
		{
			ZGuiWnd* pkWnd = (*it).second;
			
			if(IsEditorWnd(pkWnd) == false /*|| pkWnd == GetWnd("Wnd1234")*/)
			{
				GuiType eType = GetWndType(pkWnd);

				Rect rc = eType == Wnd ? pkWnd->GetScreenRect() : pkWnd->GetWndRect();

				float dx = rc.Left, dy = rc.Top, dw = rc.Width(), dh = rc.Height();

				dx = dx*fResModX;
				dy = dy*fResModY;

				dw = dw*fResModX;
				dh = dh*fResModY;

				if( GetWndType(pkWnd->GetParent()) != TabControl ) // en sida i en tabcontroll
				{
					pkWnd->SetPos((int)dx,(int)dy,(eType == Wnd),true);
					pkWnd->Resize((int)dw,(int)dh,true);
					printf("name = %s\n", pkWnd->GetName());
				}
			}
		}
	}

	m_iDesignResX = w;
	m_iDesignResY = h;

	if(m_iDesignResX == 800 && m_iDesignResY == 600)
		SelListItem("GuiEd_SelectedDesignMode", "800x600");
	else
	if(m_iDesignResX == 1024 && m_iDesignResY == 768)
		SelListItem("GuiEd_SelectedDesignMode", "1024x768");
	else
	if(m_iDesignResX == 1280 && m_iDesignResY == 1024)
		SelListItem("GuiEd_SelectedDesignMode", "1200x1024");
	else
	if(m_iDesignResX == 1600 && m_iDesignResY == 1200)
		SelListItem("GuiEd_SelectedDesignMode", "1600x1200");

}

void ZGuiResEd::OnConfirmMsgBox(bool bAnswer)
{
	string strMsgBoxCaption = GetText("DefMsgBoxCaption");

	if(strMsgBoxCaption == "Switch Design Resolution")
	{
		if(char* item = GetSelItem("GuiEd_SelectedDesignMode"))
		{
			string t = item;
			int p = t.find_first_of("x");

			if(p != string::npos)
			{
				string h = t; h.erase(0,p+1); printf("%s\n", h.c_str());
				string w = t; w.erase(p,99); printf("%s\n", w.c_str());
				SwitchDesignRes(atoi(w.c_str()),atoi(h.c_str()), bAnswer);
			}
		}
	}
	else
	if(strMsgBoxCaption == "Create New GUI")
	{
		if(bAnswer == true)
			m_iTask = TASK_NEW_GUI;
	}
	else
	if(strMsgBoxCaption == "Confirm Save Script")
	{
		if(bAnswer == true)
		{
			m_iTask = TASK_SAVE_SCRIPT;
			m_bConfirmSaveScript = false;
		}
	}
	else
	if(strMsgBoxCaption == "Delete Wnd")
	{
		if(bAnswer == true)
			DeleteSelWindow(false);
	}

	ShowWnd("GuiDefMsgBox", false);
}





