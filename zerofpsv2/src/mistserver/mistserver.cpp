/**
	\defgroup MistServer MistServer
	\ingroup MistLand

  MistServer is the Server of the game MistLands.
*/ 

#include <set> 
#include <algorithm>

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
#include "../zerofpsv2/engine/inputhandle.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../mcommon/si_dm.h"

MistServer g_kMistServer("MistServer", 0, 0, 0);

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

/*	case ZGM_SETFOCUS:
		g_kMistServer.SetViewPort(win->GetName());
		cout << "Set Focus to window: "  << win->GetName() << endl;
		break;*/

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
	m_bEditSun			= false;
	m_bSoloMode       = false;

	// Register Variables
	RegisterVariable("s_newlogins",				&m_AcceptNewLogins,			CSYS_BOOL);	

	// Register Commands
	Register_Cmd("new",FID_NEW);		
	Register_Cmd("load",FID_LOAD);		
	Register_Cmd("save",FID_SAVE);
	Register_Cmd("saveas",FID_SAVEAS);
	Register_Cmd("users",FID_USERS);		
	Register_Cmd("lo",FID_LOCALORDER);		
	Register_Cmd("lightmode", FID_LIGHTMODE);		
	Register_Cmd("editsun", FID_EDITSUN);		
	Register_Cmd("setcam", FID_SETCAM);		
	Register_Cmd("camlink", FID_CAMLINK);
	Register_Cmd("camsolo", FID_CAMSOLO);
	Register_Cmd("camgrid", FID_CAMGRID);
	Register_Cmd("selnone", FID_SELNONE);
	Register_Cmd("gridsize", FID_GRIDSIZE);
	Register_Cmd("gridsnap", FID_GRIDSNAP);
	Register_Cmd("camfollow", FID_CAMFOLLOW);
	Register_Cmd("camnofollow", FID_CAMNOFOLLOW);
	Register_Cmd("delsel", FID_DELETE);
	Register_Cmd("clone",	FID_CLONE);
	Register_Cmd("cut",		FID_CUT);
	Register_Cmd("copy",		FID_COPY);
	Register_Cmd("paste",	FID_PASTE);
	Register_Cmd("jiddra",	FID_TEST_JIDDRA);

	m_kDrawPos.Set(0,0,0);

	m_fHMInRadius  = 1;
	m_fHMOutRadius = 2;
	m_iEditLayer	= 1;
	m_fDelayTime   = 0.0;
	m_strWorldDir  = "";
   
	m_pkActiveCameraObject	= NULL;
	m_pkActiveCamera			= NULL;

	m_strActiveViewPort = "none";
} 


int MistServer::GetView(float x, float y)
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

bool MistServer::SetCamera(int iNum)
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

/*	if(iNum == -1)	return false;
	if(m_pkActiveCamera == m_pkCamera[iNum])	return false;

	if(m_pkActiveCamera) m_pkActiveCamera->m_bSelected = false;
	m_pkActiveCameraObject	= m_pkCameraObject[iNum];
	m_pkActiveCamera			= m_pkCamera[iNum];
	m_pkActiveCamera->m_bSelected = true;

	if(m_bSoloMode) 
	{
		m_pkCamera[0]->SetViewPort(0,0,0,0);	m_pkCamera[0]->m_bRender = false;
		m_pkCamera[1]->SetViewPort(0,0,0,0);	m_pkCamera[1]->m_bRender = false;
		m_pkCamera[2]->SetViewPort(0,0,0,0);	m_pkCamera[2]->m_bRender = false;
		m_pkCamera[3]->SetViewPort(0,0,0,0);	m_pkCamera[3]->m_bRender = false;
		m_pkActiveCamera->SetViewPort(0,0,1,1);
		m_pkActiveCamera->m_bRender = true;
	}

	return true;*/
}


bool MistServer::SetViewPort(const char* szVpName)
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
	m_pkActiveCameraObject	= m_pkObjectMan->GetObjectByNetWorkID( pkCam->m_iEntity );
	m_pkActiveCamera			= pkCam;
	m_pkActiveCamera->SetSelected(true);

	m_strActiveViewPort  = szVpName;

	return true;
}

