/**
	\defgroup MistServer MistServer
	\ingroup MistLand

  MistServer is the Server of the game MistLands.
*/

#include "mistserver.h"
#include "../zerofpsv2/engine_systems/common/heightmap.h"
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "../zerofpsv2/engine_systems/propertys/p_primitives3d.h"
#include "../zerofpsv2/engine_systems/propertys/p_track.h"
#include "../zerofpsv2/engine/p_pfmesh.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../zerofpsv2/basic/zguifont.h"
#include <set> 
#include <algorithm>


MistServer g_kMistServer("MistServer",0,0,0);

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
	switch(msg)
	{
	case ZGM_COMMAND:
		g_kMistServer.OnCommand(((int*)params)[0], win);
		break;

	case ZGM_SELECTLISTITEM:
		g_kMistServer.OnClickListbox(
			//g_kMistServer.GetWnd(((int*)params)[0]), 
			((int*)params)[0],
			((int*)params)[1],win);
		break;

	case ZGM_SELECTTREEITEM:
		char** pszParams; pszParams = (char**) params;
		g_kMistServer.OnClickTreeItem( pszParams[0], pszParams[1], 
			pszParams[2], pszParams[3][0] == '1' ? true : false);		

		if(pszParams[0])
			delete[] pszParams[0];
		if(pszParams[1])
			delete[] pszParams[1];
		if(pszParams[2])
			delete[] pszParams[2];
		if(pszParams[3])
			delete[] pszParams[3];

		break;

	case ZGM_TCN_SELCHANGE:
		int* data; data = (int*) params; 
		g_kMistServer.OnClickTabPage((ZGuiTabCtrl*) data[2], data[0], data[1]);// fram med släggan
		break;
	}
	return true;
}

MistServer::MistServer(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	g_ZFObjSys.Log_Create("mistserver");

	m_pkServerInfoP = NULL;
	
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
	//default edit mode 
	m_iEditMode = EDIT_ZONES;

	//object defaults
	m_iCurrentObject = -1;
	
	//zone defaults
	m_kZoneSize.Set(8,8,8);
	m_iCurrentMarkedZone = -1;
	m_strActiveZoneName = "data/mad/zones/emptyzone.mad";
	m_strActiveObjectName = "";//data/script/objects/t_test.lua";
	m_strActiveEnviroment = "data/enviroments/sun.env";

	//click delay
	m_fClickDelay = pkFps->GetTicks();
	
	//register commmands bös
	Register_Cmd("new",FID_NEW);		
	Register_Cmd("load",FID_LOAD);		
	Register_Cmd("save",FID_SAVE);		

	//damn "#¤(="%#( lighting fix bös
	pkLight->SetLighting(true);
	pkZShader->SetForceLighting(ALWAYS_OFF);	
	
	//register property bös
	RegisterPropertys();

	//register resources
	RegisterResources();

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

	pkGui->SetCursor(0,0, pkTexMan->Load("data/textures/gui/cursor.bmp", 0),
		pkTexMan->Load("data/textures/gui/cursor_a.bmp", 0), 32, 32);

	// hide cursor
	SDL_ShowCursor(SDL_DISABLE);

	SDL_WM_SetCaption("MistServer", NULL);

	CreateMenu("data/script/gui/menu.txt");
}

void MistServer::RegisterResources()
{
	pkResourceDB->RegisterResource( string(".env"), Create__EnvSetting	);
}

void MistServer::RegisterPropertys()
{
	pkPropertyFactory->Register("P_Enviroment", Create_P_Enviroment);
	pkPropertyFactory->Register("P_ClientControl", Create_P_ClientControl);
	pkPropertyFactory->Register("P_ServerInfo", Create_P_ServerInfo);
	pkPropertyFactory->Register("P_Ml", Create_P_Ml);
	pkPropertyFactory->Register("P_Event", Create_P_Event);
	pkPropertyFactory->Register("P_CharStats", Create_P_CharStats);
   pkPropertyFactory->Register("P_Item", Create_P_Item);
   pkPropertyFactory->Register("P_Spell", Create_P_Spell);
}



void MistServer::OnIdle()
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	

	if(pkGui->m_bHaveInputFocus == false)
	{
		Input();	
	}

 	pkFps->UpdateCamera(); 		

	for(unsigned int iPath = 0; iPath < kPath.size(); iPath++)
		pkRender->Draw_MarkerCross(kPath[iPath],Vector3(1,1,1),1);
   
	if(m_pkServerInfoP)
	{
		pkFps->DevPrintf("server","ServerName: %s", m_pkServerInfoP->m_sServerName.c_str());
		pkFps->DevPrintf("server","Players: %d", m_pkServerInfoP->GetNrOfPlayers());
	
	}
	
	if(m_iEditMode == EDIT_ZONES)
	{
		UpdateZoneMarkerPos();		
		DrawZoneMarker(m_kZoneMarkerPos);		
		
		/*
		//draw selected zone marker
		if(m_iCurrentMarkedZone != -1)
		{
			ZoneData* z = pkObjectMan->GetZoneData(m_iCurrentMarkedZone);
		
			if(z)
			{
				Vector3 kMin = z->m_kPos - z->m_kSize/2;
				Vector3 kMax = z->m_kPos + z->m_kSize/2;
		
				pkRender->DrawAABB( kMin,kMax, Vector3(1,1,0) );
			}
		}
		*/
	}
	
	if(m_iEditMode == EDIT_OBJECTS)
	{	
		UpdateObjectMakerPos();
		
		DrawCrossMarker(m_kObjectMarkerPos);		
	
		if(m_iCurrentObject != -1)
		{
			Entity* pkObj = pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);
			
			if(pkObj)
			{
				Vector3 kMin = pkObj->GetWorldPosV() - pkObj->GetRadius()/2;
				Vector3 kMax = pkObj->GetWorldPosV() + pkObj->GetRadius()/2;
		
				pkRender->DrawAABB( kMin,kMax, Vector3(1,1,0) );
			}
		}
	}

	PathTest();
}

