#include "zerotank.h"
#include "scriptinterfaces.h"
#include "../zerofpsv2/engine_systems/common/heightmap.h"
#include "../zerofpsv2/engine_systems/propertys/madproperty.h"
#include "../zerofpsv2/engine_systems/propertys/primitives3d.h"
#include "../zerofpsv2/gui/zgui.h"

ZeroTank g_kZeroTank("ZeroTank",0,0,0);
Object* g_pkTower;
Object* g_pkGun;
Vector3 g_kRotTower = Vector3(0,0,0);;

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) {return true;}

ZeroTank::ZeroTank(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), GuiApp(GUIPROC)
{ 

	m_iSelfObjectID				= -1;
	m_HaveFoundHMapObject		= false;
	m_iGameType					= 1;
	
	g_ZFObjSys.Log_Create("zerorts");
 
	m_pkZeroTankHull	= NULL;
	m_pkZeroTankTower	= NULL;
	m_pkZeroTankGun	= NULL;
	m_pkZeroTank_Modify = NULL;
	m_fConePosY = 0.0f;
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

	// Moved oject creation to OnServerStart
}

void ZeroTank::Init()
{	

	//get heightmap pointer bös
	m_pkMap = pkLevelMan->GetHeightMap();	

	//register variables
	RegisterVariable("m_iGameType", &m_iGameType,CSYS_INT);
	
	//register commmands bös
	Register_Cmd("load",FID_LOAD);		
	Register_Cmd("unload",FID_UNLOAD);			
	Register_Cmd("massspawn",FID_MASSSPAWN);			

	//damn "#¤(="%#( lighting fix bös
	glEnable(GL_LIGHTING );
	
	//initiate our camera bös
	m_pkCamera=new Camera(Vector3(0,10,0),Vector3(0,0,0),85,1.333,0.25,250);	
//	m_pkCamera->SetViewPort(0,0.24,1,0.76);
	
	//disable zones modells bös
	pkLevelMan->SetVisibleZones(false);

	m_pkMap2 = new Heightmap2(/*"HeightMap"*/);
	m_pkMap2->CreateHMFromImage("/data/textures/hmap.tga");

	//register actions bös
	RegisterActions();

	//register property bös
	RegisterPropertys();

//	m_pkTestObject = pkObjectMan->CreateObjectByArchType("ZeroRTSTestBox");

	m_pkTestObject = pkObjectMan->CreateObject();
/*	MadProperty* pkMad = new MadProperty();
	m_pkTestObject->AddProperty(pkMad);
	pkMad->SetBase("/data/mad/dog.mad");
	pkObjectMan->Add(m_pkTestObject);*/
	
	m_pkTestObject->SetLocalPosV(Vector3(0,-10,0));
	m_pkTestObject->AttachToClosestZone();
	m_pkTestObject->AddProperty(new P_Primitives3D(PYRAMID));
	
/*
	srand( (int) (pkFps->GetGameTime()*1000) );

	char* szRandom[] = {
		"Mistland, the land of mist" };

	char szTitle[150];
	sprintf(szTitle, "zero rts - %s",szRandom[rand()%(sizeof(szRandom)/sizeof(szRandom[1]))]);
*/
	//SDL_WM_SetCaption("Mistland, the land of mist", NULL);

	pkScript->ExposeFunction("CreateWnd", GuiAppLua::CreateWndLua);
	
	InitializeGui(pkGui, pkTexMan, pkScript);

//	SetupGUI();
	
}

void ZeroTank::RegisterActions()
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

void ZeroTank::RegisterPropertys()
{

}


void ZeroTank::OnIdle() 
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	
			
	Input();
	
 	pkFps->UpdateCamera(); 	
	
	m_pkMap2->SetPos(Vector3(0,0,0));
	pkRender->DrawHM2(m_pkMap2,pkFps->GetCam()->GetPos());


