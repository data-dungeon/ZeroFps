#include "zerorts.h"
#include "userpanel.h"
#include "minimap.h"
//#include "../common/p_clientinput.h"

ZeroRTS g_kZeroRTS("ZeroRTS",1024,768,16);

static bool USERPANELPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) { 
	return g_kZeroRTS.m_pkUserPanel->DlgProc(win,msg,numparms,params); }

ZeroRTS::ZeroRTS(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth) 
{
	m_pkMiniMap = NULL;
	m_pkTestPath = NULL;
	m_pkClientInput = NULL;
	m_pkStart = Point(-1,-1);
	m_pkEnd = Point(-1,-1);
	m_iSelfObjectID = -1;

}

void ZeroRTS::OnInit() 
{
	pkConsole->Printf(" ZeroRTS");
	pkConsole->Printf("--------------------------------");
	pkConsole->Printf(" Does anything work yet?");

	Init();

	//run autoexec script
	if(!pkIni->ExecuteCommands("zerorts_autoexec.ini"))
		pkConsole->Printf("No game_autoexec.ini.ini found");
		
	//SetUp FOG render property
	pkObjectMan->GetWorldObject()->AddProperty("P_FogRender");
	m_pkFogRender = (P_FogRender*)pkObjectMan->GetWorldObject()->GetProperty("P_FogRender");
	
}

void ZeroRTS::Init()
{	
	//get heightmap pointer bös
	m_pkMap = pkLevelMan->GetHeightMap();	

	//register commmands bös
	g_ZFObjSys.Register_Cmd("load",FID_LOAD,this);		
	g_ZFObjSys.Register_Cmd("unload",FID_UNLOAD,this);			
	
	//damn "#¤(="%#( lighting fix bös
	glEnable(GL_LIGHTING );
	
	//initiate our camera bös
	m_pkCamera=new Camera(Vector3(0,CAMERA_HEIGHT,0),Vector3(70,0,0),90,1.333,0.25,250);	
	
	//disable zones modells bös
	pkLevelMan->SetVisibleZones(false);

	//register actions bös
	RegisterActions();

	//register property bös
	RegisterPropertys();
	
	//set clicktimer bös
	m_fClickTimer = pkFps->GetTicks();
	m_fClickDelay = 0.2;
	
	//set fog timer
	m_fFogTimer = pkFps->GetTicks();	
	
	//gui bös
	pkInput->SetCursorInputPos(m_iWidth/2, m_iHeight/2);
	pkGui->ShowCursor(true);
	SDL_ShowCursor(SDL_DISABLE);	

	pkFps->m_bGuiMode = false;
	pkFps->ToggleGui();

	m_pkGuiBuilder = new GuiBuilder(pkTexMan, pkGui);

	m_pkUserPanel = new UserPanel(this, USERPANELPROC);
	m_pkUserPanel->Create(100,100,NULL,NULL);
	m_pkUserPanel->Open();

	pkInput->ToggleGrab();
}

void ZeroRTS::RegisterActions()
{
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

void ZeroRTS::RegisterPropertys()
{
	pkPropertyFactory->Register("P_FogRender", Create_P_FogRender);	
	pkPropertyFactory->Register("P_RenderSelection", Create_P_RenderSelection);	
	pkPropertyFactory->Register("P_ClientUnit", Create_P_ClientUnit);
	pkPropertyFactory->Register("P_ServerInfo", Create_P_ServerInfo);		
	pkPropertyFactory->Register("P_ServerUnit", Create_P_ServerUnit);	
	pkPropertyFactory->Register("P_ClientInput", Create_P_ClientInput);	
}


void ZeroRTS::OnIdle() 
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	
			
	Input();
	

	Vector3 mpos = Get3DMousePos();
	
/*	glDisable(GL_LIGHTING);
		pkRender->Line(mpos-Vector3(1,0,0),mpos+Vector3(1,0,0));
		pkRender->Line(mpos-Vector3(0,1,0),mpos+Vector3(0,1,0));		
		pkRender->Line(mpos-Vector3(0,0,1),mpos+Vector3(0,0,1));				
	glEnable(GL_LIGHTING);
*/
//	m_pkFogRender->Explore(mpos.x,mpos.z,30);		

	// tassa
	if(m_pkMoveObject)
		MovePath(m_pkMoveObject);

	if(pkFps->m_bServerMode == false) {
		int iObjID = pkFps->GetClientObjectID();
		m_iSelfObjectID = iObjID;
		}

	Object* pkObj = pkObjectMan->GetObjectByNetWorkID( m_iSelfObjectID );
	if(pkObj) {
		pkObjectMan->OwnerShip_Take( pkObj );
		pkObj->SetPos(pkFps->GetCam()->GetPos());
		pkObj->SetPos(pkFps->GetCam()->GetPos());
		}

	if(m_pkMiniMap)
		m_pkMiniMap->Draw(m_pkCamera, pkGui, m_pkFogRender, pkRender); 
}

