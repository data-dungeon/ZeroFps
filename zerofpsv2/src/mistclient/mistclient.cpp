/**
	\defgroup MistClient MistClient
	\ingroup MistLand

  MistClient is the Client of the game MistLands.
*/

#include "mistclient.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../zerofpsv2/engine_systems/common/heightmap.h"
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "../zerofpsv2/engine_systems/propertys/p_primitives3d.h"
#include "../zerofpsv2/engine_systems/propertys/p_track.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../mcommon/si_mistland.h"
#include "../zerofpsv2/basic/zguifont.h"
#include <typeinfo>
 
MistClient g_kMistClient("MistClient",0,0,0);

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
	switch(msg)
	{
	case ZGM_COMMAND:
		g_kMistClient.OnCommand(((int*)params)[0], (((int*)params)[1] == 1) ? true : false, win);
		break;

	case ZGM_LBUTTONDOWN:
		g_kMistClient.OnClick(((int*)params)[0], ((int*)params)[1], true, true, win);
		break;
	case ZGM_LBUTTONUP:
		g_kMistClient.OnClick(((int*)params)[0], ((int*)params)[1], false, true, win);
		break;
	case ZGM_RBUTTONDOWN:    
		g_kMistClient.OnClick(((int*)params)[0], ((int*)params)[1], true, false, win);
		break;
	case ZGM_RBUTTONUP:
		g_kMistClient.OnClick(((int*)params)[0], ((int*)params)[1], false, false, win);
		break;

	case ZGM_LBUTTONDBLCLK:
		g_kMistClient.OnDClick(((int*)params)[0], ((int*)params)[1], true, win);
		break;
	case ZGM_RBUTTONDBLCLK:
		g_kMistClient.OnDClick(((int*)params)[0], ((int*)params)[1], false, win);
		break;

	case ZGM_MOUSEMOVE:
		g_kMistClient.OnMouseMove(((int*)params)[1], ((int*)params)[2], 
			((int*)params)[0] == 1 ? true : false, win);
		break;

	case ZGM_SCROLL:
		g_kMistClient.OnScroll(((int*)params)[0], ((int*)params)[2], win);
		break;

	case ZGM_CBN_SELENDOK:
		g_kMistClient.OnSelectCB(((int*)params)[0], ((int*)params)[1], win);
		break;

	case ZGM_KEYPRESS:
		g_kMistClient.OnKeyPress(((int*)params)[0], win);		
		break;

	}
	return true;
}

MistClient::MistClient(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC), MAX_NUM_ACTION_BUTTONS(8)
{ 
	m_iActiveCaracterObjectID = -1;
	m_iActiveCaracter			= -1;
	m_iSelfObjectID			= -1;
	m_pkClientObject			= NULL;
	m_pkClientControlP		= NULL;
	m_pkServerInfo				= NULL;
	m_pkActiveCharacter		= NULL;
	m_pkTargetObject			= NULL;
	
	m_fMaxCamDistance			= 10;
	m_fMinCamDistance			= 4;

	m_bActionMenuIsOpen		= false;
	m_pkQuickBoard				= NULL;
	m_pkInventDlg				= NULL;
	m_pkSpellDlg				= NULL;
	m_pkSkillDlg				= NULL;
	
	g_ZFObjSys.Log_Create("mistclient");
} 

void MistClient::OnInit() 
{
	pkConsole->Printf(" MistClient , im scarred  =/");
	pkConsole->Printf("--------------------------------");
	pkConsole->Printf(" ugga?  blub?");

	Init();

	//run autoexec script
	if(!pkIni->ExecuteCommands("mistclient_autoexec.ini"))
		pkConsole->Printf("No game_autoexec.ini.ini found");
}

void MistClient::Init()
{	
	m_fClickDelay = pkFps->GetTicks();

	//register commmands bös
	Register_Cmd("load",FID_LOAD);		
	Register_Cmd("unload",FID_UNLOAD);			
	Register_Cmd("massspawn",FID_MASSSPAWN);			

	//damn "#¤(="%#( lighting fix bös
	glEnable(GL_LIGHTING );
	
	//initiate our camera bös
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.25,100);	
	
	//register actions bös
	RegisterActions();

	//register property bös
	RegisterPropertys();

	//register resources
	RegisterResources();

	// set caption
	SDL_WM_SetCaption("MistClient", NULL);
 
	// create gui script
	GuiAppLua::Init(&g_kMistClient, pkScript);

	HenchmanButton::s_iHenchemanAlphaTex = pkTexMan->Load("/data/textures/gui/portraits/portrait_a.bmp", 0);

	// init gui
	InitializeGui(pkGui, pkTexMan, pkScript, pkGuiMan, 
		"data/textures/text/paternoster8.bmp",
		"data/script/gui/gui_create_client.lua"
		/*"data/script/gui/test2.lua"*/);

	//init mistland script intreface
	MistLandLua::Init(pkObjectMan,pkScript);
	
	pkGui->SetCursor(0,0, pkTexMan->Load("data/textures/gui/cursor.bmp", 0),
		pkTexMan->Load("data/textures/gui/cursor_a.bmp", 0), 32, 32);

	// hide cursor
	SDL_ShowCursor(SDL_DISABLE);

	// Varde ljus!
	pkLight->SetLighting(true);
	//pkZShader->SetForceLighting(ALWAYS_OFF);   //GE FAN I DENNA "#¤%)"#?=&(?!"=#(&!?#¤%)! =)

	pkScript->Call(m_pkScriptResHandle, "CreateIntroScene", 0, 0);

	// Fulhack så länge för att kunna styra gui:t innan man har kopplat upp mot serven.
	pkFps->m_bClientMode = true;

	ZFResourceHandle kIpSetupScript;
	kIpSetupScript.SetRes("data/script/net/ipsetup.lua");
	pkScript->Call(&kIpSetupScript, "SetupIP", 0, 0);

/*	OggMusic* pkMusic = static_cast<OggMusic*>(g_ZFObjSys.GetObjectPtr("OggMusic"));
	pkMusic->LoadFile("data/music/ambient_loops/grotta3_fx_120bpm.ogg");
	pkMusic->Play();
*/

}

void MistClient::RegisterResources()
{
	pkResourceDB->RegisterResource( string(".env"), Create__EnvSetting	);
}

void MistClient::RegisterActions()
{
	m_iActionDoOrder=pkInput->RegisterAction("DoOrder");
	m_iActionPrintServerInfo=pkInput->RegisterAction("PrintServerInfo");
	m_iActionUnExploreAll=pkInput->RegisterAction("UnExploreAll");
	m_iActionExploreAll=pkInput->RegisterAction("ExploreAll");
	m_iActionCamLeft=pkInput->RegisterAction("CamLeft");
	m_iActionCamRight=pkInput->RegisterAction("CamRight");
	m_iActionCamUp=pkInput->RegisterAction("CamUp");
	m_iActionCamDown=pkInput->RegisterAction("CamDown");
	m_iActionSelect=pkInput->RegisterAction("Select");
	m_iActionScroll=pkInput->RegisterAction("Scroll");	
	m_iActionSelectManyModifier=pkInput->RegisterAction("SelectManyModifier");	
}

