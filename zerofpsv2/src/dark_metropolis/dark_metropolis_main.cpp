// dark_metropolis_main.cpp

#ifndef _DONT_MAIN					// <- OBS! Flytta inte på denna. Måste ligga i
	#define _MAINAPPLICATION_		// just denna fil och inte på flera ställen.
	#define _DONT_MAIN
#endif

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

void DarkMetropolis::OnInit() 
{
	SetTitle("Dark Metropolis");

	Register_Cmd("load",FID_LOAD);		

	//m_pkFps->m_bClientMode = true;

	m_pkLight->SetLighting(true);
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.25,250);	
	m_pkFps->SetRenderTarget(m_pkCamera);
	m_pkCamera->m_bRender = true;


	// create gui script
	GuiAppLua::Init(&g_kDM, m_pkScript);
	InitGui(m_pkScript,
		"data/textures/text/ms_sans_serif8.bmp",
		"data/script/gui/defskins.lua",
		NULL);

	LoadGuiFromScript(m_pkScript, "data/script/gui/dm_start.lua");

	m_pkInput->ShowCursor(true);
	m_pkLight->SetLighting(true);
	
	
	m_kSun.kRot = Vector3(1,2,1);
	m_kSun.kDiffuse=Vector4(1,1,1,0);
	m_kSun.kAmbient=Vector4(0.2,0.2,0.2,0);
	m_kSun.iType=DIRECTIONAL_LIGHT;			
	m_kSun.iPriority=10;
	m_kSun.fConst_Atten=1;
	m_kSun.fLinear_Atten=0;
	m_kSun.fQuadratic_Atten=0;	
	
	m_pkLight->Add(&m_kSun);

	
}

void DarkMetropolis::OnIdle() 
{
	m_pkFps->SetCamera(m_pkCamera);		
	m_pkFps->GetCam()->ClearViewPort();	

	Input();

	m_pkFps->UpdateCamera(); 	
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
	cout<<"STarting server"<<endl;

	m_pkCameraEntity = m_pkObjectMan->CreateObjectFromScript("data/script/objects/t_camedit.lua");	
	if(m_pkCameraEntity)
	{
		m_pkCameraEntity->SetParent( m_pkObjectMan->GetWorldObject() );
		m_pkCameraEntity->SetWorldPosV(Vector3(0,0,0));
		P_Camera* m_pkCamProp = (P_Camera*)m_pkCameraEntity->GetProperty("P_Camera");
		m_pkCamProp->SetCamera(m_pkCamera);
		m_pkCameraEntity->GetSave() = false;
	}
			

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

void DarkMetropolis::RegisterPropertys()
{
	m_pkPropertyFactory->Register("P_ClientControl", Create_P_ClientControl);
	m_pkPropertyFactory->Register("P_ServerInfo", Create_P_ServerInfo);
	m_pkPropertyFactory->Register("P_Event", Create_P_Event);
}

void DarkMetropolis::Input()
{
}

void DarkMetropolis::RunCommand(int cmdid, const CmdArgument* kCommand)
{
//	unsigned int i;
//	vector<string>	kUsers;
//	int iMode;
//	float fTest;

	switch(cmdid) {

		case FID_LOAD:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				m_pkConsole->Printf("load [mapdir]");
				break;				
			}
			
			cout<<"BLUB:"<<kCommand->m_kSplitCommand.size()<<endl;
			cout<<"loading world:"<<kCommand->m_kSplitCommand[1]<<endl;
			
			if(kCommand->m_kSplitCommand.size() > 2)
			{
				m_pkConsole->Printf("loading savegame: %s",kCommand->m_kSplitCommand[2].c_str());
				
				if(!m_pkObjectMan->LoadWorld(kCommand->m_kSplitCommand[1],kCommand->m_kSplitCommand[2]))
				{
					cout<<"Error loading world"<<endl;
					break;
				}				
			}
			else if(!m_pkObjectMan->LoadWorld(kCommand->m_kSplitCommand[1]))
			{
				cout<<"Error loading world"<<endl;
				break;
			}				

			cout<<"starting server"<<endl;
			GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);			
			

			
			
			break;		

	}
}

