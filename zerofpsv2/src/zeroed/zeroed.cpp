/**
	\defgroup ZeroEd ZeroEd
	\ingroup MistLand

  ZeroEd is the Editor Application of the game MistLands.
*/

#ifndef _DONT_MAIN					// <- OBS! Flytta inte p�denna. M�te ligga i
	#define _MAINAPPLICATION_		// just denna fil och inte p�flera st�len.
	#define _DONT_MAIN
#endif

#include "../mcommon/si_mistland.h"

#include "zeroed.h"
#include "../zerofpsv2/engine_systems/common/heightmap.h"
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "../zerofpsv2/engine_systems/propertys/p_hmrp2.h"
#include "../zerofpsv2/engine_systems/propertys/p_ambientsound.h"
#include "../zerofpsv2/engine/p_pfpath.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../mcommon/ml_netmessages.h"
#include "../mcommon/mainmcommon.h"

ZeroEd g_kZeroEd("ZeroEd", 0, 0, 0);




static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
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
		g_kZeroEd.OnClickTabPage((ZGuiTabCtrl*) data[2], data[0], data[1]);// fram med sl�gan
		break;
	case ZGM_KEYPRESS:
		g_kZeroEd.OnKeyPress(((int*)params)[0], win);
		break;
	}
	return true;
}


ZeroEd::ZeroEd(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 

	g_ZFObjSys.SetPreLogName("zeroed");
	g_ZFObjSys.Log_Create("zeroed");

	//register mcommon systems
	MCommon_RegisterSystems();
	
	
	// Set Default values	
	m_bEditSun					=	true;
	m_bSoloMode					=	true;
	m_bPlaceObjectsOnGround	=	false;
	m_bIsEditor					=	true;
   strMasterSmiley			=	"Vim";
	m_iSelectFileState		= 	NONE;
	m_pkPreviewEntity			= 	NULL;
	m_bRemoteEditing			=	false;
	m_kZoneModelRotation		= 	Vector3(0,0,0);
	m_pkZoneMarkerEntity 	=	NULL;
	m_iHMapEditMode			=  HMAP_EDITVERTEX; 
	m_bPlaneMovement			=	false;	
		
	strcpy(szCoolName , "Guldfisk");	

	int olle = 2;

	// Register Variables
	RegisterVariable("ap_coolname",				&strMasterSmiley,			CSYS_STRING);	

	m_strLoginName = "madviewclient";
   m_strLoginPW = "topsecret";

   RegisterVariable("ap_loginname", 	&m_strLoginName, CSYS_STRING);
   RegisterVariable("ap_loginpw", 	&m_strLoginPW, CSYS_STRING);
   
   RegisterVariable("ap_planemovement", 	&m_bPlaneMovement, CSYS_BOOL);
	
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
	Register_Cmd("gridautosnap", FID_GRIDAUTOSNAP);
	Register_Cmd("snapsize", 	FID_SNAPSIZE);
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
	Register_Cmd("findent",		FID_FINDENT);

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

	m_fZoneMarkerDistance = 15;
	m_fObjectMarkerDistance = 2;

	m_kSnapSize.Set(2,2,2);
	m_kSnapOffset.Set(0,0,0);
	
	m_kLastZonePos = Vector3(0,0,0);

	m_kTestGraph.SetSize(60,60,100);
	m_kTestGraph.SetMinMax(0,1500);

	m_iAutoSnapZoneCorner = -1;
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

	ZGuiWnd* akViewports[4] = {GetWnd("vp1"),GetWnd("vp2"),GetWnd("vp3"),GetWnd("vp4")};	
	for(int i=0; i<4; i++)
	{
		if(akViewports[i] != pkWnd)
		{
			akViewports[i]->GetSkin()->m_afBorderColor[0] = 0;
			akViewports[i]->GetSkin()->m_afBorderColor[1] = 0;
			akViewports[i]->GetSkin()->m_afBorderColor[2] = 0;
		}
		else
		{
			akViewports[i]->GetSkin()->m_afBorderColor[0] = 1;
			akViewports[i]->GetSkin()->m_afBorderColor[1] = 0;
			akViewports[i]->GetSkin()->m_afBorderColor[2] = 0;
		}
	}

	Camera* pkCam = static_cast<Camera*>(pkWnd->GetRenderTarget());
	//Camera* pkCam = pkWnd->GetRenderTarget();
	if(!pkCam)
		return false;

	if(m_pkActiveCamera == pkCam)	return false;

	if(m_pkActiveCamera) m_pkActiveCamera->SetSelected(false);
	m_pkActiveCameraObject	= m_pkEntityManager->GetEntityByID( pkCam->GetEntityID() );
	m_pkActiveCamera		= pkCam;
	m_pkActiveCamera->SetSelected(true);

	m_strActiveViewPort  = szVpName;

	return true;
}

void ZeroEd::CreateEditCameras()
{
	//create edit cameras
	for(int i=0; i<4; i++) 
	{
		m_pkCameraObject[i] = m_pkEntityManager->CreateEntityFromScript("data/script/objects/cameraedit.lua");
		if(m_pkCameraObject[i]) 
		{
			if(i == 0)
				m_pkCameraObject[i]->SetParent( m_pkEntityManager->GetWorldEntity() );				
			else
				m_pkCameraObject[i]->SetParent( m_pkCameraObject[0] );				
			
			P_Camera* pkCamProp = (P_Camera*)m_pkCameraObject[i]->GetProperty("P_Camera");
			pkCamProp->SetCamera(m_pkCamera[i]);
			m_pkCameraObject[i]->SetSave(false);
		}
	}

//      	m_pkCamera[0]->SetFSSEnabled(true);
//      	m_pkCamera[0]->SetFSSGLSLShader("#bloom1.frag");
//     	m_pkCamera[0]->SetFSSGLSLShader("#dead.frag");
 	
    	
	m_pkActiveCameraObject	= NULL;
	m_pkActiveCamera			= NULL;

	GetWnd("vp1")->SetRenderTarget(m_pkCamera[0]);
	GetWnd("vp2")->SetRenderTarget(m_pkCamera[1]);
	GetWnd("vp3")->SetRenderTarget(m_pkCamera[2]);
	GetWnd("vp4")->SetRenderTarget(m_pkCamera[3]);

	SetViewPort("vp1");
	
	//link cameras as default
	m_pkCameraObject[1]->SetRelativeOri(true);
	m_pkCameraObject[2]->SetRelativeOri(true);
	m_pkCameraObject[3]->SetRelativeOri(true);		
 	m_pkCameraObject[1]->SetParent( m_pkCameraObject[0] );
 	m_pkCameraObject[2]->SetParent( m_pkCameraObject[0] );
 	m_pkCameraObject[3]->SetParent( m_pkCameraObject[0] );
}


void ZeroEd::OnInit() 
{
	m_pkZFVFileSystem->AddRootPath( "../datafiles/zeroed/" ,		"/data");
	//m_pkZFVFileSystem->AddRootPath( "../datafiles/dm/" ,			"/data");	
	m_pkZFVFileSystem->AddRootPath( "../datafiles/mistlands/",	"/data");	
	//m_pkZFVFileSystem->AddRootPath( "../datafiles/stopemup/" ,		"/data");

	m_pkConsole->Printf(" ZeroEd ");
	m_pkConsole->Printf("--------------------------------");
	m_pkConsole->Printf("");

	Init();

	//init dm script interface (register script functions for gameplay)
	//DMLua::Init(m_pkEntityManager,m_pkScript,m_pkGuiMan);

	// Run Config script for editor colors.
	if(!m_pkIni->ExecuteCommands("/data/colors.cfg"))
		m_pkConsole->Printf("No colors.cfg");

	//run autoexec script
	if(!m_pkIni->ExecuteCommands("/zeroed_autoexec.ini"))
		m_pkConsole->Printf("No zeroed_autoexec.ini found");


	//set reference distance
// 	m_pkAudioSys->SetReferensDistance(0.5);

//	GetWnd("AddPointsToSounAreaBn")->m_bUseAlhpaTest = false;

	InitMainMenu();
}

