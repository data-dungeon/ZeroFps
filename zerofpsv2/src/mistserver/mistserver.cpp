/**
	\defgroup MistServer MistServer
	\ingroup MistLand

  MistServer is the Server of the game MistLands.
*/ 
 
  
#include "mistserver.h"
#include "../zerofpsv2/engine/p_pfpath.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_objectmanager.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../mcommon/si_mistland.h"
#include "../mcommon/ml_netmessages.h"
#include "../mcommon/zssmltime.h"
#include "../mcommon/zssenviroment.h"
#include "../mcommon/p_characterproperty.h"
#include "../mcommon/p_container.h"
#include "../mcommon/p_item.h"

#include "../zerofpsv2/engine_systems/propertys/p_track.h"
#include "../zerofpsv2/engine_systems/tcs/tcs.h"
#include "../zerofpsv2/engine/inputhandle.h"

#include "../mcommon/mainmcommon.h"



using namespace ObjectManagerLua;
 
MistServer g_kMistServer("MistServer", 0, 0, 0);
Entity* pkEntTestArc;


static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
	return true;
}


MistServer::MistServer(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	g_ZFObjSys.SetPreLogName("mistserver");
	g_ZFObjSys.Log_Create("mistserver");

	m_pkEnviroment =	new ZSSEnviroment;
	m_pkTime = 			new ZSSMLTime;
	
	// Set Default values
	m_AcceptNewLogins = true;


	// Register Variables
	RegisterVariable("ap_newlogins",				&m_AcceptNewLogins,			CSYS_BOOL);	

	m_bStartMinimized = false;
	RegisterVariable("ap_startminimized", 	&m_bStartMinimized, CSYS_BOOL);

	// Register Commands
	Register_Cmd("new",			FID_NEW);		
	Register_Cmd("load",			FID_LOAD);		
	Register_Cmd("save",			FID_SAVE);
	Register_Cmd("saveas",		FID_SAVEAS);
	Register_Cmd("users",		FID_USERS);		
	Register_Cmd("lo",			FID_LOCALORDER);		
	Register_Cmd("lightmode",	FID_LIGHTMODE);		
	Register_Cmd("jiddra",		FID_TEST_JIDDRA);
	Register_Cmd("say",			FID_SAY);

	m_strWorldDir  = "";
   
	m_pkActiveCameraObject	= NULL;
	m_pkActiveCamera			= NULL;
	
	m_pkServerInfoP			= NULL;
} 


void MistServer::CreateEditCameras()
{
	m_pkActiveCameraObject = m_pkEntityManager->CreateEntityFromScript("data/script/objects/cameraedit.lua");
	if(m_pkActiveCameraObject) 
	{
		m_pkActiveCameraObject->SetParent( m_pkEntityManager->GetWorldEntity() );
		m_pkActiveCameraObject->SetSave(false);
		
		P_Camera* m_pkCamProp = (P_Camera*)m_pkActiveCameraObject->GetProperty("P_Camera");
		m_pkCamProp->SetCamera(m_pkCamera);
		
		
/*		P_Enviroment* pe = (P_Enviroment*)m_pkActiveCameraObject->AddProperty("P_Enviroment");
		pe->SetEnable(true);		
		pe->SetEnviroment("data/enviroments/server.env");
*/
	}

/* mega �er super evil
	GetWnd("vp1")->SetRenderTarget(m_pkCamera);
	GetWnd("vp1")->SetMoveArea(Rect(0,0,800,600), false);
*/

	m_pkActiveCamera = m_pkCamera;
	m_pkActiveCamera->SetSelected(true);

	m_pkZeroFps->AddRenderCamera(m_pkCamera);
	m_pkCamera->m_bForceFullScreen = true;
	
	
	if(m_pkActiveCameraObject->GetUseZones())
		cout<<"using zones"<<endl;
	else
		cout<<"not using zones"<<endl;
	
}


void MistServer::OnInit() 
{
	m_pkZFVFileSystem->AddRootPath( string("../datafiles/mistlands") ,"/data");
	//m_pkZFVFileSystem->AddRootPath( string("../datafiles/dm") ,"/data");
	
	m_pkConsole->Printf(" MistServer (mistland dedicated server)");
	m_pkConsole->Printf("--------------------------------");
	m_pkConsole->Printf("");

	Init();

	//init dm script interface (register script functions for gameplay)
	//DMLua::Init(m_pkEntityManager,m_pkScript,m_pkGuiMan);

	//run autoexec script
	if(!m_pkIni->ExecuteCommands("mistserver_autoexec.ini"))
		m_pkConsole->Printf("No mistserver_autoexec.ini found");
}

void MistServer::Init()
{	
	//create player database
	m_pkPlayerDB = new PlayerDatabase();

	//m_pkZShaderSystem->SetForceLighting(LIGHT_ALWAYS_OFF);	
	//m_pkZeroFps->SetSystemFps(30);
	//m_pkZeroFps->SetNetworkFps(15);
	//m_pkNetwork->SetNetSpeed(4000);	
	
	//set tracker los
	m_pkEntityManager->SetTrackerLos(25);
	
	
	//setup physics
	m_pkTcs->SetGravity(15);
	m_pkTcs->SetColIt(2);
	m_pkTcs->SetConIt(2);	
	
	//system ant network
	m_pkZeroFps->SetSystemFps(20);
	m_pkZeroFps->SetNetworkFps(15);
	m_pkZeroFps->SetSyncNetwork(false);
	
	//enable debug graphics
	m_pkZeroFps->SetDebugGraph(true);
	
	//register script functions
	RegisterScriptFunctions();
	
	//register property b�
	RegisterPropertys();

	//register resources
	RegisterResources();

	//initiate our camera
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.25,250);	
	m_pkCamera->SetName("persp");

	//init mistland script intreface
	MistLandLua::Init(m_pkEntityManager,m_pkScript);
	
	// create gui script funktions
	GuiAppLua::Init(&g_kMistServer, m_pkScript);

	// Load default texture and create default font and menu (NULL = No menu).
	InitGui(m_pkScript, "defguifont", "data/script/gui/defskins.lua", NULL, true, AUTO_SCALE); 

	// Create startup GUI for the the server from script.
	SetupGuiEnviroment();

	//setup caption
	SetTitle("MistServer");
	// hide cursor
	m_pkInput->ShowCursor(true);

	// Setup the Edit Sun that are used for simple lightning in the editor.
	m_kSun.kRot = Vector3(2,2,1);
	m_kSun.kDiffuse=Vector4(0.5,0.5,0.5,0);
	m_kSun.kAmbient=Vector4(0.5,0.5,0.5,0);
	m_kSun.iType=DIRECTIONAL_LIGHT;			
	m_pkLight->Add(&m_kSun);
			
