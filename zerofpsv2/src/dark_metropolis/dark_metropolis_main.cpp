// dark_metropolis.cpp

#ifndef _DONT_MAIN					// <- OBS! Flytta inte på denna. Måste ligga i
	#define _MAINAPPLICATION_		// just denna fil och inte flera ställen.
	#define _DONT_MAIN
#endif

#include "../zerofpsv2/engine/application.h"
#include "../zerofpsv2/gui/zgui.h"
#include "dark_metropolis.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"

DarkMetropolis g_kDM("DarkMetropolis",0,0,0);

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
	m_pkLight->SetLighting(true);
}

void DarkMetropolis::OnSystem() 
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