void ZeroRTS::OnSystem() 
{
	//update fog once every 1 sec
	if(pkFps->GetTicks() - m_fFogTimer > 0.5)
	{			
		Explore();		
		m_fFogTimer = pkFps->GetTicks();
	}
	
	//setup client
	if(pkFps->m_bClientMode)
	{
		if(m_pkClientInput == NULL)		
		{
			if(m_iSelfObjectID != -1)
			{
				m_pkClientInput = (P_ClientInput*)pkObjectMan->GetObjectByNetWorkID(m_iSelfObjectID)->GetProperty("P_ClientInput");
				
				if(m_pkClientInput != NULL)
				{
					cout<<"Found client input property"<<endl;					
					cout<<"Assuming that server has started,executing client init"<<endl;
					
					//run client init
					ClientInit();
				}
			}
		}
	};
	
	//if server is running
	if(pkFps->m_bServerMode)	
		HandleOrders();
}

void ZeroRTS::Input()
{
	// have clicked on userpanel, skip (gui bös)
	if(m_pkUserPanel->Click())
		return;

	int mx,my;
	pkInput->MouseXY(mx,my);

	static int s_iCursorTex=-1; 
	static int s_iCursorTex_a=-1;
	int iNewCursorTex=0;
	int iNewCursorTex_a=0;

	enum MOUSEDIR {
		Left, 
		Right, 
		Up, 
		Down, 
		Nothing
	};
	
	MOUSEDIR eMouseDir; 
	eMouseDir = Nothing;

	if(mx == 0) {
		iNewCursorTex = pkTexMan->Load("file:../data/textures/cursor_l.bmp", 0);
		iNewCursorTex_a = pkTexMan->Load("file:../data/textures/cursor_l_a.bmp", 0);
		eMouseDir = Left;} 
	else if(mx >= m_iWidth-32) {
		iNewCursorTex = pkTexMan->Load("file:../data/textures/cursor_r.bmp", 0);
		iNewCursorTex_a = pkTexMan->Load("file:../data/textures/cursor_r_a.bmp", 0);
		eMouseDir = Right;} 
	else if(my == 0) {
		iNewCursorTex = pkTexMan->Load("file:../data/textures/cursor_u.bmp", 0);
		iNewCursorTex_a = pkTexMan->Load("file:../data/textures/cursor_u_a.bmp", 0);
		eMouseDir = Up;} 
	else if(my >= m_iHeight-32) {
		iNewCursorTex = pkTexMan->Load("file:../data/textures/cursor_d.bmp", 0);
		iNewCursorTex_a = pkTexMan->Load("file:../data/textures/cursor_d_a.bmp", 0);
		eMouseDir = Down;} 
	else {
		iNewCursorTex = pkTexMan->Load("file:../data/textures/cursor.bmp", 0);
		iNewCursorTex_a = pkTexMan->Load("file:../data/textures/cursor_a.bmp", 0);
		eMouseDir = Nothing; }

	// swap cursor image
	if(s_iCursorTex != iNewCursorTex)
		pkGui->SetCursor(mx, my, (s_iCursorTex = iNewCursorTex), 
			(s_iCursorTex_a = iNewCursorTex_a), 32, 32);

	//camera movements
	if(pkInput->Action(m_iActionCamLeft) || eMouseDir == Left)
	{
		MoveCam(Vector3(-100,0,0));
	}
	if(pkInput->Action(m_iActionCamRight) || eMouseDir == Right)
	{
		MoveCam(Vector3(100,0,0));
	}
	if(pkInput->Action(m_iActionCamUp) || eMouseDir == Up)
	{
		MoveCam(Vector3(0,0,-100));
	}
	if(pkInput->Action(m_iActionCamDown) || eMouseDir == Down)
	{
		MoveCam(Vector3(0,0,100));
	}
	
	if(pkInput->Action(m_iActionExploreAll))
	{
		m_pkFogRender->ExploreAll();
		
		
		if(m_pkClientInput)
		{
			UnitCommand bla;
			strcpy(bla.m_acCommandName,"HOCKER MOVE TO TREE ATT 4 o clock");
			m_pkClientInput->AddOrder(bla);
		
		}
	}
	
	if(pkInput->Action(m_iActionUnExploreAll))
		m_pkFogRender->UnExploreAll();
	
	if(pkInput->Action(m_iActionSelect))
	{
		if(pkFps->GetTicks() - m_fClickTimer < m_fClickDelay)
			return;
			
		m_fClickTimer = pkFps->GetTicks();
	
		PickInfo info = Pick();

		// Test pathfind
		if(info.iObject != -1)
		{
			m_pkMoveObject = pkObjectMan->GetObjectByNetWorkID(info.iObject);
			m_pkEnd = m_pkStart = GetSqrFromPos(m_pkMoveObject->GetPos());
		}
		else
		{
			if(m_pkStart != Point(-1,-1))
			{
				m_pkEnd = info.kSquare;
				m_pkStart = GetSqrFromPos(m_pkMoveObject->GetPos());
				if(m_pkTestPath->Rebuild(m_pkStart.x, m_pkStart.y, m_pkEnd.x, m_pkEnd.y) == false)
				{
					m_pkEnd = m_pkStart;
					printf("Pathfinding Failed\n");
				}

				float fx=m_pkEnd.x, fy=m_pkEnd.y;
				int tex = m_pkMap->GetMostVisibleTexture(fx,fy);
				printf("Texture=%i\n", tex);
			}

			float fx = info.kHitPos.x, fy = info.kHitPos.z;
			Vector3 sqrnorm = m_pkMap->Tilt(fx,fy);
			Vector3 ground_plane = Vector3(0,1,0);
			float angle = sqrnorm.Angle(ground_plane);
			printf("Angle: %f\n", RadToDeg(angle));
		}

		//do we want to clear?
		if(!pkInput->Action(m_iActionSelectManyModifier))
			ClearSelected();
		
		if(info.iObject != -1)
			AddSelectedObject(info.iObject);			

	}
	
	if(pkInput->Action(m_iActionScroll))
	{
		int x,y;
		pkInput->RelMouseXY(x,y);
	
		MoveCam(Vector3(x*10,0,y*10));		
	}

	if(pkInput->Action(m_iActionPrintServerInfo))
	{
		Object* sio = pkObjectMan->GetObject("A ServerInfoObject");
		
		if(sio != NULL)
		{		
			P_ServerInfo* si = (P_ServerInfo*)sio->GetProperty("P_ServerInfo");	
		
			if(si != NULL)
			{
				pkConsole->Printf("Server Name: %s",si->m_kSInfo.m_acServerName);
			
			}
		}
		else		
			pkConsole->Printf("No server info found");		
	}



	if(pkInput->Pressed(KEY_W))
		pkLevelMan->ChangeLandscapeFillMode(LINE);
	if(pkInput->Pressed(KEY_F))
		pkLevelMan->ChangeLandscapeFillMode(FILL);

	if(pkInput->GetQueuedKey() == KEY_M && m_pkMiniMap)
	{
		static bool s_bToggle = true;
		m_pkMiniMap->Show((s_bToggle=!s_bToggle));
	}


			
}