//	m_pkInputHandle->ToggleGrab();
//	m_pkPlayerDB->GetLoginCharacters(string("user"));

	#ifdef WIN32
		if(m_bStartMinimized)
			ShowWindow(GetFocus(), SW_MINIMIZE);
	#endif 
}


void MistServer::SetupGuiEnviroment()
{
	//// Create from script.
 //  if(!LoadGuiFromScript("data/script/gui/server.lua"))
	//{
	//	printf("Failed to load GUI script!\n");
	//	return;
	//}

	//m_pkGui->SetCursor( 0,0, m_pkTexMan->Load("data/textures/gui/blue_cursor.bmp", 0),
	//	m_pkTexMan->Load("data/textures/gui/blue_cursor_a.bmp", 0), 32, 32);
}


void MistServer::RegisterResources()
{
	//m_pkResourceDB->RegisterResource( string(".env"), Create__EnvSetting	);
}

void MistServer::RegisterScriptFunctions()
{
	g_pkScript->ExposeFunction("SayToCharacter",	SI_MistServer::SayToCharacterLua);
	g_pkScript->ExposeFunction("OpenContainer",	SI_MistServer::OpenContainerLua);
	
}

void MistServer::RegisterPropertys()
{
	MCommon_RegisterPropertys( m_pkZeroFps, m_pkPropertyFactory );

/*	m_pkPropertyFactory->Register("P_ArcadeCharacter",	Create_P_ArcadeCharacter);
	m_pkPropertyFactory->Register("P_Enviroment", Create_P_Enviroment);
	m_pkPropertyFactory->Register("P_ShadowBlob", Create_P_ShadowBlob);
	m_pkPropertyFactory->Register("P_Item", Create_P_Item);	
		
	Register_P_CharacterProperty(m_pkZeroFps);	
	Register_P_Ml(m_pkZeroFps);
	Register_P_CharacterControl(m_pkZeroFps);
	Register_P_FogPlane(m_pkZeroFps);
	Register_P_Container(m_pkZeroFps);*/
}

void MistServer::OnIdle()
{	
	if(m_pkGui->m_bHandledMouse == false)
	{
		Input();	
	}

	
	//update ml time and save time in serverinfo
	m_pkTime->AddTime(m_pkZeroFps->GetFrameTime());		
	m_pkServerInfoP->SetTime(m_pkTime->GetTotalMLTime());
	
	//print current mistlands time
	m_pkZeroFps->DevPrintf("server","date: %s", m_pkTime->GetDateString().c_str());
}


void MistServer::RenderInterface(void)
{
}


void MistServer::OnSystem()
{
	m_pkEnviroment->Update();
}

void MistServer::Input_Camera(float fMouseX, float fMouseY)
{
	if(m_pkActiveCamera == NULL)
		return;

	float fSpeedScale = m_pkZeroFps->GetFrameTime()*20;

	if(m_pkActiveCamera->GetViewMode() == Camera::CAMMODE_PERSP) 
	{

		Vector3 newpos = m_pkActiveCameraObject->GetLocalPosV();
		
		Matrix4 kRm;
		kRm = m_pkActiveCameraObject->GetLocalRotM();

		//kRm.Transponse();

		
		Vector3 xv = kRm.GetAxis(0);
		Vector3 zv = kRm.GetAxis(2);

		xv.y = 0;
		zv.y = 0;
		
		xv.Normalize();
		zv.Normalize();

		if(m_pkInputHandle->VKIsDown("right"))		newpos += xv * -fSpeedScale;		
		if(m_pkInputHandle->VKIsDown("left"))		newpos += xv * fSpeedScale;		
		if(m_pkInputHandle->VKIsDown("forward"))	newpos += zv * fSpeedScale;
		if(m_pkInputHandle->VKIsDown("back"))		newpos += zv * -fSpeedScale;	

		if(m_pkInputHandle->VKIsDown("down"))		newpos.y += fSpeedScale;
		if(m_pkInputHandle->VKIsDown("up"))			newpos.y -= fSpeedScale;
				
		//mouse wheel test
		if(m_pkInputHandle->Pressed(MOUSEWUP)) 	newpos.y += 1.0;
		if(m_pkInputHandle->Pressed(MOUSEWDOWN))	newpos.y -= 1.0;
		
		
		Vector3 rot;
		rot.Set(float(-fMouseY / 5.0),float(-fMouseX / 5.0),0);

		kRm.Transponse();		
		kRm.Rotate(rot);
		kRm.Transponse();		
		Vector3 bla = Vector3(0,0,1);
		bla = kRm.VectorTransform(bla);
		kRm.LookDir(bla,Vector3(0,1,0));
		kRm.Transponse();		

				
		m_pkActiveCameraObject->SetLocalPosV(newpos);		
		if(m_pkInputHandle->VKIsDown("pancam"))
		{
			SDL_ShowCursor(false);
			m_pkActiveCameraObject->SetLocalRotM(kRm);	
		}
		else
			SDL_ShowCursor(true);
	}

	else 
	{
		Vector3 kMove = Vector3::ZERO;

		if(m_pkInputHandle->VKIsDown("forward"))	kMove.y += fSpeedScale;
		if(m_pkInputHandle->VKIsDown("back"))		kMove.y -= fSpeedScale;
		if(m_pkInputHandle->VKIsDown("right"))		kMove.x += fSpeedScale;
		if(m_pkInputHandle->VKIsDown("left"))		kMove.x -= fSpeedScale;	
		
		if(m_pkInputHandle->VKIsDown("down"))		m_pkActiveCamera->OrthoZoom(0.9);
		if(m_pkInputHandle->VKIsDown("up"))			m_pkActiveCamera->OrthoZoom(1.1);

		
		
		if(m_pkInputHandle->VKIsDown("pancam"))
		{
			kMove += Vector3(-fMouseX * fSpeedScale,fMouseY * fSpeedScale,0);
		}


		/*if(m_pkCameraObject[1]->GetParent() == m_pkCameraObject[0])
		{
			// If Cameras are linked.
			kMove = m_pkActiveCamera->GetOrthoMove(kMove);
			//kMove.Print();
			//cout << endl;
			m_pkCameraObject[0]->SetLocalPosV(m_pkCameraObject[0]->GetLocalPosV() + kMove);
		}
		else 
		{
			P_Camera* pkCam = dynamic_cast<P_Camera*>(m_pkActiveCameraObject->GetProperty("P_Camera"));
			if(pkCam)
				pkCam->OrthoMove(kMove);
		}*/
	}
}