void MistServer::OnSystem()
{
	HandleOrders();
}


void MistServer::Input()
{

	int iPressedKey = pkInput->GetQueuedKey();

	switch(iPressedKey)
	{
	case KEY_F9:
		printf("Num sounds in system = %i\nNum active channels = %i\n",
			pkAudioSys->GetNumSounds(), pkAudioSys->GetNumActiveChannels());
		break;
	}

	float speed = 20;
	
	//set speed depending on edit mode
	if(m_iEditMode == EDIT_ZONES)
		speed = 20;
	
	if(m_iEditMode == EDIT_OBJECTS)
		speed = 5;
	

	if(pkInput->Pressed(KEY_F5) ) {
		if(m_pkServerInfoP)
			m_pkServerInfoP->MessagePlayer(-1,"Tjena julle");
	} 
	
	int x,z;		
	pkInput->RelMouseXY(x,z);	

	P_Mad* mp;
	Entity* pkAnimObj = pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);								
	if(pkAnimObj)
		mp = (P_Mad*)pkAnimObj->GetProperty("P_Mad");
	
	if(pkInput->Pressed(KEY_F5) && mp) {
		mp->SetAnimationActive(false);
		} 
	if(pkInput->Pressed(KEY_F6) && mp) {
		mp->SetAnimationActive(true);
		} 
	if(pkInput->Pressed(KEY_F7) && mp) {
		mp->NextCoreAnimation();
		} 
	