void ZeroEd::Init()
{	
	m_pkZeroFps->m_bEditMode = true;

	//default edit mode 
	
	m_iEditMode = EDIT_ZONES;

	//object defaults
	m_iCurrentObject = -1;
	
	//zone defaults
	m_kZoneSize.Set(8,8,8);
	m_iCurrentMarkedZone = -1;
	m_strActiveZoneName = "";
	m_strActiveEnviroment = "";

//	m_pkZShader->SetForceLighting(LIGHT_ALWAYS_OFF);	
	
	//enable debug graphics
// 	m_pkZeroFps->SetDebugGraph(true);
	
	//register property b�
	RegisterPropertys();

	//register resources
	RegisterResources();


	//initiate our camera
	m_pkCamera[0]=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.1,250);	
	m_pkCamera[0]->SetName("persp");
	m_pkCamera[0]->SetFog(Vector4(1,1,1,1),300,400,true);
	//m_pkCamera[0]->SetViewPort(0.5,0.5,0.5,0.5);
	//m_pkFps->SetRenderTarget(m_pkCamera[0]);

	m_pkCamera[1]=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.1,250);	
	m_pkCamera[1]->SetName("top");
	//m_pkCamera[1]->SetViewPort(0.0,0.5,0.5,0.5);
	m_pkCamera[1]->SetViewMode("top");
	m_pkCamera[1]->SetFog(Vector4(1,1,1,1),300,400,true);
	//m_pkFps->SetRenderTarget(m_pkCamera[1]);
	
	m_pkCamera[2]=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.1,250);	
	m_pkCamera[2]->SetName("front");
	//m_pkCamera[2]->SetViewPort(0.0,0.0,0.5,0.5);
	m_pkCamera[2]->SetViewMode("front");
	m_pkCamera[2]->SetFog(Vector4(1,1,1,1),300,400,true);	
	//m_pkFps->SetRenderTarget(m_pkCamera[2]);

	m_pkCamera[3]=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.1,250);	
	m_pkCamera[3]->SetName("right");
	//m_pkCamera[3]->SetViewPort(0.5,0.0,0.5,0.5);
	m_pkCamera[3]->SetViewMode("right");
	m_pkCamera[3]->SetFog(Vector4(1,1,1,1),300,400,true);
	//m_pkFps->SetRenderTarget(m_pkCamera[3]);


	//init mistland script intreface
	MistLandLua::Init(m_pkEntityManager,m_pkScript);
	
	// create gui script funktions
	GuiAppLua::Init(&g_kZeroEd, m_pkScript);

	// Load default texture and create default font and menu (NULL = No menu).
	InitGui(m_pkScript, "defguifont", "data/script/gui/defskins.lua", 
		"data/script/gui/menu.txt", true, MANUALLY_SCALE ); 

	//setup caption
	SetTitle("ZeroEd");

	// hide cursor
	m_pkInput->ShowCursor(true);
	
	
	// Setup the Edit Sun that are used for simple lightning in the editor.
	LightSource* pkSun = m_pkLight->GetSunPointer();
 	pkSun->kRot = Vector3(2,2,1);
	
	//enable sun as default
  	ToogleLight();

 	
	// Create startup GUI for the the server from script.
	SetupGuiEnviroment();

	//start a clean world
	m_pkEntityManager->Clear();
	m_pkZeroFps->StartServer(true,false);
	m_strWorldDir = "";

	m_pkInput->VKBind(string("mus"), string("z"), 0);



}

void ZeroEd::OnServerStart(void)
{		
	CreateEditCameras();
	
	m_pkZoneMarkerEntity = NULL;
	
	// Create and setup the Env on the server.
	/*
	P_Enviroment* pe = (P_Enviroment*)m_pkCameraObject[0]->AddProperty("P_Enviroment");
	pe->SetEnable(true);		
	pe->SetEnviroment("data/enviroments/zeroed.env");
*/
	
	SoloToggleView();
	m_fDelayTime = m_pkZeroFps->GetEngineTime();
	SoloToggleView();
	
	
}

void ZeroEd::OnClientStart()
{
	char szIpPort[256];
	char szTitle[256];
	m_pkZeroFps->m_pkNetWork->AddressToStr( &m_pkZeroFps->m_pkNetWork->m_kServerAddress, szIpPort);
	sprintf(szTitle, "ZeroEd - %s", szIpPort);
	SetTitle(szTitle);

	m_pkActiveCameraObject = NULL;
	m_pkZoneMarkerEntity = NULL;
}

void ZeroEd::RegisterResources()
{
	//m_pkResourceDB->RegisterResource( string(".env"), Create__EnvSetting	);
}

void ZeroEd::RegisterPropertys()
{
	//m_pkPropertyFactory->Register("P_CharacterProperty",	Create_P_CharacterProperty);
	
	MCommon_RegisterPropertys( m_pkZeroFps, m_pkPropertyFactory );
}


void ZeroEd::DrawHMEditMarker(Vector3 kCenterPos, float fInRadius, float fOutRadius )
{
	if(m_SelectedEntitys.empty())
		return;

	vector<HeightMap*>	kHMaps;

	//get all hmaps	
	for(set<int>::iterator itEntity = m_SelectedEntitys.begin(); itEntity != m_SelectedEntitys.end(); itEntity++ ) 
	{
		if(P_HMRP2* hmrp = (P_HMRP2*)m_pkEntityManager->GetPropertyFromEntityID(*itEntity,"P_HMRP2"))
		{
			if(hmrp->GetHeightMap())
				kHMaps.push_back(hmrp->GetHeightMap());
		}
	}	
	
	
	m_pkRender->DrawBillboard(m_pkZeroFps->GetCam()->GetModelViewMatrix(),kCenterPos,1,
		m_pkTexMan->Load("/data/textures/pointer.tga",T_NOMIPMAPPING), false,false,true);	

	Vector3	kVertex;
	vector<Vector3>	kVertexList;

	kCenterPos.y = 0;
	Vector3 kPos;

	for(int i=0; i<360; i+=(int)12.25) 
	{
		kVertex.x = float( cos(DegToRad( float(i) )) * fInRadius );
		kVertex.z = float( sin(DegToRad( float(i) )) * fInRadius );
		kVertex.y = 0;
		
		for(int j = 0;j<kHMaps.size();j++)
			if(kHMaps[j]->Inside(kCenterPos.x+kVertex.x,kCenterPos.z + kVertex.z))
				kVertex.y = float( kHMaps[j]->Height(kCenterPos.x+kVertex.x,kCenterPos.z + kVertex.z) + 0.01 );
			
		kVertex += kCenterPos;
		kVertexList.push_back(kVertex);
	}

	m_pkRender->DrawCircle(kVertexList, m_pkRender->GetEditColor("hmapbrush") );

	kVertexList.clear();
	for(int i=0; i<360; i+=(int)12.25) 
	{
		kVertex.x = float( cos(DegToRad( float(i) )) * fOutRadius );
		kVertex.z = float( sin(DegToRad( float(i) )) * fOutRadius );
		kVertex.y = 0;

		for(int j = 0;j<kHMaps.size();j++)
			if(kHMaps[j]->Inside(kCenterPos.x+kVertex.x,kCenterPos.z + kVertex.z))
				kVertex.y = float( kHMaps[j]->Height(kCenterPos.x+kVertex.x,kCenterPos.z + kVertex.z) + 0.01 );

		kVertex += kCenterPos;
		kVertexList.push_back(kVertex);
	}

	m_pkRender->DrawCircle(kVertexList, m_pkRender->GetEditColor("hmapbrush") );
}


