#include "game.h"
#include "gamescript.h"
#include "gamescriptinterface.h"

Game g_kGame("ZeroFPS game",1024,768,24);

Game::Game(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) { }

void Game::OnInit() 
{
	m_pkInventoryBox = NULL;
	m_pkPlayer = NULL;
	Init();
	
	pkConsole->Printf(" ZeroFPS project game");
	pkConsole->Printf("--------------------------------");
	pkConsole->Printf(" Use load [map] to load a map");
	
	SetUpMenuScreen();			
}

static bool WINPROC( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams ) 
{
	return true; 
}

static bool INVENTORYPROC( ZGuiWnd* wnd, unsigned int msg, int num, void *parms ) {
	return g_kGame.m_pkInventoryBox->DlgProc(wnd,msg,num,parms); }


void Game::Init()
{
	//setup some default variables
	RegisterPropertys();

	//register commmands
	g_ZFObjSys.Register_Cmd("load",FID_LOAD,this);		
	g_ZFObjSys.Register_Cmd("unload",FID_UNLOAD,this);			
	
	//damn "#¤(="%#( lighting fix
	glEnable(GL_LIGHTING );
	
	//initiate our camera
	m_pkCamera=new Camera(Vector3(0,10,0),Vector3(0,0,0),90,1.333,0.25,250);	
	
	//disable zones modells
	pkLevelMan->SetVisibleZones(false);
	
	//set gamestate to menu
	m_iGameState=GAME_STATE_MENU;

	InitGui();
	InitScript();
	
	//run autoexec script
	if(!pkIni->ExecuteCommands("game_autoexec.ini"))
		pkConsole->Printf("No game_autoexec.ini.ini found");
}

void Game::OnServerStart(void)
{	
}

void Game::OnClientStart(void)
{
}



void Game::OnIdle(void) 
{
	
	switch(m_iGameState)
	{
		case GAME_STATE_MENU:
		{
			pkFps->SetCamera(m_pkCamera);		
			//pkFps->GetCam()->ClearViewPort();	
			//pkObjectMan->Update(PROPERTY_TYPE_RENDER, PROPERTY_SIDE_CLIENT, true);
			pkFps->DevPrintf("common", "Active Propertys: %d",pkObjectMan->GetActivePropertys());
	
			break;
		}
	
		case GAME_STATE_INGAME:
		{
			pkFps->SetCamera(m_pkCamera);		
			//pkFps->GetCam()->ClearViewPort();	
			//pkObjectMan->Update(PROPERTY_TYPE_RENDER, PROPERTY_SIDE_CLIENT, true);
			pkFps->DevPrintf("common","Active Propertys: %d",pkObjectMan->GetActivePropertys());			
			
			break;
		}
	}

/*	if(pkInput->Pressed(KEY_C)) {
		CameraProperty* pkCam = dynamic_cast<CameraProperty*>(m_pkPlayer->GetProperty("CameraProperty"));
		if(pkCam) {
			pkCam->NextType((CameraProperty::CamType_e) 0);
			}
		}*/

	Input();

}

void Game::OnHud(void) 
{	
	glPushAttrib(GL_LIGHTING_BIT);	
	glDisable(GL_LIGHTING);
	
	pkFps->DevPrintf("common", "Fps: %d",pkFps->m_iFps);	
			
			
	switch(m_iGameState)
	{
		case GAME_STATE_MENU:
		{
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
				
			pkRender->Quad(Vector3(0,0,-1),Vector3(0,0,0),Vector3(1.7,1,1),pkTexMan->Load("file:../data/textures/logo_menu.tga",0));		
					
			glDisable(GL_BLEND);	
		
			break;
		}

		case GAME_STATE_INGAME:
		{
//			pkRender->SetFont("file:../data/textures/text/console.tga");

			glAlphaFunc(GL_GREATER,0.3);
			glEnable(GL_ALPHA_TEST);

				pkRender->Quad(Vector3(0,0,-1),Vector3(0,0,0),Vector3(0.1,0.1,0),pkTexMan->Load("file:../data/textures/pointer.tga",0));	
//	pkRender->Quad(Vector3(.8,.8,-1),Vector3(0,0,m_pkPlayer->GetRot().y),Vector3(0.2,0.2,0.2),pkTexMan->Load("file:../data/textures/compas.tga",0));
	
			glDisable(GL_ALPHA_TEST);

			const int max_width = 100;
			float fMod = (float) pkGuiMan->Wnd("helthbar_bk")->GetScreenRect().Width() / max_width;

			float fHelth = *m_pfPlayerHealth;
			float fArmor = *m_pfPlayerArmor;

			if(fHelth < 0)
				fHelth = 0;
			if(fArmor < 0)
				fArmor = 0;

			pkGuiMan->Wnd("helthbar")->Resize((int)(fMod*fHelth),10);
			pkGuiMan->Wnd("armorbar")->Resize((int)(fMod*fArmor),10);

			pkFps->m_bGuiMode = false;
			pkFps->ToggleGui();
		}
	}

	glEnable(GL_LIGHTING);		
	glPopAttrib();
}


