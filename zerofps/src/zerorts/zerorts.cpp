#include "zerorts.h"
#include "userpanel.h"
#include "minimap.h"
//#include "../common/p_clientinput.h"

ZeroRTS g_kZeroRTS("ZeroRTS",1024,768,16);

static bool USERPANELPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) { 
	return g_kZeroRTS.m_pkUserPanel->DlgProc(win,msg,numparms,params); }

#define NO_SELECTION Vector3(9000,0,9000)

ZeroRTS::ZeroRTS(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth) 
{
	m_pkTileEngine=new TileEngine();

	
	m_pkMiniMap = NULL;
	m_pkClientInput = NULL;
	m_iSelfObjectID = -1;
	m_HaveFoundHMapObject = false;
	m_bDisableCameraScroll = false;

	m_kClickPos = m_kDragPos = NO_SELECTION;

/*	COMMENT OUT BY ZEB
	m_pkTestPath = NULL;
	m_pkStart = Point(-1,-1);
	m_pkEnd = Point(-1,-1);*/
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
	g_ZFObjSys.Register_Cmd("massspawn",FID_MASSSPAWN,this);			

	

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
	m_fClickDelay = 0.4;
	
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

	// load command icon names from file
	if(pkIni->Open("cmdicons.txt", true))
	{
		vector<string> kNames;
		pkIni->GetCommandStrings(&kNames);

		for(unsigned int i=0; i<kNames.size(); i++)
		{
			m_pkUserPanel->m_kCmdIconNameMap.insert(
				map<int,string>::value_type(i, kNames[i]));

			m_pkUserPanel->m_kCmdIconIDMap.insert(
				map<string,int>::value_type(kNames[i], i));

			string szFileName = "file:../data/textures/cmdbuttons/";
			szFileName += kNames[i];
			szFileName += "_bnu.bmp";
			m_pkGuiBuilder->AddSkin(pkTexMan->Load(szFileName.c_str(), 0), 
				kNames[i]+string("_bnu"));

			szFileName = "file:../data/textures/cmdbuttons/";
			szFileName += kNames[i];
			szFileName += "_bnd.bmp";
			m_pkGuiBuilder->AddSkin(pkTexMan->Load(szFileName.c_str(), 0), 
				kNames[i]+string("_bnd"));

			szFileName = "file:../data/textures/cmdbuttons/";
			szFileName += kNames[i];
			szFileName += "_bnf.bmp";
			m_pkGuiBuilder->AddSkin(pkTexMan->Load(szFileName.c_str(), 0), 
				kNames[i]+string("_bnf"));
		}
	}
	else
		printf("Failed to load command icon names!!!\n\n");

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
	pkPropertyFactory->Register("P_UnitMoveAI", Create_P_UnitMoveAI);	
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
	
	Point bla = GetSqrFromPos(mpos);
	
	cout<<"square "<<bla.x<<" "<<bla.y<<endl;
	
	Tile* t = m_pkTileEngine->GetTile(bla.x,bla.y);
	if(t)
	{
		cout<<"angle:"<<t->fAngle<<endl;
		cout<<"terra:"<<t->iTerrainType<<endl;
			
	}
	
	
/*	glDisable(GL_LIGHTING);
		pkRender->Line(mpos-Vector3(1,0,0),mpos+Vector3(1,0,0));
		pkRender->Line(mpos-Vector3(0,1,0),mpos+Vector3(0,1,0));		
		pkRender->Line(mpos-Vector3(0,0,1),mpos+Vector3(0,0,1));				
	glEnable(GL_LIGHTING);
*/
//	m_pkFogRender->Explore(mpos.x,mpos.z,30);		

/* COMMENT OUT BY ZEB
	// tassa
	if(m_pkMoveObject)
		MovePath(m_pkMoveObject);

	if(pkFps->m_bServerMode == false) {
		int iObjID = pkFps->GetClientObjectID();
		m_iSelfObjectID = iObjID;
		}
*/

	//update player possition
	Object* pkObj = pkObjectMan->GetObjectByNetWorkID( m_iSelfObjectID );
	if(pkObj) {
		pkObjectMan->OwnerShip_Take( pkObj );
		pkObj->SetPos(pkFps->GetCam()->GetPos() + Vector3(0,-10,0));
		pkObj->SetPos(pkFps->GetCam()->GetPos() + Vector3(0,-10,0));
	}

	if(m_pkMiniMap)
		m_pkMiniMap->Draw(m_pkCamera, pkGui, m_pkFogRender, pkRender); 

	// Draw selection rectangle
	if(m_kClickPos != NO_SELECTION)
	{
		Vector3 left(m_kClickPos.x,m_kClickPos.y+1,m_kDragPos.z), 
			top(m_kClickPos.x,m_kClickPos.y+1,m_kClickPos.z), 
			right(m_kDragPos.x,m_kClickPos.y+1,m_kClickPos.z), 
			bottom(m_kDragPos.x,m_kClickPos.y+1,m_kDragPos.z);
		
		pkRender->SetColor(Vector3(1,0,0));
		pkRender->Line(left, top);
		pkRender->Line(top, right);
		pkRender->Line(right, bottom);
		pkRender->Line(bottom, left);
	}
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
				Object* pkClientObj = pkObjectMan->GetObjectByNetWorkID(m_iSelfObjectID);
				if(pkClientObj) {
					m_pkClientInput = (P_ClientInput*)pkClientObj->GetProperty("P_ClientInput");

					if(m_pkClientInput != NULL)
					{
						cout<<"Found client input property"<<endl;					
						
						//run client init
						cout << "My Num: " << m_pkClientInput->m_iPlayerID;
						//ClientInit();
						
					}
				}
			}
		}

		if(m_pkClientInput && m_HaveFoundHMapObject == false) 
		{
			Object* pkHmap = pkObjectMan->GetObject("HeightMapObject");
	
			if(pkHmap) 
			{
				m_HaveFoundHMapObject = true;
				
				cout<<"Assuming that server has started,executing client init"<<endl;
				ClientInit();
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
		Center
	};
	
	MOUSEDIR eMouseDir; 
	eMouseDir = Center;

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
		eMouseDir = Center; }

	// swap cursor image
	if(s_iCursorTex != iNewCursorTex)
		pkGui->SetCursor(mx, my, (s_iCursorTex = iNewCursorTex), 
			(s_iCursorTex_a = iNewCursorTex_a), 32, 32);

	UpdateSelectionArea();

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
			PickInfo info2 = Pick();
			//PickInfo info2;

			for(list<int>::iterator it = m_kSelectedObjects.begin(); it != m_kSelectedObjects.end();it++)
			{
				UnitCommand bla;

				// Try to get last command from user panel
				// (the commando generated when the user clicks on a button)
				if(!m_pkUserPanel->PopLastButtonCommand(bla.m_acCommandName))
				{
					// No commando exist. Use move commando instead,
					strcpy(bla.m_acCommandName, "Move");
				}

				//bla.m_iXDestinaton = 100;
				//bla.m_iYDestinaton = 100;			
				//info2.kSquare.x = 100;
				//info2.kSquare.y = 100;

				bla.m_iXDestinaton = info2.kSquare.x;
				bla.m_iYDestinaton = info2.kSquare.y;			
				bla.m_iUnitID = (*it);
				m_pkClientInput->AddOrder(bla);
			}
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
		
		/* COMMENT OUT BY ZEB
		// Test pathfind
		if(info.iObject != -1)
		{
			m_pkMoveObject = pkObjectMan->GetObjectByNetWorkID(info.iObject);
			m_pkEnd = m_pkStart = m_pkMap->GetSqrFromPos(m_pkMoveObject->GetPos());
		}
		else
		{
			if(m_pkStart != Point(-1,-1))
			{
				m_pkEnd = info.kSquare;
				m_pkStart = m_pkMap->GetSqrFromPos(m_pkMoveObject->GetPos());
				if(m_pkTestPath->Rebuild(m_pkStart.x, m_pkStart.y, m_pkEnd.x, m_pkEnd.y) == false)
				{
					m_pkEnd = m_pkStart;
					printf("Pathfinding Failed\n");
				}

				float fx=m_pkEnd.x, fy=m_pkEnd.y;
				int tex = m_pkMap->GetMostVisibleTexture(fx,fy);
				//printf("Texture=%i\n", tex);
			}

			float fx = info.kHitPos.x, fy = info.kHitPos.z;
			Vector3 sqrnorm = m_pkMap->Tilt(fx,fy);
			Vector3 ground_plane = Vector3(0,1,0);
			float angle = sqrnorm.Angle(ground_plane);
			//printf("Angle: %f\n", RadToDeg(angle));
		}
		*/

		//do we want to clear?
		if(!pkInput->Action(m_iActionSelectManyModifier))
			ClearSelected();
		
		if(info.iObject != -1)
			AddSelectedObject(info.iObject);		
		
		m_pkUserPanel->UpdateCmdButtons();
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

