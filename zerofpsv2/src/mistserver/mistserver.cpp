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


	SDL_WM_SetCaption("Mistland, the land of mist", NULL);
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
		Vector3 rot;
		rot.Set(0,0,0);
		
		Matrix4 kRm = m_pkCameraObject->GetLocalRotM();

		kRm.Transponse();
	
		if(pkInput->Pressed(KEY_D))	newpos += kRm.GetAxis(0) * fSpeedScale;		
		if(pkInput->Pressed(KEY_A))	newpos += kRm.GetAxis(0) * -fSpeedScale;		
		if(pkInput->Pressed(KEY_W))	newpos += kRm.GetAxis(2) * -fSpeedScale;
		if(pkInput->Pressed(KEY_S))	newpos += kRm.GetAxis(2) * fSpeedScale;	
	
		if(pkInput->Pressed(KEY_Q))	rot.z += 5 * fSpeedScale;
		if(pkInput->Pressed(KEY_E))	rot.z -= 5 * fSpeedScale;
		
		rot.x += z / 5.0;
		rot.y -= x / 5.0;	

		m_pkCameraObject->SetLocalPosV(newpos);
		m_pkCameraObject->RotateLocalRotV(rot);	
	
	
	
		if(pkInput->Pressed(KEY_SPACE))
		{
			AddZone(m_pkCameraObject->GetWorldPosV());
	
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
	

}

void MistServer::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Parted"<<endl;	
}


void MistServer::OnServerStart(void)
{		
	//create a camera for the server
	m_pkCameraObject = pkObjectMan->CreateObjectFromScript("data/script/objects/t_camera.lua");
	
	if(m_pkCameraObject)
	{	
		CameraProperty* m_pkCamProp = (CameraProperty*)m_pkCameraObject->GetProperty("CameraProperty");
		m_pkCamProp->SetCamera(m_pkCamera);
		//m_pkCamProp->SetType(CAM_TYPE3PERSON);
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
>>>>>>> 1.8
	if(iID == 6)
		pkScript->Call(m_pkScriptResHandle, "OnClickStats", 0, 0);
	if(iID == 4)
		pkScript->Call(GetGuiScript(), "OnClickMap", 0, 0);
		pkScript->Call(m_pkScriptResHandle, "OnClickMap", 0, 0);
*/
}

Vector3 MistServer::Get3DMousePos()
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


void MistServer::AddZone(Vector3 kPos )
{
	Vector3 kSnap;
	
	kSnap.x = round(kPos.x/4.0) * 4.0;
	kSnap.y = round(kPos.y/4.0) * 4.0;
	kSnap.z = round(kPos.z/4.0) * 4.0;



	if(pkObjectMan->IsInsideZone(kSnap,m_kZoneSize))
		return;
		
	
	int id = pkObjectMan->CreateZone(kSnap,m_kZoneSize);
	pkObjectMan->UpdateZoneLinks(id);
	

}