void ZeroEd::DrawSelectedEntity()
{
	for(set<int>::iterator itEntity = m_SelectedEntitys.begin(); itEntity != m_SelectedEntitys.end(); itEntity++ ) 
	{
		if(Entity* pkEnt = m_pkEntityManager->GetEntityByID((*itEntity))) 
		{
			//zone selected
			if(pkEnt->IsZone())
			{
				if(ZoneData* pkZone = GetZoneByEntityID((*itEntity)))
				{
					Vector3 kMin = pkZone->m_kPos - (pkZone->m_kSize/2.0) - pkZone->m_kSize.Unit() *0.2;
					Vector3 kMax = pkZone->m_kPos + (pkZone->m_kSize/2.0) + pkZone->m_kSize.Unit() *0.2;
									
					
					if(m_iCurrentObject == (*itEntity))
						m_pkRender->DrawAABB( kMin,kMax, m_pkRender->GetEditColor("active/firstentity") );
					else
						m_pkRender->DrawAABB( kMin,kMax, m_pkRender->GetEditColor("active/entity") );							
				}
			}
			else //object selected
			{
				float fRadius = pkEnt->GetRadius();
				if(fRadius < 0.1)
					fRadius = 0.1;
						
				Vector3 kMin = pkEnt->GetWorldPosV() - fRadius;
				Vector3 kMax = pkEnt->GetWorldPosV() + fRadius;

				P_Mad* mp = (P_Mad*)pkEnt->GetProperty("P_Mad");
				if(mp)
				{ 
					Vector3 kPos, kLocalMin, kLocalMax;
					if(mp->GetBBox(kLocalMin, kLocalMax, kPos))
					{
						kMin = pkEnt->GetWorldPosV() + kLocalMin;
						kMax = pkEnt->GetWorldPosV() + kLocalMax;
					}
				}

				if(m_iCurrentObject == (*itEntity))
					m_pkRender->DrawAABB( kMin,kMax, m_pkRender->GetEditColor("active/firstentity") );
				else
					m_pkRender->DrawAABB( kMin,kMax, m_pkRender->GetEditColor("active/entity") );
			}
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
		if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iCurrentObject))
		{
      	Vector3 kPos = pkEnt->GetWorldPosV();
			sprintf(szInfoText, "Enity selected ID:[%d] Name:[%s] Pos:%f,%f,%f", pkEnt->GetEntityID(), pkEnt->GetName().c_str(),kPos.x,kPos.y,kPos.z);
		}
	}
   else
      sprintf(szInfoText, "");

   SetText("ZeroEdInfoLabel", szInfoText, true);
   
   UpdatePropertyList(m_iCurrentObject);
}

void ZeroEd::AddSelected(NetPacket* pkNetPack)
{
	pkNetPack->Write((int) m_SelectedEntitys.size());

	for(set<int>::iterator itEntity = m_SelectedEntitys.begin(); itEntity != m_SelectedEntitys.end(); itEntity++ ) 
	{
		int iId = (*itEntity);
		pkNetPack->Write((int)iId);
	}
}

void ZeroEd::SendDeleteSelected()
{
	if(m_SelectedEntitys.size() == 0)	
		return;

	NetPacket kNp;
	kNp.Clear();
	kNp.Write((char) ZFGP_EDIT);
	kNp.Write_Str("del");
	/*kNp.Write((int) m_SelectedEntitys.size());

	for(set<int>::iterator itEntity = m_SelectedEntitys.begin(); itEntity != m_SelectedEntitys.end(); itEntity++ ) 
	{
		int iId = (*itEntity);
		kNp.Write((int)iId);
	}*/
	AddSelected(&kNp);
	m_pkZeroFps->RouteEditCommand(&kNp);

	m_SelectedEntitys.clear();
	m_iCurrentObject = -1;
}

