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
		g_kMistClient.OnCommand(((int*)params)[0], win);
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
	
	m_fMaxCamDistance			= 8;
	m_fMinCamDistance			= 2;
	
	g_ZFObjSys.Log_Create("mistclient");

	m_bActionMenuIsOpen = false;

 
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
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.25,250);	
	
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
		"data/script/gui/gui_create_client.lua");

	//init mistland script intreface
	MistLandLua::Init(pkObjectMan,pkScript);
	
	pkGui->SetCursor(0,0, pkTexMan->Load("data/textures/gui/cursor.bmp", 0),
		pkTexMan->Load("data/textures/gui/cursor_a.bmp", 0), 32, 32);

	// hide cursor
	SDL_ShowCursor(SDL_DISABLE);

	// Zeb: Tar bort detta, det görs ju i OnServerStart!
	//pkObjectMan->CreateObjectFromScript("data/script/objects/t_player.lua");

	// Varde ljus!
	pkLight->SetLighting(true);
	//pkZShader->SetForceLighting(ALWAYS_OFF);   //GE FAN I DENNA "#¤%)"#?=&(?!"=#(&!?#¤%)! =)

	pkScript->Call(m_pkScriptResHandle, "CreateIntroScene", 0, 0);

	// Fulhack så länge för att kunna styra gui:t innan man har kopplat upp mot serven.
	pkFps->m_bClientMode = true;

	ZFResourceHandle kIpSetupScript;
	kIpSetupScript.SetRes("data/script/net/ipsetup.lua");
	pkScript->Call(&kIpSetupScript, "SetupIP", 0, 0);
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
}


void MistClient::OnIdle() 
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	
		
	if(pkGui->m_bHaveInputFocus == false)
	{
		Input();
	}
	
 	pkFps->UpdateCamera(); 	
	
/*	if(m_pkMap2) {
		m_pkMap2->SetPos(Vector3(0,0,0));
		//pkRender->DrawHM2(m_pkMap2,pkFps->GetCam()->GetPos());	
		}*/

	Vector3 pos = Get3DMousePos();
	pos+=pkFps->GetCam()->GetPos();
	
	pkRender->Line(pos,pos+Vector3(10,0,0));
	pkRender->Line(pos,pos+Vector3(0,10,0));	
	pkRender->Line(pos,pos+Vector3(0,0,10));	
	
	// FULHACK Tm Vim
		pkObjectMan->OwnerShip_Take( pkObjectMan->GetObjectByNetWorkID( pkFps->GetClientObjectID() ) );
	

	if(m_pkServerInfo)
	{
		pkFps->DevPrintf("client","ServerName: %s", m_pkServerInfo->m_sServerName.c_str());
		pkFps->DevPrintf("client","Players: %d", m_pkServerInfo->GetNrOfPlayers());
	
		PlayerInfo* pi = m_pkServerInfo->GetPlayerInfo(pkFps->GetConnectionID());
		if(pi)
			pkFps->DevPrintf("client","PlayerName: %s", pi->sPlayerName.c_str());		
	}	

}