void MistClient::RegisterPropertys()
{
	pkPropertyFactory->Register("P_Enviroment", Create_P_Enviroment);
	pkPropertyFactory->Register("P_ClientControl", Create_P_ClientControl);
	pkPropertyFactory->Register("P_ServerInfo", Create_P_ServerInfo);
	pkPropertyFactory->Register("P_Ml", Create_P_Ml);
	pkPropertyFactory->Register("P_Event", Create_P_Event);
	pkPropertyFactory->Register("P_CharStats", Create_P_CharStats);
   pkPropertyFactory->Register("P_Item", Create_P_Item);
   pkPropertyFactory->Register("P_Container", Create_P_Container);
}


void MistClient::OnIdle() 
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	
		
	if(pkGui->m_bHaveInputFocus == false)
	{
		Input();
	}

   // update inventory dialog
   if ( m_pkInventDlg )
      m_pkInventDlg->Update();
	
 	pkFps->UpdateCamera(); 	
	
/*	if(m_pkMap2) {
		m_pkMap2->SetPos(Vector3(0,0,0));
		//pkRender->DrawHM2(m_pkMap2,pkFps->GetCam()->GetPos());	
		}*/

	Vector3 pos = Get3DMousePos();
	pos+=pkFps->GetCam()->GetPos();
	
/*	pkRender->Line(pos,pos+Vector3(10,0,0));
	pkRender->Line(pos,pos+Vector3(0,10,0));	
	pkRender->Line(pos,pos+Vector3(0,0,10));	
*/	
	// FULHACK Tm Vim
		pkObjectMan->OwnerShip_Take( pkObjectMan->GetObjectByNetWorkID( pkFps->GetClientObjectID() ) );
	

	if(m_pkServerInfo)
	{
		pkFps->DevPrintf("client","ServerName: %s", m_pkServerInfo->GetServerName().c_str());
		pkFps->DevPrintf("client","Players: %d", m_pkServerInfo->GetNrOfPlayers());
	
		PlayerInfo* pi = m_pkServerInfo->GetPlayerInfo(pkFps->GetConnectionID());
		if(pi)
			pkFps->DevPrintf("client","PlayerName: %s", pi->sPlayerName.c_str());		
	}	

	if(m_pkSkillDlg && m_pkSkillDlg->IsVisible())
		m_pkSkillDlg->Update();

	if(m_pkStatsDlg && m_pkStatsDlg->IsVisible())
		m_pkStatsDlg->Update();


	CharacterProperty* pkCharacterProperty = NULL;

	if(m_pkActiveCharacter)
	{
		pkCharacterProperty = static_cast<CharacterProperty*>(
			m_pkActiveCharacter->GetProperty("P_CharStats"));
	}

	if(pkCharacterProperty)
	{
		CharacterStats* pkCharacterStats = pkCharacterProperty->GetCharStats();
		
		UpdateManaAndHealthBar(pkCharacterStats);

		//pkCharacterProperty->GetProperty("P_ServerInfo")
	}

}

void MistClient::OnSystem() 
{
	UpdateCullObjects();

	//setup client
	if(pkFps->m_bClientMode && !pkFps->m_bServerMode)
	{
		GetSystem().Logf("net","??? m_iSelfObjectID %d\n", m_iSelfObjectID);

		if(!m_pkClientControlP)
		{
			m_iSelfObjectID = pkFps->GetClientObjectID();	
			if(m_iSelfObjectID != -1)
			{		
				m_pkClientObject = pkObjectMan->GetObjectByNetWorkID(m_iSelfObjectID);		
				if(m_pkClientObject)
				{			
					pkConsole->Printf("Got client object, Trying to get client control");
			
					m_pkClientControlP = (P_ClientControl*)m_pkClientObject->GetProperty("P_ClientControl");				
					if(m_pkClientControlP)
					{
						pkConsole->Printf("Got client control");				
					}				
				}
			}
		}
		
		if(!m_pkServerInfo)
		{
			Entity* pkServerI = pkObjectMan->GetObject("A t_serverinfo.lua");
			if(pkServerI)
			{
				m_pkServerInfo = (P_ServerInfo*)pkServerI->GetProperty("P_ServerInfo");
			}
				
			if(m_pkServerInfo)
			{
				pkConsole->Printf("Got server info");

				// Skapa spelar panelen
				if(GetWnd("IntroWnd") != NULL && GetWnd("IntroWnd")->IsVisible() )
				{
/*					pkMusic->LoadFile("data/music/ambient_loops/grotta3_fx_120bpm.ogg");
					pkMusic->Play();*/
					GetWnd("IntroWnd")->Hide();
					pkScript->Call(m_pkScriptResHandle, "CreatePlayerPanel", 0, 0);
					CreateGuiInterface();
				}

				printf("Connecting to server\n");
			}
		}
	};

	if(m_pkServerInfo)
	{		
		PlayerInfo* pi = m_pkServerInfo->GetPlayerInfo(pkFps->GetConnectionID());
		if(pi)
		{
/*				FAN ÄR DETTA FÖR DJÄVLA KRAP KÅD, INTE KONSTIGT ATT KAMERAN CPAR SIG IBLAND		

			int id = pi->kControl[m_iActiveCaracter].first;	
			Entity* pkObj = pkObjectMan->GetObjectByNetWorkID(id);
			
			//setup camera for active caracter
			if(pkObj)
			{
				P_Camera* cp = (P_Camera*)pkObj->GetProperty("P_Camera");
			
				if(!cp)
					P_Camera* cp = (P_Camera*)pkObj->AddProperty("P_Camera");
		
				if(cp)
				{
					cp->SetCamera(m_pkCamera);
					cp->SetType(CAM_TYPE3PERSON);
					m_pkCamProp = cp;
				}

				m_pkActiveCharacter = pkObj;
			}	
*/			
			//print server messages
			while(!m_pkServerInfo->m_kMyMessages.empty())
			{
				PrintInfoBox(m_pkServerInfo->m_kMyMessages.front().c_str());
				m_pkServerInfo->m_kMyMessages.pop();
			}

			//play private sounds
			while(!pi->kSounds.empty())
			{
				pair<int,string> sound_info = pi->kSounds.front();

				Entity* pkGenerator = pkObjectMan->GetObjectByNetWorkID(sound_info.first);

				if(pkGenerator)
				{
					Vector3 pos = pkGenerator->GetWorldPosV();
					Vector3 dir = pkGenerator->GetVel();

					pkAudioSys->StartSound(string("data/sound/") + 
						sound_info.second.c_str(), pos, dir, false);  
				}

				pi->kSounds.pop();
			}
			
		}else
			cout<<"cant find player object id"<<pkFps->GetConnectionID()<<endl;
	}

	SetActiveCaracter(0);
}

