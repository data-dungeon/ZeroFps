/**
	\defgroup MistServer MistServer
	\ingroup MistLand

  MistServer is the Server of the game MistLands.
*/

#include "mistserver.h"
#include "../zerofpsv2/engine_systems/common/heightmap.h"
#include "../zerofpsv2/engine_systems/propertys/madproperty.h"
#include "../zerofpsv2/engine_systems/propertys/primitives3d.h"
#include "../zerofpsv2/engine_systems/propertys/proxyproperty.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"

MistServer g_kMistServer("MistServer",0,0,0);

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
	switch(msg)
	{
	case ZGM_COMMAND:
		g_kMistServer.OnCommand(((int*)params)[0], win);
		break;
	}
	return true;
}
MistServer::MistServer(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	g_ZFObjSys.Log_Create("mistserver");
} 

void MistServer::OnInit() 
{
	pkConsole->Printf(" MistServer (mistland dedicated server)");
	pkConsole->Printf("--------------------------------");
	pkConsole->Printf(" Hugga?");

	Init();

	//run autoexec script
	if(!pkIni->ExecuteCommands("mistserver_autoexec.ini"))
		pkConsole->Printf("No mistserver_autoexec.ini found");
}

void MistServer::Init()
{	
	m_kZoneSize.Set(8,8,8);
	
	//register commmands bös
	Register_Cmd("load",FID_LOAD);		

	//damn "#¤(="%#( lighting fix bös
	glEnable(GL_LIGHTING );
	
	//register property bös
	RegisterPropertys();

	//initiate our camera
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.25,250);	

	//init mistland script intreface
	MistLandLua::Init(pkObjectMan,pkScript);
	
	// create gui script funktions
	GuiAppLua::Init(&g_kMistServer, pkScript);

	// init gui
	InitializeGui(pkGui, pkTexMan, pkScript, pkGuiMan, 
		"data/textures/text/ms_sans_serif8.bmp",
		"data/script/gui/gui_create_server.lua");

	// hide cursor
	SDL_ShowCursor(SDL_DISABLE);

	SDL_WM_SetCaption("Mistland, the land of mist", NULL);
}

void MistServer::RegisterPropertys()
{
	pkPropertyFactory->Register("P_Ml", Create_P_Ml);
	pkPropertyFactory->Register("P_Event", Create_P_Event);
}

void MistServer::OnIdle()
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	

	Input();	
 	pkFps->UpdateCamera(); 		

	UpdateZoneMarkerPos();
	DrawZoneMarker(m_kZoneMarkerPos);
}

void MistServer::OnSystem()
{

}

