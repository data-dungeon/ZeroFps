#include "zerorts.h"
#include "userpanel.h"
#include "minimap.h"

ZeroRTS g_kZeroRTS("ZeroRTS",1024,768,16);

static bool USERPANELPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) { 
	return g_kZeroRTS.m_pkUserPanel->DlgProc(win,msg,numparms,params); }

ZeroRTS::ZeroRTS(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth) 
{
	m_pkMiniMap = NULL;
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

}

void ZeroRTS::Init()
{	
	//get heightmap pointer 
	m_pkMap = pkLevelMan->GetHeightMap();	

	//register commmands
	g_ZFObjSys.Register_Cmd("load",FID_LOAD,this);		
	g_ZFObjSys.Register_Cmd("unload",FID_UNLOAD,this);			
	
	
	//damn "#¤(="%#( lighting fix
	glEnable(GL_LIGHTING );
	
	//initiate our camera
	m_pkCamera=new Camera(Vector3(0,CAMERA_HEIGHT,0),Vector3(70,0,0),90,1.333,0.25,250);	
	
	//disable zones modells
	pkLevelMan->SetVisibleZones(false);

	//register actions
	RegisterActions();

	//register property
	RegisterPropertys();
	
	//set clicktimer
	m_fClickTimer = pkFps->GetTicks();
	m_fClickDelay = 0.2;
	
	
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
	pkPropertyFactory->Register("P_RenderSelection", Create_P_RenderSelection);	
	pkPropertyFactory->Register("P_ClientUnit", Create_P_ClientUnit);
	pkPropertyFactory->Register("P_ServerUnit", Create_P_ServerUnit);	
}

void ZeroRTS::OnIdle(void) 
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	
			
	Input();
	

	Vector3 mpos = Get3DMousePos();
/*	
	glDisable(GL_LIGHTING);
		pkRender->Sphere(mpos,1,20,Vector3(1,0,0),false);
	glEnable(GL_LIGHTING);
*/
}

void ZeroRTS::Input()
{
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

	if(pkInput->Action(m_iActionSelect))
	{
		if(pkFps->GetTicks() - m_fClickTimer < m_fClickDelay)
			return;
			
		m_fClickTimer = pkFps->GetTicks();
	
		PickInfo info = Pick();
		
		//do we want to clear?
		if(!pkInput->Action(m_iActionSelectManyModifier))
			ClearSelected();
		
/*		if(bla != NULL)
		{	
			pkObjectMan->Delete(bla);
			cout<<"diiiiiiiiie object from hell"<<endl;
		}*/

		if(info.iObject != -1)
			AddSelectedObject(info.iObject);			

	}
	
	if(pkInput->Action(m_iActionScroll))
	{
		int x,y;
		pkInput->RelMouseXY(x,y);
	
		MoveCam(Vector3(x*10,0,y*10));		
	}

	if(pkInput->Pressed(KEY_W))
		pkLevelMan->ChangeLandscapeFillMode(LINE);
	if(pkInput->Pressed(KEY_F))
		pkLevelMan->ChangeLandscapeFillMode(FILL);

	if(pkInput->GetQueuedKey() == KEY_M && m_pkMiniMap)
		m_pkMiniMap->bDraw = !m_pkMiniMap->bDraw;
			
}


void ZeroRTS::OnHud(void) 
{	
	pkFps->DevPrintf("common","Active Propertys: %d",pkObjectMan->GetActivePropertys());	
	pkFps->DevPrintf("common", "Fps: %f",pkFps->m_fFps);	
	pkFps->DevPrintf("common","Avrage Fps: %f",pkFps->m_fAvrageFps);			

	pkFps->m_bGuiMode = false;
	pkFps->ToggleGui();

	if(m_pkMiniMap)
		m_pkMiniMap->Draw(m_pkCamera, pkGui); 
}

void ZeroRTS::OnServerStart(void)
{	
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

			m_pkMiniMap = new MiniMap();
			m_pkMiniMap->Create(pkTexMan, pkLevelMan, m_pkGuiBuilder); 
			
			pkConsole->Printf("Level loaded");
			
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
	float dist= 999999999;
	
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
	float bb = 20;	//bothom border

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
	{	
		GetClientUnit((*it))->m_bSelected = false;			
	}

	m_kSelectedObjects.clear();
}


P_ClientUnit* ZeroRTS::GetClientUnit(int iID)
{
	Object* pkObject = pkObjectMan->GetObjectByNetWorkID(iID);
	
	return (P_ClientUnit*)pkObject->GetProperty("P_ClientUnit");
}

