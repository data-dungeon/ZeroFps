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
#include "../zerofpsv2/engine_systems/propertys/p_skyboxrender.h"
#include "../zerofpsv2/engine_systems/propertys/p_hmrp2.h"

#include "../zerofpsv2/engine/p_pfpath.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../zerofpsv2/basic/zguifont.h"
#include "../mcommon/rulesystem/sendtype.h"
#include <set> 
#include <algorithm>

#include "../zerofpsv2/gui/zguiresourcemanager.h"


MistServer g_kMistServer("MistServer",0,0,0);

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
	switch(msg)
	{
	case ZGM_COMMAND:
		g_kMistServer.OnCommand(((int*)params)[0], (((int*)params)[1] == 1) ? true : false, win);
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

	// Set Default values
	m_AcceptNewLogins = true;
	m_bUpdateMarker = true;

	// Register Variables
	RegisterVariable("s_newlogins",				&m_AcceptNewLogins,			CSYS_BOOL);	
	RegisterVariable("s_marker",					&m_bUpdateMarker,			CSYS_BOOL);	

	// Register Commands
	Register_Cmd("new",FID_NEW);		
	Register_Cmd("load",FID_LOAD);		
	Register_Cmd("save",FID_SAVE);		
	Register_Cmd("users",FID_USERS);		
	Register_Cmd("lo",FID_LOCALORDER);		
	Register_Cmd("lightmode", FID_LIGHTMODE);		

	m_kDrawPos.Set(0,0,0);

	m_fHMInRadius  = 1;
	m_fHMOutRadius = 2;
} 

void MistServer::OnInit() 
{
	m_pkConsole->Printf(" MistServer (mistland dedicated server)");
	m_pkConsole->Printf("--------------------------------");
	m_pkConsole->Printf("");

	Init();

	//run autoexec script
	if(!m_pkIni->ExecuteCommands("mistserver_autoexec.ini"))
		m_pkConsole->Printf("No mistserver_autoexec.ini found");
}

void MistServer::Init()
{	
	//create player database
	m_pkPlayerDB = new PlayerDatabase();

	//default edit mode 
	m_iEditMode = EDIT_ZONES;

	//object defaults
	m_iCurrentObject = -1;
	
	//zone defaults
	m_kZoneSize.Set(8,8,8);
	m_iCurrentMarkedZone = -1;
//	m_strActiveZoneName = "data/mad/zones/emptyzone.mad";
	m_strActiveZoneName = "";
	m_strActiveEnviroment = "data/enviroments/sun.env";

	//click delay
	m_fClickDelay = m_pkFps->GetTicks();
	
	//damn "#¤(="%#( lighting fix bös
	m_pkLight->SetLighting(true);
	m_pkZShader->SetForceLighting(LIGHT_ALWAYS_OFF);	
	
	//register property bös
	RegisterPropertys();

	//register resources
	RegisterResources();

	//initiate our camera
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.25,250);	

	//init mistland script intreface
	MistLandLua::Init(m_pkObjectMan,m_pkScript);
	
	// create gui script funktions
	GuiAppLua::Init(&g_kMistServer, m_pkScript);

	// init gui
	InitializeGui(pkGui, pkTexMan, m_pkScript, m_pkGuiMan, 
		"data/textures/text/ms_sans_serif8.bmp",
		"data/script/gui/gui_create_server.lua");
		//"data/script/gui/test2s.lua");

	CreateGuiInterface();

//	pkGui->SetCursor(0,0, pkTexMan->Load("data/textures/gui/cursor.bmp", 0),
//		pkTexMan->Load("data/textures/gui/cursor_a.bmp", 0), 32, 32);

	//setup caption
	SetTitle("MistServer");
	// hide cursor
	m_pkInput->ShowCursor(true);

	// give focus to main window
	//pkGui->SetFocus(GetWnd("MainWnd")); 

	// Init tooltip
	pkGui->GetToolTip()->SetToolTip(GetWnd("ToogleLight"),"Light");
	pkGui->GetToolTip()->SetToolTip(GetWnd("OpenWorkTabButton"),"Worktab");
	
	ZGuiSkin kSkin(pkTexMan->Load("data/textures/gui/sb_bk.bmp", 0), true);
	kSkin.m_unBorderSize = 1;
	memset(kSkin.m_afBorderColor, 0, sizeof(float)*3);
	pkGui->GetToolTip()->SetSkin(kSkin);	

	m_pkInput->ToggleGrab();

//	m_pkPlayerDB->GetLoginCharacters(string("user"));

	m_kSun.kRot= Vector3(1,2,1);
	m_kSun.kDiffuse=Vector4(1,1,1,1);
	m_kSun.kAmbient=Vector4(0.05,0.05,0.05,1);
	m_kSun.iType=DIRECTIONAL_LIGHT;			
	m_kSun.iPriority=10;
	m_kSun.fConst_Atten=1;
	m_kSun.fLinear_Atten=0;
	m_kSun.fQuadratic_Atten=0;
	m_pkLight->Add(&m_kSun);
}

	

void MistServer::RegisterResources()
{
	m_pkResourceDB->RegisterResource( string(".env"), Create__EnvSetting	);
}

void MistServer::RegisterPropertys()
{
	m_pkPropertyFactory->Register("P_Spawn", Create_P_Spawn);
	m_pkPropertyFactory->Register("P_Enviroment", Create_P_Enviroment);
	m_pkPropertyFactory->Register("P_ClientControl", Create_P_ClientControl);
	m_pkPropertyFactory->Register("P_ServerInfo", Create_P_ServerInfo);
	m_pkPropertyFactory->Register("P_Ml", Create_P_Ml);
	m_pkPropertyFactory->Register("P_Event", Create_P_Event);
	m_pkPropertyFactory->Register("P_CharStats", Create_P_CharStats);
   m_pkPropertyFactory->Register("P_Item", Create_P_Item);
   m_pkPropertyFactory->Register("P_Spell", Create_P_Spell);
   m_pkPropertyFactory->Register("P_AI", Create_P_AI);
   m_pkPropertyFactory->Register("P_Container", Create_P_Container);
}

void MistServer::DrawHMEditMarker(HeightMap* pkHmap, Vector3 kCenterPos, float fInRadius, float fOutRadius )
{
	if(pkHmap == NULL)	return;

	m_pkRender->DrawBillboard(m_pkFps->GetCam()->GetModelViewMatrix(),kCenterPos,1,pkTexMan->Load("../data/textures/pointer.tga",T_NOMIPMAPPING));	

	Vector3				kVertex;
	vector<Vector3>	kVertexList;

//	float x,z;
	kCenterPos.y = 0;
	Vector3 kPos;

	for(int i=0; i<360; i+=(int)12.25) {
		kVertex.x = float( cos(DegToRad( float(i) )) * fInRadius );
		kVertex.z = float( sin(DegToRad( float(i) )) * fInRadius );
		kVertex.y = pkHmap->Height(kCenterPos.x+kVertex.x,kCenterPos.z + kVertex.z) + 0.01;
		kVertex += kCenterPos;
		kVertexList.push_back(kVertex);
	}

	m_pkRender->DrawCircle(kVertexList, Vector3(0.8,0.8,0));

	kVertexList.clear();
	for(int i=0; i<360; i+=(int)12.25) {
		kVertex.x = float( cos(DegToRad( float(i) )) * fOutRadius );
		kVertex.z = float( sin(DegToRad( float(i) )) * fOutRadius );
		kVertex.y = pkHmap->Height(kCenterPos.x+kVertex.x,kCenterPos.z + kVertex.z) + 0.01;
		kVertex += kCenterPos;
		kVertexList.push_back(kVertex);
	}

	m_pkRender->DrawCircle(kVertexList, Vector3(1,1,0));
}