void MistServer::Input()
{
	float speed = 20;

	int x,z;		
	pkInput->RelMouseXY(x,z);	

	if(m_pkCameraObject)	
	{	
		if(pkInput->Pressed(KEY_X)){
			speed*=0.25;
		}
	
		float fSpeedScale = pkFps->GetFrameTime()*speed;

		Vector3 newpos = m_pkCameraObject->GetLocalPosV();
		
		Matrix4 kRm = m_pkCameraObject->GetLocalRotM();

		kRm.Transponse();
	
		
		Vector3 xv = kRm.GetAxis(0);
		Vector3 zv = kRm.GetAxis(2);
	
		xv.y = 0;
		zv.y = 0;
		
		xv.Normalize();
		zv.Normalize();
	
		if(pkInput->Pressed(KEY_D))	newpos += xv * fSpeedScale;		
		if(pkInput->Pressed(KEY_A))	newpos += xv * -fSpeedScale;		
		if(pkInput->Pressed(KEY_W))	newpos += zv * -fSpeedScale;
		if(pkInput->Pressed(KEY_S))	newpos += zv * fSpeedScale;	
	
		if(pkInput->Pressed(KEY_Q))	newpos.y += fSpeedScale;
		if(pkInput->Pressed(KEY_E))	newpos.y -= fSpeedScale;
				

		Vector3 rot;
		rot.Set(-z / 5.0,-x / 5.0,0);

		kRm.Transponse();		
		kRm.Rotate(rot);
		kRm.Transponse();		
		Vector3 bla = Vector3(0,0,1);
		bla = kRm.VectorTransform(bla);
		kRm.LookDir(bla,Vector3(0,1,0));

		m_pkCameraObject->SetLocalPosV(newpos);
		m_pkCameraObject->SetLocalRotM(kRm);	
	
	
		if(pkInput->Pressed(MOUSELEFT))
		{
			AddZone();
	
		}
		if(pkInput->Pressed(MOUSERIGHT))
		{
			int id = pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
			pkObjectMan->DeleteZone(id);
	
		}
	
		if(pkInput->Pressed(KEY_SPACE))
		{
			pkObjectMan->CreateObjectFromScriptInZone("data/script/objects/t_test.lua",pkFps->GetCam()->GetPos() + Get3DMousePos(false)*20);
		}
	
		if(pkInput->Pressed(KEY_1)) m_kZoneSize.Set(4,4,4);
		if(pkInput->Pressed(KEY_2)) m_kZoneSize.Set(8,8,8);
		if(pkInput->Pressed(KEY_3)) m_kZoneSize.Set(16,16,16);	
		if(pkInput->Pressed(KEY_4)) m_kZoneSize.Set(32,16,32);	
		if(pkInput->Pressed(KEY_5)) m_kZoneSize.Set(64,16,64);			
		if(pkInput->Pressed(KEY_6)) m_kZoneSize.Set(16,8,8);		
		if(pkInput->Pressed(KEY_7)) m_kZoneSize.Set(8,8,16);		
		if(pkInput->Pressed(KEY_8)) m_kZoneSize.Set(4,8,16);				
		if(pkInput->Pressed(KEY_9)) m_kZoneSize.Set(16,8,4);						
		
		if(pkInput->Pressed(KEY_Z))
		{
			int id = pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
			pkObjectMan->SetZoneModel("data/mad/zones/tcross.mad",id);		
		}
		
		if(pkInput->Pressed(KEY_X))
		{
			int id = pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
			pkObjectMan->SetZoneModel("data/mad/zones/largeroom.mad",id);		
		}
		
		if(pkInput->Pressed(KEY_C))
		{
			int id = pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
			pkObjectMan->SetZoneModel("data/mad/zones/large_room_een.mad",id);		
		}
	
	
	}
};

void MistServer::OnHud(void)
{
	pkFps->DevPrintf("common","Active Propertys: %d",pkObjectMan->GetActivePropertys());	
	pkFps->DevPrintf("common", "Fps: %f",pkFps->m_fFps);	
	pkFps->DevPrintf("common","Avrage Fps: %f",pkFps->m_fAvrageFps);			
		
	pkFps->m_bGuiMode = false;
	pkFps->ToggleGui();
}

void MistServer::RunCommand(int cmdid, const CmdArgument* kCommand)
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
		
	
	}

}

void MistServer::ClientInit()
{
	cout<<"Client Join granted"<<endl;
	
	cout<<"Join Complete"<<endl;
}

void MistServer::OnServerClientJoin(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Joined"<<endl;
	
	
	pkClient->m_pkObject->AddProperty("P_Primitives3D");	
	cout << "Now adding tracker to client" << endl;
	pkClient->m_pkObject->AddProperty("TrackProperty");	
	pkClient->m_pkObject->SetUseZones(false);

}

void MistServer::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Parted"<<endl;	
}


