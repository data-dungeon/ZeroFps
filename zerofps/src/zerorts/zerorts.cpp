#include "zerorts.h"

ZeroRTS g_kZeroRTS("ZeroRTS",1024,768,16);

ZeroRTS::ZeroRTS(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) { }

void ZeroRTS::OnInit() 
{
	pkConsole->Printf(" ZeroRTS");
	pkConsole->Printf("--------------------------------");
	pkConsole->Printf(" Does anything work yet?");
	
	//run autoexec script
	if(!pkIni->ExecuteCommands("zerorts_autoexec.ini"))
		pkConsole->Printf("No game_autoexec.ini.ini found");


	Init();
}

void ZeroRTS::Init()
{
	//register commmands
	g_ZFObjSys.Register_Cmd("load",FID_LOAD,this);		
	g_ZFObjSys.Register_Cmd("unload",FID_UNLOAD,this);			
	
	
	//damn "#¤(="%#( lighting fix
	glEnable(GL_LIGHTING );
	
	//initiate our camera
	m_pkCamera=new Camera(Vector3(0,60,0),Vector3(70,0,0),90,1.333,0.25,250);	
	
	//disable zones modells
	pkLevelMan->SetVisibleZones(false);

	//register actions
	RegisterActions();

	// Show cursor
	int cursor_tex = pkTexMan->Load("file:../data/textures/cursor.bmp", 0);
	int cursor_tex_a = pkTexMan->Load("file:../data/textures/cursor_a.bmp", 0);
	
	int mx,my;
	pkInput->MouseXY(mx,my);
	pkGui->SetCursor(mx, my, cursor_tex, cursor_tex_a, 32, 32);
	pkGui->ShowCursor(true);
	SDL_ShowCursor(SDL_DISABLE);	

	pkFps->m_bGuiMode = false;
	pkFps->ToggleGui();
	
}

void ZeroRTS::RegisterActions()
{
	m_iActionCamLeft=pkInput->RegisterAction("CamLeft");
	m_iActionCamRight=pkInput->RegisterAction("CamRight");
	m_iActionCamUp=pkInput->RegisterAction("CamUp");
	m_iActionCamDown=pkInput->RegisterAction("CamDown");
}



void ZeroRTS::OnIdle(void) 
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	
	pkFps->DevPrintf("common","Active Propertys: %d",pkObjectMan->GetActivePropertys());
			
	Input();
}

void ZeroRTS::Input()
{
	
	//camera movements
	if(pkInput->Action(m_iActionCamLeft))
	{
		m_pkCamera->GetPos().x -= 100*pkFps->GetFrameTime();
	}
	if(pkInput->Action(m_iActionCamRight))
	{
		m_pkCamera->GetPos().x += 100*pkFps->GetFrameTime();
	}
	if(pkInput->Action(m_iActionCamUp))
	{
		m_pkCamera->GetPos().z -= 100*pkFps->GetFrameTime();
	}
	if(pkInput->Action(m_iActionCamDown))
	{
		m_pkCamera->GetPos().z += 100*pkFps->GetFrameTime();
	}
}


void ZeroRTS::OnHud(void) 
{	
	pkFps->m_bGuiMode = false;
	pkFps->ToggleGui();
}

void ZeroRTS::OnServerStart(void)
{	
}

void ZeroRTS::OnClientStart(void)
{
}

void ZeroRTS::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) {
		case FID_LOAD:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				pkConsole->Printf("load [mapname]");
				break;				
			}
			if(!pkLevelMan->LoadLevel(kCommand->m_kSplitCommand[1].c_str()))	
			{
				pkConsole->Printf("Error loading level");
				break;			
			}
			
			pkConsole->Printf("Level loaded");
			
			break;		
		
		case FID_UNLOAD:
			break;
	}
}











