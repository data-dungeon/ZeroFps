/**
	\defgroup ZeroEd ZeroEd
	\ingroup MistLand

  ZeroEd is the Editor Application of the game MistLands.
*/

#ifndef _DONT_MAIN					// <- OBS! Flytta inte på denna. Måste ligga i
	#define _MAINAPPLICATION_		// just denna fil och inte på flera ställen.
	#define _DONT_MAIN
#endif
 
#include <set> 
#include <algorithm>

#include "zeroed.h"
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
#include "../zerofpsv2/engine/inputhandle.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../zerofpsv2/render/glguirender.h"
#include "../mcommon/si_dm.h"
#include "../zerofpsv2/engine_systems/propertys/p_scriptinterface.h"

ZeroEd g_kZeroEd("ZeroEd", 0, 0, 0);

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{static int oka = 0;
	switch(msg)
	{
	case ZGM_COMMAND:
		g_kZeroEd.OnCommand(((int*)params)[0], (((int*)params)[1] == 1) ? true : false, win);
		break;
	case ZGM_SELECTLISTITEM:
		g_kZeroEd.OnClickListbox(((int*)params)[0],((int*)params)[1],win);
		break;
	case ZGM_SELECTTREEITEM:
		char** pszParams; pszParams = (char**) params;
		g_kZeroEd.OnClickTreeItem( pszParams[0], pszParams[1], 
			pszParams[2], pszParams[3][0] == '1' ? true : false);		
		break;
	case ZGM_TCN_SELCHANGE:
		int* data; data = (int*) params; 
		g_kZeroEd.OnClickTabPage((ZGuiTabCtrl*) data[2], data[0], data[1]);// fram med släggan
		break;
	case ZGM_KEYPRESS:
		if(((int*)params)[0] == KEY_RETURN)
		{
			if(strcmp("PropertyValEb", win->GetName()) == 0)
			{
				g_kZeroEd.AddPropertyVal();
			}
         else
         if(strcmp("SaveScriptFileNameEb", win->GetName()) == 0)
         {
            g_kZeroEd.SaveCurrentToScript();
         }
		}
		break;
	}
	return true;
}


ZeroEd::ZeroEd(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	g_ZFObjSys.Log_Create("zeroed");

	// Set Default values
	m_bEditSun		= 				true;
	m_bSoloMode     = 			true;
	m_bPlaceObjectsOnGround = 	false;
	m_bDisableFreeZonePlacement = false;

	// Register Variables
	
	// Register Commands
	Register_Cmd("new",			FID_NEW);		
	Register_Cmd("load",			FID_LOAD);		
	Register_Cmd("save",			FID_SAVE);
	Register_Cmd("saveas",		FID_SAVEAS);
	Register_Cmd("users",		FID_USERS);		
	Register_Cmd("lo",			FID_LOCALORDER);		
	Register_Cmd("lightmode", 	FID_LIGHTMODE);		
	Register_Cmd("editsun", 	FID_EDITSUN);		
	Register_Cmd("setcam", 		FID_SETCAM);		
	Register_Cmd("camlink", 	FID_CAMLINK);
	Register_Cmd("camsolo", 	FID_CAMSOLO);
	Register_Cmd("camgrid", 	FID_CAMGRID);
	Register_Cmd("selnone", 	FID_SELNONE);
	Register_Cmd("gridsize", 	FID_GRIDSIZE);
	Register_Cmd("gridsnap", 	FID_GRIDSNAP);
	Register_Cmd("camfollow",	FID_CAMFOLLOW);
	Register_Cmd("camnofollow",FID_CAMNOFOLLOW);
	Register_Cmd("delsel", 		FID_DELETE);
	Register_Cmd("clone",		FID_CLONE);
	Register_Cmd("cut",			FID_CUT);
	Register_Cmd("copy",			FID_COPY);
	Register_Cmd("paste",		FID_PASTE);
	Register_Cmd("jiddra",		FID_TEST_JIDDRA);

	Register_Cmd("snapsave",	FID_SNAPSAVE);
	Register_Cmd("snapload",	FID_SNAPLOAD);

	m_kDrawPos.Set(0,0,0);

	m_fHMInRadius  = 1;
	m_fHMOutRadius = 2;
	m_iEditLayer	= 1;
	m_fDelayTime   = 0.0;
	m_strWorldDir  = "";
   
	m_pkActiveCameraObject	= NULL;
	m_pkActiveCamera	= NULL;

	m_strActiveViewPort = "none";
	
	m_bGrabing = false;
} 


int ZeroEd::GetView(float x, float y)
{
	Vector3 kMin, kMax;

	for(int i=0; i<4; i++) 
	{
		kMin = m_pkCamera[i]->GetViewPortCorner();
		kMax = m_pkCamera[i]->GetViewPortCorner() + m_pkCamera[i]->GetViewPortSize();
		if(x < kMin.x)	continue;
		if(y < kMin.y)	continue;
		if(x > kMax.x)	continue;
		if(y > kMax.y)	continue;
		return i;
	}

	return -1;
}

bool ZeroEd::SetCamera(int iNum)
{
	switch(iNum) 
	{
		case 0:	return SetViewPort("vp1");	break;
		case 1:	return SetViewPort("vp2");	break;
		case 2:	return SetViewPort("vp3");	break;
		case 3:	return SetViewPort("vp4");	break;
		default:
			return false;
	}

	return false;
}

bool ZeroEd::SetViewPort(const char* szVpName)
{
	ZGuiWnd* pkWnd = GetWnd(szVpName);	
	if(!pkWnd)
		return false;
	if(!pkWnd->IsVisible())
		return false;

	Camera* pkCam = pkWnd->GetRenderTarget();
	if(!pkCam)
		return false;

	if(m_pkActiveCamera == pkCam)	return false;

	if(m_pkActiveCamera) m_pkActiveCamera->SetSelected(false);
	m_pkActiveCameraObject	= m_pkObjectMan->GetObjectByNetWorkID( pkCam->GetEntityID() );
	m_pkActiveCamera		= pkCam;
	m_pkActiveCamera->SetSelected(true);

	m_strActiveViewPort  = szVpName;

	return true;
}

