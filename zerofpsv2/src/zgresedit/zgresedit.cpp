// zgresedit.cpp: implementation of the ZGResEdit class.
//
//////////////////////////////////////////////////////////////////////

#include "zgresedit.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "scene.h"

ZGResEdit g_kResEdit("ResEdit",0,0,0);

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
	switch(msg)
	{
	case ZGM_COMMAND:
		g_kResEdit.OnCommand(((int*)params)[0], (((int*)params)[1] == 1) ? true : false, win);
		break;
	case ZGM_CBN_SELENDOK:
		g_kResEdit.OnSelectCB(((int*)params)[0], ((int*)params)[1], win);
		break;
	case ZGM_SELECTTREEITEM:
		char** pszParams; pszParams = (char**) params;
		g_kResEdit.OnClickTreeItem( pszParams[0], pszParams[1], 
			pszParams[2], pszParams[3][0] == '1' ? true : false);		
		if(pszParams[0])
			delete[] pszParams[0];
		if(pszParams[1])
			delete[] pszParams[1];
		if(pszParams[2])
			delete[] pszParams[2];
		if(pszParams[3])
			delete[] pszParams[3];
		break;
	case ZGM_SELECTLISTITEM:
		g_kResEdit.OnClickListbox(((int*)params)[0],((int*)params)[1],win);
		break;
	}
	return true;
}

ZGResEdit::ZGResEdit(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	m_pkFocusWnd = m_pkResizeWnd = m_pkMainWnd = m_pkMoveWnd = NULL;
	m_eEditMode = MOVE;
	m_iXPos = 0;  m_iYPos = 0; m_iWidth = 200; m_iHeight = 150;
	m_kSelStart = Point(-1,-1);
	m_iRadiogroupCounter = RADIO_GROUP_COUNTER_START;
	m_bLeftButtonPressed = false;
	//m_iHighestZValue = 10;
}

void ZGResEdit::OnInit()
{
	glEnable(GL_LIGHTING );

	m_pkLight->SetLighting(true);

	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.25,100);	

	// create gui script
	GuiAppLua::Init(&g_kResEdit, m_pkScript);

	InitializeGui(pkGui, pkTexMan, m_pkScript, m_pkGuiMan, 
		"data/textures/text/small.bmp",
		"data/script/gui/gui_res_edit.lua");

	SetTitle("The Migthy GUI Editor!");
	m_pkInput->ShowCursor(false);

	pkGui->SetCursor(0,0, pkTexMan->Load("data/textures/gui/cursor.bmp", 0),
		pkTexMan->Load("data/textures/gui/cursor_a.bmp", 0), 32, 32);

	m_pkScene = new Scene();
	m_pkScene->Init(this);

	m_pkFps->m_bClientMode = true;

	m_pkInput->ToggleGrab(); // koppla på grab mode
}

void ZGResEdit::OnIdle()
{
	m_pkFps->SetCamera(m_pkCamera);		
	m_pkFps->GetCam()->ClearViewPort();	
	m_pkFps->UpdateCamera(); 	

	OnKeyDown(m_pkInput->GetQueuedKey());

	if(m_eEditMode == VIEW)
		return;

	int x,y;
	m_pkInput->MouseXY(x,y);

	pkGui->SetLineColor(255,0,0);

	if(!m_pkInput->Pressed(KEY_LSHIFT))
	{
		m_eEditMode = MOVE;
	}

	if(m_pkInput->Pressed(MOUSELEFT))
	{
		if(m_pkInput->Pressed(KEY_LSHIFT) && m_pkResizeWnd != NULL)
		{
			pkGui->SetLineColor(255,255,0);
			m_eEditMode = RESIZE;
		}

		if(!m_bLeftButtonPressed)
		{
			OnMouseClick(false, x, y);
			m_bLeftButtonPressed = false;
		}
		
		m_bLeftButtonPressed = true;
	}
	else
	{
		if(m_bLeftButtonPressed)
		{
			OnMouseClick(true, x, y);
			m_bLeftButtonPressed = false;
		}
	}

	if(m_eEditMode == MOVE)
		MoveWnd(x,y);
	else
	if(m_eEditMode == RESIZE)
		ResizeWnd(x,y);

	if(m_pkFocusWnd)
	{
		ZGuiWnd* pkParent = m_pkFocusWnd->GetParent();
		ZGuiTabCtrl* pkTabCtrl = NULL;

		if(pkParent && GetType(pkParent) == TabControl)
			pkTabCtrl = static_cast<ZGuiTabCtrl*>(pkParent);
		else
		if(GetType(m_pkFocusWnd) == TabControl)
			pkTabCtrl = static_cast<ZGuiTabCtrl*>(m_pkFocusWnd);

		if(pkTabCtrl)
		{
			unsigned int pressed_keynumber = 1000;
			if(m_pkInput->Pressed(KEY_1)) pressed_keynumber = 0;
			else if(m_pkInput->Pressed(KEY_2)) pressed_keynumber = 1;
			else if(m_pkInput->Pressed(KEY_3)) pressed_keynumber = 2;
			else if(m_pkInput->Pressed(KEY_4)) pressed_keynumber = 3;
			else if(m_pkInput->Pressed(KEY_5)) pressed_keynumber = 4;
			else if(m_pkInput->Pressed(KEY_6)) pressed_keynumber = 5;
			else if(m_pkInput->Pressed(KEY_7)) pressed_keynumber = 6;
			else if(m_pkInput->Pressed(KEY_8)) pressed_keynumber = 7;
			else if(m_pkInput->Pressed(KEY_9)) pressed_keynumber = 8;
			else if(m_pkInput->Pressed(KEY_0)) pressed_keynumber = 9;

			if(pressed_keynumber < pkTabCtrl->GetNumPages() )
			{
				pkTabCtrl->SetCurrentPage(pressed_keynumber); 
				
				ZGuiWnd* pkPage = pkTabCtrl->GetPage(pressed_keynumber);

				pkGui->SetFocus(pkPage);
				m_pkFocusWnd = pkPage;
				m_pkMainWnd = pkPage;
			}
		}
	}

	if(m_pkFocusWnd)
		DrawSelectionRect(m_pkFocusWnd);
}

void ZGResEdit::OnHud(void) 
{	
	m_pkFps->DevPrintf("common","Active Propertys: %d",m_pkObjectMan->GetActivePropertys());	
	m_pkFps->DevPrintf("common", "Fps: %f",m_pkFps->m_fFps);	
	m_pkFps->DevPrintf("common","Avrage Fps: %f",m_pkFps->m_fAvrageFps);			
	
	m_pkFps->m_bGuiMode = false;
	m_pkFps->ToggleGui();
}