void MistServer::Input()
{
	float x ;
	float z ;		
	m_pkInputHandle->RelMouseXY(x,z);	

	Vector3 kMove(0,0,0);

	if(m_pkInputHandle->Pressed(KEY_8))	assert(0);
	if(m_pkInputHandle->Pressed(KEY_9))	ZFAssert(0, "Fet med test");

	/*
	if(pkEntTestArc)
	{
		P_ArcadeCharacter* pkArc = dynamic_cast<P_ArcadeCharacter*>( pkEntTestArc->GetProperty("P_ArcadeCharacter"));
		if(pkArc)
		{
			if(m_pkInputHandle->Pressed(KEY_I))	pkArc->m_kActions[2] = 1;
			if(m_pkInputHandle->Pressed(KEY_K))	pkArc->m_kActions[3] = 1;
			if(m_pkInputHandle->Pressed(KEY_J))	pkArc->m_kActions[1] = 1;
			if(m_pkInputHandle->Pressed(KEY_L))	pkArc->m_kActions[0] = 1;
			
		}
	}
	*/

	Input_Camera(float(x),float(z));
};

void MistServer::OnHud(void)
{
		
	if(m_pkActiveCamera) {
		m_pkZeroFps->DevPrintf("editor","Grid Size: %f", m_pkActiveCamera->m_fGridSpace);			
		m_pkZeroFps->DevPrintf("editor","Grid Snap: %i", m_pkActiveCamera->m_bGridSnap);			
		m_pkZeroFps->DevPrintf("editor","View: %s", m_pkActiveCamera->GetName().c_str());			
		}

	m_pkZeroFps->m_bGuiMode = false;
	m_pkZeroFps->ToggleGui();


}

void MistServer::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	NetPacket kNp;
	Vector3 kStartPos;

	unsigned int i;
	vector<string>	kUsers;
	string strNewTitle;

	switch(cmdid) {
		case FID_NEW:
			m_pkEntityManager->Clear();
			//GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);
			m_pkZeroFps->StartServer(false,true,4242);
			m_strWorldDir = "";
			SetTitle("MistServer");
			break;
		
		case FID_LOAD:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				m_pkConsole->Printf("load [worlddir]");
				break;				
			}
			
			m_pkZeroFps->StopAll();
			if(!m_pkEntityManager->LoadWorld(kCommand->m_kSplitCommand[1]))
			{
				cout<<"Error loading world"<<endl;
				break;
			}				
			
			// Set the title to include the world name.
			m_strWorldDir = kCommand->m_kSplitCommand[1];
			strNewTitle = "MistServer - " + m_strWorldDir;
			SetTitle(strNewTitle);
	
			cout<<"starting server"<<endl;
			//GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);			
			m_pkZeroFps->StartServer(false,true,4242);
			
			break;		
		
		case FID_SAVE:
			if(kCommand->m_kSplitCommand.size() > 1)
			{
				m_pkConsole->Printf(" The save command is now only for saving a loaded level.\nCan it be the 'saveas' command you should use? ");
				m_pkConsole->Printf(" To use save, enter it without a argument. Name of world dir can be found in title bar.");
				break;				
			}
		
			if(m_strWorldDir.empty()) 
			{
				m_pkConsole->Printf(" The current level has no name. Use saveas first");
				break;				
			}

			if(!m_pkEntityManager->SaveWorld(m_strWorldDir,true))
			{
				m_pkConsole->Printf("Error saving world");
				break;
			}	
			break;

		case FID_SAVEAS:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				m_pkConsole->Printf("save [worlddir]");
				break;				
			}
			
			if(!m_pkEntityManager->SaveWorld(kCommand->m_kSplitCommand[1],true))
			{
				m_pkConsole->Printf("Error saving world");
				break;
			}	

			// Set the title to include the world name.
			m_strWorldDir = kCommand->m_kSplitCommand[1];
			strNewTitle = "MistServer - " + m_strWorldDir;
			SetTitle(strNewTitle);

