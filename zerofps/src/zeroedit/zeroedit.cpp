#include "zeroedit.h"
#include "gui.h"

ZeroEdit Editor("ZeroEdit",800,600,16);

static bool g_ZGWinProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams )
{
	return Editor.m_pkGui->ZGWinProc(pkWindow, uiMessage, iNumberOfParams, pkParams);
}

ZeroEdit::ZeroEdit(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) 
{
	
}

void ZeroEdit::OnInit(void) 
{
	RegisterPropertys();
		
	pkLevelMan->Fog(Vector3(0,0,0),200,250);
	
	//register commands
	g_ZFObjSys.Register_Cmd("loadmap",FID_LOADMAP,this);	
	g_ZFObjSys.Register_Cmd("loadimagemap",FID_LOADIMAGEMAP,this);		
	g_ZFObjSys.Register_Cmd("savemap",FID_SAVEMAP,this);		
	g_ZFObjSys.Register_Cmd("newmap",FID_NEWMAP,this);		
	g_ZFObjSys.Register_Cmd("objecttree",FID_OBJECTTREE,this);		
	g_ZFObjSys.Register_Cmd("maketemplate",FID_MAKETEMPLATE,this);		
	g_ZFObjSys.Register_Cmd("listtemplates",FID_LISTTEMPLATES,this);
	g_ZFObjSys.Register_Cmd("savetemplate",FID_SAVETEMPLATE,this);
	g_ZFObjSys.Register_Cmd("loadtemplate",FID_LOADTEMPLATE,this);	
	g_ZFObjSys.Register_Cmd("saveobjects",FID_SAVEOBJECTS,this);	
	g_ZFObjSys.Register_Cmd("loadobjects",FID_LOADOBJECTS,this);		
	g_ZFObjSys.Register_Cmd("save",FID_SAVE,this);	
	g_ZFObjSys.Register_Cmd("load",FID_LOAD,this);		
	g_ZFObjSys.Register_Cmd("fog",FID_FOG,this);		
	g_ZFObjSys.Register_Cmd("water",FID_WATER,this);		
	g_ZFObjSys.Register_Cmd("skybox",FID_SKYBOX,this);		
	g_ZFObjSys.Register_Cmd("addland",FID_ADDLAND,this);		
	g_ZFObjSys.Register_Cmd("removeland",FID_REMOVELAND,this);		
	g_ZFObjSys.Register_Cmd("listland",FID_LISTLAND,this);		
	g_ZFObjSys.Register_Cmd("loadland",FID_LOADLAND,this);		
	g_ZFObjSys.Register_Cmd("saveland",FID_SAVELAND,this);		

	//start text =)
	pkConsole->Printf("            ZeroEdit ");
	pkConsole->Printf("--------------------------------");

	
	m_pkMap=pkLevelMan->GetHeightMap();
	m_pkCamera=new Camera(Vector3(0,10,0),Vector3(0,0,0),85,1.333,0.25,250);	

	glEnable(GL_LIGHTING);
	

	m_fTimer=pkFps->GetTicks();
	m_kDrawPos.Set(0,0,0);
//	pkObjectMan->SetNoUpdate(true);
	
	m_pkCurentChild=NULL;
	
	m_fPointerHeight=1;
		
	m_kCurentTemplate="null";
	pkFps->m_pkCmd->Add(&m_kCurentTemplate,"g_template",type_string);			
	
	m_iPencilSize=4;
	pkFps->m_pkCmd->Add(&m_iPencilSize,"g_PencilSize",type_int);		
	
	m_iLandType=1;
	pkFps->m_pkCmd->Add(&m_iLandType,"g_landtype",type_int);		
	
	m_iRandom=1;
	pkFps->m_pkCmd->Add(&m_iRandom,"g_Random",type_int);		
	
	m_fDrawRate=0.1;
	pkFps->m_pkCmd->Add(&m_fDrawRate,"g_DrawRate",type_float);		
	
	m_iMode=ADDOBJECT;		
	pkFps->m_pkCmd->Add(&m_iMode,"g_mode",type_int);		
		
	m_fPointDistance=10;
	pkFps->m_pkCmd->Add(&m_fPointDistance,"g_PointDistance",type_float);	
	
	
	//create a default small world
	pkLevelMan->CreateEmptyLevel(128);

	//default light
	LightSource *sol=new LightSource;	
	Vector3 *solrot=new Vector3(.4,.4,.2);	
	Vector3 *solpos=new Vector3(1000,1000,1000);
		sol->kRot=solrot;
		sol->kPos=solpos;		
		sol->kDiffuse=Vector4(1.2,1.2,1.2,1);	//Dag
//		sol->kDiffuse=Vector4(.01,.01,.01,1);	//Dag
		sol->kAmbient=Vector4(0.05,0.05,0.05,1);
		sol->iType=POINT_LIGHT;			
		sol->iPriority=10;
		sol->fConst_Atten=1;
		sol->fLinear_Atten=0;
		sol->fQuadratic_Atten=0;

	pkLight->Add(sol);

	m_pkGui = new Gui(this, g_ZGWinProc);

/*	ZGuiWnd* pkMainWindow = new ZGuiWnd(Rect(0,0,500,500),NULL,true,245123241);
	pkMainWindow->SetSkin(m_pkGui->GetSkin("blue"));
	pkMainWindow->SetMoveArea(Rect(0,0,m_iWidth,m_iHeight));

	int	iID = 22541; printf("%i", iID);
	ZGuiButton* pkCloseButton = new ZGuiButton(Rect(500-20,0,500,20),pkMainWindow,true,iID);
	pkCloseButton->SetButtonHighLightSkin(m_pkGui->GetSkin("bn_focus"));
	pkCloseButton->SetButtonDownSkin(m_pkGui->GetSkin("bn_down"));
	pkCloseButton->SetButtonUpSkin(m_pkGui->GetSkin("bn_up"));
	pkGui->AddMainWindow(24512324122, pkMainWindow, m_pkGui->m_pkWndProc, true);*/

}