void ZGResEdit::OnKeyDown(int iKey)
{
	switch(iKey)
	{
	case KEY_RETURN:
		ExecuteCommand();
		break;

	case KEY_F1:
		m_eEditMode = VIEW;
		m_pkScene->m_pkWorkSpace->Hide();
		m_pkScene->m_pkPropertyWnd->Hide();
		m_pkScene->m_pkViewWindow->Hide();
		m_pkScene->m_pkSelectFileWnd->Hide();

		EnableWnds(true);
		break;
	
	case KEY_F2:

		m_eEditMode = MOVE;
		m_pkScene->m_pkWorkSpace->Show();
		m_pkScene->m_pkPropertyWnd->Show();

		EnableWnds(false);
		break;

	case KEY_DELETE:
		DeleteWnd(m_pkFocusWnd);
		break;

	case KEY_UP:
		if(m_pkFocusWnd) 
		{
			if(m_pkInput->Pressed(KEY_LSHIFT))
			{
				Rect rc = m_pkFocusWnd->GetScreenRect(); 
				m_pkFocusWnd->Resize(rc.Width(),rc.Height()-1);
				SetTextInt("HeightTextbox", rc.Height()-1); 
			}
			else
			{
				m_pkFocusWnd->Move(0,-1,true,false);
				SetTextInt("PosYTextbox", m_pkFocusWnd->GetParent() ? 
					m_pkFocusWnd->GetWndRect().Top : m_pkFocusWnd->GetScreenRect().Top); 
			}
		}
		break;
	case KEY_DOWN:
		if(m_pkFocusWnd) 
		{
			if(m_pkInput->Pressed(KEY_LSHIFT))
			{
				Rect rc = m_pkFocusWnd->GetScreenRect(); 
				m_pkFocusWnd->Resize(rc.Width(),rc.Height()+1);
				SetTextInt("HeightTextbox", rc.Height()+1); 
			}
			else
			{
				m_pkFocusWnd->Move(0,1,true,false);
				SetTextInt("PosYTextbox", m_pkFocusWnd->GetParent() ? 
					m_pkFocusWnd->GetWndRect().Top : m_pkFocusWnd->GetScreenRect().Top); 
			}
		}
		break;
	case KEY_LEFT:
		if(m_pkFocusWnd)
		{
			if(m_pkInput->Pressed(KEY_LSHIFT))
			{
				Rect rc = m_pkFocusWnd->GetScreenRect(); 
				m_pkFocusWnd->Resize(rc.Width()-1,rc.Height());
				SetTextInt("WidthTextbox", rc.Width()-1); 
			}
			else
			{
				m_pkFocusWnd->Move(-1,0,true,false);

				SetTextInt("PosXTextbox", m_pkFocusWnd->GetParent() ? 
					m_pkFocusWnd->GetWndRect().Left : m_pkFocusWnd->GetScreenRect().Left); 
			}
		}
		break;
	case KEY_RIGHT:
		if(m_pkFocusWnd)
		{
			if(m_pkInput->Pressed(KEY_LSHIFT))
			{
				Rect rc = m_pkFocusWnd->GetScreenRect(); 
				m_pkFocusWnd->Resize(rc.Width()+1,rc.Height());
				SetTextInt("WidthTextbox", rc.Width()+1); 
			}
			else
			{
				m_pkFocusWnd->Move(1,0,true,false);
				SetTextInt("PosXTextbox", m_pkFocusWnd->GetParent() ? 
					m_pkFocusWnd->GetWndRect().Left : m_pkFocusWnd->GetScreenRect().Left); 
			}
		}
		break;
	}
}