void MistServer::OnServerStart(void)
{		
	//create a camera for the server
	m_pkCameraObject = pkObjectMan->CreateObjectFromScript("data/script/objects/t_camera.lua");

	pkObjectMan->CreateObjectFromScript("data/script/objects/t_player.lua");
	
	if(m_pkCameraObject)
	{	
		CameraProperty* m_pkCamProp = (CameraProperty*)m_pkCameraObject->GetProperty("CameraProperty");
		m_pkCamProp->SetCamera(m_pkCamera);
		//m_pkCamProp->SetType(CAM_TYPE3PERSON);
		
		//the builder/server shuld not connect to zones
		m_pkCameraObject->SetUseZones(false);		
	}
	
	
	
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
/*	if(iID == 5)
		pkScript->Call(GetGuiScript(), "OnClickBackpack", 0, 0); 
	if(iID == 5)
		pkScript->Call(m_pkScriptResHandle, "OnClickBackpack", 0, 0); 
	if(iID == 6)
		pkScript->Call(m_pkScriptResHandle, "OnClickStats", 0, 0);
	if(iID == 4)
		pkScript->Call(m_pkScriptResHandle, "OnClickMap", 0, 0);
*/
}

Vector3 MistServer::Get3DMousePos(bool m_bMouse=true)
{
	Vector3 dir;
	float x,y;		
	
	//screen propotions
	float xp=4;
	float yp=3;
	
	if(m_bMouse)
	{
		pkInput->UnitMouseXY(x,y);	
		dir.Set(x*xp,-y*yp,-1.5);
		dir.Normalize();
	}
	else
	{
		dir.Set(0,0,-1.5);
		dir.Normalize();	
	}
	
	Matrix4 rm = m_pkCamera->GetRotM();
	rm.Transponse();
	dir = rm.VectorTransform(dir);
	
	return dir;
}

Object* MistServer::GetTargetObject()
{
	Vector3 start = pkFps->GetCam()->GetPos();
	Vector3 dir = Get3DMousePos();

	vector<Object*> kObjects;
	kObjects.clear();
	
	pkObjectMan->TestLine(&kObjects,start,dir);
	
	
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


void MistServer::AddZone()
{

// wtf? (zerom)
/* Vector3 kSnap;
	
	kSnap.x = int(kPos.x/4.0) * 4.0;
	kSnap.y = int(kPos.y/4.0) * 4.0;
	kSnap.z = int(kPos.y/4.0) * 4.0;

//	printf("%f,%f,%f\n", m_kZoneSize.x, m_kZoneSize.y, m_kZoneSize.z);
*/


	if(pkObjectMan->IsInsideZone(m_kZoneMarkerPos,m_kZoneSize))
		return;
		
	int id = pkObjectMan->CreateZone(m_kZoneMarkerPos,m_kZoneSize);
}


void MistServer::DrawZoneMarker(Vector3 kPos)
{
	Vector3 bla = m_kZoneSize / 2;
	pkRender->DrawAABB(kPos-bla,kPos+bla,Vector3(1,1,1));
}

void MistServer::UpdateZoneMarkerPos()
{
	Vector3 temp = pkFps->GetCam()->GetPos() + Get3DMousePos(false)*20;


	m_kZoneMarkerPos.x = int(temp.x/4.0) * 4.0;
	m_kZoneMarkerPos.y = int(temp.y/4.0) * 4.0;
	m_kZoneMarkerPos.z = int(temp.z/4.0) * 4.0;

	if(m_kZoneSize.x != 4) m_kZoneMarkerPos.x = round(temp.x/4.0) * 4.0;
		else  m_kZoneMarkerPos.x = round(temp.x/4.0) * 4.0 + 2;						

	if(m_kZoneSize.y != 4) m_kZoneMarkerPos.y = round(temp.y/4.0) * 4.0;
		else  m_kZoneMarkerPos.y = round(temp.y/4.0) * 4.0 + 2;						

	if(m_kZoneSize.z != 4) m_kZoneMarkerPos.z = round(temp.z/4.0) * 4.0;
		else  m_kZoneMarkerPos.z = round(temp.z/4.0) * 4.0 + 2;						

//	m_kZoneMarkerPos.y = round(temp.y/4.0) * 4.0;
//	m_kZoneMarkerPos.z = round(temp.z/4.0) * 4.0;



}