void ZeroEdit::OnIdle(void) 
{	
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	

	pkObjectMan->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true);
//	m_pkHeightMapObject->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true);
		
	SetPointer();
	DrawMarkers();

	Input();
}


void ZeroEdit::OnHud(void) 
{
	glPushAttrib(GL_LIGHTING_BIT);
	
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	pkRender->SetFont("file:../data/textures/text/console.tga");
	//pkRender->Print(Vector3(-1.1,.85,-1),Vector3(0,0,0),Vector3(0.06,0.06,0.06),"FPS:");	
	//pkRender->Print(Vector3(-.9,.85,-1),Vector3(0,0,0),Vector3(0.06,0.06,0.06),fps);

	pkFps->DevPrintf("Fps: %d",pkFps->m_iFps);
	pkFps->DevPrintf("Mode: %d",m_iMode);
	pkFps->DevPrintf("Active Propertys: %d",pkObjectMan->GetActivePropertys());
	pkFps->DevPrintf("Pointer Altidude: %f",m_fPointerHeight);

	glAlphaFunc(GL_GREATER,0.3);
	glEnable(GL_ALPHA_TEST);

	pkRender->Quad(Vector3(.8,.8,-1),Vector3(0,0,m_pkCamera->GetRot().y),Vector3(0.2,0.2,0.2),pkTexMan->Load("file:../data/textures/compas.tga",0));
	
	glDisable(GL_ALPHA_TEST);
	
	glPopAttrib();

	glEnable(GL_DEPTH_TEST);
}