void ZGResEdit::OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd *pkMainWnd)
{
	ZGuiWnd* pkWndClicked = GetWnd(iID);

	if(pkWndClicked == NULL)
		return; 

	string strMainWndName = pkMainWnd->GetName();
	string strClickWndName = pkWndClicked->GetName();

	if(strMainWndName == "PropertyWnd")
	{
		if(strClickWndName == "UpdateWndDataBn" && m_pkFocusWnd)
		{
			UpdatePropertyData();
		}
		else
		if(strClickWndName == "ResizeWndToTexBn")
		{
			char* szSkinType = GetSelItem("SkinTypeCB");

			if(szSkinType != NULL && m_pkFocusWnd != NULL )
				m_pkScene->ScaleWndToTexSize(m_pkFocusWnd, szSkinType);
		}
	}
	else
	if(strMainWndName == "WorkSpace")
	{
		if(strClickWndName == "OpenBn")
		{
			m_bSaveFile = false;
			OpenWnd(m_pkScene->m_pkSelectFileWnd, true);
			SetText("OwerwriteWarning", "", true);
		}
		else
		if(strClickWndName == "SaveBn")
		{
			m_bSaveFile = true;
			OpenWnd(m_pkScene->m_pkSelectFileWnd, true);
			m_bOverwriteWarning = true;
			SetText("OwerwriteWarning", "", true);
		}
		else
		if(strClickWndName == "DeleteWnd")
		{
			DeleteWnd(m_pkFocusWnd);
			UpdateViewWnd();
		}
		else
		if(strClickWndName == "ToogleViewWnd")
		{	
			if(m_pkScene->m_pkViewWindow->IsVisible())
			{
				OpenWnd(m_pkScene->m_pkViewWindow, false);
			}
			else
			{
				UpdateViewWnd();
				OpenWnd(m_pkScene->m_pkViewWindow, true);
			}
		}
		else
		if(strClickWndName == "StretchTextureCB" && m_pkFocusWnd)
		{
			vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
			m_pkFocusWnd->GetWndSkinsDesc(vkSkinDesc);

			bool bTile = !IsButtonChecked("StretchTextureCB");

			char* szSkinType = GetSelItem("SkinTypeCB");
			for(unsigned int i=0; i<vkSkinDesc.size(); i++)
				if(strcmp(szSkinType, vkSkinDesc[i].second.c_str()) == 0)
				{
					(*vkSkinDesc[i].first)->m_bTileBkSkin = bTile;

					if(strcmp(vkSkinDesc[i].second.c_str() , "Button up") == 0)
						m_pkFocusWnd->SetSkin((*vkSkinDesc[i].first));
					if(strcmp(vkSkinDesc[i].second.c_str(), "Checkbox: Button up") == 0)
						m_pkFocusWnd->SetSkin((*vkSkinDesc[i].first));
					break;
				}
		}
		else
		if(strClickWndName == "RemoveTextureBn" && m_pkFocusWnd)
		{
			// Ta bort textur på vald kontroll
			vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
			m_pkFocusWnd->GetWndSkinsDesc(vkSkinDesc);

			char* szSkinType = GetSelItem("SkinTypeCB");
			for(unsigned int i=0; i<vkSkinDesc.size(); i++)
				if(strcmp(szSkinType, vkSkinDesc[i].second.c_str()) == 0)
				{
					if(IsButtonChecked("TextureBK"))
						(*vkSkinDesc[i].first)->m_iBkTexID = -1;
					else
					if(IsButtonChecked("TextureBdH"))
						(*vkSkinDesc[i].first)->m_iHorzBorderTexID = -1;
					else
					if(IsButtonChecked("TextureBdV"))
						(*vkSkinDesc[i].first)->m_iVertBorderTexID = -1;
					else
					if(IsButtonChecked("TextureBdC"))
						(*vkSkinDesc[i].first)->m_iBorderCornerTexID = -1;			
					else
					if(IsButtonChecked("TextureAlphaBK"))
						(*vkSkinDesc[i].first)->m_iBkTexAlphaID = -1;
					else
					if(IsButtonChecked("TextureAlphaBdH"))
						(*vkSkinDesc[i].first)->m_iHorzBorderTexAlphaID = -1;
					else
					if(IsButtonChecked("TextureAlphaBdV"))
						(*vkSkinDesc[i].first)->m_iVertBorderTexAlphaID = -1;
					else
					if(IsButtonChecked("TextureAlphaBdC"))
						(*vkSkinDesc[i].first)->m_iBorderCornerTexAlphaID = -1;

					break;
				}
		}
		else
		if(strClickWndName == "TransparentTextureCB" && m_pkFocusWnd)
		{
			vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
			m_pkFocusWnd->GetWndSkinsDesc(vkSkinDesc);

			bool bTrans = IsButtonChecked("TransparentTextureCB");

			char* szSkinType = GetSelItem("SkinTypeCB");
			for(unsigned int i=0; i<vkSkinDesc.size(); i++)
				if(strcmp(szSkinType, vkSkinDesc[i].second.c_str()) == 0)
				{
					(*vkSkinDesc[i].first)->m_bTransparent = bTrans;
					break;
				}
		}
		else
		if(strClickWndName == "CreateWndBn")
		{
			char* szName = GetText("NewWndNameTextbox");
			char szParent[100];
			char szLabel[100];

			if(m_pkMainWnd)
				strcpy(szParent, m_pkMainWnd->GetName());
			else
				strcpy(szParent, "");

			string strWndType = ((ZGuiCombobox*) GetWnd("SelectWndType"))->
				GetListbox()->GetSelItem()->GetText();   

			unsigned long dwFlag = 0;

			GuiType eWndType = Wnd;
			if(strWndType == "Button")			eWndType = Button;		else
			if(strWndType == "Checkbox")		eWndType = Checkbox;		else
			if(strWndType == "Combobox")		eWndType = Combobox;		else
			if(strWndType == "Label")			eWndType = Label;			else
			if(strWndType == "Listbox")		eWndType = Listbox;		else
			if(strWndType == "Radiobn.group")eWndType = Radiobutton; else
			if(strWndType == "Radiobutton")	eWndType = Radiobutton; else
			if(strWndType == "H.Scrollbar")	eWndType = Scrollbar;	else
			if(strWndType == "V.Scrollbar")	eWndType = Scrollbar;	else
			if(strWndType == "Slider")			eWndType = Slider;		else
			if(strWndType == "TabControl")	eWndType = TabControl;	else
			if(strWndType == "S.Textbox")		eWndType = Textbox;		else
			if(strWndType == "M.Textbox")		{eWndType = Textbox;	dwFlag = EB_IS_MULTILINE; }	else
			if(strWndType == "Treebox")		eWndType = Treebox;

			if(strWndType == "Radiobn.group" || (strWndType == "Radiobutton" &&
				m_iRadiogroupCounter == RADIO_GROUP_COUNTER_START))
			{
				if(m_bHaveAskedForDefProp == false)
				{
					OpenDefPropWnd(strWndType);
					return;
				}
				else
				{
					m_iRadiogroupCounter++;
					CreateNewRadiobuttonGroup(GetText("NewRadiogroupNameEB"),m_iRadiogroupCounter);
				}
			}

			if(eWndType != Wnd && m_pkMainWnd == NULL)
			{
				printf("Failed to create control! No parent selected\n");
				return;
			}

			if(eWndType == Label || eWndType == Radiobutton || eWndType == Checkbox)
				strcpy(szLabel, szName);
			else
				strcpy(szLabel, "");

			if(eWndType == Wnd)
				strcpy(szParent,"");

			if( m_pkFocusWnd && GetType(m_pkFocusWnd) == TabControl && eWndType != Wnd)
			{
				ZGuiTabCtrl* pkTabCtrl = (ZGuiTabCtrl*) m_pkFocusWnd;

				if(pkTabCtrl->GetNumPages() == 0)
				{
					printf("Failed to create control, no pages in tabcontrol!\n");
					return;
				}

				int iCurrPage = pkTabCtrl->GetCurrentPage(); 
				m_pkFocusWnd = pkTabCtrl->GetPage(iCurrPage);
				dwFlag = iCurrPage;
			}

			if( m_pkFocusWnd && GetType(m_pkFocusWnd) == TabControl)
			{
				if(eWndType == Wnd)
				{
					AddTabPage((char*)m_pkFocusWnd->GetName(), szName, szName);
				}
			}
			else
			{
				if(m_pkFocusWnd)
				{
					if(GetType(m_pkFocusWnd) == eWndType)
					{
						Rect rc = m_pkFocusWnd->GetScreenRect();
						m_iWidth = rc.Width();
						m_iHeight = rc.Height();
					}
				}

				if(CreateWnd( eWndType, szName,  szParent, szLabel, m_iXPos,  
					m_iYPos, m_iWidth, m_iHeight, dwFlag) == NULL)
				{
					printf("Failed to create window!\n");
					return;
				}
			}

			ZGuiWnd* pkWnd = m_pkScene->GetWnd(szName);

			if(pkWnd == NULL)
			{
				printf("Failed to get window\n");
				return;
			}

			m_pkFocusWnd = pkWnd;

			if(eWndType == Wnd)
			{
				m_pkMainWnd = pkWnd;
			}
		
			// Skapa nya texturer och kopiera de gamla mot nya unika...
			vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
			pkWnd->GetWndSkinsDesc(vkSkinDesc);
			for(unsigned int i=0; i<vkSkinDesc.size(); i++)
			{
				ZGuiSkin* pkPrevSkin = *vkSkinDesc[i].first; 

				if(pkPrevSkin)
				{
					ZGuiSkin* pkNewSkin = new ZGuiSkin(pkPrevSkin);
					*vkSkinDesc[i].first = pkNewSkin;

					if(i==0)
					{
						//
						// Set skin parameter settings
						//
						((ZGuiRadiobutton*)GetWnd("TextureBK"))->Check();

						if(!pkNewSkin->m_bTileBkSkin)
							((ZGuiCheckbox*)GetWnd("StretchTextureCB"))->CheckButton();
						else
							((ZGuiCheckbox*)GetWnd("StretchTextureCB"))->UncheckButton();

						if(pkNewSkin->m_bTransparent)
							((ZGuiCheckbox*)GetWnd("TransparentTextureCB"))->CheckButton();
						else
							((ZGuiCheckbox*)GetWnd("TransparentTextureCB"))->UncheckButton();
					}
				}
				else
				{
					*vkSkinDesc[i].first = new ZGuiSkin; 
					(*vkSkinDesc[i].first)->m_bTransparent = true;
				}
			}

			AddStandardElements(pkWnd);

			// Slumpa fram ett nytt controllnamn till nästa fönster...
			char szNewName[100];
			sprintf(szNewName, "%s%i", ((ZGuiCombobox*)GetWnd("SelectWndType"))->
				GetListbox()->GetSelItem()->GetText(), rand()%1000);
			SetText("NewWndNameTextbox", szNewName);

			pkGui->SetFocus(pkWnd);
			pkWnd->Disable(); // Disable wnd (otherwise some ctrls like listoboxes can't be moved easily)

			UpdateViewWnd();
			UpdateSkinList(pkWnd);
		}
	}
	else
	if(strMainWndName == "ViewWindow")
	{
		if(strClickWndName == "ShowHideWndCB")
		{
			char* szItem = GetSelItem("MainWndList");

			if(szItem)
			{
				if(IsButtonChecked("ShowHideWndCB"))
				{
					m_pkScene->GetWnd(szItem)->Show();
				}
				else
				{
					ZGuiWnd* pkWnd = m_pkScene->GetWnd(szItem);

					if(m_pkFocusWnd)
					{
						if(m_pkFocusWnd->GetParent(true) == pkWnd)
							m_pkFocusWnd = NULL;
						else
						if(m_pkFocusWnd->GetParent() == pkWnd)
							m_pkFocusWnd = NULL;
						else
						if( pkWnd == m_pkFocusWnd )
							m_pkFocusWnd = NULL;
					}

					if( pkWnd == m_pkMainWnd )
						m_pkMainWnd = NULL;
					if( pkWnd == m_pkResizeWnd )
						m_pkResizeWnd = NULL;

					m_pkMoveWnd = NULL;

					pkWnd->Hide();
				}
			}
		}
		else
		if(strClickWndName == "CloseViewWnd")
		{
			OpenWnd(m_pkScene->m_pkViewWindow, false);
		}
	}
	else
	if(strMainWndName == "SelectFileWnd")
	{
		if(strClickWndName == "SelectedFileCancel")
		{
			OpenWnd(m_pkScene->m_pkSelectFileWnd, false);
		}
		else
		if(strClickWndName == "SelectedFileOK")
		{
			char szFileName[1024];

			char* szTextbox = GetText("SelectedFileEB");
			int length = strlen(szTextbox);
			
			if(szTextbox != NULL && length > 1 && szTextbox[length-1] != '/')
				strcpy(szFileName, GetText("SelectedFileEB"));
			else
			{
				SetText("SelectedFileEB", "../data/");
				return;
			}

			if(m_bSaveFile && m_bOverwriteWarning && m_pkBasicFS->FileExist(szFileName))
			{
				m_bOverwriteWarning = false;
				SetText("OwerwriteWarning", "Overwrite?", true);
				return;
			}

			OpenWnd(m_pkScene->m_pkSelectFileWnd, false);

			if(szFileName && strlen(szFileName) > 0)
			{
				Serialization kSerialize;

				if(m_bSaveFile)
				{
					kSerialize.SaveGUI(szFileName, m_pkScene);
				}
				else
				{
					string strLoadName(szFileName);
					strLoadName.erase(0,2);

					// Börja med att ta bort alla fönster
					map<string, ZGuiWnd*> kWindows;
					m_pkGuiMan->GetWindows(kWindows);
					map<string, ZGuiWnd*>::iterator it2;
					for( it2 = kWindows.begin(); it2 != kWindows.end(); it2++)
					{
						if(m_pkScene->IsSceneWnd(it2->second) == false)
							DeleteWnd(it2->second);
					}

					kSerialize.LoadGUI(strLoadName.c_str(), this);

					kWindows.clear();
					m_pkGuiMan->GetWindows(kWindows);
					for( it2 = kWindows.begin(); it2 != kWindows.end(); it2++)
					{
						if(m_pkScene->IsSceneWnd(it2->second) == false)
						{
							ZGResEdit::AddStandardElements(it2->second);
							it2->second->Disable();
						}
					}

					UpdateViewWnd();
				}
			}
		}
		else
		if(strClickWndName == "SelectedFileUpdate")
		{
			((ZGuiTreebox*)GetWnd("FileTree"))->Clear();
			m_pkScene->BuildFileTree("FileTree", "data", ".lua");
			SetText("SelectedFileEB", "");
		}
	}
	else
	if(strMainWndName == "DefPropWnd")
	{
		if(strClickWndName == "DefPropWndCancelBn")
		{
			pkGui->ShowMainWindow(m_pkScene->m_pkDefProp, false);
		}
		else
		if(strClickWndName == "DefPropWndOKBn")
		{
			m_bHaveAskedForDefProp = true;
			pkGui->ShowMainWindow(m_pkScene->m_pkDefProp, false);

			OnCommand( GetWnd("CreateWndBn")->GetID(), false, m_pkScene->m_pkWorkSpace); // skapa knappen...
		}
	}
}