/*			cout<<"saving world:"<<endl;
			m_pkEntityManager->ForceSave();
			m_pkEntityManager->SaveZones();			
			cout<<"saved"<<endl;
*/			
			break;

		case FID_SAY:
		{
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				m_pkConsole->Printf("say [message]");
				break;				
			}
			
			string strMsg;
			for(int i = 4;i<kCommand->m_strFullCommand.size();i++)
				strMsg.push_back(kCommand->m_strFullCommand[i]);
					
				
			SayToClients(strMsg);	
		}			
			
		case FID_USERS:			
			m_pkPlayerDB->GetUsers(&kUsers);
			for(i=0; i<kUsers.size(); i++) 
			{
				cout << "User: " << kUsers[i] << endl;
			}
			break;		

		case FID_TEST_JIDDRA:
			/*pkEntity = this->m_pkEntityManager->CreateObjectFromScript("data/script/objects/characters/hosplayer.lua");		// t_player
			//pkEntity = this->m_pkEntityManager->CreateObjectFromScript("data/script/objects/characters/BadGuy.lua");		// t_player
			pkEntity->SetUseZones(true);
			kStartPos = Vector3(0,0.5,0);
			pkEntity->SetWorldPosV(kStartPos);*/


			m_pkConsole->Printf("Long Text: ");
			m_pkConsole->Printf("This is a totaly pointless text that have no other purpose then being long and boring and boring and long. In short, don't fall asleep when you read this");
			m_pkConsole->Printf("\n");
			m_pkConsole->Printf("Long Text with no spaces: ");
			m_pkConsole->Printf("Thisisanotherpointlesstextbutwithoutanyspacesthistimesoitisnotaseasytoreadunlessyouareapersonthatareusedtoreadbookwithoutspacesandifitissothenyouareinsane.");
			m_pkConsole->Printf("\n\n");
			m_pkConsole->Printf("Text with three rows.");
			m_pkConsole->Printf("Rad 1 :(\nRad 2 :|\nRad 3 :)");
			m_pkConsole->Printf("\n\n\n");
			m_pkConsole->Printf("Mult rows with newline at the end.");
			m_pkConsole->Printf("Rad 1 :(\nRad 2 :|\nRad 3 :)\nRad 4 =)\n");

			break;

		case FID_LIGHTMODE:

			if(kCommand->m_kSplitCommand.size() <= 1)
				break;
/*
			iMode = atoi(kCommand->m_kSplitCommand[1].c_str());
			if(iMode == 0)	m_pkZShader->SetForceLighting(LIGHT_ALWAYS_ON);		
			if(iMode == 1)	m_pkZShader->SetForceLighting(LIGHT_ALWAYS_OFF);		
			if(iMode == 2)	m_pkZShader->SetForceLighting(LIGHT_MATERIAL);		
			break;
*/


	

	}

}

void MistServer::ClientInit()
{
	cout<<"Client Join granted"<<endl;
	
	//cout<<"Join Complete"<<endl;
}

bool MistServer::OnPreConnect(IPaddress kRemoteIp, char* szLogin, char* szPass, bool bIsEditor, string& strWhy)
{
	//dessa skall du fixa till vim =D
	string strPlayer		= szLogin;
	string strPasswd		= szPass;
	string strCharacter	= "mrbad";

	// Check that this is a valid login.
	if(m_pkPlayerDB->LoginExist(strPlayer)) 
	{
		if(m_pkPlayerDB->IsOnline(szLogin))
		{
			m_pkConsole->Printf("Player '%s' tried to join twice",strPlayer.c_str());
			strWhy = "Your login name is already connected";
			return false;	
		}

		// Check Password
		if(!m_pkPlayerDB->VerifyPlayer(strPlayer,strPasswd)) 
		{
			m_pkConsole->Printf("Player '%s' found, password check FAILED",strPlayer.c_str());
			strWhy = "Wrong login name and/or password";
			return false;
		}

		m_pkConsole->Printf("Player '%s' found, password check OK",strPlayer.c_str());
		return true;
	}
	else 
	{
		if(!m_AcceptNewLogins)
		{
			m_pkConsole->Printf("A new player '%s' tried to join but new players are not accepted", strPlayer.c_str());
			strWhy = "No login with that name found";
			return false;
		}

		// Try To Create the new player
		if(!m_pkPlayerDB->CreatePlayer(strPlayer,strPasswd)) 
		{
			m_pkConsole->Printf("Failed to create new player '%s'",strPlayer.c_str());
			strWhy = "Failed to create player on server";
			return false;
		}
	
		if(m_pkPlayerDB->VerifyPlayer(strPlayer,strPasswd))
		{
			m_pkConsole->Printf("Player '%s' created",strPlayer.c_str());
			return true;
		}
		else
		{
			m_pkConsole->Printf("Error while creating player '%s' (this shuld never hapen)",strPlayer.c_str());
			return false;
		}
	}

	return false;
}

void MistServer::OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass, bool bIsEditor)
{
	string strPlayer		= szLogin;
	string strPasswd		= szPass;

	pkClient->m_strCharacter = szLogin;  //anv�der spelar namnet som karakt�s namn s�l�ge..
	pkClient->m_strLogin = szLogin;

	if(!pkClient->m_pkObject)
		cout<<"ERROR: client object not created"<<endl;

	//login player
	m_pkPlayerDB->Login(strPlayer,strPasswd);

	//setup some player info
	if(PlayerData* pkNewPlayer = m_pkPlayerDB->GetPlayerData(strPlayer))
	{
		pkNewPlayer->m_iConnectionID = iConID;		
		pkNewPlayer->m_fLoginTime = m_pkZeroFps->GetTicks();	
	}
	
	
	bool bEditorConnect = bIsEditor;
	cout << szLogin << " Joined with editmode " << bEditorConnect << endl;

	if(bEditorConnect) 
	{
		if(P_Track* pkTrack = (P_Track*)pkClient->m_pkObject->AddProperty("P_Track"))
			pkTrack->SetClient(iConID);			
		
		pkClient->m_pkObject->AddProperty("P_EditIcon");		
		
		pkClient->m_pkObject->m_ucIcon = 1;
		
		return;
	}
	else
		SpawnPlayer(iConID);
		
		
	SayToClients(strPlayer + string(" connected"));
}