void ZeroEd::OnSystem()
{
	if(m_bGrabing)
	{
		if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iGrabEntity))
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

	
	Entity* pkClient = m_pkEntityManager->GetEntityByID(m_pkZeroFps->GetClientObjectID());
	if(pkClient)
	{
		if(!m_pkActiveCameraObject)
		{
			cout<<"got client object"<<endl;
	
			m_pkCameraObject[0] = pkClient;
			m_pkActiveCameraObject = m_pkCameraObject[0];
			
			P_Camera* pkCamProp = (P_Camera*)m_pkCameraObject[0]->AddProperty("P_Camera");
			pkCamProp->SetCamera(m_pkCamera[0]);
		
			SoloToggleView();
			m_fDelayTime = m_pkZeroFps->GetEngineTime();
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
	m_pkEntityManager->OwnerShip_Take( m_pkEntityManager->GetEntityByID( m_pkZeroFps->GetClientObjectID() ) );


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

	if(m_iEditMode == EDIT_ZONES )
	{
		UpdateZoneMarkerPos();		
		//DrawZoneMarker(m_kZoneMarkerPos);		

	}
	
	if(m_iEditMode == EDIT_OBJECTS || m_iEditMode == EDIT_AMBIENTSOUNDS)
	{	
		UpdateObjectMakerPos();
		//DrawCrossMarker(m_kObjectMarkerPos);		
	}

	if(IsWndVisible("PreviewWnd"))
	{
		UpdatePreviewObject();
	}

	ZGuiMenu* pkWnd = (ZGuiMenu*)GetWnd("MainMenu");

	if(pkWnd && pkWnd->IsOpen() )
	{
		m_pkZeroFps->DevPrint_Show(false); 
	}
	else
	{
		m_pkZeroFps->DevPrint_Show(true); 
	}
	
}


void ZeroEd::RenderInterface(void)
{
	DrawSelectedEntity();
	if(m_iEditMode == EDIT_HMAP) 
	{
		//HeightMap* pkMap = SetPointer();
		DrawHMEditMarker(m_kDrawPos, m_fHMInRadius,m_fHMOutRadius);
	}

	if(m_iEditMode == EDIT_ENVIROMENTS)
	{
		
	}
	
	
	if(m_iEditMode == EDIT_ZONES)
		UpdateModelMarker(m_kZoneMarkerPos,true);
	else
		UpdateModelMarker(m_kZoneMarkerPos,false);

		
	if(m_iEditMode == EDIT_ZONES) DrawZoneMarker(m_kZoneMarkerPos);
	if(m_iEditMode == EDIT_OBJECTS || m_iEditMode == EDIT_AMBIENTSOUNDS)	DrawCrossMarker(m_kObjectMarkerPos);		
	
	if(m_iEditMode == EDIT_OBJECTS && m_bGrabing)
	{							
		//m_pkRender->SetColor(Vector3(1,1,1));
		m_pkRender->Line(m_kGrabPos,m_kGrabCurrentPos);
	}

	if(m_iEditMode == EDIT_AMBIENTSOUNDS || m_iEditMode == EDIT_OBJECTS)
	{
	//	m_pkRender->Line(m_pkAudioSys->GetListnerPos()+Vector3(0,-100,0),m_pkAudioSys->GetListnerPos()+Vector3(0,100,0));

		Entity* pkEnt;
		P_AmbientSound* pkProp;

		if((pkEnt = m_pkEntityManager->GetEntityByID(m_iCurrentObject)))
		{
			if(pkProp = (P_AmbientSound*) pkEnt->GetProperty("P_AmbientSound"))
			{
				float fFloor, fRoof;
				vector<Vector2> kPoints;
				pkProp->GetArea(kPoints, fFloor, fRoof); // pkProp->m_kPolygon;
				Vector3 pos =  pkEnt->GetWorldPosV();

				int size = kPoints.size();
				if(size > 1)
				{
					Vector3 kColor = Vector3(1,1,0);

					for(int i=0; i<size-1; i++)
					{
						// Draw floor
						m_pkRender->Line(
							Vector3(pos.x+kPoints[i].x,pos.y+fFloor,pos.z + kPoints[i].y), 
							Vector3(pos.x+kPoints[i+1].x,pos.y+fFloor,pos.z + kPoints[i+1].y), kColor);

						m_pkRender->Line(
							Vector3(pos.x+kPoints[i].x+0.1f,pos.y+fFloor,pos.z + kPoints[i].y+0.1f), 
							Vector3(pos.x+kPoints[i+1].x+0.1f,pos.y+fFloor,pos.z + kPoints[i+1].y+0.1f), kColor);

						// Draw roof
						m_pkRender->Line(
							Vector3(pos.x+kPoints[i].x,pos.y+fRoof,pos.z + kPoints[i].y), 
							Vector3(pos.x+kPoints[i+1].x,pos.y+fRoof,pos.z + kPoints[i+1].y), kColor);

						m_pkRender->Line(
							Vector3(pos.x+kPoints[i].x+0.1f,pos.y+fRoof,pos.z + kPoints[i].y+0.1f), 
							Vector3(pos.x+kPoints[i+1].x+0.1f,pos.y+fRoof,pos.z + kPoints[i+1].y+0.1f), kColor);
						
					}
				}
			}
		}
	}
	
	//draw zone list if connected to a server
	if(m_bRemoteEditing)
		m_pkEntityManager->DrawZones(&m_kNetworkZones);
}

HeightMap* ZeroEd::SetPointer()
{
	m_kDrawPos.Set(0,0,0);

	Entity* pkEntity = m_pkEntityManager->GetEntityByID(m_iCurrentObject);								
	if(!pkEntity)	return NULL;
	P_HMRP2* hmrp = static_cast<P_HMRP2*>(pkEntity->GetProperty("P_HMRP2"));
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
	static vector<HMSelectVertex> kSelVertex;
	static P_HMRP2* hmrp;
	
	
	float fTime = m_pkZeroFps->GetFrameTime();
	m_kSelectedHMVertex.clear();
	

	//loop all heightmaps
	for(set<int>::iterator itEntity = m_SelectedEntitys.begin(); itEntity != m_SelectedEntitys.end(); itEntity++ ) 
	{
		kSelVertex.clear();
	
		if(hmrp = (P_HMRP2*)m_pkEntityManager->GetPropertyFromEntityID(*itEntity,"P_HMRP2"))
		{
			Vector3 kLocalOffset = m_kDrawPos - hmrp->m_pkHeightMap->m_kCornerPos;
					
			//get selected vertexes 
			kSelVertex = hmrp->m_pkHeightMap->GetSelection(m_kDrawPos,m_fHMInRadius,m_fHMOutRadius);
			
			//add to vertex list
			m_kSelectedHMVertex.insert(m_kSelectedHMVertex.begin(), kSelVertex.begin(), kSelVertex.end());			
		}		
	}

	if(m_kSelectedHMVertex.size() > 0) 
	{
		if(fSize == 0.0)
			hmrp->m_pkHeightMap->Smooth(m_kSelectedHMVertex);
		else
			hmrp->m_pkHeightMap->Raise(m_kSelectedHMVertex, fSize * fTime);
		m_kSelectedHMVertex.clear();
	}
}

void ZeroEd::OnHud(void)
{
	if(m_pkActiveCamera) 
	{
		/*   //dvoid tyckte den va iv�en
	
		//m_kTestGraph.PushValue(m_pkZeroFps->m_fFps);
		m_kTestGraph.DrawGraph(10,50); // [ m_kTestGraph.DrawGraph(10,10); ] < zeb: h�er upp den lite s�den inte t�ker infolabeln
		m_pkZeroFps->DevPrintf("editor","Grid Size: %f", m_pkActiveCamera->m_fGridSpace);			
		m_pkZeroFps->DevPrintf("editor","Grid Snap: %i", m_pkActiveCamera->m_bGridSnap);			
		m_pkZeroFps->DevPrintf("editor","View: %s", m_pkActiveCamera->GetName().c_str());			
		/*m_pkRender->DrawAABB(m_pkActiveCamera->GetViewPortCorner(),
			m_pkActiveCamera->GetViewPortCorner() + m_pkActiveCamera->GetViewPortSize(),
			Vector3(1,1,1),1);*
			
		*/
	}
}

bool ZeroEd::DelayCommand()
{
	if(m_pkZeroFps->GetEngineTime() < m_fDelayTime)
		return true;

	m_fDelayTime = m_pkZeroFps->GetEngineTime() + float(0.3);
	return false;
}

void ZeroEd::EditRunCommand(FuncId_e eEditCmd)
{
	Entity* pkActiveEntity = m_pkEntityManager->GetEntityByID( m_iCurrentObject );
	if(pkActiveEntity == NULL)
		return;

	if(eEditCmd == FID_CLONE)
	{
		m_pkEntityManager->CloneEntity(m_iCurrentObject);	// Select_Toggle
	}

	ZFVFile kFile;

	if(eEditCmd == FID_CUT)
	{
		kFile.Open("copybuffer.dat",0,true);
		pkActiveEntity->Save(&kFile);
		kFile.Close();

		SendDeleteSelected();
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
			Entity* pkObjNew = m_pkEntityManager->CreateEntityFromScriptInZone(
				"data/script/objects/empty.lua", m_kObjectMarkerPos);

			pkObjNew->Load(&kFile,false);
			kFile.Close();
	
			pkObjNew->SetWorldPosV(m_pkActiveCamera->GetPos() + Get3DMousePos(true)*2);
		}
	}
}


void ZeroEd::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	NetPacket kNp;
//	ClientOrder kOrder;

	vector<string>	kUsers;
	float fTest;
	string strNewTitle;
	int iMode;

	switch(cmdid) {
		case FID_NEW:
			if(m_bSoloMode)
				SoloToggleView();		
		
			m_pkEntityManager->Clear();
			//GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);
			m_pkZeroFps->StartServer(true,false);
			m_strWorldDir = "";
			SetTitle("ZeroEd");
			m_kLastZonePos=Vector3(0,0,0); 
			break;
		
		case FID_LOAD:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				m_pkConsole->Printf("load [worlddir]");
				break;				
			}
			
			if(m_bSoloMode)
				SoloToggleView();

			if(!m_pkEntityManager->LoadWorld(kCommand->m_kSplitCommand[1]))
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
				
				if(!m_pkEntityManager->LoadWorld(kCommand->m_kSplitCommand[1],kCommand->m_kSplitCommand[2]))
				{
					cout<<"Error loading world"<<endl;
					break;
				}				
			}
			else if(!m_pkEntityManager->LoadWorld(kCommand->m_kSplitCommand[1]))
			{
				cout<<"Error loading world"<<endl;
				break;
			}				
			