void MistServer::CreateEditCameras()
{
	for(int i=0; i<4; i++) 
	{
		m_pkCameraObject[i] = m_pkObjectMan->CreateObjectFromScript("data/script/objects/t_camedit.lua");
		if(m_pkCameraObject[i]) 
		{
			m_pkCameraObject[i]->SetParent( m_pkObjectMan->GetWorldObject() );
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


void MistServer::OnInit() 
{
	m_pkConsole->Printf(" MistServer (mistland dedicated server)");
	m_pkConsole->Printf("--------------------------------");
	m_pkConsole->Printf("");

	Init();

	//init dm script interface (register script functions for gameplay)
	DMLua::Init(m_pkObjectMan,m_pkScript,m_pkGuiMan);

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

	m_pkZShader->SetForceLighting(LIGHT_ALWAYS_OFF);	
	
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
	GuiAppLua::Init(&g_kMistServer, m_pkScript);

	// Load default texture and create default font and menu (NULL = No menu).
	InitGui(m_pkScript, 
		"data/textures/text/ms_sans_serif8.bmp", 
		"data/script/gui/defskins.lua", 
		"data/script/gui/menu.txt", true);
	
	// Create startup GUI for the the server from script.
	SetupGuiEnviroment();

	//setup caption
	SetTitle("MistServer");
	// hide cursor
	m_pkInput->ShowCursor(true);

//	m_pkInputHandle->ToggleGrab();
//	m_pkPlayerDB->GetLoginCharacters(string("user"));

	// Setup the Edit Sun that are used for simple lightning in the editor.
	m_kSun.kRot = Vector3(1,2,1);
	m_kSun.kDiffuse=Vector4(1,1,1,0);
	m_kSun.kAmbient=Vector4(0.2,0.2,0.2,0);
	m_kSun.iType=DIRECTIONAL_LIGHT;			
	m_kSun.iPriority=10;
	m_kSun.fConst_Atten=1;
	m_kSun.fLinear_Atten=0;
	m_kSun.fQuadratic_Atten=0;
}


void MistServer::SetupGuiEnviroment()
{
	// Create from script.
	LoadGuiFromScript(m_pkScript, "data/script/gui/server.lua");
	GetWnd("worktab")->Hide();

	m_pkGui->SetCursor( 0,0, m_pkTexMan->Load("data/textures/gui/blue_cursor.bmp", 0),
		m_pkTexMan->Load("data/textures/gui/blue_cursor_a.bmp", 0), 32, 32);
	
	// Fill zone- and object treebox.
	BuildFileTree("ZoneModelTree", "data/mad/zones", ".mad");
	BuildFileTree("ObjectTree", "data/script/objects", ".lua");

	// Fill enviroment listbox.
	vector<string> vkFileNames;
	m_pkZFVFileSystem->ListDir(&vkFileNames, "/data/enviroments", false);
	for(unsigned int i=0; i<vkFileNames.size(); i++)
		AddListItem("EnviromentPresetList", (char*) vkFileNames[i].c_str());

	m_pkGui->GetToolTip()->AddToolTip(GetWnd("ToggleLight"),"Light");
	m_pkGui->GetToolTip()->AddToolTip(GetWnd("OpenWorkTabButton"),"Worktab");
	m_pkGui->GetToolTip()->AddToolTip(GetWnd("RotateZoneModellButton"),"Rotate");
}


void MistServer::RegisterResources()
{
	m_pkResourceDB->RegisterResource( string(".env"), Create__EnvSetting	);
}


void MistServer::RegisterPropertys()
{
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

	m_pkRender->DrawBillboard(m_pkFps->GetCam()->GetModelViewMatrix(),kCenterPos,1,
		m_pkTexMan->Load("../data/textures/pointer.tga",T_NOMIPMAPPING));	

	Vector3				kVertex;
	vector<Vector3>	kVertexList;

	kCenterPos.y = 0;
	Vector3 kPos;

	for(int i=0; i<360; i+=(int)12.25) 
	{
		kVertex.x = float( cos(DegToRad( float(i) )) * fInRadius );
		kVertex.z = float( sin(DegToRad( float(i) )) * fInRadius );
		kVertex.y = pkHmap->Height(kCenterPos.x+kVertex.x,kCenterPos.z + kVertex.z) + 0.01;
		kVertex += kCenterPos;
		kVertexList.push_back(kVertex);
	}

	m_pkRender->DrawCircle(kVertexList, m_pkRender->GetEditColor("hmapbrush") );

	kVertexList.clear();
	for(int i=0; i<360; i+=(int)12.25) 
	{
		kVertex.x = float( cos(DegToRad( float(i) )) * fOutRadius );
		kVertex.z = float( sin(DegToRad( float(i) )) * fOutRadius );
		kVertex.y = pkHmap->Height(kCenterPos.x+kVertex.x,kCenterPos.z + kVertex.z) + 0.01;
		kVertex += kCenterPos;
		kVertexList.push_back(kVertex);
	}

	m_pkRender->DrawCircle(kVertexList, m_pkRender->GetEditColor("hmapbrush") );
}


void MistServer::DrawSelectedEntity()
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


void MistServer::Select_Toggle(int iId, bool bMultiSelect)
{
	if(!bMultiSelect && m_iCurrentObject != iId)		Select_None();
	
	if(m_SelectedEntitys.find(iId) == m_SelectedEntitys.end())
	{
		Select_Add(iId);
		m_iCurrentObject = iId;
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
}


void MistServer::DeleteSelected()
{
	if(m_SelectedEntitys.size() == 0)	return;

	cout << "Delete Selected: ID, Type, Name" << endl;
	for(set<int>::iterator itEntity = m_SelectedEntitys.begin(); itEntity != m_SelectedEntitys.end(); itEntity++ ) 
	{
		Entity* pkEntity = m_pkObjectMan->GetObjectByNetWorkID((*itEntity));
		if(!pkEntity)	continue;
	
		cout << " " << pkEntity->GetEntityID() << " - '" << pkEntity->GetType() << "' - '" << pkEntity->GetName() << "'" <<endl;
		if(pkEntity->GetName() == string("ZoneObject"))
		{
			int iZoneID = m_pkObjectMan->GetZoneIndex( pkEntity->GetEntityID() );
			m_pkObjectMan->DeleteZone(iZoneID);
			cout << "Delete zone " << iZoneID << endl;
		}
		else
			m_pkObjectMan->Delete(pkEntity->GetEntityID());		
	}

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


void MistServer::OnIdle()
{	
	m_pkFps->SetCamera(m_pkActiveCamera);		
	m_pkFps->GetCam()->ClearViewPort();	

	if(m_pkGui->m_bHandledMouse == false)
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

	PathTest();
}


void MistServer::RenderInterface(void)
{
	DrawSelectedEntity();
	if(m_iEditMode == EDIT_HMAP) {
		HeightMap* pkMap = SetPointer();
		DrawHMEditMarker(pkMap, m_kDrawPos, m_fHMInRadius,m_fHMOutRadius);
		}

	if(m_iEditMode == EDIT_ZONES)		DrawZoneMarker(m_kZoneMarkerPos);
	if(m_iEditMode == EDIT_OBJECTS)	DrawCrossMarker(m_kObjectMarkerPos);		
}


void MistServer::OnSystem()
{
	HandleOrders();
}


HeightMap* MistServer::SetPointer()
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


void MistServer::HMModifyCommand(float fSize)
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


// Handles input for EditMode Terrain.
void MistServer::Input_EditTerrain()
{
	if(m_pkInputHandle->VKIsDown("inrad+"))		m_fHMInRadius += 1 * m_pkFps->m_pkObjectMan->GetSimDelta();
	if(m_pkInputHandle->VKIsDown("inrad-"))		m_fHMInRadius -= 1 * m_pkFps->m_pkObjectMan->GetSimDelta();
	if(m_pkInputHandle->VKIsDown("outrad+"))		m_fHMOutRadius += 1 * m_pkFps->m_pkObjectMan->GetSimDelta();
	if(m_pkInputHandle->VKIsDown("outrad-"))		m_fHMOutRadius -= 1 * m_pkFps->m_pkObjectMan->GetSimDelta();
	if(m_fHMInRadius > m_fHMOutRadius)
		m_fHMInRadius = m_fHMOutRadius;

	if(m_pkInputHandle->VKIsDown("hmraise"))		HMModifyCommand(5); 
	if(m_pkInputHandle->VKIsDown("hmlower"))		HMModifyCommand(-5);
	if(m_pkInputHandle->VKIsDown("hmsm") ) 		HMModifyCommand(0.0); 

	if(m_pkInputHandle->VKIsDown("hmpaint")) 
	{
		for(set<int>::iterator itEntity = m_SelectedEntitys.begin(); itEntity != m_SelectedEntitys.end(); itEntity++ ) 
		{
			Entity* pkEntity = m_pkObjectMan->GetObjectByNetWorkID((*itEntity));
			if(!pkEntity)			continue;
			P_HMRP2* hmrp = dynamic_cast<P_HMRP2*>(pkEntity->GetProperty("P_HMRP2"));
			if(hmrp == NULL)		continue;
			Vector3 kLocalOffset = m_kDrawPos - hmrp->m_pkHeightMap->m_kCornerPos;
			hmrp->m_pkHeightMap->DrawMask(m_kDrawPos, m_iEditLayer,m_fHMInRadius,255,255,255,1);
/*			if(m_iEditLayer == 1)
				hmrp->m_pkHeightMap->DrawVisible(kLocalOffset, false);
			else
				hmrp->m_pkHeightMap->DrawVisible(kLocalOffset, true);*/
			}
		}

	if(m_pkInputHandle->Pressed(KEY_1)) m_iEditLayer = 1;		
	if(m_pkInputHandle->Pressed(KEY_2)) m_iEditLayer = 2;			
	if(m_pkInputHandle->Pressed(KEY_3)) m_iEditLayer = 3;			

	if(m_pkInputHandle->Pressed(KEY_4))
	{
		Entity* pkEntity = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);
		if(pkEntity)
		{
			P_HMRP2* hmrp = dynamic_cast<P_HMRP2*>(pkEntity->GetProperty("P_HMRP2"));
			if(hmrp)
            hmrp->m_pkHeightMap->Invert();			
		}
	}
}


// Handles input for EditMode Zones.
void MistServer::Input_EditZone()
{
	if(m_pkInputHandle->Pressed(MOUSELEFT) && !DelayCommand())
	{
		AddZone(m_kZoneMarkerPos, m_kZoneSize, m_strActiveZoneName);	
	}
	
	if(m_pkInputHandle->VKIsDown("remove"))	DeleteSelected();
		
	if(m_pkInputHandle->VKIsDown("rotate") && !DelayCommand())
	{
		m_iCurrentMarkedZone = m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
		
		/*
		ZoneData* zd = pkObjectMan->GetZoneData(m_iCurrentMarkedZone);
		if(zd)
			if(zd->m_bUnderContruction)*/
		RotateActiveZoneObject();
	}
	
	/*
	if(m_pkInputHandle->VKIsDown("buildmodeon") && !DelayCommand())
	{
		m_iCurrentMarkedZone = m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
		m_pkObjectMan->SetUnderConstruction(m_iCurrentMarkedZone);
	}
	
	if(m_pkInputHandle->VKIsDown("buildmodeoff") && !DelayCommand())
	{
		m_iCurrentMarkedZone = m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
		m_pkObjectMan->CommitZone(m_iCurrentMarkedZone);
	}	
	*/
	if(m_pkInputHandle->VKIsDown("selectzone") && !DelayCommand())
	{	
		m_iCurrentMarkedZone =  m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
		ZoneData* pkData = m_pkObjectMan->GetZoneData(m_iCurrentMarkedZone);
		if(pkData && pkData->m_pkZone)
			Select_Toggle(pkData->m_pkZone->GetEntityID(), m_pkInputHandle->Pressed(KEY_LSHIFT));
	}

	if(m_pkInputHandle->Pressed(KEY_1)) m_kZoneSize.Set(4,4,4);
	if(m_pkInputHandle->Pressed(KEY_2)) m_kZoneSize.Set(8,8,8);
	if(m_pkInputHandle->Pressed(KEY_3)) m_kZoneSize.Set(16,16,16);	
	if(m_pkInputHandle->Pressed(KEY_4)) m_kZoneSize.Set(32,16,32);	
	if(m_pkInputHandle->Pressed(KEY_5)) m_kZoneSize.Set(64,16,64);		
	if(m_pkInputHandle->Pressed(KEY_6)) m_kZoneSize.Set(1024,32,1024);		
}


// Handles input for EditMode Object.
void MistServer::Input_EditObject(float fMouseX, float fMouseY)
{
	if(m_pkInputHandle->VKIsDown("copy"))	EditRunCommand(FID_COPY);
	if(m_pkInputHandle->VKIsDown("paste"))	EditRunCommand(FID_PASTE);

	if(m_pkInputHandle->Pressed(MOUSELEFT) && !DelayCommand())
	{
		m_pkObjectMan->CreateObjectFromScriptInZone(
			m_strActiveObjectName.c_str(), m_kObjectMarkerPos);

		cout << "Spawning " << m_strActiveObjectName.c_str() << endl;
	}
	
	if(m_pkInputHandle->VKIsDown("selectzone") && !DelayCommand())
	{		
		Entity* pkObj =  GetTargetObject();
		if(pkObj)
			Select_Toggle(pkObj->GetEntityID(), m_pkInputHandle->Pressed(KEY_LSHIFT));
	}
	
	//remove			
	if(m_pkInputHandle->VKIsDown("remove"))	DeleteSelected();

	Entity* pkObj = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);								
	if(!pkObj)
		return;		

	//return if its a static object
	if(pkObj->GetParent()->GetName() == "StaticEntity")
		return;
		
		
	//hack for collisions test
	if(m_pkInputHandle->VKIsDown("setvel"))		pkObj->SetVel(Vector3(1,0,0));

	// Move Selected Entity
	Vector3 kMove(0,0,0);
	Vector3 kClick(0,0,0);


	if(m_pkInputHandle->VKIsDown("reposent")) 
	{
		kMove = pkObj->GetLocalPosV();
		kMove = m_pkActiveCamera->GetPos() + Get3DMousePos(true) * 2;
		
		if(m_pkActiveCamera->GetViewMode() != Camera::CAMMODE_PERSP) {
			// In Ortho mode we keep the old coo for the axis that go into the screen.
			Vector3 kAxisX, kAxisY, kAxisZ;
			kAxisX = m_pkActiveCamera->GetOrthoAxisX();
			kAxisY = m_pkActiveCamera->GetOrthoAxisY();
			kAxisZ = m_pkActiveCamera->GetOrthoAxisZ();
			kAxisX.Abs();
			kAxisY.Abs();
			kAxisZ.Abs();

			kMove = kMove.PEP(kAxisX) + 
				kMove.PEP(kAxisY) + 
				pkObj->GetLocalPosV().PEP(kAxisZ);
			}

		pkObj->SetLocalPosV( m_pkActiveCamera->SnapToGrid(kMove) );
	}
	else 
	{
		kMove.Set(0,0,0);
		if(m_pkInputHandle->VKIsDown("moveleft"))		kMove += Vector3(-1 * m_pkFps->GetFrameTime(),0,0);			
		if(m_pkInputHandle->VKIsDown("moveright"))	kMove += Vector3(1 * m_pkFps->GetFrameTime(),0,0);			
		if(m_pkInputHandle->VKIsDown("movefrw"))		kMove += Vector3(0,0,-1 * m_pkFps->GetFrameTime());			
		if(m_pkInputHandle->VKIsDown("moveback"))		kMove += Vector3(0,0,1 * m_pkFps->GetFrameTime());			
		if(m_pkInputHandle->VKIsDown("moveup"))		kMove += Vector3(0,1 * m_pkFps->GetFrameTime(),0);			
		if(m_pkInputHandle->VKIsDown("movedown"))		kMove += Vector3(0,-1 * m_pkFps->GetFrameTime(),0);

		if(m_pkInputHandle->VKIsDown("moveent")) 
		{
			kMove += m_pkActiveCamera->GetOrthoMove( Vector3(fMouseX, fMouseY,0) );
		}

		pkObj->SetLocalPosV(pkObj->GetLocalPosV() + kMove);
	}

/*	if(m_pkInputHandle->VKIsDown("rotent")) 
	{
		pkObj->RotateLocalRotV( Vector3(0, fMouseX,0));
	}*/

	// Rotate Selected Entity
	if(m_pkInputHandle->VKIsDown("rotx+"))			pkObj->RotateLocalRotV(Vector3(100*m_pkFps->GetFrameTime(),0,0));			
	if(m_pkInputHandle->VKIsDown("rotx-"))			pkObj->RotateLocalRotV(Vector3(-100*m_pkFps->GetFrameTime(),0,0));			
	if(m_pkInputHandle->VKIsDown("roty+"))			pkObj->RotateLocalRotV(Vector3(0,100*m_pkFps->GetFrameTime(),0));			
	if(m_pkInputHandle->VKIsDown("roty-"))			pkObj->RotateLocalRotV(Vector3(0,-100*m_pkFps->GetFrameTime(),0));			
	if(m_pkInputHandle->VKIsDown("rotz+"))			pkObj->RotateLocalRotV(Vector3(0,0,100*m_pkFps->GetFrameTime()));			
	if(m_pkInputHandle->VKIsDown("rotz-"))			pkObj->RotateLocalRotV(Vector3(0,0,-100*m_pkFps->GetFrameTime()));			
}

void MistServer::Input_Camera(float fMouseX, float fMouseY)
{
//	if(m_pkInputHandle->Pressed(KEY_Z))		SetCamera(0);
//	if(m_pkInputHandle->Pressed(KEY_X))		SetCamera(1);
//	if(m_pkInputHandle->Pressed(KEY_C))		SetCamera(0);
//	if(m_pkInputHandle->Pressed(KEY_V))		SetCamera(1);

	if(m_pkInputHandle->Pressed(KEY_Z))		SetViewPort("vp1");
	if(m_pkInputHandle->Pressed(KEY_X))		SetViewPort("vp2");
	if(m_pkInputHandle->Pressed(KEY_C))		SetViewPort("vp3");
	if(m_pkInputHandle->Pressed(KEY_V))		SetViewPort("vp4");



	if(m_pkInputHandle->VKIsDown("slow")){
		m_CamMoveSpeed *= 0.25;
	}

	float fSpeedScale = m_pkFps->GetFrameTime()*m_CamMoveSpeed;

	if(m_pkActiveCamera->GetViewMode() == Camera::CAMMODE_PERSP) 
	{

		Vector3 newpos = m_pkActiveCameraObject->GetLocalPosV();
		
		Matrix4 kRm;
		kRm = m_pkActiveCameraObject->GetLocalRotM();

		kRm.Transponse();

		
		Vector3 xv = kRm.GetAxis(0);
		Vector3 zv = kRm.GetAxis(2);

		xv.y = 0;
		zv.y = 0;
		
		xv.Normalize();
		zv.Normalize();

		if(m_pkInputHandle->VKIsDown("right"))		newpos += xv * fSpeedScale;		
		if(m_pkInputHandle->VKIsDown("left"))		newpos += xv * -fSpeedScale;		
		if(m_pkInputHandle->VKIsDown("forward"))	newpos += zv * -fSpeedScale;
		if(m_pkInputHandle->VKIsDown("back"))		newpos += zv * fSpeedScale;	

		if(m_pkInputHandle->VKIsDown("down"))		newpos.y += fSpeedScale;
		if(m_pkInputHandle->VKIsDown("up"))			newpos.y -= fSpeedScale;
				

		Vector3 rot;
		rot.Set(float(-fMouseY / 5.0),float(-fMouseX / 5.0),0);

		kRm.Transponse();		
		kRm.Rotate(rot);
		kRm.Transponse();		
		Vector3 bla = Vector3(0,0,1);
		bla = kRm.VectorTransform(bla);
		kRm.LookDir(bla,Vector3(0,1,0));
		
		m_pkActiveCameraObject->SetLocalPosV(newpos);		
		if(m_pkInputHandle->VKIsDown("pancam"))
		{
			SDL_ShowCursor(false);
			m_pkActiveCameraObject->SetLocalRotM(kRm);	
		}
		else
			SDL_ShowCursor(true);
	}

	else 
	{
		Vector3 kMove = Vector3::ZERO;

		if(m_pkInputHandle->VKIsDown("forward"))	kMove.y += fSpeedScale;
		if(m_pkInputHandle->VKIsDown("back"))		kMove.y -= fSpeedScale;
		if(m_pkInputHandle->VKIsDown("right"))		kMove.x += fSpeedScale;
		if(m_pkInputHandle->VKIsDown("left"))		kMove.x -= fSpeedScale;	
		
		if(m_pkInputHandle->VKIsDown("down"))		m_pkActiveCamera->OrthoZoom(0.9);
		if(m_pkInputHandle->VKIsDown("up"))			m_pkActiveCamera->OrthoZoom(1.1);

		if(m_pkInputHandle->VKIsDown("pancam"))
		{
			kMove += Vector3(-fMouseX * fSpeedScale,fMouseY * fSpeedScale,0);
		}


		if(m_pkCameraObject[1]->GetParent() == m_pkCameraObject[0])
		{
			// If Cameras are linked.
			kMove = m_pkActiveCamera->GetOrthoMove(kMove);
			//kMove.Print();
			//cout << endl;
			m_pkCameraObject[0]->SetLocalPosV(m_pkCameraObject[0]->GetLocalPosV() + kMove);
		}
		else 
		{
			P_Camera* pkCam = dynamic_cast<P_Camera*>(m_pkActiveCameraObject->GetProperty("P_Camera"));
			if(pkCam)
				pkCam->OrthoMove(kMove);
		}
	}
}

void MistServer::Input()
{
	// *** HANDE Quueued Keys.
	int iPressedKey = m_pkInputHandle->GetQueuedKey().m_iKey;

	switch(iPressedKey)
	{
	case KEY_F9:
		printf("smurf\n");
		break;
	}

	//set speed depending on edit mode
	if(m_iEditMode == EDIT_HMAP)		m_CamMoveSpeed = 20;
	if(m_iEditMode == EDIT_ZONES)		m_CamMoveSpeed = 20;
	if(m_iEditMode == EDIT_OBJECTS)	m_CamMoveSpeed = 5;
	
	
	int x = 0;
	int z = 0;		
	m_pkInputHandle->RelMouseXY(x,z);	

	// First see if we clicked to change view port.
	if(m_pkInputHandle->Pressed(MOUSELEFT) || m_pkInputHandle->Pressed(KEY_0))
	{
		int mx,my;
		m_pkInputHandle->SDLMouseXY(mx,my);
		my = m_iHeight - my; 
		int iClickedViewPort = GetView(mx, my);
		if(SetCamera(iClickedViewPort))
			m_fDelayTime = m_pkFps->GetEngineTime() + 0.5;
	}

	if(m_pkInputHandle->VKIsDown("makeland")) {
		Entity* pkObj = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);								
		if(!pkObj)
			return;		
		//int id = m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
		//ZoneData* z = m_pkObjectMan->GetZoneData(id);
		//m_pkFps->AddHMProperty(z, z->m_iZoneObjectID,z->m_kSize);
		m_pkFps->AddHMProperty(pkObj, pkObj->GetEntityID(),m_kZoneSize);
	}  

	if(m_pkInputHandle->Pressed(KEY_F6)) 
	{
		Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);

		//m_iCurrentMarkedZone =  m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
		//ZoneData* pkData = m_pkObjectMan->GetZoneData(m_iCurrentMarkedZone);
		//if(pkData && pkData->m_pkZone) 
		//{
			pkEnt->AddProperty("P_PfMesh");
			P_PfMesh* pkMesh = (P_PfMesh*)pkEnt->GetProperty("P_PfMesh");
			pkMesh->AutoMakeNaviMesh();
			//P_HMRP2* pkHmap = (P_HMRP2*)pkEnt->GetProperty("P_HMRP2");
			//if(pkHmap)
			//	pkMesh->SetHmap(pkHmap);
		//}
	}

	Vector3 kMove(0,0,0);
//	Vector3 kRotate(0,0,0);

	if(m_pkInputHandle->Pressed(KEY_H))	m_pkRender->DumpGLState("zzz.txt");			

	if(m_pkInputHandle->Pressed(KEY_8))	assert(0);
	if(m_pkInputHandle->Pressed(KEY_9))	ZFAssert(0, "Fet med test");


	if(m_pkCameraObject)	
	{	
		Input_Camera(x,z);
	
		if(m_pkInputHandle->VKIsDown("modezone"))			m_iEditMode = EDIT_ZONES;
		if(m_pkInputHandle->VKIsDown("modeobj"))			m_iEditMode = EDIT_OBJECTS;		
		if(m_pkInputHandle->VKIsDown("modehmvertex"))		m_iEditMode = EDIT_HMAP;		

		if(m_pkInputHandle->VKIsDown("lighton"))			m_pkZShader->SetForceLighting(LIGHT_ALWAYS_ON);	
		if(m_pkInputHandle->VKIsDown("lightoff"))			m_pkZShader->SetForceLighting(LIGHT_ALWAYS_OFF);
		if(m_pkInputHandle->VKIsDown("lightstd"))			m_pkZShader->SetForceLighting(LIGHT_MATERIAL);
	
		if(m_iEditMode == EDIT_HMAP)				Input_EditTerrain();
		if(m_iEditMode == EDIT_ZONES)				Input_EditZone();
		if(m_iEditMode == EDIT_OBJECTS)			Input_EditObject(x,z);

		if(m_pkInputHandle->VKIsDown("solo"))				SoloToggleView();
	}
};

void MistServer::OnHud(void)
{
	m_pkFps->DevPrintf("common","Active Propertys: %d",m_pkObjectMan->GetActivePropertys());	
	m_pkFps->DevPrintf("common", "Fps: %f",m_pkFps->m_fFps);	
	m_pkFps->DevPrintf("common","Avrage Fps: %f",m_pkFps->m_fAvrageFps);			
		
	if(m_pkActiveCamera) {
		m_pkFps->DevPrintf("editor","Grid Size: %f", m_pkActiveCamera->m_fGridSpace);			
		m_pkFps->DevPrintf("editor","Grid Snap: %i", m_pkActiveCamera->m_bGridSnap);			
		m_pkFps->DevPrintf("editor","View: %s", m_pkActiveCamera->GetName().c_str());			
		/*m_pkRender->DrawAABB(m_pkActiveCamera->GetViewPortCorner(),
			m_pkActiveCamera->GetViewPortCorner() + m_pkActiveCamera->GetViewPortSize(),
			Vector3(1,1,1),1);*/
		}

	m_pkFps->m_bGuiMode = false;
	m_pkFps->ToggleGui();


}

bool MistServer::DelayCommand()
{
	if(m_pkFps->GetEngineTime() < m_fDelayTime)
		return true;

	m_fDelayTime = m_pkFps->GetEngineTime() + 0.5;
	return false;
}

void MistServer::EditRunCommand(FuncId_e eEditCmd)
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


void MistServer::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	ClientOrder kOrder;

	unsigned int i;
	vector<string>	kUsers;
	int iMode;
	float fTest;
	string strNewTitle;

	switch(cmdid) {
		case FID_NEW:
			m_pkObjectMan->Clear();
			GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);
			m_strWorldDir = "";
			SetTitle("MistServer");
			break;
		
		case FID_LOAD:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				m_pkConsole->Printf("load [worlddir]");
				break;				
			}
			
			if(!m_pkObjectMan->LoadWorld(kCommand->m_kSplitCommand[1]))
			{
				cout<<"Error loading world"<<endl;
				break;
			}				
			
			// Set the title to include the world name.
			m_strWorldDir = kCommand->m_kSplitCommand[1];
			strNewTitle = "MistServer - " + m_strWorldDir;
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
			GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);			
			
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
			strNewTitle = "MistServer - " + m_strWorldDir;
			SetTitle(strNewTitle);