void MistClient::Input()
{
	float speed = 20;
	
	int x,z;		
	pkInput->RelMouseXY(x,z);	
		
	if(pkInput->Pressed(MOUSEMIDDLE))
	{

		if(m_pkCamProp)
		{
			m_fAngle +=x/100.0;
			m_fDistance += z/60.0;
	
			if(m_fDistance < m_fMinCamDistance)
				m_fDistance = m_fMinCamDistance;
		
			if(m_fDistance > m_fMaxCamDistance)
				m_fDistance = m_fMaxCamDistance;
				
			m_pkCamProp->Set3PYAngle(m_fAngle);
			m_pkCamProp->Set3PDistance(m_fDistance);
			m_pkCamProp->Set3PYPos(1.5);
		}
	}

	
	if(pkInput->Pressed(MOUSELEFT))
	{
		if(m_bActionMenuIsOpen) 
			CloseActionMenu();

		if(pkFps->GetTicks() - m_fClickDelay > 0.2)
		{	
			m_pkTargetObject = GetTargetObject();
			
			if(m_pkTargetObject)
			{
				P_Ml* pkMistLandProp = static_cast<P_Ml*>(m_pkTargetObject->GetProperty("P_Ml")); 

				if(pkMistLandProp)
				{
					vector<string> vkActionNames;
					pkMistLandProp->GetActions(vkActionNames);

					if(m_pkClientControlP && !vkActionNames.empty())
					{
						ClientOrder order;
						
						order.m_sOrderName = vkActionNames[0]; //"Klicka";
						order.m_iClientID = pkFps->GetConnectionID();
						order.m_iObjectID = m_pkTargetObject->iNetWorkID;				
						order.m_iCharacter = m_iActiveCaracterObjectID;
												
						m_pkClientControlP->AddOrder(order);
					} 
				}
			}
			else
			{
				if(PickZones())
				{
					if(m_pkClientControlP)
					{
						ClientOrder order;
						
						order.m_sOrderName = "G_Move";
						order.m_iClientID = pkFps->GetConnectionID();
						order.m_iCharacter = m_iActiveCaracterObjectID;
						
						order.m_kPos = m_kTargetPos;
						
						m_pkClientControlP->AddOrder(order);
					} 
				}			
			}
			m_fClickDelay = pkFps->GetTicks();					
		}
	}

	if(pkInput->Pressed(MOUSERIGHT)  && (pkInput->Pressed(KEY_RSHIFT) || pkInput->Pressed(KEY_LSHIFT)) )
	{
		//attack ground
		if(pkFps->GetTicks() - m_fClickDelay > 0.2)
		{	
			if(PickZones())
			{
				if(m_pkClientControlP)
				{
					ClientOrder order;
					
					order.m_sOrderName = "G_Attack";
					order.m_iClientID = pkFps->GetConnectionID();
					order.m_iCharacter = m_iActiveCaracterObjectID;
					
					order.m_kPos = m_kTargetPos;
					
					m_pkClientControlP->AddOrder(order);
				} 
			}						
	
			m_fClickDelay = pkFps->GetTicks();					
		}	
	}
	else if(pkInput->Pressed(MOUSERIGHT))			//if no shift is pressed bring up action menu
	{
		m_pkTargetObject = GetTargetObject();

		pkGui->SetFocus(GetWnd("MainWnd")); // set focus to main wnd.

		int mx, my;
		pkInput->MouseXY(mx,my);
		OpenActionMenu(mx, my); 
	}

	int pressed_key = pkInput->GetQueuedKey();

	if(pressed_key == KEY_F1)
	{
		int iNumSounds = pkAudioSys->GetNumSounds(); 
		int iNumActiveChannels = pkAudioSys->GetNumActiveChannels(); 

		printf("sounds =%i, channels =%i\n",  iNumSounds, iNumActiveChannels);
	}
	
	if(pressed_key == KEY_RETURN)
	{
		ZGuiWnd* pkInputWnd = GetWnd("InputWnd");

		if( pkInputWnd->IsVisible() == false)
		{
			pkAudioSys->StartSound("/data/sound/open_window2.wav",pkAudioSys->GetListnerPos());
			
			pkInputWnd->Show();
			pkGui->SetFocus(GetWnd("InputBox"));
		}
		else
		{		
			ZGuiWnd* pkTextbox = GetWnd("InputBox");

			pkAudioSys->StartSound("/data/sound/close_window2.wav",pkAudioSys->GetListnerPos());
			
			OnClientInputSend(pkTextbox->GetText());
			pkTextbox->SetText("");

			pkInputWnd->Hide();
			pkGui->SetFocus(GetWnd("PanelBkWnd")); // set focus to panel (very importent, crash if not)
		}
	}
}

void MistClient::OnHud(void) 
{	
	pkFps->DevPrintf("common","Active Propertys: %d",pkObjectMan->GetActivePropertys());	
	pkFps->DevPrintf("common", "Fps: %f",pkFps->m_fFps);	
	pkFps->DevPrintf("common","Avrage Fps: %f",pkFps->m_fAvrageFps);			
	pkFps->DevPrintf("common","SelfID: %d", m_iSelfObjectID);	
	
	
	pkFps->m_bGuiMode = false;
	pkFps->ToggleGui();

}

void MistClient::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) {
		case FID_LOAD:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				pkConsole->Printf("load [mapname]");
				break;				
			}
			
			cout<<"loading world:"<<kCommand->m_kSplitCommand[1].c_str()<<endl;
			
			if(!pkObjectMan->LoadWorld(kCommand->m_kSplitCommand[1].c_str()))
			{
				cout<<"Error loading world"<<endl;
				break;
			}
						
			cout<<"starting server"<<endl;
			GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);			
			
			break;	
		
		case FID_UNLOAD:
			break;
	
	}
}



void MistClient::ClientInit()
{
	cout<<"Client Join granted"<<endl;


	cout<<"Join Complete"<<endl;
	
}

void MistClient::OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass)
{
	cout<<"Client "<<iConID<<" Joined"<<endl;
	
	pkClient->m_pkObject->AddProperty("P_Primitives3D");	
	cout << "Now adding er to client" << endl;
}

void MistClient::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Parted"<<endl;	
	
}


