/**
	\defgroup ZeroTank ZeroTank - ZeroFPS Test App

	ZeroTank is the main test application to test ZeroFPS.
*/


#include "zerotank.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../zerofpsv2/engine_systems/common/heightmap.h"
#include "../zerofpsv2/engine_systems/propertys/madproperty.h"
#include "../zerofpsv2/engine_systems/propertys/primitives3d.h"
#include "../zerofpsv2/engine_systems/propertys/p_ambientsound.h"
#include "../zerofpsv2/engine_systems/propertys/proxyproperty.h"
#include "../zerofpsv2/gui/zgui.h"

ZeroTank g_kZeroTank("ZeroTank",0,0,0);

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
	switch(msg)
	{
	case ZGM_COMMAND:
		g_kZeroTank.OnCommand(((int*)params)[0], win);
		break;
	}
	return true;
}

ZeroTank::ZeroTank(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 

	m_iSelfObjectID			= -1;
	m_HaveFoundHMapObject	= false;
	
	g_ZFObjSys.Log_Create("zerorts");
 
	m_pkZeroTankHull		= NULL;
	m_pkZeroTank_Modify	= NULL;
	m_pkZeroTankClientObject = NULL;
	m_pkGoblinLord = NULL;
	m_iContollIndex = 0;


} 

void ZeroTank::OnInit() 
{
	pkConsole->Printf(" ZeroTank");
	pkConsole->Printf("--------------------------------");
	pkConsole->Printf(" Can we shoot yet?");

	Init();

	//run autoexec script
	if(!pkIni->ExecuteCommands("zerotank_autoexec.ini"))
		pkConsole->Printf("No game_autoexec.ini.ini found");

}

void ZeroTank::Init()
{	
	//register commmands bös
	Register_Cmd("massspawn",FID_MASSSPAWN);			

	//damn "#¤(="%#( lighting fix bös
	glEnable(GL_LIGHTING );
	
	//initiate our camera bös
	m_pkCamera=new Camera(Vector3(0,10,0),Vector3(0,0,0),70,1.333,0.25,250);	
//	m_pkCamera->SetViewPort(0,0.24,1,0.76);
//	m_pkCamera=new Camera(Vector3(0,10,0),Vector3(0,0,0),85,1.333,0.25,250);	
	
	//disable zones modells bös
//	pkLevelMan->SetVisibleZones(false);

	//register property bös
	RegisterPropertys();

	GuiAppLua::Init(&g_kZeroTank, GetScript());
	InitializeGui(pkGui, pkTexMan, pkScript, pkGuiMan,
		"data/textures/text/paternoster8.bmp",
		"data/script/gui/gui_create_client.lua");

	// Set SDL Options.
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("ZeroFps", NULL);
}

void ZeroTank::RegisterPropertys()
{

}

void ZeroTank::OnIdle() 
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	
			
	Input();
	
 	pkFps->UpdateCamera(); 	
	
	//update player possition 
	if(pkFps->m_bClientMode && !pkFps->m_bServerMode) {
		if(!m_pkZeroTankClientObject) {
			m_pkZeroTankClientObject = pkObjectMan->GetObjectByNetWorkID( m_iSelfObjectID );
			if(m_pkZeroTankClientObject) {
				cout << "Create Client Object" << m_iSelfObjectID << endl;
				m_pkZeroTankClientObject->AddProperty("CameraProperty");
				CameraProperty* cam = (CameraProperty*)m_pkZeroTankClientObject->GetProperty("CameraProperty");
				cam->SetCamera(m_pkCamera);
				}
			}
		else {
			m_pkZeroTank_Modify = m_pkZeroTankClientObject;
			pkObjectMan->OwnerShip_Take( m_pkZeroTankClientObject );
			m_pkZeroTankClientObject->SetWorldPosV(m_pkZeroTankClientObject->GetLocalPosV());	// **************** CLIENT CANT MOVE ERROR VIM ****************
		}
	}	

	if(m_pkGoblinLord)
		m_pkGoblinSlave->SetLocalRotM( m_pkGoblinLord->GetLocalRotM() );