/*	if(pkInput->Pressed(KEY_F5)) {
		kPathStart = m_kObjectMarkerPos;
		cout << "Setting Start" << endl;
		} 
	if(pkInput->Pressed(KEY_F6)) {
		kPathEnd = m_kObjectMarkerPos;
		cout << "Setting end" << endl;
		} 

	if(pkInput->Pressed(KEY_L)) {
		kPath.clear();
		bool bres = m_pkAStar->GetPath(kPathStart,kPathEnd,kPath);

		if(bres) {
			cout << "Path was found. Size " << kPath.size()  << endl;
			}
		else {
			cout << "Path was NOT found" << endl;
			}
		}*/


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
		if(pkInput->Pressed(MOUSERIGHT))
			m_pkCameraObject->SetLocalRotM(kRm);	
	
	
		if(pkInput->Pressed(KEY_F1))
			m_iEditMode = EDIT_ZONES;
		
		if(pkInput->Pressed(KEY_F2))
			m_iEditMode = EDIT_OBJECTS;		
	
		if(pkInput->Pressed(KEY_I))
			pkZShader->SetForceLighting(ALWAYS_ON);	
		if(pkInput->Pressed(KEY_O))
			pkZShader->SetForceLighting(ALWAYS_OFF);
		if(pkInput->Pressed(KEY_P))
			pkZShader->SetForceLighting(MATERIAL);
	
	
		//edit zone  mode
		if(m_iEditMode == EDIT_ZONES)
		{
			if(pkInput->Pressed(MOUSELEFT))
			{
				AddZone();	
			}
	
			if(pkInput->Pressed(KEY_R))
			{
				int id = pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
				
				pkObjectMan->DeleteZone(id);
			}
			
			if(pkInput->Pressed(KEY_F))
			{
				if(pkFps->GetTicks() - m_fClickDelay > 0.2)
				{	
					m_fClickDelay = pkFps->GetTicks();						
					m_iCurrentMarkedZone = pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
					RotateActiveZoneObject();
				}
			}
			
			if(pkInput->Pressed(KEY_C))
			{
				if(pkFps->GetTicks() - m_fClickDelay > 0.2)
				{	
					m_fClickDelay = pkFps->GetTicks();
					m_iCurrentMarkedZone = pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
					pkObjectMan->SetUnderConstruction(m_iCurrentMarkedZone);
				}
			}
			
			if(pkInput->Pressed(KEY_V))
			{
				if(pkFps->GetTicks() - m_fClickDelay > 0.2)
				{	
					m_fClickDelay = pkFps->GetTicks();
					m_iCurrentMarkedZone = pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
					pkObjectMan->CommitZone(m_iCurrentMarkedZone);
				}
			}	
	
/*			if(pkInput->Pressed(MOUSEMIDDLE))
			{		
				m_iCurrentMarkedZone =  pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
			}
*/
	
			if(pkInput->Pressed(KEY_1)) m_kZoneSize.Set(4,4,4);
			if(pkInput->Pressed(KEY_2)) m_kZoneSize.Set(8,8,8);
			if(pkInput->Pressed(KEY_3)) m_kZoneSize.Set(16,16,16);	
			if(pkInput->Pressed(KEY_4)) m_kZoneSize.Set(32,16,32);	
			if(pkInput->Pressed(KEY_5)) m_kZoneSize.Set(64,16,64);			
			if(pkInput->Pressed(KEY_6)) m_kZoneSize.Set(16,8,8);		
			if(pkInput->Pressed(KEY_7)) m_kZoneSize.Set(8,8,16);		
			if(pkInput->Pressed(KEY_8)) m_kZoneSize.Set(4,8,16);				
			if(pkInput->Pressed(KEY_9)) m_kZoneSize.Set(16,8,4);						
		}	
	
		//edit object mode
		if(m_iEditMode == EDIT_OBJECTS)
		{	
			if(pkInput->Pressed(MOUSELEFT))
			{
				if(pkFps->GetTicks() - m_fClickDelay > 0.2)
				{	
					m_fClickDelay = pkFps->GetTicks();		
					pkObjectMan->CreateObjectFromScriptInZone(
						m_strActiveObjectName.c_str(), m_kObjectMarkerPos);
				}
			}
			
			if(pkInput->Pressed(MOUSEMIDDLE))
			{		
				if(pkFps->GetTicks() - m_fClickDelay > 0.2)
				{	
					m_fClickDelay = pkFps->GetTicks();		
					
					Entity* pkObj =  GetTargetObject();
				
					if(pkObj)
					{
						m_iCurrentObject = pkObj->iNetWorkID;
					}
				}
			}
			
			//remove			
			if(pkInput->Pressed(KEY_R))
			{
				
				Entity* pkObj = pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);
								
				if(pkObj)
				{
					//fulhack deluxe för att inte kunna ta bort statiska entitys i zoner som inte är underconstruction
					if(pkObj->GetParent()->GetName() == "StaticEntity")
					{
						cout<<"zone is not under construction "<<endl;
						return;
					}
						
					cout<<"Deleting object:"<<m_iCurrentObject<<endl;					
					pkObjectMan->Delete(pkObj);				
				}

			
				m_iCurrentObject = -1;
			}
		
			Entity* pkObj = pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);								
			if(!pkObj)
				return;		
		
			//return if its a static object
			if(pkObj->GetParent()->GetName() == "StaticEntity")
				return;
		
			//move left
			if(pkInput->Pressed(KEY_LEFT))
					pkObj->SetLocalPosV(pkObj->GetLocalPosV() + Vector3(-1 * pkFps->GetFrameTime(),0,0));			
			if(pkInput->Pressed(KEY_RIGHT))
					pkObj->SetLocalPosV(pkObj->GetLocalPosV() + Vector3(1 * pkFps->GetFrameTime(),0,0));			
			if(pkInput->Pressed(KEY_UP))
					pkObj->SetLocalPosV(pkObj->GetLocalPosV() + Vector3(0,0,-1 * pkFps->GetFrameTime()));			
			if(pkInput->Pressed(KEY_DOWN))
					pkObj->SetLocalPosV(pkObj->GetLocalPosV() + Vector3(0,0,1 * pkFps->GetFrameTime()));			
			if(pkInput->Pressed(KEY_RSHIFT))
					pkObj->SetLocalPosV(pkObj->GetLocalPosV() + Vector3(0,1 * pkFps->GetFrameTime(),0));			
			if(pkInput->Pressed(KEY_RCTRL))
					pkObj->SetLocalPosV(pkObj->GetLocalPosV() + Vector3(0,-1 * pkFps->GetFrameTime(),0));			
			//rotation		
			if(pkInput->Pressed(KEY_INSERT))
				pkObj->RotateLocalRotV(Vector3(100*pkFps->GetFrameTime(),0,0));			
			if(pkInput->Pressed(KEY_DELETE))
				pkObj->RotateLocalRotV(Vector3(-100*pkFps->GetFrameTime(),0,0));			
			if(pkInput->Pressed(KEY_HOME))
				pkObj->RotateLocalRotV(Vector3(0,100*pkFps->GetFrameTime(),0));			
			if(pkInput->Pressed(KEY_END))
				pkObj->RotateLocalRotV(Vector3(0,-100*pkFps->GetFrameTime(),0));			
			if(pkInput->Pressed(KEY_PAGEUP))
				pkObj->RotateLocalRotV(Vector3(0,0,100*pkFps->GetFrameTime()));			
			if(pkInput->Pressed(KEY_PAGEDOWN))
				pkObj->RotateLocalRotV(Vector3(0,0,-100*pkFps->GetFrameTime()));			
				
			//cout << "Pos:" << pkObj->GetLocalPosV().x << ", " << pkObj->GetLocalPosV().y << "," << pkObj->GetLocalPosV().z << endl;
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
		case FID_NEW:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				pkConsole->Printf("new [mapdir]");
				break;				
			}
			
			pkObjectMan->SetWorldDir(kCommand->m_kSplitCommand[1].c_str());
			pkObjectMan->Clear();
			
			GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);
			break;
		
		case FID_LOAD:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				pkConsole->Printf("load [mapdir]");
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
		
		case FID_SAVE:
			
			cout<<"saving world:"<<endl;
			
			pkObjectMan->ForceSave();
			pkObjectMan->SaveZones();			
			
			cout<<"saved"<<endl;
			
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
	
	//pkClient->m_pkObject->AddProperty("P_Primitives3D");	
	//TrackProperty* pkTrack = dynamic_cast<TrackProperty*>(pkClient->m_pkObject->GetProperty("TrackProperty"));
	//pkTrack->SetClient(iConID);

	P_ClientControl* pcc = (P_ClientControl*)pkClient->m_pkObject->AddProperty("P_ClientControl");
	if(pcc)	
		pcc->m_iClientID = iConID;
		
	UpdateStartLocatons();
	
	// moget ;)
	vector<string> kNames;
	kNames.push_back("Kalle");
	kNames.push_back("stefan");	
	kNames.push_back("olle");	
	kNames.push_back("bengt");	
	kNames.push_back("röva");	
	kNames.push_back("fitta");	
	kNames.push_back("hellvete");	
	kNames.push_back("kuk");	
	kNames.push_back("hora");	
	
	//create player object
	int iPlayerID  = CreatePlayer(kNames[iConID].c_str(),"Start",iConID);
	
	if(m_pkServerInfoP)
	{	
		//wich rights shuld a client have on its player caracter
		int playerrights = PR_OWNER|PR_CONTROLS|PR_LOOKAT;
		
		m_pkServerInfoP->AddPlayer(iConID,kNames[iConID].c_str());
		m_pkServerInfoP->AddObject(iConID,iPlayerID,playerrights);
	}
}