void MistClient::OnSystem() 
{
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
				m_pkServerInfo = (P_ServerInfo*)pkServerI->GetProperty("P_ServerInfo");
				
			if(m_pkServerInfo)
			{
				pkConsole->Printf("Got server info");

				// Skapa spelar panelen
				if(GetWnd("IntroWnd") != NULL && GetWnd("IntroWnd")->IsVisible() )
				{
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
			
			//print server messages
			while(!pi->kMessages.empty())
			{
				PrintInfoBox(pi->kMessages.front().c_str());
				pi->kMessages.pop();
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
						order.m_iCaracter = m_iActiveCaracterObjectID;
						
						//set this to -1 if its not a ground click
						order.m_iFace = -1;
						
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
						
						order.m_sOrderName = "Move";
						order.m_iClientID = pkFps->GetConnectionID();
						order.m_iCaracter = m_iActiveCaracterObjectID;
						
						order.m_kPos = m_kTargetPos;
						order.m_iZoneObjectID = m_iTargetZoneObject;										
						order.m_iFace = m_iTargetFace;
						
						m_pkClientControlP->AddOrder(order);
					} 
				}			
			}
			m_fClickDelay = pkFps->GetTicks();					
		}
	}

	if(pkInput->Pressed(MOUSERIGHT))
	{
		m_pkTargetObject2 = GetTargetObject();

		pkGui->SetFocus(GetWnd("MainWnd")); // set focus to main wnd.

		int mx, my;
		pkInput->MouseXY(mx,my);
		OpenActionMenu(mx, my); 
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

void MistClient::OnServerClientJoin(ZFClient* pkClient,int iConID)
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

	return true; 
}
bool MistClient::IsValid()	{ return true; }


void MistClient::OnCommand(int iID, ZGuiWnd *pkMainWnd)
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

			/*		const int ANTAL = 5;

					ItemStats* pkTestItems = new ItemStats[ANTAL];

					typedef pair<EquipmentCategory, pair<string,string> > tInfo;

					tInfo info[ANTAL] =
					{
						tInfo(Weapon, pair<string, string>("dagger.bmp", "dagger_a.bmp")),
						tInfo(Weapon, pair<string, string>("dagger.bmp", "dagger_a.bmp")),
						tInfo(Belt, pair<string, string>("spellbook.bmp", "spellbook_a.bmp")),
						tInfo(Item, pair<string, string>("gembag1.bmp", "gembag1_a.bmp")),
						tInfo(Item, pair<string, string>("gembag1.bmp", "gembag1_a.bmp")),
					};

					vector<InventoryDlg::itItem> kItems;

					for( int i=0; i<ANTAL; i++)
					{
						pkTestItems[i].SetEquipmentCategory(info[i].first); 
						kItems.push_back( InventoryDlg::itItem(
							pair<string,string>(info[i].second.first, 
							info[i].second.second), &pkTestItems[i]) );
					}

					kItems[3].second->RegisterAsContainer(); 
					kItems[4].second->RegisterAsContainer(); 
					
					m_pkInventDlg->AddItems(kItems);	*/
				}
			}
			if(strClickWndName == "StatsButton")
				pkScript->Call(m_pkScriptResHandle, "OnClickStats", 0, 0);
			else
			if(strClickWndName == "MapButton")
				pkScript->Call(m_pkScriptResHandle, "OnClickMap", 0, 0);
			else
			if(strClickWndName == "ToggleInputBoxBn")
				pkScript->Call(m_pkScriptResHandle, "OnClickToggleInput", 0, 0);
			else
			if(strClickWndName == "ToggleInfoBoxBn")
				pkScript->Call(m_pkScriptResHandle, "OnClickToggleInfoBox", 0, 0);
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
				m_pkSelHenchmanIcon = m_vkHenchmanIcons[i];
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
							printf("m_pkClientControlP\n");
							if(m_pkTargetObject2)
							{
								printf("m_pkTargetObject2\n");
								if(m_iActiveCaracterObjectID != -1)
								{
									printf("m_iActiveCaracterObjectID != -1\n");

									ClientOrder order;

									order.m_sOrderName = res->second;
									order.m_iClientID = pkFps->GetConnectionID();
									order.m_iObjectID = m_pkTargetObject2->iNetWorkID;				
									order.m_iCaracter = m_iActiveCaracterObjectID;

									//set this to -1 if its not a ground click
									order.m_iFace = -1;

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
			
			map<string,string>::iterator itIPs = MistLandLua::g_kServerList.begin();
			for( ; itIPs != MistLandLua::g_kServerList.end(); itIPs++)
			{
				AddListItem("IPNumbersComboBox", (char*) itIPs->first.c_str());
			}		

			// Select the default ip name
//			if(static_cast<ZGuiCombobox*>(GetWnd(
//				"IPNumbersComboBox"))->GetListbox()->GetSelItem() == NULL)
			{
				map<string,string>::iterator itDef =
					MistLandLua::g_kServerList.find(MistLandLua::g_kDefServer);
				if(itDef != MistLandLua::g_kServerList.end())
				{
					ZGuiCombobox* pkCBox = static_cast<ZGuiCombobox*>(GetWnd("IPNumbersComboBox"));
					GetWnd("IPNumberEditbox")->SetText((char*) itDef->second.c_str());
					pkCBox->SetLabelText((char*) itDef->first.c_str());
				}
			}

		}
		else
		if(strClickWndName == "ConnectToServerButton")
		{
			char* szIpName = GetWnd("IPNumberEditbox")->GetText();
			pkFps->m_pkNetWork->ClientStart(szIpName);
			pkApp->OnClientStart();
		}
	}
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
	
	pkInput->UnitMouseXY(x,y);	
	dir.Set(x*xp,-y*yp,-1.5);
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

		map<string, string>::iterator itIPInfo;
		itIPInfo = MistLandLua::g_kServerList.find(string(szUser));

		if(itIPInfo != MistLandLua::g_kServerList.end())
		{
			GetWnd("IPNumberEditbox")->SetText((char*)itIPInfo->second.c_str());
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
						//disable enviroment for this caracter
						ep->SetEnable(false);						
						cout<<"disabled enviroment"<<endl;
					}
									
				}
				m_iActiveCaracter = -1;
				m_iActiveCaracterObjectID = -1;
			}
			
			//if the new caracter is valid
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
							P_Camera* cp = (P_Camera*)pkObj->AddProperty("P_Camera");
		
						if(cp)
						{
							cp->SetCamera(m_pkCamera);
							cp->SetType(CAM_TYPE3PERSON);								
							m_pkCamProp = cp;
							
							//set current active character
							m_iActiveCaracter = iCaracter;
							m_iActiveCaracterObjectID = id;
							cout<<"current caracter is: "<<m_iActiveCaracter<<endl;
						}
					
						if(ep)
						{
							//enable enviroment for this caracter
							ep->SetEnable(true);						
							cout<<"enabled enviroment"<<endl;
						}
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