void MistClient::OnServerStart(void)
{		
	m_pkTestobj = pkObjectMan->CreateObjectFromScript("data/script/objects/t_player.lua");
	if(m_pkTestobj)
	{
		m_pkTestobj->SetParent(pkObjectMan->GetWorldObject());
	
		m_pkTestobj->AddProperty("P_Track");	

		m_pkCamProp = (P_Camera*)m_pkTestobj->GetProperty("P_Camera");
		m_pkCamProp->SetCamera(m_pkCamera);
		m_pkCamProp->SetType(CAM_TYPE3PERSON);
		m_pkCamProp->Set3PDistance(m_fMinCamDistance);	
		m_pkCamProp->Set3PYPos(1.5);	
	
		m_pkTestobj->SetWorldPosV(Vector3(0,0.1,0));
		MistLandLua::g_iCurrentPCID = m_pkTestobj->iNetWorkID;

		// Skapa spelar panelen
		if(GetWnd("IntroWnd") != NULL && GetWnd("IntroWnd")->IsVisible() )
		{
			GetWnd("IntroWnd")->Hide();
			pkScript->Call(m_pkScriptResHandle, "CreatePlayerPanel", 0, 0);
			CreateGuiInterface();

			OggMusic* pkMusic = static_cast<OggMusic*>(g_ZFObjSys.GetObjectPtr("OggMusic"));
			pkMusic->Stop();
		}

		printf("Connecting to server\n");
	}


	if(pkObjectMan->GetNumOfZones() != 0) {
		pkConsole->Printf("Num of Zones: %d",pkObjectMan->GetNumOfZones());
	}
}

void MistClient::OnClientStart(void)
{
	pkConsole->Printf("Connected, Waiting for client object");
}

bool MistClient::StartUp()	{ return true; }
bool MistClient::ShutDown()	
{ 
	printf("MistClient::ShutDown\n"); 

	if(m_pkInventDlg)
		delete m_pkInventDlg;

	if(m_pkQuickBoard)
		delete m_pkQuickBoard;

	if(m_pkSpellDlg)
		delete m_pkSpellDlg;

	return true; 
}
bool MistClient::IsValid()	{ return true; }


void MistClient::OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd *pkMainWnd)
{
	ZGuiWnd* pkWndClicked = GetWnd(iID);

	if(pkWndClicked == NULL)
		return; 

	string strMainWndName = pkMainWnd->GetName();
	string strClickWndName = pkWndClicked->GetName();

	if(strMainWndName == "PanelBkWnd")
	{
		if(pkWndClicked)
		{
			if(strClickWndName == "BackPackButton")
			{
				bool bExist = GetWnd("BackPackWnd") != NULL;
				
				pkScript->Call(m_pkScriptResHandle, "OnClickBackpack", 0, 0);

				if(bExist == false)
				{
					m_pkInventDlg = new InventoryDlg(GetWnd("BackPackWnd"));

					// Set current container and main container
					if(m_pkActiveCharacter)
					{
                  P_Container* pkC = (P_Container*) m_pkActiveCharacter->GetProperty("P_Container");
						
                  if ( pkC )
                  {                  
						   m_pkInventDlg->SetCurrentContainer( pkC->m_iContainerID );
                     m_pkInventDlg->SetMainContainer( pkC->m_iContainerID );
                  }
                  else
                  {
                     cout << "Error! PlayerCharacter doesn't have any P_Container!! (MistClient::OnCommand)" << endl;
                     m_pkInventDlg->SetCurrentContainer( -1 );
                     m_pkInventDlg->SetMainContainer( -1 );
                  }

					}
				}

				// order itemcontainer to begin gather iteminfo from server
				if ( GetWnd("BackPackWnd")->IsVisible() )
				{
					pkGui->SetFocus(GetWnd("BackPackWnd"));

					if(m_pkActiveCharacter)
						((P_Container*)m_pkActiveCharacter->GetProperty("P_Container"))->
							GetAllItemsInContainer(m_pkInventDlg->m_pkAddItemList);  

				}
				else
					pkGui->SetFocus(GetWnd("PanelBkWnd"));

				// comment out by zeb
            /*CharacterProperty* pkCP = (CharacterProperty*)m_pkActiveCharacter->GetProperty("P_CharStats");

            if ( pkCP ) 
            {
               pkCP->RequestUpdateFromServer("skills");
               pkCP->RequestUpdateFromServer("data");
               pkCP->RequestUpdateFromServer("attributes");
            }
            else
               cout << "Errorrrrrrrrrrrrr!" << endl;*/
			}
			if(strClickWndName == "StatsButton")
			{
				bool bExist = GetWnd("StatsWnd") != NULL;

				pkScript->Call(m_pkScriptResHandle, "OnClickStats", 0, 0); // create

				if ( GetWnd("StatsWnd")->IsVisible() )
					pkGui->SetFocus(GetWnd("StatsWnd"));

				if(m_pkActiveCharacter)
					m_pkStatsDlg->SetCharacterProperty((CharacterProperty*)
						m_pkActiveCharacter->GetProperty("P_CharStats"));

				m_pkStatsDlg->ToogleOpen();
			}
			else
			if(strClickWndName == "MapButton")
			{
				pkScript->Call(m_pkScriptResHandle, "OnClickMap", 0, 0);

				if ( GetWnd("MapWnd")->IsVisible() )
					pkGui->SetFocus(GetWnd("MapWnd"));
			}
			else
			if(strClickWndName == "ToggleInputBoxBn")
			{
				pkScript->Call(m_pkScriptResHandle, "OnClickToggleInput", 0, 0);

				if ( GetWnd("InputWnd")->IsVisible() )
				{
					pkGui->SetFocus(GetWnd("InputBox"));
				}
			}
			else
			if(strClickWndName == "ToggleInfoBoxBn")
			{
				pkScript->Call(m_pkScriptResHandle, "OnClickToggleInfoBox", 0, 0);

				ZGuiWnd* pkQuickSlotMainWnd = GetWnd("QuickSlotMainWnd");

				if(GetWnd("InfoBoxWnd")->IsVisible() )
					pkQuickSlotMainWnd->SetPos(0, GetHeight()-21-48-85, true, true);
				else
					pkQuickSlotMainWnd->SetPos(0, GetHeight()-21-44, true, true); 	
			}
			else
			if(strClickWndName == "SelectSpellBn")
			{
				m_pkSpellDlg->ToogleOpen();
			}
			else
			if(strClickWndName == "SelectSkillBn")
			{
				if(m_pkActiveCharacter != NULL)
				{
					m_pkSkillDlg->SetCharacterProperty((CharacterProperty*)m_pkActiveCharacter->GetProperty("P_CharStats"));
				}
				else
				{
					printf("Failed to get character property! m_pkActiveCharacter = NULL\n");
				}

				m_pkSkillDlg->ToogleOpen();
			}
		}
	}
	else
	if(strMainWndName == "MainWnd")
	{
		unsigned int i;

		for(i=0; i<m_vkHenchmanIcons.size(); i++)
		{
			bool IsClicked = (m_vkHenchmanIcons[i]->GetName() == strClickWndName);
			m_vkHenchmanIcons[i]->Check(IsClicked);

			if(IsClicked)
			{
				m_pkSelHenchmanIcon = m_vkHenchmanIcons[i];
				SetActiveCaracter(i);
			}
		}

		char szActionButton[25];
		for(i=0; i<10; i++)
		{
			sprintf(szActionButton, "ActionButton%i", i);
			if(strClickWndName == szActionButton)
			{
				if(pkWndClicked)
				{
					map<ZGuiButton*,string>::iterator res =
						m_kActionBnTranslator.find(static_cast<ZGuiButton*>(pkWndClicked));

					if(res != m_kActionBnTranslator.end())
					{
						if(m_pkClientControlP)
						{
							if(m_pkTargetObject)
							{
								if(m_iActiveCaracterObjectID != -1)
								{
									ClientOrder order;

									order.m_sOrderName = res->second;
									order.m_iClientID = pkFps->GetConnectionID();
									
									order.m_iObjectID = m_pkTargetObject->iNetWorkID;				
									order.m_iCharacter = m_iActiveCaracterObjectID;


									m_pkClientControlP->AddOrder(order);

									printf("selected order = %s\n", res->second.c_str());
								}
							}
						}
					}
				}

				CloseActionMenu();
				break;
			}
		}
	}
	else
	if(strMainWndName == "InputWnd")
	{
		if(strClickWndName == "SendInputBoxBn")
		{
			ZGuiWnd* pkInputWnd = GetWnd("InputWnd");		
			pkInputWnd->Hide();
			pkAudioSys->StartSound("/data/sound/close_window2.wav",pkAudioSys->GetListnerPos());
			
			pkInputWnd = GetWnd("InputBox");			
			OnClientInputSend(pkInputWnd->GetText());
			pkInputWnd->SetText("");
		}
	}
	else
	if(strMainWndName == "IntroWnd")
	{
		if(strClickWndName == "OpenConnectButton")
		{
			pkScript->Call(m_pkScriptResHandle, "OnClickConnect",0,0);

			// Add server names
			ClearListbox("IPNumbersComboBox");

			int iNumRows = MistLandLua::g_kServerList.size();
			
			if(iNumRows > 10) 
				iNumRows = 10;

			static_cast<ZGuiCombobox*>(GetWnd("IPNumbersComboBox"))->SetNumVisibleRows(iNumRows);
			
			map<string,LogInInfo*>::iterator itIPs = MistLandLua::g_kServerList.begin();
			for( ; itIPs != MistLandLua::g_kServerList.end(); itIPs++)
			{
				AddListItem("IPNumbersComboBox", (char*) itIPs->first.c_str());
			}		

			// Select the default ip name
//			if(static_cast<ZGuiCombobox*>(GetWnd(
//				"IPNumbersComboBox"))->GetListbox()->GetSelItem() == NULL)
			{
				map<string,LogInInfo*>::iterator itDef =
					MistLandLua::g_kServerList.find(MistLandLua::g_kDefServer);
				if(itDef != MistLandLua::g_kServerList.end())
				{
					ZGuiCombobox* pkCBox = static_cast<ZGuiCombobox*>(GetWnd("IPNumbersComboBox"));
					GetWnd("IPNumberEditbox")->SetText((char*) itDef->second->acIP.c_str());
					pkCBox->SetLabelText((char*) itDef->first.c_str());
				}
			}

		}
		else
		if(strClickWndName == "ConnectToServerButton")
		{
			char* szIpName = GetWnd("IPNumberEditbox")->GetText();
			char* szLoginName = GetWnd("UserEditbox")->GetText();
			char* szPassName = GetWnd("PasswordEditbox")->GetText();
			pkFps->m_pkNetWork->ClientStart(szIpName, szLoginName, szPassName);
			pkApp->OnClientStart();
		}
	}
	else
	if(strMainWndName == "QuickSlotMainWnd")
	{
		m_pkQuickBoard->OnCommand(pkWndClicked, bRMouseBnClick);
	}
	else
	if(strMainWndName == "SpellBookMainWnd")
	{
		m_pkSpellDlg->OnCommand(pkWndClicked);
	}
	else
	if(strMainWndName == "SelectSkillsMainWnd")
	{
		m_pkSkillDlg->OnCommand(pkWndClicked);
	}
	else
	if(strMainWndName == "ContainerDlg")
		m_pkContainerDlg->OnCommand(iID);

	if(m_pkInventDlg)
	{
		m_pkInventDlg->OnCommand(iID); 
	}
}