void MistServer::SpawnPlayer(int iConID)
{
	cout << "MistServer::SpawnPlayer" << endl;
	if(m_pkZeroFps->m_kClient[iConID].m_strCharacter.size() == 0) 
	{
		m_pkZeroFps->PrintToClient(iConID, "You must select a character before you can join" );
		return;
	}

	//create player object
	int iPlayerID  = CreatePlayer(m_pkZeroFps->m_kClient[iConID].m_strLogin.c_str(),m_pkZeroFps->m_kClient[iConID].m_strCharacter.c_str(),"Start",iConID);
	
	if(iPlayerID == -1)
	{
		cout<<"Error creating playercharacter"<<endl;
	}
	
	
	//set chracter id in playerdata
	if(PlayerData* pkNewPlayer = m_pkPlayerDB->GetPlayerData(iConID))
	{
		pkNewPlayer->m_iCharacterID = iPlayerID;		
	}	
}



void MistServer::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	if(PlayerData* pkNewPlayer = m_pkPlayerDB->GetPlayerData(iConID))
	{
		SayToClients(pkNewPlayer->m_strPlayerName + string(" disconnected"));				
	}	

	
	DeletePlayerCharacter(iConID);
	m_pkPlayerDB->Logout(pkClient->m_strLogin);
	cout<<"Client "<<iConID<<" Parted"<<endl;	
	

}


void MistServer::OnServerStart(void)
{		
	m_pkServerInfoP = NULL;



	CreateEditCameras();

	//create/check for server info property
	if(m_pkServerInfoP = (P_ServerInfo*)m_pkEntityManager->GetGlobalEntity()->GetProperty("P_ServerInfo"))
	{
		cout<<"servern info property found =D"<<endl;	
		cout<<"server name:"<<m_pkServerInfoP->GetServerName()<<endl;
		cout<<"current server time:"<<m_pkServerInfoP->GetTime()<<endl;
		
		m_pkTime->SetTime(m_pkServerInfoP->GetTime());
	}
	else
	{
		if(m_pkServerInfoP = (P_ServerInfo*)m_pkEntityManager->GetGlobalEntity()->AddProperty("P_ServerInfo"))
		{
			cout<<"Created server info property"<<endl;
			m_pkServerInfoP->SetServerName("MistServer alpha");
		}
	}
	
	
	
/*	
	//create server info object
	m_pkServerInfo = m_pkEntityManager->CreateObjectFromScript("data/script/objects/t_serverinfo.lua");
	if(m_pkServerInfo)
	{
		m_pkServerInfo->SetParent(m_pkEntityManager->GetGlobalObject());
		m_pkServerInfoP = (P_ServerInfo*)m_pkServerInfo->GetProperty("P_ServerInfo");		
		if(m_pkServerInfoP)
		{
			m_pkServerInfoP->SetServerName("Test Server");
		}
		else
			cout<<"ERROR: No server P_ServerInfo property created, this is no good"<<endl;
	}
*/	
}

void MistServer::OnClientStart(void)
{
	CreateEditCameras();

}

bool MistServer::StartUp()	
{ 
//	m_pkAStar	= static_cast<AStar*>(GetSystem().GetObjectPtr("AStar"));
	return true; 
}

bool MistServer::ShutDown()	{ return true; }
bool MistServer::IsValid()	{ return true; }


int MistServer::CreatePlayer(const char* csPlayer,const char* csCharacter,const char* csLocation,int iConID)
{

	//try to crreate saved character
	Entity* pkObject = m_pkPlayerDB->CreateCharacter(csPlayer,csCharacter);
	
	
	//if it fails try to create a new character
	if(!pkObject)
	{
		cout<<"Character not found, trying to create it"<<endl;
		if(m_pkPlayerDB->CreateNewCharacter(csPlayer,csCharacter))
		{
			//new character created , now load it
			pkObject = m_pkPlayerDB->CreateCharacter(csPlayer,csCharacter);
		}
		else	
			return -1;
		
	}
	
	
	if(pkObject)
	{		
		//finaly set objects position
		pkObject->SetWorldPosV(GetPlayerStartPos());
		
		//setup tracker to correct tracker id
		P_Track* pkTrack = dynamic_cast<P_Track*>(pkObject->GetProperty("P_Track"));	
		if(pkTrack)
			pkTrack->SetClient(iConID);	
	}
	else
	{	
		cout<<"Error creating caracter entity"<<endl;
		return -1;
	}
			
	cout<<"created character entity "<<csPlayer<<" -> "<<csCharacter<<endl;
	pkEntTestArc = pkObject;

	return pkObject->GetEntityID();
}

void MistServer::DeletePlayerCharacter(int iConID)
{
	if(PlayerData* pkPD = m_pkPlayerDB->GetPlayerData(	iConID ))
	{
		//first save and delete the player character
		Entity* pkObj = m_pkEntityManager->GetEntityByID(pkPD->m_iCharacterID);
		if(pkObj)
		{
			m_pkPlayerDB->SaveCharacter(pkObj,pkPD->m_strPlayerName);
			m_pkEntityManager->Delete(pkObj);
		}
	}

}