/*			cout<<"saving world:"<<endl;
			m_pkObjectMan->ForceSave();
			m_pkObjectMan->SaveZones();			
			cout<<"saved"<<endl;
*/			
			break;

	
		case FID_USERS:
			kUsers = m_pkPlayerDB->GetUsers();
			for(i=0; i<kUsers.size(); i++) {
				cout << "User: " << kUsers[i] << endl;
				}
			break;		

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
			
			fTest =  atof( kCommand->m_kSplitCommand[1].c_str());
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

void MistServer::SoloToggleView()
{
	if(DelayCommand())	return;
	
	Camera* pkCam = GetWnd(m_strActiveViewPort)->GetRenderTarget();

	if(m_bSoloMode) 
	{
		m_pkFps->RemoveRenderTarget(NULL);
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
			m_pkFps->SetRenderTarget(pkCam);
			pkCam->m_bForceFullScreen = true;
		}
	}
}

void MistServer::CamFollow(bool bFollowMode)
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
		Entity* pkObj = m_pkObjectMan->GetObjectByNetWorkID( m_pkActiveCamera->m_iEntity );		
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
	CreateEditCameras();

	// Create and setup the Env on the server.
	P_Enviroment* pe = (P_Enviroment*)m_pkCameraObject[0]->AddProperty("P_Enviroment");
	pe->SetEnable(true);		
	pe->SetEnviroment("data/enviroments/server.env");

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

	SoloToggleView();
	m_fDelayTime = m_pkFps->GetEngineTime();
	SoloToggleView();
	GetWnd("vp1")->SetZValue(0);
	GetWnd("vp2")->SetZValue(0);
	GetWnd("vp3")->SetZValue(0);
	GetWnd("vp4")->SetZValue(0);
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