void MistServer::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	DeletePlayer(iConID);

	if(m_pkServerInfoP)
		m_pkServerInfoP->RemovePlayer(iConID);	
	cout<<"Client "<<iConID<<" Parted"<<endl;	
}


void MistServer::OnServerStart(void)
{		
	//create a camera for the server
	m_pkCameraObject = pkObjectMan->CreateObjectFromScript("data/script/objects/t_camera.lua");
	if(m_pkCameraObject)
	{	
		m_pkCameraObject->SetParent(pkObjectMan->GetWorldObject());
		P_Camera* m_pkCamProp = (P_Camera*)m_pkCameraObject->GetProperty("P_Camera");
		m_pkCamProp->SetCamera(m_pkCamera);
		
	}
	
	//create server info object
	m_pkServerInfo = pkObjectMan->CreateObjectFromScript("data/script/objects/t_serverinfo.lua");
	if(m_pkServerInfo)
	{
		m_pkServerInfo->SetParent(pkObjectMan->GetGlobalObject());
		m_pkServerInfoP = (P_ServerInfo*)m_pkServerInfo->GetProperty("P_ServerInfo");		
		if(m_pkServerInfoP)
		{
			cout<<"Server info created"<<endl;
			m_pkServerInfoP->m_sServerName = "Test Server";
		}
		else
			cout<<"ERROR: No server P_ServerInfo property created, this is no good"<<endl;
	}
	
}

void MistServer::OnClientStart(void)
{
}

bool MistServer::StartUp()	
{ 
	m_pkAStar	= static_cast<AStar*>(GetSystem().GetObjectPtr("AStar"));
	return true; 
}

bool MistServer::ShutDown()	{ return true; }
bool MistServer::IsValid()	{ return true; }

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