void ZGResEdit::OnMouseClick(bool bReleased, int x, int y)
{
	if(m_pkInput->Pressed(KEY_A))
		pkGui->m_bDisableAlphatest = true;
	else
		pkGui->m_bDisableAlphatest = false;

	ZGuiWnd* pkWnd = GetWndFromPoint(x,y);

	if(pkWnd == NULL)
		return;

	if(m_pkScene->IsSceneWnd(pkWnd))
	{
		m_pkResizeWnd = NULL;
		m_pkMoveWnd = NULL;

		m_kSelStart = Point(-1,-1);
		m_kClickPos = Point(-1,-1);

		if(bReleased)
			OnReleaseButton(x,y);

		return;
	}

	if(!bReleased) // om musknappen har tryckts ner men inte släppts.
	{
		m_kSelStart = Point(x,y);
		m_kClickPos = Point(x,y);

		if(m_pkMoveWnd == NULL)
		{
			bool bLegalMoveWnd = true;
			ZGuiWnd* pkParent = pkWnd->GetParent();

			if(pkParent)
			{
				GuiType parent_type = GetType(pkParent);
				if(parent_type == TabControl)
				{
					bLegalMoveWnd = false;
				}
			}

			if(bLegalMoveWnd)
				m_pkMoveWnd = pkWnd;

			Rect rc = pkWnd->GetScreenRect();
			m_iXPosBeforeMove = rc.Left; 
			m_iYPosBeforeMove = rc.Top;
		}

		if(pkWnd)
		{
			if(!m_pkScene->IsSceneWnd(pkWnd))
			{
				if(GetType(pkWnd) == Wnd)
				{
					m_pkMainWnd = pkWnd;
				}

				if(m_pkResizeWnd != NULL)
				{
					m_pkResizeWnd = pkWnd;
					return;
				}

				m_pkFocusWnd = pkWnd;

				bool bLegalResizeWnd = true;
				ZGuiWnd* pkParent = pkWnd->GetParent();

				if(pkParent)
				{
					GuiType parent_type = GetType(pkParent);
					if(parent_type == TabControl)
					{
						bLegalResizeWnd = false;
					}
				}

				if(bLegalResizeWnd)
					m_pkResizeWnd = pkWnd;

				OnSelectWnd(m_pkFocusWnd);
				
				Rect rc = pkWnd->GetScreenRect();

				int dl = abs(x - rc.Left), dr = abs(x - rc.Right);
				int dt = abs(y - rc.Top), db = abs(y - rc.Bottom);

				m_eCurrentResizeType = None;

				if(dl <= dr && dl <= dt && dl <= db)
					m_eCurrentResizeType = LeftSide;
				if(dr <= dl && dr <= dt && dr <= db)
					m_eCurrentResizeType = RightSide;
				if(dt <= dr && dt <= dl && dt <= db)
					m_eCurrentResizeType = TopSide;
				if(db <= dr && db <= dt && db <= dl)
					m_eCurrentResizeType = BottomSide;
				
				switch(m_eCurrentResizeType)
				{
					case LeftSide:   if(dl > 20) m_pkResizeWnd = NULL; break;
					case RightSide:  if(dr > 20) m_pkResizeWnd = NULL; break;
					case TopSide:    if(dt > 20) m_pkResizeWnd = NULL; break;
					case BottomSide: if(db > 20) m_pkResizeWnd = NULL; break;
				}
			}
		}
		else
		{
			m_pkFocusWnd = NULL;
			m_pkResizeWnd = NULL;
		}
	}
	else
	{
		OnReleaseButton(x,y);
	}
}


bool ZGResEdit::ClickInsideWnd(ZGuiWnd* pkWnd, int x, int y)
{
	ZGuiWnd* pkChild = pkWnd;
	Rect rc = pkChild->GetScreenRect();

	if(rc.Inside(x,y) && pkChild->IsVisible() )
	{
		if(m_eEditMode == RESIZE)
			return true;
		if(pkGui->ClickedWndAlphaTex(x,y,pkWnd)==true)
			return true;
	}

	return false;
}

ZGuiWnd* ZGResEdit::GetWndFromPoint(int x, int y)
{
	list<pair<ZGuiWnd*, int> > kTargets;

	map<string,ZGuiWnd*> kWindows;
	m_pkGuiMan->GetWindows(kWindows); 
	map<string,ZGuiWnd*>::iterator it = kWindows.begin();
	for( ; it != kWindows.end(); it++)
	{
		if(GetType((*it).second) == Wnd )
		{
			if(ClickInsideWnd((*it).second, x,y))
				kTargets.push_back( pair<ZGuiWnd*, int>((*it).second,0) );
		}
	}

	if(kTargets.empty())
		return NULL; // inte klickat på någon mainwindow

	kTargets.sort(SortZValue);

	ZGuiWnd* pkMainWnd = kTargets.front().first; // best mainwindow

	list<ZGuiWnd*> childs;
	pkMainWnd->GetChildrens(childs);

	if(childs.empty())
		return pkMainWnd; // finns inga barn så vi returnerar null

	kTargets.clear();

	list<ZGuiWnd*>::iterator it2 = childs.begin();
	for( ; it2 != childs.end(); it2++)
	{
		if(ClickInsideWnd((*it2), x,y))
			kTargets.push_back( pair<ZGuiWnd*, int>((*it2),0) );
	}

	if(kTargets.empty())
		return pkMainWnd; // klickade inte på något barn

	kTargets.sort(SortZValue);

	ZGuiWnd* pkChild = kTargets.front().first; // best child

	// Hantera tabuleringsboxar på ett speciellt sätt
	if(GetType(pkChild) == TabControl )
	{
		ZGuiTabCtrl* pkTabCtrl = static_cast<ZGuiTabCtrl*>(pkChild);

		int iZValue = pkTabCtrl->m_iZValue;
		
		ZGuiWnd* pkWnd = pkTabCtrl->GetPage(pkTabCtrl->GetCurrentPage()); 

		if(pkWnd && ClickInsideWnd(pkWnd, x,y))
		{
			pkGui->SetFocus(pkWnd);
			m_pkFocusWnd = pkWnd;
			m_pkMainWnd = pkWnd;

			list<ZGuiWnd*> childs;
			pkWnd->GetChildrens(childs);

			kTargets.clear();

			list<ZGuiWnd*>::iterator it2 = childs.begin();
			for( ; it2 != childs.end(); it2++)
			{
				if(ClickInsideWnd((*it2), x,y))
					kTargets.push_back( pair<ZGuiWnd*, int>((*it2),0) );
			}

			if(kTargets.empty())
			{
				if(!m_pkInput->Pressed(KEY_LSHIFT))
					return pkWnd;
				else
					return pkWnd->GetParent();
			}

			kTargets.sort(SortZValue);
			pkChild = kTargets.front().first; // best child
		}

	}
	
	return pkChild;
}