void ZeroEdit::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) {
		
		case FID_SAVELAND:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("saveland [filename]");
				break;
			}
			
			if(!SaveLandToFile(kCommand->m_kSplitCommand[1].c_str()))
				break;
				
			pkConsole->Printf("LandTypes Saved");				
		
			break;		
		
		case FID_LOADLAND:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("loadland [filename]");
				break;
			}
			
			if(!LoadLandFromFile(kCommand->m_kSplitCommand[1].c_str()))
				break;
				
			pkConsole->Printf("LandTypes Loaded");				
		
			break;
		
		case FID_ADDLAND:{
			if(kCommand->m_kSplitCommand.size() < 5) {
				pkConsole->Printf("addland [texture-nr] color[r][g][b]");
				break;
			}
			
			Vector3 kColor(atof(kCommand->m_kSplitCommand[2].c_str()),
								atof(kCommand->m_kSplitCommand[3].c_str()),
								atof(kCommand->m_kSplitCommand[4].c_str()));
			
			AddLandtype(atoi(kCommand->m_kSplitCommand[1].c_str()),kColor);			  
					  
			break;
			}
			
		case FID_REMOVELAND:{
			if(kCommand->m_kSplitCommand.size() < 2) {
				pkConsole->Printf("addland [texture-nr] color[r][g][b]");
				break;
			}
			
			if(!RemoveLandtype(atoi(kCommand->m_kSplitCommand[1].c_str())))
				pkConsole->Printf("Landtype not found");				
					  
			break;
			}
		
		case FID_LISTLAND:
			ListLandTypes();
			
			break;
	
		case FID_SKYBOX:{
			if(kCommand->m_kSplitCommand.size() < 3) {
				pkConsole->Printf("skybox [Horizontal texture] [top,botom texture] (rot[x][y][z])");
				break;
			}
			
			if(kCommand->m_kSplitCommand.size() < 6)
			{
				Vector3 kRot(0,0,0);
				pkLevelMan->SkyBox(kCommand->m_kSplitCommand[1].c_str(),kCommand->m_kSplitCommand[2].c_str(),kRot);
			} else{
				Vector3 kRot(atof(kCommand->m_kSplitCommand[3].c_str()),
								 atof(kCommand->m_kSplitCommand[4].c_str()),
								 atof(kCommand->m_kSplitCommand[5].c_str()));
								 
				pkLevelMan->SkyBox(kCommand->m_kSplitCommand[1].c_str(),kCommand->m_kSplitCommand[2].c_str(),kRot);
			}
			
			break;	
		}
		
		case FID_WATER:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("water 1(on) / 0 (off)");
				break;
			}
			
			if(kCommand->m_kSplitCommand[1] == "1")
				pkLevelMan->Water(true);
				
			if(kCommand->m_kSplitCommand[1] == "0")
				pkLevelMan->Water(false);
		
			break;
			
		case FID_FOG:{
			if(kCommand->m_kSplitCommand.size() < 5) {
				pkConsole->Printf("fog [r] [g ] [b] [start] [stop]");
				break;
			}
		
			Vector3 color;		
			color.Set(atof(kCommand->m_kSplitCommand[1].c_str()),
						 atof(kCommand->m_kSplitCommand[2].c_str()),
						 atof(kCommand->m_kSplitCommand[3].c_str()));
						
			float start=atof(kCommand->m_kSplitCommand[4].c_str());
			float stop=atof(kCommand->m_kSplitCommand[5].c_str());
		
			pkLevelMan->Fog(color,start,stop);
		
			break;
			}
			
		case FID_OBJECTTREE:
			pkObjectMan->GetWorldObject()->PrintTree(0);
			break;		
			
		case FID_LOADMAP:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("Please Supply a filename");
				break;
			}
			
			m_pkCurentChild=NULL;
			
			pkLevelMan->Clear();		//clear objects
			if(!m_pkMap->Load(kCommand->m_kSplitCommand[1].c_str())){
				pkConsole->Printf("Could not load map =(");
			} else {
				pkLevelMan->CreateZones();			
			}
			
			break;
			
		case FID_LOADIMAGEMAP:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("Please Supply a filename");
				break;
			}
			
			m_pkCurentChild=NULL;
			
			pkLevelMan->Clear();		//clear objects
			if(!m_pkMap->LoadImageHmap(kCommand->m_kSplitCommand[1].c_str())){
				pkConsole->Printf("Could not load map =(");
			} else  {
				m_pkMap->GenerateNormals(); 
				m_pkMap->GenerateTextures();
				pkLevelMan->CreateZones();
			}
			
			break;
	
		case FID_SAVEMAP:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("Please Supply a filename");
				break;
			}
			
			if(!m_pkMap->Save(kCommand->m_kSplitCommand[1].c_str()))
				pkConsole->Printf("Could not save map =(");			

			break;
			
		case FID_NEWMAP:{
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("Please Type a mapsize");
				break;
			}
			
			m_pkCurentChild=NULL;
			
			int size=atoi(kCommand->m_kSplitCommand[1].c_str());
			
			pkConsole->Printf("Creating new map with size %d",size);
			pkLevelMan->CreateEmptyLevel(size);
			
			break;
			}
		case FID_MAKETEMPLATE:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("maketemplate [template]");
				break;
			}
			
			if(m_pkCurentChild==NULL){
				pkConsole->Printf("You have not selected any child");
				break;
			}
						
			if(!pkObjectMan->MakeTemplate(kCommand->m_kSplitCommand[1].c_str(),m_pkCurentChild))
			{
				pkConsole->Printf("Failed To Make Template");
				pkConsole->Printf("Template %s already exist",kCommand->m_kSplitCommand[1].c_str());
				break;
			}
			
			
			pkConsole->Printf("Template %s Created",kCommand->m_kSplitCommand[1].c_str());
			
			break;
			
		case FID_LISTTEMPLATES:			
			ListTemplates();
			break;

		case FID_LOADTEMPLATE:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("loadtemplate [filename]");
				break;
			}
		
			if(!pkObjectMan->LoadTemplate(kCommand->m_kSplitCommand[1].c_str())){
				pkConsole->Printf("Error loading template");
				break;
			}
			
			pkConsole->Printf("Template %s Loaded",kCommand->m_kSplitCommand[1].c_str());
			break;

		case FID_SAVETEMPLATE:
			if(kCommand->m_kSplitCommand.size() <= 2) {
				pkConsole->Printf("savetemplate [template] [file]");
				break;
			}
			
			if(!pkObjectMan->SaveTemplate(kCommand->m_kSplitCommand[1].c_str(),kCommand->m_kSplitCommand[2].c_str()))
			{
				pkConsole->Printf("Error Saving template");
				break;
			}
			
			pkConsole->Printf("Template %s Saved",kCommand->m_kSplitCommand[1].c_str());
			break;
		case FID_SAVEOBJECTS:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("saveobjects [file]");
				break;				
			}
			
			if(!pkObjectMan->SaveAllObjects(kCommand->m_kSplitCommand[1].c_str())){
				pkConsole->Printf("Error Saving");
				break;			
			}
				
			pkConsole->Printf("Objects Saved");
			
			break;
			
		case FID_LOADOBJECTS:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("loadobjects [file]");
				break;				
			}
			
			m_pkCurentChild=NULL;
			
			if(!pkObjectMan->LoadAllObjects(kCommand->m_kSplitCommand[1].c_str())){
				pkConsole->Printf("Error loading");
				break;			
			}
				
			pkConsole->Printf("Objects loaded");
			
			break;
			
		case FID_LOAD:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("load [mapname]");
				break;				
			}
			
			m_pkCurentChild=NULL;
			
			if(!pkLevelMan->LoadLevel(kCommand->m_kSplitCommand[1].c_str()))	
			{
				pkConsole->Printf("Error loading level");
				break;			
			}		
			
			pkConsole->Printf("Level loaded");
			
			break;	
			
		case FID_SAVE:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("load [mapname]");
				break;				
			}
			if(!pkLevelMan->SaveLevel(kCommand->m_kSplitCommand[1].c_str()))	
			{
				pkConsole->Printf("Error saving level");
				break;			
			}		
				
			pkConsole->Printf("Level saved");				
			
			break;
	}
}