*/						
			cout<<"starting server"<<endl;
			//GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);			
			m_pkZeroFps->StartServer(true,false);

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

			if(!m_pkEntityManager->SaveWorld(m_strWorldDir,true))
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
			
			if(!m_pkEntityManager->SaveWorld(kCommand->m_kSplitCommand[1],true))
			{
				m_pkConsole->Printf("Error saving world");
				break;
			}	

			// Set the title to include the world name.
			m_strWorldDir = kCommand->m_kSplitCommand[1];
			strNewTitle = "ZeroEd - " + m_strWorldDir;
			SetTitle(strNewTitle);

			break;

		case FID_SNAPSAVE:
			m_pkEntityManager->SaveWorld("snapshot",true);
			break;

		case FID_SNAPLOAD:
			if(m_bSoloMode)
				SoloToggleView();
			GetSystem().RunCommand("set e_simspeed 0.0",CSYS_SRC_SUBSYS);
			m_pkEntityManager->LoadWorld("snapshot");
			//GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);			
			m_pkZeroFps->StartServer(true,false);
			break;

		/*case FID_USERS:
			kUsers = m_pkPlayerDB->GetUsers();
			for(i=0; i<kUsers.size(); i++) {
				cout << "User: " << kUsers[i] << endl;
				}
			break;		*/

		case FID_EDITSUN:
				ToogleLight();

			break;

		case FID_SETCAM:
			if(kCommand->m_kSplitCommand.size() <= 1)
				break;
			m_pkActiveCamera->SetViewMode(kCommand->m_kSplitCommand[1]);
			break;

		case FID_SNAPSIZE:
			if(kCommand->m_kSplitCommand.size() <= 1)
				break;
			if ( kCommand->m_kSplitCommand[1] == "zonesize" )
				m_kSnapSize = m_kZoneSize;
			else
			{
				fTest = float( atof( kCommand->m_kSplitCommand[1].c_str()) );
				m_kSnapSize.Set(fTest, fTest, fTest);
			}

			/*m_kSnapOffset.x = (1 - ( int(m_kZoneMarkerPos.x / m_kSnapSize.x) - (m_kZoneMarkerPos.x / m_kSnapSize.x) ) * m_kSnapSize.x)
							  ((m_kZoneMarkerPos.x/m_kSnapSize.x) - int(m_kZoneMarkerPos.x/m_kSnapSize.x));

			m_kSnapOffset.y = (1 - ( int(m_kZoneMarkerPos.y / m_kSnapSize.y) - (m_kZoneMarkerPos.y / m_kSnapSize.y) ) * m_kSnapSize.y)
							  ((m_kZoneMarkerPos.y/m_kSnapSize.y) - int(m_kZoneMarkerPos.y/m_kSnapSize.y));

			m_kSnapOffset.z = (1 - ( int(m_kZoneMarkerPos.z / m_kSnapSize.z) - (m_kZoneMarkerPos.z / m_kSnapSize.z) ) * m_kSnapSize.z)
							  ((m_kZoneMarkerPos.z/m_kSnapSize.z) - int(m_kZoneMarkerPos.z/m_kSnapSize.z));
*/
			//m_kSnapOffset.x = -round2(m_kSnapOffset.x);
			//m_kSnapOffset.y = -round2(m_kSnapOffset.y);
			//m_kSnapOffset.z = -round2(m_kSnapOffset.z);
/*
			cout << "RealPos:" << m_kZoneMarkerPos.x << endl;
			cout << "OffsetX:" << m_kSnapOffset.x << endl;
			cout << "SnapX:" << ((m_kZoneMarkerPos.x/m_kSnapSize.x) - round2(m_kZoneMarkerPos.x/m_kSnapSize.x)) << endl;*/
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
				if(m_pkCameraObject[1]->GetParent() == m_pkCameraObject[0]) 
				{
					// Unlink
					cout << "Unlink" << endl;
					
					m_pkCameraObject[1]->SetRelativeOri(false);
					m_pkCameraObject[2]->SetRelativeOri(false);
					m_pkCameraObject[3]->SetRelativeOri(false);
					
					
					m_pkCameraObject[1]->SetParent( m_pkEntityManager->GetWorldEntity() );
					m_pkCameraObject[1]->SetLocalPosV(m_pkCameraObject[0]->GetLocalPosV());
					m_pkCameraObject[2]->SetParent( m_pkEntityManager->GetWorldEntity() );
					m_pkCameraObject[2]->SetLocalPosV(m_pkCameraObject[0]->GetLocalPosV());
					m_pkCameraObject[3]->SetParent( m_pkEntityManager->GetWorldEntity() );
					m_pkCameraObject[3]->SetLocalPosV(m_pkCameraObject[0]->GetLocalPosV());
				}
				else {
					// Link
					cout << "Link" << endl;
					m_pkCameraObject[1]->SetRelativeOri(true);
					m_pkCameraObject[2]->SetRelativeOri(true);
					m_pkCameraObject[3]->SetRelativeOri(true);
					
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
		case FID_GRIDSNAP:	Camera::m_bGridSnap = !Camera::m_bGridSnap;	break;
		case FID_GRIDAUTOSNAP: if(m_iAutoSnapZoneCorner==-1) m_iAutoSnapZoneCorner=0; else m_iAutoSnapZoneCorner=-1;	break; 
		case FID_SELNONE:		Select_None();		break;
		case FID_DELETE:		SendDeleteSelected();	break;

		case FID_CLONE:		EditRunCommand(FID_CLONE);		break;
		case FID_CUT:			EditRunCommand(FID_CUT);		break;
		case FID_COPY:			EditRunCommand(FID_COPY);		break;
		case FID_PASTE:		EditRunCommand(FID_PASTE);		break;

		case FID_FINDENT:
			if(kCommand->m_kSplitCommand.size() <= 1)
				break;
			iMode = atoi(kCommand->m_kSplitCommand[1].c_str());
		
			Select_None();
			Select_Add( iMode );
			break;

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

	

	}

}

void ZeroEd::SoloToggleView()
{
	if(DelayCommand())	return;

	Camera* pkCam = static_cast<Camera*>(GetWnd(m_strActiveViewPort)->GetRenderTarget());
	//Camera* pkCam = GetWnd(m_strActiveViewPort)->GetRenderTarget();

	if(m_bSoloMode) 
	{
		//m_pkFps->RemoveRenderTarget(NULL);
		m_pkZeroFps->ClearRenderCameras();
		pkCam->m_bForceFullScreen = false;
		m_bSoloMode = false;

		int w = m_pkGui->m_iResX / 2 - 2;
		int h = (m_pkGui->m_iResY-20) / 2 - 2;

		GetWnd("vp2")->Show(); GetWnd("vp2")->SetPos(1,21,true,true); GetWnd("vp2")->Resize(w,h,true);
		GetWnd("vp1")->Show(); GetWnd("vp1")->SetPos(w+3,21,true,true); GetWnd("vp1")->Resize(w,h,true);
		GetWnd("vp3")->Show(); GetWnd("vp3")->SetPos(1,h+21+2,true,true); GetWnd("vp3")->Resize(w,h,true);
		GetWnd("vp4")->Show(); GetWnd("vp4")->SetPos(w+3,h+21+2,true,true); GetWnd("vp4")->Resize(w,h,true);
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
			m_pkZeroFps->AddRenderCamera(pkCam);
			pkCam->m_bForceFullScreen = true;
		}
	}

	ZGuiMenu* pkMenu = ((ZGuiMenu*)GetWnd("MainMenu"));
	pkMenu->SetCheckMark("Menu_ViewSolo", m_bSoloMode);
}