/*
	Vector3 pos = pkFps->GetCam()->GetPos();
	pos.y  = m_pkMap2->Height(pos.x,pos.z) ;
	pkFps->GetCam()->SetPos(pos);
	
	Vector3 norm = m_pkMap2->Tilt(pos.x,pos.z);
	float deg=Vector3(0,1,0).Angle(norm);
/*	
	norm.Angels();
	cout<<"x:"<<norm.x<<endl;
	cout<<"y:"<<norm.y<<endl;	
	cout<<"z:"<<norm.z<<endl;	
	
	norm.x *= degtorad;
	norm.y *= degtorad;
	norm.z *= degtorad;
	
	pkFps->GetCam()->SetRot(Vector3(norm.x,pkFps->GetCam()->GetRot().y,norm.z));
*/	

	
	//update player possition 
	Object* pkObj = pkObjectMan->GetObjectByNetWorkID( m_iSelfObjectID );
	if(pkObj) {
		pkObjectMan->OwnerShip_Take( pkObj );
		pkObj->SetWorldPosV(pkFps->GetCam()->GetPos());
	}

	if(m_pkTestObject)
	{
		m_pkTestObject->SetWorldPosV(Vector3(0,-10+m_fConePosY,0)); 
	}

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
	int mx,my;
	pkInput->MouseXY(mx,my);

	static int s_iCursorTex=-1; 
	static int s_iCursorTex_a=-1;
	int iNewCursorTex=0;
	int iNewCursorTex_a=0;


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
	
	// Switch Modify Object
	if(pkInput->Pressed(KEY_1))	m_pkZeroTank_Modify = m_pkZeroTankHull;
	if(pkInput->Pressed(KEY_2))	m_pkZeroTank_Modify = m_pkZeroTankTower;
	if(pkInput->Pressed(KEY_3))	m_pkZeroTank_Modify = m_pkZeroTankGun;

	static float fRotate = 0;
	static Vector3 kRotate(0,0,0); 
	kRotate.Set(0,0,0);
	
	if(m_pkZeroTank_Modify) {
		// Translate
		newpos = m_pkZeroTank_Modify->GetLocalPosV();

		if(pkInput->Pressed(KEY_H))	newpos.x +=	fSpeedScale;			
		if(pkInput->Pressed(KEY_F))	newpos.x -=	fSpeedScale;			
		if(pkInput->Pressed(KEY_T))	newpos.z +=	fSpeedScale;			
		if(pkInput->Pressed(KEY_G))	newpos.z -=	fSpeedScale;			

		m_pkZeroTank_Modify->SetLocalPosV(newpos);		
	
		// Rotate
		if(pkInput->Pressed(KEY_J))	kRotate.x += fSpeedScale;			
		if(pkInput->Pressed(KEY_U))	kRotate.x -= fSpeedScale;			
		if(pkInput->Pressed(KEY_I))	kRotate.y += fSpeedScale;			
		if(pkInput->Pressed(KEY_K))	kRotate.y -= fSpeedScale;			
		if(pkInput->Pressed(KEY_O))	kRotate.z += fSpeedScale;			
		if(pkInput->Pressed(KEY_L))	kRotate.z -= fSpeedScale;			

		m_pkZeroTank_Modify->RotateLocalRotV(kRotate);
		}
	
	

	
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

	pkFps->m_bGuiMode = false;
	pkFps->ToggleGui();

}

void ZeroTank::RunCommand(int cmdid, const CmdArgument* kCommand)
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

			SetupSpawnPoints();
			
						
			pkConsole->Printf("Everything is loaded ,Starting server");
			GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);	
			break;		
		
		case FID_UNLOAD:
			break;
	
	}
}




void ZeroTank::HandleOrders()
{

}

void ZeroTank::ClientInit()
{
	cout<<"Client Join granted"<<endl;

	int mapwidth = pkLevelMan->GetHeightMap()->GetSize();

	cout<<"Mapsize is :"<<mapwidth<<endl;
	
	m_pkMap = pkLevelMan->GetHeightMap();	

	
	cout<<"Join Complete"<<endl;
}

void ZeroTank::OnServerClientJoin(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Joined"<<endl;
	
	pkClient->m_pkObject->AddProperty("P_Primitives3D");	
	
	//setup client input
	pkClient->m_pkObject->AddProperty("P_ClientInput");


}

void ZeroTank::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Parted"<<endl;	
	
}