void ZeroEdit::OnServerStart(void)
{	
}

void ZeroEdit::OnClientStart(void)
{	
}

void ZeroEdit::Input() 
{
	float childmovespeed=2;
	float childrotatespeed=15;
	float speed=40;

	if(pkInput->GetQueuedKey() == KEY_F10)
		m_pkGui->ToogleMenu();

	//camera movements
	if(pkInput->Pressed(KEY_X)){
		speed*=0.5;
	}
	if(pkInput->Pressed(KEY_D)){
		pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y)/degtorad) *pkFps->GetFrameTime()*speed;			
		pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y)/degtorad) *pkFps->GetFrameTime()*speed;				
	}
	if(pkInput->Pressed(KEY_A)){
		pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y+180)/degtorad)*pkFps->GetFrameTime()*speed;			
		pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y+180)/degtorad)*pkFps->GetFrameTime()*speed;				
	}	
	if(pkInput->Pressed(KEY_W))	{
		pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
		pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
	}					
	if(pkInput->Pressed(KEY_S))	{
		pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;			
		pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;
	}		
	
	//pointer
	if(pkInput->Pressed(KEY_R)){
		m_fPointerHeight+=pkFps->GetFrameTime();
	}
	if(pkInput->Pressed(KEY_F)){
		m_fPointerHeight-=pkFps->GetFrameTime();
	}
	
	if(m_pkCurentChild!=NULL){	
		//child movements
		if(pkInput->Pressed(KEY_LEFT)) {
			m_pkCurentChild->GetPos().x-=pkFps->GetFrameTime()*childmovespeed;
		}
		if(pkInput->Pressed(KEY_RIGHT)) {
			m_pkCurentChild->GetPos().x+=pkFps->GetFrameTime()*childmovespeed;	
		}
		if(pkInput->Pressed(KEY_UP)) {
			m_pkCurentChild->GetPos().z-=pkFps->GetFrameTime()*childmovespeed;	
		}
		if(pkInput->Pressed(KEY_DOWN)) {
			m_pkCurentChild->GetPos().z+=pkFps->GetFrameTime()*childmovespeed;	
		}
		if(pkInput->Pressed(KEY_RSHIFT)) {
			m_pkCurentChild->GetPos().y+=pkFps->GetFrameTime()*childmovespeed;	
		}
		if(pkInput->Pressed(KEY_RCTRL)) {
			m_pkCurentChild->GetPos().y-=pkFps->GetFrameTime()*childmovespeed;	
		}

	
		//child rotation
		if(pkInput->Pressed(KEY_HOME)) {
			m_pkCurentChild->GetRot().y+=pkFps->GetFrameTime()*childrotatespeed;
		}
		if(pkInput->Pressed(KEY_END)) {
			m_pkCurentChild->GetRot().y-=pkFps->GetFrameTime()*childrotatespeed;
		}
		if(pkInput->Pressed(KEY_INSERT)) {
			m_pkCurentChild->GetRot().x+=pkFps->GetFrameTime()*childrotatespeed;
		}
		if(pkInput->Pressed(KEY_DELETE)) {
			m_pkCurentChild->GetRot().x-=pkFps->GetFrameTime()*childrotatespeed;
		}
		if(pkInput->Pressed(KEY_PAGEUP)) {
			m_pkCurentChild->GetRot().z+=pkFps->GetFrameTime()*childrotatespeed;
		}
		if(pkInput->Pressed(KEY_PAGEDOWN)) {
			m_pkCurentChild->GetRot().z-=pkFps->GetFrameTime()*childrotatespeed;
		}
		
		//child delete mohahaha
		if(pkInput->Pressed(KEY_BACKSPACE)) {
			if(m_pkCurentChild!=NULL)
				if(m_pkCurentChild->GetName() != "HeightMapObject") 
					if(m_pkCurentChild->GetName() != "WorldObject"){					
						delete m_pkCurentChild;
						m_pkCurentChild=NULL;
//						m_pkCurentParent=m_pkHeightMapObject;
					}

		}	
	}
	
	if(pkInput->Pressed(KEY_Q))
		pkFps->GetCam()->GetPos().y+=2*pkFps->GetFrameTime()*speed;			
	if(pkInput->Pressed(KEY_E))
		pkFps->GetCam()->GetPos().y-=2*pkFps->GetFrameTime()*speed;


	//Get mouse x,y		
	int x,z;		
	pkInput->RelMouseXY(x,z);

	//rotate the camera		
	if(!m_pkGui->IsMenuActive())
	{
		pkFps->GetCam()->GetRot().x+=z/5.0;
		pkFps->GetCam()->GetRot().y+=x/5.0;	
	}

	switch(m_iMode){
		case FLATTEN:
			if(pkInput->Pressed(MOUSELEFT))
			{
				float height=m_pkMap->GetVert(int(m_kDrawPos.x),int(m_kDrawPos.z))->height;
				int texture=m_pkMap->GetVert(int(m_kDrawPos.x),int(m_kDrawPos.z))->texture;
				Vector3 color=m_pkMap->GetVert(int(m_kDrawPos.x),int(m_kDrawPos.z))->color;
//				float height=m_pkMap->Height(m_kDrawPos.x,m_kDrawPos.z);
				for(int xp=-2;xp<3;xp++){
					for(int yp=-2;yp<3;yp++){
						m_pkMap->GetVert(int(m_kDrawPos.x+xp),int(m_kDrawPos.z+yp))->texture=texture;
						m_pkMap->GetVert(int(m_kDrawPos.x+xp),int(m_kDrawPos.z+yp))->color=color;//Vector3(.6,.45,0.3);		
						m_pkMap->GetVert(int(m_kDrawPos.x+xp),int(m_kDrawPos.z+yp))->height=height;				
					}
				}
				m_pkMap->GenerateNormals((int)m_kDrawPos.x-4,(int)m_kDrawPos.z-4,8,8);
				
			}
			break;
			
		case TEXTURE:
			if(pkInput->Pressed(MOUSELEFT))
			{
				HeightMapDraw(m_kDrawPos);			
			}
			break;
		case ADDOBJECT:
			if(pkInput->Pressed(MOUSELEFT))
			{
				if(pkFps->GetTicks()-m_fTimer < m_fDrawRate)
					break;			
				m_fTimer=pkFps->GetTicks();
			
//				Object *object = new BallObject();
				Object *object=pkObjectMan->CreateObject(m_kCurentTemplate.c_str());
				if(object==NULL)
					break;
				
				object->GetPos()=m_kDrawPos-Vector3(0,1,0);
				
				if(m_iRandom){
					object->GetRot().y+=rand()%360;					
					object->GetRot().x+=((rand()%25000)-12500)/1000.0;

					object->GetRot().z+=((rand()%25000)-12500)/1000.0;					
				}
				
				object->AttachToClosestZone();

				m_pkCurentChild=object;

			}
			if(pkInput->Pressed(MOUSERIGHT))
			{
				SelectChild();
			}
			if(pkInput->Pressed(KEY_SPACE))
			{
				SelectParent();
			}
			if(pkInput->Pressed(KEY_V))
			{
				if(pkFps->GetTicks()-m_fTimer < .5)
					break;			
				m_fTimer=pkFps->GetTicks();
				
				Object *object = new ZoneObject();
				object->GetPos()=m_kDrawPos-Vector3(0,1,0);
				object->SetParent(pkObjectMan->GetWorldObject());
//				pkObjectMan->Add(object);
				m_pkCurentChild=object;								
				m_pkCurentParent=object;
			}
			if(pkInput->Pressed(KEY_Z))
			{
				if(pkFps->GetTicks()-m_fTimer < m_fDrawRate)
					break;			
				m_fTimer=pkFps->GetTicks();

				Object *object = new BallObject();					
				object->GetPos()=m_kDrawPos;	
				
				object->AttachToClosestZone();
				m_pkCurentChild=object;
			}
			if(pkInput->Pressed(KEY_C))
			{
				list<Property*> kProp;
				m_pkCurentChild->GetAllPropertys(&kProp,PROPERTY_TYPE_NORMAL,PROPERTY_SIDE_ALL);
				
				for(list<Property*>::iterator it=kProp.begin();it!=kProp.end();it++) {
					(*it)->Update();				
				}				
			}
					
			break;			
	}
}