Entity* MistServer::GetTargetObject()
{
	Vector3 start = pkFps->GetCam()->GetPos();
	Vector3 dir = Get3DMousePos();

	vector<Entity*> kObjects;
	kObjects.clear();
	
	pkObjectMan->TestLine(&kObjects,start,dir);
	
	
	float closest = 9999999999;
	Entity* pkClosest = NULL;	
	for(unsigned int i=0;i<kObjects.size();i++)
	{
		if(kObjects[i] == m_pkCameraObject)
			continue;
		
		if(kObjects[i]->iNetWorkID <100000)
			continue;
		
		if(kObjects[i]->GetName() == "ZoneObject")
			continue;
		
		if(kObjects[i]->GetName() == "StaticEntity")
			continue;
		
		if(kObjects[i]->GetName() == "A t_serverinfo.lua")
			continue;
		
		
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
	if(pkObjectMan->IsInsideZone(m_kZoneMarkerPos,m_kZoneSize))
		return;
		
	int id = pkObjectMan->CreateZone(m_kZoneMarkerPos,m_kZoneSize);

	//force loading of this zone
	pkObjectMan->LoadZone(id);

	//set to active
	m_iCurrentMarkedZone = id;

	if(id != -1)
	{
		pkObjectMan->SetZoneModel(m_strActiveZoneName.c_str(),id);
	}	

	
	SetZoneEnviroment(m_strActiveEnviroment.c_str());
}


void MistServer::DrawZoneMarker(Vector3 kPos)
{
	Vector3 bla = m_kZoneSize / 2;
	pkRender->DrawAABB(kPos-bla,kPos+bla,Vector3(1,1,1));
}


void MistServer::DrawCrossMarker(Vector3 kPos)
{
	pkRender->Line(kPos-Vector3(1,0,0),kPos+Vector3(1,0,0));
	pkRender->Line(kPos-Vector3(0,1,0),kPos+Vector3(0,1,0));	
	pkRender->Line(kPos-Vector3(0,0,1),kPos+Vector3(0,0,1));	
}


void MistServer::UpdateZoneMarkerPos()
{
	Vector3 temp = pkFps->GetCam()->GetPos() + Get3DMousePos(false)*15;

	float fStep = 2.0;

	m_kZoneMarkerPos.x = int(temp.x/fStep) * fStep;
	m_kZoneMarkerPos.y = int(temp.y/fStep) * fStep;
	m_kZoneMarkerPos.z = int(temp.z/fStep) * fStep;


	m_kZoneMarkerPos.x = round2(temp.x/fStep) * fStep;
	m_kZoneMarkerPos.y = round2(temp.y/fStep) * fStep;
	m_kZoneMarkerPos.z = round2(temp.z/fStep) * fStep;
	
/*	if(m_kZoneSize.x != 4) m_kZoneMarkerPos.x = round2(temp.x/fStep) * fStep;
		else  m_kZoneMarkerPos.x = round2(temp.x/fStep) * fStep + 2;						

	if(m_kZoneSize.y != 4) m_kZoneMarkerPos.y = round2(temp.y/fStep) * fStep;
		else  m_kZoneMarkerPos.y = round2(temp.y/fStep) * fStep + 2;						

	if(m_kZoneSize.z != 4) m_kZoneMarkerPos.z = round2(temp.z/fStep) * fStep;
		else  m_kZoneMarkerPos.z = round2(temp.z/fStep) * fStep + 2;						
*/
//	m_kZoneMarkerPos.y = round2(temp.y/4.0) * 4.0;
//	m_kZoneMarkerPos.z = round2(temp.z/4.0) * 4.0;

}


void MistServer::UpdateObjectMakerPos()
{
	m_kObjectMarkerPos = pkFps->GetCam()->GetPos() + Get3DMousePos(true)*2;
}


void MistServer::OnCommand(int iID, ZGuiWnd *pkMainWnd)
{
	ZGuiWnd* pkWndClicked = GetWnd(iID);

	if(pkWndClicked)
	{
		string strMainWnd = pkMainWnd->GetName();

		string strWndClicked = pkWndClicked->GetName();

		string strParent = "null";

		if(pkWndClicked->GetParent())
			strParent = pkWndClicked->GetParent()->GetName();

		if(strMainWnd == "MainWnd")
		{
			if(strWndClicked == "OpenWorkTabButton")
			{
				bool bExist = true;

				if(GetWnd("ZonePage") == NULL)
					bExist = false;

				pkScript->Call(m_pkScriptResHandle, "OpenWorkPad", 0, 0);
				
				if(!bExist)
				{
					BuildFileTree("ZoneModelTree", "data/mad/zones");
					BuildFileTree("ObjectTree", "data/script/objects");
					//GetWnd("WorkTabWnd")->SetMoveArea(Rect(0,0,800,600), true);
				}
			}
			else
			if(strWndClicked == "ToogleLight")
			{
				ToogleLight( static_cast<ZGuiCheckbox*>(pkWndClicked)->IsChecked() );
			}
			else
			if(strWndClicked == "MainMenu")
			{
				printf("oooooo\n");
			}
		}
		else
		if(strMainWnd == "ZonePage")
		{
			if(strWndClicked == "RotateZoneModellButton")
			{
				RotateActiveZoneObject();
			}
			else
			if(strWndClicked == "DeleteZoneButton")
			{
				int id = pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
				pkObjectMan->DeleteZone(id);
			}
		}
		else
		if(strMainWnd == "ObjectPage")
		{
			if(strWndClicked == "DeleteObjectButton")
			{		
				Entity* pkObj = pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);		
				if(pkObj) {
					pkObjectMan->Delete(pkObj);
					m_iCurrentObject = -1;
				}
			}
			else
			if(strWndClicked == "PlaceongroundButton")
			{
				Entity* pkObj = pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);		
				if(pkObj) {
					Vector3 pos = pkObj->GetLocalPosV(); pos.y = 0.0;
					pkObj->SetLocalPosV(pos); 
					m_iCurrentObject = -1;
				}
			}
		}

	}

}

void MistServer::OnClickListbox(int iListBoxID, int iListboxIndex, ZGuiWnd* pkMain)
{
	if(pkMain == NULL)
		return;

	string strMainWndName, strListBox;

	ZGuiWnd* pkListBox = NULL;

	list<ZGuiWnd*> kChilds;
	pkMain->GetChildrens(kChilds); 

	list<ZGuiWnd*>::iterator it = kChilds.begin();
	for( ; it != kChilds.end(); it++)
	{
		if((*it)->GetID() == (unsigned int) iListBoxID)
		{
			pkListBox = (*it);
			break;
		}	 
	}

	strMainWndName = pkMain->GetName();

	if(pkListBox != NULL)
		strListBox = pkListBox->GetName();

	printf("%s\n", strMainWndName.c_str() );

	if(strMainWndName == "EnviromentPage")
	{
		if(strListBox == "EnviromentPresetList")
		{
			char *szPreset = static_cast<ZGuiListbox*>(pkListBox)->GetSelItem()->GetText();

			if(szPreset)
			{
				string szFull = "data/enviroments/" + string(szPreset);
				printf("setting enviroment %s\n", szFull.c_str());
				SetZoneEnviroment( szFull.c_str()  );  
			}
		}
	}
	else
	if(strMainWndName == "MainMenu")
	{
		// Run Menu command
		for(int i=0; i<m_uiNumMenuItems; i++)
		{
			if( m_pkMenuInfo[i].iIndex == iListboxIndex)
			{
				char* cmd = m_pkMenuInfo[i].szCommando;
				pkFps->m_pkConsole->Execute(cmd);
				break;
			}
		}
	}
}