void ZeroRTS::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	Object* pkmad;

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

			SetupSpawnPoints();
		/*	BuildPath();*/
			
			//setup tile engine
			m_pkTileEngine->CreateMap();
			
			pkConsole->Printf("Everything is loaded ,Starting server");
			g_ZFObjSys.RunCommand("server Default server");
			
			break;		
		
		case FID_UNLOAD:
			break;

		case FID_MASSSPAWN:
			pkConsole->Printf("Nu fettar vi till det igen.");				
			int x,y;
			x = y = 0;
			int iAntal = 0;
			for( x=-200; x < 200; x+=10) {
				for(y=-200; y < 200; y+=10) {
					pkmad = pkObjectMan->CreateObjectByArchType("ZeroRTSTestBox");
					if(pkmad) {
						Vector3 kPos = Vector3(x,0,y);
						pkmad->SetPos(kPos);
						pkmad->SetPos(kPos);
						pkmad->AttachToClosestZone();
						}
					iAntal++;
					}
				}
			
			pkConsole->Printf("Spawned %d",iAntal);				
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
	temp.kSquare = m_pkMap->GetSqrFromPos(temp.kHitPos);

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
	if(m_bDisableCameraScroll)
		return;

	SetCamPos(GetCamPos() + kVel * pkFps->GetFrameTime());
}