void ZeroEd::CreateEditCameras()
{
	for(int i=0; i<4; i++) 
	{
		m_pkCameraObject[i] = m_pkObjectMan->CreateObjectFromScript("data/script/objects/t_camedit.lua");
		if(m_pkCameraObject[i]) 
		{
			if(i == 0)
				m_pkCameraObject[i]->SetParent( m_pkObjectMan->GetWorldObject() );				
			else
				m_pkCameraObject[i]->SetParent( m_pkCameraObject[0] );				
			
			P_Camera* m_pkCamProp = (P_Camera*)m_pkCameraObject[i]->GetProperty("P_Camera");
			m_pkCamProp->SetCamera(m_pkCamera[i]);
			m_pkCameraObject[i]->GetSave() = false;
		}
	}

	m_pkActiveCameraObject	= NULL;
	m_pkActiveCamera			= NULL;

	GetWnd("vp1")->SetRenderTarget(m_pkCamera[0]);
	GetWnd("vp2")->SetRenderTarget(m_pkCamera[1]);
	GetWnd("vp3")->SetRenderTarget(m_pkCamera[2]);
	GetWnd("vp4")->SetRenderTarget(m_pkCamera[3]);

	GetWnd("vp1")->SetMoveArea(Rect(0,0,800,600), false);
	GetWnd("vp2")->SetMoveArea(Rect(0,0,800,600), false);
	GetWnd("vp3")->SetMoveArea(Rect(0,0,800,600), false);
	GetWnd("vp4")->SetMoveArea(Rect(0,0,800,600), false);

	SetViewPort("vp1");
}


void ZeroEd::OnInit() 
{
	m_pkZFVFileSystem->AddRootPath( string("../data/mistlands/") ,"data/");
	m_pkZFVFileSystem->AddRootPath( string("../data/dm/") ,"data/");
	m_pkZFVFileSystem->AddRootPath( string("../data/zeroed/") ,"data/");

	m_pkConsole->Printf(" ZeroEd ");
	m_pkConsole->Printf("--------------------------------");
	m_pkConsole->Printf("");

	Init();

	//init dm script interface (register script functions for gameplay)
	DMLua::Init(m_pkObjectMan,m_pkScript,m_pkGuiMan);

	//run autoexec script
	if(!m_pkIni->ExecuteCommands("zeroed_autoexec.ini"))
		m_pkConsole->Printf("No zeroed_autoexec.ini found");


	//set reference distance
	m_pkAudioSys->SetReferensDistance(0.5);
	
}

void ZeroEd::Init()
{	
//	m_pkFps->m_bClientMode = true;

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

//	m_pkZShader->SetForceLighting(LIGHT_ALWAYS_OFF);	
	
	//enable debug graphics
	m_pkFps->SetDebugGraph(true);
	
	//register property bös
	RegisterPropertys();

	//register resources
	RegisterResources();


	//initiate our camera
	m_pkCamera[0]=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.25,250);	
	m_pkCamera[0]->SetName("persp");
	//m_pkCamera[0]->SetViewPort(0.5,0.5,0.5,0.5);
	//m_pkFps->SetRenderTarget(m_pkCamera[0]);

	m_pkCamera[1]=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.25,250);	
	m_pkCamera[1]->SetName("top");
	//m_pkCamera[1]->SetViewPort(0.0,0.5,0.5,0.5);
	m_pkCamera[1]->SetViewMode("top");
	//m_pkFps->SetRenderTarget(m_pkCamera[1]);
	
	m_pkCamera[2]=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.25,250);	
	m_pkCamera[2]->SetName("front");
	//m_pkCamera[2]->SetViewPort(0.0,0.0,0.5,0.5);
	m_pkCamera[2]->SetViewMode("front");
	//m_pkFps->SetRenderTarget(m_pkCamera[2]);

	m_pkCamera[3]=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.25,250);	
	m_pkCamera[3]->SetName("right");
	//m_pkCamera[3]->SetViewPort(0.5,0.0,0.5,0.5);
	m_pkCamera[3]->SetViewMode("right");
	//m_pkFps->SetRenderTarget(m_pkCamera[3]);


	//init mistland script intreface
	MistLandLua::Init(m_pkObjectMan,m_pkScript);
	
	// create gui script funktions
	GuiAppLua::Init(&g_kZeroEd, m_pkScript);

	// Load default texture and create default font and menu (NULL = No menu).
	InitGui(m_pkScript, "defguifont", "data/script/gui/defskins.lua", "data/script/gui/menu.txt", true, true); 

	//setup caption
	SetTitle("ZeroEd");
	// hide cursor
	m_pkInput->ShowCursor(true);

	// Setup the Edit Sun that are used for simple lightning in the editor.
	m_kSun.kRot = Vector3(1,2,1);
	m_kSun.kDiffuse=Vector4(1,1,1,0);
	m_kSun.kAmbient=Vector4(0.2,0.2,0.2,0);
	m_kSun.iType=DIRECTIONAL_LIGHT;			
	m_kSun.iPriority=10;
	m_kSun.fConst_Atten=1;
	m_kSun.fLinear_Atten=0;
	m_kSun.fQuadratic_Atten=0;

	// Create startup GUI for the the server from script.
	SetupGuiEnviroment();

	//start a clean world
	m_pkObjectMan->Clear();
	m_pkFps->StartServer(true,false);
	m_strWorldDir = "";

	m_pkInput->VKBind(string("mus"), string("z"), 0);

}

void ZeroEd::OnServerStart(void)
{		
	CreateEditCameras();

	// Create and setup the Env on the server.
	P_Enviroment* pe = (P_Enviroment*)m_pkCameraObject[0]->AddProperty("P_Enviroment");
	pe->SetEnable(true);		
	pe->SetEnviroment("data/enviroments/zeroed.env");

	SoloToggleView();
	m_fDelayTime = m_pkFps->GetEngineTime();
	SoloToggleView();
	GetWnd("vp1")->SetZValue(0);
	GetWnd("vp2")->SetZValue(0);
	GetWnd("vp3")->SetZValue(0);
	GetWnd("vp4")->SetZValue(0);
}

void ZeroEd::OnClientStart()
{
	m_pkActiveCameraObject = NULL;
}

void ZeroEd::RegisterResources()
{
	m_pkResourceDB->RegisterResource( string(".env"), Create__EnvSetting	);
}