/*	if(pkObj) {
		m_pkZeroTank_Modify = pkObj;
		pkObjectMan->OwnerShip_Take( pkObj );
//		pkObj->SetWorldPosV(pkFps->GetCam()->GetPos());
		pkObj->SetWorldPosV(m_pkZeroTank_Modify->GetLocalPosV());	// **************** CLIENT CANT MOVE ERROR VIM ****************
	}*/
}

void ZeroTank::OnSystem() 
{
	//setup client
	if(pkFps->m_bClientMode && !pkFps->m_bServerMode)
	{
		GetSystem().Logf("net","??? m_iSelfObjectID %d\n", m_iSelfObjectID);

		//try to get self id
		if(m_iSelfObjectID == -1)
			m_iSelfObjectID = pkFps->GetClientObjectID();
	};
}

		int					m_iContollIndex;
		
void ZeroTank::NextControllObject()
{
	m_iContollIndex++;

	if(m_iContollIndex >= m_kEditList.size())
		m_iContollIndex = 0;

	m_pkZeroTank_Modify = m_kEditList[ m_iContollIndex ];

	if(m_pkZeroTank_Modify) {
		cout << "Selected : " << m_pkZeroTank_Modify->iNetWorkID << endl;
		}
}

void ZeroTank::Input()
{
	const int PRESSED_KEY = pkInput->GetQueuedKey();

	switch(PRESSED_KEY)
	{
	case KEY_F1:
		pkAudioSys->StartSound("data/sound/dummy.wav", pkFps->GetCam()->GetPos(), Vector3(0,0,1), true);
		break;
	case KEY_F5:
		pkAudioSys->StopSound("data/sound/dummy.wav", pkFps->GetCam()->GetPos());
		break;
	case KEY_F2:
		pkAudioSys->StartSound("data/sound/walk.wav", pkFps->GetCam()->GetPos(), Vector3(0,0,1), true);
		break;
	case KEY_F6:
		pkAudioSys->StopSound("data/sound/walk.wav", pkFps->GetCam()->GetPos());
		break;
	case KEY_F9:
		printf("Num sounds in system = %i\nNum active channels = %i\n",
			pkAudioSys->GetNumSounds(), pkAudioSys->GetNumActiveChannels());
		break;
	case KEY_F10:
		pkAudioSys->UnloadAll();
		break;
	}


	int mx,my;
	pkInput->MouseXY(mx,my);

	static int s_iCursorTex=-1; 
	static int s_iCursorTex_a=-1;
	int iNewCursorTex=0;
	int iNewCursorTex_a=0;

	int x,z;		
	pkInput->RelMouseXY(x,z);
	
	float speed = 20;

	// Switch Modify Object
	if(pkInput->Pressed(KEY_1))	m_pkZeroTank_Modify = m_pkCameraObject;
	if(pkInput->Pressed(KEY_2))	NextControllObject();

	if(m_pkZeroTank_Modify)
	{
		if(pkInput->Pressed(KEY_X)){
			speed*=0.25;
		}
	
		float fSpeedScale = pkFps->GetFrameTime()*speed;

		Vector3 newpos = m_pkZeroTank_Modify->GetLocalPosV();
		Vector3 rot;
		rot.Set(0,0,0);
		
		Matrix4 kRm = m_pkZeroTank_Modify->GetLocalRotM();

		if(m_pkZeroTank_Modify == m_pkCameraObject ||
			 m_pkZeroTank_Modify == m_pkZeroTankClientObject) 
			kRm.Transponse();
	
		if(pkInput->Pressed(KEY_D))	newpos += kRm.GetAxis(0) * fSpeedScale;		
		if(pkInput->Pressed(KEY_A))	newpos += kRm.GetAxis(0) * -fSpeedScale;		
		if(pkInput->Pressed(KEY_W))	newpos += kRm.GetAxis(2) * -fSpeedScale;
		if(pkInput->Pressed(KEY_S))	newpos += kRm.GetAxis(2) * fSpeedScale;	
	
		if(pkInput->Pressed(KEY_Q))	rot.z += 5 * fSpeedScale;
		if(pkInput->Pressed(KEY_E))	rot.z -= 5 * fSpeedScale;
		
		rot.x += z / 5.0;
		rot.y -= x / 5.0;	

		m_pkZeroTank_Modify->SetLocalPosV(newpos);
		m_pkZeroTank_Modify->RotateLocalRotV(rot);
	
	}

	if(pkInput->Pressed(KEY_C) && m_pkZeroTankHull) {
		pkObjectMan->Delete(m_pkZeroTankHull);
		//m_pkZeroTankHull		= NULL;
		//m_pkZeroTank_Modify	= NULL;
		}
}

