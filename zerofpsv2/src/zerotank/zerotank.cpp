#include "zerotank.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../zerofpsv2/engine_systems/common/heightmap.h"
#include "../zerofpsv2/engine_systems/propertys/madproperty.h"
#include "../zerofpsv2/engine_systems/propertys/primitives3d.h"
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
	m_pkZeroTankTower		= NULL;
	m_pkZeroTankGun		= NULL;
	m_pkZeroTank_Modify	= NULL;
	m_pkZeroTankClientObject = NULL;

	assert(0);
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
	Register_Cmd("load",FID_LOAD);		
	Register_Cmd("unload",FID_UNLOAD);			
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

	m_pkTestObject = pkObjectMan->CreateObject();
	m_pkTestObject->SetLocalPosV(Vector3(0,-10,0));
	m_pkTestObject->AttachToClosestZone();
	m_pkTestObject->AddProperty(new P_Primitives3D(PYRAMID));
	
	GuiAppLua::Init(&g_kZeroTank, GetScript());
	InitializeGui(pkGui, pkTexMan, pkScript, pkGuiMan);

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

void ZeroTank::Input()
{
	const int PRESSED_KEY = pkInput->GetQueuedKey();

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
	if(pkInput->Pressed(KEY_2))	m_pkZeroTank_Modify = m_pkZeroTankHull;
	if(pkInput->Pressed(KEY_3))	m_pkZeroTank_Modify = m_pkZeroTankTower;
	if(pkInput->Pressed(KEY_4))	m_pkZeroTank_Modify = m_pkZeroTankGun;
	if(pkInput->Pressed(KEY_5))	m_pkZeroTank_Modify = m_pkZeroTankTrack;


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
	
		/*
		if(pkInput->Pressed(KEY_D))	newpos-=kRm.VectorRotate(Vector3(-1,0,0))	* fSpeedScale;		
		if(pkInput->Pressed(KEY_A))	newpos-=kRm.VectorRotate(Vector3(1,0,0))	* fSpeedScale;		
		if(pkInput->Pressed(KEY_W))	newpos-=kRm.VectorRotate(Vector3(0,0,1))	* fSpeedScale;
		if(pkInput->Pressed(KEY_S))	newpos-=kRm.VectorRotate(Vector3(0,0,-1)) * fSpeedScale;	
		*/

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

/*
	if(pkInput->Action(m_iActionPrintServerInfo))
	{
	}

	float speed = 20;

	//camera movements
	if(pkInput->Pressed(KEY_X)){
		speed*=0.25;
	}

	Vector3 newpos = m_pkCamera->GetPos();
	Vector3 rot = m_pkCamera->GetRot();
	
	if(pkInput->Pressed(KEY_D)){
		newpos.x+=cos((rot.y)/degtorad) *pkFps->GetFrameTime()*speed;			
		newpos.z+=sin((rot.y)/degtorad) *pkFps->GetFrameTime()*speed;				
	}
	if(pkInput->Pressed(KEY_A)){
		newpos.x+=cos((rot.y+180)/degtorad)*pkFps->GetFrameTime()*speed;			
		newpos.z+=sin((rot.y+180)/degtorad)*pkFps->GetFrameTime()*speed;				
	}	
	if(pkInput->Pressed(KEY_W))	{
		newpos.x+=cos((rot.y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
		newpos.z+=sin((rot.y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
	}					
	if(pkInput->Pressed(KEY_S))	{
		newpos.x+=cos((rot.y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;			
		newpos.z+=sin((rot.y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;
	}		
	
	if(pkInput->Pressed(KEY_Q))
		newpos.y+=2*pkFps->GetFrameTime()*speed;			
	if(pkInput->Pressed(KEY_E))
		newpos.y-=2*pkFps->GetFrameTime()*speed;

	int x,z;		
	pkInput->RelMouseXY(x,z);	
	
	rot.x+=z/5.0;
	rot.y+=x/5.0;	
	
//	m_pkME->SetLocalPosV(newpos);
//	m_pkME->SetLocalRotV(rot);	

	float fSpeedScale = pkFps->GetFrameTime()*speed;

	m_pkCamera->SetPos(newpos);
	m_pkCamera->SetRot(rot);
	

	static float fRotate = 0;
	static Vector3 kRotate(0,0,0); 
	
	if(m_pkZeroTank_Modify) {
		// Translate
		newpos = m_pkZeroTank_Modify->GetLocalPosV();

		if(pkInput->Pressed(KEY_H))	newpos.x +=	fSpeedScale;			
		if(pkInput->Pressed(KEY_F))	newpos.x -=	fSpeedScale;			
		if(pkInput->Pressed(KEY_R))	newpos.y +=	fSpeedScale;			
		if(pkInput->Pressed(KEY_Y))	newpos.y -=	fSpeedScale;			
		if(pkInput->Pressed(KEY_T))	newpos.z +=	fSpeedScale;			
		if(pkInput->Pressed(KEY_G))	newpos.z -=	fSpeedScale;			

		m_pkZeroTank_Modify->SetLocalPosV(newpos);		
	
		kRotate = m_pkZeroTank_Modify->GetLocalRotV();
		
		//cout<<"x:"<<kRotate.x<<" y:"<<kRotate.y<<" z:"<<kRotate.z<<endl;
		kRotate.Set(0,0,0);
		
		
		// Rotate
		if(pkInput->Pressed(KEY_J))	kRotate.x += fSpeedScale;			
		if(pkInput->Pressed(KEY_U))	kRotate.x -= fSpeedScale;			
		if(pkInput->Pressed(KEY_I))	kRotate.y += fSpeedScale;			
		if(pkInput->Pressed(KEY_K))	kRotate.y -= fSpeedScale;			
		if(pkInput->Pressed(KEY_O))	kRotate.z += fSpeedScale;			
		if(pkInput->Pressed(KEY_L))	kRotate.z -= fSpeedScale;			

		m_pkZeroTank_Modify->RotateLocalRotV(kRotate);
		//m_pkZeroTank_Modify->SetLocalRotV(kRotate);
	}
	
	if(PRESSED_KEY == KEY_R)
	{
		static int w = 0;
		if(w++ > 400-16) w = 0;
		ResizeWnd("ManaBarProgress", w, -1);		
	}	

*/
	
/*	
	if(m_pkME)	
	{	
newpos = m_pkME->GetLocalPosV();
//	rot = m_pkME->GetLocalRotV();
	
	if(pkInput->Pressed(KEY_H)){
		newpos.x+=pkFps->GetFrameTime()*speed;			
	}
	if(pkInput->Pressed(KEY_F)){
		newpos.x-=pkFps->GetFrameTime()*speed;			
	}	
	if(pkInput->Pressed(KEY_T))	{
		newpos.z+=pkFps->GetFrameTime()*speed;			
	}					
	if(pkInput->Pressed(KEY_G))	{
		newpos.z-=pkFps->GetFrameTime()*speed;
	}		

	if(pkInput->Pressed(KEY_H)){
		newpos.x+=cos((rot.y)/degtorad) *pkFps->GetFrameTime()*speed;			
		newpos.z+=sin((rot.y)/degtorad) *pkFps->GetFrameTime()*speed;				
	}
	if(pkInput->Pressed(KEY_F)){
		newpos.x+=cos((rot.y+180)/degtorad)*pkFps->GetFrameTime()*speed;			
		newpos.z+=sin((rot.y+180)/degtorad)*pkFps->GetFrameTime()*speed;				
	}	
	if(pkInput->Pressed(KEY_T))	{
		newpos.x+=cos((rot.y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
		newpos.z+=sin((rot.y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
	}					
	if(pkInput->Pressed(KEY_G))	{
		newpos.x+=cos((rot.y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;			
		newpos.z+=sin((rot.y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;
	}		
	
	if(pkInput->Pressed(KEY_R))
		newpos.y+=2*pkFps->GetFrameTime()*speed;			
	if(pkInput->Pressed(KEY_Y))
		newpos.y-=2*pkFps->GetFrameTime()*speed;
		

  if(pkInput->Pressed(KEY_U))
		rot.x += pkFps->GetFrameTime()*speed*10;	
	if(pkInput->Pressed(KEY_J))	
		rot.x -= pkFps->GetFrameTime()*speed*10;
	
	if(pkInput->Pressed(KEY_I))
		rot.y += pkFps->GetFrameTime()*speed*10;	
	if(pkInput->Pressed(KEY_K))	
		rot.y -= pkFps->GetFrameTime()*speed*10;
		 
	if(pkInput->Pressed(KEY_O))
		rot.z += pkFps->GetFrameTime()*speed*10;	
	if(pkInput->Pressed(KEY_L))	
		rot.z -= pkFps->GetFrameTime()*speed*10;

  m_pkME->SetLocalPosV(newpos);				
		//rot.Set(SDL_GetTicks()/50.0,SDL_GetTicks()/50.0,SDL_GetTicks()/50.0);
		
//	m_pkME->SetLocalPosV(newpos);		
	//rot.y+=0.1;

  rot.Set(SDL_GetTicks()/50.0,SDL_GetTicks()/50.0,SDL_GetTicks()/50.0);
	m_pkME->SetLocalRotV(rot);
	}
	*/
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
/*		case FID_LOAD:
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

			pkConsole->Printf("Everything is loaded ,Starting server");
			GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);	
			break;		
		
		case FID_UNLOAD:
			break;*/
	
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
//	pkObjectMan->AddTracker(pkClient->m_pkObject);
}