void MistServer::OnClickTreeItem(char *szTreeBox, char *szParentNodeText, 
											char *szClickNodeText, bool bHaveChilds)
{
	if(strcmp(szTreeBox, "ZoneModelTree") == 0)
	{
		if(szClickNodeText && bHaveChilds == false)
		{
			string strFullpath = string("data/mad/zones/");

			if(szParentNodeText)
				strFullpath += string(szParentNodeText);

			if(szClickNodeText)
				strFullpath += string(szClickNodeText);

			m_strActiveZoneName = strFullpath;
			
			m_iCurrentMarkedZone = pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
			// Setting new zone modell
			if(m_iCurrentMarkedZone != -1)	// ÄR någon zon markerad?
			{
				//pkObjectMan->LoadZone(m_iCurrentMarkedZone);
				pkObjectMan->SetZoneModel(strFullpath.c_str(),m_iCurrentMarkedZone);
				printf("Setting new zone modell to %s\n", strFullpath.c_str());
			}
		}
	}
	else
	if(strcmp(szTreeBox, "ObjectTree") == 0)
	{
		if(szClickNodeText && bHaveChilds == false)
		{
			string strFullpath = string("data/script/objects/");

			if(szParentNodeText)
				strFullpath += string(szParentNodeText);

			if(szClickNodeText)
				strFullpath += string(szClickNodeText);

			m_strActiveObjectName = strFullpath;

			printf("%s\n", m_strActiveObjectName.c_str());
		}
	}
}

void MistServer::OnClickTabPage(ZGuiTabCtrl *pkTabCtrl, int iNewPage, int iPrevPage)
{
	string strTabCtrlName = pkTabCtrl->GetName();

	if(strTabCtrlName == "WorkTabWnd")
	{
		switch(iNewPage)
		{
		case 0:
			m_iEditMode = EDIT_ZONES;
			break;
		case 1:
			m_iEditMode = EDIT_OBJECTS;
			break;
		}
	}
}

void MistServer::RotateActiveZoneObject()
{
	if(m_iCurrentMarkedZone != -1)
	{
		ZoneData* pkData = pkObjectMan->GetZoneData(m_iCurrentMarkedZone);
		if(pkData)
			pkData->m_pkZone->RotateLocalRotV( Vector3(0,90.0f,0) ); 
	}
}

void MistServer::ToogleLight(bool bEnabled)
{
	if(bEnabled)
		pkZShader->SetForceLighting(ALWAYS_ON);
	else
		pkZShader->SetForceLighting(ALWAYS_OFF);
}

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
			return m_kLocations[i].second+Vector3((rand()%1000)/1000.0,0,(rand()%1000)/1000.0);
		}
	
	return Vector3(0,0,0);
}

int MistServer::CreatePlayer(const char* csName,const char* csLocation,int iConID)
{
	cout<<"creating new player:"<<csName<<endl;
	
	Vector3 kStartPos = GetPlayerStartLocation(csLocation);
	
	Entity* pkObject = pkObjectMan->CreateObjectFromScriptInZone("data/script/objects/t_player.lua",kStartPos);
	
	if(pkObject)
	{	
		pkObject->GetSave() = false;
		pkObject->AddProperty("P_Track");	
		P_Track* pkTrack = dynamic_cast<P_Track*>(pkObject->GetProperty("P_Track"));	
		if(pkTrack)
			pkTrack->SetClient(iConID);	

		printf("id %i\n", pkObject->iNetWorkID);
	}
	else
	{	
		cout<<"Error creating player caracter"<<endl;
		return -1;
	}
	
	cout<<"player created"<<endl;	
		
	return pkObject->iNetWorkID;
}

void MistServer::DeletePlayer(int iConID)
{
	if(m_pkServerInfoP)
	{
		PlayerInfo* pi = m_pkServerInfoP->GetPlayerInfo(iConID);
		if(pi)
		{
			for(unsigned int i = 0;i<pi->kControl.size();i++)
			{
				if(pi->kControl[i].second & PR_OWNER)
				{
					Entity* pkObj = pkObjectMan->GetObjectByNetWorkID(pi->kControl[i].first);
					
					//save object here
					
					//delete it
					if(pkObj)
						pkObjectMan->Delete(pkObj);
				}
			}		
		}
	}
}

