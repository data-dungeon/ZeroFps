/**
	\defgroup MistServer MistServer
	\ingroup MistLand

  MistServer is the Server of the game MistLands.
*/ 
 
#include <set> 
#include <algorithm>

#include "mistserver.h"
#include "../zerofpsv2/engine_systems/common/heightmap.h"
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "../zerofpsv2/engine_systems/propertys/p_primitives3d.h"
#include "../zerofpsv2/engine_systems/propertys/p_track.h"
#include "../zerofpsv2/engine_systems/propertys/p_skyboxrender.h"
#include "../zerofpsv2/engine_systems/propertys/p_hmrp2.h"
#include "../zerofpsv2/engine/p_pfpath.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../zerofpsv2/basic/zguifont.h"
#include "../mcommon/rulesystem/sendtype.h"
#include "../zerofpsv2/engine/inputhandle.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../zerofpsv2/render/glguirender.h"
#include "../mcommon/si_dm.h"
#include "../zerofpsv2/engine_systems/propertys/p_scriptinterface.h"

MistServer g_kMistServer("MistServer", 0, 0, 0);

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
	return true;
}


MistServer::MistServer(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	g_ZFObjSys.Log_Create("mistserver");
	m_pkServerInfoP = NULL;

	// Set Default values
	m_AcceptNewLogins = true;

	// Register Variables
	RegisterVariable("s_newlogins",				&m_AcceptNewLogins,			CSYS_BOOL);	

	// Register Commands
	Register_Cmd("new",			FID_NEW);		
	Register_Cmd("load",		FID_LOAD);		
	Register_Cmd("save",		FID_SAVE);
	Register_Cmd("saveas",		FID_SAVEAS);
	Register_Cmd("users",		FID_USERS);		
	Register_Cmd("lo",			FID_LOCALORDER);		
	Register_Cmd("lightmode",	FID_LIGHTMODE);		
	Register_Cmd("jiddra",		FID_TEST_JIDDRA);

	m_strWorldDir  = "";
   
	m_pkActiveCameraObject		= NULL;
	m_pkActiveCamera			= NULL;
} 


void MistServer::CreateEditCameras()
{
	m_pkActiveCameraObject = m_pkObjectMan->CreateObjectFromScript("data/script/objects/t_camedit.lua");
	if(m_pkActiveCameraObject) 
	{
		m_pkActiveCameraObject->SetParent( m_pkObjectMan->GetWorldObject() );
		P_Camera* m_pkCamProp = (P_Camera*)m_pkActiveCameraObject->GetProperty("P_Camera");
		m_pkCamProp->SetCamera(m_pkCamera);
		m_pkActiveCameraObject->GetSave() = false;
		
		P_Enviroment* pe = (P_Enviroment*)m_pkActiveCameraObject->AddProperty("P_Enviroment");
		pe->SetEnable(true);		
		pe->SetEnviroment("data/enviroments/server.env");

	}

	GetWnd("vp1")->SetRenderTarget(m_pkCamera);
	GetWnd("vp1")->SetMoveArea(Rect(0,0,800,600), false);

	m_pkActiveCamera = m_pkCamera;
	m_pkActiveCamera->SetSelected(true);

	m_pkFps->AddRenderCamera(m_pkCamera);
	m_pkCamera->m_bForceFullScreen = true;
}


void MistServer::OnInit() 
{
	m_pkZFVFileSystem->AddRootPath( string("../data/mistlands/") ,"data/");
	m_pkZFVFileSystem->AddRootPath( string("../data/dm/") ,"data/");
	
	m_pkConsole->Printf(" MistServer (mistland dedicated server)");
	m_pkConsole->Printf("--------------------------------");
	m_pkConsole->Printf("");

	Init();

	//init dm script interface (register script functions for gameplay)
	DMLua::Init(m_pkObjectMan,m_pkScript,m_pkGuiMan);

	//run autoexec script
	if(!m_pkIni->ExecuteCommands("mistserver_autoexec.ini"))
		m_pkConsole->Printf("No mistserver_autoexec.ini found");
}