Vector3 MistClient::Get3DMousePos()
{
	Vector3 dir;
	float x,y;		
	
	//screen propotions
	float xp=4;
	float yp=3;
	float fovshit=-2.15;
	
	pkInput->UnitMouseXY(x,y);	
	dir.Set(x*xp,-y*yp,fovshit);
	dir.Normalize();
	
	Matrix4 rm = m_pkCamera->GetRotM();
	rm.Transponse();
	dir = rm.VectorTransform(dir);
	
/*	Vector3 pos = (dir*1)+m_pkCamera->GetPos();//m_pkTestobj->GetLocalPosV();

	pkRender->Line(pos,pos+Vector3(2,0,0));
	pkRender->Line(pos,pos+Vector3(0,2,0));	
	pkRender->Line(pos,pos+Vector3(0,0,2));	
	pkRender->Sphere(pos,0.1,10,Vector3(0,1,0),false);
*/

	return dir;
}

Entity* MistClient::GetTargetObject()
{
	Vector3 start = m_pkCamera->GetPos();
	Vector3 dir = Get3DMousePos();

	vector<Entity*> kObjects;
	kObjects.clear();
	
	//pkObjectMan->TestLine(&kObjects,start,dir);
	
	pkObjectMan->GetZoneObject()->GetAllObjects(&kObjects);
	
	//cout<<"nr of targets: "<<kObjects.size()<<endl;
	
	float closest = 9999999999;
	Entity* pkClosest = NULL;	
	for(unsigned int i=0;i<kObjects.size();i++)
	{
		
		//objects that shuld not be clicked on (special cases)
		if(kObjects[i]->iNetWorkID <100000)
			continue;
		
		if(kObjects[i]->iNetWorkID == m_iActiveCaracterObjectID)
			continue;		
		
		if(kObjects[i]->GetName() == "ZoneObject")
			continue;
		
		if(kObjects[i]->GetName() == "StaticEntity")
			continue;		
		
		if(kObjects[i]->GetName() == "A t_serverinfo.lua")
			continue;		
		
		if(kObjects[i]->GetProperty("P_Ml")==NULL)
			continue;
		//-------------
		
		//get mad property and do a linetest
		P_Mad* mp = (P_Mad*)kObjects[i]->GetProperty("P_Mad");
		if(mp)
		{
			if(mp->TestLine(start,dir))
			{	
				float d = (start - kObjects[i]->GetWorldPosV()).Length();
	
				if(d < closest)
				{
					closest = d;
					pkClosest = kObjects[i];
				}				
			}
		}		
		
	}
	
	return pkClosest;
}

