#include "game.h"
#include "gamescript.h"
#include "gamescriptinterface.h"
#include "../../data/gui_resource_files/inventary_id.h"
#include "../../data/gui_resource_files/container_id.h"

Game g_kGame("ZeroFPS game",1024,768,24);

Game::Game(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) { }

void Game::OnInit() 
{
	m_pkExamineMenu = NULL;
	m_pkPlayerInventoryBox = NULL;
	m_pkContainerBox = NULL;
	m_pkPlayer = NULL;
	Init();
	
	pkConsole->Printf(" ZeroFPS project game");
	pkConsole->Printf("--------------------------------");
	pkConsole->Printf(" Use load [map] to load a map");
	
	SetUpMenuScreen();
	
	//run autoexec script
	if(!pkIni->ExecuteCommands("game_autoexec.ini"))
		pkConsole->Printf("No game_autoexec.ini.ini found");
}

static bool WINPROC( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams ) {
	return true; }

static bool PLAYER_INVENTORYPROC( ZGuiWnd* wnd, unsigned int msg, int num, void *parms ) 
{
	int mx,my;

	switch(msg)
	{
	case ZGM_COMMAND:
		switch(((int*)parms)[0]) // control id
		{
		case InventCloseBN:
			g_kGame.m_pkPlayerInventoryBox->OnClose(false);
			break;
		}
		break;

	case ZGM_RBUTTONUP:
		mx = ((int*)parms)[0];
		my = ((int*)parms)[1];

		Object* pkSelObject;
		pkSelObject = g_kGame.m_pkPlayerInventoryBox->GetItemObject(mx,my);

		if(pkSelObject)
			g_kGame.OpenExamineMenu(pkSelObject,INVENTORYUSE,mx,my);
		break;

	case ZGM_LBUTTONUP:
		if(g_kGame.DragAndDropItem(((int*)parms)[0],((int*)parms)[1],
			g_kGame.m_pkPlayerInventoryBox->m_pkMoveItem,
			g_kGame.m_pkPlayerInventoryBox,g_kGame.m_pkContainerBox))
			return true;
		break;

	}
	return g_kGame.m_pkPlayerInventoryBox->DlgProc(wnd,msg,num,parms); 
}

static bool CONTAINER_BOXPROC( ZGuiWnd* wnd, unsigned int msg, int num, void *parms ) 
{
	switch(msg)
	{
	case ZGM_COMMAND:
		switch(((int*)parms)[0]) // control id
		{
		case ContainerClose:
			g_kGame.m_pkContainerBox->OnClose(false);
			break;
		}
		break;

	case ZGM_LBUTTONUP:
		if(g_kGame.DragAndDropItem(((int*)parms)[0],((int*)parms)[1],
			g_kGame.m_pkContainerBox->m_pkMoveItem,
			g_kGame.m_pkContainerBox,g_kGame.m_pkPlayerInventoryBox))
			return true;
		break;
	}

	return g_kGame.m_pkContainerBox->DlgProc(wnd,msg,num,parms); 
}

static bool EXAMINE_BOXPROC( ZGuiWnd* wnd, unsigned int msg, int num, void *parms ) 
{
	switch(msg)
	{
	// Command Messages
	case ZGM_SELECTLISTITEM:
		char* szUseString;
		szUseString = g_kGame.m_pkExamineMenu->GetUseString(((int*)parms)[1]);
		
		if(szUseString && strcmp(szUseString, "Open container") == 0)
		{
			g_kGame.m_pkExamineMenu->OnClose(true);
			g_kGame.OpenContainer();
			return true;
		}
		break;
	}

	return g_kGame.m_pkExamineMenu->DlgProc(wnd,msg,num,parms); 
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
	m_pkCamera=new Camera(Vector3(0,10,0),Vector3(0,0,0),90,1.333,0.25,250);	
	
	//disable zones modells
	pkLevelMan->SetVisibleZones(false);
	
	//set gamestate to menu
	m_iGameState=GAME_STATE_MENU;

	InitGui();
	InitScript();
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
			pkFps->GetCam()->ClearViewPort();	
			//pkObjectMan->Update(PROPERTY_TYPE_RENDER, PROPERTY_SIDE_CLIENT, true);
			pkFps->DevPrintf("common", "Active Propertys: %d",pkObjectMan->GetActivePropertys());
	
			break;
		}
	
		case GAME_STATE_INGAME:
		{
			pkFps->SetCamera(m_pkCamera);		
			pkFps->GetCam()->ClearViewPort();	
			//pkObjectMan->Update(PROPERTY_TYPE_RENDER, PROPERTY_SIDE_CLIENT, true);

			pkFps->DevPrintf("common","Active Propertys: %d",pkObjectMan->GetActivePropertys());
			
			if(m_pkPlayerInventoryBox->IsOpen())
				m_pkPlayerInventoryBox->Update();

			if(m_pkContainerBox->IsOpen())
			{
				m_pkContainerBox->Update();
			}

			PlayerExamineObject();

			if(m_pkPlayerInventoryBox->IsOpen() || m_pkContainerBox->IsOpen() ||
				m_pkExamineMenu->IsOpen())
			{

				LockPlayerCamera(true);
			}
			else
				LockPlayerCamera(false);

			Input();

			break;
		}
	}

