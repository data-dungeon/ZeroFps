#include "zguitest.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"

ZGuiTest g_kGuiTest("GuiTest",0,0,0);

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
	switch(msg)
	{
	case ZGM_COMMAND:
		g_kGuiTest.OnCommand(((int*)params)[0], (((int*)params)[1] == 1) ? true : false, win);
		break;
	case ZGM_LBUTTONDOWN:
		break;
	case ZGM_LBUTTONUP:
		break;
	case ZGM_RBUTTONDOWN:    
		break;
	case ZGM_RBUTTONUP:
		break;
	case ZGM_LBUTTONDBLCLK:
		break;
	case ZGM_RBUTTONDBLCLK:
		break;
	case ZGM_MOUSEMOVE:
		break;
	case ZGM_SCROLL:
		break;
	case ZGM_CBN_SELENDOK:
		g_kGuiTest.OnSelectCB(((int*)params)[0], ((int*)params)[1], win);
		break;
	case ZGM_KEYPRESS:	
		g_kGuiTest.OnKeyDown(((int*)params)[0], win);
		break;

	case ZGM_SELECTTREEITEM:
		char** pszParams; pszParams = (char**) params;
		g_kGuiTest.OnClickTreeItem( pszParams[0], pszParams[1], 
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

	}
	return true;
}

ZGuiTest::ZGuiTest(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	m_pkFocusWnd = m_pkResizeWnd = m_pkMainWnd = NULL;
	m_eEditMode = MOVE;
	m_iXPos = 0;  m_iYPos = 0; m_iWidth = 100; m_iHeight = 100;
	m_kSelStart = Point(-1,-1);

}

void ZGuiTest::OnInit()
{
	glEnable(GL_LIGHTING );

	pkLight->SetLighting(true);

	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.25,100);	

	// create gui script
	GuiAppLua::Init(&g_kGuiTest, pkScript);

	InitializeGui(pkGui, pkTexMan, pkScript, pkGuiMan, 
		"data/textures/text/small.bmp",
		"data/script/gui/gui_res_edit.lua");

	SDL_ShowCursor(SDL_DISABLE);

	pkGui->SetCursor(0,0, pkTexMan->Load("data/textures/gui/cursor.bmp", 0),
		pkTexMan->Load("data/textures/gui/cursor_a.bmp", 0), 32, 32);

	CreateUI();

	pkFps->m_bClientMode = true;
}