void MistServer::Init()
{	
	//create player database
	m_pkPlayerDB = new PlayerDatabase();

	//m_pkZShaderSystem->SetForceLighting(LIGHT_ALWAYS_OFF);	
	
	//enable debug graphics
	m_pkFps->SetDebugGraph(true);
	
	//register property bös
	RegisterPropertys();

	//register resources
	RegisterResources();

	//initiate our camera
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.25,250);	
	m_pkCamera->SetName("persp");

	//init mistland script intreface
	MistLandLua::Init(m_pkObjectMan,m_pkScript);

	ZGuiRender* pkRenderer = static_cast<ZGuiRender*>(g_ZFObjSys.GetObjectPtr("ZGuiRender"));
	pkRenderer->SetScaleMode(GUIScaleManually);
	
	// create gui script funktions
	GuiAppLua::Init(&g_kMistServer, m_pkScript);

	// Load default texture and create default font and menu (NULL = No menu).
	InitGui(m_pkScript, 
		//"data/textures/text/ms_sans_serif8.bmp", 
		"data/textures/text/ms_sans_serif8.tga", 
		"data/script/gui/defskins.lua", 
		"data/script/gui/menu.txt", true); 

	// Create startup GUI for the the server from script.
	SetupGuiEnviroment();

	//setup caption
	SetTitle("MistServer");
	// hide cursor
	m_pkInput->ShowCursor(true);

//	m_pkInputHandle->ToggleGrab();
//	m_pkPlayerDB->GetLoginCharacters(string("user"));
}


void MistServer::SetupGuiEnviroment()
{
	// Create from script.
	LoadGuiFromScript("data/script/gui/server.lua");

	m_pkGui->SetCursor( 0,0, m_pkTexMan->Load("data/textures/gui/blue_cursor.bmp", 0),
		m_pkTexMan->Load("data/textures/gui/blue_cursor_a.bmp", 0), 32, 32);
}


void MistServer::RegisterResources()
{
	m_pkResourceDB->RegisterResource( string(".env"), Create__EnvSetting	);
}


void MistServer::RegisterPropertys()
{
	m_pkPropertyFactory->Register("P_Car", Create_P_Car);

	m_pkPropertyFactory->Register("P_DMClickMe", Create_P_DMClickMe);
	m_pkPropertyFactory->Register("P_DMMission", Create_P_DMMission);
	m_pkPropertyFactory->Register("P_DMItem", Create_P_DMItem);
	m_pkPropertyFactory->Register("P_DMGun", Create_P_DMGun);
	m_pkPropertyFactory->Register("P_ShadowBlob", Create_P_ShadowBlob);
	m_pkPropertyFactory->Register("P_DMCharacter", Create_P_DMCharacter);
	m_pkPropertyFactory->Register("P_DMShop", Create_P_DMShop);
	m_pkPropertyFactory->Register("P_DMHQ", Create_P_DMHQ);
	m_pkPropertyFactory->Register("P_Spawn", Create_P_Spawn);
	m_pkPropertyFactory->Register("P_Enviroment", Create_P_Enviroment);
	m_pkPropertyFactory->Register("P_ClientControl", Create_P_ClientControl);
	m_pkPropertyFactory->Register("P_ServerInfo", Create_P_ServerInfo);
	m_pkPropertyFactory->Register("P_Ml", Create_P_Ml);
	m_pkPropertyFactory->Register("P_CharStats", Create_P_CharStats);
	m_pkPropertyFactory->Register("P_Item", Create_P_Item);
	m_pkPropertyFactory->Register("P_Spell", Create_P_Spell);
	m_pkPropertyFactory->Register("P_AI", Create_P_AI);
	m_pkPropertyFactory->Register("P_Container", Create_P_Container);
	m_pkPropertyFactory->Register("P_ArcadeCharacter", Create_P_ArcadeCharacter);
}

void MistServer::OnIdle()
{	
	//m_pkFps->GetCam()->ClearViewPort();	

	if(m_pkGui->m_bHandledMouse == false)
	{
		Input();	
	}

 	//m_pkFps->UpdateCamera(); 		

	if(m_pkServerInfoP)
	{
		m_pkFps->DevPrintf("server","ServerName: %s", m_pkServerInfoP->GetServerName().c_str());
		m_pkFps->DevPrintf("server","Players: %d", m_pkServerInfoP->GetNrOfPlayers());	
	}
}


