#ifndef _DONT_MAIN					// <- OBS! Flytta inte på denna. Måste ligga i
	#define _MAINAPPLICATION_		// just denna fil och inte på flera ställen.
	#define _DONT_MAIN
#endif

#include "zgr_app.h"
#include "../../../zerofpsv2/engine_systems/script_interfaces/si_gui.h"

ZGuiResEd g_kZGuiEd("zguiresed",1024,768,0);

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params )
{ 
	if(win && msg == ZGM_COMMAND || msg == ZGM_SELECTLISTITEM || msg == ZGM_CBN_SELENDOK)
	{
		list<ZGuiWnd*> kChilds;
		win->GetChildrens(kChilds); 

		list<ZGuiWnd*>::iterator it = kChilds.begin();
		for( ; it!=kChilds.end(); it++)
			if((*it)->GetID() == ((int*)params)[0])
			{				
				int iCmdExtra = -1;
				if(msg == ZGM_SELECTLISTITEM)
					iCmdExtra = ((int*)params)[2]; // double click

				g_kZGuiEd.OnCommand((*it)->GetName(), iCmdExtra); 
				break;
			}
	}
	else
	if(msg == ZGM_KEYPRESS && win != NULL)
	{
		if(g_kZGuiEd.m_bTestGUI && ((int*)params)[0] == KEY_ESCAPE)
			g_kZGuiEd.m_iTask = ZGuiResEd::TASK_TEST_GUI;		

		g_kZGuiEd.OnTextboxInput(string(win->GetName()), ((int*)params)[0]);
	}

	return true; 
};

ZGuiResEd::ZGuiResEd(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC) { }