void MistServer::OnIdle()
{	

	m_pkFps->SetCamera(m_pkCamera);		
	m_pkFps->GetCam()->ClearViewPort();	

	if(pkGui->m_bHaveInputFocus == false)
	{
		Input();	
	}

 	m_pkFps->UpdateCamera(); 		

	//for(unsigned int iPath = 0; iPath < kPath.size(); iPath++)
	//	pkRender->Draw_MarkerCross(kPath[iPath],Vector3(1,1,1),1);
   
	if(m_pkServerInfoP)
	{
		m_pkFps->DevPrintf("server","ServerName: %s", m_pkServerInfoP->GetServerName().c_str());
		m_pkFps->DevPrintf("server","Players: %d", m_pkServerInfoP->GetNrOfPlayers());	
	}
	
	if(m_iEditMode == EDIT_HMAP) {
		HeightMap* pkMap = SetPointer();
		DrawHMEditMarker(pkMap, m_kDrawPos, m_fHMInRadius,m_fHMOutRadius);
		}

	if(m_iEditMode == EDIT_ZONES)
	{
		UpdateZoneMarkerPos();		
		DrawZoneMarker(m_kZoneMarkerPos);		
		
		
		//draw selected zone marker
		if(m_iCurrentMarkedZone != -1)
		{
			ZoneData* z = m_pkObjectMan->GetZoneData(m_iCurrentMarkedZone);
		
			if(z)
			{
				Vector3 kMin = z->m_kPos - z->m_kSize/2;
				Vector3 kMax = z->m_kPos + z->m_kSize/2;
		
				m_pkRender->DrawAABB( kMin,kMax, Vector3(1,1,0) );
			}
		}
		
	}
	
	if(m_iEditMode == EDIT_OBJECTS)
	{	
		UpdateObjectMakerPos();
		
		DrawCrossMarker(m_kObjectMarkerPos);		
	
		if(m_iCurrentObject != -1)
		{
			Entity* pkObj = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);
			
			if(pkObj)
			{
				Vector3 kMin = pkObj->GetWorldPosV() - pkObj->GetRadius()/2;
				Vector3 kMax = pkObj->GetWorldPosV() + pkObj->GetRadius()/2;
		
				m_pkRender->DrawAABB( kMin,kMax, Vector3(1,1,0) );
			}
		}
	}

	PathTest();
}

void MistServer::OnSystem()
{
	HandleOrders();
	

}

HeightMap* MistServer::SetPointer()
{
	if(m_bUpdateMarker == false)
		return NULL;

	m_kDrawPos.Set(0,0,0);

	int id = m_iCurrentMarkedZone;	//m_pkObjectMan->GetZoneIndex(m_iCurrentMarkedZone,-1,false);
	
	if(id ==-1)	return NULL;
	ZoneData* kZData = m_pkObjectMan->GetZoneData(id);
	if(!kZData)	return NULL;
	P_HMRP2* hmrp = dynamic_cast<P_HMRP2*>(kZData->m_pkZone->GetProperty("P_HMRP2"));
	if(!hmrp)	return NULL;

	Vector3 start	= m_pkFps->GetCam()->GetPos();
	Vector3 dir		= Get3DMousePos(true);
	Vector3 end    = start + dir * 1000;

	if(dir.y >= 0) return NULL;

	// Find Level 0 for the selected zone in u.
	//float fLevelZero = kZData->m_pkZone->GetWorldPosV().y;
	//float fDiff = start.y - fLevelZero;
	//m_kDrawPos = start + dir * (fDiff);
	Plane kP;
	kP.m_kNormal.Set(0,1,0);
	kP.m_fD = - kZData->m_pkZone->GetWorldPosV().y;

	Vector3 kIsect;

	if(kP.LineTest(start,end, &kIsect)) {
		m_kDrawPos = kIsect;
		m_kDrawPos.y = hmrp->m_pkHeightMap->Height(m_kDrawPos.x,m_kDrawPos.z);
		}

	return hmrp->m_pkHeightMap;


	//Vector3 kLocalOffset = m_kDrawPos - hmrp->m_pkHeightMap->m_kCornerPos;
	//cout << "Local pos: " << kLocalOffset.x << ", " << kLocalOffset.y << ", " << kLocalOffset.z << endl;
}

void MistServer::HMModifyCommand(float fSize)
{
	float fTime = m_pkFps->GetGameFrameTime();

	for(int i=0; i<m_pkObjectMan->GetNumOfZones(); i++) {
		ZoneData* kZData = m_pkObjectMan->GetZoneData(i);
		if(kZData == NULL)	continue;
		P_HMRP2* hmrp = dynamic_cast<P_HMRP2*>(kZData->m_pkZone->GetProperty("P_HMRP2"));
		if(hmrp == NULL)		continue;
	
		Vector3 kLocalOffset = m_kDrawPos - hmrp->m_pkHeightMap->m_kCornerPos;

		m_kSelectedHMVertex = hmrp->m_pkHeightMap->GetSelection(m_kDrawPos,m_fHMInRadius,m_fHMOutRadius);
		if(m_kSelectedHMVertex.size() > 0) {
			hmrp->m_pkHeightMap->Raise(m_kSelectedHMVertex, fSize * fTime);
			m_kSelectedHMVertex.clear();
			}
		}
}

// Handles input for EditMode Terrain.
void MistServer::Input_EditTerrain()
{
	if(m_pkInput->VKIsDown("inrad+"))	m_fHMInRadius += 1 * m_pkFps->GetGameFrameTime();
	if(m_pkInput->VKIsDown("inrad-"))	m_fHMInRadius -= 1 * m_pkFps->GetGameFrameTime();
	if(m_pkInput->VKIsDown("outrad+"))	m_fHMOutRadius += 1 * m_pkFps->GetGameFrameTime();
	if(m_pkInput->VKIsDown("outrad-"))	m_fHMOutRadius -= 1 * m_pkFps->GetGameFrameTime();
	if(m_fHMInRadius > m_fHMOutRadius)
		m_fHMInRadius = m_fHMOutRadius;

		if(m_pkInput->VKIsDown("hmraise"))
			HMModifyCommand(5); 
		if(m_pkInput->VKIsDown("hmlower"))
			HMModifyCommand(-5); 

		if(m_pkInput->VKIsDown("hmpaint")) {
			int id = m_iCurrentMarkedZone;
			ZoneData* kZData = m_pkObjectMan->GetZoneData(id);
			P_HMRP2* hmrp = dynamic_cast<P_HMRP2*>(kZData->m_pkZone->GetProperty("P_HMRP2"));
			Vector3 kLocalOffset = m_kDrawPos - hmrp->m_pkHeightMap->m_kCornerPos;
			hmrp->m_pkHeightMap->DrawMask(m_kDrawPos, 1,m_fHMInRadius,255,255,255,255);
			}
}

// Handles input for EditMode Zones.
void MistServer::Input_EditZone()
{
	if(m_pkInput->Pressed(MOUSELEFT))
	{
		AddZone(m_kZoneMarkerPos, m_kZoneSize, m_strActiveZoneName);	
	}
	
	if(m_pkInput->Pressed(KEY_T))
	{
		AddZone(m_kZoneMarkerPos, m_kZoneSize, m_strActiveZoneName,true);	
	}

	if(m_pkInput->VKIsDown("remove"))
	{
		int id = m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
		
		m_pkObjectMan->DeleteZone(id);
	}
	
	if(m_pkInput->VKIsDown("rotate"))
	{
		if(m_pkFps->GetTicks() - m_fClickDelay > 0.2)
		{	
			
			m_fClickDelay = m_pkFps->GetTicks();						
			m_iCurrentMarkedZone = m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
			
			/*
			ZoneData* zd = pkObjectMan->GetZoneData(m_iCurrentMarkedZone);
			if(zd)
				if(zd->m_bUnderContruction)*/
			RotateActiveZoneObject();
		}
	}
	
	if(m_pkInput->VKIsDown("buildmodeon"))
	{
		if(m_pkFps->GetTicks() - m_fClickDelay > 0.2)
		{	
			m_fClickDelay = m_pkFps->GetTicks();
			m_iCurrentMarkedZone = m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
			m_pkObjectMan->SetUnderConstruction(m_iCurrentMarkedZone);
		}
	}
	
	if(m_pkInput->VKIsDown("buildmodeoff"))
	{
		if(m_pkFps->GetTicks() - m_fClickDelay > 0.2)
		{	
			m_fClickDelay = m_pkFps->GetTicks();
			m_iCurrentMarkedZone = m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
			m_pkObjectMan->CommitZone(m_iCurrentMarkedZone);
		}
	}	

	if(m_pkInput->VKIsDown("selectzone"))
	{		
		m_iCurrentMarkedZone =  m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
	}

	if(m_pkInput->Pressed(KEY_1)) m_kZoneSize.Set(4,4,4);
	if(m_pkInput->Pressed(KEY_2)) m_kZoneSize.Set(8,8,8);
	if(m_pkInput->Pressed(KEY_3)) m_kZoneSize.Set(16,16,16);	
	if(m_pkInput->Pressed(KEY_4)) m_kZoneSize.Set(32,16,32);	
	if(m_pkInput->Pressed(KEY_5)) m_kZoneSize.Set(64,16,64);		
	if(m_pkInput->Pressed(KEY_9)) m_bUpdateMarker = true;				
   if(m_pkInput->Pressed(KEY_0)) m_bUpdateMarker = false;

}