void MistServer::PathTest() 
{
	return;

	int iNumOfZones = pkObjectMan->GetNumOfZones();
	if(iNumOfZones < 10)
		return; 

	int iRuns = 10;

	for(int i=0; i<iRuns; i++) {

		int iStartZone  = 10;
		int iEndZone	= 1;

		kPathStart = pkObjectMan->GetZoneCenter(iStartZone);
		kPathEnd   = pkObjectMan->GetZoneCenter(iEndZone);

		bool bres = m_pkAStar->GetPath(kPathStart,kPathEnd,kPath);
		}
}

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
		cout<<"handling order "<<order->m_sOrderName<<" from client:"<<order->m_iClientID<<endl;
		
		//handle input messages from client
		if(strncmp(order->m_sOrderName.c_str(),"(IM)",4) == 0) 
		{
			order->m_sOrderName.erase(0,4);
			string playername; 
			string message;
			
			int pos;
			for(pos=0;pos<order->m_sOrderName.size();pos++)
			{
				if(order->m_sOrderName[pos] == ' ' && playername.size()>0)
					break;
				playername+=order->m_sOrderName[pos];
			}
			
			for(;pos<order->m_sOrderName.size();pos++)
			{	
				message+=order->m_sOrderName[pos];
			}
				
			//cout<<"got message to "<<playername<<": "<<message<<endl;
			
			if(m_pkServerInfoP)
			{
				m_pkServerInfoP->MessagePlayer(playername.c_str(),message);
			}
		}
		else if(order->m_iFace != -1) 		//else ground klick
		{
			Entity* ob = pkObjectMan->GetObjectByNetWorkID(order->m_iCharacter);			
			if(ob)
			{
				/* Vim Test Path*/
				kPathStart = ob->GetWorldPosV();
				kPathEnd   = order->m_kPos;
				kPath.clear();
				bool bres = m_pkAStar->GetPath(kPathStart,kPathEnd,kPath);

				if(bres) {
					cout << "Path was found. Size " << kPath.size()  << endl;
					P_PfMesh* pm = (P_PfMesh*)ob->GetProperty("P_PfMesh");
					if(pm) {
						reverse(kPath.begin(), kPath.end());
						kPath.push_back(order->m_kPos);
						pm->SetPath(kPath);
						}
					
					}
				else {
					cout << "Path was NOT found" << endl;
					}



				/*P_Event* pe = (P_Event*)ob->GetProperty("P_Event");
				if(pe)
					pe->SendGroudClickEvent(order->m_sOrderName.c_str(), order->m_kPos,order->m_iFace,order->m_iCharacter ,order->m_iZoneObjectID);
				*/
			}

		}

      // request orders
      else if (strncmp(order->m_sOrderName.c_str(),"(rq)",4) == 0 )
      {
         // type of request

         //unsigned char* strRequest = _mbsninc( (const unsigned char*)order->m_sOrderName.c_str(), 4 );

         // item request
         if ( strncmp(order->m_sOrderName.c_str(), "item", 4) )
         {
   			Entity* pkItemObject = pkObjectMan->GetObjectByNetWorkID(order->m_iObjectID);

            if ( pkItemObject )
            {
               P_Item *pkItProp = (P_Item*) pkItemObject->GetProperty("P_Item");
               
               if ( pkItProp )
               {
                  // if the items is of the same version, no need to send data
                  if ( pkItProp->m_pkItemStats->m_uiVersion != order->m_iFace )
                  {
                     pkItProp->m_kSends.push_back ( order->m_iClientID );
                  }

               }
               else
                  cout << "Error! Non-P_Item_Object requested for updated iteminfo! This should't be possible!!!" << endl;
            }


         }

      }

		else
		{
			//other orders
			Entity* ob = pkObjectMan->GetObjectByNetWorkID(order->m_iObjectID);
			if(ob)
			{
				P_Event* pe = (P_Event*)ob->GetProperty("P_Event");
				if(pe)
				{	
					pe->SendObjectClickEvent(order->m_sOrderName.c_str(), order->m_iCharacter);				
				
				}			
			}
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
	}
	
	return false;
}

bool MistServer::BuildFileTree(char* szTreeBoxName, char* szRootPath)
{
	// kolla inparametrar
	if(szRootPath == NULL || szTreeBoxName == NULL)
		return false;

	// sista tecknet får inte vara ett '/' tecken
	if(szRootPath[strlen(szRootPath)] == '/')
		szRootPath[strlen(szRootPath)] = '\0';

	set<string> kSearchedFiles;
	list<string> dir_list;
	string strPrevNode;

	dir_list.push_back(string(szRootPath));
	strPrevNode = "RootNode";
	
	while(1)
	{
		list<string> vkFileNames;
		string currentFolder = dir_list.back();

		// Hämta filerna i den aktuella katalogen och sortera listan.
		vector<string> t;
		pkZFVFileSystem->ListDir(&t, currentFolder);
		for(unsigned int i=0; i<t.size(); i++)
			vkFileNames.push_back(t[i]); 
		t.clear(); vkFileNames.sort(SortFiles);

		// Lägg till alla filer
		for(list<string>::iterator it = vkFileNames.begin(); it != vkFileNames.end(); it++)  
		{
			string strLabel = (*it);
			string id = currentFolder + string("/") + strLabel;

			bool bIsFolder = strLabel.find(".") == string::npos;

			if(kSearchedFiles.find(strLabel) == kSearchedFiles.end())
			{			
				if(bIsFolder)
				{
					string d = currentFolder + string("/") + strLabel;
					dir_list.push_back(d);

					AddTreeItem(szTreeBoxName, id.c_str(), 
						strPrevNode.c_str(), (char*) strLabel.c_str(), 1, 2);
				}
				else
					AddTreeItem(szTreeBoxName, id.c_str(), 
						strPrevNode.c_str(), (char*) strLabel.c_str(), 0, 1);

				kSearchedFiles.insert(strLabel);
			}
		}

		// ej klivit in i ett nytt dir? gå tillbaks
		if(dir_list.back() == currentFolder)
		{
			// sista?
			if(currentFolder == szRootPath)
				break;

			dir_list.pop_back();
		}

		strPrevNode = dir_list.back();
	}

	return true;

}

void MistServer::SendTextToMistClientInfoBox(char *szText)
{
	  
}