void ZGuiResEd::OnInit()			
{
	m_iTask = TASK_DO_NOTHING;

	m_strNewFileToLoad = "";

	m_pkZFVFileSystem->AddRootPath( "../datafiles/dm/",	"/data");
	m_pkZFVFileSystem->AddRootPath( "../datafiles/mistlands/",	"/data");
	
  	//create camera
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.25,250);	
	m_pkCamera->SetClearViewPort(true);
	m_pkCamera->SetClearColor(Vector4(0.44f,0.44f,0.44f,0));
	m_pkZeroFps->AddRenderCamera(m_pkCamera);

	// get basicfs pointer
	m_pkBasicFS = m_pkZeroFps->m_pkBasicFS;

	m_bAlphaTextureMode = false;

	// create gui script
	GuiAppLua::Init(&g_kZGuiEd, m_pkScript);
	InitGui(m_pkScript, "defguifont", "data/script/gui/defskins.lua", 
		NULL, true, DISABLE_SCALE);

	SetTitle("ZGuiResEd");

	m_iZValueCounter = 1;
	m_bResize = false;
	m_bTestGUI = false;
	m_bForceCaptureToSel = false;

	m_strCurrTexDir = "data/textures/gui/";
	string strInfo = string( "Textures (root=)" ) + m_strCurrTexDir;
	SetText("GuiEd_TextureListLabel", (char*)strInfo.c_str());
	
	m_pkGui->Activate(true);

	m_bEditorMode = true;

	m_iTask = 5; // update texture list

	LoadGuiFromScript("data/script/gui/zguiresed.lua");

	GetWnd("GuiEd_SelectFileWnd")->Hide();

	((ZGuiCombobox*) GetWnd("GuiEd_WidgetTypeList"))->SetNumVisibleRows(9);
	
	ZGuiSkin* pkSkin = new ZGuiSkin();
	pkSkin->m_afBkColor[0] = 0.9f;
	pkSkin->m_afBkColor[1] = 0.9f;
	pkSkin->m_afBkColor[2] = 0.3f;
	pkSkin->m_unBorderSize = 1;
	((ZGuiListbox*) GetWnd("GuiEd_WidgetList"))->SetItemSelectedSkin(pkSkin);
	((ZGuiListbox*) GetWnd("GuiEd_TextureList"))->SetItemSelectedSkin(pkSkin);
	((ZGuiListbox*) GetWnd("GuiEd_SkinElementsList"))->SetItemSelectedSkin(pkSkin);

	AddListItem("GuiEd_WidgetTypeList", "Wnd" );
	AddListItem("GuiEd_WidgetTypeList", "Button" );
	AddListItem("GuiEd_WidgetTypeList", "Checkbox" );
	AddListItem("GuiEd_WidgetTypeList", "Combobox" );
	AddListItem("GuiEd_WidgetTypeList", "Label" );
	AddListItem("GuiEd_WidgetTypeList", "Listbox" );
	AddListItem("GuiEd_WidgetTypeList", "Radiobutton" );
	AddListItem("GuiEd_WidgetTypeList", "Scrollbar" );
	AddListItem("GuiEd_WidgetTypeList", "Slider" );
	AddListItem("GuiEd_WidgetTypeList", "TabControl" );
	AddListItem("GuiEd_WidgetTypeList", "Textbox" );
	AddListItem("GuiEd_WidgetTypeList", "Treebox" );
	AddListItem("GuiEd_WidgetTypeList", "Menu" );
	AddListItem("GuiEd_WidgetTypeList", "Progressbar" );

	SelListItem("GuiEd_WidgetTypeList", "Wnd");

	map<string, ZGuiWnd*> kWnds;
	m_pkGuiMan->GetWindows(kWnds);

	for(map<string, ZGuiWnd*>::iterator it = kWnds.begin(); it != kWnds.end(); it++)
	{
		SetFont(it->second->GetName(), "small7", 0, 0, 0, 0);
	}

	SetFont("GuiEd_ScriptList", "defguifont", 0, 0, 0, 0);

	if(m_pkRender->GetFullscreen() == false)
		ShowWnd("GuiEd_Close", false);

	//GetWnd("GuiEd_EditWnd")->m_bUseAlhpaTest = false;
	//GetWnd("GuiEd_NewWnd")->m_bUseAlhpaTest = false;

	Rect rc = 
	rc = GetWnd("GuiEd_EditWnd")->GetScreenRect();
	GetWnd("GuiEd_EditWnd")->SetPos(0, GetHeight()-rc.Height(), true, true); 

	rc = GetWnd("GuiEd_NewWnd")->GetScreenRect();
	GetWnd("GuiEd_NewWnd")->SetPos(GetWidth()-rc.Width(), 0, true, true); 
	
	NewGUI(false);

	m_iDesignResX = 800;
	m_iDesignResY = 600;

	SwitchDesignRes(m_iDesignResX,m_iDesignResY,false);

	GetWnd("GuiEd_DesignresLabel")->SetZValue(0);
	GetWnd("GuiMainWnd")->SortChilds(); 

	m_pkGui->PlaceWndFrontBack(GetWnd("GuiEd_EditWnd"), true);
	m_pkGui->PlaceWndFrontBack(GetWnd("GuiEd_NewWnd"), true);

	CheckButton("GuiEd_SelBackgroundTexture", true);
}

bool ZGuiResEd::IsEditorWnd(ZGuiWnd* pkWnd)
{
	if(pkWnd == NULL)
		return false;

	string strWnd = pkWnd->GetName();
	
	if(strWnd == "GuiEd_EditWnd" ||
		strWnd == "GuiEd_NewWnd" ||
		strWnd == "GuiEd_SelectFileWnd" ||
		strWnd == "GuiEd_DesignBorder" ||
		strWnd == "GuiDefMsgBox" )
		return true;

	if(ZGuiWnd* pkParent = pkWnd->GetParent())
	{
		string strParent = pkParent->GetName();
		if(strParent == "GuiEd_EditWnd" ||
			strParent == "GuiEd_NewWnd" ||
			strParent == "GuiEd_SelectFileWnd" ||
			strParent == "GuiDefMsgBox" )
			return true;
	}

	if(strWnd == "GuiMainWnd")
		return true;

	return false;
}