void MistServer::OnNetworkMessage(NetPacket *PkNetMessage)
{
	unsigned char ucType;

	// Read Type of Message.
	PkNetMessage->Read(ucType);

	switch(ucType)
	{
		case MLNM_CS_REQ_CHARACTERID:
		{
			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
			{
				NetPacket kNp;
				
				kNp.Clear();
				kNp.Write((char) MLNM_SC_CHARACTERID);
				kNp.Write(pkData->m_iCharacterID);
				kNp.TargetSetClient(PkNetMessage->m_iClientID);
				SendAppMessage(&kNp);
			}
			break;
		}
		
		case MLNM_CS_REQ_KILLME:
		{
			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
			{
				cout<<"Player "<<pkData->m_strPlayerName<<" requested to be killed, BYE BYE =D"<<endl;
				
				//move player to start
				if(Entity* pkCharacter = m_pkEntityManager->GetEntityByID(pkData->m_iCharacterID))
				{
					pkCharacter->SetWorldPosV(GetPlayerStartPos());
				}				
			}
					
			break;
		}		
		
		case MLNM_CS_CONTROLS:
		{
			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
			{
				bitset<6> kControls;
				float	fYAngle;
				float fPAngle;
								
				PkNetMessage->Read(kControls);
				PkNetMessage->Read(fYAngle);
				PkNetMessage->Read(fPAngle);
				
				
				if(Entity* pkCharacter = m_pkEntityManager->GetEntityByID(pkData->m_iCharacterID))
				{
					if(P_CharacterControl* pkCC = (P_CharacterControl*)pkCharacter->GetProperty("P_CharacterControl"))
					{
						pkCC->SetKeys(&kControls);
						pkCC->SetRotation(fYAngle,fPAngle);
					}
					else
						cout<<"character has no controls ...WHYYYYYYYY!!!!!!!!"<<endl;
				}
				else
					cout<<"missing player character entity:"<<pkData->m_iCharacterID<<endl;
			}
			else
				cout<<"missting playerdata in plyerDB"<<endl;
			
			break;
		}

		case MLNM_CS_ANIM:
		{
			int iTauntID;
			PkNetMessage->Read(iTauntID);

			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
			{
				if(Entity* pkCharacter = m_pkEntityManager->GetEntityByID(pkData->m_iCharacterID))
				{
					if(P_CharacterProperty* pkCC = (P_CharacterProperty*)pkCharacter->GetProperty("P_CharacterProperty"))
					{
						pkCC->DoTaunt(iTauntID);
					}
				}
			}
			break;
		}
		
		case MLNM_CS_SAY:
		{
			string strMsg;
			PkNetMessage->Read_Str(strMsg);
			
			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
			{				
				SayToClients(strMsg,pkData->m_strPlayerName,pkData->m_iCharacterID);
				
				// --- for server output only , not reqired
				m_pkConsole->Printf("Msg> %s: %s",pkData->m_strPlayerName.c_str(),strMsg.c_str());								
				if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(pkData->m_iCharacterID,"P_CharacterProperty"))
					pkCP->AddChatMsg(strMsg);
				// ---
				
			}			
			
			break;
		}			
			
		case MLNM_CS_REQ_PLAYERLIST:
		{
			SendPlayerListToClient(PkNetMessage->m_iClientID);
						
			break;
		}
		
		case MLNM_CS_REQ_EQIPMENT:
		{
			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
			{				
				SendCharacterEqipment(pkData->m_iCharacterID, PkNetMessage->m_iClientID);
				
			}
						
			break;
		}
		
		
		case MLNM_CS_ACTION:
		{
			int iEntity;
			string strAction;
			PkNetMessage->Read(iEntity);
			PkNetMessage->Read_Str(strAction);
			
			if(Entity* pkObj = m_pkEntityManager->GetEntityByID(iEntity))
			{
				// Get EntityID for client doing stuff.
            if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
				{
					//distance check
					if(Entity* pkChar = m_pkEntityManager->GetEntityByID(pkData->m_iCharacterID))
						if(pkObj->GetWorldPosV().DistanceTo(pkChar->GetWorldPosV()) > 2.0)
						{
							SayToClients("You are to far away","Server",-1,PkNetMessage->m_iClientID);
							break;
						}
				
					if(P_Item* pkItem = (P_Item*)pkObj->GetProperty("P_Item"))
					{
						if(pkItem->GetInContainerID() != -1)
						{
							SayToClients("This item is in a container","Server",-1,PkNetMessage->m_iClientID);
							break;
						}					
					}
					
					//cout<<"character: "<<	pkData->m_strCharacterName<<" doint action: "<<strAction<<" on "<<iEntity<<endl;
				
					vector<ScriptFuncArg> args(2);
					args[0].m_kType.m_eType = tINT;
					args[0].m_pData = &(pkData->m_iCharacterID);
					args[1].m_kType.m_eType = tSTLSTRING;
					args[1].m_pData = &strAction;
					
					m_pkEntityManager->CallFunction(pkObj, "Useit",&args);	
				}
			}
			break;
		}
		
		case MLNM_CS_MOVE_ITEM:
		{
			int iItemID;
			int iTargetContainer;
			int iPosX;
			int iPosY;
			int iCount;
			
			PkNetMessage->Read(iItemID);
			PkNetMessage->Read(iTargetContainer);
			PkNetMessage->Read(iPosX);
			PkNetMessage->Read(iPosY);			
			PkNetMessage->Read(iCount);			
		
			P_Container*	pkTargetContainer = 	NULL;
			P_Container*	pkInContainer = 		NULL;
			P_Item* 			pkItem = 				NULL;
			Entity*			pkCharacter = 			NULL;
			
			cout<<"got move package  item:"<<iItemID<<" targetcontainer:"<<iTargetContainer<<" pos:"<<iPosX<<"x"<<iPosY<<" count:"<<iCount<<endl;
			
			//get container
			if(Entity* pkContainerEnt = m_pkEntityManager->GetEntityByID(iTargetContainer))
				pkTargetContainer = (P_Container*)pkContainerEnt->GetProperty("P_Container");

			//Get item
			if(Entity* pkContainerEnt = m_pkEntityManager->GetEntityByID(iItemID))
				if(!(pkItem = (P_Item*)pkContainerEnt->GetProperty("P_Item")))			
				{
					cout<<"WARNING: MLNM_CS_MOVE_ITEM  could not find item "<<iItemID<<endl;
					break;
				}

			//get in container
			if(Entity* pkContainerEnt = m_pkEntityManager->GetEntityByID(pkItem->GetInContainerID()))
				pkInContainer = (P_Container*)pkContainerEnt->GetProperty("P_Container");
				
			//get players character entity
			if(PlayerData* pkPlayerData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
				if(!(pkCharacter = m_pkEntityManager->GetEntityByID(pkPlayerData->m_iCharacterID)))
				{
					cout<<"WARNING: MLNM_CS_MOVE_ITEM could not find any character"<<endl;
					break;
				}
							
				
			//item is not in any container	
			if(!pkInContainer)
			{
				//do a distance check
				if(pkItem->GetEntity()->GetWorldPosV().DistanceTo(pkCharacter->GetWorldPosV()) > 2.0)
				{
					SayToClients("You are to far away","Server",-1,PkNetMessage->m_iClientID);
					break;
				}						
			
				//do we have a target container , if so try to put item in it
				if(pkTargetContainer)
				{
					//check if target container is ours
					if(pkTargetContainer->GetOwnerID() != pkCharacter->GetEntityID())
					{
						SayToClients("target container is not yours","Server",-1,PkNetMessage->m_iClientID);
						break;					
					}								
							
					cout<<"picking up item from ground"<<endl;
					
					//try adding item from gound									
					if(!pkTargetContainer->AddMove(iItemID,iPosX,iPosY,iCount))
						SayToClients("You could not pick that up","Server",-1,PkNetMessage->m_iClientID);					
					
					SendContainer(iTargetContainer,PkNetMessage->m_iClientID,false);											
						
					break;

				}
				else
				{
					cout<<"WARNING: item not in any container, nor does it have ny target, noting to do :("<<endl;
					break;
				}		
					
			}
			//item is in a container
			else
			{
				//ware we owner of this container?
				if(pkInContainer->GetOwnerID() != pkCharacter->GetEntityID())
				{
					SayToClients("That container is not yours","Server",-1,PkNetMessage->m_iClientID);
					break;					
				}
			

				//do we have a target container 
				if(!pkTargetContainer)
				{
				
					//if we have a position, try moving there
					if(iPosX != -1)
					{
						cout<<"no target container, moving within this container"<<endl;
					
						if(!pkInContainer->AddMove(iItemID,iPosX,iPosY,iCount))
							SayToClients("Could not move item","Server",-1,PkNetMessage->m_iClientID);				
														
						SendContainer(pkInContainer->GetEntity()->GetEntityID(),PkNetMessage->m_iClientID,false);																		
							
						break;		
					}
					//no position, try drop
					else
					{
						cout<<"got no position, droping item"<<endl;
						
						
						if(!pkInContainer->DropItem(iItemID,pkCharacter->GetWorldPosV()))
							SayToClients("Could not drop item","Server",-1,PkNetMessage->m_iClientID);																				
						
						SendContainer(pkInContainer->GetEntity()->GetEntityID(),PkNetMessage->m_iClientID,false);											
							
						break;
					}

				}
				else
				{
					//first do we own this container?						
					if(pkTargetContainer->GetOwnerID() != pkCharacter->GetEntityID())
					{
						SayToClients("Target container is not yours","Server",-1,PkNetMessage->m_iClientID);
						break;					
					}				
				
					cout<<"moving item to another container"<<endl;
					if(!pkTargetContainer->AddMove(iItemID,iPosX,iPosY,iCount))
						SayToClients("Could not move item to that container","Server",-1,PkNetMessage->m_iClientID);	
					
					SendContainer(pkInContainer->GetEntity()->GetEntityID(),PkNetMessage->m_iClientID,false);
					SendContainer(iTargetContainer,PkNetMessage->m_iClientID,false);						
					
					break;	
		
				}
			}
			

			cout<<"WARNING: bad item movement"<<endl;
			SayToClients("Bad item movement","Server",-1,PkNetMessage->m_iClientID);
			
			break;	
		}		
		
		case MLNM_CS_REQ_CONTAINER:
		{
			int iContainerID;
			PkNetMessage->Read(iContainerID);
			
			OpenContainer(iContainerID,PkNetMessage->m_iClientID);	
					
			break;
		}
		

		default:
			cout << "Error in game packet : " << (int) ucType << endl;
			PkNetMessage->SetError(true);
			return;
	}
}