void ZGResEdit::SetPos(ZGuiWnd* pkWnd, int x, int y)
{
	Rect rc = pkWnd->GetScreenRect();

	bool bScreenSpace = false;

	if(pkWnd->GetParent() == NULL)
	{
		if(x < 0) x = 0;
		if(x + rc.Width() > 800) x -= x + rc.Width() - 800;

		if(y < 0) y = 0;
		if(y + rc.Height() > 600) y -= y + rc.Height() - 600;

		bScreenSpace = true;
	}
	else
	{
		Rect rcParent = pkWnd->GetParent()->GetScreenRect();

		if(x > rcParent.Width() - rc.Width()) 
			x = rcParent.Width() - rc.Width();
		if(x < 0) x = 0;

		if(y > rcParent.Height() - rc.Height()) 
			y = rcParent.Height() - rc.Height();
		if(y < 0) y = 0;
	}

	pkWnd->SetPos(x, y, bScreenSpace, true); 
}

void ZGResEdit::Resize(ZGuiWnd *pkWnd, int w, int h)
{
	Rect rc = pkWnd->GetScreenRect();

	if(pkWnd->GetParent() == NULL)
	{
		if(w + rc.Left > 800) w = 800-rc.Left;
		if(h + rc.Top  > 600) h = 800-rc.Top;

		list<ZGuiWnd*> childs;
		pkWnd->GetChildrens(childs);
	}

	pkWnd->Resize(w, h, true); 
}

void ZGResEdit::OnSelectCB(int ListBoxID, int iItemIndex, ZGuiWnd *pkMain)
{
	ZGuiCombobox* pkComboBox = NULL;
	list<ZGuiWnd*> childs;
	pkMain->GetChildrens(childs);

	list<ZGuiWnd*>::iterator it = childs.begin();
	for(; it != childs.end(); it++)
	{
		ZGuiWnd* pkWnd = (*it);
		if(pkWnd->GetID() == (unsigned int) ListBoxID)
		{
			if(typeid(*pkWnd)==typeid(ZGuiCombobox))
			{
				pkComboBox = static_cast<ZGuiCombobox*>(pkWnd);
				break;
			}
		}
	}

	if(pkComboBox != NULL)
	{
		if(pkComboBox == GetWnd("SelectWndType"))
		{
			string strWndType = pkComboBox->GetListbox()->GetSelItem()->GetText();

			char szName[100];
			sprintf(szName, "%s%i", strWndType.c_str(), rand()%1000);

			SetText("NewWndNameTextbox", szName);

			int x, y, w, h;

			if(strWndType == "Wnd")						{ x=0; y=0; w=200; h=150; }
			else if(strWndType == "Button")			{ x=20; y=20; w=20;  h=20; } 
			else if(strWndType == "Checkbox")		{ x=0; y=0; w=20;  h=20; }
			else if(strWndType == "Combobox")		{ x=0; y=0; w=100; h=20; }
			else if(strWndType == "Label")			{ x=0; y=0; w=50;  h=20; }
			else if(strWndType == "Listbox")			{ x=0; y=0; w=100; h=100; }
			else if(strWndType == "Radiobutton")	{ x=0; y=0; w=20;  h=20; }
			else if(strWndType == "Radiobn.group")	{ m_bHaveAskedForDefProp = false; x=0; y=0; w=20;  h=20; }
			else if(strWndType == "H.Scrollbar")	{ x=0; y=0; w=100; h=20; }
			else if(strWndType == "V.Scrollbar")	{ x=0; y=0; w=20;	 h=100; }
			else if(strWndType == "Slider")			{ x=0; y=0; w=100; h=20; }
			else if(strWndType == "TabControl")		{ x=0; y=0; w=150; h=100; }
			else if(strWndType == "S.Textbox")		{ x=0; y=0; w=100; h=20; }
			else if(strWndType == "M.Textbox")		{ x=0; y=0; w=100; h=50; }
			else if(strWndType == "Treebox")			{ x=0; y=0; w=100; h=100; }

			m_iXPos = x; m_iYPos = y;
			m_iWidth = w; m_iHeight = h;
		}
		else
		if(pkComboBox == GetWnd("SkinTypeCB"))
		{
			char* szSkinType = GetSelItem("SkinTypeCB");

			if(m_pkFocusWnd && szSkinType != NULL)
			{
				vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
				m_pkFocusWnd->GetWndSkinsDesc(vkSkinDesc);

				for(unsigned int i=0; i<vkSkinDesc.size(); i++)
					if(strcmp(szSkinType, vkSkinDesc[i].second.c_str()) == 0)
					{
						ZGuiSkin* pkSkin = (*vkSkinDesc[i].first);

						if(pkSkin)
						{
							if(!pkSkin->m_bTileBkSkin)
								((ZGuiCheckbox*)GetWnd("StretchTextureCB"))->CheckButton();
							else
								((ZGuiCheckbox*)GetWnd("StretchTextureCB"))->UncheckButton();

							if(pkSkin->m_bTransparent)
								((ZGuiCheckbox*)GetWnd("TransparentTextureCB"))->CheckButton();
							else
								((ZGuiCheckbox*)GetWnd("TransparentTextureCB"))->UncheckButton();
						}
						break;
					}
			}
		}
	}
}

void ZGResEdit::DeleteWnd(ZGuiWnd *pkWnd)
{
	if(pkWnd != NULL)
	{
		ZGuiWnd* pkSearch = pkWnd;

		while(1)
		{
			if(m_pkMainWnd == pkSearch)
				m_pkMainWnd = NULL;
			if(m_pkFocusWnd == pkSearch)
				m_pkFocusWnd = NULL;

			pkSearch = pkWnd->GetParent();
			if(pkSearch == NULL)
				break;
		}

		if(pkWnd->GetParent())
		{
			m_pkFocusWnd = pkWnd->GetParent();
			pkGui->SetFocus(m_pkFocusWnd);
		}

		m_pkScene->RemoveAlias(pkWnd);

		pkGui->UnregisterWindow(pkWnd);
		pkWnd = NULL;
	}
}