void ZeroEd::CamFollow(bool bFollowMode)
{
	if(bFollowMode)
	{
		// Start Follow mode.
		Entity* pkObj = m_pkEntityManager->GetEntityByID(m_iCurrentObject);		
		if(!pkObj)
			return;

		P_Camera* pkCamProp;

		// Remov old
		pkCamProp = (P_Camera*)m_pkActiveCameraObject->GetProperty("P_Camera");
		pkCamProp->SetCamera(NULL);

		// Add new.
		pkObj->AddProperty("P_Camera");
		pkCamProp = (P_Camera*)pkObj->GetProperty("P_Camera");
		pkCamProp->SetCamera(m_pkActiveCamera);
	}
	else
	{
		// End Follow Mode
		Entity* pkObj = m_pkEntityManager->GetEntityByID( m_pkActiveCamera->GetEntityID() );		
		if(!pkObj)
			return;	// Hey we are not following anyone.

		// Remov old
		P_Camera* pkCamProp;
		pkCamProp = (P_Camera*)pkObj->GetProperty("P_Camera");
		pkCamProp->SetCamera(NULL);

		pkCamProp = (P_Camera*)m_pkActiveCameraObject->GetProperty("P_Camera");
		pkCamProp->SetCamera(m_pkActiveCamera);
		
		m_pkActiveCameraObject->SetWorldPosV(pkObj->GetWorldPosV());
	}
}