void MistServer::OpenContainer(int iContainerID,int iClientID)
{
	if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(iClientID))
	{		
		if(Entity* pkCharacter = m_pkEntityManager->GetEntityByID(pkData->m_iCharacterID))
		{
			if(Entity* pkContainerEnt = m_pkEntityManager->GetEntityByID(iContainerID))
			{
				if(P_Container* pkContainerP = (P_Container*)pkContainerEnt->GetProperty("P_Container"))
				{				
					//check owner
					if(pkContainerP->GetOwnerID() != -1)
					{
						if(pkContainerP->GetOwnerID() != pkData->m_iCharacterID)
						{
							//cout<<"and its not me =("<<endl;
							SayToClients("Someone else is using this container","Server",-1,iClientID);
							return;
						}
					}
				
				
					//is this a inventory container
					if(pkContainerP->GetContainerType() == eInventory)
					{
						SendContainer(iContainerID,iClientID,true);
					}
					else
					{
						//check if its in a container or if its close enoguth
						if( (!pkContainerEnt->GetParent()->IsZone()) || (pkCharacter->GetWorldPosV().DistanceTo(pkContainerEnt->GetWorldPosV()) < 2.0) )
						{
							 SayToClients("Opening container","Server",-1,iClientID);
							 
							 //set new owner of this container
							 pkContainerP->SetOwnerID(pkData->m_iCharacterID);
							 
							 SendContainer(iContainerID,iClientID,true);
						}
						else
							SayToClients("You are to far away","Server",-1,iClientID);
					}
				}
			}
		}
	}
}