void ZGResEdit::OnClickTreeItem(char *szTreeBox, char *szParentNodeText, 
										 char *szClickNodeText, bool bHaveChilds)
{
	//
	// Byt textur på vald kontroll typ
	//
	if(strcmp(szTreeBox, "TextureTree") == 0)
	{
		if(szClickNodeText && bHaveChilds == false)
		{
			string strFullpath = string("data/textures/gui/");

			if(szParentNodeText)
				strFullpath += string(szParentNodeText);

			if(szClickNodeText)
				strFullpath += string(szClickNodeText);

			if(m_pkFocusWnd != NULL)
			{
				char* szSkinType = GetSelItem("SkinTypeCB");

				vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
				m_pkFocusWnd->GetWndSkinsDesc(vkSkinDesc);

				bool bFound = false;

				for(unsigned int i=0; i<vkSkinDesc.size(); i++)
					if(strcmp(szSkinType, vkSkinDesc[i].second.c_str()) == 0)
					{
						int id = pkTexMan->Load(strFullpath.c_str(), 0);

						(*vkSkinDesc[i].first)->m_bTransparent = false; // ändra

						if(IsButtonChecked("TextureBK"))
							(*vkSkinDesc[i].first)->m_iBkTexID = id;
						else
						if(IsButtonChecked("TextureBdH"))
							(*vkSkinDesc[i].first)->m_iHorzBorderTexID = id;
						else
						if(IsButtonChecked("TextureBdV"))
							(*vkSkinDesc[i].first)->m_iVertBorderTexID = id;
						else
						if(IsButtonChecked("TextureBdC"))
							(*vkSkinDesc[i].first)->m_iBorderCornerTexID = id;			
						else
						if(IsButtonChecked("TextureAlphaBK"))
							(*vkSkinDesc[i].first)->m_iBkTexAlphaID = id;
						else
						if(IsButtonChecked("TextureAlphaBdH"))
							(*vkSkinDesc[i].first)->m_iHorzBorderTexAlphaID = id;
						else
						if(IsButtonChecked("TextureAlphaBdV"))
							(*vkSkinDesc[i].first)->m_iVertBorderTexAlphaID = id;
						else
						if(IsButtonChecked("TextureAlphaBdC"))
							(*vkSkinDesc[i].first)->m_iBorderCornerTexAlphaID = id;

						bFound = true;

						break;
					}

				if(bFound == false)
				{
					GuiType eWndType = GetType(m_pkFocusWnd);

					// Byt ut textur på noderna i en trädbox
					if(eWndType == Treebox)
					{
						if(strcmp(szSkinType, "child node")==0)
						{
							ZGuiSkin* pkNewSkin = new ZGuiSkin();
							pkNewSkin->m_iBkTexID = pkTexMan->Load(strFullpath.c_str(), 0);
							((ZGuiTreebox*) m_pkFocusWnd)->InsertBranchSkin(0,pkNewSkin,true);
						}
						else
						if(strcmp(szSkinType, "closed node")==0)
						{
							ZGuiSkin* pkNewSkin = new ZGuiSkin();
							pkNewSkin->m_iBkTexID = pkTexMan->Load(strFullpath.c_str(), 0);
							((ZGuiTreebox*) m_pkFocusWnd)->InsertBranchSkin(1,pkNewSkin,true);
						}
						else
						if(strcmp(szSkinType, "open node")==0)
						{
							ZGuiSkin* pkNewSkin = new ZGuiSkin();
							pkNewSkin->m_iBkTexID = pkTexMan->Load(strFullpath.c_str(), 0);
							((ZGuiTreebox*) m_pkFocusWnd)->InsertBranchSkin(2,pkNewSkin,true);
						}
					}
					
						printf("olle\n");
					

				}
			}
		}
	}
	// Byt text på filnamnet i textboxen
	else
	if(strcmp(szTreeBox, "FileTree") == 0)
	{
		string strFullpath = string("../data/");

		if(szParentNodeText)
			strFullpath += string(szParentNodeText);

		if(szClickNodeText)
			strFullpath += string(szClickNodeText);		

		SetText("SelectedFileEB", (char*) strFullpath.c_str());

		m_bOverwriteWarning = true;
	}
}

void ZGResEdit::OnSelectWnd(ZGuiWnd *pkWnd)
{
	unsigned int i=0;

	UpdateSkinList(pkWnd);
	
	const char* szAlias = m_pkScene->GetAlias(m_pkFocusWnd);

	if(szAlias == NULL)
		GetWnd("WndNameTextbox")->SetText( (char*) m_pkFocusWnd->GetName() );
	else
		GetWnd("WndNameTextbox")->SetText( (char*) szAlias );

	GetWnd("WndTitleTextbox")->SetText( (char*) m_pkFocusWnd->GetText() );

	if(m_pkFocusWnd->GetParent())
		GetWnd("ParentLabel")->SetText( (char*) m_pkFocusWnd->GetParent()->GetName() );
	else
		GetWnd("ParentLabel")->SetText( "-" );

	if(m_pkInput->Pressed(KEY_LCTRL))
	{
		int x = m_pkFocusWnd->GetScreenRect().Left;
		int y = m_pkFocusWnd->GetScreenRect().Top;

		ZGuiWnd* pkCopy = m_pkScene->CloneWnd(m_pkFocusWnd, x, y); 

		m_pkFocusWnd = pkCopy;
		m_pkMoveWnd = m_pkFocusWnd;
	}
}

void ZGResEdit::OnClickListbox(int iListBoxID, int iListboxIndex, ZGuiWnd* pkMain)
{
	if(pkMain == NULL)
		return;

	string strMainWndName, strListBox;

	ZGuiWnd* pkListBox = NULL;

	list<ZGuiWnd*> kChilds;
	pkMain->GetChildrens(kChilds); 

	list<ZGuiWnd*>::iterator it = kChilds.begin();
	for( ; it != kChilds.end(); it++)
	{
		if((*it)->GetID() == (unsigned int) iListBoxID)
		{
			pkListBox = (*it);
			break;
		}	 
	}

	strMainWndName = pkMain->GetName();

	if(pkListBox != NULL)
		strListBox = pkListBox->GetName();

	if(strMainWndName == "ViewWindow")
	{
		if(strListBox == "MainWndList")
		{
			char *szItem = static_cast<ZGuiListbox*>(pkListBox)->GetSelItem()->GetText();

			if(szItem)
			{
				ZGuiWnd* pkWnd = m_pkScene->GetWnd(szItem);

				if(pkWnd->IsVisible())
				{
					((ZGuiCheckbox*)GetWnd("ShowHideWndCB"))->CheckButton();

					ZGuiWnd* pkNewActiveWnd = m_pkScene->GetWnd(szItem);
					m_pkFocusWnd = pkNewActiveWnd;
					m_pkMainWnd = pkNewActiveWnd;
				}
				else
					((ZGuiCheckbox*)GetWnd("ShowHideWndCB"))->UncheckButton();
			}
		}
	}
}

void ZGResEdit::AddStandardElements(ZGuiWnd *pkWnd)
{
	GuiType eWndType = GetType(pkWnd);

	char* szName = (char*) pkWnd->GetName();

	//
	// Lägg till element till listboxar och trädboxar
	//
	if(eWndType == Treebox)
	{
		((ZGuiTreebox*) m_pkScene->GetWnd(szName))->Clear();

		AddTreeItem(szName, "ParentNode", "RootNode", "Parent", 1, 2);

		char szNodeName[50], szNodeID[50];

		for(int i=0; i<25; i++)
		{	
			sprintf(szNodeID, "ChildNode%i", i);
			sprintf(szNodeName, "This is Child number %i", i);
			AddTreeItem(szName, szNodeID, "ParentNode", szNodeName, 0, 1);
		}
	}
	else
	if(eWndType == Listbox || eWndType == Combobox)
	{
		ClearListbox(szName);

		char szText[50];

		for(int i=0; i<20; i++)
		{
			sprintf(szText, "Item%i", i);
			AddListItem(szName, szText);
		}
	}
}

void ZGResEdit::OpenDefPropWnd(string strWndType)
{
	// Ta bort alla gamla fönster
	list<ZGuiWnd*> kList;
	m_pkScene->m_pkDefProp->GetChildrens(kList);
	list<ZGuiWnd*>::iterator it = kList.begin();
	for( ; it != kList.end(); it++)
	{
		if(strcmp((*it)->GetName(),"DefPropWndCancelBn") == 0 ||
			strcmp((*it)->GetName(),"DefPropWndOKBn") == 0)
			continue;

		pkGui->UnregisterWindow((*it));
	}

	if(strWndType == "Radiobn.group" || strWndType == "Radiobutton")
	{
		CreateWnd(Label, "NewRadiogroupLabel", "DefPropWnd", "New radiogroup name:", 10, 20, 150, 22, 0);
		CreateWnd(Textbox, "NewRadiogroupNameEB", "DefPropWnd", "", 10, 40, 150, 22, 0);
	}

	OpenWnd(m_pkScene->m_pkDefProp, true);
}

