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
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.25,250);	
	
	//register actions bös
	RegisterActions();

	//register property bös
	RegisterPropertys();

	m_pkMap2 = new Heightmap2(/*"HeightMap"*/);
	m_pkMap2->CreateHMFromImage("/data/textures/hmap.tga");

	// set caption
	SDL_WM_SetCaption("Mistland", NULL);
	
	// create gui script
	GuiAppLua::Init(&g_kMistClient, GetScript());
	
	// init gui
	InitializeGui(pkGui, pkTexMan, pkScript, pkGuiMan);
	
	// hide cursor
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

	Vector3 pos = Get3DMousePos();
	pos+=pkFps->GetCam()->GetPos();
	
	pkRender->Line(pos,pos+Vector3(10,0,0));
	pkRender->Line(pos,pos+Vector3(0,10,0));	
	pkRender->Line(pos,pos+Vector3(0,0,10));	
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
		
		//gubbe rotation

		Vector3 rot;
		rot.Set(0,0,0);
		
		if(pkInput->Pressed(KEY_U))
			rot.x+=fSpeedScale*5;
		if(pkInput->Pressed(KEY_J))
			rot.x-=fSpeedScale*5;		
		if(pkInput->Pressed(KEY_I))
			rot.y+=fSpeedScale*5;
		if(pkInput->Pressed(KEY_K))
			rot.y-=fSpeedScale*5;
		if(pkInput->Pressed(KEY_O))
			rot.z+=fSpeedScale*5;
		if(pkInput->Pressed(KEY_L))
			rot.z-=fSpeedScale*5;



		m_pkME->SetLocalPosV(newpos);				
		m_pkME->RotateLocalRotV(rot);				
		
	}

	if(pkInput->Pressed(MOUSELEFT))
	{
		Vector3 start = pkFps->GetCam()->GetPos();
		Vector3 dir = Get3DMousePos();
	
		vector<Object*> kObjects;
		kObjects.clear();
		
		pkObjectMan->TestLine(&kObjects,start,dir);
		
		cout<<"Objects targeted:"<<kObjects.size()<<endl;
	}

	int iPressedKey = pkInput->GetQueuedKey();

	switch(iPressedKey)
	{
	case KEY_P:
/*		{
			static ZFSound kTestSound;
			kTestSound.m_kPos = Vector3(0,0,0);
			kTestSound.m_kVel = Vector3(0,0,1);
			kTestSound.m_bLoop = false;
			kTestSound.Create("../data/sound/walk.wav");
			pkAudioSys->AddSound(&kTestSound);
		}*/

		static ZFSound* s = new ZFSound;
		s->m_kPos = Vector3(0,0,0);
		//s.m_kVel = Vector3(0,0,1);
		s->Create("../data/sound/walk.wav");
		s->m_bLoop = false;
		pkAudioSys->AddSound(s);

		break;
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
	cout << "Now adding tracker to client" << endl;
	pkClient->m_pkObject->AddProperty("TrackProperty");	
	pkObjectMan->AddTracker(pkClient->m_pkObject);
	
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
	
		m_pkTestobj->SetWorldPosV(Vector3(0,0.1,0));
	}
	pkObjectMan->Test_CreateZones();



	if(pkObjectMan->GetNumOfZones() != 0) {
		pkConsole->Printf("Num of Zones: %d",pkObjectMan->GetNumOfZones());


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