void ZeroTank::SetupSpawnPoints()
{
	cout<<"Searching for spawn points"<<endl;	
	
	vector<Object*>	kObjects;	
	kObjects.clear();
	m_kSpawnPoints.clear();
	
	pkObjectMan->GetAllObjects(&kObjects);
	
	for(unsigned int i=0;i<kObjects.size();i++)
	{
		if(kObjects[i]->GetName() == "A ZeroRTSSpawnPoint")
		{
			m_kSpawnPoints.push_back(kObjects[i]->GetWorldPosV());
			pkObjectMan->Delete(kObjects[i]);
		}
	}

	cout<<"found "<<m_kSpawnPoints.size()<< " spawn points"<<endl;	
}

void ZeroTank::OnServerStart(void)
{		
/*	m_pkME = pkObjectMan->CreateObject();
	m_pkME->AttachToClosestZone();
	m_pkME->AddProperty("CameraProperty");
	
	CameraProperty* cam = (CameraProperty*)m_pkME->GetProperty("CameraProperty");
	cam->SetCamera(m_pkCamera);
	*/

/*	m_pkME = pkObjectMan->CreateObjectByArchType("body");
	if(m_pkME) {
		m_pkME->SetWorldPosV(Vector3(0,0,0));
		m_pkME->AttachToClosestZone();
	
		
		m_pkME->AddProperty("CameraProperty");
	
		CameraProperty* cam = (CameraProperty*)m_pkME->GetProperty("CameraProperty");
		cam->SetCamera(m_pkCamera);
		
	}

	Object* pk1 = pkObjectMan->CreateObjectByArchType("ZeroRTSTestBox");
	if(pk1) {
		pk1->SetParent(m_pkME);
		pk1->SetLocalPosV(Vector3(0,0.81,0));
		//pk1->AttachToClosestZone();		
	}

	Object* pk2 = pkObjectMan->CreateObjectByArchType("ZeroRTSGun");
	if(pk2) {
		pk2->SetParent(pk1);
		pk2->SetLocalPosV(Vector3(0,0.0,0));
	}*/


	// ZeroTank
 	m_pkZeroTankHull	= NULL;
	m_pkZeroTankTower	= NULL;
	m_pkZeroTankGun	= NULL;

	m_pkZeroTankHull = pkObjectMan->CreateObjectByArchType("ZeroRTSHull");
	if(m_pkZeroTankHull) {
		m_pkZeroTankHull->SetWorldPosV(Vector3(0,10,0));
		m_pkZeroTankHull->AttachToClosestZone();

		//m_pkZeroTankHull->AddProperty("CameraProperty");
		//CameraProperty* cam = (CameraProperty*)m_pkZeroTankHull->GetProperty("CameraProperty");
		//cam->SetCamera(m_pkCamera);
	}

	m_pkZeroTankTower = pkObjectMan->CreateObjectByArchType("ZeroRTSTower");
	if(m_pkZeroTankTower) {
		m_pkZeroTankTower->SetParent(m_pkZeroTankHull);
		m_pkZeroTankTower->SetLocalPosV(Vector3(0,0.81,0));
	}

	m_pkZeroTankGun = pkObjectMan->CreateObjectByArchType("ZeroRTSGun");
	if(m_pkZeroTankGun) {
		m_pkZeroTankGun->SetParent(m_pkZeroTankTower);
		m_pkZeroTankGun->SetLocalPosV(Vector3(-1.1,0.4,0));
	}




	//add server info property
	if(!pkObjectMan->GetObject("A ServerInfoObject"))
	{
		Object* pkObj = pkObjectMan->CreateObjectByArchType("ServerInfoObject");
		if(!pkObj)
			cout<<"Faild to create serverinfoobject"<<endl;
		 else
			pkObjectMan->GetWorldObject()->AddChild(pkObj);
	}	
}

void ZeroTank::OnClientStart(void)
{
}

bool ZeroTank::StartUp()	{ return true; }
bool ZeroTank::ShutDown()	{ return true; }
bool ZeroTank::IsValid()	{ return true; }



void ZeroTank::SetupGUI()
{
/*	CreateWnd(Wnd, "TestWnd", NULL,  10, -1, 0, 0, 300, 300, 0);
	CreateWnd(Textbox, "TestTextBox", "Apa", 11, 10, 100, 100, 50, 20, 0);
	CreateWnd(Button, "TestButton", "Apa", 12, 10, 100, 140, 50, 20, 0);*/
}