// Handles input for EditMode Object.
void MistServer::Input_EditObject()
{
	if(m_pkInput->Pressed(MOUSELEFT))
	{
		if(m_pkFps->GetTicks() - m_fClickDelay > 0.2)
		{	
			m_fClickDelay = m_pkFps->GetTicks();		
			m_pkObjectMan->CreateObjectFromScriptInZone(
				m_strActiveObjectName.c_str(), m_kObjectMarkerPos);

			cout << "Spawning " << m_strActiveObjectName.c_str() << endl;
		}
	}
	
	if(m_pkInput->Pressed(MOUSEMIDDLE) || (m_pkInput->Pressed(MOUSERIGHT) && m_pkInput->Pressed(KEY_LSHIFT)))
	{		
		if(m_pkFps->GetTicks() - m_fClickDelay > 0.2)
		{	
			m_fClickDelay = m_pkFps->GetTicks();		
			
			Entity* pkObj =  GetTargetObject();
		
			if(pkObj)
			{
				m_iCurrentObject = pkObj->iNetWorkID;
			}
		}
	}
	
	//remove			
	if(m_pkInput->VKIsDown("remove"))
	{
		
		Entity* pkObj = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);
										
		if(pkObj)
		{
			//fulhack deluxe för att inte kunna ta bort statiska entitys i zoner som inte är underconstruction
		/*	if(pkObj->GetParent()->GetName() == "StaticEntity")
			{
				cout<<"zone is not under construction "<<endl;
				return;
			}
		*/						
			cout<<"Deleting ID:"<<pkObj->iNetWorkID<<" Name:"<<pkObj->GetName()<<" Type:"<<pkObj->GetType()<<endl;
			
			m_pkObjectMan->Delete(pkObj);				
		}

	
		m_iCurrentObject = -1;
	}

	Entity* pkObj = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);								
	if(!pkObj)
		return;		

	//return if its a static object
	if(pkObj->GetParent()->GetName() == "StaticEntity")
		return;
		
		
	//hack for collisions test
	if(m_pkInput->VKIsDown("setvel"))		pkObj->SetVel(Vector3(1,0,0));

	// Move Selected Entity
	Vector3 kMove(0,0,0);
	kMove.Set(0,0,0);
	if(m_pkInput->VKIsDown("moveleft"))		kMove += Vector3(-1 * m_pkFps->GetFrameTime(),0,0);			
	if(m_pkInput->VKIsDown("moveright"))	kMove += Vector3(1 * m_pkFps->GetFrameTime(),0,0);			
	if(m_pkInput->VKIsDown("movefrw"))		kMove += Vector3(0,0,-1 * m_pkFps->GetFrameTime());			
	if(m_pkInput->VKIsDown("moveback"))		kMove += Vector3(0,0,1 * m_pkFps->GetFrameTime());			
	if(m_pkInput->VKIsDown("moveup"))		kMove += Vector3(0,1 * m_pkFps->GetFrameTime(),0);			
	if(m_pkInput->VKIsDown("movedown"))		kMove += Vector3(0,-1 * m_pkFps->GetFrameTime(),0);

	pkObj->SetLocalPosV(pkObj->GetLocalPosV() + kMove);

	// Rotate Selected Entity
	if(m_pkInput->VKIsDown("rotx+"))			pkObj->RotateLocalRotV(Vector3(100*m_pkFps->GetFrameTime(),0,0));			
	if(m_pkInput->VKIsDown("rotx-"))			pkObj->RotateLocalRotV(Vector3(-100*m_pkFps->GetFrameTime(),0,0));			
	if(m_pkInput->VKIsDown("roty+"))			pkObj->RotateLocalRotV(Vector3(0,100*m_pkFps->GetFrameTime(),0));			
	if(m_pkInput->VKIsDown("roty-"))			pkObj->RotateLocalRotV(Vector3(0,-100*m_pkFps->GetFrameTime(),0));			
	if(m_pkInput->VKIsDown("rotz+"))			pkObj->RotateLocalRotV(Vector3(0,0,100*m_pkFps->GetFrameTime()));			
	if(m_pkInput->VKIsDown("rotz-"))			pkObj->RotateLocalRotV(Vector3(0,0,-100*m_pkFps->GetFrameTime()));			

}

void MistServer::Input_Camera(float fMouseX, float fMouseY)
{
	if(m_pkInput->VKIsDown("slow")){
		m_CamMoveSpeed *= 0.25;
	}

	float fSpeedScale = m_pkFps->GetFrameTime()*m_CamMoveSpeed;

	Vector3 newpos = m_pkCameraObject->GetLocalPosV();
	
	Matrix4 kRm;
	kRm = m_pkCameraObject->GetLocalRotM();

	kRm.Transponse();

	
	Vector3 xv = kRm.GetAxis(0);
	Vector3 zv = kRm.GetAxis(2);

	xv.y = 0;
	zv.y = 0;
	
	xv.Normalize();
	zv.Normalize();

	if(m_pkInput->VKIsDown("right"))		newpos += xv * fSpeedScale;		
	if(m_pkInput->VKIsDown("left"))		newpos += xv * -fSpeedScale;		
	if(m_pkInput->VKIsDown("forward"))	newpos += zv * -fSpeedScale;
	if(m_pkInput->VKIsDown("back"))		newpos += zv * fSpeedScale;	

	if(m_pkInput->VKIsDown("down"))		newpos.y += fSpeedScale;
	if(m_pkInput->VKIsDown("up"))			newpos.y -= fSpeedScale;
			

	Vector3 rot;
	rot.Set(float(-fMouseY / 5.0),float(-fMouseX / 5.0),0);

	kRm.Transponse();		
	kRm.Rotate(rot);
	kRm.Transponse();		
	Vector3 bla = Vector3(0,0,1);
	bla = kRm.VectorTransform(bla);
	kRm.LookDir(bla,Vector3(0,1,0));
	
	m_pkCameraObject->SetLocalPosV(newpos);		
	if(m_pkInput->VKIsDown("pancam"))
		m_pkCameraObject->SetLocalRotM(kRm);	
}


void MistServer::Input()
{
	// *** HANDE Quueued Keys.
	int iPressedKey = m_pkInput->GetQueuedKey();

	switch(iPressedKey)
	{
	case KEY_F9:
	/*	printf("Num sounds in system = %i\nNum active channels = %i\n",
			pkAudioSys->GetNumSounds(), pkAudioSys->GetNumActiveChannels());*/

		//pkGui->ShowMainWindow(GetWnd("MainMenu"),false); 
		pkGui->SetFocus(GetWnd("MainWnd")); 
		break;
	}

	//set speed depending on edit mode
	if(m_iEditMode == EDIT_HMAP)		m_CamMoveSpeed = 20;
	if(m_iEditMode == EDIT_ZONES)		m_CamMoveSpeed = 20;
	if(m_iEditMode == EDIT_OBJECTS)	m_CamMoveSpeed = 5;
	
	
	int x,z;		
	m_pkInput->RelMouseXY(x,z);	

	if(m_pkInput->VKIsDown("makeland")) {
		int id = m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
		ZoneData* z = m_pkObjectMan->GetZoneData(id);
		m_pkFps->AddHMProperty(z, z->m_iZoneObjectID,z->m_kSize);
	}  

	Vector3 kMove(0,0,0);
//	Vector3 kRotate(0,0,0);

	if(m_pkCameraObject)	
	{	
		Input_Camera(x,z);
	
		if(m_pkInput->VKIsDown("modezone"))			m_iEditMode = EDIT_ZONES;
		if(m_pkInput->VKIsDown("modeobj"))			m_iEditMode = EDIT_OBJECTS;		
		if(m_pkInput->VKIsDown("modehmvertex"))	m_iEditMode = EDIT_HMAP;		

		if(m_pkInput->VKIsDown("lighton"))			m_pkZShader->SetForceLighting(LIGHT_ALWAYS_ON);	
		if(m_pkInput->VKIsDown("lightoff"))			m_pkZShader->SetForceLighting(LIGHT_ALWAYS_OFF);
		if(m_pkInput->VKIsDown("lightstd"))			m_pkZShader->SetForceLighting(LIGHT_MATERIAL);
	
		if(m_iEditMode == EDIT_HMAP)					Input_EditTerrain();
		if(m_iEditMode == EDIT_ZONES)					Input_EditZone();
		if(m_iEditMode == EDIT_OBJECTS)				Input_EditObject();
	}
};

