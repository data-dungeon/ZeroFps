#include "zgresedit.h"
#include "resource_id.h"
#include "guibuilder.h"
#include "propertybox.h"
#include "controlbox.h"
#include "skinbox.h"
#include "msgbox.h"
#include "../zeroedit/fileopendlg.h"
#include "serialization.h"

///////////////////////////////////////////////////////////////////////////////
// Global objects
//
static ZGResEdit g_kResEditor("ZGResEdit",1024,768,24);

///////////////////////////////////////////////////////////////////////////////
// Global functions
//
static bool MAINPROC( ZGuiWnd* wnd,unsigned int msg, int num,void *parms) { 
	return g_kResEditor.WinProc(wnd,msg,num,parms); }

static bool SKINPROC( ZGuiWnd* wnd,unsigned int msg, int num,void *parms ) { 
	return g_kResEditor.m_pkSkinBox->DlgProc(wnd,msg,num,parms); }

static bool PROPERTYPROC( ZGuiWnd* wnd,unsigned int msg, int num,void *parms ) { 
	return g_kResEditor.m_pkPropertyBox->DlgProc(wnd,msg,num,parms); }

static bool CONTROLPROC( ZGuiWnd* wnd,unsigned int msg, int num,void *parms ) { 
	return g_kResEditor.m_pkControlBox->DlgProc(wnd,msg,num,parms); }

static bool OPENFILEPROC(ZGuiWnd* wnd,unsigned int msg,int num,void *parms ) {
	return g_kResEditor.m_pkFileOpenDlg->DlgProc(wnd,msg,num,parms); }

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//
ZGResEdit::ZGResEdit(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth) 
{
	m_kMouseState = IDLE;
	m_pnCursorRangeDiffX=m_pnCursorRangeDiffY=-1;
	m_iGridPrec = 5;

	m_pkFileOpenDlg = NULL;
	m_pkControlBox = NULL;
	m_pkPropertyBox = NULL;
	m_pkSkinBox = NULL;
}

ZGResEdit::~ZGResEdit()
{

}

///////////////////////////////////////////////////////////////////////////////
// Name: OnInit
// Description: 
//
void ZGResEdit::OnInit()
{
	m_pkCamera=new Camera(Vector3(0,10,0),Vector3(0,0,0),85,1.333,0.25,250);
	m_pkGuiBuilder=new GuiBuilder(pkGui,pkTexMan,pkGuiMan,
		Rect(0,0,m_iWidth,m_iHeight));

	glClearColor(1,1,1,0);

	SDL_ShowCursor(SDL_DISABLE);

	m_pkGuiBuilder->InitSkins();

	// Change gui font
	ZGuiFont* pkDefaultFont = new ZGuiFont(16,16,0,ZG_DEFAULT_GUI_FONT);
	pkDefaultFont->CreateFromFile("../data/textures/text/ms_sans_serif8.bmp");
	pkGui->SetDefaultFont(pkDefaultFont);

	if(!Create())
	{
		cout << "Failed to create app!" << endl;
	}

	SetCursor("file:../data/textures/cursor.bmp",
		"file:../data/textures/cursor_a.bmp");

	pkFps->m_bGuiTakeControl = true;
	pkFps->ToggleGui();

	SDL_WM_SetCaption("Resource Editor","mainicon.ico"); 
}

///////////////////////////////////////////////////////////////////////////////
// Name: OnIdle
// Description: 
//
void ZGResEdit::OnIdle()
{
	static bool s_bMouseDown = false;
	if( pkInput->Pressed(KEY_LSHIFT) && pkInput->Pressed(MOUSELEFT) )
	{
		if(s_bMouseDown == false)
		{
			m_pkRectWnd->Show();
			int mx, my;
			pkInput->MouseXY(mx, my);
			pkGui->SetFocus(m_pkRectWnd);
			m_pkRectWnd->SetPos(mx, my, true, true);
			m_pkRectWnd->Resize(0,0);
			s_bMouseDown = true;
			m_bResizeRectWnd = true;
			return;
		}
	}	
	if(!pkInput->Pressed(MOUSELEFT))
	{
		s_bMouseDown = false;
		m_bResizeRectWnd = false;
		
		if(ZGuiWnd::m_pkFocusWnd != m_pkRectWnd )
		{
			m_pkRectWnd->Hide();
			m_pkRectWnd->Resize(0,0);
		}
	}


	DisableClickWnd();
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();

/*	if(pkFps->m_bConsoleMode == true)
		pkFps->QuitEngine();*/

	EnableClickWnd();
}