void ZGuiTest::OnIdle()
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	
	pkFps->UpdateCamera(); 	

	int iKey = pkInput->GetQueuedKey();

	if(iKey == KEY_F1)
	{
		if(m_pkToolbar->IsVisible())
			m_pkToolbar->Hide();
		else
			m_pkToolbar->Show();

		if(m_pkEditbar->IsVisible())
			m_pkEditbar->Hide();
		else
			m_pkEditbar->Show();
	}

	if(iKey == KEY_DELETE)
		DeleteWnd(m_pkFocusWnd);

	int x,y;
	pkInput->MouseXY(x,y);

	static s_bLeftButtonPressed = false;

	if(pkInput->Pressed(MOUSELEFT))
	{
		if(!s_bLeftButtonPressed)
		{
			OnMouseClick(false, x, y);
			s_bLeftButtonPressed = false;
		}

		s_bLeftButtonPressed = true;
	}
	else
	{
		if(s_bLeftButtonPressed)
		{
			OnMouseClick(true, x, y);
			s_bLeftButtonPressed = false;
		}
	}

	if(!m_vkMoveWnds.empty() && m_eEditMode == MOVE && 
		pkInput->Pressed(MOUSELEFT) && m_kClickPos != Point(-1,-1))
	{
		for(unsigned int i=0; i<m_vkMoveWnds.size(); i++)
		{

			Rect rc = m_vkMoveWnds[i].pkWnd->GetScreenRect();

			int screenpos_x = m_vkMoveWnds[i].m_iXPosBeforeMove + (x - m_kClickPos.x);
			int screenpos_y = m_vkMoveWnds[i].m_iYPosBeforeMove + (y - m_kClickPos.y);

			if(screenpos_x > 800 - rc.Width())
				screenpos_x = 800 - rc.Width();

			if(screenpos_x < 0)
				screenpos_x = 0;

			if(screenpos_y > 600 - rc.Height())
				screenpos_y = 600 - rc.Height();

			if(screenpos_y < 0)
				screenpos_y = 0;

			if(m_vkMoveWnds[i].pkWnd->GetParent() != NULL)
			{
				Rect rcParent = m_vkMoveWnds[i].pkWnd->GetParent()->GetScreenRect();
				screenpos_x -= rcParent.Left;
				screenpos_y -= rcParent.Top;
			}

			SetPos(m_vkMoveWnds[i].pkWnd, screenpos_x, screenpos_y);

			if(m_vkMoveWnds[i].pkWnd->GetParent() != NULL)
				rc = m_vkMoveWnds[i].pkWnd->GetWndRect();
			else
				rc = m_vkMoveWnds[i].pkWnd->GetScreenRect();

			SetTextInt("PosXTextbox", rc.Left);
			SetTextInt("PosYTextbox", rc.Top);
			SetTextInt("WidthTextbox", rc.Width());
			SetTextInt("HeightTextbox", rc.Height());
		}
	}

	if(m_pkResizeWnd && m_eEditMode == RESIZE )
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

	if(m_vkMoveWnds.empty())
	{
		if(m_kSelStart != Point(-1,-1))
		{
			Rect rc = Rect(m_kSelStart.x, m_kSelStart.y, x, y);

			pkGui->DrawLine(Point(rc.Left, rc.Top), Point(rc.Right, rc.Top));
			pkGui->DrawLine(Point(rc.Left, rc.Bottom), Point(rc.Right, rc.Bottom));
			pkGui->DrawLine(Point(rc.Left, rc.Top), Point(rc.Left, rc.Bottom));
			pkGui->DrawLine(Point(rc.Right, rc.Top), Point(rc.Right, rc.Bottom));
		}
	}

	for(unsigned int i=0; i<m_vkMoveWnds.size(); i++)
	{
		Rect rc = m_vkMoveWnds[i].pkWnd->GetScreenRect();

		pkGui->DrawLine(Point(rc.Left, rc.Top), Point(rc.Right, rc.Top));
		pkGui->DrawLine(Point(rc.Left, rc.Bottom), Point(rc.Right, rc.Bottom));
		pkGui->DrawLine(Point(rc.Left, rc.Top), Point(rc.Left, rc.Bottom));
		pkGui->DrawLine(Point(rc.Right, rc.Top), Point(rc.Right, rc.Bottom));
	}

	if(m_pkFocusWnd)
	{
		Rect rc = m_pkFocusWnd->GetScreenRect();

		pkGui->DrawLine(Point(rc.Left, rc.Top), Point(rc.Right, rc.Top));
		pkGui->DrawLine(Point(rc.Left, rc.Bottom), Point(rc.Right, rc.Bottom));
		pkGui->DrawLine(Point(rc.Left, rc.Top), Point(rc.Left, rc.Bottom));
		pkGui->DrawLine(Point(rc.Right, rc.Top), Point(rc.Right, rc.Bottom));
	}
	
}

void ZGuiTest::OnHud(void) 
{	
	pkFps->DevPrintf("common","Active Propertys: %d",pkObjectMan->GetActivePropertys());	
	pkFps->DevPrintf("common", "Fps: %f",pkFps->m_fFps);	
	pkFps->DevPrintf("common","Avrage Fps: %f",pkFps->m_fAvrageFps);			
	
	pkFps->m_bGuiMode = false;
	pkFps->ToggleGui();
}