void ZeroRTS::OnHud(void) 
{	
	pkFps->DevPrintf("common","Active Propertys: %d",pkObjectMan->GetActivePropertys());	
	pkFps->DevPrintf("common", "Fps: %f",pkFps->m_fFps);	
	pkFps->DevPrintf("common","Avrage Fps: %f",pkFps->m_fAvrageFps);			
	pkFps->DevPrintf("common","SelfID: %d", m_iSelfObjectID);	

	pkFps->m_bGuiMode = false;
	pkFps->ToggleGui();

/*	if(m_pkMiniMap)
		m_pkMiniMap->Draw(m_pkCamera, pkGui, m_pkFogRender, pkRender); */
}

void ZeroRTS::OnServerStart(void)
{	
	//add server info property
	if(!pkObjectMan->GetObject("A ServerInfoObject"))
	{
		Object* pkObj = pkObjectMan->CreateObjectByArchType("ServerInfoObject");
		if(!pkObjectMan->CreateObjectByArchType("ServerInfoObject"))
			cout<<"Faild to create serverinfoobject"<<endl;
		else
			pkObjectMan->GetWorldObject()->AddChild(pkObj);
	}
	
}

void ZeroRTS::OnClientStart(void)
{
}

void ZeroRTS::RunCommand(int cmdid, const CmdArgument* kCommand)
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

			m_pkMiniMap = new MiniMap(m_pkGuiBuilder, pkTexMan);
			m_pkMiniMap->Create(/*pkTexMan,*/ pkLevelMan); 
			
			pkConsole->Printf("Level loaded");

			BuildPath();
			break;		
		
		case FID_UNLOAD:
			break;
	}
}