/*	if(pkInput->Pressed(KEY_C)) {
		CameraProperty* pkCam = dynamic_cast<CameraProperty*>(m_pkPlayer->GetProperty("CameraProperty"));
		if(pkCam) {
			pkCam->NextType((CameraProperty::CamType_e) 0);
			}
		}*/

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

	/*		const int max_width = 100;
			float fMod = (float) pkGuiMan->Wnd("helthbar_bk")->GetScreenRect().Width() / max_width;

			float fHelth = *m_pfPlayerHealth;
			float fArmor = *m_pfPlayerArmor;

			if(fHelth < 0)
				fHelth = 0;
			if(fArmor < 0)
				fArmor = 0;

			pkGuiMan->Wnd("helthbar")->Resize((int)(fMod*fHelth),10);
			pkGuiMan->Wnd("armorbar")->Resize((int)(fMod*fArmor),10);*/

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

	case KEY_H:
		{
			char szFile[] = "hulk_jump.lua";
			if(!m_pkScript->RunScript(szFile))
				printf("Failed to run script %s.\n", szFile);
		}
		break;
	
	case KEY_I:
		// Open/Close inventory window
		if(m_pkPlayerInventoryBox->IsOpen() == false)
			m_pkPlayerInventoryBox->OnOpen(-1,-1); 
		else
			m_pkPlayerInventoryBox->OnClose(false);
		break;

	case KEY_ESCAPE:
	
		if(m_pkExamineMenu->IsOpen())
			m_pkExamineMenu->OnClose(false);

		if(m_pkPlayerInventoryBox->IsOpen())
			m_pkPlayerInventoryBox->OnClose(false);
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
	pkPropertyFactory->Register("LogProperty", Create_LogProperty);
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
//	pkLevelMan->LoadLevel("adaptors");	
	
//	m_pkCamera->GetPos().Set(18,7,-130);	
//	m_pkCamera->GetRot().Set(21,360,0);	
	