void ZGuiTest::CreateUI()
{
	//
	// Create edit bar
	//

	CreateWnd(Wnd, "Editbar", "", "", 800-204, 8, 200, 500, 0);
	(m_pkEditbar = GetWnd("Editbar"))->SetMoveArea(Rect(0,0,800,600),true);

	CreateWnd(Checkbox, "ToogleEditMode", "Editbar", "", 0, 0, 24, 22, 0);
	((ZGuiCheckbox*)GetWnd("ToogleEditMode"))->GetUncheckedSkin()->
		m_iBkTexID = pkTexMan->Load("data/textures/gui/move.bmp", 0);
	((ZGuiCheckbox*)GetWnd("ToogleEditMode"))->GetCheckedSkin()->
		m_iBkTexID = pkTexMan->Load("data/textures/gui/scale.bmp", 0);

	CreateWnd(Button, "DeleteWnd", "Editbar", "", 28, 0, 24, 22, 0);
	((ZGuiButton*)GetWnd("DeleteWnd"))->GetButtonUpSkin()->
		m_iBkTexID = pkTexMan->Load("data/textures/gui/delete_u.bmp", 0);
	((ZGuiButton*)GetWnd("DeleteWnd"))->GetButtonHighLightSkin()->
		m_iBkTexID = pkTexMan->Load("data/textures/gui/delete_u.bmp", 0);
	((ZGuiButton*)GetWnd("DeleteWnd"))->GetButtonDownSkin()->
		m_iBkTexID = pkTexMan->Load("data/textures/gui/delete_d.bmp", 0);

	CreateWnd(Label, "NewWindowLabel",  "Editbar",  "Create new window", 4,  28, 120, 20, 0);

	CreateWnd(Combobox, "SelectWndType", "Editbar", "", 4, 48, 100, 16, 0);
	AddListItem("SelectWndType", "Wnd");
	AddListItem("SelectWndType", "Button");
	AddListItem("SelectWndType", "Checkbox");
	AddListItem("SelectWndType", "Combobox");
	AddListItem("SelectWndType", "Label");
	AddListItem("SelectWndType", "Listbox");
	AddListItem("SelectWndType", "Radiobutton");
	AddListItem("SelectWndType", "Scrollbar");
	AddListItem("SelectWndType", "Slider");
	AddListItem("SelectWndType", "TabControl");
	AddListItem("SelectWndType", "Textbox");
	AddListItem("SelectWndType", "Treebox");

	((ZGuiCombobox*)GetWnd("SelectWndType"))->SetLabelText("Wnd"); 
	((ZGuiCombobox*)GetWnd("SelectWndType"))->GetListbox()->SelItem(0); 

	CreateWnd(Label, "NewNameLabel",  "Editbar",  "Name:", 4,  72, 25, 20, 0);
	CreateWnd(Textbox, "NewWndNameTextbox", "Editbar", "Nonmame", 40,  72, 150, 20, 0);

	CreateWnd(Button, "CreateWndBn", "Editbar", "OK", 108, 48, 80, 20, 0);

	((ZGuiButton*)GetWnd("CreateWndBn"))->SetButtonUpSkin(new ZGuiSkin());
	((ZGuiButton*)GetWnd("CreateWndBn"))->SetButtonHighLightSkin(new ZGuiSkin());
	((ZGuiButton*)GetWnd("CreateWndBn"))->SetButtonDownSkin(new ZGuiSkin());

	((ZGuiButton*)GetWnd("CreateWndBn"))->GetButtonUpSkin()->
		m_iBkTexID = pkTexMan->Load("data/textures/gui/bn_u.bmp", 0);
	((ZGuiButton*)GetWnd("CreateWndBn"))->GetButtonHighLightSkin()->
		m_iBkTexID = pkTexMan->Load("data/textures/gui/bn_f.bmp", 0);
	((ZGuiButton*)GetWnd("CreateWndBn"))->GetButtonDownSkin()->
		m_iBkTexID = pkTexMan->Load("data/textures/gui/bn_d.bmp", 0);

	CreateWnd(Treebox, "TextureTree", "Editbar", "", 8,105,200-16,250,0);

	BuildFileTree("TextureTree", "data/textures/gui");

	CreateWnd(Radiobutton, "TextureBK", "Editbar", "Background", 2, 367, 100, 16, 0);
	CreateWnd(Radiobutton, "TextureBdH", "Editbar", "Border, horz", 2, 385, 100, 16, 0);
	CreateWnd(Radiobutton, "TextureBdV", "Editbar", "Border, vert", 2, 403, 100, 16, 0);
	CreateWnd(Radiobutton, "TextureBdC", "Editbar", "Border, corner", 2, 421, 100, 16, 0);

	CreateWnd(Radiobutton, "TextureAlphaBK", "Editbar", "(A)", 122, 367, 100, 16, 0);
	CreateWnd(Radiobutton, "TextureAlphaBdH", "Editbar", "(A)", 122, 385, 100, 16, 0);
	CreateWnd(Radiobutton, "TextureAlphaBdV", "Editbar", "(A)", 122, 403, 100, 16, 0);
	CreateWnd(Radiobutton, "TextureAlphaBdC", "Editbar", "(A)", 122, 421, 100, 16, 0);

	CreateWnd(Label, "SkinTypeCBLabel",  "Editbar",  "Skin type", 2,  440, 190, 19, 0);
	CreateWnd(Combobox, "SkinTypeCB", "Editbar", "Skin type", 2, 460, 190, 20, 0);

	//
	// Create toolbar
	//
	CreateWnd(Wnd, "Toolbar", "", "", 800/2-300/2, 600-138, 300, 110, 0);
	(m_pkToolbar = GetWnd("Toolbar"))->SetMoveArea(Rect(0,0,800,600),true);

	CreateWnd(Label, "NameLabel",  "Toolbar",  "Name:", 4,  4+2, 25, 20, 0);
	CreateWnd(Textbox, "WndNameTextbox", "Toolbar", "Nonmame", 40,  4, 250, 20, READ_ONLY);

	CreateWnd(Label, "TextLabel",  "Toolbar",  "Title:", 4,  28+2, 25, 20, 0);
	CreateWnd(Textbox, "WndTitleTextbox", "Toolbar", "Nonmame", 40,  28, 250, 20, 0);

	CreateWnd(Button, "UpdateWndDataBn", "Toolbar", "OK", 300-44, 110-24, 40, 20, 0);

	((ZGuiButton*)GetWnd("UpdateWndDataBn"))->SetButtonUpSkin(new ZGuiSkin());
	((ZGuiButton*)GetWnd("UpdateWndDataBn"))->SetButtonHighLightSkin(new ZGuiSkin());
	((ZGuiButton*)GetWnd("UpdateWndDataBn"))->SetButtonDownSkin(new ZGuiSkin());

	((ZGuiButton*)GetWnd("UpdateWndDataBn"))->GetButtonUpSkin()->
		m_iBkTexID = pkTexMan->Load("data/textures/gui/bn_u.bmp", 0);
	((ZGuiButton*)GetWnd("UpdateWndDataBn"))->GetButtonHighLightSkin()->
		m_iBkTexID = pkTexMan->Load("data/textures/gui/bn_f.bmp", 0);
	((ZGuiButton*)GetWnd("UpdateWndDataBn"))->GetButtonDownSkin()->
		m_iBkTexID = pkTexMan->Load("data/textures/gui/bn_d.bmp", 0);

	CreateWnd(Label, "PosXLabel",   "Toolbar", "X", 4,  54+2, 20, 20, 0);
	CreateWnd(Textbox, "PosXTextbox",   "Toolbar", "0", 20,  54, 40, 20, 0);

	CreateWnd(Label, "PosYLabel",   "Toolbar", "Y", 65, 54+2, 20, 20, 0);
	CreateWnd(Textbox, "PosYTextbox",   "Toolbar", "0", 85, 54, 40, 20, 0);
	
	CreateWnd(Label, "WidthLabel",  "Toolbar",  "W", 130,  54+2, 20, 20, 0);
	CreateWnd(Textbox, "WidthTextbox",  "Toolbar", "100", 150,  54, 40, 20, 0);

	CreateWnd(Label, "HeightLabel", "Toolbar",  "H", 195, 54+2, 20, 20, 0);
	CreateWnd(Textbox, "HeightTextbox", "Toolbar", "100", 215, 54, 40, 20, 0);

	CreateWnd(Label, "ParentLabel2", "Toolbar",  "Parent:", 4, 80+2, 50, 20, 0);
	CreateWnd(Label, "ParentLabel", "Toolbar",  "None", 54, 80+2, 150, 20, 0);
}

