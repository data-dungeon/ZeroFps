#include "mistclient.h"
#include "../zerofpsv2/engine_systems/script_interfaces/gui_scriptinterface.h"
#include "../zerofpsv2/engine_systems/common/heightmap.h"
#include "../zerofpsv2/engine_systems/propertys/madproperty.h"
#include "../zerofpsv2/engine_systems/propertys/primitives3d.h"
#include "../zerofpsv2/engine_systems/propertys/proxyproperty.h"
#include "../zerofpsv2/gui/zgui.h"

MistClient g_kMistClient("MistClient",0,0,0);

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
	switch(msg)
	{
	case ZGM_COMMAND:
		g_kMistClient.OnCommand(((int*)params)[0], win);
		break;
	}
	return true;
}

MistClient::MistClient(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 

	m_iSelfObjectID			= -1;
	
	g_ZFObjSys.Log_Create("mistclient");
 
} 

void MistClient::OnInit() 
{
	pkConsole->Printf(" MistClient");
	pkConsole->Printf("--------------------------------");
	pkConsole->Printf(" ugga?");

	Init();

	//run autoexec script
	if(!pkIni->ExecuteCommands("mistclient_autoexec.ini"))
		pkConsole->Printf("No game_autoexec.ini.ini found");

}

void MistClient::Init()
{	

	//register commmands bös
	Register_Cmd("load",FID_LOAD);		
	Register_Cmd("unload",FID_UNLOAD);			
	Register_Cmd("massspawn",FID_MASSSPAWN);			

	//damn "#¤(="%#( lighting fix bös
	glEnable(GL_LIGHTING );
	
	//initiate our camera bös
	m_pkCamera=new Camera(Vector3(0,10,0),Vector3(0,0,0),85,1.333,0.25,250);	
	
	//register actions bös
	RegisterActions();

	//register property bös
	RegisterPropertys();

	m_pkMap2 = new Heightmap2(/*"HeightMap"*/);
	m_pkMap2->CreateHMFromImage("/data/textures/hmap.tga");
/*
	srand( (int) (pkFps->GetGameTime()*1000) );

	char* szRandom[] = {
		"Mistland, the land of mist" };

	char szTitle[150];
	sprintf(szTitle, "zero rts - %s",szRandom[rand()%(sizeof(szRandom)/sizeof(szRandom[1]))]);
*/
	//SDL_WM_SetCaption("Mistland, the land of mist", NULL);
	
/*	GuiAppLua::g_pkGuiApp = &g_kMistClient;
	GuiAppLua::g_pkScript = GetScript();*/

	GuiAppLua::Init(&g_kMistClient, GetScript());

	InitializeScript();
	InitializeGui(pkGui, pkTexMan, pkScript, pkGuiMan);
	SDL_ShowCursor(SDL_DISABLE);

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

}


void MistClient::OnIdle() 
{
		
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	
			
	Input();
	
 	pkFps->UpdateCamera(); 	
	
	
	m_pkMap2->SetPos(Vector3(0,0,0));

	pkRender->DrawHM2(m_pkMap2,pkFps->GetCam()->GetPos());	

}

void MistClient::OnSystem() 
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
			m_fDistance += z/10.0;
	
			if(m_fDistance < 10)
				m_fDistance = 10;
		
			if(m_fDistance > 35)
				m_fDistance = 35;
		
		
			m_pkCamProp->Set3PYAngle(m_fAngle);
			m_pkCamProp->Set3PDistance(m_fDistance);
		}
	}
	

	Object* m_pkME = m_pkTestobj;
	
	if(m_pkME)	
	{	
		float fSpeedScale = pkFps->GetFrameTime()*speed;	
		
		Vector3 newpos = m_pkME->GetLocalPosV();
	
		if(pkInput->Pressed(KEY_D)){
			newpos.x+=fSpeedScale;			
		}
		if(pkInput->Pressed(KEY_A)){
			newpos.x-=fSpeedScale;			
		}	
		if(pkInput->Pressed(KEY_W))	{
			newpos.z+=fSpeedScale;			
		}					
		if(pkInput->Pressed(KEY_S))	{
			newpos.z-=fSpeedScale;
		}		

		if(pkInput->Pressed(KEY_Q))
			newpos.y+=2*fSpeedScale;			
		if(pkInput->Pressed(KEY_E))
			newpos.y-=2*fSpeedScale;
		

		m_pkME->SetLocalPosV(newpos);				
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
	int i;

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

						
			pkConsole->Printf("Everything is loaded ,Starting server");
			GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);	
			break;		
		
		case FID_UNLOAD:
			break;
	
		case FID_MASSSPAWN:
			for(i=0; i < 1;i++) {
			}


			