/*
void ZeroEdit::Clear() 
{
	CreateNew(100);
}

void ZeroEdit::CreateNew(int iSize) 
{
	pkLevelMan->CreateNew(iSize);
	
	m_pkMap=pkLevelMan->GetHeightMap();
	m_pkCurentChild=NULL;
	m_pkCurentParent=NULL;

/*
	pkObjectMan->Clear();
	m_pkCurentChild=NULL;
	
	m_pkHeightMapObject=new HeightMapObject(m_pkMap);		
	m_pkHeightMapObject->SetParent(pkObjectMan->GetWorldObject());
	
//	pkObjectMan->Add(m_pkHeightMapObject);	
	pkCollisionMan->Add(m_pkHeightMapObject);

	m_pkCurentParent=m_pkHeightMapObject;
	
	m_pkMap->Create(iSize);
	m_pkMap->GenerateNormals(); 
	m_pkMap->GenerateTextures();

	m_pkHeightMapObject->GetPos().Set(0,-4,0);				
	m_pkMap->SetPosition(Vector3(0,-4,0));


	cout<<"new map"<<endl;

}
*/


void ZeroEdit::SetPointer()
{
//	float degtorad=57.3248;
	Vector3 kPos=pkFps->GetCam()->GetPos();
	Vector3 kHead=pkFps->GetCam()->GetRot();
	
	m_fPointDistance=-(sin((kHead.x-90)/degtorad)/(sin((180-90-(kHead.x-90))/degtorad)/kPos.y));
	m_kDrawPos.Set(pkFps->GetCam()->GetPos().x,0,pkFps->GetCam()->GetPos().z);	
	m_kDrawPos.x+=cos((pkFps->GetCam()->GetRot().y-90)/degtorad)*m_fPointDistance;			
	m_kDrawPos.z+=sin((pkFps->GetCam()->GetRot().y-90)/degtorad)*m_fPointDistance;			
	m_kDrawPos.y = m_fPointerHeight+m_pkMap->Height(m_kDrawPos.x,m_kDrawPos.z);
}