void Game::Input() 
{
	int iKey = pkInput->GetQueuedKey();

	switch(iKey)
	{
	case KEY_SPACE:
		{
			char szFile[] = "delete_closest_object.lua";
			if(!m_pkScript->RunScript(szFile))
				printf("Failed to run script %s.\n", szFile);
		}
		break;
	case KEY_I:
		// Open/Close inventory window
		if(m_pkInventoryBox->IsOpen() == false)
			m_pkInventoryBox->OnOpen(300,200); 
		else
			m_pkInventoryBox->OnClose(false);
		break;
	}


}

void Game::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) {
		case FID_LOAD:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("load [mapname]");
				break;				
			}
			if(!pkLevelMan->LoadLevel(kCommand->m_kSplitCommand[1].c_str()))	
			{
				pkConsole->Printf("Error loading level");
				break;			
			}		
			
			SetupLevel();
			
			pkConsole->Printf("Level loaded");
			
			break;		
		
		case FID_UNLOAD:
			SetUpMenuScreen();			
			break;
	}
}


void Game::RegisterPropertys()
{
	pkPropertyFactory->Register("DoorProperty", Create_DoorProperty);
	pkPropertyFactory->Register("ContainerProperty", Create_ContainerProperty);
	pkPropertyFactory->Register("ItemProperty", Create_ItemProperty);
	pkPropertyFactory->Register("TreeIdleProperty", Create_TreeIdleProperty);
	pkPropertyFactory->Register("AdaptorSniper", Create_AdaptorSniper);
	pkPropertyFactory->Register("StatusProperty", Create_StatusProperty);
	pkPropertyFactory->Register("TeleportProperty", Create_TeleportProperty);
	pkPropertyFactory->Register("MassDriverProperty", Create_MassDriverProperty);	
	pkPropertyFactory->Register("MachineGunProperty", Create_MachineGunProperty);		
	pkPropertyFactory->Register("MassDriverProjectile", Create_MassDriverProjectile);		
	pkPropertyFactory->Register("MachineGunProjectile", Create_MachineGunProjectile);			
}


void Game::SetUpMenuScreen()
{
	pkLevelMan->LoadLevel("adaptors");	
	
	m_pkCamera->GetPos().Set(18,7,-130);	
	m_pkCamera->GetRot().Set(21,360,0);	
	
	pkFps->m_pkAudioMan->LoadMusic("file:../data/music/theme.mp3");		
	pkFps->m_pkAudioMan->PlayMusic();

	m_iGameState=GAME_STATE_MENU;

}


