#include "zguied.h"
#include "resource.h"
#include "shellapi.h"

void ZGuiEd::HandleInput()
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
	
	if(m_pkFocusWnd && !TextboxFocus())
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

	if(!m_pkInputHandle->Pressed(KEY_ESCAPE))
		s_bEscPressed = false;

	if(m_pkInputHandle->Pressed(KEY_DELETE) && !DelayCommand() ) {
		if(m_pkFocusWnd && !TextboxFocus())
		{
			DeleteSelWindow(GetWndType(m_pkFocusWnd) == Wnd && m_pkFocusWnd->GetNumChildrens() != 0);
		}
	}

	if(m_pkInputHandle->Pressed(MOUSELEFT) && s_bLeftMouseButtonPressed == false)
	{
		int x,y;
		m_pkInputHandle->SDLMouseXY(x,y);
		MouseClick(true, x,y);
		
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

	if(m_pkInputHandle->Pressed(KEY_LSHIFT) && s_bLShiftPressed == false)
	{
		s_bLShiftPressed = true;
		if(m_bResize == false)
		{
			SendMessage(GetCtrl(IDC_RESIZE_WND, 1), BM_SETCHECK, BST_CHECKED, 0);
			m_bResize = true;
		}
	}
	else if(!m_pkInputHandle->Pressed(KEY_LSHIFT))
	{
		if(m_bResize == true && s_bLShiftPressed)
		{
			s_bLShiftPressed = false;
			SendMessage(GetCtrl(IDC_RESIZE_WND, 1), BM_SETCHECK, BST_UNCHECKED, 0);
			m_bResize = false;
			SetCursor(LoadCursor(NULL, IDC_ARROW));
		}
	}

	if(!TextboxFocus())
	{
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

	if(m_pkInputHandle->Pressed(KEY_ESCAPE) && !s_bEscPressed)
	{
		s_bEscPressed = true;
		m_pkZeroFps->QuitEngine();
	}
	else
	if(m_pkInputHandle->Pressed(KEY_RETURN) && s_bLShiftPressed == false)
	{
		s_bReturnPressed = true;

		if(m_pkFocusWnd)
		{
			HWND kFocusWnd = GetFocus();
			if(kFocusWnd)
			{				
				char text[512];
				GetWindowText(kFocusWnd, text, 512);				

				if(kFocusWnd == GetCtrl(IDC_TEXTLABEL_EB, 0))
					m_pkFocusWnd->SetText(text);
				else
				if(kFocusWnd == GetCtrl(IDC_WINDOW_NAMEID_EB, 0))
				{
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
				}
				else
				if(kFocusWnd == GetCtrl(IDC_POSX_EB, 0))
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
				if(kFocusWnd == GetCtrl(IDC_POSY_EB, 0))
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
				if(kFocusWnd == GetCtrl(IDC_WIDTH_EB, 0))
				{
					m_pkFocusWnd->Resize(atoi(text),-1);
					CheckMovement();
				}
				else
				if(kFocusWnd == GetCtrl(IDC_HEIGHT_EB, 0))
				{
					m_pkFocusWnd->Resize(-1, atoi(text));
					CheckMovement();
				}
				else
				if(kFocusWnd == GetCtrl(IDC_RGB_COLOR_R_EB, 0))
				{
					ZGuiSkin** ppkSkin;
					if(GetSelSkin(ppkSkin))
					{
						float fColor = (float) atoi(text) / 255.0f;
						if(IsDlgButtonChecked(g_kDlgBoxRight, IDC_SKINTYPE_BACKGROUND_RB))
							(*ppkSkin)->m_afBkColor[0] = fColor;
						else
							(*ppkSkin)->m_afBorderColor[0] = fColor;
					}
				}
				else
				if(kFocusWnd == GetCtrl(IDC_RGB_COLOR_G_EB, 0))
				{
					ZGuiSkin** ppkSkin;
					if(GetSelSkin(ppkSkin))
					{
						float fColor = (float) atoi(text) / 255.0f;
						if(IsDlgButtonChecked(g_kDlgBoxRight, IDC_SKINTYPE_BACKGROUND_RB))
							(*ppkSkin)->m_afBkColor[1] = fColor;
						else
							(*ppkSkin)->m_afBorderColor[1] = fColor;
					}
				}
				else
				if(kFocusWnd == GetCtrl(IDC_RGB_COLOR_B_EB, 0))
				{
					ZGuiSkin** ppkSkin;
					if(GetSelSkin(ppkSkin))
					{
						float fColor = (float) atoi(text) / 255.0f;
						if(IsDlgButtonChecked(g_kDlgBoxRight, IDC_SKINTYPE_BACKGROUND_RB))
							(*ppkSkin)->m_afBkColor[2] = fColor;
						else
							(*ppkSkin)->m_afBorderColor[2] = fColor;
					}
				}
				else
				if(kFocusWnd == GetCtrl(IDC_BORDER_EB, 0))
				{
					ZGuiSkin** ppkSkin;
					if(GetSelSkin(ppkSkin))
					{
						(*ppkSkin)->m_unBorderSize = atoi(text);
					}
				}

				SetFocus(GetParent(g_kDlgBoxRight));
			}
		}
	}
	else if(!m_pkInputHandle->Pressed(KEY_RETURN))
	{
		s_bReturnPressed = false;
	}	
}

bool ZGuiEd::DelayCommand(float delay)
{
	if(m_pkZeroFps->GetEngineTime() < m_fDelayTime)
		return true;
	m_fDelayTime = m_pkZeroFps->GetEngineTime() + delay;

	return false;
}

void ZGuiEd::MouseClick(bool bLeft, int x, int y)
{
	if(x > 800 || y > 600)
		return;

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
			
			SendDlgItemMessage(g_kDlgBoxRight, IDC_WINDOW_LIST, LB_SELECTSTRING, -1, 
				(LPARAM) (LPCSTR) m_pkFocusWnd->GetName() );

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
					SetCursor(LoadCursor(NULL, IDC_SIZEWE));
				}
				else
				{
					m_eResizeDir = Up;
					SetCursor(LoadCursor(NULL, IDC_SIZENS));
				}
			}
			else
			{
				SetCursor(LoadCursor(NULL, IDC_ARROW));				
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

void ZGuiEd::MouseMove(bool bLeft, int x, int y)
{
	if(IsDlgButtonChecked(g_kDlgBoxRight, IDC_DISABLE_MOVE_CB) && m_bResize == false)
		return;

	if(x > 800 || y > 600)
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
				m_pkFocusWnd->SetPos(x > 0 ? x : 0, y > 0 ? y : 0, true, true);

			CheckMovement();
		}
	}

	if(m_pkFocusWnd)
	{
		char text[25];
		Rect rc;

		if(m_pkFocusWnd->GetParent())
			rc = m_pkFocusWnd->GetWndRect();
		else
			rc = m_pkFocusWnd->GetScreenRect();

		sprintf(text, "%i", rc.Left); SetWindowText(GetCtrl(IDC_POSX_EB, 0), text);
		sprintf(text, "%i", rc.Top); SetWindowText(GetCtrl(IDC_POSY_EB, 0), text);
		sprintf(text, "%i", rc.Width()); SetWindowText(GetCtrl(IDC_WIDTH_EB, 0), text);
		sprintf(text, "%i", rc.Height()); SetWindowText(GetCtrl(IDC_HEIGHT_EB, 0), text);
	}
}