bool ZeroRTS::AddSelectedObject(int iID)
{
	if(AlreadySelected(iID))
		return false;
		
	GetClientUnit(iID)->m_bSelected = true;
	
	m_kSelectedObjects.push_back(iID);

	cout<<"object Added to selection"<<endl;

/*	printf("%Propertys:\n");
		vector<Property*> akProps;
		pkObjectMan->GetObjectByNetWorkID(iID)->GetPropertys(&akProps, 
			PROPERTY_TYPE_ALL, PROPERTY_SIDE_ALL);
		for(int i=0; i<akProps.size(); i++)
			printf("%s\n", akProps[i]->m_acName);
	printf("\n");*/

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
	int iSquareX = m_pkMap->m_iHmSize/2+floor(pos.x / HEIGHTMAP_SCALE);
	int iSquareY = m_pkMap->m_iHmSize/2+floor(pos.z / HEIGHTMAP_SCALE);

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
			UnitCommand* uc = &P_ClientInput::m_kServerCommands[i];		
		
			cout<<"Player:    "<< int(uc->m_cPlayerID)<<endl;
			cout<<"executing: "<<uc->m_acCommandName<<endl;		
			cout<<"on unit:   "<<uc->m_iUnitID<<endl;
			cout<<"target:    "<<uc->m_iTarget<<endl;
			cout<<"targetpos: "<<uc->m_iXDestinaton<< " "<<uc->m_iYDestinaton<<endl;
			
			Object* ob = pkObjectMan->GetObjectByNetWorkID(uc->m_iUnitID);
			if(ob != NULL)			
			{	
				P_ServerUnit* su = (P_ServerUnit*)ob->GetProperty("P_ServerUnit");
				
				if(su != NULL)
				{
					//check that teems mach
					if( int(su->m_kInfo.m_cTeam) == int(uc->m_cPlayerID))							
						su->RunExternalCommand(uc);							
					else
						cout<<"Player :"<<int(uc->m_cPlayerID)<< " is trying to Cheat"<<endl;
				}
				else
					cout<<"Error: P_ServerUnit not found on unit"<<endl;
			}
			else
				cout<<"Error: Unit not found"<<endl;
		}
	
		P_ClientInput::m_kServerCommands.clear();
	}
}

void ZeroRTS::ClientInit()
{
	cout<<"Client Join granted"<<endl;

	int mapwidth = pkLevelMan->GetHeightMap()->GetSize();

	cout<<"Mapsize is :"<<mapwidth<<endl;
	
	m_pkMap = pkLevelMan->GetHeightMap();	

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
	
	//setup client input
	pkClient->m_pkObject->AddProperty("P_ClientInput");
	P_ClientInput* ci = (P_ClientInput*)pkClient->m_pkObject->GetProperty("P_ClientInput");	
	ci->m_iPlayerID = iConID;

	CreateClientUnits(iConID);
}

void ZeroRTS::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Parted"<<endl;	
	
	RemoveClientUnits(iConID);
}