void ZGResEdit::OnHud()
{
/*	StartDraw2D();

		OnPaint();

	EndDraw2D();*/
}

///////////////////////////////////////////////////////////////////////////////
// Name: WinProc
// Description: 
//
bool ZGResEdit::WinProc(ZGuiWnd* pkWnd,unsigned int uiMessage,
						int iNumberOfParams,void *pkParams ) 
{
	static int FOCUS_BORDER_COL[3] =
	{
		pkGui->GetFocusBorderSkin()->m_afBorderColor[0]*255,
		pkGui->GetFocusBorderSkin()->m_afBorderColor[1]*255,
		pkGui->GetFocusBorderSkin()->m_afBorderColor[2]*255
	};

	switch(uiMessage)
	{
	// Command Messages
	case ZGM_COMMAND:
		switch(((int*)pkParams)[0]) // control id
		{
		case ID_CTRLBOX_OPENSKINDLG_BN:
			if(SelectWnd::GetInstance()->m_pkWnd)
				m_pkSkinBox->OnOpen();
			break;
		case ID_GRID_CB:
			m_bUseGrid = m_pkGuiBuilder->IsButtonChecked("GridCB"); 
			break;
		case ID_FILEPATH_OPEN_BN:
			// Open File
			if(!m_pkFileOpenDlg->GetFlag(SAVE_FILES))
			{
				char szFile[512];
				sprintf(szFile, "%s/%s", m_pkFileOpenDlg->m_szSearchPath.c_str(),
					m_pkFileOpenDlg->m_szCurrentFile.c_str());

				Serialization kLoadRC(szFile, m_pkINI, false);
				m_pkControlBox->LoadGUI(m_pkINI, pkTexMan);

				sprintf(szFile, "ZeroFps Resource Editor [%s/%s]", 
					m_pkFileOpenDlg->m_szSearchPath.c_str(),
					m_pkFileOpenDlg->m_szCurrentFile.c_str());
				SDL_WM_SetCaption(szFile,"mainicon.ico"); 
			}
			// Save File
			else
			{
				SaveFile();
			}
			break;
		
		// This message is sent from the ControlBox object
		// and most work is already done.
		case ID_CTRLBOX_COPY_BN:
			m_pkPropertyBox->Update(SelectWnd::GetInstance()->m_pkWnd);
			break;

		}
		break;

	// Left Mouse Click Down Message
	case ZGM_LBUTTONDOWN:
		{				
			if(pkWnd != m_pkRectWnd)
			{
				m_pkRectWnd->Hide();
			}

			if(!IsGuiWnd(pkWnd) || pkWnd == m_pkRectWnd)
			{			
				//DisableClickWnd();

				m_iClickPosX = ((int*)pkParams)[0];
				m_iClickPosY = ((int*)pkParams)[1];

				Rect rc = pkWnd->GetScreenRect();

				m_pnCursorRangeDiffX = m_iClickPosX-rc.Left;
				m_pnCursorRangeDiffY = m_iClickPosY-rc.Top;

				int dist_to_horz_border = abs(m_iClickPosX-rc.Right);
				int dist_to_vert_border = abs(m_iClickPosY-rc.Bottom);

				// Push shift to prevent window from being resized
				// and ctrl to force window to be resized.

				bool bResize = !pkInput->Pressed(KEY_LSHIFT) && 
					!pkInput->Pressed(KEY_RSHIFT);
				bool bForceResize = pkInput->Pressed(KEY_LCTRL) || 
					pkInput->Pressed(KEY_RCTRL);

				if(dist_to_horz_border < 4 && dist_to_vert_border < 4 && bResize
					|| bForceResize)
				{
					m_kMouseState =	RESIZING_BOTH;
				}
				else
				if(dist_to_horz_border < 4 && bResize || bForceResize)
				{
					m_kMouseState =	RESIZING_HORZ; 
				}
				else
				if(dist_to_vert_border < 4 && bResize || bForceResize)
				{
					m_kMouseState =	RESIZING_VERT; 
				}
				else
				{
					if(pkWnd->GetScreenRect().Inside(m_iClickPosX,m_iClickPosY))
						m_kMouseState = MOVING;
					bResize = false;
				}
				
				if(bResize || bForceResize)
				{
					m_iResizewnd_old_x = pkWnd->GetScreenRect().Left;
					m_iResizewnd_old_y = pkWnd->GetScreenRect().Top;

					m_pkGuiBuilder->SetColor(
						pkGui->GetFocusBorderSkin()->m_afBorderColor,0,0,0); 
				}

				if(pkWnd != m_pkRectWnd)
					SelectWnd::GetInstance()->m_pkWnd = pkWnd;

				pkGui->SetFocus(pkWnd);
				m_pkPropertyBox->Update(SelectWnd::GetInstance()->m_pkWnd);
			}
		}
		break;
	// Left Mouse Click Up Message
	case ZGM_LBUTTONUP:
		{
			m_kMouseState = IDLE;
			m_pkGuiBuilder->SetColor(
				pkGui->GetFocusBorderSkin()->m_afBorderColor,
					FOCUS_BORDER_COL[0],FOCUS_BORDER_COL[1],FOCUS_BORDER_COL[2]);
		}
		break;
	// Left Mouse Double Click Message
	case ZGM_LBUTTONDBLCLK:
		if(!IsGuiWnd(pkWnd))
		{
			int x = ((int*)pkParams)[0];
			int y = ((int*)pkParams)[1];
			
			m_pkPropertyBox->OnOpen(x,y);
		}
		break;
	// Mouse Move Message
	case ZGM_MOUSEMOVE:
		if(((bool*)pkParams)[0] == true) // Left button down
		{
			switch(m_kMouseState)
			{
			case RESIZING_BOTH:
			case RESIZING_HORZ:
			case RESIZING_VERT:
				if(!IsGuiWnd(pkWnd) || pkWnd == m_pkRectWnd)
				{
					int x = ((int*)pkParams)[1];
					int y = ((int*)pkParams)[2];

					if(m_bUseGrid)
					{
						x -= x%m_iGridPrec;
						y -= y%m_iGridPrec;
					}

					ResizeWnd(pkWnd,x,y);
				}
				break;
			case MOVING:
				int x = ((int*)pkParams)[1]-m_pnCursorRangeDiffX;
				int y = ((int*)pkParams)[2]-m_pnCursorRangeDiffY;

				if(pkWnd == m_pkRectWnd )
				{
					// Kolla vilka fönster som befinner sig innanför
					// rammens gränser.

					vector<ControlBox::CreateWnd> pkWindows = 
						m_pkControlBox->m_pkCreatedWindows;

					m_pkMoveWnds.clear(); 

					for(unsigned int i=0; i<pkWindows.size(); i++)
					{
						if(pkWnd->GetScreenRect().Inside(
							pkWindows[i].m_pkWnd->GetScreenRect()))
						{
							m_pkMoveWnds.push_back(pkWindows[i].m_pkWnd);
						}
					}
				}
				
				if(!IsGuiWnd(pkWnd) || pkWnd == m_pkRectWnd )
				{
					if(m_bUseGrid)
					{
						x -= x%m_iGridPrec;
						y -= y%m_iGridPrec;
					}

					if(pkWnd == m_pkRectWnd )
					{
						int iOldPosXRectWnd = pkWnd->GetScreenRect().Left;
						int iOldPosYRectWnd = pkWnd->GetScreenRect().Top;

						if(m_bResizeRectWnd)
						{
							int u,v;
							pkInput->MouseXY(u,v);
							int sx = u-iOldPosXRectWnd; if(sx<0) sx=0;
							int sy = v-iOldPosYRectWnd; if(sy<0) sy=0;
							pkWnd->Resize(sx,sy);
						}
						else
						{
							MoveWnd(pkWnd,x,y);

							int iNewPosXRectWnd = pkWnd->GetScreenRect().Left;
							int iNewPosYRectWnd = pkWnd->GetScreenRect().Top;

							int MoveOffsetX = iNewPosXRectWnd-iOldPosXRectWnd;
							int MoveOffsetY = iNewPosYRectWnd-iOldPosYRectWnd;

							for(int i=0; i<m_pkMoveWnds.size(); i++)
							{
								bool bSkip = false;

								// Flytta inte på ett fönster om dess parent
								// också skall flyttas (tårta-på-tårta).
								ZGuiWnd* pkParent = m_pkMoveWnds[i]->GetParent();
								for(int j=0; j<m_pkMoveWnds.size(); j++)
									if( m_pkMoveWnds[i] != m_pkMoveWnds[j] &&
										m_pkMoveWnds[j] == pkParent)
										{
											bSkip = true;
											break;
										}

								if(bSkip == false)
									m_pkMoveWnds[i]->Move(
										MoveOffsetX, MoveOffsetY);
							}

							return true;
						}
					}
					else
					{
						MoveWnd(pkWnd,x,y);
					}
				}
				break;
			}
		}
		break;
	// Keypush Down Message
	case ZGM_KEYDOWN:
		switch(((int*)pkParams)[0]) // key pressed
		{
		case KEY_DELETE:

			int i;

			for( i=0; i<m_pkMoveWnds.size(); i++)
				if(!IsGuiWnd(m_pkMoveWnds[i]))
				{
					if(SelectWnd::GetInstance()->m_pkWnd == m_pkMoveWnds[i])
						SelectWnd::GetInstance()->m_pkWnd = NULL;

					// First, delete skin.
					ZGuiSkin* pkSkin = m_pkMoveWnds[i]->GetSkin();
					delete pkSkin;
					pkSkin = NULL;
					
					// Erase the new type from the list in the ControlBox class
					m_pkControlBox->UnregisterNewType(m_pkMoveWnds[i]);

					if(!pkGui->UnregisterWindow(m_pkMoveWnds[i]))
						cout << "Failed to unregister window!" << endl;

					if(ZGuiWnd::m_pkFocusWnd && !IsGuiWnd(ZGuiWnd::m_pkFocusWnd))
						SelectWnd::GetInstance()->m_pkWnd = ZGuiWnd::m_pkFocusWnd;
				}

			if(!IsGuiWnd(pkWnd))
			{
				if(SelectWnd::GetInstance()->m_pkWnd == pkWnd)
					SelectWnd::GetInstance()->m_pkWnd = NULL;

				// First, delete skin.
				ZGuiSkin* pkSkin = pkWnd->GetSkin();
				delete pkSkin;
				pkSkin = NULL;
				
				// Erase the new type from the list in the ControlBox class
				m_pkControlBox->UnregisterNewType(pkWnd);

				if(!pkGui->UnregisterWindow(pkWnd))
					cout << "Failed to unregister window!" << endl;

				if(ZGuiWnd::m_pkFocusWnd && !IsGuiWnd(ZGuiWnd::m_pkFocusWnd))
					SelectWnd::GetInstance()->m_pkWnd = ZGuiWnd::m_pkFocusWnd;
			}

			break;
		case KEY_RETURN:
			{
				ZGuiWnd* pkFocusWnd = ZGuiWnd::m_pkFocusWnd;

				if(!IsGuiWnd(pkFocusWnd) && !m_pkPropertyBox->IsOpen())
				{
					SelectWnd::GetInstance()->m_pkWnd = pkFocusWnd;

					int x = pkFocusWnd->GetScreenRect().Right;
					int y = pkFocusWnd->GetScreenRect().Bottom;
					
					m_pkPropertyBox->OnOpen(x,y);

					pkInput->Reset();
				}
			}
			break;
		case KEY_UP:
		case KEY_DOWN:
			{
				if( !IsGuiWnd(SelectWnd::GetInstance()->m_pkWnd) && 
					!m_pkPropertyBox->IsOpen())
				{
					int offset = 1;
					if(m_bUseGrid)
						offset = m_iGridPrec;

					if(((int*)pkParams)[0] == KEY_UP)
						offset = -offset;
					
					Rect rc = SelectWnd::GetInstance()->m_pkWnd->GetScreenRect();
					MoveWnd(SelectWnd::GetInstance()->m_pkWnd,
						rc.Left,rc.Top+offset);
					pkGui->SetFocus(SelectWnd::GetInstance()->m_pkWnd);
				}
			}
			break;
		case KEY_LEFT:
		case KEY_RIGHT:
			{
				if( !IsGuiWnd(SelectWnd::GetInstance()->m_pkWnd) && 
					!m_pkPropertyBox->IsOpen())
				{
					int offset = 1;
					if(m_bUseGrid)
						offset = m_iGridPrec;

					if(((int*)pkParams)[0] == KEY_LEFT)
						offset = -offset;

					Rect rc = SelectWnd::GetInstance()->m_pkWnd->GetScreenRect();
					MoveWnd(SelectWnd::GetInstance()->m_pkWnd,rc.Left+offset,
						rc.Top);
					pkGui->SetFocus(SelectWnd::GetInstance()->m_pkWnd);
				}
			}
			break;
		}
		break;
	// Editbox Typing Message
	case ZGM_EN_CHANGE:
		switch(((int*)pkParams)[0]) // id of the textbox
		{
		case ID_GRID_PRECISION:
			m_iGridPrec = m_pkGuiBuilder->GetTextInt("GridPrecEB");
			break;
		// detta meddelande har skickats fron propertybox objektet.
		}
		break;
	// Combobox select ok.
	case ZGM_CBN_SELENDOK:
		int iID = ((int*)pkParams)[0];
		ZGuiListitem* pkSelItem;
		switch(iID)
		{
			case ID_FILE_MENU:
			{
				ZGuiCombobox* pkCbox = (ZGuiCombobox*)
					m_pkGuiBuilder->GetWnd("FileMenuCB");
				pkSelItem = pkCbox->GetListbox()->GetSelItem();

				unsigned long flags = 0;

				// Switch Menu Item
				switch(pkSelItem->GetIndex())
				{	
				case IDM_NEW:
					m_pkControlBox->ClearAll(); 
					break;

				case IDM_EXIT:
					pkFps->QuitEngine();
					break;
				case IDM_OPENFILE:
					if(m_pkFileOpenDlg)
						delete m_pkFileOpenDlg;
					m_pkFileOpenDlg = new FileOpenDlg(pkGui, pkFps->m_pkBasicFS,
						MAINPROC, flags);
					m_pkFileOpenDlg->Create(100,100,500,500,OPENFILEPROC);
					break;
				case IDM_SAVEFILE:
					if(m_pkFileOpenDlg)
					{
						SaveFile();
					}
					break;
				case IDM_SAVEFILE_AS:
					flags = SAVE_FILES;
					if(m_pkFileOpenDlg)
						delete m_pkFileOpenDlg;
					m_pkFileOpenDlg = new FileOpenDlg(pkGui, pkFps->m_pkBasicFS,
						MAINPROC, flags);
					m_pkFileOpenDlg->Create(100,100,500,500,OPENFILEPROC);
					break;
				}
			}
			break;
		}
	}

	return true; 
}