/*	Return 3D postion of mouse in world. */
Vector3 MistServer::Get3DMouseDir(bool bMouse)
{
	Vector3 dir;
	float x,y;		
	
	//screen propotions
	float xp=4;
	float yp=3;

	Vector3 kViewSize, kViewCorner;
	kViewSize = m_pkActiveCamera->GetViewPortSize();
	kViewCorner = m_pkActiveCamera->GetViewPortCorner();
	
	if(bMouse)
	{
		// Zeb was here! Nu kör vi med operativsystemets egna snabba musmarkör
		// alltså måste vi använda den position vi får därifrån.
		//	m_pkInputHandle->UnitMouseXY(x,y);
		//x = -0.5f + (float) m_pkInputHandle->m_iSDLMouseX / (float) m_pkApp->m_iWidth;
		//y = -0.5f + (float) m_pkInputHandle->m_iSDLMouseY / (float) m_pkApp->m_iHeight;
		int mx;		
		int my;
		
		m_pkInputHandle->SDLMouseXY(mx,my);
		
		x = -0.5f + (float) (mx - kViewCorner.x) / (float) kViewSize.x;
		y = -0.5f + (float) ((m_pkApp->m_iHeight - my) - kViewCorner.y) / (float) kViewSize.y;

		if(m_pkActiveCamera->GetViewMode() == Camera::CAMMODE_PERSP)
		{
			dir.Set(x*xp,y*yp,-1.5);
			dir.Normalize();
		}
		else
		{
			dir.Set(0,0,-1);
			//dir.Normalize();
			//return dir;
		}
	}
	else
	{
		dir.Set(0,0,-1.5);
		dir.Normalize();	
	}
	
	Matrix4 rm = m_pkActiveCamera->GetRotM();
	rm.Transponse();
	dir = rm.VectorTransform(dir);
	
	return dir;
}