void ZGuiTest::OnKeyDown(int iKey, ZGuiWnd* pkWnd)
{

}

void ZGuiTest::OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd *pkMainWnd)
{
	ZGuiWnd* pkWndClicked = GetWnd(iID);

	if(pkWndClicked == NULL)
		return; 

	string strMainWndName = pkMainWnd->GetName();
	string strClickWndName = pkWndClicked->GetName();

	if(strMainWndName == "Toolbar")
	{
		if(strClickWndName == "UpdateWndDataBn")
		{
			if(m_pkFocusWnd)
			{
				bool bSuccess;

				m_pkFocusWnd->SetText( GetText("WndTitleTextbox") );

				int x = GetTextInt("PosXTextbox",&bSuccess);
				int y = GetTextInt("PosYTextbox",&bSuccess);

				SetPos(m_pkFocusWnd, x, y);

				Resize(m_pkFocusWnd, GetTextInt("WidthTextbox",&bSuccess), 
					GetTextInt("HeightTextbox",&bSuccess));

				if(bSuccess == false)
				{
					printf("failed to get text from a editbox!\n");
				}
			}
		}
	}
	else
	if(strMainWndName == "Editbar")
	{
		if(strClickWndName == "ToogleEditMode")
		{
			if(((ZGuiCheckbox*) pkWndClicked)->IsChecked())
				m_eEditMode = RESIZE;
			else
				m_eEditMode = MOVE;
		}
		else
		if(strClickWndName == "DeleteWnd")
		{
			DeleteWnd(m_pkFocusWnd);
		}
		else
		if(strClickWndName == "CreateWndBn")
		{
			char* szName = GetText("NewWndNameTextbox");
			char szParent[100];
			char szlabel[100];

			if(m_pkMainWnd)
				strcpy(szParent, m_pkMainWnd->GetName());
			else
				strcpy(szParent, "");

			string strWndType = ((ZGuiCombobox*) GetWnd("SelectWndType"))->
				GetListbox()->GetSelItem()->GetText();   

			GuiType eWndType = Wnd;
			if(strWndType == "Button") eWndType = Button; else
			if(strWndType == "Checkbox") eWndType = Checkbox; else
			if(strWndType == "Combobox") eWndType = Combobox; else
			if(strWndType == "Label") eWndType = Label; else
			if(strWndType == "Listbox") eWndType = Listbox; else
			if(strWndType == "Radiobutton") eWndType = Radiobutton; else
			if(strWndType == "Scrollbar") eWndType = Scrollbar; else
			if(strWndType == "Slider") eWndType = Slider; else
			if(strWndType == "TabControl") eWndType = TabControl; else
			if(strWndType == "Textbox") eWndType = Textbox; else
			if(strWndType == "Treebox") eWndType = Treebox;

			if(eWndType != Wnd && m_pkMainWnd == NULL)
			{
				printf("Failed to create control! No parent selected\n");
				return;
			}

			if(strWndType == "Label")
				strcpy(szlabel, szName);
			else
				strcpy(szlabel, "");

			if(eWndType == Wnd)
				strcpy(szParent,"");
	
			CreateWnd( eWndType, szName,  szParent, szlabel, m_iXPos,  m_iYPos, m_iWidth, m_iHeight, 0);

			if(eWndType == Wnd)
			{
				m_pkMainWnd = GetWnd(szName);
			}

			// Skapa nya texturer och kopiera de gamla mot nya unika...
			ZGuiWnd* pkWnd = GetWnd(szName);
			vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
			pkWnd->GetWndSkinsDesc(vkSkinDesc);
			for(unsigned int i=0; i<vkSkinDesc.size(); i++)
			{
				ZGuiSkin* pkPrevSkin = *vkSkinDesc[i].first; 
				ZGuiSkin* pkNewSkin = new ZGuiSkin(pkPrevSkin);
				*vkSkinDesc[i].first = pkNewSkin;
			}

/*			SetTextInt("PosXTextbox", m_iXPos, false);
			SetTextInt("PosYTextbox", m_iYPos, false);
			SetTextInt("WidthTextbox", m_iWidth, false);
			SetTextInt("HeightTextbox", m_iHeight, false);		*/
		}
	}
}