/*		for(int i=0; i<5; i++) {
			m_pkZeroTankTrack = pkObjectMan->CreateObjectByArchType("TrackObject");
			if(m_pkZeroTankTrack) {
				int iRandZone =  rand() % pkObjectMan->GetNumOfZones();
				m_pkZeroTankTrack->SetWorldPosV( pkObjectMan->GetZoneCenter(iRandZone) );
				m_pkZeroTankTrack->AttachToClosestZone();
				pkObjectMan->AddTracker(m_pkZeroTankTrack);

			}
		}*/
			break;


	}
}



void MistClient::ClientInit()
{
	cout<<"Client Join granted"<<endl;


	cout<<"Mapsize is :"<<endl;
	
	cout<<"Join Complete"<<endl;
}

void MistClient::OnServerClientJoin(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Joined"<<endl;
	
	pkClient->m_pkObject->AddProperty("P_Primitives3D");	
	cout << "Now adding tracker to client" << endl;
	pkClient->m_pkObject->AddProperty("TrackProperty");	
	pkObjectMan->AddTracker(pkClient->m_pkObject);
	
	//setup client input
	pkClient->m_pkObject->AddProperty("P_ClientInput");


}

void MistClient::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Parted"<<endl;	
	
}


void MistClient::OnServerStart(void)
{		
	m_pkTestobj = pkObjectMan->CreateObjectByArchType("playertest");
	if(m_pkTestobj)
	{
		m_pkTestobj->AttachToClosestZone();
	
		m_pkCamProp = (CameraProperty*)m_pkTestobj->GetProperty("CameraProperty");
		m_pkCamProp->SetCamera(m_pkCamera);
		m_pkCamProp->SetType(CAM_TYPE3PERSON);
	
		m_pkTestobj->SetWorldPosV(Vector3(0,20,0));
	}


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


	pkObjectMan->Test_CreateZones();



	if(pkObjectMan->GetNumOfZones() != 0) {
		pkConsole->Printf("Num of Zones: %d",pkObjectMan->GetNumOfZones());

/*		for(int i=0; i<5; i++) {
			m_pkZeroTankTrack = pkObjectMan->CreateObjectByArchType("TrackObject");
			if(m_pkZeroTankTrack) {
				int iRandZone =  rand() % pkObjectMan->GetNumOfZones();
				m_pkZeroTankTrack->SetWorldPosV( pkObjectMan->GetZoneCenter(iRandZone) );
				m_pkZeroTankTrack->AttachToClosestZone();
				pkObjectMan->AddTracker(m_pkZeroTankTrack);

			}
		}*/

	
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

void MistClient::OnClientStart(void)
{
}

bool MistClient::StartUp()	{ return true; }
bool MistClient::ShutDown()	{ return true; }
bool MistClient::IsValid()	{ return true; }


void MistClient::OnCommand(int iID, ZGuiWnd *pkMainWnd)
{
/*	static const int apA = GetApa();
	if(pkScript->GetGlobalInt(NULL,  
	pkScript->CallScript("OnClickOK", 0, 0);*/

	if(iID == 5)
		pkScript->CallScript("OnClickBackpack", 0, 0); 
	if(iID == 6)
		pkScript->CallScript("OnClickStats", 0, 0);
	if(iID == 4)
		pkScript->CallScript("OnClickMap", 0, 0);

}


bool MistClient::InitializeScript()
{
	// Script functions for using the gui
	pkScript->ExposeFunction("CreateWnd", GuiAppLua::CreateWndLua);
	pkScript->ExposeFunction("AddTabPage", GuiAppLua::AddTabPageLua);
	pkScript->ExposeFunction("AddListItem", GuiAppLua::AddListboxItemLua);
	pkScript->ExposeFunction("ClearListbox", GuiAppLua::ClearListboxLua);
	pkScript->ExposeFunction("GetWnd", GuiAppLua::GetWndLua);
	pkScript->ExposeFunction("CloseWnd", GuiAppLua::CloseWndLua); 
	pkScript->ExposeFunction("ChangeSkin", GuiAppLua::ChangeSkinLua); 
	pkScript->ExposeFunction("GetScreenWidth", GuiAppLua::GetScreenWidthLua); 
	pkScript->ExposeFunction("GetScreenHeight", GuiAppLua::GetScreenHeightLua); 
	pkScript->ExposeFunction("IsWndVisible", GuiAppLua::IsWndVisibleLua); 
	pkScript->ExposeFunction("SetTextInt", GuiAppLua::SetTextInt); 

	return true;
}