void MistClient::PickUp()
{
	if(m_pkActiveCharacter)
	{
		CharacterProperty* cp = static_cast<CharacterProperty*>(
			m_pkActiveCharacter->GetProperty("P_CharStats"));

		if(cp)
		{
			CharacterStats* stats = cp->GetCharStats();

			if(stats)
			{
				map<string, Entity*>* vkEquipmentList = stats->GetEquippedList();

				int size = vkEquipmentList->size();

				printf("Num of objects in inventory = %i\n", size);

				map<string, Entity*>::iterator it;
				for( it=vkEquipmentList->begin(); it!=vkEquipmentList->end(); it++)
				{
					P_Item* pkItemProp = static_cast<P_Item*>(it->second->GetProperty("P_Item"));

					if(pkItemProp)
					{
						ItemStats* pkStats = pkItemProp->m_pkItemStats;

						if(pkStats)
							m_pkInventDlg->AddItem(pkStats);
					}
				}
			}
		}
	}
}

void MistClient::CreateGuiInterface()
{
	ZGuiSkin* pkSkin; 

	CreateWnd(Button, "ScrollPortraitsUp", "MainWnd", "", 800-51-9, 4, 8,8, 0);

	pkSkin = new ZGuiSkin(pkTexMan->Load("/data/textures/gui/scrollbar_clicktop_bn_u.bmp", 0), 0);
	static_cast<ZGuiButton*>(GetWnd("ScrollPortraitsUp"))->SetButtonUpSkin(pkSkin);

	pkSkin = new ZGuiSkin(pkTexMan->Load("/data/textures/gui/scrollbar_clicktop_bn_d.bmp", 0), 0);
	static_cast<ZGuiButton*>(GetWnd("ScrollPortraitsUp"))->SetButtonDownSkin(pkSkin);
	static_cast<ZGuiButton*>(GetWnd("ScrollPortraitsUp"))->SetButtonHighLightSkin(pkSkin);

	CreateWnd(Button, "ScrollPortraitsDown", "MainWnd", "", 800-51-9, 12, 8,8, 0);

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

	// tillfällig Fulhack delux (ta bort sen)
/*	pkFps->DevPrintf("common", "=)");
	pkFps->DevPrint_FindPage("common")->m_bVisible = true;
	pkFps->DevPrint_Show(true);*/
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
	string message = "(IM)";
	message+=szText;

	ClientOrder order;
						
	order.m_sOrderName = message; 
	order.m_iClientID = pkFps->GetConnectionID();
	order.m_iCaracter = m_iActiveCaracterObjectID;				
	order.m_iFace = -1;
						
	m_pkClientControlP->AddOrder(order);
}