void MistServer::RenderInterface(void)
{
}


void MistServer::OnSystem()
{
	HandleOrders();
}

void MistServer::Input_Camera(float fMouseX, float fMouseY)
{
	float fSpeedScale = m_pkFps->GetFrameTime()*20;

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
	// *** HANDE Quueued Keys.
	int iPressedKey = m_pkInputHandle->GetQueuedKey().m_iKey;

	switch(iPressedKey)
	{
	case KEY_F9:
		printf("smurf\n");
		break;
	}

	int x = 0;
	int z = 0;		
	m_pkInputHandle->RelMouseXY(x,z);	

	Vector3 kMove(0,0,0);

	if(m_pkInputHandle->Pressed(KEY_8))	assert(0);
	if(m_pkInputHandle->Pressed(KEY_9))	ZFAssert(0, "Fet med test");

	Input_Camera(float(x),float(z));
/*
	if(m_pkInputHandle->VKIsDown("lighton"))			m_pkZShader->SetForceLighting(LIGHT_ALWAYS_ON);	
	if(m_pkInputHandle->VKIsDown("lightoff"))			m_pkZShader->SetForceLighting(LIGHT_ALWAYS_OFF);
	if(m_pkInputHandle->VKIsDown("lightstd"))			m_pkZShader->SetForceLighting(LIGHT_MATERIAL);
*/	
};

void MistServer::OnHud(void)
{
	m_pkFps->DevPrintf("common","Active Propertys: %d",m_pkObjectMan->GetActivePropertys());	
	m_pkFps->DevPrintf("common", "Fps: %f",m_pkFps->m_fFps);	
	m_pkFps->DevPrintf("common","Avrage Fps: %f",m_pkFps->m_fAvrageFps);			
		
	if(m_pkActiveCamera) {
		m_pkFps->DevPrintf("editor","Grid Size: %f", m_pkActiveCamera->m_fGridSpace);			
		m_pkFps->DevPrintf("editor","Grid Snap: %i", m_pkActiveCamera->m_bGridSnap);			
		m_pkFps->DevPrintf("editor","View: %s", m_pkActiveCamera->GetName().c_str());			
		}

	m_pkFps->m_bGuiMode = false;
	m_pkFps->ToggleGui();


}

void MistServer::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	ClientOrder kOrder;

	unsigned int i;
	vector<string>	kUsers;
	int iMode;
	float fTest;
	string strNewTitle;

	switch(cmdid) {
		case FID_NEW:
			m_pkObjectMan->Clear();
			//GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);
			m_pkFps->StartServer(true,true);
			m_strWorldDir = "";
			SetTitle("MistServer");
			break;
		
		case FID_LOAD:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				m_pkConsole->Printf("load [worlddir]");
				break;				
			}
			
			if(!m_pkObjectMan->LoadWorld(kCommand->m_kSplitCommand[1]))
			{
				cout<<"Error loading world"<<endl;
				break;
			}				
			
			// Set the title to include the world name.
			m_strWorldDir = kCommand->m_kSplitCommand[1];
			strNewTitle = "MistServer - " + m_strWorldDir;
			SetTitle(strNewTitle);

			/*			if(kCommand->m_kSplitCommand.size() <= 1)
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
			
*/						
			cout<<"starting server"<<endl;
			//GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);			
			m_pkFps->StartServer(true,true);
			
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

			if(!m_pkObjectMan->SaveWorld(m_strWorldDir,true))
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
			
			if(!m_pkObjectMan->SaveWorld(kCommand->m_kSplitCommand[1],true))
			{
				m_pkConsole->Printf("Error saving world");
				break;
			}	

			// Set the title to include the world name.
			m_strWorldDir = kCommand->m_kSplitCommand[1];
			strNewTitle = "MistServer - " + m_strWorldDir;
			SetTitle(strNewTitle);