void ZGResEdit::OnReleaseButton(int mx, int my)
{
	m_pkMoveWnd = NULL;
	m_pkResizeWnd = NULL;
	
	m_kSelStart = Point(-1,-1);
	m_kClickPos = Point(-1,-1);
}

// kLine = En linje som skall klippas mot alla rektanglar
// rects = Alla rektanglar som linjen skall klippas mot
// out = vektor med linjer som blir resultatet efter klippning

void ZGResEdit::ClipLine(Line kLine, const vector<Rect> rects, vector<Line>& out)
{
	bool bHorz = kLine.s.y == kLine.e.y;

	vector<Line> kLines;
	kLines.push_back(kLine); 

	if(bHorz)
	{
		for(unsigned int i=0; i<rects.size(); i++)
		{
			Rect rc = rects[i];

			if(kLine.s.y >= rc.Top && kLine.s.y <= rc.Bottom)
				if(!rc.Inside(kLine.s.x, kLine.s.y) && !rc.Inside(kLine.e.x, kLine.e.y))
					if(kLine.s.x < rc.Left && kLine.e.x > rc.Right || kLine.e.x < rc.Left && kLine.s.x > rc.Right)
					{
						Line l1 = Line(kLine.s, Point(rc.Left, kLine.s.y));
						Line l2 = Line(kLine.e, Point(rc.Right, kLine.s.y));

						kLines.pop_back();
						kLines.push_back(l1);
						kLines.push_back(l2);
					}
		}
	}
	else
	{
		for(unsigned int i=0; i<rects.size(); i++)
		{
			Rect rc = rects[i];

			if(kLine.s.x >= rc.Left && kLine.s.x <= rc.Right)
				if(!rc.Inside(kLine.s.x, kLine.s.y) && !rc.Inside(kLine.e.x, kLine.e.y))
					if(kLine.s.y < rc.Top && kLine.e.y > rc.Bottom || 
						kLine.e.y < rc.Top && kLine.s.y > rc.Bottom)
					{
						Line l1 = Line(kLine.s, Point(kLine.s.x, rc.Top));
						Line l2 = Line(kLine.e, Point(kLine.s.x, rc.Bottom));

						if(!kLines.empty()) kLines.pop_back();
						kLines.push_back(l1);
						kLines.push_back(l2);
					}
		}
	}

	for(unsigned int j=0; j<kLines.size(); j++)
	{
		Line current = kLines[j];

		for(unsigned int i=0; i<rects.size(); i++)
		{
			Rect rc = rects[i];

			if(rc.Inside(current.s.x, current.s.y) && rc.Inside(current.e.x, current.e.y))
				return;

			if(bHorz)
			{
				if(rc.Inside(current.s.x, current.s.y))
				{
					if(rc.Left < current.s.x && current.e.x < rc.Right) current.s.x = rc.Left;
					if(rc.Right > current.s.x && current.e.x > rc.Left) current.s.x = rc.Right;
				}

				if(rc.Inside(current.e.x, current.e.y))
				{
					if(rc.Left < current.e.x && current.s.x < rc.Right) current.e.x = rc.Left;
					if(rc.Right > current.e.x && current.s.x > rc.Left) current.e.x = rc.Right;	
				}
			}
			else
			{
				if(rc.Inside(current.s.x, current.s.y))
				{
					if(rc.Top < current.s.y && current.e.y < rc.Bottom) current.s.y = rc.Top;
					if(rc.Bottom > current.s.y && current.e.y > rc.Top) current.s.y = rc.Bottom;
				}

				if(rc.Inside(current.e.x, current.e.y))
				{
					if(rc.Top < current.e.y && current.s.y < rc.Bottom) current.e.y = rc.Top;
					if(rc.Bottom > current.e.y && current.s.y > rc.Top) current.e.y = rc.Bottom;	
				}
			}
		}

		kLines[j] = current;
	}

	for(unsigned int i=0; i<kLines.size(); i++)
		out.push_back( kLines[i] );
}

void ZGResEdit::EnableWnds(bool bEnable)
{
	map<string, ZGuiWnd*> kWindows;
	m_pkGuiMan->GetWindows(kWindows);
	map<string, ZGuiWnd*>::iterator it2;
	for( it2 = kWindows.begin(); it2 != kWindows.end(); it2++)
	{
		if(!m_pkScene->IsSceneWnd(it2->second))
		{
			if(bEnable)
				it2->second->Enable();
			else
				it2->second->Disable();
		}
	}
}

void ZGResEdit::MoveWnd(int x, int y)
{
	if(m_pkMoveWnd != NULL && m_pkInput->Pressed(MOUSELEFT) && 
		m_kClickPos != Point(-1,-1))
	{
		Rect rc = m_pkMoveWnd->GetScreenRect();

		int screenpos_x = m_iXPosBeforeMove + (x - m_kClickPos.x);
		int screenpos_y = m_iYPosBeforeMove + (y - m_kClickPos.y);

		if(screenpos_x > 800 - rc.Width()) screenpos_x = 800 - rc.Width();
		if(screenpos_x < 0) screenpos_x = 0;
		if(screenpos_y > 600 - rc.Height()) screenpos_y = 600 - rc.Height();
		if(screenpos_y < 0) screenpos_y = 0;

		if(m_pkMoveWnd->GetParent() != NULL)
		{
			Rect rcParent = m_pkMoveWnd->GetParent()->GetScreenRect();
			screenpos_x -= rcParent.Left;
			screenpos_y -= rcParent.Top;
		}

		SetPos(m_pkMoveWnd, screenpos_x, screenpos_y);

		if(m_pkMoveWnd->GetParent() != NULL)
			rc = m_pkMoveWnd->GetWndRect();
		else
			rc = m_pkMoveWnd->GetScreenRect();

		SetTextInt("PosXTextbox", rc.Left);
		SetTextInt("PosYTextbox", rc.Top);
		SetTextInt("WidthTextbox", rc.Width());
		SetTextInt("HeightTextbox", rc.Height());
	}
}

void ZGResEdit::ResizeWnd(int x, int y)
{
	if(m_pkResizeWnd)
	{
		Rect rc = m_pkResizeWnd->GetScreenRect();
		int w = rc.Width(), h = rc.Height();
		int screen_x = x, screen_y = y;

		switch(m_eCurrentResizeType)
		{
		case RightSide:
			screen_x = rc.Left; screen_y = rc.Top;
			w = x - rc.Left;	if(w < 2) w = 2;
			Resize(m_pkResizeWnd, w,h);
			break;

		case BottomSide:
			screen_x = rc.Left; screen_y = rc.Top;
			h = y - rc.Top;	if(h < 2) h = 2;
			Resize(m_pkResizeWnd, w,h);
			break;

		case LeftSide:
			screen_y = rc.Top;
			w = rc.Right - x;	if(w < 2) w = 2; h = rc.Height();
			Resize(m_pkResizeWnd, w,h);
			if(screen_x > rc.Right-2) screen_x = rc.Right-2;
			
			if(m_pkResizeWnd->GetParent() != NULL)
			{
				Rect rcParent = m_pkResizeWnd->GetParent()->GetScreenRect();
				screen_x -= rcParent.Left;
				rc.Top -= rcParent.Top;
			}

			SetPos(m_pkResizeWnd, screen_x, rc.Top);
			break;

		case TopSide:
			screen_x = rc.Left;
			w = rc.Width(); h = rc.Bottom - y;	if(h < 2) h = 2;
			Resize(m_pkResizeWnd, w,h);
			if(screen_y > rc.Bottom-2) screen_y = rc.Bottom-2;

			if(m_pkResizeWnd->GetParent() != NULL)
			{
				Rect rcParent = m_pkResizeWnd->GetParent()->GetScreenRect();
				screen_y -= rcParent.Top;
				rc.Left -= rcParent.Left;
			}

			SetPos(m_pkResizeWnd, rc.Left,screen_y);
			break;
		}

		SetTextInt("PosXTextbox", screen_x);
		SetTextInt("PosYTextbox", screen_y);
		SetTextInt("WidthTextbox", w);
		SetTextInt("HeightTextbox", h);
	}
}