Vector3 ZeroRTS::Get3DMousePos()
{
	Vector3 mpos;
	mpos.y = 0;
	pkInput->UnitMouseXY(mpos.x,mpos.z);
	
	//these have to be recalculated depending on camera distsance 
	mpos.x*=75;
	mpos.z*=56;
	mpos.z-=10.5;
	
	Matrix4 bla;
	bla.Identity();
	bla.Rotate(-20,0,0);
	
	mpos=bla.VectorTransform(mpos);
	
	mpos.x+=m_pkCamera->GetPos().x;
	mpos.z+=m_pkCamera->GetPos().z;	
	

	return mpos;
}


Object* ZeroRTS::PickObject()
{
	Vector3 dir=(Get3DMousePos() - m_pkCamera->GetPos()).Unit();
	vector<Object*> obs;
	pkObjectMan->TestLine(&obs,m_pkCamera->GetPos(),dir);
	
	Object* close =NULL;
	float dist = 999999999;
	
	for(unsigned int i=0;i<obs.size();i++)
	{
		if(obs[i]->GetName() == "ZoneObject")
			continue;
			
		if(!obs[i]->GetProperty("P_ClientUnit"))			
			continue;
			
		float d = (m_pkCamera->GetPos() - obs[i]->GetPos()).Length();
		if(d < dist)
		{
			dist = d;
			close = obs[i];
		}
	}

	obs.clear();
	return close;
}

HM_vert* ZeroRTS::PickMap(Vector3& kHitPos)
{
	Vector3 dir=(Get3DMousePos() - m_pkCamera->GetPos()).Unit();
	Vector3 center = (m_pkCamera->GetPos() - Vector3(0,0,20));
	HM_vert* vert = m_pkMap->LinePick(m_pkCamera->GetPos(),dir,center,140,kHitPos);

	return vert;
}

PickInfo ZeroRTS::Pick()
{
	PickInfo temp;
	
	temp.pkVert = PickMap(temp.kHitPos);
	temp.kSquare = GetSqrFromPos(temp.kHitPos);
	
	Object* pkPicked = PickObject();	
	
	if(pkPicked != NULL)	
		temp.iObject = pkPicked->iNetWorkID;
	else		
		temp.iObject = -1;

	return temp;
}

void ZeroRTS::SetCamPos(Vector3 kPos)
{
	float wb = 75;	//width border
	float tb = 74;	//top border
	float bb = 10;	//bothom border

	if(kPos.x < (-m_pkMap->GetSize()/2 + wb))
		kPos.x = (-m_pkMap->GetSize()/2 + wb);
	
	if(kPos.x > (m_pkMap->GetSize()/2 - wb))
		kPos.x = (m_pkMap->GetSize()/2 - wb);

	if(kPos.z < (-m_pkMap->GetSize()/2 +tb))
		kPos.z = (-m_pkMap->GetSize()/2 +tb );

	if(kPos.z > (m_pkMap->GetSize()/2 -bb))
		kPos.z = (m_pkMap->GetSize()/2 -bb);

	m_pkCamera->GetPos().x = kPos.x;
	m_pkCamera->GetPos().z = kPos.z;
}

Vector3 ZeroRTS::GetCamPos()
{
	return m_pkCamera->GetPos();
}

void ZeroRTS::MoveCam(Vector3 kVel)
{
	SetCamPos(GetCamPos() + kVel * pkFps->GetFrameTime());
}

bool ZeroRTS::AddSelectedObject(int iID)
{
	if(AlreadySelected(iID))
		return false;
		
	GetClientUnit(iID)->m_bSelected = true;
	
	m_kSelectedObjects.push_back(iID);

	cout<<"object Added to selection"<<endl;

	return true;
}

bool ZeroRTS::AlreadySelected(int iID)
{
	for(list<int>::iterator it = m_kSelectedObjects.begin();it != m_kSelectedObjects.end();it++)		
		if((*it) == iID)
			return true;
	
	return false;
}

bool ZeroRTS::RemoveSelectedObject(int iID)
{
	GetClientUnit(iID)->m_bSelected = false;	
	m_kSelectedObjects.remove(iID);	
	
	return true;
}

void ZeroRTS::ClearSelected()
{
	for(list<int>::iterator it = m_kSelectedObjects.begin();it != m_kSelectedObjects.end();it++)		
		GetClientUnit((*it))->m_bSelected = false;

	m_kSelectedObjects.clear();
}


P_ClientUnit* ZeroRTS::GetClientUnit(int iID)
{
	Object* pkObject = pkObjectMan->GetObjectByNetWorkID(iID);
	
	return (P_ClientUnit*)pkObject->GetProperty("P_ClientUnit");
}