void ZeroEd::RegisterPropertys()
{
	m_pkPropertyFactory->Register("P_Car", Create_P_Car);

	m_pkPropertyFactory->Register("P_DMClickMe", Create_P_DMClickMe);
	m_pkPropertyFactory->Register("P_DMMission", Create_P_DMMission);
	m_pkPropertyFactory->Register("P_DMItem", Create_P_DMItem);
	m_pkPropertyFactory->Register("P_DMGun", Create_P_DMGun);
	m_pkPropertyFactory->Register("P_ShadowBlob", Create_P_ShadowBlob);
	m_pkPropertyFactory->Register("P_DMCharacter", Create_P_DMCharacter);
	m_pkPropertyFactory->Register("P_DMShop", Create_P_DMShop);
	m_pkPropertyFactory->Register("P_DMHQ", Create_P_DMHQ);
	m_pkPropertyFactory->Register("P_Spawn", Create_P_Spawn);
	m_pkPropertyFactory->Register("P_Enviroment", Create_P_Enviroment);
	m_pkPropertyFactory->Register("P_ClientControl", Create_P_ClientControl);
	m_pkPropertyFactory->Register("P_ServerInfo", Create_P_ServerInfo);
	m_pkPropertyFactory->Register("P_Ml", Create_P_Ml);
//	m_pkPropertyFactory->Register("P_Event", Create_P_Event);
	m_pkPropertyFactory->Register("P_CharStats", Create_P_CharStats);
	m_pkPropertyFactory->Register("P_Item", Create_P_Item);
	m_pkPropertyFactory->Register("P_Spell", Create_P_Spell);
	m_pkPropertyFactory->Register("P_AI", Create_P_AI);
	m_pkPropertyFactory->Register("P_Container", Create_P_Container);
	m_pkPropertyFactory->Register("P_ArcadeCharacter", Create_P_ArcadeCharacter);
}


void ZeroEd::DrawHMEditMarker(HeightMap* pkHmap, Vector3 kCenterPos, float fInRadius, float fOutRadius )
{
	if(pkHmap == NULL)	return;

	m_pkRender->DrawBillboard(m_pkFps->GetCam()->GetModelViewMatrix(),kCenterPos,1,
		m_pkTexMan->Load("../data/textures/pointer.tga",T_NOMIPMAPPING));	

	Vector3	kVertex;
	vector<Vector3>	kVertexList;

	kCenterPos.y = 0;
	Vector3 kPos;

	for(int i=0; i<360; i+=(int)12.25) 
	{
		kVertex.x = float( cos(DegToRad( float(i) )) * fInRadius );
		kVertex.z = float( sin(DegToRad( float(i) )) * fInRadius );
		kVertex.y = float( pkHmap->Height(kCenterPos.x+kVertex.x,kCenterPos.z + kVertex.z) + 0.01 );
		kVertex += kCenterPos;
		kVertexList.push_back(kVertex);
	}

	m_pkRender->DrawCircle(kVertexList, m_pkRender->GetEditColor("hmapbrush") );

	kVertexList.clear();
	for(int i=0; i<360; i+=(int)12.25) 
	{
		kVertex.x = float( cos(DegToRad( float(i) )) * fOutRadius );
		kVertex.z = float( sin(DegToRad( float(i) )) * fOutRadius );
		kVertex.y = float(pkHmap->Height(kCenterPos.x+kVertex.x,kCenterPos.z + kVertex.z) + 0.01 );
		kVertex += kCenterPos;
		kVertexList.push_back(kVertex);
	}

	m_pkRender->DrawCircle(kVertexList, m_pkRender->GetEditColor("hmapbrush") );
}


void ZeroEd::DrawSelectedEntity()
{
	for(set<int>::iterator itEntity = m_SelectedEntitys.begin(); itEntity != m_SelectedEntitys.end(); itEntity++ ) 
	{
		Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID((*itEntity));
	
		if(pkEnt) 
		{
			float fRadius = pkEnt->GetRadius();
			if(fRadius < 0.1)
				fRadius = 0.1;

			Vector3 kMin = pkEnt->GetWorldPosV() - fRadius;
			Vector3 kMax = pkEnt->GetWorldPosV() + fRadius;

			if(m_iCurrentObject == (*itEntity))
				m_pkRender->DrawAABB( kMin,kMax, m_pkRender->GetEditColor("active/firstentity") );
			else
				m_pkRender->DrawAABB( kMin,kMax, m_pkRender->GetEditColor("active/entity") );
		}
	}
}


void ZeroEd::Select_Toggle(int iId, bool bMultiSelect)
{
   char szInfoText[100];

	if(!bMultiSelect && m_iCurrentObject != iId)		
		Select_None();
	
	if(m_SelectedEntitys.find(iId) == m_SelectedEntitys.end())
	{
		Select_Add(iId);
		m_iCurrentObject = iId;

      //if(m_iEditMode == EDIT_OBJECTS)
        // ((ZGuiTabCtrl*)GetWnd("WorkTabWnd"))->SetCurrentPage(2);
	}
	else 
	{
		Select_Remove(iId);
		if(iId == m_iCurrentObject)
		{
			if(m_SelectedEntitys.size())
				m_iCurrentObject = (*m_SelectedEntitys.begin());
			else
				m_iCurrentObject = -1;
		}
	}
 
   if(m_iCurrentObject != -1)
	{
		if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject))
      	sprintf(szInfoText, "Enity selected: ID[%d] Name[%s]", pkEnt->GetEntityID(), pkEnt->GetName().c_str());
	}
   else
      sprintf(szInfoText, "");

   SetText("ZeroEdInfoLabel", szInfoText, true);
   
   UpdatePropertyList(m_iCurrentObject);
}

void ZeroEd::DeleteSelected()
{
	if(m_SelectedEntitys.size() == 0)	return;

	NetPacket kNp;
	kNp.Clear();
	kNp.Write((char) ZFGP_EDIT);
	kNp.Write_Str("del");
	kNp.Write((int) m_SelectedEntitys.size());

	cout << "Delete Selected: ID, Type, Name" << endl;
	for(set<int>::iterator itEntity = m_SelectedEntitys.begin(); itEntity != m_SelectedEntitys.end(); itEntity++ ) 
	{
		int iId = (*itEntity);
		kNp.Write((int)iId);

	/*	Entity* pkEntity = m_pkObjectMan->GetObjectByNetWorkID((*itEntity));
		if(!pkEntity)	continue;
	
		cout << " " << pkEntity->GetEntityID() << " - '" << pkEntity->GetType() << "' - '" << pkEntity->GetName() << "'" <<endl;
		if(pkEntity->GetName() == string("ZoneObject"))
		{
			int iZoneID = m_pkObjectMan->GetZoneIndex( pkEntity->GetEntityID() );

			// Remove zoneplacement element in array
			ZoneData* pkData = m_pkObjectMan->GetZoneData(iZoneID);
			vector< pair<Vector3,Vector3> >::iterator it = m_kAddedZonePlacement.begin();
			for( ; it != m_kAddedZonePlacement.end(); it++)
				if(it->first == pkData->m_kPos && it->second == pkData->m_kSize) {
					m_kAddedZonePlacement.erase(it);
					break;
				}

			m_pkObjectMan->DeleteZone(iZoneID);
			cout << "Delete zone " << iZoneID << endl;
		}
		else
			m_pkObjectMan->Delete(pkEntity->GetEntityID());		*/
	}
	m_pkFps->RouteEditCommand(&kNp);

	m_SelectedEntitys.clear();
	m_iCurrentObject = -1;

			//fulhack deluxe för att inte kunna ta bort statiska entitys i zoner som inte är underconstruction
		/*	if(pkObj->GetParent()->GetName() == "StaticEntity")
			{
				cout<<"zone is not under construction "<<endl;
				return;
			}
		*/	
}