void ZGuiTest::OnMouseClick(bool bReleased, int x, int y)
{

	ZGuiWnd* pkWnd = GetWndFromPoint(x,y);

	if(pkWnd)
	{
		if(IsResWnd(pkWnd))
		{
			//m_pkFocusWnd = NULL;
			m_pkResizeWnd = NULL;
	
			m_kSelStart = Point(-1,-1);
			m_kClickPos = Point(-1,-1);

			return;
		}
	}

	if(!bReleased)
	{
		m_kSelStart = Point(x,y);
		m_kClickPos = Point(x,y);

		if(m_vkMoveWnds.empty())
		{
			if(pkWnd && !pkInput->Pressed(KEY_LSHIFT))
			{
				m_vkMoveWnds.clear();

				MOVE_INFO mi;
				mi.pkWnd = pkWnd;

				Rect rc = pkWnd->GetScreenRect();
				mi.m_iXPosBeforeMove = rc.Left; 
				mi.m_iYPosBeforeMove = rc.Top;

				m_vkMoveWnds.push_back(mi);
			}
		}

		ZGuiWnd* pkClickWnd = GetWndFromPoint(x,y);

		if(pkClickWnd)
		{
			if(!IsResWnd(pkClickWnd))
			{
				if(GetType(pkClickWnd) == Wnd)
				{
					m_pkMainWnd = pkClickWnd;
				}

				if(m_pkResizeWnd != NULL)
				{
					m_pkResizeWnd = pkClickWnd;
					return;
				}

				m_pkFocusWnd = pkClickWnd;
				m_pkResizeWnd = pkClickWnd;

				OnSelectWnd(m_pkFocusWnd);
				
				Rect rc = pkClickWnd->GetScreenRect();

				m_iXPosBeforeResize = rc.Left; 
				m_iYPosBeforeResize = rc.Top;

				GetWnd("WndNameTextbox")->SetText( (char*) m_pkFocusWnd->GetName() );
				GetWnd("WndTitleTextbox")->SetText( (char*) m_pkFocusWnd->GetText() );

				if(m_pkFocusWnd->GetParent())
					GetWnd("ParentLabel")->SetText( (char*) m_pkFocusWnd->GetParent()->GetName() );
				else
					GetWnd("ParentLabel")->SetText( "-" );

				int dl = abs(x - rc.Left), dr = abs(x - rc.Right);
				int dt = abs(y - rc.Top), db = abs(y - rc.Bottom);

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
			//m_pkFocusWnd = NULL;
			m_pkResizeWnd = NULL;
		}
	}
	else
	{
		m_vkMoveWnds.clear();

		int min_x = min(m_kSelStart.x, x);
		int max_x = max(m_kSelStart.x, x);
		int min_y = min(m_kSelStart.y, y);
		int max_y = max(m_kSelStart.y, y);

		Rect rcSelection(min_x, min_y, max_x, max_y);
		
		map<string,ZGuiWnd*> kWindows;
		pkGuiMan->GetWindows(kWindows); 
		map<string,ZGuiWnd*>::iterator it = kWindows.begin();
		for( ; it != kWindows.end(); it++)
		{
			ZGuiWnd* pkWnd = (*it).second;

			if(IsResWnd(pkWnd) == false)
			{
				if(rcSelection.Left <= pkWnd->GetScreenRect().Left && 
					rcSelection.Right >= pkWnd->GetScreenRect().Right && 
					rcSelection.Top <= pkWnd->GetScreenRect().Top && 
					rcSelection.Bottom >= pkWnd->GetScreenRect().Bottom ) 
				{
					MOVE_INFO mi;
					mi.pkWnd = pkWnd;

					Rect rc = pkWnd->GetScreenRect();
					mi.m_iXPosBeforeMove = rc.Left; 
					mi.m_iYPosBeforeMove = rc.Top;

					m_vkMoveWnds.push_back(mi);
				}
			}
		}
		
		m_kSelStart = Point(-1,-1);
		m_kClickPos = Point(-1,-1);

		m_pkResizeWnd = NULL;		
	}
}

ZGuiWnd* ZGuiTest::GetWndFromPoint(int x, int y)
{
	ZGuiWnd* pkMainWnd = pkGui->GetMainWindowFromPoint(x,y) ;

	if(pkMainWnd)
	{
		if(!IsResWnd(pkMainWnd))
		{
			list<ZGuiWnd*> kChildList;
			pkMainWnd->GetChildrens(kChildList);

			list<ZGuiWnd*>::iterator it = kChildList.begin();
			for( ; it != kChildList.end(); it++)
			{
				ZGuiWnd* pkChild = (*it);
				Rect rc = pkChild->GetScreenRect();

				if(rc.Inside(x,y))
					return pkChild;
			}
		}

		return pkMainWnd;
	}

	return NULL;
}

bool ZGuiTest::IsResWnd(ZGuiWnd* pkWnd)
{
	if(pkWnd == m_pkToolbar)
		return true;

	if(pkWnd == m_pkEditbar)
		return true;

	ZGuiWnd* pkParent = pkWnd->GetParent();

	if(pkParent == m_pkToolbar || pkParent == m_pkEditbar)
		return true;

	return false;
}

void ZGuiTest::SetPos(ZGuiWnd* pkWnd, int x, int y)
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

/*		if(x < rcParent.Left) x = rcParent.Left;
		if(x + rc.Width() > rcParent.Right) x = rcParent.Right - rc.Width();

		if(y < rcParent.Top) y = rcParent.Top;
		if(y + rc.Height() > rcParent.Bottom) y = rcParent.Bottom - rc.Height();*/

		if(x > rcParent.Width() - rc.Width()) 
			x = rcParent.Width() - rc.Width();
		if(x < 0) x = 0;

		if(y > rcParent.Height() - rc.Height()) 
			y = rcParent.Height() - rc.Height();
		if(y < 0) y = 0;
	}

	pkWnd->SetPos(x, y, bScreenSpace, true); 
}