void ZeroRTS::SetObjDstPos(int sqr_x, int sqr_y, Object* pkObject)
{	
	if(pkObject == NULL)
		return;

	Vector3 newp = GetPosFromSqr(Point(sqr_x, sqr_y));

	pkObject->SetPos(newp);
	pkObject->SetPos(newp);
}

void ZeroRTS::BuildPath()
{
	static bool bDone = false;
	if(bDone == false)
		bDone = true;
	else
		return;

	int aiCost[5];
	aiCost[0] = 15; // gräs (grön nyans)
	aiCost[1] = 1; // väg (röd nyans)
	aiCost[2] = 7; // sten (blå nyans)
	aiCost[3] = 10; // öken (röd nyans)
	aiCost[4] = 999; // vatten

	PathBuilder kPathBuilder(m_pkMap, &m_pkTestPath);
	kPathBuilder.Build(aiCost);
}

Point ZeroRTS::GetSqrFromPos(Vector3 pos)
{
	int iSquareX = m_pkMap->m_iHmSize/2+ceil(pos.x / HEIGHTMAP_SCALE);
	int iSquareY = m_pkMap->m_iHmSize/2+ceil(pos.z / HEIGHTMAP_SCALE);

	return Point(iSquareX,iSquareY);
}

Vector3 ZeroRTS::GetPosFromSqr(Point square)
{
	float x = -(m_pkMap->m_iHmSize/2)*HEIGHTMAP_SCALE + square.x*HEIGHTMAP_SCALE;
	float z = -(m_pkMap->m_iHmSize/2)*HEIGHTMAP_SCALE + square.y*HEIGHTMAP_SCALE;

	x -= HEIGHTMAP_SCALE/2;	// Translate to center 
	z -= HEIGHTMAP_SCALE/2;	// of square.*/

	float y = m_pkMap->Height(x,z);

	return Vector3(x,y,z);
}

bool ZeroRTS::MovePath(Object* pkObject)
{
	static float prev_time = 0;

	float time = pkFps->GetGameTime();

	if(time - prev_time > 0.125f)
	{
		int x=-1, y=-1;
		if(!m_pkTestPath->GetNextStep(x,y))
		{
			return true; // do nothing
		}
		
		if(!(x==-1&&y==-1))
			SetObjDstPos(x, y, pkObject);

		prev_time = time;
	}

	return true;
}

void ZeroRTS::Explore()
{
	vector<Object*> kObject;
	
	pkObjectMan->GetAllObjects(&kObject);
	
	for(int i=0;i<kObject.size();i++)
	{
		P_ClientUnit* cu =(P_ClientUnit*)kObject[i]->GetProperty("P_ClientUnit");
		if(cu == NULL)
			continue;
		
		float vd = (float)cu->m_kInfo.m_cViewDistance;
	
		m_pkFogRender->Explore(kObject[i]->GetPos().x,kObject[i]->GetPos().z,vd);		
	}
}

void ZeroRTS::HandleOrders()
{
	
	if(P_ClientInput::m_kServerCommands.size() > 0)
	{
		cout<<"GOT "<<P_ClientInput::m_kServerCommands.size()<<" Commands"<<endl;
		
		for(int i=0;i<P_ClientInput::m_kServerCommands.size();i++)
		{
			cout<<"Player:"<< int(P_ClientInput::m_kServerCommands[i].m_cPlayerID)<<endl;
			cout<<"executing "<<P_ClientInput::m_kServerCommands[i].m_acCommandName<<endl;		
			
			
		}
	
		P_ClientInput::m_kServerCommands.clear();
	}
}

void ZeroRTS::ClientInit()
{
	cout<<"Client Join granted"<<endl;

	int mapwidth = pkLevelMan->GetHeightMap()->GetSize();

	cout<<"Mapsize is :"<<mapwidth<<endl;
	
	//Set fog size
	m_pkFogRender->SetScale(mapwidth);
	
	//setup minimap
	m_pkMiniMap = new MiniMap(m_pkGuiBuilder, pkTexMan);
	m_pkMiniMap->Create(/*pkTexMan,*/ pkLevelMan); 
	
	
	cout<<"Join Complete"<<endl;
}

void ZeroRTS::OnServerClientJoin(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Joined"<<endl;
	
	pkClient->m_pkObject->AddProperty("ModelProperty");	
	pkClient->m_pkObject->AddProperty("P_ClientInput");

	P_ClientInput* ci = (P_ClientInput*)pkClient->m_pkObject->GetProperty("P_ClientInput");
	
	ci->m_iPlayerID = iConID;

}

void ZeroRTS::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Parted"<<endl;	
}