void MistServer::OnHud(void)
{
	m_pkFps->DevPrintf("common","Active Propertys: %d",m_pkObjectMan->GetActivePropertys());	
	m_pkFps->DevPrintf("common", "Fps: %f",m_pkFps->m_fFps);	
	m_pkFps->DevPrintf("common","Avrage Fps: %f",m_pkFps->m_fAvrageFps);			
		
	m_pkFps->m_bGuiMode = false;
	m_pkFps->ToggleGui();


}

void MistServer::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	ClientOrder kOrder;

	unsigned int i;
	vector<string>	kUsers;
	int iMode;

	switch(cmdid) {
		case FID_NEW:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				m_pkConsole->Printf("new [mapdir]");
				break;				
			}
			
			m_pkObjectMan->SetWorldDir(kCommand->m_kSplitCommand[1].c_str());
			m_pkObjectMan->Clear();
			
			GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);
			break;
		
		case FID_LOAD:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				m_pkConsole->Printf("load [mapdir]");
				break;				
			}
			
			cout<<"loading world:"<<kCommand->m_kSplitCommand[1].c_str()<<endl;
			
			if(!m_pkObjectMan->LoadWorld(kCommand->m_kSplitCommand[1].c_str()))
			{
				cout<<"Error loading world"<<endl;
				break;
			}
						
			cout<<"starting server"<<endl;
			GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);			
			
			break;		
		
		case FID_SAVE:
			
			cout<<"saving world:"<<endl;
			
			m_pkObjectMan->ForceSave();
			m_pkObjectMan->SaveZones();			
			
			cout<<"saved"<<endl;
			
			break;		
	
		case FID_USERS:
			kUsers = m_pkPlayerDB->GetUsers();
			for(i=0; i<kUsers.size(); i++) {
				cout << "User: " << kUsers[i] << endl;
				}
			break;		

		
		case FID_LIGHTMODE:
			if(kCommand->m_kSplitCommand.size() <= 1)
				break;

			iMode = atoi(kCommand->m_kSplitCommand[1].c_str());
			if(iMode == 0)	m_pkZShader->SetForceLighting(LIGHT_ALWAYS_ON);		
			if(iMode == 1)	m_pkZShader->SetForceLighting(LIGHT_ALWAYS_OFF);		
			if(iMode == 2)	m_pkZShader->SetForceLighting(LIGHT_MATERIAL);		
			break;


		case FID_LOCALORDER:
			string strSo;
			strSo = kCommand->m_strFullCommand;
			strSo.erase(0, kCommand->m_kSplitCommand[0].length() + 1);
			//SendOrder( strSo );	
			//m_pkConsole->Printf("SO is = %s", strSo.c_str());

			//char szFullCmd[1024];
			//sprintf(szFullCmd, "addzone %f %f %f %f %f %f %s",m_kZoneMarkerPos.x,m_kZoneMarkerPos.y,m_kZoneMarkerPos.z,
			//	m_kZoneSize.x,m_kZoneSize.y,m_kZoneSize.z, m_strActiveZoneName.c_str());
			//cout << "FullCmd " << szFullCmd << endl;
			
			P_ClientControl pkClient;
			string strOrder;
			kOrder.m_sOrderName = strSo;
			kOrder.m_iCharacter = -1;
			cout << "Sending LocalOrder: " << kOrder.m_sOrderName << "\n";
			pkClient.AddServerOrder(kOrder);
			break;		
	}

}

void MistServer::ClientInit()
{
	cout<<"Client Join granted"<<endl;
	
	cout<<"Join Complete"<<endl;
}

bool MistServer::OnPreConnect(IPaddress kRemoteIp, char* szLogin, char* szPass)
{
	//dessa skall du fixa till vim =D
	string strPlayer		= szLogin;
	string strPasswd		= szPass;
	string strCharacter	= "mrbad";
	
	// Check that this is a valid login.
	if(m_pkPlayerDB->LoginExist(strPlayer)) {
		// Check Password
		if(!m_pkPlayerDB->Login(strPlayer,strPasswd)) {
			return false;
			}
		// Login Ok.

		}
	else {
		/* Failed to find the login. */
		cout << "Login not found" << endl;

		if(m_AcceptNewLogins) {
			if(!m_pkPlayerDB->CreatePlayer(strPlayer,strPasswd)) {
				return false;
				}
	
			m_pkPlayerDB->Login(strPlayer,strPasswd);
			}
		else 
			return false;
		}


	if(m_pkServerInfoP)
	{
		if(m_pkServerInfoP->PlayerExist(strPlayer))
		{
			cout<<"Player "<<strPlayer<< " is already connected"<<endl;
		
			//KICKA O DÖDA SPELARJÄVELN HÄR OCKSÅ FÖR HAN FÅR FAN INTE CONNECTA TVÅ GÅNGER!!!!
		
			return false;
		}
	}
	
	printf("User %s tried to join with password %s\n", szLogin, szPass);
	return true;
}

void MistServer::OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass)
{
	//dessa skall du fixa till vim =D
	string strPlayer		= szLogin;
	string strPasswd		= szPass;

	pkClient->m_strCharacter = "MrBadq";
	pkClient->m_strLogin = szLogin;

	//add client control to client object
	P_ClientControl* pcc = (P_ClientControl*)pkClient->m_pkObject->AddProperty("P_ClientControl");
	if(pcc)	
		pcc->m_iClientID = iConID;
	
	bool bEditorConnect = false;
	if(bEditorConnect) {
		P_Track* pkTrack = dynamic_cast<P_Track*>((P_ClientControl*)pkClient->m_pkObject->AddProperty("P_Track"));
		pkTrack->SetClient(iConID);
		(P_ClientControl*)pkClient->m_pkObject->AddProperty("P_Primitives3D");
		return;
		}
	else
		SpawnPlayer(iConID);
}

void MistServer::SpawnPlayer(int iConID)
{
	if(m_pkFps->m_kClient[iConID].m_strCharacter.size() == 0) {
		m_pkFps->PrintToClient(iConID, "You must select a character before you can join" );
		return;
		}

	//update start locations  
	UpdateStartLocatons();

	//create player object
	int iPlayerID  = CreatePlayer(m_pkFps->m_kClient[iConID].m_strLogin.c_str(),m_pkFps->m_kClient[iConID].m_strCharacter.c_str(),"Start",iConID);
	
	if(iPlayerID == -1)
	{
		cout<<"Error creating playercharacter"<<endl;
	}
	
	if(m_pkServerInfoP)
	{	
		//wich rights shuld a client have on its player caracter
		//int playerrights = PR_OWNER|PR_CONTROLS|PR_LOOKAT;
		
		m_pkServerInfoP->AddPlayer(iConID, m_pkFps->m_kClient[iConID].m_strLogin.c_str());
		m_pkServerInfoP->SetCharacterID(iConID,iPlayerID);
		//m_pkServerInfoP->AddObject(iConID,iPlayerID,playerrights);
	}
}



void MistServer::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	DeletePlayer(iConID);

	if(m_pkServerInfoP)
		m_pkServerInfoP->RemovePlayer(iConID);	
	
	m_pkPlayerDB->Logout(pkClient->m_strLogin);
	cout<<"Client "<<iConID<<" Parted"<<endl;	
}