void ZeroEdit::DrawMarkers()
{
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.3);
	
	pkRender->DrawBillboard(pkFps->GetCam()->GetModelMatrix(),m_kDrawPos,1,pkTexMan->Load("file:../data/textures/pointer.tga",T_NOMIPMAPPING));	
	
/*	
	if(m_pkCurentParent!=NULL){
		pkRender->DrawBillboard(pkFps->GetCam()->GetModelMatrix(),m_pkCurentParent->GetPos(),m_pkCurentParent->GetBoundingRadius()*2,pkTexMan->Load("file:../data/textures/parentmarker.tga",T_NOMIPMAPPING));	
	}
*/	

	if(m_pkCurentChild!=NULL){
		float size=m_pkCurentChild->GetBoundingRadius();
		if(size < .5)
			size=.5;
		pkRender->DrawBillboard(pkFps->GetCam()->GetModelMatrix(),m_pkCurentChild->GetPos(),size*2,pkTexMan->Load("file:../data/textures/childmarker.tga",T_NOMIPMAPPING));	
		
		if(m_pkCurentChild->GetParent()){
			pkRender->Line(m_pkCurentChild->GetParent()->GetPos(),m_pkCurentChild->GetPos());
		}
	}	
}


void ZeroEdit::SelectChild()
{
	Object* temp=GetClosest(m_kDrawPos);
	
	if(temp==NULL){
		m_pkCurentChild=NULL;	
		return;
	}
	
	if(temp->GetName() == "ZoneObject"){
		temp=NULL;
	}	

	m_pkCurentChild=temp;
}