/*			cout<<"saving world:"<<endl;
			m_pkObjectMan->ForceSave();
			m_pkObjectMan->SaveZones();			
			cout<<"saved"<<endl;
*/			
			break;

		case FID_USERS:
			kUsers = m_pkPlayerDB->GetUsers();
			for(i=0; i<kUsers.size(); i++) {
				cout << "User: " << kUsers[i] << endl;
				}
			break;		

		case FID_TEST_JIDDRA:
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


		case FID_LOCALORDER:
			string strSo;
			strSo = kCommand->m_strFullCommand;
			strSo.erase(0, kCommand->m_kSplitCommand[0].length() + 1);
			//SendOrder( strSo );	
			//m_pkConsole->Printf("SO is = %s", strSo.c_str());

			//char szFullCmd[1024];
			//sprintf(szFullCmd, "addzone %f %f %f %f %f %f %s",m_kZoneMarkerPos.x,m_kZoneMarkerPos.y,m_kZoneMarkerPos.z,
			//	m_kZoneSize.x,m_kZoneSize.y,m_kZoneSize.z, m_strActiveZoneName.c_str());
			//cout << "FullCmd " << szFullCmd << endl;
			
			P_ClientControl pkClient;
			string strOrder;
			kOrder.m_sOrderName = strSo;
			kOrder.m_iCharacter = -1;
			cout << "Sending LocalOrder: " << kOrder.m_sOrderName << "\n";
			pkClient.AddServerOrder(kOrder);
			break;		

	}

}

void MistServer::ClientInit()
{
	cout<<"Client Join granted"<<endl;
	
	cout<<"Join Complete"<<endl;
}

bool MistServer::OnPreConnect(IPaddress kRemoteIp, char* szLogin, char* szPass)
{
	cout << "MistServer::OnPreConnect" << endl;
	//dessa skall du fixa till vim =D
	string strPlayer		= szLogin;
	string strPasswd		= szPass;
	string strCharacter	= "mrbad";
	
	// Check that this is a valid login.
	if(m_pkPlayerDB->LoginExist(strPlayer)) {
		// Check Password
		if(!m_pkPlayerDB->Login(strPlayer,strPasswd)) {
			return false;
			}
		// Login Ok.

		}
	else {
		/* Failed to find the login. */
		cout << "Login not found" << endl;

		if(m_AcceptNewLogins) {
			if(!m_pkPlayerDB->CreatePlayer(strPlayer,strPasswd)) {
				return false;
				}
	
			m_pkPlayerDB->Login(strPlayer,strPasswd);
			}
		else 
			return false;
		}


	if(m_pkServerInfoP)
	{
		if(m_pkServerInfoP->PlayerExist(strPlayer))
		{
			cout<<"Player "<<strPlayer<< " is already connected"<<endl;
		
			//KICKA O DÖDA SPELARJÄVELN HÄR OCKSÅ FÖR HAN FÅR FAN INTE CONNECTA TVÅ GÅNGER!!!!
		
			return false;
		}
	}
	
	printf("User %s tried to join with password %s\n", szLogin, szPass);
	return true;
}

void MistServer::OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass)
{
	cout << "MistServer::OnServerClientJoin" << endl;
	//dessa skall du fixa till vim =D
	string strPlayer		= szLogin;
	string strPasswd		= szPass;

	pkClient->m_strCharacter = "MrBadq";
	pkClient->m_strLogin = szLogin;

	//add client control to client object
	P_ClientControl* pcc = (P_ClientControl*)pkClient->m_pkObject->AddProperty("P_ClientControl");
	if(pcc)	
		pcc->m_iClientID = iConID;
	
	bool bEditorConnect = true;
	if(bEditorConnect) {
		P_Track* pkTrack = dynamic_cast<P_Track*>((P_ClientControl*)pkClient->m_pkObject->AddProperty("P_Track"));
		pkTrack->SetClient(iConID);
		(P_ClientControl*)pkClient->m_pkObject->AddProperty("P_Primitives3D");
		return;
		}
	else
		SpawnPlayer(iConID);
}