void MistServer::OnServerStart(void)
{		
	//create a camera for the server
	m_pkCameraObject = m_pkObjectMan->CreateObjectFromScript("data/script/objects/t_camera.lua");
	if(m_pkCameraObject)
	{	
		m_pkCameraObject->SetParent(m_pkObjectMan->GetWorldObject());
		P_Camera* m_pkCamProp = (P_Camera*)m_pkCameraObject->GetProperty("P_Camera");
		m_pkCamProp->SetCamera(m_pkCamera);
		m_pkCameraObject->GetSave() = false;
		
		//eye candy for server
		//P_SkyBoxRender* sb = (P_SkyBoxRender*)m_pkCameraObject->AddProperty("P_SkyBoxRender");
		//sb->SetTexture("data/textures/env/plainsky/sky","mode6");
		//sb->SetTexture("data/textures/env/sky1.bmp","data/textures/env/sky2.bmp");
		
		P_Enviroment* pe = (P_Enviroment*)m_pkCameraObject->AddProperty("P_Enviroment");
		pe->SetEnable(true);		
		pe->SetEnviroment("data/enviroments/server.env");
	}
	
	//create server info object
	m_pkServerInfo = m_pkObjectMan->CreateObjectFromScript("data/script/objects/t_serverinfo.lua");
	if(m_pkServerInfo)
	{
		m_pkServerInfo->SetParent(m_pkObjectMan->GetGlobalObject());
		m_pkServerInfoP = (P_ServerInfo*)m_pkServerInfo->GetProperty("P_ServerInfo");		
		if(m_pkServerInfoP)
		{
			m_pkServerInfoP->SetServerName("Test Server");
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
		// Zeb was here! Nu kör vi med operativsystemets egna snabba musmarkör
		// alltså måste vi använda den position vi får därifrån.
		//	m_pkInput->UnitMouseXY(x,y);
		x = -0.5f + (float) m_pkInput->m_iSDLMouseX / (float) m_pkApp->m_iWidth;
		y = -0.5f + (float) m_pkInput->m_iSDLMouseY / (float) m_pkApp->m_iHeight;

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
	Vector3 start = m_pkFps->GetCam()->GetPos();
	Vector3 dir = Get3DMousePos();

	vector<Entity*> kObjects;
	kObjects.clear();
	
	m_pkObjectMan->TestLine(&kObjects,start,dir);
	
	
	float closest = 999999999;
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


void MistServer::AddZone(Vector3 kPos, Vector3 kSize, string strName, bool bEmpty)
{
	if(m_pkObjectMan->IsInsideZone(kPos, kSize))
		return;
		
	int id = m_pkObjectMan->CreateZone(kPos, kSize);

	//force loading of this zone
	m_pkObjectMan->LoadZone(id);

	//set to active
	m_iCurrentMarkedZone = id;

	if(id != -1)
	{
		if(!bEmpty)
			m_pkObjectMan->SetZoneModel(strName.c_str(),id);
		//pkObjectMan->SetUnderConstruction(id);
	}	

	
	
	
	SetZoneEnviroment(m_strActiveEnviroment.c_str());
}


void MistServer::DrawZoneMarker(Vector3 kPos)
{
	Vector3 bla = m_kZoneSize / 2;
	m_pkRender->DrawAABB(kPos-bla,kPos+bla,Vector3(1,1,1));
}


void MistServer::DrawCrossMarker(Vector3 kPos)
{
	m_pkRender->Line(kPos-Vector3(1,0,0),kPos+Vector3(1,0,0));
	m_pkRender->Line(kPos-Vector3(0,1,0),kPos+Vector3(0,1,0));	
	m_pkRender->Line(kPos-Vector3(0,0,1),kPos+Vector3(0,0,1));	
}


void MistServer::UpdateZoneMarkerPos()
{
	Vector3 temp = m_pkFps->GetCam()->GetPos() + Get3DMousePos(false)*15;

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
	m_kObjectMarkerPos = m_pkFps->GetCam()->GetPos() + Get3DMousePos(true)*2;
}


void MistServer::OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd *pkMainWnd)
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
				if( IsWndVisible("WorkTabWnd") )
				{
					m_pkAudioSys->StartSound("/data/sound/close_window.wav",m_pkAudioSys->GetListnerPos());
					GetWnd("WorkTabWnd")->Hide(); 
				}
				else 
				{
					m_pkAudioSys->StartSound("/data/sound/open_window.wav",m_pkAudioSys->GetListnerPos());
					GetWnd("WorkTabWnd")->Show(); 
				}
			}
			else
			if(strWndClicked == "ToogleLight")
			{
				ToogleLight( static_cast<ZGuiCheckbox*>(pkWndClicked)->IsChecked() );
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
				int id = m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
				m_pkObjectMan->DeleteZone(id);
			}
		}
		else
		if(strMainWnd == "ObjectPage")
		{
			if(strWndClicked == "DeleteObjectButton")
			{		
				Entity* pkObj = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);		
				if(pkObj) 
				{
					m_pkObjectMan->Delete(pkObj);
					m_iCurrentObject = -1;
				}
			}
			else
			if(strWndClicked == "PlaceongroundButton")
			{
				Entity* pkObj = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);		
				if(pkObj) 
				{
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
	

	ZGuiWnd* pkParent = pkMain->GetParent(); 

	if(pkParent)
	{
		string strParentName = pkParent->GetName();
		
		if(strParentName == "MainMenu")
		{
			char *szItem = static_cast<ZGuiListbox*>(pkListBox)->GetSelItem()->GetText();

			if(!m_pkIni->Open("data/script/gui/menu.txt", false))
			{
				cout << "Failed to load ini file for menu!\n" << endl;
				return;
			}

			vector<string> akSections;
			m_pkIni->GetSectionNames(akSections);

			// Run Menu command
			for(unsigned int i=0; i<akSections.size(); i++)
			{
				char* title = m_pkIni->GetValue(akSections[i].c_str(), "Title");

				if(strcmp(title, szItem) == 0)
				{
					char* cmd = m_pkIni->GetValue(akSections[i].c_str(), "Cmd");
					m_pkFps->m_pkConsole->Execute(cmd);
					break;
				}
			}

			m_pkIni->Close();
		}

	}
}

void MistServer::AutoSetZoneSize(string strName)
{
	int iPos = strName.find_last_of('-');
	if( iPos == string::npos )
		return;

	int x,y,z;
	char szString[256];
	strcpy(szString, &strName.c_str()[iPos + 1]);
	sscanf(szString,"%dx%dx%d", &x,&y,&z);
	m_kZoneSize.Set(float(x),float(y),float(z));
//	cout << "Setting Size " << x << ", " << y << ", "<< z << endl;
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

			m_iCurrentMarkedZone = m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
			// Setting new zone modell
			if(m_iCurrentMarkedZone != -1)	// ÄR någon zon markerad?
			{
				//pkObjectMan->LoadZone(m_iCurrentMarkedZone);
				m_pkObjectMan->SetZoneModel(strFullpath.c_str(),m_iCurrentMarkedZone);
				printf("Setting new zone modell to %s\n", strFullpath.c_str());
			}

			AutoSetZoneSize(m_strActiveZoneName);
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
		ZoneData* pkData = m_pkObjectMan->GetZoneData(m_iCurrentMarkedZone);
		if(pkData) 
		{
			if(pkData->m_pkZone)
			{
				pkData->m_pkZone->RotateLocalRotV( Vector3(0,90.0f,0) ); 
		
				// Update PFind Mesh
				P_PfMesh* pkMesh = (P_PfMesh*)pkData->m_pkZone->GetProperty("P_PfMesh");
				if(pkMesh)
					pkMesh->CalcNaviMesh();
			}		
		}
		
	}
}

void MistServer::ToogleLight(bool bEnabled)
{
	if(bEnabled)
		m_pkZShader->SetForceLighting(LIGHT_ALWAYS_ON);
	else
		m_pkZShader->SetForceLighting(LIGHT_ALWAYS_OFF);
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
			return m_kLocations[i].second+Vector3( float((rand()%1000)/1000.0),  0,  float((rand()%1000)/1000.0));
		}
	
	return Vector3(0,0,0);
}