void ZeroTank::OnHud(void) 
{	
	pkFps->DevPrintf("common","Active Propertys: %d",pkObjectMan->GetActivePropertys());	
	pkFps->DevPrintf("common", "Fps: %f",pkFps->m_fFps);	
	pkFps->DevPrintf("common","Avrage Fps: %f",pkFps->m_fAvrageFps);			
	pkFps->DevPrintf("common","SelfID: %d", m_iSelfObjectID);	
	
	if(m_pkZeroTankTrack) {
		TrackProperty* pkTrack = dynamic_cast<TrackProperty*>(m_pkZeroTankTrack->GetProperty("TrackProperty"));

		for(set<int>::iterator it = pkTrack->m_iActiveZones.begin(); it != pkTrack->m_iActiveZones.end(); it++) {
			pkFps->DevPrintf("common"," %d", (*it));	
			}

		}
	
	pkFps->m_bGuiMode = false;
	pkFps->ToggleGui();
}

void ZeroTank::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	int i;

	switch(cmdid) {
		case FID_MASSSPAWN:
			for(i=0; i < 64;i++) {
				m_pkZeroTankTrack = pkObjectMan->CreateObjectByArchType("TrackObject");
				if(m_pkZeroTankTrack) {
					int iRandZone = rand() % pkObjectMan->GetNumOfZones();
					Vector3 kPos = pkObjectMan->GetZoneCenter(iRandZone);
					kPos.y = -4;
					m_pkZeroTankTrack->SetWorldPosV( kPos );
					m_pkZeroTankTrack->SetParent(pkObjectMan->GetWorldObject());
					pkObjectMan->AddTracker(m_pkZeroTankTrack);
				}
			}
			break;
	}
}




void ZeroTank::HandleOrders()
{

}

void ZeroTank::ClientInit()
{
	cout<<"Client Join granted"<<endl;
//	int mapwidth = pkLevelMan->GetHeightMap()->GetSize();
//	cout<<"Mapsize is :"<<mapwidth<<endl;
//	m_pkMap = pkLevelMan->GetHeightMap();	
	cout<<"Join Complete"<<endl;
}

void ZeroTank::OnServerClientJoin(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Joined"<<endl;
	pkClient->m_pkObject->AddProperty("P_Primitives3D");	
	cout << "Now adding tracker to client" << endl;
	pkClient->m_pkObject->AddProperty("TrackProperty");	
}

void ZeroTank::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Parted"<<endl;	
}