void ZGuiTest::Resize(ZGuiWnd *pkWnd, int w, int h)
{
	Rect rc = pkWnd->GetScreenRect();

	if(pkWnd->GetParent() == NULL)
	{
		if(w + rc.Left > 800) w = 800-rc.Left;
		if(h + rc.Top  > 600) h = 800-rc.Top;

		list<ZGuiWnd*> childs;
		pkWnd->GetChildrens(childs);

/*		if(childs.size() > 0)
		{
			int biggest_x = 0;
			int biggest_y = 0;

			list<ZGuiWnd*>::iterator it = childs.begin();
			for( ; it != childs.end(); it++)
			{
				Rect rcChild = (*it)->GetScreenRect();

				if(rcChild.Right > biggest_x)
					biggest_x = rcChild.Right;
				if(rcChild.Bottom > biggest_y)
					biggest_y = rcChild.Bottom;
			}

			if(w + rc.Left > biggest_x) w = biggest_x-rc.Left;
			if(h + rc.Top  > biggest_y) h = biggest_y-rc.Top;
		}
*/
	}
	else
	{

	}

	pkWnd->Resize(w, h, true); 
}

void ZGuiTest::OnSelectCB(int ListBoxID, int iItemIndex, ZGuiWnd *pkMain)
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

			if(strWndType == "Wnd")					{ x=0; y=0; w=100; h=100; }
			else if(strWndType == "Button")		{ x=20; y=20; w=40;  h=20; } 
			else if(strWndType == "Checkbox")	{ x=0; y=0; w=20;  h=20; }
			else if(strWndType == "Combobox")	{ x=0; y=0; w=100; h=20; }
			else if(strWndType == "Label")		{ x=0; y=0; w=50;  h=20; }
			else if(strWndType == "Listbox")		{ x=0; y=0; w=100; h=100; }
			else if(strWndType == "Radiobutton"){ x=0; y=0; w=20;  h=20; }
			else if(strWndType == "Scrollbar")	{ x=0; y=0; w=100; h=20; }
			else if(strWndType == "Slider")		{ x=0; y=0; w=100; h=20; }
			else if(strWndType == "TabControl")	{ x=0; y=0; w=100; h=100; }
			else if(strWndType == "Textbox")		{ x=0; y=0; w=100; h=20; }
			else if(strWndType == "Treebox")		{ x=0; y=0; w=100; h=100; }

			m_iXPos = x; m_iYPos = y;
			m_iWidth = w; m_iHeight = h;
		}
	}
}