int	ZeroEd::GetTargetTCS(Vector3* pkPos)
{
	Vector3 kStart	= m_pkActiveCamera->GetPos();// + Get3DMousePos(true)*2;
	Vector3 kDir	= Get3DMouseDir(true);

	vector<Entity*> kObjects;		
	m_pkEntityManager->GetZoneEntity()->GetAllEntitys(&kObjects);
		
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

Entity*	ZeroEd::GetTargetObject2()
{
	Vector3 start	= m_pkActiveCamera->GetPos();	// + Get3DMousePos(true)*2;
	Vector3 dir		= Get3DMouseDir(true);

	vector<Entity*> kObjects;
	kObjects.clear();
	
	m_pkEntityManager->TestLine(&kObjects,start,dir);
	Vector3 cp;
	float d;

	
	float closest = 999999999;
	Entity* pkClosest = NULL;	
	for(unsigned int i=0;i<kObjects.size();i++)
	{
		//cout << "Check " << kObjects[i]->GetEntityID() << " - '" << kObjects[i]->GetType() << "' - '" << kObjects[i]->GetName() << "'" <<endl;

		if(kObjects[i] == m_pkCameraObject[0])		continue;
		if(kObjects[i] == m_pkCameraObject[1])		continue;
		if(kObjects[i] == m_pkCameraObject[2])		continue;
		if(kObjects[i] == m_pkCameraObject[3])		continue;
		if(kObjects[i] == m_pkZoneMarkerEntity)					continue;
		if(kObjects[i]->IsZone())										continue;
		if(kObjects[i]->GetEntityID() <100000)						continue;
		if(kObjects[i]->GetName() == "StaticEntity")				continue;
		if(kObjects[i]->GetName() == "A t_serverinfo.lua")		continue;
		
		//get mad property and do a linetest		
		//if(kObjects[i]->GetProperty("P_Tcs") == NULL)			continue;

		P_Mad* mp = (P_Mad*)kObjects[i]->GetProperty("P_Mad");
		d = 999999999;
	
		if(!mp)
		{
			d = (start - kObjects[i]->GetWorldPosV()).Length();
		}
		else
		{
			if(mp->TestLine(start,dir))
			{	
				cp = mp->GetLastColPos();
				d = start.DistanceTo(cp);
			}
		}

		if(d < closest)
		{
			closest = d;
			pkClosest = kObjects[i];
		}
	}
	
	return pkClosest;
}

Entity* ZeroEd::GetTargetObject()
{
	Vector3 start	= m_pkActiveCamera->GetPos() + Get3DMousePos(true)*2;
	Vector3 dir		= Get3DMouseDir(true);
//	dir.Set(0,1,0);

	//start.Print();
	//cout << " - "; 
	//dir.Print();
	//cout << endl; 

	vector<Entity*> kObjects;
	kObjects.clear();
	
	m_pkEntityManager->TestLine(&kObjects,start,dir);
	
	
	float closest = 999999999;
	Entity* pkClosest = NULL;	
	for(unsigned int i=0;i<kObjects.size();i++)
	{
		//cout << "Check " << kObjects[i]->GetEntityID() << " - '" << kObjects[i]->GetType() << "' - '" << kObjects[i]->GetName() << "'" <<endl;

		if(kObjects[i] == m_pkCameraObject[0])	continue;
		if(kObjects[i] == m_pkCameraObject[1])	continue;
		if(kObjects[i] == m_pkCameraObject[2])	continue;
		if(kObjects[i] == m_pkCameraObject[3])	continue;

		if(kObjects[i] == m_pkZoneMarkerEntity) continue;
				
		if(kObjects[i]->IsZone())
			continue;
		
		if(kObjects[i]->GetEntityID() <100000)
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


void ZeroEd::SendAddZone(Vector3 kPos, Vector3 kSize, Vector3 kModelRot,string strName)
{
	NetPacket kNp;
	kNp.Write((char) ZFGP_EDIT);
	kNp.Write_Str("create_zone");
	kNp.Write_Str(strName);
	kNp.Write(kPos);
	kNp.Write(kSize);		
	kNp.Write(kModelRot);		
	m_pkZeroFps->RouteEditCommand(&kNp);		

	//SetZoneEnviroment(m_strActiveEnviroment.c_str());	
}
 

void ZeroEd::DrawZoneMarker(Vector3 kPos)
{

	Vector3 kMarkerSize = m_kZoneSize / 2;
	// fix for zones with odd sizes
	kPos.x += kMarkerSize.x - int(kMarkerSize.x);
	kPos.y += kMarkerSize.y - int(kMarkerSize.y);
	kPos.z += kMarkerSize.z - int(kMarkerSize.z);

	m_pkRender->DrawAABB(kPos-kMarkerSize, kPos+kMarkerSize, m_pkRender->GetEditColor( "zonemarker" ));

	if(m_iAutoSnapZoneCorner != -1)
	{
		switch(m_iAutoSnapZoneCorner)
		{
		case eTop:
			m_pkRender->Line(m_kZoneMarkerPos, m_kZoneMarkerPos+Vector3(0,0,16));
			break;
		case eBottom:
			m_pkRender->Line(m_kZoneMarkerPos, m_kZoneMarkerPos+Vector3(0,0,-16));
			break;
		case eLeft:
			m_pkRender->Line(m_kZoneMarkerPos, m_kZoneMarkerPos+Vector3(16,0,0));
			break;
		case eRight:
			m_pkRender->Line(m_kZoneMarkerPos, m_kZoneMarkerPos+Vector3(-16,0,0));
			break;
		case eTopLeft:
			m_pkRender->Line(m_kZoneMarkerPos, m_kZoneMarkerPos+Vector3(16,0,16));
			break;
		case eTopRight:
			m_pkRender->Line(m_kZoneMarkerPos, m_kZoneMarkerPos+Vector3(-16,0,16));
			break;
		case eBottomLeft:
			m_pkRender->Line(m_kZoneMarkerPos, m_kZoneMarkerPos+Vector3(16,0,-16));
			break;
		case eBottomRight:
			m_pkRender->Line(m_kZoneMarkerPos, m_kZoneMarkerPos+Vector3(-16,0,-16));
			break;
		}

	}
}


void ZeroEd::DrawCrossMarker(Vector3 kPos)
{

	// Set Color here.
	m_pkRender->Line(kPos-Vector3(1,0,0),kPos+Vector3(1,0,0));
	m_pkRender->Line(kPos-Vector3(0,1,0),kPos+Vector3(0,1,0));	
	m_pkRender->Line(kPos-Vector3(0,0,1),kPos+Vector3(0,0,1));	
}

void ZeroEd::UpdateModelMarker(Vector3 kPos,bool bEnabled)
{
	//create zonemaker entity if none
	if(!m_pkZoneMarkerEntity)
	{	
		//create zonemodelmarker
		m_pkZoneMarkerEntity = m_pkEntityManager->CreateEntity();
		m_pkZoneMarkerEntity->SetParent(m_pkEntityManager->GetWorldEntity());
		m_pkZoneMarkerEntity->SetSave(false);
		m_pkZoneMarkerEntity->SetInterpolate(false);
		m_pkZoneMarkerEntity->AddProperty("P_Mad");
	}	
	
	
	if(m_pkZoneMarkerEntity)
	{
		m_pkZoneMarkerEntity->SetWorldPosV(kPos);
		m_pkZoneMarkerEntity->SetLocalRotV(m_kZoneModelRotation);
		
		if(P_Mad* pkMad = (P_Mad*)m_pkZoneMarkerEntity->GetProperty("P_Mad"))
		{
			pkMad->SetBase(m_strActiveZoneName.c_str());		
			pkMad->SetVisible(bEnabled);
		}		
	}
}

void ZeroEd::UpdateZoneMarkerPos()
{
	if(m_pkActiveCameraObject)
	{
		Vector3 temp = m_pkActiveCamera->GetPos() + Get3DMousePos(false)*m_fZoneMarkerDistance;
		
		if(m_iAutoSnapZoneCorner == -1)
		{
			m_kZoneMarkerPos.x = round2(temp.x/m_kSnapSize.x) * m_kSnapSize.x + m_kSnapOffset.x;
			m_kZoneMarkerPos.y = round2(temp.y/m_kSnapSize.y) * m_kSnapSize.y + m_kSnapOffset.y;
			m_kZoneMarkerPos.z = round2(temp.z/m_kSnapSize.z) * m_kSnapSize.z + m_kSnapOffset.z;
			
			/*
			m_kZoneMarkerPos.x = (temp.x/m_kSnapSize.x ) * m_kSnapSize.x;
			m_kZoneMarkerPos.y = (temp.y/m_kSnapSize.y ) * m_kSnapSize.y;
			m_kZoneMarkerPos.z = (temp.z/m_kSnapSize.z ) * m_kSnapSize.z;			
			*/
			
			//m_kZoneMarkerPos = temp;
		}
		else
		{
			if(m_kLastZonePos.IsZero())
				m_kZoneMarkerPos = temp;
			else
			{
				m_kZoneMarkerPos = GetZonePosAutoSnap();
			}
		}
		
		//
		// Tvinga kameran att beh�la samma X,Y eller Z position som tidigare
		// om man befinner sig i ortogonalt kameral�e.
		//
		static float s_fLastYPos = 0.0f;
		static float s_fLastXPos = 0.0f;
		static float s_fLastZPos = 0.0f;

		if(m_pkActiveCamera->GetViewMode() == Camera::CAMMODE_ORTHO_LEFT ||
			m_pkActiveCamera->GetViewMode() == Camera::CAMMODE_ORTHO_RIGHT)
			m_kZoneMarkerPos.x = s_fLastXPos; 
		else
			s_fLastXPos = m_kZoneMarkerPos.x;
		if(m_pkActiveCamera->GetViewMode() == Camera::CAMMODE_ORTHO_TOP ||
			m_pkActiveCamera->GetViewMode() == Camera::CAMMODE_ORTHO_BOT)
			m_kZoneMarkerPos.y = s_fLastYPos;
		else
			s_fLastYPos = m_kZoneMarkerPos.y;
		if(m_pkActiveCamera->GetViewMode() == Camera::CAMMODE_ORTHO_FRONT ||
			m_pkActiveCamera->GetViewMode() == Camera::CAMMODE_ORTHO_BACK)
			m_kZoneMarkerPos.z = s_fLastZPos; 
		else
			s_fLastZPos = m_kZoneMarkerPos.z;
	}	
}

void ZeroEd::UpdateObjectMakerPos()
{
	m_kObjectMarkerPos = /*m_pkFps->GetCam()*/ m_pkActiveCamera->GetPos() + Get3DMousePos(true)*m_fObjectMarkerDistance;


	if(m_pkActiveCameraObject && m_iEditMode == EDIT_AMBIENTSOUNDS)
	{
		Vector3 temp = m_pkActiveCamera->GetPos() + Get3DMousePos(false) * 15;
	
		m_kObjectMarkerPos.x = round2(temp.x/m_kSnapSize.x) * m_kSnapSize.x;
		m_kObjectMarkerPos.y = 0;
		m_kObjectMarkerPos.z = round2(temp.z/m_kSnapSize.z) * m_kSnapSize.z;
	}	
}

void ZeroEd::AutoSetZoneSize(string strName)
{
	int iPos = strName.find_last_of('-');
	if( iPos == string::npos )
	{
		m_kZoneSize.Set(8,8,8);
		return;
	}

	float x,y,z;
	char szString[256];
	strcpy(szString, &strName.c_str()[iPos + 1]);
	sscanf(szString,"%fx%fx%f", &x,&y,&z);
	m_kZoneSize.Set(float(x),float(y),float(z));
	//cout << "Setting Size " << x << ", " << y << ", "<< z << endl;
	
	//also reset rotation 
	m_kZoneModelRotation.Set(0,0,0);
}

void ZeroEd::SendRotateEntity(int iEntityID,const Vector3& kRot)
{
	NetPacket kNp;

	kNp.Clear();
	kNp.Write((char) ZFGP_EDIT);
	kNp.Write_Str("rot");
	kNp.Write(iEntityID);
	kNp.Write(kRot);
	m_pkZeroFps->RouteEditCommand(&kNp);	
}


void ZeroEd::ToogleLight()
{
	//m_bEditSun = !m_bEditSun;

	if(!m_bEditSun)
	{	
		m_bEditSun = true;	
		LightSource* pkLight = m_pkLight->GetSunPointer();
		pkLight->kDiffuse.Set(0,0,0,0);		
		pkLight->kAmbient.Set(0,0,0,0);
		pkLight->kSpecular=Vector4(0,0,0,0);
	}
	else
	{
		m_bEditSun = false;	
		LightSource* pkLight = m_pkLight->GetSunPointer();
 		pkLight->kDiffuse=Vector4(0.8,0.8,0.8,0);
 		pkLight->kAmbient=Vector4(0.6,0.6,0.6,0);
 		pkLight->kSpecular=Vector4(1,1,1,0);
	}
}


string ZeroEd::GetZoneEnviroment()
{
	string env;

	const Vector3 pos = m_kZoneMarkerPos;
	
	m_iCurrentMarkedZone = m_pkEntityManager->GetZoneIndex(pos,m_iCurrentMarkedZone,false);
	ZoneData* z = m_pkEntityManager->GetZoneData(m_iCurrentMarkedZone);
		
	if(z)
		env = z->m_strEnviroment;

	return env;
}

bool ZeroEd::PlaceObjectOnGround(int iObjectID)
{
	Entity* pkObj = m_pkEntityManager->GetEntityByID(iObjectID);		
	if(pkObj) 
	{
		ZoneData* pkData = m_pkEntityManager->GetZoneData(pkObj->GetCurrentZone());		
		if(pkData == NULL || pkData->m_pkZone == NULL)
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

void ZeroEd::OnNetworkMessage(NetPacket *PkNetMessage)
{
	unsigned char ucType;

	// Read Type of Message.
	PkNetMessage->Read(ucType);
	//int iJiddra = ucType;
	//m_pkConsole->Printf("AppMessageType: %d", iJiddra );

	switch(ucType)
	{

		case ZPGP_ZED_ZONELIST:
		{
			//cout<<"got zone list"<<endl;
		
			bool bClear;	
			bool bRemove;		
			PkNetMessage->Read(bClear);
			PkNetMessage->Read(bRemove);
			
			//shuld list be cleared
			if(bClear)
				m_kNetworkZones.clear();
					
			ZoneData	kTemp;
			
			//read first zone status
			PkNetMessage->Read(kTemp.m_iStatus);
			while(kTemp.m_iStatus != -1)
			{
				PkNetMessage->Read(kTemp.m_iZoneID);
				PkNetMessage->Read(kTemp.m_iZoneObjectID);
				PkNetMessage->Read(kTemp.m_kPos);
				PkNetMessage->Read(kTemp.m_kSize);
				// calculate testing position for zone
				kTemp.m_kTestPos.x = kTemp.m_kPos.x + ( (kTemp.m_kSize.x / 2) - int(kTemp.m_kSize.x / 2) );
				kTemp.m_kTestPos.y = kTemp.m_kPos.y + ( (kTemp.m_kSize.y / 2) - int(kTemp.m_kSize.y / 2) );
				kTemp.m_kTestPos.z = kTemp.m_kPos.z + ( (kTemp.m_kSize.z / 2) - int(kTemp.m_kSize.z / 2) );


				//remove or add zone?
				if(!bRemove)
				{					
					m_kNetworkZones.push_back(kTemp);
				}
				else
				{
					for(vector<ZoneData>::iterator it=m_kNetworkZones.begin();it!=m_kNetworkZones.end();it++) 
					{
						if((*it).m_iZoneID == kTemp.m_iZoneID)
						{
							cout<<"remove zone:"<<kTemp.m_iZoneID<<endl;
							m_kNetworkZones.erase(it);
							break;
						}
					}
				}
							
				PkNetMessage->Read(kTemp.m_iStatus);
			}
			break;
		}
			
		default:
			cout << "Error in game packet : " << (int) ucType << endl;
			PkNetMessage->SetError(true);
			return;
	}
}


void ZeroEd::OnDisconnect(int iConnectionID)
{
	m_bRemoteEditing = false;
	cout << "NOOOOOOOOOOOO im disconnected" << endl;
}

void	ZeroEd::SendZoneListRequest()
{
	NetPacket kNp;
	kNp.Clear();
	kNp.Write((char) ZFGP_EDIT);
	kNp.Write_Str("request_zones");
	
	m_pkZeroFps->RouteEditCommand(&kNp);
}

void	ZeroEd::SendRotateZoneModel(int iZoneID)
{
	NetPacket kNp;
	kNp.Clear();
	kNp.Write((char) ZFGP_EDIT);
	kNp.Write_Str("rotatezonemodel");
	kNp.Write(iZoneID);
	
	m_pkZeroFps->RouteEditCommand(&kNp);
}

void	ZeroEd::SendSetZoneEnviroment(string strEnviroment,int iZoneID)
{
	NetPacket kNp;
	kNp.Clear();
	kNp.Write((char) ZFGP_EDIT);
	kNp.Write_Str("setzoneenviroment");
	kNp.Write(iZoneID);
	kNp.Write_Str(strEnviroment);	
	
	m_pkZeroFps->RouteEditCommand(&kNp);

}

void	ZeroEd::SendSetZoneModel(string strModel,int iZoneID)
{
	NetPacket kNp;
	kNp.Clear();
	kNp.Write((char) ZFGP_EDIT);
	kNp.Write_Str("setzonemodel");
	kNp.Write(iZoneID);
	kNp.Write_Str(strModel);	
	
	m_pkZeroFps->RouteEditCommand(&kNp);

}

void ZeroEd::OnClientConnected() 
{
	m_bRemoteEditing = true;
	cout<<"connected"<<endl;	
	SendZoneListRequest();
}

int ZeroEd::GetZoneID(const Vector3& kPos)
{
	if(m_bRemoteEditing)
	{
		for(int i = 0;i<m_kNetworkZones.size();i++)
		{
			if(m_kNetworkZones[i].IsInside(kPos))
				return m_kNetworkZones[i].m_iZoneID;	
		}	
		return -1;	
	}
	else
	{
		return m_pkEntityManager->GetZoneIndex(m_kZoneMarkerPos,m_iCurrentMarkedZone,false);		
	}
}

ZoneData* ZeroEd::GetZoneData(int iZoneID)
{
	if(m_bRemoteEditing)
	{
		for(int i = 0;i<m_kNetworkZones.size();i++)
		{
			if(m_kNetworkZones[i].m_iZoneID == iZoneID)
				return &m_kNetworkZones[i];
		}	
		
		return NULL;
	}
	else
	{
		return m_pkEntityManager->GetZoneData(iZoneID);
	}
}

ZoneData* ZeroEd::GetZoneByEntityID(int iEntityID)
{
	if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(iEntityID))
	{
		if(m_bRemoteEditing)
		{
			for(int i = 0;i<m_kNetworkZones.size();i++)
			{
				if(m_kNetworkZones[i].m_iZoneObjectID == iEntityID)
				{
					return &m_kNetworkZones[i];
				}
			}
			
			return NULL;
		}
		else
		{				
			return m_pkEntityManager->GetZone(pkEnt);
		}	
	}
	
	return NULL;
}

Vector3 ZeroEd::GetZonePosAutoSnap()
{
	Vector3 pos = m_kLastZonePos + Vector3(m_kZoneSize.x,0,m_kZoneSize.z) - 
		Vector3( (m_kZoneSize.x-m_kLastZoneSize.x)/2, -(m_kZoneSize.y-m_kLastZoneSize.y)/2, 
		(m_kZoneSize.z-m_kLastZoneSize.z)/2 );

	switch(m_iAutoSnapZoneCorner)
	{
	case eTop:
		pos.x += (m_kLastZonePos.x-pos.x) - (m_kZoneSize.x-m_kLastZoneSize.x)/2;
		break;
	case eTopRight:
		pos.x += (m_kLastZonePos.x-pos.x) - (m_kZoneSize.x-m_kLastZoneSize.x)/2 - m_kLastZoneSize.x;
		break;
	case eBottom:
		pos.x += (m_kLastZonePos.x-pos.x) - (m_kZoneSize.x-m_kLastZoneSize.x)/2;
		pos.z += (m_kLastZonePos.z-pos.z) - (m_kZoneSize.z-m_kLastZoneSize.z)/2 - m_kLastZoneSize.z;
		break;
	case eBottomRight:
		pos.x += (m_kLastZonePos.x-pos.x) - (m_kZoneSize.x-m_kLastZoneSize.x)/2 - m_kLastZoneSize.x;
		pos.z += (m_kLastZonePos.z-pos.z) - (m_kZoneSize.z-m_kLastZoneSize.z)/2 - m_kLastZoneSize.z;
		break;
	case eBottomLeft:
		pos.z += (m_kLastZonePos.z-pos.z) - (m_kZoneSize.z-m_kLastZoneSize.z)/2 - m_kLastZoneSize.z;
		break;
	case eLeft:
		pos.z += - m_kLastZoneSize.z;
		break;
	case eRight:
		pos.x += (m_kLastZonePos.x-pos.x) - (m_kZoneSize.x-m_kLastZoneSize.x)/2 - m_kLastZoneSize.x;
		pos.z += - m_kLastZoneSize.z;
		break;
	case eNone:
		pos = m_kLastZonePos;
		break;
	}

	return pos;
}


