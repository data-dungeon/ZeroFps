#include "game.h"

Game olle("ZeroFPS game",1024,768,16);

Game::Game(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) { }

void Game::OnInit() 
{
	m_pkPlayer = NULL;
	Init();
	
	pkConsole->Printf(" ZeroFPS project game");
	pkConsole->Printf("--------------------------------");
	pkConsole->Printf(" Use load [map] to load a map");
	
	SetUpMenuScreen();



}

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
	m_pkCamera=new Camera(Vector3(0,10,0),Vector3(0,0,0),85,1.333,0.25,250);	
	
	//disable zones modells
	pkLevelMan->SetVisibleZones(false);
	
	//set gamestate to menu
	m_iGameState=GAME_STATE_MENU;
}

void Game::OnServerStart(void)
{	
}

void Game::OnClientStart(void)
{
}



void Game::OnIdle(void) {
	switch(m_iGameState)
	{
		case GAME_STATE_MENU:
		{
			pkFps->SetCamera(m_pkCamera);		
			pkFps->GetCam()->ClearViewPort();	
			
			pkObjectMan->Update(PROPERTY_TYPE_RENDER, PROPERTY_SIDE_CLIENT, true);
			pkFps->DevPrintf("Active Propertys: %d",pkObjectMan->GetActivePropertys());
		
			
			break;
		}
	
		case GAME_STATE_INGAME:
		{
			pkFps->SetCamera(m_pkCamera);		
			pkFps->GetCam()->ClearViewPort();	
			
			pkObjectMan->Update(PROPERTY_TYPE_RENDER, PROPERTY_SIDE_CLIENT, true);
			pkFps->DevPrintf("Active Propertys: %d",pkObjectMan->GetActivePropertys());			
			
			break;
		}
	}

}

void Game::OnHud(void) 
{	
	glPushAttrib(GL_LIGHTING_BIT);	
	glDisable(GL_LIGHTING);
	
	pkFps->DevPrintf("Fps: %d",pkFps->m_iFps);	
			
			
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

				pkRender->Quad(Vector3(0,0,-1),Vector3(0,0,0),Vector3(0.2,0.2,0),pkTexMan->Load("file:../data/textures/pointer.tga",0));	
//	pkRender->Quad(Vector3(.8,.8,-1),Vector3(0,0,m_pkPlayer->GetRot().y),Vector3(0.2,0.2,0.2),pkTexMan->Load("file:../data/textures/compas.tga",0));
	
			glDisable(GL_ALPHA_TEST);
		}
	}

	glEnable(GL_LIGHTING);		
	glPopAttrib();
}


void Game::input() 
{
	cout << ";(" << endl;

		/*
	//Get mouse x,y		
	int x,z;		
	pkInput->RelMouseXY(x,z);

	//rotate the camera		
	pkFps->GetCam()->GetRot().x+=z/5.0;
	pkFps->GetCam()->GetRot().y+=x/5.0;
	*/
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
	pkPropertyFactory->Register("AdaptorSniper", Create_AdaptorSniper);
	pkPropertyFactory->Register("StatusProperty", Create_StatusProperty);
	pkPropertyFactory->Register("TeleportProperty", Create_TeleportProperty);
	pkPropertyFactory->Register("MassDriverProperty", Create_MassDriverProperty);	
	pkPropertyFactory->Register("MachineGunProperty", Create_MachineGunProperty);		
	pkPropertyFactory->Register("MassDriverProjectile", Create_MassDriverProjectile);		
	pkPropertyFactory->Register("MachineGunProjectile", Create_MassDriverProjectile);			
}


void Game::SetUpMenuScreen()
{
	pkLevelMan->LoadLevel("menu");

	m_pkCamera->GetPos().Set(36,19,18);	
	m_pkCamera->GetRot().Set(17,18,0);	
	
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
//			pkCollisionMan->Add(m_pkPlayer);			
			pkLevelMan->AddTracker(m_pkPlayer);
			
			po=(*it);
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