void ZGuiTest::DeleteWnd(ZGuiWnd *pkWnd)
{
	if(pkWnd != NULL)
	{
		if(m_pkMainWnd == pkWnd)
			m_pkMainWnd = NULL;

		pkGui->UnregisterWindow(pkWnd);
		pkWnd = NULL;
	}
}

bool ZGuiTest::BuildFileTree(char* szTreeBoxName, char* szRootPath)
{
	// kolla inparametrar
	if(szRootPath == NULL || szTreeBoxName == NULL)
		return false;

	// sista tecknet får inte vara ett '/' tecken
	if(szRootPath[strlen(szRootPath)] == '/')
		szRootPath[strlen(szRootPath)] = '\0';

	set<string> kSearchedFiles;
	list<string> dir_list;
	string strPrevNode;

	dir_list.push_back(string(szRootPath));
	strPrevNode = "RootNode";
	
	while(1)
	{
		list<string> vkFileNames;
		string currentFolder = dir_list.back();

		// Hämta filerna i den aktuella katalogen och sortera listan.
		vector<string> t;
		pkZFVFileSystem->ListDir(&t, currentFolder);
		for(unsigned int i=0; i<t.size(); i++)
			vkFileNames.push_back(t[i]); 
		t.clear(); vkFileNames.sort(SortFiles);

		// Lägg till alla filer
		for(list<string>::iterator it = vkFileNames.begin(); it != vkFileNames.end(); it++)  
		{
			string strLabel = (*it);
			string id = currentFolder + string("/") + strLabel;

			bool bIsFolder = strLabel.find(".") == string::npos;

			if(kSearchedFiles.find(id) == kSearchedFiles.end())
			{			
				if(bIsFolder)
				{
					string d = currentFolder + string("/") + strLabel;
					dir_list.push_back(d);

					AddTreeItem(szTreeBoxName, id.c_str(), 
						strPrevNode.c_str(), (char*) strLabel.c_str(), 1, 2);
				}
				else
					AddTreeItem(szTreeBoxName, id.c_str(), 
						strPrevNode.c_str(), (char*) strLabel.c_str(), 0, 1);

				kSearchedFiles.insert(id);
			}
		}

		// ej klivit in i ett nytt dir? gå tillbaks
		if(dir_list.back() == currentFolder)
		{
			// sista?
			if(currentFolder == szRootPath)
				break;

			dir_list.pop_back();
		}

		strPrevNode = dir_list.back();
	}

	return true;

}