/*	Returns 3D dir of mouse click in world. */
Vector3 MistServer::Get3DMousePos(bool m_bMouse=true)
{
	Vector3 dir;
	float x,y;		
	
	//screen propotions
	float xp=4;
	float yp=3;

	Vector3 kViewSize, kViewCorner;
	kViewSize = m_pkActiveCamera->GetViewPortSize();
	kViewCorner = m_pkActiveCamera->GetViewPortCorner();

	if(m_bMouse)
	{
		// Zeb was here! Nu kör vi med operativsystemets egna snabba musmarkör
		// alltså måste vi använda den position vi får därifrån.
		//	m_pkInputHandle->UnitMouseXY(x,y);
		/*
			x = -0.5f + (float) m_pkInputHandle->m_iSDLMouseX / (float) m_pkApp->m_iWidth;
			y = -0.5f + (float) m_pkInputHandle->m_iSDLMouseY / (float) m_pkApp->m_iHeight;
		*/
		int mx;		
		int my;
		
		m_pkInputHandle->SDLMouseXY(mx,my);		
		
		x = -0.5f + (float) (mx - kViewCorner.x) / (float) kViewSize.x;
		y = -0.5f + (float) ((m_pkApp->m_iHeight - my) - kViewCorner.y) / (float) kViewSize.y;
		
		//cout << "ViewCorner: " << kViewCorner.x << ", " << kViewCorner.y << endl;
		//cout << "kViewSize: " << kViewSize.x << ", " << kViewSize.y << endl;

		if(m_pkActiveCamera->GetViewMode() == Camera::CAMMODE_PERSP) 
		{
			dir.Set(x*xp,y*yp,-1.5);
			dir.Normalize();
		}
		else 
		{
			dir.x = x* m_pkActiveCamera->GetOrthoSize().x;
			dir.y = y* m_pkActiveCamera->GetOrthoSize().y;
			dir.z = -1.5; 
//			cout << "Cam XY: " << dir.x << "," << dir.y << endl;
		}
	}
	else
	{
		dir.Set(0,0,-1.5);
		dir.Normalize();	
	}
	
	Matrix4 rm = m_pkActiveCamera->GetRotM();
	rm.Transponse();
	dir = rm.VectorTransform(dir);
	
	return dir;
}