//	pkFps->m_pkAudioMan->LoadMusic("file:../data/music/theme.mp3");		
//	pkFps->m_pkAudioMan->PlayMusic();

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

			// Set container pointer to itembox
			ContainerProperty* pkPlayerContainerProp = 
				static_cast<ContainerProperty*>(m_pkPlayer->GetProperty("ContainerProperty"));
			pkPlayerContainerProp->m_kContainer.SetSize(5,5);
			m_pkPlayerInventoryBox->SetContainer(&pkPlayerContainerProp->m_kContainer);

			// Set player control property pointer to itembox
			PlayerControlProperty* pkPlayerControlProperty = 
				static_cast<PlayerControlProperty*>(m_pkPlayer->GetProperty("PlayerControlProperty"));

			if(pkPlayerControlProperty)
				m_pkPlayerInventoryBox->SetPlayerControlProperty(pkPlayerControlProperty);
			else
				printf("failed to set player controller property to gui window!\n");
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
	int x = 1024-200, y = 768-200;

	ZGuiSkin* pkMainSkin     =	new ZGuiSkin(192, 192, 192, 0,   0,   0,   2);
	ZGuiSkin* pkHealthBkSkin =	new ZGuiSkin(255, 0,   0,   0,   0,   128, 4);
	ZGuiSkin* pkHealthSkin   =	new ZGuiSkin(0,   0,   255, 0,   0,   128, 4);
	ZGuiSkin* pkArmorBkSkin  =	new ZGuiSkin(255, 0,   0,   0,   0,   128, 4);
	ZGuiSkin* pkArmorSkin    =	new ZGuiSkin(0,   255, 0,   0,   0,   128, 4);

	ZGuiWnd* pkPlayerStatusMainWnd = new ZGuiWnd(Rect(x,y,x+190,y+190),NULL,true,id++);
	pkPlayerStatusMainWnd->SetSkin(pkMainSkin);
	pkPlayerStatusMainWnd->RemoveWindowFlag(WF_CANHAVEFOCUS);

	x = 10; y = 10;
	ZGuiWnd* pkHelthbarBk = new ZGuiWnd(Rect(x,y,x+170,y+10),pkPlayerStatusMainWnd,true,id++);
	ZGuiWnd* pkHelthbar = new ZGuiWnd(Rect(0,0,170,10),pkHelthbarBk,true,id++);
	pkHelthbarBk->SetSkin(pkHealthBkSkin);
	pkHelthbar->SetSkin(pkHealthSkin);
	pkHelthbar->RemoveWindowFlag(WF_CANHAVEFOCUS);
	pkHelthbarBk->RemoveWindowFlag(WF_CANHAVEFOCUS);

	y += 20;
	ZGuiWnd* pkArmorbarBk = new ZGuiWnd(Rect(x,y,x+170,y+10),pkPlayerStatusMainWnd,true,id++);
	ZGuiWnd* pkArmorbar = new ZGuiWnd(Rect(0,0,170,10),pkArmorbarBk,true,id++);
	pkArmorbarBk->SetSkin(pkArmorBkSkin);
	pkArmorbar->SetSkin(pkArmorSkin);
	pkArmorbar->RemoveWindowFlag(WF_CANHAVEFOCUS);
	pkArmorbarBk->RemoveWindowFlag(WF_CANHAVEFOCUS);

	pkGui->AddMainWindow(id++, pkPlayerStatusMainWnd, "PlayerStatusMainWnd", WINPROC, true);

	pkGui->RegisterWindow(pkHelthbar, "helthbar");
	pkGui->RegisterWindow(pkArmorbar, "armorbar");
	pkGui->RegisterWindow(pkHelthbarBk, "helthbar_bk");
	pkGui->RegisterWindow(pkArmorbarBk, "armorbar_bk");

	// Create inventory window
	m_iActionOpenInventory = pkInput->RegisterAction("inventory_open");
	m_iActionCloseInventory = pkInput->RegisterAction("inventory_close");

	m_pkPlayerInventoryBox = new ItemBox(pkGui, PLAYER_INVENTORYPROC, pkTexMan, pkInput);
	m_pkPlayerInventoryBox->Create(0,0,
		"../data/gui_resource_files/inventary_rc.txt", "InventoryWnd");

	// Create container windows
	m_pkContainerBox = new ItemBox(pkGui, CONTAINER_BOXPROC, pkTexMan, pkInput);
	m_pkContainerBox->Create(0,0,"../data/gui_resource_files/container_rc.txt",
		"ContainerWnd");

	// Create examine menu
	m_pkExamineMenu = new ExaminePUMenu(pkGui, pkInput, EXAMINE_BOXPROC, pkTexMan);
	m_pkExamineMenu->Create(0,0,NULL,NULL);
	
	pkFps->m_bGuiTakeControl = false;

	// Show cursor
	int cursor_tex = pkTexMan->Load("file:../data/textures/cursor.bmp", 0);
	int cursor_tex_a = pkTexMan->Load("file:../data/textures/cursor_a.bmp", 0);
	pkGui->SetCursor(cursor_tex, cursor_tex_a, 32, 32);
	pkGui->ShowCursor(false);
	SDL_ShowCursor(SDL_DISABLE);
}

void Game::InitScript()
{
	m_pkScript = new GameScript();
	m_pkScript->ExposeObject("pkConsole", pkConsole, tConsole);

	m_pkGameScriptInterface = new GameScriptInterface();
	m_pkScript->ExposeObject("game", m_pkGameScriptInterface, tGame);
}