void ZGResEdit::DrawSelectionRect(ZGuiWnd *pkWnd)
{
	int bdz = 0;

	if(pkWnd->GetSkin())
		bdz = pkWnd->GetSkin()->m_unBorderSize; 
	
	bdz++;

	Rect rc = pkWnd->GetScreenRect();

	int min_x = rc.Left-bdz, max_x = rc.Right+bdz;
	int min_y = rc.Top-bdz,  max_y = rc.Bottom+bdz;

	Line side[] = 
	{
		Line(Point(min_x, max_y), Point(min_x, min_y)),
		Line(Point(max_x, min_y), Point(max_x, max_y)),
		Line(Point(max_x, min_y), Point(min_x, min_y)),
		Line(Point(min_x, max_y), Point(max_x, max_y)),
	}; 

	int z_value = pkWnd->m_iZValue;

	ZGuiWnd* pkParent = pkWnd->GetParent();

	while(1)
	{
		if(pkParent == NULL)
			break;
		else
			z_value += pkParent->m_iZValue;

		pkParent = pkParent->GetParent();
	}

	vector<Rect> tmp;

	// Hämta rektanglarna på alla fönster som befinner sig ovanför fönstret.
	map<string, ZGuiWnd*> kWindows;
	m_pkGuiMan->GetWindows(kWindows);
	map<string, ZGuiWnd*>::iterator it;
	for( it = kWindows.begin(); it != kWindows.end(); it++)
	{
		ZGuiWnd* pkSearchWnd = (*it).second;
		if(pkSearchWnd != pkWnd && pkSearchWnd->IsVisible() )
		{
			if(pkSearchWnd->m_iZValue > z_value)
				tmp.push_back(pkSearchWnd->GetScreenRect(true));
		}
	}
	
	unsigned int i; 
	vector< Line > out;
	const vector<Rect> rects=tmp;

	for(int s=0; s<4; s++)
	{
		ClipLine(side[s], rects, out);
		for(i=0; i<out.size(); i++)
			pkGui->DrawLine(out[i].s, out[i].e); 

		out.clear();
	}
}

void ZGResEdit::UpdateViewWnd()
{
	ClearListbox("MainWndList");

	map<string, ZGuiWnd*> kWindows;
	m_pkGuiMan->GetWindows(kWindows);

	map<string, ZGuiWnd*>::iterator it;
	for( it = kWindows.begin(); it != kWindows.end(); it++)
	{		
		ZGuiWnd* pkWnd = (*it).second;
		if( GetType(pkWnd) == Wnd && !m_pkScene->IsSceneWnd(pkWnd) )
		{
			const char* szAlias = m_pkScene->GetAlias(pkWnd);
			if(szAlias == NULL)
				szAlias = (*it).first.c_str();

			AddListItem("MainWndList", (char*) szAlias);
		}
	}
}

void ZGResEdit::MoveWndToTop(ZGuiWnd *pkWnd)
{
	pkWnd->Show();
	pkGui->SetFocus(pkWnd);
}

void ZGResEdit::ExecuteCommand()
{
	ZGuiWnd* pkTopWnd = pkGui->GetActiveMainWnd();
	
	if(pkTopWnd == m_pkScene->m_pkPropertyWnd)
	{
		UpdatePropertyData();
	}
}

void ZGResEdit::UpdatePropertyData()
{
	if(m_pkFocusWnd)
	{
		bool bSuccess;

		int x = GetTextInt("PosXTextbox",&bSuccess);
		int y = GetTextInt("PosYTextbox",&bSuccess);
		int bd_size = GetTextInt("BdSizeTextbox",&bSuccess);

		SetPos(m_pkFocusWnd, x, y);

		Resize(m_pkFocusWnd, GetTextInt("WidthTextbox",&bSuccess), 
			GetTextInt("HeightTextbox",&bSuccess));

		// obs! anropet till settext måste ske efter anropet till resize eftersom
		// settext kan ändra bredden på fönstret igen.
		bool bResize = GetType(m_pkFocusWnd) == Radiobutton;
		m_pkFocusWnd->SetText( GetText("WndTitleTextbox"), bResize );

		// Försök byta namn på fönstret
		const char* szNewName = GetText("WndNameTextbox");
		if( strcmp( m_pkFocusWnd->GetName(), szNewName ) != 0) // om fönstrets namn har ändrats
		{
			if(m_pkScene->RenameWnd(m_pkFocusWnd, szNewName) == false)
			{
				const char* old_alias = m_pkScene->GetAlias(m_pkFocusWnd);
				if(old_alias)
				{
					SetText( "WndNameTextbox", (char*) old_alias );
				}
				else
				{
					SetText( "WndNameTextbox", (char*) m_pkFocusWnd->GetName() );
				}
			}

			UpdateViewWnd();
		}

		// change borde size on selected control type.
		char* szSkinType = GetSelItem("SkinTypeCB");
		vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
		m_pkFocusWnd->GetWndSkinsDesc(vkSkinDesc);
		for(unsigned int i=0; i<vkSkinDesc.size(); i++)
			if(strcmp(szSkinType, vkSkinDesc[i].second.c_str()) == 0)
			{
				(*vkSkinDesc[i].first)->m_unBorderSize = bd_size; 
				break;
			}

		if(bSuccess == false)
		{
			printf("failed to get text from a editbox!\n");
		}
	}
}

void ZGResEdit::UpdateSkinList(ZGuiWnd *pkFocusWnd)
{
	unsigned int i=0;

	// Fyll listan med olika skin typer...
	ZGuiCombobox* pkCBox = (ZGuiCombobox*)GetWnd("SkinTypeCB");
	
	char* szCurrSel = GetSelItem("SkinTypeCB");

	pkCBox->RemoveAllItems();
	vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
	pkFocusWnd->GetWndSkinsDesc(vkSkinDesc);
	for(i=0; i<vkSkinDesc.size(); i++)
	{
		pkCBox->AddItem((char*)vkSkinDesc[i].second.c_str(), i, (i==0)?true:false);

		if(i==0)
		{
			ZGuiSkin* pkSkin = *vkSkinDesc[i].first;

			if(pkSkin)
			{
				//
				// Set skin parameter settings
				//

				if(!pkSkin->m_bTileBkSkin)
					((ZGuiCheckbox*)GetWnd("StretchTextureCB"))->CheckButton();
				else
					((ZGuiCheckbox*)GetWnd("StretchTextureCB"))->UncheckButton();

				if(pkSkin->m_bTransparent)
					((ZGuiCheckbox*)GetWnd("TransparentTextureCB"))->CheckButton();
				else
					((ZGuiCheckbox*)GetWnd("TransparentTextureCB"))->UncheckButton();
			}
		}
	}

	if(GetType(pkFocusWnd) == Treebox)
	{
		pkCBox->AddItem("child node", vkSkinDesc.size(),0);	
		pkCBox->AddItem("closed node", vkSkinDesc.size()+1,0);
		pkCBox->AddItem("open node", vkSkinDesc.size()+2,0);
	}

	if(szCurrSel)
		SelListItem("SkinTypeCB", szCurrSel); // markera den som var vald igen...

	// uppdatera textboxen med storlek på aktuell ram
	char* szSkinType = GetSelItem("SkinTypeCB");
	for(i=0; i<vkSkinDesc.size(); i++)
		if(strcmp(szSkinType, vkSkinDesc[i].second.c_str()) == 0)
		{
			SetTextInt("BdSizeTextbox", (*vkSkinDesc[i].first)->m_unBorderSize);
			break;
		}

}

void ZGResEdit::OpenWnd(ZGuiWnd *pkWnd, bool bOpen)
{
	if(bOpen)
	{
		m_pkAudioSys->StartSound("/data/sound/open_door1.wav");
		MoveWndToTop(pkWnd);
	}
	else
	{
		m_pkAudioSys->StartSound("/data/sound/close_door1.wav");
		pkWnd->Hide();
	}
}