void ZeroEd::OnSystem()
{
	if(m_bGrabing)
	{
		if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_iGrabEntity))
		{
			if(P_Tcs* pkTcs = (P_Tcs*)pkEnt->GetProperty("P_Tcs"))
			{
				Vector3 kRotatedGrabPos = pkEnt->GetWorldRotM().VectorTransform(m_kLocalGrabPos);
				Vector3 kNewGrabPos = pkEnt->GetWorldPosV() + kRotatedGrabPos;
									
				m_kGrabPos = kNewGrabPos;
				
				m_kGrabCurrentPos = m_pkActiveCamera->GetPos() + Get3DMousePos(true)*m_fArmLength;
				Vector3 kForce = (m_kGrabCurrentPos - m_kGrabPos)*20;

				pkTcs->ApplyForce(m_kGrabPos,kForce,false);

			}
			else
				m_bGrabing = false;			
		}
		else
			m_bGrabing = false;			
	}

	
	Entity* pkClient = m_pkObjectMan->GetObjectByNetWorkID(m_pkFps->GetClientObjectID());
	if(pkClient)
	{
		if(!m_pkActiveCameraObject)
		{
			cout<<"got client object"<<endl;
	
			m_pkCameraObject[0] = pkClient;
			m_pkActiveCameraObject = m_pkCameraObject[0];
			
			P_Camera* m_pkCamProp = (P_Camera*)m_pkCameraObject[0]->AddProperty("P_Camera");
			m_pkCamProp->SetCamera(m_pkCamera[0]);
		
			SoloToggleView();
			m_fDelayTime = m_pkFps->GetEngineTime();
			SoloToggleView();
			GetWnd("vp1")->SetZValue(0);
			GetWnd("vp2")->SetZValue(0);
			GetWnd("vp3")->SetZValue(0);
			GetWnd("vp4")->SetZValue(0);		
		}
	}
}


void ZeroEd::OnIdle()
{	
	// FULHACK Tm Vim
	m_pkObjectMan->OwnerShip_Take( m_pkObjectMan->GetObjectByNetWorkID( m_pkFps->GetClientObjectID() ) );


	//m_pkFps->SetCamera(m_pkActiveCamera);		
	//m_pkFps->GetCam()->ClearViewPort();	

	if(m_pkGui->m_bHandledMouse == false)
	{
		Input();	
	}

 	//m_pkFps->UpdateCamera(); 		

	//for(unsigned int iPath = 0; iPath < kPath.size(); iPath++)
	//	pkRender->Draw_MarkerCross(kPath[iPath],Vector3(1,1,1),1);
   
	//if(m_pkServerInfoP)
	//{
	//	m_pkFps->DevPrintf("server","ServerName: %s", m_pkServerInfoP->GetServerName().c_str());
	//	m_pkFps->DevPrintf("server","Players: %d", m_pkServerInfoP->GetNrOfPlayers());	
	//}
	if(m_pkActiveCamera)
		m_pkAudioSys->SetListnerPosition(m_pkActiveCamera->GetPos(),m_pkActiveCamera->GetRotM());
	
	
	if(m_iEditMode == EDIT_HMAP) {
		HeightMap* pkMap = SetPointer();
		//DrawHMEditMarker(pkMap, m_kDrawPos, m_fHMInRadius,m_fHMOutRadius);
		}

	if(m_iEditMode == EDIT_ZONES)
	{
		UpdateZoneMarkerPos();		
		//DrawZoneMarker(m_kZoneMarkerPos);		
	}
	
	if(m_iEditMode == EDIT_OBJECTS)
	{	
		UpdateObjectMakerPos();
		//DrawCrossMarker(m_kObjectMarkerPos);		
	}
}


void ZeroEd::RenderInterface(void)
{
	DrawSelectedEntity();
	if(m_iEditMode == EDIT_HMAP) {
		HeightMap* pkMap = SetPointer();
		DrawHMEditMarker(pkMap, m_kDrawPos, m_fHMInRadius,m_fHMOutRadius);
		}

	if(m_iEditMode == EDIT_ZONES)		DrawZoneMarker(m_kZoneMarkerPos);
	if(m_iEditMode == EDIT_OBJECTS)	DrawCrossMarker(m_kObjectMarkerPos);		
	
	if(m_iEditMode == EDIT_OBJECTS && m_bGrabing)
	{							
		//m_pkRender->SetColor(Vector3(1,1,1));
		m_pkRender->Line(m_kGrabPos,m_kGrabCurrentPos);
	}
}

HeightMap* ZeroEd::SetPointer()
{
	m_kDrawPos.Set(0,0,0);

	Entity* pkEntity = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);								
	if(!pkEntity)	return NULL;
	P_HMRP2* hmrp = dynamic_cast<P_HMRP2*>(pkEntity->GetProperty("P_HMRP2"));
	if(!hmrp)		return NULL;

	Vector3 start	= m_pkActiveCamera->GetPos() + Get3DMousePos(true)*2;
	Vector3 dir		= Get3DMouseDir(true);

/*	Vector3 start	= m_pkFps->GetCam()->GetPos();
	Vector3 dir		= Get3DMouseDir(true);*/
	Vector3 end    = start + dir * 1000;

	if(dir.y >= 0) return NULL;

	// Find Level 0 for the selected zone in u.
	//float fLevelZero = kZData->m_pkZone->GetWorldPosV().y;
	//float fDiff = start.y - fLevelZero;
	//m_kDrawPos = start + dir * (fDiff);
	Plane kP;
	kP.m_kNormal.Set(0,1,0);
	kP.m_fD = - pkEntity->GetWorldPosV().y;

	Vector3 kIsect;

	if(kP.LineTest(start,end, &kIsect))
	{
		m_kDrawPos = kIsect;
		m_kDrawPos.y = hmrp->m_pkHeightMap->Height(m_kDrawPos.x,m_kDrawPos.z);
	}

	return hmrp->m_pkHeightMap;


	//Vector3 kLocalOffset = m_kDrawPos - hmrp->m_pkHeightMap->m_kCornerPos;
	//cout << "Local pos: " << kLocalOffset.x << ", " << kLocalOffset.y << ", " << kLocalOffset.z << endl;
}