void ZeroEdit::SelectParent()
{
	Object* temp=GetClosest(m_kDrawPos);
	
	if(temp!=NULL)
		m_pkCurentParent=temp;	
}

Object* ZeroEdit::GetClosest(Vector3 kPos)
{
	list<Object*> temp;
	float mindistance=999999999;
	Object* minobject=NULL;
	
	pkObjectMan->GetWorldObject()->GetAllObjects(&temp);
	
	for(list<Object*>::iterator it=temp.begin();it!=temp.end();it++) {
		float distance = abs(((*it)->GetPos() - kPos).Length());
		if(distance<mindistance){
			mindistance=distance;
			minobject=(*it);
		}
	}			
	
	temp.clear();
	
	return minobject;
}

void ZeroEdit::RegisterPropertys()
{
	pkPropertyFactory->Register("FloatProperty", Create_FloatProperty);
	pkPropertyFactory->Register("GravityProperty", Create_GravityProperty);	
//	pkPropertyFactory->Register("WorldInfoProperty", Create_WorldInfoProperty);	
}

void ZeroEdit::ListTemplates()
{
	vector<string> akNames;
	
	pkObjectMan->GetTemplateList(&akNames);
	
	pkConsole->Print("--Object Templates--");
	for(unsigned int i=0;i<akNames.size();i++)
	{
		pkConsole->Print(akNames[i].c_str());
	}

	akNames.clear();
}