void MistServer::SendPlayerListToClient(int iClient)
{
	NetPacket kNp;
	vector<string> kActivePlayers;		
	
	//get user list
	m_pkPlayerDB->GetUsers(&kActivePlayers);
	
	//clear package and write pkg id
	kNp.Clear();
	kNp.Write((char) MLNM_SC_PLAYERLIST);
	
	//write player names			
	kNp.Write(kActivePlayers.size());
	for(int i = 0;i<kActivePlayers.size();i++)
	{
		kNp.Write_Str(kActivePlayers[i]);
	}

	//send package
	kNp.TargetSetClient(iClient);
	SendAppMessage(&kNp);	
}

void MistServer::SendContainer(int iContainerID,int iClientID,bool bOpen)
{	
	if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(iContainerID))
	{
		if(P_Container* pkContainer = (P_Container*)pkEnt->GetProperty("P_Container"))
		{
			vector<MLContainerInfo> kItemList;
			pkContainer->GetItemList(&kItemList);
			
			
			NetPacket kNp;			
			kNp.Write((char) MLNM_SC_CONTAINER);
			
			kNp.Write(bOpen);
			kNp.Write(iContainerID);
			kNp.Write(pkContainer->GetContainerType());
			kNp.Write(pkContainer->GetSizeX());
			kNp.Write(pkContainer->GetSizeY());
			
			int iItems = kItemList.size();
			kNp.Write(iItems);
			
			for(int i = 0;i<kItemList.size();i++)
			{
				kNp.Write_Str(kItemList[i].m_strName);
				kNp.Write_Str(kItemList[i].m_strIcon);
				
				kNp.Write(kItemList[i].m_iItemID);
				kNp.Write(kItemList[i].m_cItemX);
				kNp.Write(kItemList[i].m_cItemY);
				kNp.Write(kItemList[i].m_cItemW);
				kNp.Write(kItemList[i].m_cItemH);	
				kNp.Write(kItemList[i].m_cItemType);										
				kNp.Write(kItemList[i].m_iStackSize);				
				kNp.Write(kItemList[i].m_bIsContainer);	
				
				if(kNp.m_iPos > 1000)
				{
					cout<<"container to big, lets krash =P"<<endl;
					
				}
			}
			
			kNp.TargetSetClient(iClientID);
			SendAppMessage(&kNp);
		
		}
		else
			cout<<"WARNING: requested container did not have any p_container"<<endl;
	}
	else
		cout<<"container not found"<<endl;

}

void MistServer::SendCharacterEqipment(int iCharacter,int iClientID)
{
	cout<<"sending eqipment for character "<<iCharacter<<" to client "<<iClientID<<endl;

	if(P_CharacterProperty* pkCharacter = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(iCharacter,"P_CharacterProperty"))
	{
		//send all containers to client
		SendContainer(pkCharacter->m_iHead,			iClientID,true);
		SendContainer(pkCharacter->m_iGloves,		iClientID,true);
		SendContainer(pkCharacter->m_iCape,			iClientID,true);
		SendContainer(pkCharacter->m_iNecklace,	iClientID,true);
		SendContainer(pkCharacter->m_iBody,			iClientID,true);
		SendContainer(pkCharacter->m_iLeftHand,	iClientID,true);
		SendContainer(pkCharacter->m_iRightHand,	iClientID,true);
		SendContainer(pkCharacter->m_iBracers,		iClientID,true);
		SendContainer(pkCharacter->m_iRing1,		iClientID,true);
		SendContainer(pkCharacter->m_iRing2,		iClientID,true);
		SendContainer(pkCharacter->m_iRing3,		iClientID,true);
		SendContainer(pkCharacter->m_iRing4,		iClientID,true);
		SendContainer(pkCharacter->m_iBelt,			iClientID,true);
		SendContainer(pkCharacter->m_iFeets,		iClientID,true);
	}
}


void MistServer::SayToClients(const string& strMsg,const string& strSource,int iCharacterID ,int iClientID)
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_SC_SAY);
	kNp.Write(iCharacterID);
	
	kNp.Write_Str(strSource);
	kNp.Write_Str(strMsg);
	kNp.TargetSetClient(iClientID);
	SendAppMessage(&kNp);
	
}


Vector3 MistServer::GetPlayerStartPos()
{
	if(Entity* pkEnt = m_pkEntityManager->GetEntityByType("hosstartpos.lua"))
	{
		return pkEnt->GetWorldPosV();	
	}
	else
	{
		cout<<"missing hosstartpos.lua object, starting at <0 1 0>"<<endl;
		return Vector3(0,3,0);			
	}
}



//--------- script interface for mistserver
namespace SI_MistServer
{
	int SayToCharacterLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;
		
		int id;
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		id = (int)dTemp;
		
		char	acMessage[256];
		g_pkScript->GetArgString(pkLua, 1, acMessage);		
		
		if(PlayerData* pkData = g_kMistServer.m_pkPlayerDB->GetPlayerDataByCharacterID(id))		
			g_kMistServer.SayToClients(acMessage,"Server",-1,pkData->m_iConnectionID);
		else
			cout<<"WARNING: could not find character ID:"<<id<<endl;
						
		return 0;
	}

	int OpenContainerLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;			
			
		int iCharacter;
		int iContainer;
		double dTemp;
		
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		iContainer = (int)dTemp;
		
		g_pkScript->GetArgNumber(pkLua, 1, &dTemp);
		iCharacter = (int)dTemp;	
		
		if(PlayerData* pkData = g_kMistServer.m_pkPlayerDB->GetPlayerDataByCharacterID(iCharacter))
		{
			g_kMistServer.OpenContainer(iContainer,pkData->m_iConnectionID);
		}										
	}
}