void ZeroEd::HMModifyCommand(float fSize)
{
	float fTime = m_pkFps->m_pkObjectMan->GetSimDelta();

	for(set<int>::iterator itEntity = m_SelectedEntitys.begin(); itEntity != m_SelectedEntitys.end(); itEntity++ ) 
	{
		Entity* pkEntity = m_pkObjectMan->GetObjectByNetWorkID((*itEntity));
		if(!pkEntity)			continue;
		P_HMRP2* hmrp = dynamic_cast<P_HMRP2*>(pkEntity->GetProperty("P_HMRP2"));
		if(hmrp == NULL)		continue;

		Vector3 kLocalOffset = m_kDrawPos - hmrp->m_pkHeightMap->m_kCornerPos;

		m_kSelectedHMVertex = hmrp->m_pkHeightMap->GetSelection(m_kDrawPos,m_fHMInRadius,m_fHMOutRadius);
		if(m_kSelectedHMVertex.size() > 0) {
			if(fSize == 0.0)
				hmrp->m_pkHeightMap->Smooth(m_kSelectedHMVertex);
			else
				hmrp->m_pkHeightMap->Raise(m_kSelectedHMVertex, fSize * fTime);
			m_kSelectedHMVertex.clear();
			}
		}
}

void ZeroEd::OnHud(void)
{
	if(m_pkActiveCamera) 
	{
		m_pkFps->DevPrintf("editor","Grid Size: %f", m_pkActiveCamera->m_fGridSpace);			
		m_pkFps->DevPrintf("editor","Grid Snap: %i", m_pkActiveCamera->m_bGridSnap);			
		m_pkFps->DevPrintf("editor","View: %s", m_pkActiveCamera->GetName().c_str());			
		/*m_pkRender->DrawAABB(m_pkActiveCamera->GetViewPortCorner(),
			m_pkActiveCamera->GetViewPortCorner() + m_pkActiveCamera->GetViewPortSize(),
			Vector3(1,1,1),1);*/
	}
}

bool ZeroEd::DelayCommand()
{
	if(m_pkFps->GetEngineTime() < m_fDelayTime)
		return true;

	m_fDelayTime = m_pkFps->GetEngineTime() + float(0.3);
	return false;
}

void ZeroEd::EditRunCommand(FuncId_e eEditCmd)
{
	Entity* pkActiveEntity = m_pkObjectMan->GetObjectByNetWorkID( m_iCurrentObject );
	if(pkActiveEntity == NULL)
		return;

	if(eEditCmd == FID_CLONE)
	{
		m_pkObjectMan->CloneObject(m_iCurrentObject);	// Select_Toggle
	}

	ZFVFile kFile;

	if(eEditCmd == FID_CUT)
	{
		kFile.Open("copybuffer.dat",0,true);
		pkActiveEntity->Save(&kFile);
		kFile.Close();
	}

	if(eEditCmd == FID_COPY)
	{
		kFile.Open("copybuffer.dat",0,true);
		pkActiveEntity->Save(&kFile);
		kFile.Close();
	}
	
	if(eEditCmd == FID_PASTE)
	{
		if( kFile.Open("copybuffer.dat",0,false) ) 
		{
			Entity* pkObjNew = m_pkObjectMan->CreateObjectFromScriptInZone("data/script/objects/basic.lua", m_kObjectMarkerPos);

			Vector3 kPos = pkObjNew->GetLocalPosV();
			pkObjNew->Load(&kFile,false);
			kFile.Close();
			pkObjNew->SetLocalPosV(kPos);
		}
	}
}


void ZeroEd::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	ClientOrder kOrder;

	vector<string>	kUsers;
	float fTest;
	string strNewTitle;

	switch(cmdid) {
		case FID_NEW:
			if(m_bSoloMode)
				SoloToggleView();		
		
			m_pkObjectMan->Clear();
			//GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);
			m_pkFps->StartServer(true,false);
			m_strWorldDir = "";
			SetTitle("ZeroEd");
			m_kAddedZonePlacement.clear(); 
			break;
		
		case FID_LOAD:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				m_pkConsole->Printf("load [worlddir]");
				break;				
			}
			
			if(m_bSoloMode)
				SoloToggleView();

			if(!m_pkObjectMan->LoadWorld(kCommand->m_kSplitCommand[1]))
			{
				cout<<"Error loading world"<<endl;
				break;
			}				
			
			// Set the title to include the world name.
			m_strWorldDir = kCommand->m_kSplitCommand[1];
			strNewTitle = "ZeroEd - " + m_strWorldDir;
			SetTitle(strNewTitle);

			/*			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				m_pkConsole->Printf("load [mapdir]");
				break;				
			}
			
			cout<<"BLUB:"<<kCommand->m_kSplitCommand.size()<<endl;
			cout<<"loading world:"<<kCommand->m_kSplitCommand[1]<<endl;
			
			if(kCommand->m_kSplitCommand.size() > 2)
			{
				m_pkConsole->Printf("loading savegame: %s",kCommand->m_kSplitCommand[2].c_str());
				
				if(!m_pkObjectMan->LoadWorld(kCommand->m_kSplitCommand[1],kCommand->m_kSplitCommand[2]))
				{
					cout<<"Error loading world"<<endl;
					break;
				}				
			}
			else if(!m_pkObjectMan->LoadWorld(kCommand->m_kSplitCommand[1]))
			{
				cout<<"Error loading world"<<endl;
				break;
			}				
			
*/						
			cout<<"starting server"<<endl;
			//GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);			
			m_pkFps->StartServer(true,false);
			
			m_bNeedToRebuildZonePosArray = true;
			break;		
		
		case FID_SAVE:
			if(kCommand->m_kSplitCommand.size() > 1)
			{
				m_pkConsole->Printf(" The save command is now only for saving a loaded level.\nCan it be the 'saveas' command you should use? ");
				m_pkConsole->Printf(" To use save, enter it without a argument. Name of world dir can be found in title bar.");
				break;				
			}
		
			if(m_strWorldDir.empty()) 
			{
				m_pkConsole->Printf(" The current level has no name. Use saveas first");
				break;				
			}

			if(!m_pkObjectMan->SaveWorld(m_strWorldDir,true))
			{
				m_pkConsole->Printf("Error saving world");
				break;
			}	
			break;

		case FID_SAVEAS:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				m_pkConsole->Printf("save [worlddir]");
				break;				
			}
			
			if(!m_pkObjectMan->SaveWorld(kCommand->m_kSplitCommand[1],true))
			{
				m_pkConsole->Printf("Error saving world");
				break;
			}	

			// Set the title to include the world name.
			m_strWorldDir = kCommand->m_kSplitCommand[1];
			strNewTitle = "ZeroEd - " + m_strWorldDir;
			SetTitle(strNewTitle);

