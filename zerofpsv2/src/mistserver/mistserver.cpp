#include "mistserver.h"
#include "../zerofpsv2/engine_systems/common/heightmap.h"
#include "../zerofpsv2/engine_systems/propertys/madproperty.h"
#include "../zerofpsv2/engine_systems/propertys/primitives3d.h"
#include "../zerofpsv2/engine_systems/propertys/proxyproperty.h"
#include "../zerofpsv2/gui/zgui.h"

MistServer g_kMistServer("MistServer",0,0,0);

MistServer::MistServer(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth)
{ 

	m_iSelfObjectID			= -1;
	
	g_ZFObjSys.Log_Create("mistserver");
 
} 

void MistServer::OnInit() 
{
	pkConsole->Printf(" MistServer (mistland dedicated server)");
	pkConsole->Printf("--------------------------------");
	pkConsole->Printf(" Hugga?");

	Init();

	//run autoexec script
	if(!pkIni->ExecuteCommands("mistclient_autoexec.ini"))
		pkConsole->Printf("No game_autoexec.ini.ini found");

}

void MistServer::Init()
{	

	//register commmands bös
	Register_Cmd("load",FID_LOAD);		
	Register_Cmd("unload",FID_UNLOAD);			
	Register_Cmd("massspawn",FID_MASSSPAWN);			

	//damn "#¤(="%#( lighting fix bös
	glEnable(GL_LIGHTING );
	
	//initiate our camera bös
	m_pkCamera=new Camera(Vector3(0,10,0),Vector3(0,0,0),85,1.333,0.25,250);	
//	m_pkCamera->SetViewPort(0,0.24,1,0.76);
	
	//register actions bös
	RegisterActions();

	//register property bös
	RegisterPropertys();

	SDL_WM_SetCaption("Mistland, the land of mist", NULL);
	
	InitializeScript();
	//InitializeGui(pkGui, pkTexMan, pkScript, pkGuiMan);


}

void MistServer::RegisterActions()
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

void MistServer::RegisterPropertys()
{

}


void MistServer::OnIdle() 
{
		
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	
			
	Input();
	
 	pkFps->UpdateCamera(); 	
	
	

}

void MistServer::OnSystem() 
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

void MistServer::Input()
{
	const int PRESSED_KEY = pkInput->GetQueuedKey();

	int mx,my;
	pkInput->MouseXY(mx,my);
	
	int x,z;		
	pkInput->RelMouseXY(x,z);
	
	float speed = 20;

	if(m_pkCameraObject)
	{

		//camera movements
		if(pkInput->Pressed(KEY_X)){
			speed*=0.25;
		}
	
		Vector3 newpos = m_pkCameraObject->GetLocalPosV();
		Vector3 rot;
		rot.Set(0,0,0);
		
		Matrix4 kRm = m_pkCameraObject->GetLocalRotM();
		kRm.Transponse();
	
		if(pkInput->Pressed(KEY_D)){
			newpos-=kRm.VectorRotate(Vector3(-1,0,0))* pkFps->GetFrameTime()*speed;		
		}
		if(pkInput->Pressed(KEY_A)){
			newpos-=kRm.VectorRotate(Vector3(1,0,0))* pkFps->GetFrameTime()*speed;		
		}	
		if(pkInput->Pressed(KEY_W))	{
			newpos-=kRm.VectorRotate(Vector3(0,0,1))* pkFps->GetFrameTime()*speed;
		}					
		if(pkInput->Pressed(KEY_S))	{
			newpos-=kRm.VectorRotate(Vector3(0,0,-1))* pkFps->GetFrameTime()*speed;	
		}		
	
		if(pkInput->Pressed(KEY_Q))
			rot.z+= 5 * pkFps->GetFrameTime()*speed;
		if(pkInput->Pressed(KEY_E))
			rot.z-= 5 * pkFps->GetFrameTime()*speed;

		
		rot.x+=z/5.0;
		rot.y-=x/5.0;	

		m_pkCameraObject->SetLocalPosV(newpos);
		m_pkCameraObject->RotateLocalRotV(rot);
	
	}

};

void MistServer::OnHud(void)
{
	pkFps->DevPrintf("common","Active Propertys: %d",pkObjectMan->GetActivePropertys());	
	pkFps->DevPrintf("common", "Fps: %f",pkFps->m_fFps);	
	pkFps->DevPrintf("common","Avrage Fps: %f",pkFps->m_fAvrageFps);			
	pkFps->DevPrintf("common","SelfID: %d", m_iSelfObjectID);	
	
	
	pkFps->m_bGuiMode = false;
	pkFps->ToggleGui();

}

void MistServer::RunCommand(int cmdid, const CmdArgument* kCommand)
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



void MistServer::ClientInit()
{
	cout<<"Client Join granted"<<endl;


	cout<<"Mapsize is :"<<endl;
	
	cout<<"Join Complete"<<endl;
}

void MistServer::OnServerClientJoin(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Joined"<<endl;
	
	pkClient->m_pkObject->AddProperty("P_Primitives3D");	
	cout << "Now adding tracker to client" << endl;
	pkClient->m_pkObject->AddProperty("TrackProperty");	
	pkObjectMan->AddTracker(pkClient->m_pkObject);
	
	//setup client input
	pkClient->m_pkObject->AddProperty("P_ClientInput");


}

void MistServer::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Parted"<<endl;	
	
}


void MistServer::OnServerStart(void)
{		
	m_pkCameraObject = pkObjectMan->CreateObjectByArchType("camera");
	if(m_pkCameraObject)
	{
		m_pkCameraObject->AttachToClosestZone();
		CameraProperty* cam = (CameraProperty*)m_pkCameraObject->GetProperty("CameraProperty");
		cam->SetCamera(m_pkCamera);
	}
	
	pkObjectMan->Test_CreateZones();



	if(pkObjectMan->GetNumOfZones() != 0) {
		pkConsole->Printf("Num of Zones: %d",pkObjectMan->GetNumOfZones());

	}
}

void MistServer::OnClientStart(void)
{
}

bool MistServer::StartUp()	{ return true; }
bool MistServer::ShutDown()	{ return true; }
bool MistServer::IsValid()	{ return true; }


void MistServer::OnCommand(int iID, ZGuiWnd *pkMainWnd)
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


bool MistServer::InitializeScript()
{
/*	// Script functions for using the gui
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
*/
	return true;
}