void MistClient::OpenActionMenu(int mx, int my)
{
	if(m_bActionMenuIsOpen || m_pkTargetObject2 == NULL)
		return;

	P_Ml* pkMistLandProp = static_cast<P_Ml*>(m_pkTargetObject2->GetProperty("P_Ml")); 

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

	for(int i=0; i<MAX_NUM_ACTION_BUTTONS; i++)
	{
		sprintf(name, "%s%i", "ActionButton", i);
		bWndExist = GetWnd(name) != NULL;

		CreateWnd(Button, name, "MainWnd", "", mx-x_diff+x-fOffset, my-y_diff+y-fOffset, 32, 32, 0);
		ZGuiButton* pkButton = static_cast<ZGuiButton*>(GetWnd(name));

		if(!bWndExist)
		{
			ZGuiSkin* pkButtonSkins = new ZGuiSkin[3];
			pkButtonSkins[0].m_iBkTexID = pkTexMan->Load("data/textures/gui/actions/actionbutton_u.bmp", 0);
			pkButtonSkins[1].m_iBkTexID = pkTexMan->Load("data/textures/gui/actions/actionbutton_d.bmp", 0);
			pkButtonSkins[2].m_iBkTexID = pkTexMan->Load("data/textures/gui/actions/actionbutton_f.bmp", 0);

			pkButtonSkins[0].m_iBkTexAlphaID = pkTexMan->Load("data/textures/gui/actions/actionbutton_a.bmp", 0);
			pkButtonSkins[1].m_iBkTexAlphaID = pkTexMan->Load("data/textures/gui/actions/actionbutton_a.bmp", 0);
			pkButtonSkins[2].m_iBkTexAlphaID = pkTexMan->Load("data/textures/gui/actions/actionbutton_a.bmp", 0);

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

		sprintf(name, "%s%i", "ActionLabel", i);
		bWndExist = GetWnd(name) != NULL;

		CreateWnd(Label, name, "MainWnd", "", mx-x_diff+x-fOffset, my-y_diff+y-fOffset, 32, 32, 0);

		if(!bWndExist)
		{
			ZGuiSkin* pkActionSkin = new ZGuiSkin;
			pkActionSkin->m_iBkTexAlphaID = pkTexMan->Load("data/textures/gui/actions/action_a.bmp", 0);
			pkActionSkin->m_iBkTexID = pkTexMan->Load("data/textures/gui/actions/noaction.bmp", 0);
			GetWnd(name)->SetSkin(pkActionSkin);
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
			GetWnd(name)->GetSkin()->m_iBkTexID = pkTexMan->Load(szActionIcon, 0);

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

		sprintf(szName, "%s%i", "ActionLabel", i);
		pkWnd = GetWnd(szName);
		if(pkWnd) pkWnd->Hide();
	}

	m_bActionMenuIsOpen = false;
}