/*			cout<<"saving world:"<<endl;
			m_pkObjectMan->ForceSave();
			m_pkObjectMan->SaveZones();			
			cout<<"saved"<<endl;
*/			
			break;

		case FID_SNAPSAVE:
			m_pkObjectMan->SaveWorld("snapshot",true);
			break;

		case FID_SNAPLOAD:
			if(m_bSoloMode)
				SoloToggleView();
			GetSystem().RunCommand("set e_simspeed 0.0",CSYS_SRC_SUBSYS);
			m_pkObjectMan->LoadWorld("snapshot");
			//GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);			
			m_pkFps->StartServer(true,false);
			break;

		/*case FID_USERS:
			kUsers = m_pkPlayerDB->GetUsers();
			for(i=0; i<kUsers.size(); i++) {
				cout << "User: " << kUsers[i] << endl;
				}
			break;		*/

		case FID_EDITSUN:
			m_bEditSun = !m_bEditSun;

			if(m_bEditSun)
				m_pkLight->Add(&m_kSun);
			else
				m_pkLight->Remove(&m_kSun);

			break;

		case FID_SETCAM:
			if(kCommand->m_kSplitCommand.size() <= 1)
				break;
			m_pkActiveCamera->SetViewMode(kCommand->m_kSplitCommand[1]);
			break;
	
		case FID_GRIDSIZE:
			if(kCommand->m_kSplitCommand.size() <= 1)
				break;
			
			fTest = float( atof( kCommand->m_kSplitCommand[1].c_str()) );
			Camera::m_fGridSpace = fTest;
			break;

		case FID_TEST_JIDDRA:
			m_pkConsole->Printf("Long Text: ");
			m_pkConsole->Printf("This is a totaly pointless text that have no other purpose then being long and boring and boring and long. In short, don't fall asleep when you read this");
			m_pkConsole->Printf("\n");
			m_pkConsole->Printf("Long Text with no spaces: ");
			m_pkConsole->Printf("Thisisanotherpointlesstextbutwithoutanyspacesthistimesoitisnotaseasytoreadunlessyouareapersonthatareusedtoreadbookwithoutspacesandifitissothenyouareinsane.");
			m_pkConsole->Printf("\n\n");
			m_pkConsole->Printf("Text with three rows.");
			m_pkConsole->Printf("Rad 1 :(\nRad 2 :|\nRad 3 :)");
			m_pkConsole->Printf("\n\n\n");
			m_pkConsole->Printf("Mult rows with newline at the end.");
			m_pkConsole->Printf("Rad 1 :(\nRad 2 :|\nRad 3 :)\nRad 4 =)\n");
			break;

		case FID_CAMLINK:
				if(m_pkCameraObject[1]->GetParent() == m_pkCameraObject[0]) {
					// Unlink
					cout << "Unlink" << endl;
					m_pkCameraObject[1]->SetParent( m_pkObjectMan->GetWorldObject() );
					m_pkCameraObject[1]->SetLocalPosV(m_pkCameraObject[0]->GetLocalPosV());
					m_pkCameraObject[2]->SetParent( m_pkObjectMan->GetWorldObject() );
					m_pkCameraObject[2]->SetLocalPosV(m_pkCameraObject[0]->GetLocalPosV());
					m_pkCameraObject[3]->SetParent( m_pkObjectMan->GetWorldObject() );
					m_pkCameraObject[3]->SetLocalPosV(m_pkCameraObject[0]->GetLocalPosV());
					}
				else {
					// Link
					cout << "Link" << endl;
					m_pkCameraObject[1]->SetParent( m_pkCameraObject[0] );
					m_pkCameraObject[1]->SetLocalPosV(Vector3::ZERO);
					m_pkCameraObject[2]->SetParent( m_pkCameraObject[0] );
					m_pkCameraObject[2]->SetLocalPosV(Vector3::ZERO);
					m_pkCameraObject[3]->SetParent( m_pkCameraObject[0] );
					m_pkCameraObject[3]->SetLocalPosV(Vector3::ZERO);
				}

				break;

		case FID_CAMSOLO:			SoloToggleView();		break;
		case FID_CAMFOLLOW:		CamFollow(true);		break;
		case FID_CAMNOFOLLOW:	CamFollow(false);		break;
			
		case FID_CAMGRID:		Camera::m_bDrawOrthoGrid = !Camera::m_bDrawOrthoGrid;		break;
		case FID_GRIDSNAP:	Camera::m_bGridSnap = !Camera::m_bGridSnap;		break;
		case FID_SELNONE:		Select_None();		break;
		case FID_DELETE:		DeleteSelected();	break;

		case FID_CLONE:		EditRunCommand(FID_CLONE);		break;
		case FID_CUT:			EditRunCommand(FID_CUT);		break;
		case FID_COPY:			EditRunCommand(FID_COPY);		break;
		case FID_PASTE:		EditRunCommand(FID_PASTE);		break;

		case FID_LIGHTMODE:
			if(kCommand->m_kSplitCommand.size() <= 1)
				break;
/*
			iMode = atoi(kCommand->m_kSplitCommand[1].c_str());
			if(iMode == 0)	m_pkZShader->SetForceLighting(LIGHT_ALWAYS_ON);		
			if(iMode == 1)	m_pkZShader->SetForceLighting(LIGHT_ALWAYS_OFF);		
			if(iMode == 2)	m_pkZShader->SetForceLighting(LIGHT_MATERIAL);		
			break;
*/


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