int MistServer::CreatePlayer(const char* csPlayer,const char* csCharacter,const char* csLocation,int iConID)
{

	//try to create character entity
	Entity* pkObject = m_pkPlayerDB->CreateCharacter(csPlayer,csCharacter);
	
	
	//if it fails try to create it
	if(!pkObject)
	{
		cout<<"Character not found, trying to create it"<<endl;
		if(!m_pkPlayerDB->CreateNewCharacter(csPlayer,csCharacter))
			return -1;
		else	//it was created, now load it
			pkObject = m_pkPlayerDB->CreateCharacter(csPlayer,csCharacter);
	}
	
	
	if(pkObject)
	{	
		Vector3 kStartPos = Vector3(0,0,0);
				
		//try to get recal position from characterstats
		CharacterProperty* pkCP = (CharacterProperty*)pkObject->GetProperty("P_CharStats");
      if(pkCP)
  	   {
   	   CharacterStats *pkCS = pkCP->GetCharStats();	
			kStartPos = pkCS->GetRecalPos();		
		}	
		
		//make sure position is valid and zone is loaded
		int zid = m_pkObjectMan->GetZoneIndex(kStartPos,-1,false);
		if(zid == -1)
		{
			cout<<"Error Character "<<csPlayer<<" -> "<<csCharacter<<" Tryed to start in a invalid location,trying 0,1,0"<<endl;
			kStartPos = Vector3(0,0,0);
			zid = m_pkObjectMan->GetZoneIndex(kStartPos,-1,false);						
		}		
		
		
		//force loading of zone
		m_pkObjectMan->LoadZone(zid);
		
		//finaly set objects position
		pkObject->SetWorldPosV(kStartPos);
		
		cout<<"setting pos:"<<kStartPos.x<<" "<<kStartPos.y<<" "<<kStartPos.z<<endl;
		
		//setup tracker to correct tracker id
		P_Track* pkTrack = dynamic_cast<P_Track*>(pkObject->GetProperty("P_Track"));	
		if(pkTrack)
			pkTrack->SetClient(iConID);	
	}
	else
	{	
		cout<<"Error creating caracter entity"<<endl;
		return -1;
	}
			
	cout<<"created character entity "<<csPlayer<<" -> "<<csCharacter<<endl;
			
	return pkObject->iNetWorkID;
}

void MistServer::DeletePlayer(int iConID)
{
	if(m_pkServerInfoP)
	{
		PlayerInfo* pi = m_pkServerInfoP->GetPlayerInfo(iConID);
		if(pi)
		{
			//first save and delete the player character
			Entity* pkObj = m_pkObjectMan->GetObjectByNetWorkID(pi->iCharacterObjectID);
			if(pkObj)
			{
				m_pkPlayerDB->SaveCharacter(pkObj,pi->sPlayerName);
				m_pkObjectMan->Delete(pkObj);
			}
		
			//then walk trough all characters in his control list and delete the ones the player have spawned
			for(unsigned int i = 0;i<pi->kControl.size();i++)
			{
				if(pi->kControl[i].second & PR_OWNER)
				{
					Entity* pkObj = m_pkObjectMan->GetObjectByNetWorkID(pi->kControl[i].first);
					
										
					//delete it
					if(pkObj)
						m_pkObjectMan->Delete(pkObj);
				}
			}		
		}
	}
}

void MistServer::PathTest() 
{
	return;

	int iNumOfZones = m_pkObjectMan->GetNumOfZones();
	if(iNumOfZones < 10)
		return; 

	int iRuns = 10;

	for(int i=0; i<iRuns; i++) {

		int iStartZone  = 10;
		int iEndZone	= 1;

		kPathStart = m_pkObjectMan->GetZoneCenter(iStartZone);
		kPathEnd   = m_pkObjectMan->GetZoneCenter(iEndZone);

//		bool bres = m_pkAStar->GetPath(kPathStart,kPathEnd,kPath);
		}
}

void MistServer::HSO_Edit(ClientOrder* pkOrder)
{
	CmdArgument kcmdargs;
	kcmdargs.Set(pkOrder->m_sOrderName.c_str());

	char szCmdNone[256];
	char szCmd1[256];
	char szCmd2[256];
	Vector3 kPos;
	Vector3 kSize;

	if ( kcmdargs.m_kSplitCommand[1] == string("addzone") )
	{
		cout << "Player: " << int(pkOrder->m_iConnectID) << " wish to addzone." << endl;
		cout << "m_sOrderName: " << pkOrder->m_sOrderName << endl;
		if(sscanf(pkOrder->m_sOrderName.c_str(), "%s %s %f %f %f %f %f %f", szCmd1,szCmd2, &kPos.x,&kPos.y,&kPos.z,
			&kSize.x,&kSize.y,&kSize.z) == 8) {

			AddZone(kPos, kSize, m_strActiveZoneName);	
			}
	}
	else if ( kcmdargs.m_kSplitCommand[1] == string("delzone") )
	{
		cout << "Player: " << int(pkOrder->m_iConnectID) << " wish to remove a zone." << endl;
	}
	else if ( kcmdargs.m_kSplitCommand[1] == string("spawn") )
	{
		cout << "Player: " << int(pkOrder->m_iConnectID) << " wish to create something" << endl;
		if(sscanf(pkOrder->m_sOrderName.c_str(), "%s %s %s", szCmdNone,szCmdNone, szCmd1) == 3) {
			string strObjName = string("data/script/objects/") + string(szCmd1);
			m_pkObjectMan->CreateObjectFromScriptInZone( strObjName.c_str(), m_kObjectMarkerPos);
			}

	}

	// ROTATE:	Rotera en zon.
	// DELETE:	Radera en zon.	By Index, By Pos, By NetWorkID
	// Spawn:   Skapa ett object
}

void MistServer::HSO_Character(ClientOrder* pkOrder)
{
	CmdArgument kcmdargs;
	kcmdargs.Set(pkOrder->m_sOrderName.c_str());
	
	cout << "kcmdargs.m_kSplitCommand[1] " << kcmdargs.m_kSplitCommand[1].c_str(); 

	if ( kcmdargs.m_kSplitCommand[1] == string("Play") )
	{
		cout << "Player: " << pkOrder->m_iConnectID << " wish to start play" << endl;
		SpawnPlayer(pkOrder->m_iConnectID);
	}
	else if ( kcmdargs.m_kSplitCommand[1] == string("CharList") )
	{
		cout << "Player: " << int(pkOrder->m_iConnectID) << " wish to know what char he have." << endl;
		vector<string> kChars;
		kChars = m_pkPlayerDB->GetLoginCharacters( m_pkFps->m_kClient[pkOrder->m_iConnectID].m_strLogin.c_str() );
		for(unsigned int i=0; i<kChars.size(); i++)
			m_pkFps->PrintToClient(pkOrder->m_iConnectID, kChars[i].c_str());
	}
	else if ( kcmdargs.m_kSplitCommand[1] == string("Select") )
	{
		cout << "Player: " << int(pkOrder->m_iConnectID) << " wish to use char '" << kcmdargs.m_kSplitCommand[2].c_str()  << "'" << endl;
		m_pkFps->m_kClient[pkOrder->m_iConnectID].m_strCharacter = kcmdargs.m_kSplitCommand[2].c_str();
	}

	// WHO:		Kolla vilken karaktär man är.
	// LWHO:	Kolla vilket login man är.
}