void ZGuiTest::OnClickTreeItem(char *szTreeBox, char *szParentNodeText, 
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

			if(IsButtonChecked("TextureBK"))
			{
				char* szSkinType = GetSelItem("SkinTypeCB");

				ZGuiSkin* pkSkin = NULL;

				vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
				m_pkFocusWnd->GetWndSkinsDesc(vkSkinDesc);

				for(unsigned int i=0; i<vkSkinDesc.size(); i++)
					if(strcmp(szSkinType, vkSkinDesc[i].second.c_str()) == 0)
					{
						pkSkin = *vkSkinDesc[i].first;
						break;
					}

				if(pkSkin)
					pkSkin->m_iBkTexID = pkTexMan->Load(strFullpath.c_str(), 0);
			}
		}
	}
}

void ZGuiTest::OnSelectWnd(ZGuiWnd *pkWnd)
{
	// Fyll listan med olika skin typer...
	ZGuiCombobox* pkCBox = (ZGuiCombobox*)GetWnd("SkinTypeCB");
	pkCBox->RemoveAllItems();
	vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
	pkWnd->GetWndSkinsDesc(vkSkinDesc);
	for(unsigned int i=0; i<vkSkinDesc.size(); i++)
		pkCBox->AddItem((char*)vkSkinDesc[i].second.c_str(), i);
}