void ZeroEd::SoloToggleView()
{
	if(DelayCommand())	return;
	
	Camera* pkCam = GetWnd(m_strActiveViewPort)->GetRenderTarget();

	if(m_bSoloMode) 
	{
		//m_pkFps->RemoveRenderTarget(NULL);
		m_pkFps->ClearRenderCameras();
		pkCam->m_bForceFullScreen = false;
		m_bSoloMode = false;
		GetWnd("vp1")->Show();	GetWnd("vp1")->SetPos(400,0,true,true);		GetWnd("vp1")->Resize(400,300,false);
		GetWnd("vp2")->Show();	GetWnd("vp2")->SetPos(0,0,true,true);			GetWnd("vp2")->Resize(400,300,false);
		GetWnd("vp3")->Show();	GetWnd("vp3")->SetPos(0,300,true,true);		GetWnd("vp3")->Resize(400,300,false);
		GetWnd("vp4")->Show();	GetWnd("vp4")->SetPos(400,300,true,true);		GetWnd("vp4")->Resize(400,300,false);
	} 
	else
	{
		m_bSoloMode = true;
		GetWnd("vp1")->Hide();
		GetWnd("vp2")->Hide();
		GetWnd("vp3")->Hide();
		GetWnd("vp4")->Hide();

		// Add Active as ZeroFps Fullscreen Render Target.
		if(pkCam) 
		{
			m_pkFps->AddRenderCamera(pkCam);
			pkCam->m_bForceFullScreen = true;
		}
	}
}

void ZeroEd::CamFollow(bool bFollowMode)
{
	if(bFollowMode)
	{
		// Start Follow mode.
		Entity* pkObj = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);		
		if(!pkObj)
			return;

		P_Camera* m_pkCamProp;

		// Remov old
		m_pkCamProp = (P_Camera*)m_pkActiveCameraObject->GetProperty("P_Camera");
		m_pkCamProp->SetCamera(NULL);

		// Add new.
		pkObj->AddProperty("P_Camera");
		m_pkCamProp = (P_Camera*)pkObj->GetProperty("P_Camera");
		m_pkCamProp->SetCamera(m_pkActiveCamera);
	}
	else
	{
		// End Follow Mode
		Entity* pkObj = m_pkObjectMan->GetObjectByNetWorkID( m_pkActiveCamera->GetEntityID() );		
		if(!pkObj)
			return;	// Hey we are not following anyone.

		// Remov old
		P_Camera* m_pkCamProp;
		m_pkCamProp = (P_Camera*)pkObj->GetProperty("P_Camera");
		m_pkCamProp->SetCamera(NULL);

		m_pkCamProp = (P_Camera*)m_pkActiveCameraObject->GetProperty("P_Camera");
		m_pkCamProp->SetCamera(m_pkActiveCamera);
		
		m_pkActiveCameraObject->SetWorldPosV(pkObj->GetWorldPosV());
	}
}

int	ZeroEd::GetTargetTCS(Vector3* pkPos)
{
	Vector3 kStart	= m_pkActiveCamera->GetPos();// + Get3DMousePos(true)*2;
	Vector3 kDir	= Get3DMouseDir(true);

	vector<Entity*> kObjects;		
	m_pkObjectMan->GetZoneObject()->GetAllEntitys(&kObjects);
		
	float d;	
	Vector3 cp;
	float closest = 999999999;
	Entity* pkClosest = NULL;
	Vector3 kPickPos;		
	
	for(unsigned int i=0;i<kObjects.size();i++)
	{
		//get mad property and do a linetest		
		if(kObjects[i]->GetProperty("P_Tcs"))
		{
			if(P_Mad* mp = (P_Mad*)kObjects[i]->GetProperty("P_Mad"))
			{
				if(mp->TestLine(kStart,kDir))
				{	
					cp = mp->GetLastColPos();
					d = kStart.DistanceTo(cp);
	
					if(d < closest)
					{
						closest = d;
						pkClosest = kObjects[i];
						kPickPos = cp;					
					}
				}
			}
		}
	}
	
	if(pkClosest)
	{
		*pkPos = kPickPos;
		return pkClosest->GetEntityID();
	}
		
	return -1;
}