void ZGuiEd::OnCommand(int iCtrlID, int iEvent)
{	
	// Just skip all commands if in test mode and not pressing the test button.
	if(m_bTestGUI && iCtrlID != IDC_TESTGUI_BN)
		return;

	string strText;

	ZGuiSkin** ppkSkin;

	switch(iCtrlID)
	{	
		case IDC_HIDDEN_FROM_START_CB:
			if(m_pkFocusWnd)
			{
				bool bHidden = IsDlgButtonChecked(g_kDlgBoxBottom, 
					IDC_HIDDEN_FROM_START_CB);

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
			break;

		case IDC_MULTILINE_CB:
			if(m_pkFocusWnd)
			{
				bool bMultiLine = IsDlgButtonChecked(g_kDlgBoxBottom, IDC_MULTILINE_CB);
				((ZGuiTextbox*)m_pkFocusWnd)->ToggleMultiLine(bMultiLine);

				if(bMultiLine)
				{
					((ZGuiTextbox*)m_pkFocusWnd)->SetScrollbarSkin(GetSkin("DefSBrBkSkin"),
						GetSkin("DefSBrNSkin"), GetSkin("DefSBrFSkin"),
						GetSkin("DefSBrScrollUpSkin_u"), GetSkin("DefSBrScrollUpSkin_d"),
						GetSkin("DefSBrScrollDownSkin_u"), GetSkin("DefSBrScrollDownSkin_d"));
						}
				}
			break;

		case IDC_CLOSE_BUTTON:
			m_pkZeroFps->QuitEngine();
			break;

		case IDC_ACTIVATE_HELP_CB:
			ActivateHelp(IsDlgButtonChecked(g_kDlgBoxRight, iCtrlID));
			break;

		case IDC_FILELIST_CB:
			if(iEvent == CBN_DROPDOWN)
				UpdateScriptList();
			break;

		case IDC_SAVE_SCRIPT:
			m_iTask = TASK_SAVE_SCRIPT;
			break;

		case IDC_NEWCNTRL_CB:
			if(iEvent == CBN_SELENDOK)
				FilterWnd();
			break;

		case IDC_COPY_WND_BN:
			m_iTask = TASK_COPY_WND;
			break;

		case IDC_PASTE_WND_BN:
			m_iTask = TASK_PASTE_WND;
			break;

		case IDC_COPY_SKIN:
			m_iTask = TASK_COPY_SKIN;
			break;

		case IDC_COPY_ALL_SKINS:
			m_iTask = TASK_COPY_ALL_SKINS;
			break;
			
		case IDC_PASTE_SKIN:
			m_iTask = TASK_PASTE_SKIN;
			break;

		// Select window from list.
		case IDC_WINDOW_LIST:
			if(iEvent == LBN_SELCHANGE)
			{
				int sel = SendDlgItemMessage(g_kDlgBoxRight, iCtrlID, LB_GETCURSEL, 0, 0);
				if(sel != LB_ERR)
				{
					char szWndName[200];
					if(SendDlgItemMessage(g_kDlgBoxRight, iCtrlID, LB_GETTEXT, sel, 
						(LPARAM) (LPCSTR) szWndName) != LB_ERR)
					{
						ZGuiWnd* pkWnd = GetWnd(szWndName);
						if(pkWnd)
						{
							m_pkFocusWnd = pkWnd;						
							UpdateInfo();
							SelNewSkin(0);
						}
					}					
				}
			}
			break;

		case IDC_TEXTURE_LIST:
			if(iEvent == LBN_SELCHANGE)
			{
				int sel = SendDlgItemMessage(g_kDlgBoxRight, iCtrlID, LB_GETCURSEL, 0, 0);
				if(sel != LB_ERR)
				{
					char szTexName[200];
					if(SendDlgItemMessage(g_kDlgBoxRight, iCtrlID, LB_GETTEXT, sel, 
						(LPARAM) (LPCSTR) szTexName) != LB_ERR)
					{
						string strName = m_strCurrTexDir + string("/") + string(szTexName);

						ZFVFileSystem* m_pkFileSys = reinterpret_cast<ZFVFileSystem*>(
								g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));	

						string strRealName = m_pkFileSys->GetFullPath(strName.c_str());

						UpdatePreviewImage(strRealName.c_str());
					}
				}
			}			
			else
			if(iEvent == LBN_DBLCLK)
			{
				int sel = SendDlgItemMessage(g_kDlgBoxRight, iCtrlID, LB_GETCURSEL, 0, 0);
				if(sel != LB_ERR)
				{
					char szTexName[200];
					if(SendDlgItemMessage(g_kDlgBoxRight, iCtrlID, LB_GETTEXT, sel, 
						(LPARAM) (LPCSTR) szTexName) != LB_ERR)
					{
						if(strcmp(szTexName, "..") == 0)
						{							
							int slash = m_strCurrTexDir.find_last_of('/'); 
							if(slash != string::npos)
							{								
								if( m_strCurrTexDir != "data/textures/gui" ) // dont allow browsing lower then gui texture folder
								{
									m_strCurrTexDir.erase(slash, m_strCurrTexDir.size()-slash);									
									SendDlgItemMessage(g_kDlgBoxRight, IDC_TEXTURE_LIST, LB_RESETCONTENT, 0, 0);
									AddFilesInFolderToListbox(m_strCurrTexDir.c_str());
								}
							}
						}
						else
						if(strrchr(szTexName, '.') == NULL)
						{
							m_strCurrTexDir += string("/") + string(szTexName);							
							SendDlgItemMessage(g_kDlgBoxRight, IDC_TEXTURE_LIST, LB_RESETCONTENT, 0, 0);
							AddFilesInFolderToListbox(m_strCurrTexDir.c_str());
						}					
					}
				}				
			}
			break;


		case IDC_SKINELEMENTS_LIST:
			if(iEvent == LBN_SELCHANGE)
			{
				SelNewSkin();
			}			
			break;

		case IDC_SKINTYPE_BACKGROUND_RB:
		case IDC_SKINTYPE_HORZBORDER_RB:
		case IDC_SKINTYPE_VERTBORDER_RB:
		case IDC_SKINTYPE_CORNERBORDER_RB:
			SelNewSkin();
			break;

		case IDC_CREATE_WND_BN:
			m_iTask = TASK_CREATE_NEW_WINDOW;
			break;

		case IDC_DELETE_WND_BN:
			m_iTask = TASK_DELETE_SEL_WINDOW;
			break;

		case IDC_NEWGUI_BN:
			m_iTask = TASK_NEW_GUI;
			break;

		case IDC_BRING_TO_FRONT:
			m_iTask = TASK_BRING_WND_TO_FRONT;
			break;

		case IDC_SET_TEXTURE_BN:
			m_iTask = TASK_SET_TEXTURE;
			break;

		case IDC_REMOVE_TEXTURE_BN:
			m_iTask = TASK_REMOVE_TEXTURE;
			break;

		case IDC_TILE_CB:
			if(GetSelSkin(ppkSkin))
			{
				(*ppkSkin)->m_bTileBkSkin = IsDlgButtonChecked(g_kDlgBoxBottom, IDC_TILE_CB);
			}
			break;

		case IDC_TRANSPARENT_CB:
			if(GetSelSkin(ppkSkin))
			{
				(*ppkSkin)->m_bTransparent = IsDlgButtonChecked(g_kDlgBoxBottom, IDC_TRANSPARENT_CB);
			}
			break;

		case IDC_VISIBLE_CHECK:
			if(m_pkFocusWnd)
			{
				bool bVisible = IsDlgButtonChecked(g_kDlgBoxRight, IDC_VISIBLE_CHECK);
				if(bVisible)
					m_pkFocusWnd->Show();
				else
					m_pkFocusWnd->Hide();
			}
			break;

		case IDC_LOADGUI:
			//sel = SendDlgItemMessage(g_kDlgBoxRight, IDC_FILELIST_CB, CB_GETCURSEL, 0, 0);
			//if(sel != CB_ERR)
			//{
			//	char text[100];
			//	SendDlgItemMessage(g_kDlgBoxRight, IDC_FILELIST_CB, CB_GETLBTEXT, sel, (LPARAM) (LPTSTR) text);
			//	m_strNewFileToLoad = string("data/script/gui/") + string(text);
			//}

			strText = GetSelItemText(IDC_FILELIST_CB, true);
			if(strText != "")
				m_strNewFileToLoad = string("data/script/gui/") + strText;
			break;

		case IDC_RESIZE_WND:
			m_bResize = !m_bResize;
			break;

		case IDC_TESTGUI_BN:
					
			m_bTestGUI = !m_bTestGUI;
			m_pkGui->Activate(m_bTestGUI); 
			m_iTask = TASK_TEST_GUI;			

			break;

		case IDC_FREE_MOVEMENT_CB:
			if(m_pkFocusWnd && GetWndType(m_pkFocusWnd) == Wnd) 
			{
				bool bFreeMovement = IsDlgButtonChecked(g_kDlgBoxBottom, IDC_FREE_MOVEMENT_CB);
				if(bFreeMovement)
					m_pkFocusWnd->SetMoveArea(Rect(0,0,800,600), true);
				else
				{
					Rect rc = m_pkFocusWnd->GetScreenRect();
					m_pkFocusWnd->SetMoveArea(rc, true);					
				}
			}
			break;

		case IDC_FORCE_CAPTURE_TO_SEL_CB:
			m_bForceCaptureToSel = !m_bForceCaptureToSel;
			break;

		case IDC_ONLY_OF_WND_TYPE_CB:
			FilterWnd();
			break;

		case IDC_RESIZE_TO_IMAGE:
			m_iTask = TASK_RESIZE_WND_TO_IMAGE;
			break;

		case IDC_SELECTFONT_BN:
			OpenSelectFontDlg();
			break;

		case IDC_SELECTFONT_OK_BN:			
			m_iTask = TASK_SET_FONT;				
			break;
			
		case IDC_SELECTFONT_CANCEL_BN:			
			ShowWindow(GetParent(g_kFontDlg), SW_HIDE);				
			break;		

		case IDC_SHOW_KEYS_BN:
			{
				char text[] =
				{
					"Copy & Paste\r\n" \
					"Hold Left Ctrl and click on a widget or press Left Ctrl + C and then Left Ctrl + V.\r\n" \
					"\r\n" \
					"Resize\r\n" \
					"Hold Left Shift and click close to right side to resize the widget horizontally \r\n" \
					"or close to bottom side to resize the widget vertically.\r\n" \
					"\r\n" \
					"Delete\r\n" \
					"Press Delete key.\r\n" \
					"\r\n" \
					"Move widget\r\n" \
					"Press Left, Right, Up or Down Arrow key to move to that direction." \
					"\r\n" \
				};
			
				MessageBox(GetParent(g_kDlgBoxRight), text, "Input Info", MB_OK);
			}
			break;

		case IDC_WNDALIGNMENT_TOPLEFT:
			if(m_pkFocusWnd) m_pkFocusWnd->m_iWndAlignment = TopLeft;
			break;
		case IDC_WNDALIGNMENT_TOPRIGHT:
			if(m_pkFocusWnd) m_pkFocusWnd->m_iWndAlignment = TopRight;
			break;
		case IDC_WNDALIGNMENT_BOTTOMLEFT:
			if(m_pkFocusWnd) m_pkFocusWnd->m_iWndAlignment = BottomLeft;
			break;
		case IDC_WNDALIGNMENT_BOTTOMRIGHT:
			if(m_pkFocusWnd) m_pkFocusWnd->m_iWndAlignment = BottomRight;
			break;
		case IDC_WNDALIGNMENT_CENTERHORZ:
			if(m_pkFocusWnd) m_pkFocusWnd->m_iWndAlignment = CenterHorz;
			break;
		case IDC_WNDALIGNMENT_CENTERVERT:
			if(m_pkFocusWnd) m_pkFocusWnd->m_iWndAlignment = CenterVert;
			break;
		case IDC_WNDALIGNMENT_CENTER:
			if(m_pkFocusWnd) m_pkFocusWnd->m_iWndAlignment = Center;
			break;

		case IDC_RESIZETYPE_DONT:
			if(m_pkFocusWnd) m_pkFocusWnd->m_iResizeType = Center;
			break;
		case IDC_RESIZETYPE_WIDTH:
			if(m_pkFocusWnd) m_pkFocusWnd->m_iResizeType = ResizeWidth;
			break;
		case IDC_RESIZETYPE_HEIGHT:
			if(m_pkFocusWnd) m_pkFocusWnd->m_iResizeType = ResizeHeight;
			break;
		case IDC_RESIZETYPE_BOTH:
			if(m_pkFocusWnd) m_pkFocusWnd->m_iResizeType = Resize;
			break;
	}
}