void MistServer::SetZoneEnviroment(const char* csEnviroment)
{
	//set default enviroment
	m_strActiveEnviroment=csEnviroment;
	
	m_iCurrentMarkedZone = pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
	ZoneData* z = pkObjectMan->GetZoneData(m_iCurrentMarkedZone);
		
	if(z)
	{
		//cout<<"Setting enviroment to :"<<csEnviroment<<endl;
		z->m_strEnviroment = csEnviroment;
	}	
}

string MistServer::GetZoneEnviroment()
{
	string env;
	
	m_iCurrentMarkedZone = pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
	ZoneData* z = pkObjectMan->GetZoneData(m_iCurrentMarkedZone);
		
	if(z)
		env = z->m_strEnviroment;

	return env;
}

char* MistServer::GetSelEnviromentString()
{
	ZGuiListbox* pkEnviromentList = static_cast<ZGuiListbox*>(GetWnd("EnviromentPresetList"));
	if(pkEnviromentList)
	{
		ZGuiListitem* pkSel = pkEnviromentList->GetSelItem(); 
		if(pkSel)
			return pkSel->GetText();
	}

	return NULL;
}

bool MistServer::CreateMenu(char* szFileName)
{
	ZGuiFont* pkFont = pkGui->GetBitmapFont(ZG_DEFAULT_GUI_FONT);
	if(pkFont == NULL)
	{
		printf("Failed to find font for menu!\n");
		return false;
	}

	CreateWnd(Wnd, "MainMenu", "MainWnd", "", 0,0, 1024, 20, 0);

	ZGuiWnd* pkMenu = GetWnd("MainMenu");
	pkMenu->SetSkin(GetSkin("NullSkin"));

	if(!pkIni->Open(szFileName, false))
	{
		cout << "Failed to load ini file for menu!\n" << endl;
		return false;
	}

	vector<string> akSections;
	pkIni->GetSectionNames(akSections);

	unsigned int uiNumSections = akSections.size();
	
	// No items in file.
	if(uiNumSections < 1)
		return true;

	Rect rcMenu;
	unsigned int i=0, iMenuOffset=0, iMenuWidth=0, iMenuIDCounter=45781;
	char szParentName[50];

	// Skapa alla parents
	for(i=0; i<uiNumSections; i++)
	{
		char* parent = pkIni->GetValue(akSections[i].c_str(), "Parent");
		if(parent == NULL)
			continue;

		if(strcmp(parent, "NULL") == 0)
		{
			char szTitle[50];
			sprintf(szTitle, " %s", pkIni->GetValue(akSections[i].c_str(), "Title"));
			iMenuWidth = pkFont->GetLength(szTitle) + 6; // move rc right

			rcMenu = Rect(iMenuOffset,0,iMenuOffset+iMenuWidth,20);

			CreateWnd(Combobox, (char*)akSections[i].c_str(), "MainMenu", szTitle,
				rcMenu.Left, rcMenu.Top, rcMenu.Width(), rcMenu.Height(), 0);

			ZGuiCombobox* pkMenuCBox = static_cast<ZGuiCombobox*>(GetWnd(
				(char*)akSections[i].c_str()));

			pkMenuCBox->SetGUI(pkGui);
			pkMenuCBox->SetLabelText(szTitle);
			pkMenuCBox->SetNumVisibleRows(1);
			pkMenuCBox->IsMenu(true);
			
			iMenuOffset += iMenuWidth;
			rcMenu = rcMenu.Move(iMenuOffset,0);
		}
	}

	ZGuiWnd* pkParent;
	vector<MENU_INFO> kTempVector;

	// Skapa alla childrens.
	char szCommando[512];
	int item_counter = 0;
	for(i=0; i<uiNumSections; i++)
	{
		char* parent = pkIni->GetValue(akSections[i].c_str(), "Parent");
		if(parent == NULL)
			continue;

		strcpy(szParentName, parent);
		if(strcmp(szParentName, "NULL") != 0)
		{
			pkParent = GetWnd(szParentName);

			if(pkParent != NULL)
			{
				char szTitle[50];
				sprintf(szTitle, " %s", pkIni->GetValue(akSections[i].c_str(), "Title"));
				((ZGuiCombobox*) pkParent)->AddItem(szTitle, item_counter++);

				MENU_INFO mi;
				mi.cb = (ZGuiCombobox*) pkParent;
				mi.iIndex = item_counter-1;
				char* szCmd = pkIni->GetValue(akSections[i].c_str(), "Cmd");
				if(szCmd != NULL)
					strcpy(szCommando, szCmd);
				else
					strcpy(szCommando, "No commando!");

				mi.szCommando = new char[strlen(szCommando)+1];
				strcpy(mi.szCommando, szCommando);
				kTempVector.push_back(mi);
			}
		}
	}

	// Copy from tempvektor.
	m_uiNumMenuItems = kTempVector.size();
	m_pkMenuInfo = new MENU_INFO[m_uiNumMenuItems];
	for(i=0; i<(unsigned int) m_uiNumMenuItems; i++)
	{
		m_pkMenuInfo[i].cb = kTempVector[i].cb;
		m_pkMenuInfo[i].iIndex = kTempVector[i].iIndex;
		m_pkMenuInfo[i].szCommando = new char[strlen(kTempVector[i].szCommando)+1];
		strcpy(m_pkMenuInfo[i].szCommando, kTempVector[i].szCommando);
		delete[] kTempVector[i].szCommando;
	}

	return true;
}