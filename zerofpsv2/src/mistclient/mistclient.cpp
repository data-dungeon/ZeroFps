#include "mistclient.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../zerofpsv2/engine_systems/common/heightmap.h"
#include "../zerofpsv2/engine_systems/propertys/madproperty.h"
#include "../zerofpsv2/engine_systems/propertys/primitives3d.h"
#include "../zerofpsv2/engine_systems/propertys/proxyproperty.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../mcommon/si_mistland.h"
 
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


/*	for(int x=0;x<5;x++)
	{
		for(int y=0;y<5;y++)
		{*/
			Object* test = pkObjectMan->CreateObjectFromScript("data/script/objects/t_test.lua");
			test->SetLocalPosV(Vector3(10,0,0));
//			test->GetVel() = Vector3(-10,0,0);
			test = pkObjectMan->CreateObjectFromScript("data/script/objects/t_test.lua");
			test->SetLocalPosV(Vector3(-10,0,0));			
//			if(test)
//				test->SetLocalPosV(Vector3(x*4,0,y*4));
//		}
//	}

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

	m_pkMap2 = new Heightmap2(/*"HeightMap"*/);
	m_pkMap2->CreateHMFromImage("/data/textures/hmap.tga");

	// set caption
	SDL_WM_SetCaption("Mistland", NULL);
	 
	// create gui script
	GuiAppLua::Init(&g_kMistClient, GetScript());

	// init gui
	InitializeGui(pkGui, pkTexMan, pkScript, pkGuiMan);

	//init mistland script intreface
	MistLandLua::Init(pkObjectMan,pkScript);
	
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
	pkPropertyFactory->Register("P_Ml", Create_P_Ml);
	pkPropertyFactory->Register("P_Event", Create_P_Event);
}