void ZeroTank::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Parted"<<endl;	
}

void ZeroTank::OnServerStart(void)
{		
	//pkObjectMan->NewWorld();
	//pkObjectMan->CreateZone();
	pkObjectMan->Test_CreateZones();

	// ZeroTank
 	m_pkZeroTankHull	= NULL;
	m_pkZeroTankTower	= NULL;
	m_pkZeroTankGun	= NULL;
	m_pkCameraObject	= NULL;

	m_pkZeroTankHull = pkObjectMan->CreateObjectByArchType("Goblin");
	if(m_pkZeroTankHull) {
		//m_pkZeroTankHull->SetWorldPosV(Vector3(8,10,7));
		m_pkZeroTankHull->AttachToClosestZone();

		//m_pkZeroTankHull->AddProperty("CameraProperty");
		//CameraProperty* cam = (CameraProperty*)m_pkZeroTankHull->GetProperty("CameraProperty");
		//cam->SetCamera(m_pkCamera);
	}

/*	m_pkZeroTankTower = pkObjectMan->CreateObjectByArchType("ZeroRTSTower");
	if(m_pkZeroTankTower) {
		m_pkZeroTankTower->SetParent(m_pkZeroTankHull);
		m_pkZeroTankTower->SetLocalPosV(Vector3(0,0.81,0));
		m_pkZeroTankTower->SetRelativeOri(true);
	}

	m_pkZeroTankGun = pkObjectMan->CreateObjectByArchType("ZeroRTSGun");
	if(m_pkZeroTankGun) {
		m_pkZeroTankGun->SetParent(m_pkZeroTankTower);
		m_pkZeroTankGun->SetLocalPosV(Vector3(-1.1,0.4,0));
		m_pkZeroTankGun->SetRelativeOri(true);
	}
*/

	m_pkCameraObject = pkObjectMan->CreateObjectByArchType("camera");
	if(m_pkCameraObject)
	{
		//m_pkCameraObject->SetParent(m_pkZeroTankGun);
		//m_pkCameraObject->SetRelativeOri(true);
		m_pkCameraObject->AddProperty("TrackProperty");
		m_pkCameraObject->AttachToClosestZone();
		CameraProperty* cam = (CameraProperty*)m_pkCameraObject->GetProperty("CameraProperty");
		cam->SetCamera(m_pkCamera);
	}

	pkConsole->Printf("Num of Zones: %d",pkObjectMan->GetNumOfZones());

	//add server info property
/*	if(!pkObjectMan->GetObject("A ServerInfoObject"))
	{
		Object* pkObj = pkObjectMan->CreateObjectByArchType("ServerInfoObject");
		if(!pkObj)
			cout<<"Faild to create serverinfoobject"<<endl;
		 else
			pkObjectMan->GetWorldObject()->AddChild(pkObj);
	}*/


	// Female warrior
	Object* pk0 = pkObjectMan->CreateObjectByArchType("FWarrior");
	Object* pk1 = pkObjectMan->CreateObjectByArchType("Armour");
	Object* pk2 = pkObjectMan->CreateObjectByArchType("Sword");
	Object* pk3 = pkObjectMan->CreateObjectByArchType("Armband");
	Object* pk4 = pkObjectMan->CreateObjectByArchType("Helmet");
	Object* pk5 = pkObjectMan->CreateObjectByArchType("Stovlar");
	Object* pk6 = pkObjectMan->CreateObjectByArchType("Shield");

	// monster
	Object* pk7 = pkObjectMan->CreateObjectByArchType("Monster");

	// goblin
	Object *pkGob = pkObjectMan->CreateObjectByArchType("Goblin");
	
	// minoutar
	Object* pk8 = pkObjectMan->CreateObjectByArchType("VimTest1");		// Minoutar VimTest1
	Object* pk11 = pkObjectMan->CreateObjectByArchType("Min_axe");

	// PSystem
	Object* pk12 = pkObjectMan->CreateObjectByArchType("Smoke");

	// Inn
	Object* pk9	= pkObjectMan->CreateObjectByArchType("Inn_roof");
	Object* pk10 = pkObjectMan->CreateObjectByArchType("Inn_walls");

	// PSystem
	Object* pk13 = pkObjectMan->CreateObjectByArchType("FireSword");

	Object* pk14 = pkObjectMan->CreateObjectByArchType("Rain");

	pk7->SetWorldPosV ( Vector3 (20,0,20) );

	pk8->SetWorldPosV ( Vector3 (-15, 0, 15) );
	pk11->SetWorldPosV ( Vector3 (-15, 0, 15) );
	pk12->SetWorldPosV ( Vector3 (48.5, 17, -40) );
	pk13->SetWorldPosV ( Vector3 (-16,7,-20.3) );

	pkGob->SetWorldPosV ( Vector3 (15, 0, -15) );

	pk0->SetWorldPosV ( Vector3 (-20,0,-20) );
	pk1->SetWorldPosV ( Vector3 (-20,0,-20) );
	pk2->SetWorldPosV ( Vector3 (-20,0,-20) );
	pk3->SetWorldPosV ( Vector3 (-20,0,-20) );
	pk4->SetWorldPosV ( Vector3 (-20,0,-20) );
	pk5->SetWorldPosV ( Vector3 (-20,0,-20) );
	pk6->SetWorldPosV ( Vector3 (-20,0,-20) );


	pk9->SetWorldPosV ( Vector3 (30,0,-30) );
	pk10->SetWorldPosV ( Vector3 (30,0,-30) );

	pk0->AttachToClosestZone();
	pk1->AttachToClosestZone();
	pk2->AttachToClosestZone();
	pk3->AttachToClosestZone();
	pk4->AttachToClosestZone();
	pk5->AttachToClosestZone();
	pk6->AttachToClosestZone();
	pk7->AttachToClosestZone();
   pk8->AttachToClosestZone();
   pk9->AttachToClosestZone();
   pk10->AttachToClosestZone();
	pk11->AttachToClosestZone();
//	pk12->AttachToClosestZone();
//	pk13->AttachToClosestZone();
//	pk14->AttachToClosestZone();
//	pkGob->AttachToClosestZone();


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
		pkScript->Call(m_pkScriptResHandle, "OnClickBackpack", 0, 0); 
	if(iID == 6)
		pkScript->Call(m_pkScriptResHandle , "OnClickStats", 0, 0);
		pkScript->Call(m_pkScriptResHandle, "OnClickStats", 0, 0);
	if(iID == 4)
		pkScript->Call(m_pkScriptResHandle, "OnClickMap", 0, 0);
}