void ZeroEdit::HeightMapDraw(Vector3 kPencilPos)
{
	LandType kTemp=GetLandType(m_iLandType);
	
	cout<<"color "<<kTemp.m_kColor.x<<endl;
	
	for(int xp=-(m_iPencilSize/2);xp<(m_iPencilSize/2)+1;xp++){
		for(int yp=-(m_iPencilSize/2);yp<(m_iPencilSize/2)+1;yp++){
			m_pkMap->GetVert(int(kPencilPos.x+xp),int(kPencilPos.z+yp))->texture=kTemp.m_iTexture;
			m_pkMap->GetVert(int(kPencilPos.x+xp +1),int(kPencilPos.z+yp +1))->color=kTemp.m_kColor;
		}
	}	
}


void ZeroEdit::AddLandtype(int iTexture,Vector3 kColor)
{
	LandType temp;
	temp.m_iTexture=iTexture;
	temp.m_kColor=kColor;
	
	m_kLandTypes.push_back(temp);
}

bool ZeroEdit::RemoveLandtype(int iTexture,Vector3 kColor)
{
	for(list<LandType>::iterator it=m_kLandTypes.begin();it!=m_kLandTypes.end();it++)
	{
		if((*it).m_iTexture==iTexture && (*it).m_kColor==kColor){
			m_kLandTypes.erase(it);
			return true;
		}
	}

	return false;
}

bool ZeroEdit::RemoveLandtype(int iNr)
{
	int i=0;
	
	for(list<LandType>::iterator it=m_kLandTypes.begin();it!=m_kLandTypes.end();it++)
	{	
		i++;
		if(i == iNr){
			m_kLandTypes.erase(it);
			return true;
		}
	}

	return false;
}

void ZeroEdit::ListLandTypes()
{
	pkConsole->Printf("--Land Types--");
	pkConsole->Printf("NR     Texture     Color");	
	
	int i=0;
	for(list<LandType>::iterator it=m_kLandTypes.begin();it!=m_kLandTypes.end();it++)
	{
		i++;
		pkConsole->Printf("%i    %i    %.3f %.3f %.3f",i,(*it).m_iTexture,(*it).m_kColor.x,(*it).m_kColor.y,(*it).m_kColor.z);
	}

}

LandType ZeroEdit::GetLandType(int iNr)
{
	int i=0;
	
	for(list<LandType>::iterator it=m_kLandTypes.begin();it!=m_kLandTypes.end();it++)
	{	
		i++;
		if(i == iNr){
			return (*it);
		}
	}

	LandType temp;
	temp.m_iTexture=0;
	temp.m_kColor.Set(0,0,0);
	
	return temp;
}

bool ZeroEdit::LoadLandFromFile(const char* acFile)
{
	ZFFile kFile;
	
	if(!kFile.Open(acFile,false))
	{
		pkConsole->Printf("Error opening file");
		return false;
	}
	
	m_kLandTypes.clear();
	
	int size; 
	kFile.Read((void*)&size,4);
	
	for(int i=0;i<size;i++)
	{
		LandType temp;
		
		kFile.Read((void*)&temp.m_iTexture,4);		
		kFile.Read((void*)&temp.m_kColor,12);		
	
		m_kLandTypes.push_back(temp);
	}
	
	kFile.Close();

	return true;
}


bool ZeroEdit::SaveLandToFile(const char* acFile)
{
	ZFFile kFile;
	
	if(!kFile.Open(acFile,true))
	{
		pkConsole->Printf("Error opening file");
		return false;
	}
	
	
	int size=m_kLandTypes.size();
	kFile.Write((void*)&size,4);
	
	for(list<LandType>::iterator it=m_kLandTypes.begin();it!=m_kLandTypes.end();it++)
	{	
		kFile.Write((void*)&(*it).m_iTexture,4);		
		kFile.Write((void*)&(*it).m_kColor,12);	
	}

	kFile.Close();

	return true;
}