void MistClient::OnIdle() 
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	
			
	Input();
	
 	pkFps->UpdateCamera(); 	
	
	if(m_pkMap2) {
		m_pkMap2->SetPos(Vector3(0,0,0));
		//pkRender->DrawHM2(m_pkMap2,pkFps->GetCam()->GetPos());	
		}

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
		
		Vector3 newpos;// = m_pkME->GetLocalPosV();
		newpos.Set(0,0,0);
	
		if(pkInput->Pressed(KEY_D)){
			//newpos.x+=fSpeedScale;			
			newpos.x = 10;
		}
		if(pkInput->Pressed(KEY_A)){
			//newpos.x-=fSpeedScale;			
			newpos.x = -10;		
		}	
		if(pkInput->Pressed(KEY_W))	{
			//newpos.z+=fSpeedScale;			
			newpos.z = 10;
		}					
		if(pkInput->Pressed(KEY_S))	{
			//newpos.z-=fSpeedScale;
			newpos.z = -10;
		}		

		if(pkInput->Pressed(KEY_Q))
			newpos.y = 10;
			//newpos.y+=2*fSpeedScale;			
		if(pkInput->Pressed(KEY_E))
			newpos.y = -10;
			//newpos.y-=2*fSpeedScale;
		
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

		//m_pkME->SetLocalPosV(newpos);
		if(newpos.Length() > 0)
			m_pkME->GetVel() = newpos;
		m_pkME->RotateLocalRotV(rot);
	}

	if(pkInput->Pressed(MOUSELEFT))
	{
		if(pkFps->GetTicks() - m_fClickDelay > 0.2)
		{	
	
			Object* pkObject = GetTargetObject();
			
			if(pkObject)
			{
				P_Event* pe = (P_Event*)pkObject->GetProperty("P_Event");
				if(pe)
				{	
					pe->SendEvent("Use");
				}
			} 
			m_fClickDelay = pkFps->GetTicks();		
		}
	}

	int iPressedKey = pkInput->GetQueuedKey();

	switch(iPressedKey)
	{
	case KEY_P:
		{
			static int COUNTER = 0;

			SoundInfo kSound;
			kSound.m_bLoop = false;
			kSound.m_kDir = Vector3(0,0,1);
			kSound.m_kPos = Vector3(0,0,0);
			
			if(COUNTER==0)
			{
				strcpy(kSound.m_szFile, "data/sound/test.wav");
			//	COUNTER++;
			}
/*			else
			if(COUNTER==1)
			{
				strcpy(kSound.m_szFileName, "data/sound/dummy.wav");
				COUNTER++;
			}
			else
			if(COUNTER==2)
			{
				strcpy(kSound.m_szFileName, "data/sound/walk.wav");
				COUNTER = 0;
			}*/
			
			pkAudioSys->ActivateSound(kSound);
			
			
			printf("COUNTER = %i\n", COUNTER);
			printf("ANTAL LJUD = %i\n", pkAudioSys->GetNumActiveSounds());

			
		}
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
/*			{
				pkConsole->Printf("load [mapname]");
				break;				
			}
			
/*			if(!pkLevelMan->LoadLevel(kCommand->m_kSplitCommand[1].c_str()))	
			{
				pkConsole->Printf("Error loading level");
				break;			
			}*

			
			pkConsole->Printf("Level loaded");

						
			pkConsole->Printf("Everything is loaded ,Starting server");
			GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);	*/
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
	m_pkTestobj = pkObjectMan->CreateObjectFromScript("data/script/objects/t_player.lua");
	if(m_pkTestobj)
	{
		m_pkTestobj->AttachToClosestZone();
	
		m_pkCamProp = (CameraProperty*)m_pkTestobj->GetProperty("CameraProperty");
		m_pkCamProp->SetCamera(m_pkCamera);
		m_pkCamProp->SetType(CAM_TYPE3PERSON);
	
		m_pkTestobj->SetWorldPosV(Vector3(0,0.1,0));
		
		// Comment out by Zeb: Funkar inte att accessa variabeln.
		// får felet: mistclient.obj : error LNK2001: unresolved external symbol "int  MistLandLua::g_iCurrentPCID" 
		//MistLandLua::g_iCurrentPCID = m_pkTestobj->iNetWorkID;
	}
	pkObjectMan->Test_CreateZones();



	if(pkObjectMan->GetNumOfZones() != 0) {
		pkConsole->Printf("Num of Zones: %d",pkObjectMan->GetNumOfZones());


	}


	//add server info property
/*	if(!pkObjectMan->GetObject("A ServerInfoObject"))
	{
		Object* pkObj = pkObjectMan->CreateObjectByArchType("ServerInfoObject");
		if(!pkObj)
			cout<<"Faild to create serverinfoobject"<<endl;
		 else
			pkObjectMan->GetWorldObject()->AddChild(pkObj);
	}	*/
}

void MistClient::OnClientStart(void)
{
}

bool MistClient::StartUp()	{ return true; }
bool MistClient::ShutDown()	{ return true; }
bool MistClient::IsValid()	{ return true; }


void MistClient::OnCommand(int iID, ZGuiWnd *pkMainWnd)
{
	if(iID == 5)
		pkScript->Call(m_pkScriptResHandle, "OnClickBackpack", 0, 0); 
	if(iID == 6)
		pkScript->Call(m_pkScriptResHandle, "OnClickStats", 0, 0);
	if(iID == 4)
		pkScript->Call(m_pkScriptResHandle, "OnClickMap", 0, 0);
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

Object* MistClient::GetTargetObject()
{
	Vector3 start = pkFps->GetCam()->GetPos();
	Vector3 dir = Get3DMousePos();

	vector<Object*> kObjects;
	kObjects.clear();
	
	pkObjectMan->TestLine(&kObjects,start,dir);
	
	//cout<<"nr of targets: "<<kObjects.size()<<endl;
	
	float closest = 9999999999;
	Object* pkClosest = NULL;	
	for(int i=0;i<kObjects.size();i++)
	{
		
		float d = (start - kObjects[i]->GetWorldPosV()).Length();
	
		if(d < closest)
		{
			closest = d;
			pkClosest = kObjects[i];
		}
	}
	
	return pkClosest;
}