void MistServer::SpawnPlayer(int iConID)
{
	cout << "MistServer::SpawnPlayer" << endl;
	if(m_pkFps->m_kClient[iConID].m_strCharacter.size() == 0) {
		m_pkFps->PrintToClient(iConID, "You must select a character before you can join" );
		return;
		}

	//update start locations  
	UpdateStartLocatons();

	//create player object
	int iPlayerID  = CreatePlayer(m_pkFps->m_kClient[iConID].m_strLogin.c_str(),m_pkFps->m_kClient[iConID].m_strCharacter.c_str(),"Start",iConID);
	
	if(iPlayerID == -1)
	{
		cout<<"Error creating playercharacter"<<endl;
	}
	
	if(m_pkServerInfoP)
	{	
		//wich rights shuld a client have on its player caracter
		//int playerrights = PR_OWNER|PR_CONTROLS|PR_LOOKAT;
		
		m_pkServerInfoP->AddPlayer(iConID, m_pkFps->m_kClient[iConID].m_strLogin.c_str());
		m_pkServerInfoP->SetCharacterID(iConID,iPlayerID);
		//m_pkServerInfoP->AddObject(iConID,iPlayerID,playerrights);
	}
}



void MistServer::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	DeletePlayer(iConID);

	if(m_pkServerInfoP)
		m_pkServerInfoP->RemovePlayer(iConID);	
	
	m_pkPlayerDB->Logout(pkClient->m_strLogin);
	cout<<"Client "<<iConID<<" Parted"<<endl;	
}


