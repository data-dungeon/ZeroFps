#include "zguitest.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"

ZGuiTest g_kGuiTest("GuiTest",0,0,0);

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
	switch(msg)
	{
	case ZGM_COMMAND:
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
		break;
	case ZGM_KEYPRESS:	
		break;

	}
	return true;
}

ZGuiTest::ZGuiTest(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 

}

void ZGuiTest::OnInit()
{
	glEnable(GL_LIGHTING );

	pkLight->SetLighting(true);

	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.25,100);	

	// create gui script
	GuiAppLua::Init(&g_kGuiTest, pkScript);

	InitializeGui(pkGui, pkTexMan, pkScript, pkGuiMan, 
		"data/textures/text/paternoster8.bmp",
		"data/script/gui/gui_create_client.lua");

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
	CreateWnd(Wnd, "TestWnd", "", "", 100, 100, 200, 200, 0);	
}