void ZeroTank::OnServerStart(void)
{		
	pkObjectMan->Test_CreateZones();
	m_pkZeroTankHull	= NULL;
	m_pkCameraObject	= NULL;

	// Create our camera.
	m_pkCameraObject = pkObjectMan->CreateObjectByArchType("camera");
	if(m_pkCameraObject)
	{
		m_pkCameraObject->SetUseZones(false);
		//m_pkCameraObject->SetParent(m_pkZeroTankGun);
		//m_pkCameraObject->SetRelativeOri(true);
		m_pkCameraObject->AddProperty("TrackProperty");
		m_pkCameraObject->SetParent(pkObjectMan->GetWorldObject());
		//m_pkCameraObject->AttachToClosestZone();
		CameraProperty* cam = (CameraProperty*)m_pkCameraObject->GetProperty("CameraProperty");
		cam->SetCamera(m_pkCamera);
	}

	// Create test objects.
	Object* pkObj;
	Object* pkObj2;


	m_pkZeroTankHull = pkObjectMan->CreateObjectByArchType("Goblin");
	if(m_pkZeroTankHull) {
		m_kEditList.push_back( m_pkZeroTankHull );
		cout << "Goblin: " << m_pkZeroTankHull->iNetWorkID << endl;
		//m_pkZeroTankHull->SetWorldPosV(Vector3(8,10,7));
		m_pkZeroTankHull->SetParent(pkObjectMan->GetWorldObject());
		//m_pkZeroTankHull->AddProperty("CameraProperty");
		//CameraProperty* cam = (CameraProperty*)m_pkZeroTankHull->GetProperty("CameraProperty");
		//cam->SetCamera(m_pkCamera);
	}

		pkObj = pkObjectMan->CreateObjectByArchType("vimsshield");
		if(pkObj) {
			pkObj->SetParent(m_pkZeroTankHull);
			pkObj->SetRelativeOri(true);
			pkObj->SetLocalPosV(Vector3(0,0,0));
		}

		pkObj = pkObjectMan->CreateObjectByArchType("vimsword");
		if(pkObj) {
			pkObj->SetParent(m_pkZeroTankHull);
			pkObj->SetRelativeOri(true);
			pkObj->SetLocalPosV(Vector3(0,0,0));
		}

	// Female warrior
	pkObj = pkObjectMan->CreateObjectByArchType("FWarrior");
	if(pkObj) {
		m_kEditList.push_back( pkObj );
		pkObj->SetWorldPosV ( Vector3 (0,0,0));
		pkObj->SetParent(pkObjectMan->GetWorldObject());
		}
	
		pkObj2 = pkObjectMan->CreateObjectByArchType("vimshield");
		if(pkObj) {
			pkObj2->SetParent(pkObj);
			pkObj2->SetRelativeOri(true);
			pkObj2->SetLocalPosV(Vector3(0,0,0));
		}

		pkObj2 = pkObjectMan->CreateObjectByArchType("vimsword");
		if(pkObj2) {
			pkObj2->SetParent(pkObj);
			pkObj2->SetRelativeOri(true);
			pkObj2->SetLocalPosV(Vector3(0,0,0));
		}

	m_pkGoblinLord =  pkObjectMan->CreateObjectByArchType("Goblin");
	m_pkGoblinLord->SetWorldPosV ( Vector3 (20,30,20) );
	m_pkGoblinLord->SetParent(pkObjectMan->GetWorldObject());
	m_pkGoblinLord->AddProperty("P_Primitives3D");

	m_pkGoblinSlave =  pkObjectMan->CreateObjectByArchType("Goblin");
	m_pkGoblinSlave->SetWorldPosV ( Vector3 (20,30,20) );
	m_pkGoblinSlave->SetParent(pkObjectMan->GetWorldObject());
	
	P_AmbientSound* pkSound = (P_AmbientSound*) m_pkGoblinLord->AddProperty("P_AmbientSound");
	pkSound->SetSound("data/sound/walk.wav", true, 2);

	pkConsole->Printf("Num of Zones: %d",pkObjectMan->GetNumOfZones());

}

void ZeroTank::OnClientStart(void)
{
}

bool ZeroTank::StartUp()	{ return true; }
bool ZeroTank::ShutDown()	{ return true; }
bool ZeroTank::IsValid()	{ return true; }


void ZeroTank::OnCommand(int iID, ZGuiWnd *pkMainWnd)
{
	if(iID == 5)
		pkScript->Call(m_pkScriptResHandle , "OnClickBackpack", 0, 0); 
	if(iID == 6)
		pkScript->Call(m_pkScriptResHandle , "OnClickStats", 0, 0);
	if(iID == 4)
		pkScript->Call(m_pkScriptResHandle, "OnClickMap", 0, 0);
}