void MistServer::OnServerStart(void)
{		
	CreateEditCameras();

	//create server info object
	m_pkServerInfo = m_pkObjectMan->CreateObjectFromScript("data/script/objects/t_serverinfo.lua");
	if(m_pkServerInfo)
	{
		m_pkServerInfo->SetParent(m_pkObjectMan->GetGlobalObject());
		m_pkServerInfoP = (P_ServerInfo*)m_pkServerInfo->GetProperty("P_ServerInfo");		
		if(m_pkServerInfoP)
		{
			m_pkServerInfoP->SetServerName("Test Server");
		}
		else
			cout<<"ERROR: No server P_ServerInfo property created, this is no good"<<endl;
	}
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

void MistServer::UpdateStartLocatons()
{
	m_kLocations.clear();

	pair<string,Vector3> temp("Start",Vector3(0,1,0));
	m_kLocations.push_back(temp);
}


Vector3 MistServer::GetPlayerStartLocation(const char* csName)
{
	for(unsigned int i=0;i<m_kLocations.size();i++)
		if(m_kLocations[i].first == csName)
		{	
			cout<<"found location: "<<csName<<endl;
			return m_kLocations[i].second+Vector3( float((rand()%1000)/1000.0),  0,  float((rand()%1000)/1000.0));
		}
	
	return Vector3(0,0,0);
}

int MistServer::CreatePlayer(const char* csPlayer,const char* csCharacter,const char* csLocation,int iConID)
{

	//try to create character entity
	Entity* pkObject = m_pkPlayerDB->CreateCharacter(csPlayer,csCharacter);
	
	
	//if it fails try to create it
	if(!pkObject)
	{
		cout<<"Character not found, trying to create it"<<endl;
		if(!m_pkPlayerDB->CreateNewCharacter(csPlayer,csCharacter))
			return -1;
		else	//it was created, now load it
			pkObject = m_pkPlayerDB->CreateCharacter(csPlayer,csCharacter);
	}
	
	
	if(pkObject)
	{	
		Vector3 kStartPos = Vector3(0,3,0);
				
		//try to get recal position from characterstats
		CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
      if(pkCP)
  	   {
   	   CharacterStats *pkCS = pkCP->GetCharStats();	
			kStartPos = pkCS->GetRecalPos();		
		}	
		
		//make sure position is valid and zone is loaded
		int zid = m_pkObjectMan->GetZoneIndex(kStartPos,-1,false);
		if(zid == -1)
		{
			cout<<"Error Character "<<csPlayer<<" -> "<<csCharacter<<" Tryed to start in a invalid location,trying 0,1,0"<<endl;
			kStartPos = Vector3(0,0,0);
			zid = m_pkObjectMan->GetZoneIndex(kStartPos,-1,false);						
		}		
		
		
		//force loading of zone
		m_pkObjectMan->LoadZone(zid);
		
		//finaly set objects position
		pkObject->SetWorldPosV(kStartPos);
		
		cout<<"setting char pos:"<<kStartPos.x<<" "<<kStartPos.y<<" "<<kStartPos.z<<endl;
		
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
			
	return pkObject->GetEntityID();
}

void MistServer::DeletePlayer(int iConID)
{
	if(m_pkServerInfoP)
	{
		PlayerInfo* pi = m_pkServerInfoP->GetPlayerInfo(iConID);
		if(pi)
		{
			//first save and delete the player character
			Entity* pkObj = m_pkObjectMan->GetObjectByNetWorkID(pi->iCharacterObjectID);
			if(pkObj)
			{
				m_pkPlayerDB->SaveCharacter(pkObj,pi->sPlayerName);
				m_pkObjectMan->Delete(pkObj);
			}
		
			//then walk trough all characters in his control list and delete the ones the player have spawned
			for(unsigned int i = 0;i<pi->kControl.size();i++)
			{
				if(pi->kControl[i].second & PR_OWNER)
				{
					Entity* pkObj = m_pkObjectMan->GetObjectByNetWorkID(pi->kControl[i].first);
					
										
					//delete it
					if(pkObj)
						m_pkObjectMan->Delete(pkObj);
				}
			}		
		}
	}
}

void MistServer::HSO_Character(ClientOrder* pkOrder)
{
	CmdArgument kcmdargs;
	kcmdargs.Set(pkOrder->m_sOrderName.c_str());
	
	cout << "kcmdargs.m_kSplitCommand[1] " << kcmdargs.m_kSplitCommand[1].c_str(); 

	if ( kcmdargs.m_kSplitCommand[1] == string("Play") )
	{
		cout << "Player: " << pkOrder->m_iConnectID << " wish to start play" << endl;
		SpawnPlayer(pkOrder->m_iConnectID);
	}
	else if ( kcmdargs.m_kSplitCommand[1] == string("CharList") )
	{
		cout << "Player: " << int(pkOrder->m_iConnectID) << " wish to know what char he have." << endl;
		vector<string> kChars;
		kChars = m_pkPlayerDB->GetLoginCharacters( m_pkFps->m_kClient[pkOrder->m_iConnectID].m_strLogin.c_str() );
		for(unsigned int i=0; i<kChars.size(); i++)
			m_pkFps->PrintToClient(pkOrder->m_iConnectID, kChars[i].c_str());
	}
	else if ( kcmdargs.m_kSplitCommand[1] == string("Select") )
	{
		cout << "Player: " << int(pkOrder->m_iConnectID) << " wish to use char '" << kcmdargs.m_kSplitCommand[2].c_str()  << "'" << endl;
		m_pkFps->m_kClient[pkOrder->m_iConnectID].m_strCharacter = kcmdargs.m_kSplitCommand[2].c_str();
	}

	// WHO:		Kolla vilken karaktär man är.
	// LWHO:	Kolla vilket login man är.
}


/*
	Handle Server Orders, that is commands sent from clients to the server.
*/			
void MistServer::HandleOrders()
{
	//cout<<"nr of orders: "<<P_ClientControl::NrOfOrders()<<endl;	
	
	while(P_ClientControl::NrOfOrders() > 0 )
	{
		ClientOrder* order = P_ClientControl::GetNextOrder();	
		
		if(!CheckValidOrder(order))
		{
			cout << "Bad order from:" << order->m_iClientID << endl;
			P_ClientControl::PopOrder();
			continue;
		}
		
		cout << "handling order "<<order->m_sOrderName<<" from client:" << order->m_iClientID << endl;
		
		// Edit Order
		cout << order->m_sOrderName.c_str() << endl;

		// Character Command
		if(strncmp(order->m_sOrderName.c_str(),"CC",2) == 0) {
			HSO_Character(order);
			}
		
		// OLD UNFIXED ORDERS BELOW.
		//handle input messages from client
		else if(strncmp(order->m_sOrderName.c_str(),"(IM)",4) == 0) 
		{
			order->m_sOrderName.erase(0,4);
			string playername=""; 
			string message="";
					
			unsigned int pos=0;
			
			if(strncmp(order->m_sOrderName.c_str(),"/w",2)==0)
			{
				for(pos=3;pos<order->m_sOrderName.size();pos++)
				{
					if(order->m_sOrderName[pos] == ' ' && playername.size()>0)
						break;
					playername+=order->m_sOrderName[pos];
				}
			}
			
			for(;pos<order->m_sOrderName.size();pos++)
			{	
				message+=order->m_sOrderName[pos];
			}
			 
			//cout<<"got message to "<<playername<<": "<<message<<endl;
			
			if(m_pkServerInfoP)
			{
				PlayerInfo* pi = m_pkServerInfoP->GetPlayerInfo(order->m_iClientID);
				
				if(pi)
				{
					message = pi->sPlayerName + " : " + message;
				
				}
				
				if(playername == "")
					m_pkServerInfoP->MessagePlayer(-1,message);
				else
					m_pkServerInfoP->MessagePlayer(playername.c_str(),message);
			}
		}
		else if(strncmp(order->m_sOrderName.c_str(),"G_",2)==0)
		{
			cout<<"Got ground click order"<<endl;
		
			Entity* ob = m_pkObjectMan->GetObjectByNetWorkID(order->m_iCharacter);			
		
			if(ob)
			{
				P_ScriptInterface* pe = (P_ScriptInterface*)ob->GetProperty("P_ScriptInterface");
				if(pe)
					pe->SendGroudClickEvent(order->m_sOrderName.c_str(), order->m_kPos,order->m_iCharacter);
			}					
		}
      
      // equip
      else if ( order->m_sOrderName == "equip" )
      {
   		Entity* pkChar = m_pkObjectMan->GetObjectByNetWorkID(order->m_iCharacter);

         if ( pkChar )
         {
            // get item to equip
            Entity* pkItem = m_pkObjectMan->GetObjectByNetWorkID(order->m_iObjectID);

            CharacterProperty* pkCP = (CharacterProperty*)pkChar->GetProperty ("P_CharStats");
            P_Item* pkIP = (P_Item*)pkItem->GetProperty("P_Item");

            if ( pkIP && pkCP )
               pkCP->GetCharStats()->Equip( pkItem, order->m_iUseLess );              
         }
      }

	  // request orders
      else if ( order->m_sOrderName == "(rq)item" )    
      {
   		Entity* pkItemObject = m_pkObjectMan->GetObjectByNetWorkID(order->m_iObjectID);

         if ( pkItemObject )
         {
            P_Item *pkItProp = (P_Item*) pkItemObject->GetProperty("P_Item");
            
            if ( pkItProp )
            {
               // if the items is of the same version, no need to send data
                 if ( pkItProp->m_pkItemStats->m_uiVersion != order->m_iUseLess )				//DVOID WAS HERE
                 {
                     SendType kSendType;
                     kSendType.m_iClientID = order->m_iClientID;
                     kSendType.m_kSendType = "itemdata";
                  
                     pkItProp->AddSendsData ( kSendType );
                 }

            }
            else
               cout << "Error! Non-P_Item_Object requested for updated iteminfo! This should't be possible!!!" << endl;
         }
      }
      
      // container request
      else if ( order->m_sOrderName == "(rq)cont" )
      {
         cout << "Sever hgot cont req" << endl;

   		Entity* pkObject = m_pkObjectMan->GetObjectByNetWorkID(order->m_iObjectID);
         
         if ( pkObject )
         {

            P_Container* pkC = (P_Container*) pkObject->GetProperty("P_Container");

            if ( pkC )
            {
               // check versions...
               //if ( pkC->m_uiVersion != order->m_iUseLess )
                  pkC->AddSendsData(order->m_iClientID);
            }            
            else
               cout << "Error! Non-P_Container requested for updated containerinfo!" << endl;
         }
      }
      // request character skills
      else if ( order->m_sOrderName == "(rq)skil" )
      {
           // type of request
   		Entity* pkCharObject = m_pkObjectMan->GetObjectByNetWorkID(order->m_iObjectID);

         if ( pkCharObject  )
         {
            CharacterProperty *pkCP = (CharacterProperty*) pkCharObject ->GetProperty("P_CharStats");
            
            if ( pkCP )
            {
               // if the items is of the same version, no need to send data
                 //if ( pkCP->GetCharStats()->m_uiVersion != order->m_iUseLess )
                 //{
                     SendType kSendType;
                     kSendType.m_iClientID = order->m_iClientID;
                     kSendType.m_kSendType = "skills";
                  
                     pkCP->AddSendsData ( kSendType );
                 //}

            }
            else
               cout << "Error! Non-P_Charstats_Object requested for updated iteminfo! This should't be possible!!!" << endl;
         }
      } 
      // request character data
      else if ( order->m_sOrderName == "(rq)cdat" )
      {
           // type of request
   		Entity* pkCharObject = m_pkObjectMan->GetObjectByNetWorkID(order->m_iObjectID);

         if ( pkCharObject  )
         {
            CharacterProperty *pkCP = (CharacterProperty*) pkCharObject ->GetProperty("P_CharStats");
            
            if ( pkCP )
            {
               // if the items is of the same version, no need to send data
               //  if ( pkCP->GetCharStats()->m_uiVersion != order->m_iUseLess )
               //  {
                     SendType kSendType;
                     kSendType.m_iClientID = order->m_iClientID;
                     kSendType.m_kSendType = "data";
                  
                     pkCP->AddSendsData ( kSendType );
               //  }

            }
            else
               cout << "Error! Non-P_Charstats_Object requested for updated iteminfo! This should't be possible!!!" << endl;
         }
      }  
      // request character skills
      else if ( order->m_sOrderName == "(rq)attr" )
      {
           // type of request
   		Entity* pkCharObject = m_pkObjectMan->GetObjectByNetWorkID(order->m_iObjectID);

         if ( pkCharObject  )
         {
            CharacterProperty *pkCP = (CharacterProperty*) pkCharObject ->GetProperty("P_CharStats");
            
            if ( pkCP )
            {
               // if the items is of the same version, no need to send data
                 //if ( pkCP->GetCharStats()->m_uiVersion != order->m_iUseLess )				//DVOID WAS HERE
                 //{
                     SendType kSendType;
                     kSendType.m_iClientID = order->m_iClientID;
                     kSendType.m_kSendType = "attributes";
                  
                     pkCP->AddSendsData ( kSendType );
                 //}

            }
            else
               cout << "Error! Non-P_Charstats_Object requested for updated iteminfo! This should't be possible!!!" << endl;
         }
      }  


      // drop item from inventory to ground
      else if ( order->m_sOrderName == "DropItem" )
      {
         Entity* pkEntity = m_pkObjectMan->GetObjectByNetWorkID(order->m_iObjectID);
         Entity* pkPlayer = m_pkObjectMan->GetObjectByNetWorkID(order->m_iCharacter);

         if ( pkEntity && pkPlayer )
         {
            pkEntity->SetUpdateStatus (UPDATE_ALL);
            pkEntity->GetParent()->RemoveChild (pkEntity);

            pkEntity->SetWorldPosV ( pkPlayer->GetWorldPosV() );

            ((P_Container*)pkPlayer->GetProperty("P_Container"))->RemoveObject(order->m_iObjectID);         
         }

         if ( !pkEntity )
            cout << "Error! Client wanted to drop a non-existing item!" << endl;
      }  
		
		//normal orders
		else if(order->m_iObjectID != -1)
		{
			Entity* ob = m_pkObjectMan->GetObjectByNetWorkID(order->m_iObjectID);
			if(ob)
			{
			
				P_ScriptInterface* pe = (P_ScriptInterface*)ob->GetProperty("P_ScriptInterface");
				if(pe)
				{	
					pe->SendObjectClickEvent(order->m_sOrderName.c_str(), order->m_iCharacter);				
				
				}			
			}
		}
		else
		{
			cout<<"CLICK"<<endl;

		
		}
		
		
		P_ClientControl::PopOrder();
	} 
}

bool MistServer::CheckValidOrder(ClientOrder* pkOrder)
{
	if(pkOrder->m_iCharacter == -1)
		return true;
		
	if(m_pkServerInfoP)
	{
		if(pkOrder->m_iCharacter  == m_pkServerInfoP->GetCharacterID(pkOrder->m_iClientID))
			return true;
		
/*	
		PlayerInfo* pi = m_pkServerInfoP->GetPlayerInfo(pkOrder->m_iClientID);
		
		if(pi)
		{
			for(unsigned int i = 0;i<pi->kControl.size();i++)
			{
				//found objectID
				if(pi->kControl[i].first == pkOrder->m_iCharacter)
					if(pi->kControl[i].second & PR_CONTROLS)
						return true;				
			}
		}
*/		
	}
	
	return false;
}

void MistServer::SendTextToMistClientInfoBox(char *szText)
{
	  
}