/*
	Handle Server Orders, that is commands sent from clients to the server.
*/			
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
		
		cout << "handling order "<<order->m_sOrderName<<" from client:" << order->m_iClientID << endl;
		
		// Edit Order
		cout << order->m_sOrderName.c_str() << endl;

		if(strncmp(order->m_sOrderName.c_str(),"ED",2) == 0) {
			HSO_Edit(order);
			}
		
		// Character Command
		else if(strncmp(order->m_sOrderName.c_str(),"CC",2) == 0) {
			HSO_Character(order);
			}
		
		// OLD UNFIXED ORDERS BELOW.
		//handle input messages from client
		else if(strncmp(order->m_sOrderName.c_str(),"(IM)",4) == 0) 
		{
			order->m_sOrderName.erase(0,4);
			string playername=""; 
			string message="";
					
			unsigned int pos=0;
			
			if(strncmp(order->m_sOrderName.c_str(),"/w",2)==0)
			{
				for(pos=3;pos<order->m_sOrderName.size();pos++)
				{
					if(order->m_sOrderName[pos] == ' ' && playername.size()>0)
						break;
					playername+=order->m_sOrderName[pos];
				}
			}
			
			for(;pos<order->m_sOrderName.size();pos++)
			{	
				message+=order->m_sOrderName[pos];
			}
			 
			//cout<<"got message to "<<playername<<": "<<message<<endl;
			
			if(m_pkServerInfoP)
			{
				PlayerInfo* pi = m_pkServerInfoP->GetPlayerInfo(order->m_iClientID);
				
				if(pi)
				{
					message = pi->sPlayerName + " : " + message;
				
				}
				
				if(playername == "")
					m_pkServerInfoP->MessagePlayer(-1,message);
				else
					m_pkServerInfoP->MessagePlayer(playername.c_str(),message);
			}
		}
		else if(strncmp(order->m_sOrderName.c_str(),"G_",2)==0)
		{
			cout<<"Got ground click order"<<endl;
		
			Entity* ob = m_pkObjectMan->GetObjectByNetWorkID(order->m_iCharacter);			
		
			if(ob)
			{
				P_Event* pe = (P_Event*)ob->GetProperty("P_Event");
				if(pe)
					pe->SendGroudClickEvent(order->m_sOrderName.c_str(), order->m_kPos,order->m_iCharacter);
			}					
		}
      
      // equip
      else if ( order->m_sOrderName == "equip" )
      {
   		Entity* pkChar = m_pkObjectMan->GetObjectByNetWorkID(order->m_iCharacter);

         if ( pkChar )
         {
            // get item to equip
            Entity* pkItem = m_pkObjectMan->GetObjectByNetWorkID(order->m_iObjectID);

            CharacterProperty* pkCP = (CharacterProperty*)pkChar->GetProperty ("P_CharStats");
            P_Item* pkIP = (P_Item*)pkItem->GetProperty("P_Item");

            if ( pkIP && pkCP )
               pkCP->GetCharStats()->Equip( pkItem, order->m_iUseLess );              
         }
      }

	  // request orders
      else if ( order->m_sOrderName == "(rq)item" )    
      {
   		Entity* pkItemObject = m_pkObjectMan->GetObjectByNetWorkID(order->m_iObjectID);

         if ( pkItemObject )
         {
            P_Item *pkItProp = (P_Item*) pkItemObject->GetProperty("P_Item");
            
            if ( pkItProp )
            {
               // if the items is of the same version, no need to send data
                 if ( pkItProp->m_pkItemStats->m_uiVersion != order->m_iUseLess )				//DVOID WAS HERE
                 {
                     SendType kSendType;
                     kSendType.m_iClientID = order->m_iClientID;
                     kSendType.m_kSendType = "itemdata";
                  
                     pkItProp->AddSendsData ( kSendType );
                 }

            }
            else
               cout << "Error! Non-P_Item_Object requested for updated iteminfo! This should't be possible!!!" << endl;
         }
      }
      
      // container request
      else if ( order->m_sOrderName == "(rq)cont" )
      {
         cout << "Sever hgot cont req" << endl;

   		Entity* pkObject = m_pkObjectMan->GetObjectByNetWorkID(order->m_iObjectID);
         
         if ( pkObject )
         {

            P_Container* pkC = (P_Container*) pkObject->GetProperty("P_Container");

            if ( pkC )
            {
               // check versions...
               //if ( pkC->m_uiVersion != order->m_iUseLess )
                  pkC->AddSendsData(order->m_iClientID);
            }            
            else
               cout << "Error! Non-P_Container requested for updated containerinfo!" << endl;
         }
      }
      // request character skills
      else if ( order->m_sOrderName == "(rq)skil" )
      {
           // type of request
   		Entity* pkCharObject = m_pkObjectMan->GetObjectByNetWorkID(order->m_iObjectID);

         if ( pkCharObject  )
         {
            CharacterProperty *pkCP = (CharacterProperty*) pkCharObject ->GetProperty("P_CharStats");
            
            if ( pkCP )
            {
               // if the items is of the same version, no need to send data
                 //if ( pkCP->GetCharStats()->m_uiVersion != order->m_iUseLess )
                 //{
                     SendType kSendType;
                     kSendType.m_iClientID = order->m_iClientID;
                     kSendType.m_kSendType = "skills";
                  
                     pkCP->AddSendsData ( kSendType );
                 //}

            }
            else
               cout << "Error! Non-P_Charstats_Object requested for updated iteminfo! This should't be possible!!!" << endl;
         }
      } 
      // request character data
      else if ( order->m_sOrderName == "(rq)cdat" )
      {
           // type of request
   		Entity* pkCharObject = m_pkObjectMan->GetObjectByNetWorkID(order->m_iObjectID);

         if ( pkCharObject  )
         {
            CharacterProperty *pkCP = (CharacterProperty*) pkCharObject ->GetProperty("P_CharStats");
            
            if ( pkCP )
            {
               // if the items is of the same version, no need to send data
               //  if ( pkCP->GetCharStats()->m_uiVersion != order->m_iUseLess )
               //  {
                     SendType kSendType;
                     kSendType.m_iClientID = order->m_iClientID;
                     kSendType.m_kSendType = "data";
                  
                     pkCP->AddSendsData ( kSendType );
               //  }

            }
            else
               cout << "Error! Non-P_Charstats_Object requested for updated iteminfo! This should't be possible!!!" << endl;
         }
      }  
      // request character skills
      else if ( order->m_sOrderName == "(rq)attr" )
      {
           // type of request
   		Entity* pkCharObject = m_pkObjectMan->GetObjectByNetWorkID(order->m_iObjectID);

         if ( pkCharObject  )
         {
            CharacterProperty *pkCP = (CharacterProperty*) pkCharObject ->GetProperty("P_CharStats");
            
            if ( pkCP )
            {
               // if the items is of the same version, no need to send data
                 //if ( pkCP->GetCharStats()->m_uiVersion != order->m_iUseLess )				//DVOID WAS HERE
                 //{
                     SendType kSendType;
                     kSendType.m_iClientID = order->m_iClientID;
                     kSendType.m_kSendType = "attributes";
                  
                     pkCP->AddSendsData ( kSendType );
                 //}

            }
            else
               cout << "Error! Non-P_Charstats_Object requested for updated iteminfo! This should't be possible!!!" << endl;
         }
      }  


      // drop item from inventory to ground
      else if ( order->m_sOrderName == "DropItem" )
      {
         Entity* pkEntity = m_pkObjectMan->GetObjectByNetWorkID(order->m_iObjectID);
         Entity* pkPlayer = m_pkObjectMan->GetObjectByNetWorkID(order->m_iCharacter);

         if ( pkEntity && pkPlayer )
         {
            pkEntity->SetUpdateStatus (UPDATE_ALL);
            pkEntity->GetParent()->RemoveChild (pkEntity);

            pkEntity->SetWorldPosV ( pkPlayer->GetWorldPosV() );

            ((P_Container*)pkPlayer->GetProperty("P_Container"))->RemoveObject(order->m_iObjectID);         
         }

         if ( !pkEntity )
            cout << "Error! Client wanted to drop a non-existing item!" << endl;
      }  
		
		//normal orders
		else if(order->m_iObjectID != -1)
		{
			Entity* ob = m_pkObjectMan->GetObjectByNetWorkID(order->m_iObjectID);
			if(ob)
			{
			
				P_Event* pe = (P_Event*)ob->GetProperty("P_Event");
				if(pe)
				{	
					pe->SendObjectClickEvent(order->m_sOrderName.c_str(), order->m_iCharacter);				
				
				}			
			}
		}
		else
		{
			cout<<"CLICK"<<endl;

		
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
		if(pkOrder->m_iCharacter  == m_pkServerInfoP->GetCharacterID(pkOrder->m_iClientID))
			return true;
		
/*	
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
*/		
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
		m_pkZFVFileSystem->ListDir(&t, currentFolder);
		for(unsigned int i=0; i<t.size(); i++)
			vkFileNames.push_back(t[i]); 
		t.clear(); vkFileNames.sort(SortFiles);

		// Lägg till alla filer
		for(list<string>::iterator it = vkFileNames.begin(); it != vkFileNames.end(); it++)  
		{
			string strLabel = (*it);
			string id = currentFolder + string("/") + strLabel;

			bool bIsFolder = strLabel.find(".") == string::npos;

			if(kSearchedFiles.find(id) == kSearchedFiles.end())
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

				kSearchedFiles.insert(id);
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

	pkGui->GetToolTip()->SetToolTip(GetWnd("RotateZoneModellButton"),"Rotate");

	return true;

}

void MistServer::SendTextToMistClientInfoBox(char *szText)
{
	  
}


void MistServer::SetZoneEnviroment(const char* csEnviroment)
{
	//set default enviroment
	m_strActiveEnviroment=csEnviroment;
	
	m_iCurrentMarkedZone = m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
	ZoneData* z = m_pkObjectMan->GetZoneData(m_iCurrentMarkedZone);
		
	if(z)
	{
		//cout<<"Setting enviroment to :"<<csEnviroment<<endl;
		z->m_strEnviroment = csEnviroment;
	}	
}

string MistServer::GetZoneEnviroment()
{
	string env;
	
	m_iCurrentMarkedZone = m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
	ZoneData* z = m_pkObjectMan->GetZoneData(m_iCurrentMarkedZone);
		
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

	CreateWnd(Wnd, "MainMenu", "", "", 0,0, 800, 20, 0);
	ChangeSkin(m_pkScript, "MainMenu", "NullSkin", "Window");

	if(!m_pkIni->Open(szFileName, false))
	{
		cout << "Failed to load ini file for menu!\n" << endl;
		return false;
	}

	vector<string> akSections;
	m_pkIni->GetSectionNames(akSections);

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
		char* parent = m_pkIni->GetValue(akSections[i].c_str(), "Parent");
		if(parent == NULL)
			continue;

		if(strcmp(parent, "NULL") == 0)
		{
			char szTitle[50];
			sprintf(szTitle, " %s", m_pkIni->GetValue(akSections[i].c_str(), "Title"));
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
			pkMenuCBox->SetSkin(new ZGuiSkin());
			
			iMenuOffset += iMenuWidth;
			rcMenu = rcMenu.Move(iMenuOffset,0);
		}
	}

	ZGuiWnd* pkParent;
	vector<MENU_INFO> kTempVector;

	// Skapa alla childrens.
	char szCommando[512];
	int item_counter = 0;

	char szPrevParent[150];
	strcpy(szPrevParent, "");

	for(i=0; i<uiNumSections; i++)
	{
		char* parent = m_pkIni->GetValue(akSections[i].c_str(), "Parent");
		if(parent == NULL)
			continue;

		if(strcmp(szPrevParent, parent) != 0)
			item_counter = 0;

		strcpy(szParentName, parent);
		if(strcmp(szParentName, "NULL") != 0)
		{
			pkParent = GetWnd(szParentName);

			if(pkParent != NULL)
			{
				char szTitle[50];
				sprintf(szTitle, "%s", m_pkIni->GetValue(akSections[i].c_str(), "Title"));
				((ZGuiCombobox*) pkParent)->AddItem(szTitle, item_counter++);

				MENU_INFO mi;
				mi.cb = (ZGuiCombobox*) pkParent;
				mi.iIndex = item_counter-1;
				char* szCmd = m_pkIni->GetValue(akSections[i].c_str(), "Cmd");
				if(szCmd != NULL)
					strcpy(szCommando, szCmd);
				else
					strcpy(szCommando, "No commando!");

				mi.szCommando = new char[strlen(szCommando)+1];
				strcpy(mi.szCommando, szCommando);
				kTempVector.push_back(mi);
			}
		}

		strcpy(szPrevParent, parent);
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

void MistServer::CreateGuiInterface()
{
	int w = 800, h = 600;

	ZGuiWnd* pkWnd;
	
	pkWnd = CreateWnd(Wnd, "MainWnd", "", "", 0, 0, w, h, 0);
	ChangeSkin(m_pkScript, "MainWnd", "NullSkin", "Window"); 

	pkWnd = CreateWnd(Button, "OpenWorkTabButton", "MainWnd", "", w-40,h-40,32,32,0);
	ChangeSkin(m_pkScript, "OpenWorkTabButton", "WorkButtonSkinUp", "Button up"); 
	ChangeSkin(m_pkScript, "OpenWorkTabButton", "WorkButtonSkinDown", "Button down"); 
	ChangeSkin(m_pkScript, "OpenWorkTabButton", "WorkButtonSkinFocus", "Button focus"); 

	pkWnd = CreateWnd(Checkbox, "ToogleLight", "MainWnd", "", w-80,h-40,32,32,0);
	ChangeSkin(m_pkScript, "ToogleLight", "ToogleLightButtonSkinUp", "Checkbox: Button up");
	ChangeSkin(m_pkScript, "ToogleLight", "ToogleLightButtonSkinDown", "Checkbox: Button down");

	// Create workwnd

	pkWnd = CreateWnd(TabControl, "WorkTabWnd", "MainWnd", "", w-10-256, h-50-256, 256, 256, 0);
	pkWnd->Hide();
	pkWnd->SetMoveArea(Rect(0,0,800,600),true);

	AddTabPage("WorkTabWnd", "ZonePage", "Zone");
	AddTabPage("WorkTabWnd", "ObjectPage", "Object");
	AddTabPage("WorkTabWnd", "EnviromentPage", "Enviroment");

	//
	// Page 1
	//
	CreateWnd(Button,"RotateZoneModellButton","ZonePage","",256-32,16,16,16,0);
	ChangeSkin(m_pkScript, "RotateZoneModellButton", "RotateButtonSkinUp", "Button up");
	ChangeSkin(m_pkScript, "RotateZoneModellButton", "RotateButtonSkinDown", "Button down");
	ChangeSkin(m_pkScript, "RotateZoneModellButton", "RotateButtonSkinFocus", "Button focus");

	CreateWnd(Button,"DeleteZoneButton","ZonePage","",256-32,36,16,16,0);
	ChangeSkin(m_pkScript, "DeleteZoneButton", "DeleteButtonSkinUp", "Button up");
	ChangeSkin(m_pkScript, "DeleteZoneButton", "DeleteButtonSkinDown", "Button down");
	ChangeSkin(m_pkScript, "DeleteZoneButton", "DeleteButtonSkinFocus", "Button focus");

	CreateWnd(Treebox, "ZoneModelTree", "ZonePage", "", 10,20,200,200,0);

	//
	// Page 2
	//
	
	CreateWnd(Treebox, "ObjectTree", "ObjectPage", "", 10,20,200,200,0);
	
	CreateWnd(Button,"PlaceongroundButton","ObjectPage","",256-32,16,16,16,0);
	ChangeSkin(m_pkScript, "PlaceongroundButton", "PlaceongroundButtonSkinUp", "Button up");
	ChangeSkin(m_pkScript, "PlaceongroundButton", "PlaceongroundButtonSkinDown", "Button down");
	ChangeSkin(m_pkScript, "PlaceongroundButton", "PlaceongroundButtonSkinFocus", "Button focus");

	CreateWnd(Button,"DeleteObjectButton","ObjectPage","",256-32,36,16,16,0);
	ChangeSkin(m_pkScript, "DeleteObjectButton", "DeleteButtonSkinUp", "Button up");
	ChangeSkin(m_pkScript, "DeleteObjectButton", "DeleteButtonSkinDown", "Button down");
	ChangeSkin(m_pkScript, "DeleteObjectButton", "DeleteButtonSkinFocus", "Button focus");

	//
	// Page 3
	//

	CreateWnd(Listbox, "EnviromentPresetList", "EnviromentPage", "", 10,20,200,200,0);

	BuildFileTree("ZoneModelTree", "data/mad/zones");
	BuildFileTree("ObjectTree", "data/script/objects");

	vector<string> vkFileNames;
	m_pkZFVFileSystem->ListDir(&vkFileNames, "/data/enviroments", false);

	for(unsigned int i=0; i<vkFileNames.size(); i++)
		AddListItem("EnviromentPresetList", (char*) vkFileNames[i].c_str());

	// 
	// Create menu
	//
	CreateMenu("data/script/gui/menu.txt");

}