void MistClient::OnClick(int x, int y, bool bMouseDown, bool bLeftButton, ZGuiWnd *pkMain)
{
	if(pkMain == NULL) return;

	if(strcmp(pkMain->GetName(), "BackPackWnd") == 0)
		m_pkInventDlg->OnClick(x,y,bMouseDown,bLeftButton);
}

void MistClient::OnDClick(int x, int y, bool bLeftButton, ZGuiWnd *pkMain)
{
	if(pkMain == NULL) return;

	if(strcmp(pkMain->GetName(), "BackPackWnd") == 0)
		m_pkInventDlg->OnDClick(x,y,bLeftButton);
}

void MistClient::OnMouseMove(int x, int y, bool bMouseDown, ZGuiWnd *pkMain)
{
	if(pkMain == NULL) return;

	if(strcmp(pkMain->GetName(), "BackPackWnd") == 0)
		m_pkInventDlg->OnMouseMove(x,y,bMouseDown);	
}

void MistClient::OnScroll(int iID, int iPos, ZGuiWnd *pkMain)
{
	if(pkMain == NULL) return;

	if(strcmp(pkMain->GetName(), "BackPackWnd") == 0)
		m_pkInventDlg->OnScroll(iID,iPos);	
	else
	if(strcmp(pkMain->GetName(), "SelectSkillsMainWnd") == 0)
		m_pkSkillDlg->OnScroll(iID,iPos);	
	else
	if(strcmp(pkMain->GetName(), "ContainerDlg") == 0)
		m_pkContainerDlg->OnScroll(iID,iPos);	
}

void MistClient::OnSelectCB(int ListBoxID, int iItemIndex, ZGuiWnd *pkMain)
{
	if(pkMain == NULL) return;

	ZGuiCombobox* pkComboBox = NULL;
	list<ZGuiWnd*> childs;
	pkMain->GetChildrens(childs);

	list<ZGuiWnd*>::iterator it = childs.begin();
	for(; it != childs.end(); it++)
	{
		ZGuiWnd* pkWnd = (*it);
		if(pkWnd->GetID() == (unsigned int) ListBoxID)
		{
			if(typeid(*pkWnd)==typeid(ZGuiCombobox))
			{
				pkComboBox = static_cast<ZGuiCombobox*>(pkWnd);
				break;
			}
		}
	}

	if(pkComboBox != NULL)
	{
		char* szUser = pkComboBox->GetListbox()->GetSelItem()->GetText();

		map<string, LogInInfo*>::iterator itIPInfo;
		itIPInfo = MistLandLua::g_kServerList.find(string(szUser));

		if(itIPInfo != MistLandLua::g_kServerList.end())
		{
			GetWnd("IPNumberEditbox")->SetText((char*)itIPInfo->second->acIP.c_str());
			GetWnd("UserEditbox")->SetText((char*)itIPInfo->second->acUserName.c_str());
			GetWnd("PasswordEditbox")->SetText((char*)itIPInfo->second->acPassword.c_str());
		}
	}
}

void MistClient::SetActiveCaracter(int iCaracter)
{
	//dont try to set if caracter is already active
	if(m_iActiveCaracter == iCaracter)
		return;
		
	
	if(m_pkServerInfo)
	{		
		PlayerInfo* pi = m_pkServerInfo->GetPlayerInfo(pkFps->GetConnectionID());
		if(pi)
		{
			//if theres a current carater disable its camera
			if(m_iActiveCaracter != -1)
			{
				int id = pi->kControl[m_iActiveCaracter].first;	
				Entity* pkObj = pkObjectMan->GetObjectByNetWorkID(id);
			
				if(pkObj)
				{
					//disable camera on old active character 
					pkObj->DeleteProperty("P_Camera");
					m_pkCamProp = NULL;
				
					//get enviroment property for old active character
					P_Enviroment* ep = (P_Enviroment*)pkObj->GetProperty("P_Enviroment");
					if(ep)
					{
						//disable enviroment for this character
						ep->SetEnable(false);						
						cout<<"disabled enviroment"<<endl;
					}
									
				}
				m_iActiveCaracter = -1;
				m_iActiveCaracterObjectID = -1;
			}
			
			//if the new character is valid
			if(iCaracter != -1)
			{
				//check rights
				if(pi->kControl[iCaracter].second & PR_LOOKAT)
				{
					int id = pi->kControl[iCaracter].first;	
					Entity* pkObj = pkObjectMan->GetObjectByNetWorkID(id);
		
					//object found
					if(pkObj)
					{
						//get camera and enviroment propertys
						P_Camera* cp = (P_Camera*)pkObj->GetProperty("P_Camera");
						P_Enviroment* ep = (P_Enviroment*)pkObj->GetProperty("P_Enviroment");
				
						if(!cp) 
							cp = (P_Camera*)pkObj->AddProperty("P_Camera");
		
						//enable camera
						if(cp)
						{
							cp->SetCamera(m_pkCamera);
							cp->SetType(CAM_TYPE3PERSON);								
							m_pkCamProp = cp;
							
						}				
						
						//enable enviroment for this caracter
						if(ep)
						{
							ep->SetEnable(true);						
						}
						
						//set current active character
						m_iActiveCaracter = iCaracter;
						m_iActiveCaracterObjectID = id;


                     
                  // set active character in clientcontrol property also
                  if ( m_pkClientControlP )
                  	m_pkClientControlP->m_iActiveCaracterObjectID = id;
						
						cout<<"current character is: "<<m_iActiveCaracter<<endl;

						m_pkActiveCharacter = pkObj;
					}
				}		
			}
			
			static unsigned int s_iPrevNrOfObject = -1;

			if(s_iPrevNrOfObject != pi->kControl.size())
			{
				UpdateObjectList(pi);
			}
			
		}
		else
			cout<<"Error: cant find player info, You dont exist"<<endl;
	} 
}	

