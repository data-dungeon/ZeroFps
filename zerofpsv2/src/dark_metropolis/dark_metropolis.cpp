	// dark_metropolis.cpp

#include "dark_metropolis.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"

DarkMetropolis g_kDM("DarkMetropolis",0,0,0);

static bool GUIPROC(ZGuiWnd* win, unsigned int msg, int numparms, 
						  void *params ) 
{
	switch(msg)
	{
	case ZGM_COMMAND:
		g_kDM.OnCommand(((int*)params)[0], 
			(((int*)params)[1] == 1) ? true : false, win);
		break;
	case ZGM_LBUTTONDOWN:
		g_kDM.OnClick(((int*)params)[0], ((int*)params)[1], true, true, win);
		break;
	case ZGM_LBUTTONUP:
		g_kDM.OnClick(((int*)params)[0], ((int*)params)[1], false, true, win);
		break;
	case ZGM_RBUTTONDOWN:    
		g_kDM.OnClick(((int*)params)[0], ((int*)params)[1], true, false, win);
		break;
	case ZGM_RBUTTONUP:
		g_kDM.OnClick(((int*)params)[0], ((int*)params)[1], false, false, win);
		break;
	case ZGM_LBUTTONDBLCLK:
		g_kDM.OnDoubleClick(((int*)params)[0], ((int*)params)[1], true, win);
		break;
	case ZGM_RBUTTONDBLCLK:
		g_kDM.OnDoubleClick(((int*)params)[0], ((int*)params)[1], false, win);
		break;
	case ZGM_MOUSEMOVE:
		g_kDM.OnMouseMove(((int*)params)[1], ((int*)params)[2], 
			((int*)params)[0] == 1 ? true : false, win);
		break;
	case ZGM_SCROLL:
		g_kDM.OnScroll(((int*)params)[0], ((int*)params)[2], win);
		break;
	case ZGM_CBN_SELENDOK:
		g_kDM.OnSelectCB(((int*)params)[0], ((int*)params)[1], win);
		break;
	case ZGM_KEYPRESS:
		g_kDM.OnKeyPress(((int*)params)[0], win);		
		break;
	}
	return true;
}

DarkMetropolis::DarkMetropolis(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	g_ZFObjSys.Log_Create("DarkMetropolis");	
} 

void DarkMetropolis::OnHud() 
{	
	m_pkFps->DevPrintf("common","Active Propertys: %d",
		m_pkObjectMan->GetActivePropertys());	
	m_pkFps->DevPrintf("common", "Fps: %f",m_pkFps->m_fFps);	
	m_pkFps->DevPrintf("common","Avrage Fps: %f",m_pkFps->m_fAvrageFps);			
		
	m_pkFps->m_bGuiMode = false;
	m_pkFps->ToggleGui();
}

void DarkMetropolis::OnIdle() 
{
	m_pkFps->SetCamera(m_pkCamera);		
	m_pkFps->GetCam()->ClearViewPort();	
	m_pkFps->UpdateCamera(); 	
}

void DarkMetropolis::OnInit() 
{
	SetTitle("Dark Metropolis");

	m_pkFps->m_bClientMode = true;

	m_pkLight->SetLighting(true);
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.25,250);	

	// create gui script
	GuiAppLua::Init(&g_kDM, m_pkScript);
	InitGui(m_pkScript,
		"data/textures/text/small.bmp",
		"data/script/gui/defskins.lua",
		NULL);

	m_pkInput->ShowCursor(true);
}

void DarkMetropolis::OnSystem() 
{
						
}

void DarkMetropolis::OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd *pkMainWnd)
{

}

void DarkMetropolis::OnClick(int x, int y, bool bMouseDown, bool bLeftButton, 
									  ZGuiWnd *pkMain)
{

}

void DarkMetropolis::OnDoubleClick(int x, int y, bool bLeftButton, 
											  ZGuiWnd *pkMain)
{

}

void DarkMetropolis::OnMouseMove(int x, int y, bool bMouseDown, ZGuiWnd *pkMain)
{

}

void DarkMetropolis::OnScroll(int iID, int iPos, ZGuiWnd *pkMain)
{

}

void DarkMetropolis::OnSelectCB(int ListBoxID, int iItemIndex, ZGuiWnd *pkMain)
{

}

void DarkMetropolis::OnKeyPress(int iKey, ZGuiWnd *pkWnd)
{

}

void DarkMetropolis::OnServerClientJoin(ZFClient* pkClient,int iConID, 
													 char* szLogin, char* szPass)
{

}

void DarkMetropolis::OnServerClientPart(ZFClient* pkClient,int iConID)
{

}

void DarkMetropolis::OnServerStart()
{

}

void DarkMetropolis::OnClientStart()
{

}

bool DarkMetropolis::StartUp()
{
	return true;
}

bool DarkMetropolis::ShutDown()
{
	return true;
}

bool DarkMetropolis::IsValid()
{
	return true;
}