void ZeroRTS::SetupSpawnPoints()
{
	cout<<"Searching for spawn points"<<endl;	
	
	vector<Object*>	kObjects;	
	kObjects.clear();
	m_kSpawnPoints.clear();
	
	pkObjectMan->GetAllObjects(&kObjects);
	
	for(int i=0;i<kObjects.size();i++)
	{
		if(kObjects[i]->GetName() == "A ZeroRTSSpawnPoint")
		{
			m_kSpawnPoints.push_back(kObjects[i]->GetPos());
			pkObjectMan->Delete(kObjects[i]);
		}
	}

	cout<<"found "<<m_kSpawnPoints.size()<< " spawn points"<<endl;	
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


void ZeroRTS::CreateClientUnits(int iID)
{
	//create player start object	
	Object* ob = pkObjectMan->CreateObjectByArchType("ZeroRTSEngineringCrew");
	
	if(ob)
	{
		cout<<"setting object pos to "<<m_kSpawnPoints[iID].x<<" "<<m_kSpawnPoints[iID].y<<" "<<m_kSpawnPoints[iID].z<<endl;
		ob->SetPos(m_kSpawnPoints[iID]);
		ob->SetPos(m_kSpawnPoints[iID]);		
		
		ob->AttachToClosestZone();
		
		P_ServerUnit* su = (P_ServerUnit*)ob->GetProperty("P_ServerUnit");
		
		if(su)
		{
			su->m_kInfo.m_cTeam = iID;		
		}
		else
			cout<<"error unit did not create correctly"<<endl;
	}
	else
		cout<<"error could not create client units"<<endl;

}

void ZeroRTS::RemoveClientUnits(int iID)
{
	cout<<"removing Clients units"<<endl;	
	
	vector<Object*>	kObjects;	
	kObjects.clear();
	
	pkObjectMan->GetAllObjects(&kObjects);
	
	for(int i=0;i<kObjects.size();i++)
	{
		P_ServerUnit* su = (P_ServerUnit*)kObjects[i]->GetProperty("P_ServerUnit");		
		
		if(su)
		{
			//does this player own this unit?
			if(int(su->m_kInfo.m_cTeam) == iID)
			{	
				//yes it does,,lets kill it =) MOHAHAHA!		
				pkObjectMan->Delete(kObjects[i]);
			}
		}		
	}
}

void ZeroRTS::UpdateSelectionArea()
{
	static bool s_bClicked;

	if(pkInput->Pressed(MOUSELEFT))
	{
		static Point s_PrevMousePos(-1,-1);
	
		int x,y;
		pkInput->MouseXY(x,y);

		if(s_PrevMousePos != Point(x,y))
		{
			s_PrevMousePos = Point(x,y);
			Vector3 kHitPos;
			PickMap(kHitPos);

			if(s_bClicked == false)
			{
				if(kHitPos.y < 0)
					kHitPos.y = 0;

				m_kDragPos = m_kClickPos = kHitPos;
				m_bDisableCameraScroll = s_bClicked = true;
			}
			else
			{
				m_kDragPos = kHitPos;
				if(m_kClickPos.y < m_kDragPos.y)
					m_kClickPos.y = m_kDragPos.y;
			}
		}
	}
	else
	{
		if(s_bClicked == true)
		{
			SelectObjects(m_kClickPos, m_kDragPos);
			m_kDragPos = m_kClickPos = NO_SELECTION;
			m_bDisableCameraScroll = s_bClicked = false;
		}
	}
}

void ZeroRTS::SelectObjects(Vector3 p1, Vector3 p2)
{
	vector<Object*> akAllObjs;
	pkObjectMan->GetAllObjects(&akAllObjs);

	float fMinX = p1.x < p2.x ? p1.x : p2.x;
	float fMaxX = p1.x > p2.x ? p1.x : p2.x;

	float fMinZ = p1.z < p2.z ? p1.z : p2.z;
	float fMaxZ = p1.z > p2.z ? p1.z : p2.z;

	const int antal = akAllObjs.size();
	for(int i=0; i<antal; i++)
	{
		Vector3 pos = akAllObjs[i]->GetPos();
		if( pos.x >= fMinX && pos.x <= fMaxX &&
			pos.z >= fMinZ && pos.z <= fMaxZ)
		{
			if(GetClientUnit(akAllObjs[i]->iNetWorkID))
				AddSelectedObject(akAllObjs[i]->iNetWorkID);
		}
	}
}