void Game::PlayerExamineObject()
{
	PlayerControlProperty* pkPlayerCtrl = static_cast<PlayerControlProperty*>
			(m_pkPlayer->GetProperty("PlayerControlProperty"));

	// Om kameran är låst är gui't uppe och då skall 
	// det inte gå att undersöka object.
	if(pkPlayerCtrl->m_bLockCamera == true)
		return;

	Object* pkObjectTouched = pkPlayerCtrl->m_pkUseObject;

	if(pkObjectTouched != NULL)
	{
		ContainerProperty* cp = static_cast<ContainerProperty*>
			(pkObjectTouched->GetProperty("ContainerProperty"));

		if(cp != NULL)
		{
			if(cp->IsOpen() == false)
				m_pkContainerBox->SetContainer(&cp->m_kContainer);
		}

		OpenExamineMenu(pkObjectTouched, NORMALUSE);
	}
}

void Game::LockPlayerCamera(bool bLock)
{
	PlayerControlProperty* pkPlayerCtrl = static_cast<PlayerControlProperty*>
		(m_pkPlayer->GetProperty("PlayerControlProperty"));

	if(pkPlayerCtrl)
		pkPlayerCtrl->m_bLockCamera = bLock;

	pkGui->ShowCursor(bLock);
}

void Game::OpenContainer()
{
	if(m_pkContainerBox->GetContainer() != NULL)
	{
		m_pkContainerBox->OnOpen(m_iWidth-m_pkContainerBox->Width(),0);
	}
	else
	{
		printf("Failed to open container GUI box!\n");
	}
}

void Game::OpenExamineMenu(Object* pkObject, Action_Type eActionType, int x, int y)
{
	ItemProperty* ip = static_cast<ItemProperty*>
		(pkObject->GetProperty("ItemProperty"));

	if(ip != NULL)
	{
		PlayerControlProperty* pkPlayerCtrl = static_cast<PlayerControlProperty*>
			(m_pkPlayer->GetProperty("PlayerControlProperty"));

		m_pkExamineMenu->SetUseState(eActionType);
		m_pkExamineMenu->SetItemProperty(ip);
		m_pkExamineMenu->SetPlayerControlProperty(pkPlayerCtrl);

		if(m_pkExamineMenu->IsOpen() == false)
		{
			if(x == -1 && y== -1)
			{
				x = m_iWidth/2; 
				y = m_iHeight/2;
			}

			m_pkExamineMenu->OnOpen(x,y);
		}
	}
}

bool Game::DragAndDropItem(int mx, int my, ItemBox::slot* ppkMoveItem, 
			ItemBox* pkItemBoxFrom, ItemBox* pkItemBoxTo)
{
	pkInput->MouseXY(mx,my);

	Container* pkFrom = pkItemBoxFrom->GetContainer();
	Container* pkTo = pkItemBoxTo->GetContainer();

	if(ppkMoveItem == NULL || pkFrom == NULL || pkTo == NULL)
	{
		return false;
	}

	ZGuiWnd* pkMainWindowUnderCursor = pkGui->GetMainWindowFromPoint(mx,my);

	if(pkMainWindowUnderCursor != NULL)
	{
		if(pkItemBoxTo->GetWnd() == pkMainWindowUnderCursor)
		{
			int cell_from_x = ppkMoveItem->second.first;
			int cell_from_y = ppkMoveItem->second.second;

			ItemBox::slot_pos kCell_to = pkItemBoxTo->GetSlot(mx, my);

			if(kCell_to.first != -1 && kCell_to.second != -1)
			{
				Object* pkObjectToMove = pkFrom->GetItem(cell_from_x, cell_from_y);

				if(pkObjectToMove == NULL)
				{
					printf("Failed to move item from container!\n");
					pkItemBoxFrom->ResetMoveItem();
					pkGui->KillWndCapture();
					return false;
				}
				
				// Add object to destination container.
				if(!pkTo->AddItem(pkObjectToMove, kCell_to.first, kCell_to.second))
				{
					if(!pkTo->AddItem(pkObjectToMove))
					{
						printf("Failed to move item from container!\n");
						pkItemBoxFrom->ResetMoveItem(true);
						pkGui->KillWndCapture();
						return false;
					}
				}

				// Remove object from source container.
				pkFrom->RemoveItem(pkObjectToMove);
				pkItemBoxFrom->ResetMoveItem();
				
				pkGui->SetFocus(pkMainWindowUnderCursor);
			
				pkGui->KillWndCapture();

				return true;
			}
		}
	}

	return false;
}