void MistClient::CreateGuiInterface()
{
	int screen_w = GetWidth();
	int screen_h = GetHeight();

	ZGuiSkin* pkSkin; 

	CreateWnd(Button, "ScrollPortraitsUp", "MainWnd", "", screen_w-51-9, 4, 8,8, 0);

	pkSkin = new ZGuiSkin(pkTexMan->Load("/data/textures/gui/scrollbar_clicktop_bn_u.bmp", 0), 0);
	static_cast<ZGuiButton*>(GetWnd("ScrollPortraitsUp"))->SetButtonUpSkin(pkSkin);

	pkSkin = new ZGuiSkin(pkTexMan->Load("/data/textures/gui/scrollbar_clicktop_bn_d.bmp", 0), 0);
	static_cast<ZGuiButton*>(GetWnd("ScrollPortraitsUp"))->SetButtonDownSkin(pkSkin);
	static_cast<ZGuiButton*>(GetWnd("ScrollPortraitsUp"))->SetButtonHighLightSkin(pkSkin);

	CreateWnd(Button, "ScrollPortraitsDown", "MainWnd", "", screen_w-51-9, 12, 8,8, 0);

	pkSkin = new ZGuiSkin(pkTexMan->Load("/data/textures/gui/scrollbar_clickbottom_bn_u.bmp", 0), 0);
	static_cast<ZGuiButton*>(GetWnd("ScrollPortraitsDown"))->SetButtonUpSkin(pkSkin);

	pkSkin = new ZGuiSkin(pkTexMan->Load("/data/textures/gui/scrollbar_clickbottom_bn_d.bmp", 0), 0);
	static_cast<ZGuiButton*>(GetWnd("ScrollPortraitsDown"))->SetButtonDownSkin(pkSkin);
	static_cast<ZGuiButton*>(GetWnd("ScrollPortraitsDown"))->SetButtonHighLightSkin(pkSkin);
	
	GetWnd("ScrollPortraitsUp")->Hide();
	GetWnd("ScrollPortraitsDown")->Hide();

	static_cast<ZGuiTextbox*>(GetWnd("InfoBox"))->SetReadOnly(true); 
	GetWnd("InfoBox")->GetSkin()->m_bTileBkSkin = true; 

	ZGuiFont* pkFont = new ZGuiFont(16,16,0,0);
	pkFont->CreateFromFile("/data/textures/text/small.bmp"); 
	GetWnd("InfoBox")->SetFont( pkFont);
	GetWnd("InputBox")->SetFont( pkFont);
	GetWnd("InfoBox")->GetFont()->m_cCharCellSize = 12; 	

	m_pkQuickBoard = new QuickBoard(this);
	
	m_pkSpellDlg = new SpellDlg(this, m_pkQuickBoard);
	m_pkSkillDlg = new SkillDlg(this, m_pkQuickBoard);
	m_pkStatsDlg = new StatsDlg(this);
	m_pkContainerDlg = new ContainerDlg(this);

	// give focus to main window
	pkGui->SetFocus(GetWnd("PanelBkWnd")); 

	// Init tooltip
	pkGui->GetToolTip()->SetToolTip(GetWnd("MapButton"),"Map");
	pkGui->GetToolTip()->SetToolTip(GetWnd("BackPackButton"),"Inventory");
	pkGui->GetToolTip()->SetToolTip(GetWnd("StatsButton"),"Character");
	pkGui->GetToolTip()->SetToolTip(GetWnd("SelectSpellBn"),"Spellbook");
	pkGui->GetToolTip()->SetToolTip(GetWnd("SelectSkillBn"),"Skills");
	pkGui->GetToolTip()->SetToolTip(GetWnd("ToggleInputBoxBn"),"Inputbox on/off");
	pkGui->GetToolTip()->SetToolTip(GetWnd("ToggleInfoBoxBn"),"Infobox on/off");
	
	ZGuiSkin kSkin(pkTexMan->Load("data/textures/gui/sb_bk.bmp", 0), true);
	kSkin.m_unBorderSize = 1;
	memset(kSkin.m_afBorderColor, 0, sizeof(float)*3);
	pkGui->GetToolTip()->SetSkin(kSkin);
}

void MistClient::UpdateObjectList(PlayerInfo* pkPlayerInfo)
{
	int iNumHenchmans = pkPlayerInfo->kControl.size();
	int iNumHenchmanIcons = m_vkHenchmanIcons.size();

	if(iNumHenchmanIcons < iNumHenchmans)
	{
		m_vkHenchmanIcons.push_back( new HenchmanButton(this, 
			pkTexMan->Load("/data/textures/gui/portraits/gubbe1.bmp", 0), iNumHenchmanIcons) );

		m_vkHenchmanIcons.back()->Check(true);  
	}
}

void MistClient::PrintInfoBox(const char *c_szText)
{
	if(!c_szText)
		return;
	
	ZGuiTextbox* pkInfoBoxWnd = static_cast<ZGuiTextbox*>(GetWnd("InfoBox"));

	string strText = pkInfoBoxWnd->GetText();
	int rows = pkInfoBoxWnd->GetRowCount();

	if(rows > 20)
	{
		int offset_to_first_linebreak = strText.find("\n");

		if(offset_to_first_linebreak > string::npos || offset_to_first_linebreak > 60)
			offset_to_first_linebreak = 60;

		strText.erase( 0, offset_to_first_linebreak);
	}
	
	strText.append(c_szText);
	strText.append("\n");

	pkInfoBoxWnd->SetText((char*)strText.c_str());
	pkInfoBoxWnd->ScrollRowIntoView(rows-3);
}


bool MistClient::PickZones()
{
	Vector3 start = m_pkCamera->GetPos();
	Vector3 dir = Get3DMousePos();
	
	vector<Entity*> kObjects;	
	pkObjectMan->GetZoneObject()->GetAllObjects(&kObjects);
		
	float closest = 9999999999;
	
	for(unsigned int i=0;i<kObjects.size();i++)
	{
		if(kObjects[i]->GetName() == "ZoneObject")
		{
			P_Mad* mp = (P_Mad*)kObjects[i]->GetProperty("P_Mad");
			if(mp)
			{
				if(mp->TestLine(start,dir))
				{	
					float d = (start - kObjects[i]->GetWorldPosV()).Length();
	
					if(d < closest)
					{
						closest = d;										
					
						m_iTargetZoneObject = kObjects[i]->iNetWorkID;
						m_iTargetFace = mp->GetLastColFace();
						m_kTargetPos = mp->GetLastColPos();
					
						pkRender->Sphere(mp->GetLastColPos(),0.1,4,Vector3(1,0.5,1),false);
					}						
				}				
			}
		}
	}
	
	if(closest != 9999999999)
		return true;
	
	return false;
	//cout<<"nr of zones picked:"<<iNrOfZones<<endl;
}

void MistClient::OnClientInputSend(char *szText)
{
	if(strcmp(szText,"") == 0)
		return;

	string message = "(IM)";
	message+=szText;

	ClientOrder order;
						
	order.m_sOrderName = message; 
	order.m_iClientID = pkFps->GetConnectionID();
	order.m_iCharacter = m_iActiveCaracterObjectID;				
						
	m_pkClientControlP->AddOrder(order);
}