Entity* MistServer::GetTargetObject()
{
	Vector3 start	= m_pkFps->GetCam()->GetPos() + Get3DMousePos(true)*2;
	Vector3 dir		= Get3DMouseDir(true);
//	dir.Set(0,1,0);

	start.Print();
	cout << " - "; 
	dir.Print();
	cout << endl; 

	vector<Entity*> kObjects;
	kObjects.clear();
	
	m_pkObjectMan->TestLine(&kObjects,start,dir);
	
	
	float closest = 999999999;
	Entity* pkClosest = NULL;	
	for(unsigned int i=0;i<kObjects.size();i++)
	{
		cout << " Check " << kObjects[i]->GetEntityID() << " - '" << kObjects[i]->GetType() << "' - '" << kObjects[i]->GetName() << "'" <<endl;

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
	m_pkRender->DrawAABB(kPos-bla,kPos+bla, m_pkRender->GetEditColor( "zonemarker" ));
}


void MistServer::DrawCrossMarker(Vector3 kPos)
{
	// Set Color here.
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
	m_kObjectMarkerPos = /*m_pkFps->GetCam()*/ m_pkActiveCamera->GetPos() + Get3DMousePos(true)*2;
}


void MistServer::OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd *pkMainWnd)
{
	ZGuiWnd* pkWndClicked = NULL;

	if(GetWndType(pkMainWnd) == Menu)
	{
		list<ZGuiWnd*> kChilds;
		pkMainWnd->GetChildrens(kChilds); 

		list<ZGuiWnd*>::iterator it = kChilds.begin();
		for( ; it!=kChilds.end(); it++)
		{
			if((*it)->GetID() == iID)
			{
				pkWndClicked = (*it);
				break;
			}
		}
	}
	else
	{
		pkWndClicked = GetWnd(iID);
	}

	if(pkWndClicked)
	{
		string strMainWnd = pkMainWnd->GetName();

		string strWndClicked = pkWndClicked->GetName();

		string strParent = "null";

		if(pkWndClicked->GetParent())
			strParent = pkWndClicked->GetParent()->GetName();

		if(strMainWnd == "GuiMainWnd")
		{
			if(strWndClicked == "OpenWorkTabButton")
			{
				if( IsWndVisible("worktab") )
				{
					m_pkAudioSys->StartSound("/data/sound/close_window.wav",m_pkAudioSys->GetListnerPos());
					GetWnd("worktab")->Hide(); 
				}
				else 
				{
					m_pkAudioSys->StartSound("/data/sound/open_window.wav",m_pkAudioSys->GetListnerPos());
					GetWnd("worktab")->Show(); 
				}
			}
			else
			if(strWndClicked == "ToggleLight")
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
		else
		if(strMainWnd == "MainMenu")
		{
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
				if(strWndClicked == string(akSections[i].c_str()))
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
	//cout << "Setting Size " << x << ", " << y << ", "<< z << endl;
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
			
	return pkObject->GetEntityID();
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