///////////////////////////////////////////////////////////////////////////////
// Name: Create
// Description: 
//
bool ZGResEdit::Create()
{
	m_pkINI = static_cast<ZFIni*>(g_ZFObjSys.GetObjectPtr("ZFIni"));	

	int x,y,w,h;

	pkGuiMan->Add(string("rectwnd_skin"),new ZGuiSkin(0,0,0,255,0,0,1,true));
	m_pkRectWnd = m_pkGuiBuilder->CreateMainWindow(ID_RECTWND+1, ID_RECTWND, 
		"RectWnd", 0,0,0,0, MAINPROC, "rectwnd_skin");
	m_pkRectWnd->SetSkin(pkGuiMan->Skin("rectwnd_skin"));
	m_pkRectWnd->Hide();

	w = 150; h = 270; x = m_iWidth-w; y = 0;
	m_pkControlBox = new ControlBox(m_pkGuiBuilder,MAINPROC);
	m_pkControlBox->Create(x,y,w,h,CONTROLPROC);

	w = 400; h = 210; x = m_iWidth-w; y = m_iHeight-h;
	m_pkPropertyBox = new PropertyBox(m_pkGuiBuilder,MAINPROC);
	m_pkPropertyBox->Create(x,y,w,h,PROPERTYPROC);

	w = 550; h = 350; x = m_iWidth-w-150; y = 150;
	m_pkSkinBox = new SkinBox(m_pkGuiBuilder,MAINPROC,pkFps->m_pkBasicFS,
		pkTexMan);
	m_pkSkinBox->Create(x,y,w,h,SKINPROC);

	ZGuiWnd* pkMenu = m_pkGuiBuilder->CreateMainWindow(ID_MENU_MAINWND,
		ID_MENU_WND,"FileMenu",0,0,m_iWidth-150,20,MAINPROC);
	ZGuiCombobox* pkFileMenuCB = m_pkGuiBuilder->CreateCombobox(pkMenu,
		ID_FILE_MENU,"FileMenuCB",2,2,20,20,true);

	pkFileMenuCB->SetNumVisibleRows(5);
	pkFileMenuCB->SetLabelText("File");
	pkFileMenuCB->AddItem("New", IDM_NEW);
	pkFileMenuCB->AddItem("Open...", IDM_OPENFILE);
	pkFileMenuCB->AddItem("Save", IDM_SAVEFILE);
	pkFileMenuCB->AddItem("Save as...", IDM_SAVEFILE_AS);
	pkFileMenuCB->AddItem("Exit", IDM_EXIT);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetCursor
// Description: 
//
void ZGResEdit::SetCursor(const char* szFileNameBitmap,
						  const char* szFileNameBitmapAlpha) const
{
	int cursor_tex = pkTexMan->Load(szFileNameBitmap,0);
	int cursor_tex_a = pkTexMan->Load(szFileNameBitmapAlpha,0);
	pkGui->SetCursor(cursor_tex,cursor_tex_a,32,32);
}

///////////////////////////////////////////////////////////////////////////////
// Name: ResizeWnd
// Description: 
//
void ZGResEdit::ResizeWnd(ZGuiWnd* pkWnd,int x,int y) const
{
	Rect rc = pkWnd->GetScreenRect();
	ZGuiWnd* pkParent = pkWnd->GetParent(true);

	// Make sure that the main window isn´t smaller then it fit all childs.
	if(pkParent == NULL)
	{
		list<ZGuiWnd*> pkChilds;
		int ctrl_highest_x=-1000;
		int ctrl_highest_y=-1000;
		pkWnd->GetChildrens(pkChilds);
		for( WINit win = pkChilds.begin(); win != pkChilds.end(); win++)
			 {
				Rect rc = (*win)->GetScreenRect();
				if(rc.Bottom > ctrl_highest_y)
					ctrl_highest_y = rc.Bottom;
				if(rc.Right > ctrl_highest_x)
					ctrl_highest_x = rc.Right;
			 }

		if(x < ctrl_highest_x) x = ctrl_highest_x;
		if(y < ctrl_highest_y) y = ctrl_highest_y;
	}
	
	switch(m_kMouseState)
	{
	case RESIZING_BOTH:
		if(x-rc.Left > 0 && y-rc.Top > 0)
		{
			int iNewWidth = x-m_iResizewnd_old_x;
			if(pkParent)
			{
				if(iNewWidth + rc.Left > pkParent->GetScreenRect().Right)
					iNewWidth = pkParent->GetScreenRect().Right-rc.Left;
			}
			int iNewHeight = y-m_iResizewnd_old_y;
			if(pkParent)
			{
				if(iNewHeight + rc.Top > pkParent->GetScreenRect().Bottom)
					iNewHeight = pkParent->GetScreenRect().Bottom-rc.Top;
			}
			pkWnd->Resize(iNewWidth,iNewHeight);
		}
		break;
	case RESIZING_HORZ:
		if(x-rc.Left > 0)
		{
			int iNewWidth = x-m_iResizewnd_old_x;
			if(pkParent)
			{
				if(iNewWidth + rc.Left > pkParent->GetScreenRect().Right)
					iNewWidth = pkParent->GetScreenRect().Right-rc.Left;
			}

			pkWnd->Resize(iNewWidth,rc.Height());
		}
		break;
	case RESIZING_VERT:
		if(y-rc.Top > 0)
		{
			int iNewHeight = y-m_iResizewnd_old_y;
			if(pkParent)
			{
				if(iNewHeight + rc.Top > pkParent->GetScreenRect().Bottom)
					iNewHeight = pkParent->GetScreenRect().Bottom-rc.Top;
			}
			pkWnd->Resize(rc.Width(),iNewHeight);
		}
		break;

	default:
		return;
	}

	m_pkPropertyBox->Update(SelectWnd::GetInstance()->m_pkWnd);
}

///////////////////////////////////////////////////////////////////////////////
// Name: MoveWnd
// Description: 
//
void ZGResEdit::MoveWnd(ZGuiWnd* pkWnd,int x,int y) const
{
	Rect rc = pkWnd->GetScreenRect();
	
/*	switch(m_kMouseState)
	{
	case MOVING:*/
		ZGuiWnd* pkParent = pkWnd->GetParent(true);

		// Make sure that the window stays inside the parent frame.
		if(pkParent)
		{
			Rect rcBorder = pkParent->GetScreenRect();
			int w = pkWnd->GetScreenRect().Width();
			int h = pkWnd->GetScreenRect().Height();

			if(x+w > rcBorder.Right) x = rcBorder.Right-w;
			if(x < rcBorder.Left) x = rcBorder.Left;
			if(y+h > rcBorder.Bottom) y = rcBorder.Bottom-h;
			if(y < rcBorder.Top) y = rcBorder.Top;
		}

		// Fönstret utan parent skall oxå gå att flytta...
		pkWnd->SetPos(x,y,true,true);
	//	m_pkPropertyBox->Update(SelectWnd::GetInstance()->m_pkWnd);
	/*	break;
	}*/
}

///////////////////////////////////////////////////////////////////////////////
// Name: IsGuiWnd
// Description: Ignorera vissa fönster som tillhör själva gui:et 
//				för applikationen.
//
bool ZGResEdit::IsGuiWnd(ZGuiWnd *pkWnd) const
{
	if(!pkWnd)
		return true;

	static char* aGuiWnd[] =
	{
		"RectWnd",
		"CtrlBoxWnd",
		"CtrlPropBoxWnd",
		"SkinBoxWnd",
		"FileMenu",
		"FOOpenFileWnd",
	};

	int i;

	for( i=0; i<sizeof(aGuiWnd) / sizeof(aGuiWnd[0]); i++)
	{
		if(pkWnd == pkGuiMan->Wnd(aGuiWnd[i]))
			return true;

		ZGuiWnd* pkRootParent = pkWnd->GetParent(true);
		if(pkRootParent && pkRootParent == pkGuiMan->Wnd(aGuiWnd[i]))
			return true;
	}

	return false;
}

void ZGResEdit::DisableClickWnd()
{
	for(unsigned int i=0; i<m_pkControlBox->m_pkCreatedWindows.size(); i++)
	{
		m_pkControlBox->m_pkCreatedWindows[i].m_pkWnd->Disable();
	}
}

void ZGResEdit::EnableClickWnd()
{
	for(unsigned int i=0; i<m_pkControlBox->m_pkCreatedWindows.size(); i++)
	{
		m_pkControlBox->m_pkCreatedWindows[i].m_pkWnd->Enable();
	}
}

void ZGResEdit::SaveFile()
{
	if(m_pkFileOpenDlg)
	{
		char szFile[512];
		sprintf(szFile, "%s/%s", m_pkFileOpenDlg->m_szSearchPath.c_str(),
			"zgresource_id.h");

		Serialization kSaveIDs(szFile, m_pkINI, true);
		kSaveIDs.Output("#ifndef _ZGRESOURCE_ID_H\n#define _ZGRESOURCE_ID_H\n\nenum WindowID\n{\n");
		m_pkControlBox->PrintWindowIDs(&kSaveIDs);
		kSaveIDs.Outputa("};\n\n#endif // #ifndef _ZGRESOURCE_ID_H");

		sprintf(szFile, "%s/%s", m_pkFileOpenDlg->m_szSearchPath.c_str(),
			"zgresource_rc.txt");

		Serialization kSaveRC(szFile, m_pkINI, true);
		kSaveRC.Output("; ZGui resource script.\n\n");
		m_pkControlBox->PrintWindowRC(&kSaveRC, pkTexMan);
	}
}