void MistClient::OpenActionMenu(int mx, int my)
{
	if(m_bActionMenuIsOpen || m_pkTargetObject == NULL)
		return;

	P_Ml* pkMistLandProp = static_cast<P_Ml*>(m_pkTargetObject->GetProperty("P_Ml")); 

	if(!pkMistLandProp)
		return;

	vector<string> vkActions;
	pkMistLandProp->GetActions(vkActions);

	int x=0, y=0, knappar=vkActions.size();
	float grad=-PI/2;
	char name[25];
	bool bWndExist;
	const float fOffset = 35;
	int x_diff = -8, y_diff = 20;

	for(unsigned int i=0; i<MAX_NUM_ACTION_BUTTONS; i++)
	{
		sprintf(name, "%s%i", "ActionButton", i);
		bWndExist = GetWnd(name) != NULL;

		CreateWnd(Button, name, "MainWnd", "", mx-x_diff+x-fOffset, my-y_diff+y-fOffset, 32, 32, 0);
		ZGuiButton* pkButton = static_cast<ZGuiButton*>(GetWnd(name));

		if(!bWndExist)
		{
			ZGuiSkin* pkButtonSkins = new ZGuiSkin[3];

			pkButtonSkins[0].m_iBkTexAlphaID = pkTexMan->Load("data/textures/gui/actions/action_a.bmp", 0);
			pkButtonSkins[1].m_iBkTexAlphaID = pkTexMan->Load("data/textures/gui/actions/action_a.bmp", 0);
			pkButtonSkins[2].m_iBkTexAlphaID = pkTexMan->Load("data/textures/gui/actions/action_a.bmp", 0);

			pkButtonSkins[2].m_afBkColor[0] = 0.5f;
			pkButtonSkins[2].m_afBkColor[1] = 0.5f;
			pkButtonSkins[2].m_afBkColor[2] = 1;

			pkButtonSkins[1].m_afBkColor[0] = 0;
			pkButtonSkins[1].m_afBkColor[1] = 0;
			pkButtonSkins[1].m_afBkColor[2] = 1;

			pkButton->SetButtonUpSkin(&pkButtonSkins[0]);
			pkButton->SetButtonDownSkin(&pkButtonSkins[1]);
			pkButton->SetButtonHighLightSkin(&pkButtonSkins[2]);
		}
		else
		{
			ZGuiWnd* pkWnd = GetWnd(name);
			pkWnd->Show();
			pkWnd->SetPos(mx+x-fOffset-x_diff, my+y-fOffset-y_diff, true, true);
		}

		if(i < vkActions.size())
		{
			char szActionIcon[128];
			sprintf(szActionIcon, "data/textures/gui/actions/%s.bmp", vkActions[i].c_str());
			((ZGuiButton*) GetWnd(name))->GetButtonUpSkin()->m_iBkTexID = pkTexMan->Load(szActionIcon, 0);
			((ZGuiButton*) GetWnd(name))->GetButtonDownSkin()->m_iBkTexID = pkTexMan->Load(szActionIcon, 0);
			((ZGuiButton*) GetWnd(name))->GetButtonHighLightSkin()->m_iBkTexID = pkTexMan->Load(szActionIcon, 0);

			map<ZGuiButton*,string>::iterator res =
				m_kActionBnTranslator.find(pkButton);

			if(res != m_kActionBnTranslator.end()) // om den fanns, ta bort den gamla först
				m_kActionBnTranslator.erase(res); 
	
			m_kActionBnTranslator.insert(
				map<ZGuiButton*,string>::value_type(pkButton,vkActions[i]));
		}
		else
		{
			pkButton->Hide();
			GetWnd(name)->GetSkin()->m_iBkTexID = 
				pkTexMan->Load("data/textures/gui/actions/noaction.bmp", 0);
		}

		x -= sin(grad) * fOffset;
		y -= cos(grad) * fOffset;

		grad -= (zf_pi / (float) MAX_NUM_ACTION_BUTTONS);
	}

	m_bActionMenuIsOpen = true;
}

void MistClient::CloseActionMenu()
{
	ZGuiWnd* pkWnd;
	char szName[25];
	for(int i=0; i<MAX_NUM_ACTION_BUTTONS; i++)
	{
		sprintf(szName, "%s%i", "ActionButton", i);
		pkWnd = GetWnd(szName);
		if(pkWnd) pkWnd->Hide();
	}

	m_bActionMenuIsOpen = false;
}


void MistClient::UpdateCullObjects()
{
	static vector<P_Mad*> mads;	
	static float fAddRadius = 1;
	
	Entity* pkCar = pkObjectMan->GetObjectByNetWorkID(m_iActiveCaracterObjectID);
	
	if(!pkCar)
		return;
		
	Vector3 kStart = m_pkCamera->GetPos();
	Vector3 kDir = pkCar->GetWorldPosV() - m_pkCamera->GetPos();

   //if ( kDir != Vector3::ZERO )
      kDir.Unit();	
   //else
   //   return;
	
	float d = (pkCar->GetWorldPosV() - kStart).Length() + fAddRadius ;

	vector<Entity*> kEntitys;
	pkObjectMan->TestLine(&kEntitys,kStart,kDir);
	
	int i;

	//make all old objects visible again
	for(i =0;i<mads.size();i++)
	{
		mads[i]->SetVisible(true);
	}
	
	//clear old object
	mads.clear();
	
	//find objects to hide
	for(i=0;i<kEntitys.size();i++)
	{
		//kEntitys[i]->DeleteProperty("P_Mad");
		P_Mad* pm = (P_Mad*)kEntitys[i]->GetProperty("P_Mad");
		
		if(pm)
		{
			//do we realy want to make this object invisible?
			if(!pm->m_bCanBeInvisible)
				continue;
		
			//check if object is behind player
			if((kEntitys[i]->GetWorldPosV() - kStart).Length() < d)
			{
				//hide and add to vector
				pm->SetVisible(false);
				mads.push_back(pm);
			}
		}		
	}
	
//	cout<<"Nrof object betwen camera and player:"<<mads.size()<<endl;
		
}

void MistClient::OnKeyPress(int iKey, ZGuiWnd *pkWnd)
{
	switch(iKey)
	{
	case KEY_ESCAPE:
		{
			if(pkWnd && pkWnd->IsVisible()  )
			{
				if(strcmp(pkWnd->GetName(), "StatsWnd") == 0)
				{
					pkWnd->Hide();
					pkGui->SetFocus(GetWnd("MainWnd")); 
					pkAudioSys->StartSound("/data/sound/close_window.wav",pkAudioSys->GetListnerPos()); 
				}
			}
		}
		break;

	case KEY_SPACE:
		m_pkContainerDlg->ToggleOpen(!m_pkContainerDlg->IsOpen());
		break;
	}

}

void MistClient::UpdateManaAndHealthBar(CharacterStats* pkCharacterStats)
{
	static float max_size = ((float)GetWidth()-15.0f)/2.0f;

	// update life meter
	float fHP = pkCharacterStats->GetHPPercent();
	
	if(fHP >= 0 && fHP <= 1)
		GetWnd("LifeBarProgress")->Resize((int) (max_size*fHP), 8);

	// update mana meter
	float fMP = pkCharacterStats->GetMPPercent();

	if(fMP >= 0 && fMP <= 1)
		GetWnd("ManaBarProgress")->Resize((int) (max_size*fMP), 8);
}