void Game::SetupLevel()
{
	list<Object*> kObjects;		
	pkObjectMan->GetAllObjects(&kObjects);
	
	Object* po=NULL;
	
	for(list<Object*>::iterator it=kObjects.begin();it!=kObjects.end();it++) 
	{
		if((*it)->GetName() == "PLAYER_SPAWN_POINT")
		{
			
			//player
			m_pkPlayer=new PlayerObject(pkLevelMan->GetHeightMap(),pkInput);
			m_pkPlayer->GetPos() = (*it)->GetPos();
			m_pkPlayer->GetRot() = (*it)->GetRot();	
			//CameraProperty* pkCam = static_cast(m_pkPlayer->AddProperty(new CameraProperty()));
			CameraProperty* pkCam = dynamic_cast<CameraProperty*>(m_pkPlayer->GetProperty("CameraProperty"));
			pkCam->SetCamera(m_pkCamera);
			m_pkPlayer->AttachToClosestZone();	
			pkLevelMan->AddTracker(m_pkPlayer);

			m_pfPlayerHealth = &static_cast<StatusProperty*>(m_pkPlayer->GetProperty("StatusProperty"))->m_fHealth;
			m_pfPlayerArmor  = &static_cast<StatusProperty*>(m_pkPlayer->GetProperty("StatusProperty"))->m_fArmor;

			po=(*it);

			m_pkScript->ExposeVariable("player_pos_y", &m_pkPlayer->GetPos().y, tFLOAT);
		}		
	}
	
	if(po==NULL)
	{
		pkConsole->Printf("ERROR: Player spawn point is missing");
		return;
	}
	
	delete po;
	
	//start music	
	pkFps->m_pkAudioMan->LoadMusic("file:../data/music/fast.mp3");		
	pkFps->m_pkAudioMan->PlayMusic();	
	
	m_iGameState=GAME_STATE_INGAME;
}

///////////////////////////////////////////////////////////////////////////////
// Name: InitGUI
// Description:
//
void Game::InitGui()
{
	int id = 1;
	int x = m_iWidth-200, y = m_iHeight-200;

	ZGuiSkin* pkMainSkin     =	new ZGuiSkin(192, 192, 192, 0,   0,   0,   2);
	ZGuiSkin* pkHealthBkSkin =	new ZGuiSkin(255, 0,   0,   0,   0,   128, 4);
	ZGuiSkin* pkHealthSkin   =	new ZGuiSkin(0,   0,   255, 0,   0,   128, 4);
	ZGuiSkin* pkArmorBkSkin  =	new ZGuiSkin(255, 0,   0,   0,   0,   128, 4);
	ZGuiSkin* pkArmorSkin    =	new ZGuiSkin(0,   255, 0,   0,   0,   128, 4);

	ZGuiWnd* pkPlayerStatusMainWnd = new ZGuiWnd(Rect(x,y,x+190,y+190),NULL,true,id++);
	pkPlayerStatusMainWnd->SetSkin(pkMainSkin);

	x = 10; y = 10;
	ZGuiWnd* pkHelthbarBk = new ZGuiWnd(Rect(x,y,x+170,y+10),pkPlayerStatusMainWnd,true,id++);
	ZGuiWnd* pkHelthbar = new ZGuiWnd(Rect(0,0,170,10),pkHelthbarBk,true,id++);
	pkHelthbarBk->SetSkin(pkHealthBkSkin);
	pkHelthbar->SetSkin(pkHealthSkin);

	y += 20;
	ZGuiWnd* pkArmorbarBk = new ZGuiWnd(Rect(x,y,x+170,y+10),pkPlayerStatusMainWnd,true,id++);
	ZGuiWnd* pkArmorbar = new ZGuiWnd(Rect(0,0,170,10),pkArmorbarBk,true,id++);
	pkArmorbarBk->SetSkin(pkArmorBkSkin);
	pkArmorbar->SetSkin(pkArmorSkin);

	pkGui->AddMainWindow(id++, pkPlayerStatusMainWnd, "PlayerStatusMainWnd", WINPROC, true);

	pkGui->RegisterWindow(pkHelthbar, "helthbar");
	pkGui->RegisterWindow(pkArmorbar, "armorbar");
	pkGui->RegisterWindow(pkHelthbarBk, "helthbar_bk");
	pkGui->RegisterWindow(pkArmorbarBk, "armorbar_bk");

	// Create inventory window
	m_pkInventoryBox = new InventoryBox(pkGui, INVENTORYPROC);
	
	pkFps->m_bGuiTakeControl = false;
}

void Game::InitScript()
{
	m_pkScript = new GameScript();
	m_pkScript->ExposeObject("pkConsole", pkConsole, tConsole);

	m_pkGameScriptInterface = new GameScriptInterface();
	m_pkScript->ExposeObject("game", m_pkGameScriptInterface, tGame);
}