Entity* ZeroEd::GetTargetObject()
{
	Vector3 start	= m_pkActiveCamera->GetPos() + Get3DMousePos(true)*2;
	Vector3 dir		= Get3DMouseDir(true);
//	dir.Set(0,1,0);

	start.Print();
	//cout << " - "; 
	dir.Print();
	//cout << endl; 

	vector<Entity*> kObjects;
	kObjects.clear();
	
	m_pkObjectMan->TestLine(&kObjects,start,dir);
	
	
	float closest = 999999999;
	Entity* pkClosest = NULL;	
	for(unsigned int i=0;i<kObjects.size();i++)
	{
		//cout << "Check " << kObjects[i]->GetEntityID() << " - '" << kObjects[i]->GetType() << "' - '" << kObjects[i]->GetName() << "'" <<endl;

		if(kObjects[i] == m_pkCameraObject[0])	continue;
		if(kObjects[i] == m_pkCameraObject[1])	continue;
		if(kObjects[i] == m_pkCameraObject[2])	continue;
		if(kObjects[i] == m_pkCameraObject[3])	continue;
		
		if(kObjects[i]->GetEntityID() <100000)
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


void ZeroEd::AddZone(Vector3 kPos, Vector3 kSize, string strName, bool bEmpty)
{
	if(m_pkObjectMan->IsInsideZone(kPos, kSize))
		return;

	if(m_bDisableFreeZonePlacement && !ZoneHaveNeighbour(kPos, kSize))
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
 

void ZeroEd::DrawZoneMarker(Vector3 kPos)
{

	Vector3 bla = m_kZoneSize / 2;
	m_pkRender->DrawAABB(kPos-bla,kPos+bla, m_pkRender->GetEditColor( "zonemarker" ));
}


void ZeroEd::DrawCrossMarker(Vector3 kPos)
{

	// Set Color here.
	m_pkRender->Line(kPos-Vector3(1,0,0),kPos+Vector3(1,0,0));
	m_pkRender->Line(kPos-Vector3(0,1,0),kPos+Vector3(0,1,0));	
	m_pkRender->Line(kPos-Vector3(0,0,1),kPos+Vector3(0,0,1));	
}


void ZeroEd::UpdateZoneMarkerPos()
{
	if(m_pkActiveCameraObject)
	{
		Vector3 temp = m_pkActiveCamera->GetPos() + Get3DMousePos(false)*15;
	
		float fStep = 2.0;
	
		m_kZoneMarkerPos.x = int(temp.x/fStep) * fStep;
		m_kZoneMarkerPos.y = int(temp.y/fStep) * fStep;
		m_kZoneMarkerPos.z = int(temp.z/fStep) * fStep;
	
	
		m_kZoneMarkerPos.x = round2(temp.x/fStep) * fStep;
		m_kZoneMarkerPos.y = round2(temp.y/fStep) * fStep;
		m_kZoneMarkerPos.z = round2(temp.z/fStep) * fStep;
	}	
}


void ZeroEd::UpdateObjectMakerPos()
{
	m_kObjectMarkerPos = /*m_pkFps->GetCam()*/ m_pkActiveCamera->GetPos() + Get3DMousePos(true)*2;
}

void ZeroEd::AutoSetZoneSize(string strName)
{
	int iPos = strName.find_last_of('-');
	if( iPos == string::npos )
		return;

	int x,y,z;
	char szString[256];
	strcpy(szString, &strName.c_str()[iPos + 1]);
	sscanf(szString,"%dx%dx%d", &x,&y,&z);
	m_kZoneSize.Set(float(x),float(y),float(z));
	//cout << "Setting Size " << x << ", " << y << ", "<< z << endl;
}

void ZeroEd::RotateActive()
{
	if(m_iCurrentObject != -1 && m_iEditMode == EDIT_OBJECTS)
	{
		Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);
		if(pkEnt) 
		{
			pkEnt->RotateLocalRotV( Vector3(0,90.0f,0) ); 
	
			// Update PFind Mesh
			P_PfMesh* pkMesh = (P_PfMesh*)pkEnt->GetProperty("P_PfMesh");
			if(pkMesh)
				pkMesh->CalcNaviMesh();
		}
	}
	else
	if(m_iCurrentMarkedZone != -1 && m_iEditMode == EDIT_ZONES)
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

void ZeroEd::ToogleLight(bool bEnabled)
{
/*	if(bEnabled)
		m_pkZShader->SetForceLighting(LIGHT_ALWAYS_ON);
	else
		m_pkZShader->SetForceLighting(LIGHT_ALWAYS_OFF);
*/		
}

void ZeroEd::UpdateStartLocatons()
{
	m_kLocations.clear();

	pair<string,Vector3> temp("Start",Vector3(0,1,0));
	m_kLocations.push_back(temp);
}

Vector3 ZeroEd::GetPlayerStartLocation(const char* csName)
{
	for(unsigned int i=0;i<m_kLocations.size();i++)
		if(m_kLocations[i].first == csName)
		{	
			cout<<"found location: "<<csName<<endl;
			return m_kLocations[i].second+Vector3( float((rand()%1000)/1000.0),  0,  float((rand()%1000)/1000.0));
		}
	
	return Vector3(0,0,0);
}

void ZeroEd::SetZoneEnviroment(const char* csEnviroment)
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

string ZeroEd::GetZoneEnviroment()
{
	string env;
	
	m_iCurrentMarkedZone = m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
	ZoneData* z = m_pkObjectMan->GetZoneData(m_iCurrentMarkedZone);
		
	if(z)
		env = z->m_strEnviroment;

	return env;
}

bool ZeroEd::PlaceObjectOnGround(int iObjectID)
{
	Entity* pkObj = m_pkObjectMan->GetObjectByNetWorkID(iObjectID);		
	if(pkObj) 
	{
		ZoneData* pkData = m_pkObjectMan->GetZoneData(pkObj->GetCurrentZone());		
		if(pkData->m_pkZone == NULL)
			return false;

		if(P_Mad* pkMad = (P_Mad*)pkData->m_pkZone->GetProperty("P_Mad"))
		{
			if(pkMad->TestLine(pkObj->GetWorldPosV(),Vector3(0,-1,0)))
			{
				Vector3 kPos = pkMad->GetLastColPos();
				
				pkObj->SetWorldPosV(kPos);
				return true;
			}
		}
		
		P_PfMesh* pkMesh = (P_PfMesh*)pkData->m_pkZone->GetProperty("P_PfMesh");
		if(pkMesh == NULL)
			return false;

		NaviMeshCell* pkCurrCell = pkMesh->GetCell(pkObj->GetLocalPosV());
		if(pkCurrCell)
		{
			Vector3 pos = pkObj->GetLocalPosV(); pos.y = pkCurrCell->m_kVertex->y;
			pkObj->SetLocalPosV(pos); 
			m_iCurrentObject = -1;
		}

		return true;
	}

	return false;
}


bool ZeroEd::ZoneHaveNeighbour(const Vector3& kPos, const Vector3& kSize)
{
	if(m_bNeedToRebuildZonePosArray)
		RebuildZonePosArray();

	const int size = m_kAddedZonePlacement.size();

	if(size > 0)
	{
		bool bOkidoki = false;

		Vector3 sLeft(kPos);
		Vector3 sRight(kPos.x + kSize.x, kPos.y, kPos.z);
		Vector3 sTop(kPos.x, kPos.y, kPos.z + kSize.z);
		Vector3 sBottom(kPos.x + kSize.x, kPos.y, kPos.z + kSize.z);

		for(int i=0; i<size; i++)
		{
			Vector3 pos = m_kAddedZonePlacement[i].first;
			Vector3 sz = m_kAddedZonePlacement[i].second;

			Vector3 dLeft(pos);
			Vector3 dRight(pos.x + sz.x, pos.y, pos.z);
			Vector3 dTop(pos.x, kPos.y, pos.z + sz.z);
			Vector3 dBottom(pos.x + sz.x, pos.y, pos.z + sz.z);

			if(sLeft == dLeft || sLeft == dRight || sLeft == dTop || sLeft == dBottom) {
				bOkidoki = true; break;
			}
			if(sRight == dLeft || sRight == dRight || sRight == dTop || sRight == dBottom) {
				bOkidoki = true; break;
			}
			if(sTop == dLeft || sTop == dRight || sTop == dTop || sTop == dBottom) {
				bOkidoki = true; break;
			}
			if(sBottom == dLeft || sBottom == dRight || sBottom == dTop || sBottom == dBottom) {
				bOkidoki = true; break;
			}
		}

		if(bOkidoki == false)
			return false;
	}

	m_kAddedZonePlacement.push_back( pair<Vector3,Vector3>(kPos, kSize) );
	return true;	
}


void ZeroEd::RebuildZonePosArray()
{
	m_kAddedZonePlacement.clear();
	vector<Entity*> vkEntList;
	m_pkObjectMan->GetAllObjects(&vkEntList);
	for(int i=0; i<vkEntList.size(); i++)
	{
		if(vkEntList[i]->IsZone()) 
		{
			int zone = m_pkObjectMan->GetZoneIndex(vkEntList[i]->GetWorldPosV(),-1, false);

			ZoneData* pkData = m_pkObjectMan->GetZoneData(zone);
			if(pkData)
				m_kAddedZonePlacement.push_back( pair<Vector3,
					Vector3>(pkData->m_kPos, pkData->m_kSize) );
		}
	}

	m_bNeedToRebuildZonePosArray = false;
}	













