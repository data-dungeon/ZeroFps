#include "zeroedit.h"
#include "gui.h"
#include "fh.h"

char* pkTempObjectTemplate = "ZfEditTempObject";

ZeroEdit Editor("ZeroEdit",1024,768,24);

ZeroEdit::ZeroEdit(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth) 
{	
	m_eCameraMode = FreeFlight;
}

void ZeroEdit::OnInit(void) 
{	
	RegisterPropertys();
		
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
	g_ZFObjSys.Register_Cmd("skybox6",FID_SKYBOX6,this);		
	g_ZFObjSys.Register_Cmd("addland",FID_ADDLAND,this);		
	g_ZFObjSys.Register_Cmd("removeland",FID_REMOVELAND,this);		
	g_ZFObjSys.Register_Cmd("listland",FID_LISTLAND,this);		
	g_ZFObjSys.Register_Cmd("loadland",FID_LOADLAND,this);		
	g_ZFObjSys.Register_Cmd("saveland",FID_SAVELAND,this);		
	g_ZFObjSys.Register_Cmd("madview",FID_VIEWMAD,this);		
	g_ZFObjSys.Register_Cmd("moon",FID_MOON,this);			
	g_ZFObjSys.Register_Cmd("sun",FID_SUN,this);
	g_ZFObjSys.Register_Cmd("ambient",FID_AMBIENT,this);	
	g_ZFObjSys.Register_Cmd("duplicate",FID_DUPLICATE,this);			
	g_ZFObjSys.Register_Cmd("copy",FID_COPY,this);
	g_ZFObjSys.Register_Cmd("paste",FID_PASTE,this);			
	g_ZFObjSys.Register_Cmd("delete",FID_DELETE,this);						
	g_ZFObjSys.Register_Cmd("findobj",FID_FINDOBJECT,this);			
	g_ZFObjSys.Register_Cmd("nextobj",FID_FINDOBJECT,this);			
	g_ZFObjSys.Register_Cmd("prevobj",FID_FINDOBJECT,this);						
	g_ZFObjSys.Register_Cmd("linkobject",FID_LINKOBJECT,this);			
	g_ZFObjSys.Register_Cmd("unlinkobject",FID_UNLINKOBJECT,this);			
	g_ZFObjSys.Register_Cmd("massspawn",FID_MASSSPAWN,this);			
	g_ZFObjSys.Register_Cmd("quit",FID_QUIT,this);
	g_ZFObjSys.Register_Cmd("opendlg",FID_OPENDLG,this);
	g_ZFObjSys.Register_Cmd("fileopendlg",FID_FILEOPENDLG,this);
	

	//start text =)
	pkConsole->Printf("            ZeroEdit ");
	pkConsole->Printf("--------------------------------");

	
	pkLevelMan->Fog(Vector3(0,0,0),200,250);	
	m_pkMap=pkLevelMan->GetHeightMap();
	m_pkCamera=new Camera(Vector3(0,10,0),Vector3(0,0,0),85,1.333,0.25,250);	

	glEnable(GL_LIGHTING);

	m_fTimer=pkFps->GetTicks();
	m_kDrawPos.Set(0,0,0);
	
	m_pkCurentChild=NULL;
	
	m_fPointerHeight=1;
		
	m_kCurentTemplate="null";
	g_ZFObjSys.RegisterVariable("g_template", &m_kCurentTemplate,CSYS_STRING);
	
	m_iPencilSize=8;
	g_ZFObjSys.RegisterVariable("g_pencilsize", &m_iPencilSize,CSYS_INT);
		
	m_iRandom=1;
	g_ZFObjSys.RegisterVariable("g_random", &m_iRandom,CSYS_INT);
	
	m_fDrawRate=0.1;
	g_ZFObjSys.RegisterVariable("g_drawrate", &m_fDrawRate,CSYS_INT);
	
	m_iMode=ADDOBJECT;		
	g_ZFObjSys.RegisterVariable("g_mode", &m_iMode,CSYS_INT);
		
	m_fPointDistance=10;
	g_ZFObjSys.RegisterVariable("g_pointdistance", &m_fPointDistance,CSYS_INT);

	m_iEditMask=1;
	g_ZFObjSys.RegisterVariable("g_editmask", &m_iEditMask,CSYS_INT);
	
	m_iSmooth=1;
	g_ZFObjSys.RegisterVariable("g_smooth", &m_iSmooth,CSYS_INT);	
	
	m_iMaskColorR=255;
	g_ZFObjSys.RegisterVariable("g_mcolorr", &m_iMaskColorR,CSYS_INT);	
	m_iMaskColorG=255;
	g_ZFObjSys.RegisterVariable("g_mcolorg", &m_iMaskColorG,CSYS_INT);	
	m_iMaskColorB=255;
	g_ZFObjSys.RegisterVariable("g_mcolorb", &m_iMaskColorB,CSYS_INT);	
	m_iMaskColorA=16;
	g_ZFObjSys.RegisterVariable("g_mcolora", &m_iMaskColorA,CSYS_INT);	
	
	
	
	//create a default small world
	pkLevelMan->CreateEmptyLevel(128);

	m_kMapBaseDir = pkLevelMan->GetMapBaseDir();

	m_pkGui = new Gui(this);

	SDL_ShowCursor(SDL_DISABLE);

	pkFps->m_bGuiTakeControl = true; 
	pkFps->ToggleGui(); 


	// run zeroedit_autoexec.ini
	if(!pkIni->ExecuteCommands("zeroedit_autoexec.ini"))
		pkConsole->Printf("No zeroedit_autoexec.ini found");

	m_iCopyNetWorkID = -1;

		
	Plane* ground = new Plane;
	ground->Set(Vector3(0,1,0),4);

/*	Plane* left = new Plane;	
	Plane* right = new Plane;
	Plane* front = new Plane;
	Plane* back = new Plane;

	ground->Set(Vector3(0,1,0),4);
	left->Set(Vector3(1,0,0),5);
	right->Set(Vector3(-1,0,0),5);
	front->Set(Vector3(0,0,-1),5);
	back->Set(Vector3(0,0,1),5);
	
	
	pkPhysics_Engine->AddPlane(left);
	pkPhysics_Engine->AddPlane(right);
	pkPhysics_Engine->AddPlane(front);
	pkPhysics_Engine->AddPlane(back);
*/	
	pkPhysics_Engine->AddPlane(ground);


	pkInput->ToggleGrab();
	
	plop= pkFps->GetGameTime();
}


void ZeroEdit::OnIdle(void) 
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();		
	
	
/*	// fina bollar =)
	if(pkFps->GetGameTime() - plop >2)
	{
		Object *object = new BallObject();
		float x = ((rand()%6000)-3000) /1000.0;
		float z = ((rand()%6000)-3000) /1000.0;		
		object->SetPos(Vector3(x,10,z));	
		object->SetPos(Vector3(x,10,z));					
		
		object->AttachToClosestZone();
		
		plop= pkFps->GetGameTime();
	}
*/
	Input(); 	
 	pkFps->UpdateCamera(); 	
 	SetPointer();
	DrawMarkers();

}


void ZeroEdit::OnHud(void) 
{
	glPushAttrib(GL_LIGHTING_BIT);
	
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	pkRender->SetFont("file:../data/textures/text/console.tga");

	pkFps->DevPrintf("common","Fps: %f",pkFps->m_fFps);
	pkFps->DevPrintf("common","Avrage Fps: %f",pkFps->m_fAvrageFps);	pkFps->DevPrintf("common","Mode: %d",m_iMode);
	pkFps->DevPrintf("common","Active Propertys: %d",pkObjectMan->GetActivePropertys());
	pkFps->DevPrintf("common","Pointer Altidude: %f",m_fPointerHeight);
	pkFps->DevPrintf("common","Collissions: %d",pkPhysics_Engine->m_iNrOfCollissions);

	if(m_pkCurentChild) {
		pkFps->DevPrintf("common","Object: %i: %s selected.",m_pkCurentChild->iNetWorkID, m_pkCurentChild->GetName().c_str());

		}

	glAlphaFunc(GL_GREATER,0.3);
	glEnable(GL_ALPHA_TEST);

	pkRender->Quad(Vector3(.8,.8,-1),Vector3(0,0,m_pkCamera->GetRot().y),Vector3(0.2,0.2,0.2),pkTexMan->Load("file:../data/textures/compas.tga",0));
	
	glDisable(GL_ALPHA_TEST);
	
	glPopAttrib();

	glEnable(GL_DEPTH_TEST);
}


void ZeroEdit::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	int iObjectID;
	Object *pkmad;
	Vector3 kRot(0,0,0);
	string BaseEnvMap("../data/textures/env/");

	switch(cmdid) {
		case FID_AMBIENT:{
			if(kCommand->m_kSplitCommand.size() < 4) {
				pkConsole->Printf("ambient [r][g][b]");
				break;
			}
			
			Vector3 kColor(atof(kCommand->m_kSplitCommand[1].c_str()),
							 	atof(kCommand->m_kSplitCommand[2].c_str()),
							 	atof(kCommand->m_kSplitCommand[3].c_str()));
							 
			pkLevelMan->SetAmbientColor(kColor);			
			break;	
		}				
		case FID_SUN:{
			if(kCommand->m_kSplitCommand.size() < 4) {
				pkConsole->Printf("sun [r][g][b]");
				break;
			}
			
			Vector3 kColor(atof(kCommand->m_kSplitCommand[1].c_str()),
							 	atof(kCommand->m_kSplitCommand[2].c_str()),
							 	atof(kCommand->m_kSplitCommand[3].c_str()));
							 
			pkLevelMan->SetSunColor(kColor);			
			break;	
		}
		
		case FID_MOON:{
			if(kCommand->m_kSplitCommand.size() < 4) {
				pkConsole->Printf("moon [r][g][b]");
				break;
			}
			
			Vector3 kColor(atof(kCommand->m_kSplitCommand[1].c_str()),
							 	atof(kCommand->m_kSplitCommand[2].c_str()),
							 	atof(kCommand->m_kSplitCommand[3].c_str()));
							 
			pkLevelMan->SetMoonColor(kColor);			
			break;	
		}
		case FID_SKYBOX6: {
			string strSkyPath = BaseEnvMap + kCommand->m_kSplitCommand[1] + "/sky";

			if(kCommand->m_kSplitCommand.size() == 5) {
				kRot.Set(atof(kCommand->m_kSplitCommand[2].c_str()),
						 atof(kCommand->m_kSplitCommand[3].c_str()),
						 atof(kCommand->m_kSplitCommand[4].c_str()));
				}

			pkLevelMan->SkyBox(strSkyPath.c_str(),"mode6",kRot);

			break;
			}

		case FID_SKYBOX:{
			if(kCommand->m_kSplitCommand.size() < 3) {
				pkConsole->Printf("skybox [Horizontal texture] [top,botom texture] (rot[x][y][z])");
				break;
			}
			
			if(kCommand->m_kSplitCommand.size() < 6)
			{
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
			{
				if(kCommand->m_kSplitCommand.size() <= 1) {
					pkConsole->Printf("Please Supply a filename");
					break;
				}
			
				m_pkCurentChild=NULL;
				int oldsize=m_pkMap->GetSize();
					
				if(!m_pkMap->LoadImageHmap(kCommand->m_kSplitCommand[1].c_str())){
					pkConsole->Printf("Could not load map =(");
				} else  {		
					m_pkMap->GenerateNormals(); 
										
					if(m_pkMap->GetSize() != oldsize)	//clear objects and reacreate zones if size dont match
					{
						pkLevelMan->ClearObjects();					//clear objects																					
						pkLevelMan->CreateZones();			//recreate zones
					}
				}
			
				break;
			}
	
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

			// Update texture map list in workpanel.
			DlgBox* pkDlg;
			pkDlg = m_pkGui->GetDlg("WorkPanelDlg"); 
			if(pkDlg != NULL)
				((WorkPanelDlg*)pkDlg)->UpdateMapTexList();
			
			break;	
			
		case FID_DUPLICATE:
			if(!m_pkCurentChild)
				break;

			m_iCopyNetWorkID = m_pkCurentChild->iNetWorkID;
			PasteObject(pkObjectMan->GetObjectByNetWorkID(m_iCopyNetWorkID)->GetPos());
			m_iCopyNetWorkID = -1;
			break;

		case FID_COPY:
			if(!m_pkCurentChild)
				break;
			m_iCopyNetWorkID = m_pkCurentChild->iNetWorkID;
			break;

		case FID_PASTE:
			PasteObject(m_kDrawPos);
			break;
		
		case FID_DELETE:
			if(!m_pkCurentChild)
				break;
			DeleteSelected();
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

		case FID_FINDOBJECT:
			// Find and select object by number.
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("findobj [objectnum]");
				break;
			}

			iObjectID=atoi(kCommand->m_kSplitCommand[1].c_str());
			pkConsole->Printf("Trying to find object %d",iObjectID);				
			m_pkCurentChild = pkObjectMan->GetObjectByNetWorkID(iObjectID);
			
			if(m_pkCurentChild) {
				if(m_pkCurentChild->GetName() == "ZoneObject" ||
					m_pkCurentChild->GetName() == "WorldObject" ||
					m_pkCurentChild->GetName() == "HeightMapObject")
					{
						m_pkCurentChild=NULL;
					}
			}
		
			break;

		case FID_NEXTOBJECT:	pkConsole->Printf("This command is not done yet :(.");	break;
		case FID_PREVOBJECT:	pkConsole->Printf("This command is not done yet :(.");	break;
		
		case FID_LINKOBJECT:	
			if(m_pkCurentChild && m_pkCurentParent) {
				m_pkCurentChild->SetParent(m_pkCurentParent);
				m_pkCurentParent = NULL;
				}
			break;

		case FID_UNLINKOBJECT:	
			if(m_pkCurentChild) {
				m_pkCurentChild->AttachToClosestZone();
				}
			break;

//		case FID_VFS_SAVE:	TestFS_Write();	break;
//		case FID_VFS_LOAD:	TestFS_Read();	break;

		case FID_VIEWMAD:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("madview [filename]");
				break;
			}
			
			pkConsole->Printf("Loading MAD %s", kCommand->m_kSplitCommand[1].c_str());				

			//pkmad = new FHObject(kCommand->m_kSplitCommand[1].c_str());

			pkmad = pkObjectMan->CreateObjectByArchType("object");
			if(pkmad) {
				pkmad->AddProperty("MadProperty");
				MadProperty* madp = dynamic_cast<MadProperty*>(pkmad->GetProperty("MadProperty"));
				madp->SetBasePtr(kCommand->m_kSplitCommand[1].c_str());
				pkmad->GetPos() = m_kDrawPos + Vector3(0,0,0);
				pkmad->AttachToClosestZone();
				}
			break;

		case FID_MASSSPAWN:
			pkConsole->Printf("Die FPS, DIE.");				
			int x,y;
			x = y = 0;
			for( x=0; x < 10; x+=1) {
				for(y=0; y < 10; y+=1) {
					pkmad = pkObjectMan->CreateObjectByArchType("tree");
					if(pkmad) {
						pkmad->GetPos() = m_kDrawPos + Vector3(x,0,y);
						pkmad->AttachToClosestZone();
						}
					//pkmad  = new FHObject("../data/mad/cube.mad");
					/*pkmad = new FHObject(NULL);
					pkmad->GetPos() = m_kDrawPos + Vector3(x,0,y);
					pkmad->AttachToClosestZone();*/
					}
				}
			
			break;

		case FID_QUIT:
			pkFps->QuitEngine();
			break;

		// Temporary test Functions ID's. 
		case FID_FUNC1:	break;
		case FID_FUNC2:	break;
		case FID_FUNC3:	break;
		case FID_FUNC4:	break;

		case FID_OPENDLG:

			if(kCommand->m_kSplitCommand[1] == "PropertyDlg")
			{
				DlgBox* pkDlg = m_pkGui->GetDlg(kCommand->m_kSplitCommand[1]);
				if(pkDlg)
				{
					// Set current child if open property dialog.
					((EditPropertyDlg*) pkDlg)->m_pkCurrentChild = m_pkCurentChild;
				}

			}
			m_pkGui->OpenDlg(kCommand->m_kSplitCommand[1]);
			break;

		case FID_FILEOPENDLG:

			SEARCH_TASK task = NONE;

			printf("%s %s\n", kCommand->m_kSplitCommand[1].c_str(),
				kCommand->m_kSplitCommand[2].c_str());

			if( kCommand->m_kSplitCommand[1] == "load" && 
				kCommand->m_kSplitCommand[2] == "map")
				task = LOAD_MAP;
			else 
			if( kCommand->m_kSplitCommand[1] == "save" && 
				kCommand->m_kSplitCommand[2] == "map")
				task = SAVE_MAP;
			else 
			if( kCommand->m_kSplitCommand[1] == "load" && 
				kCommand->m_kSplitCommand[2] == "template")
				task = LOAD_TEMPLATE;
			else 
			if( kCommand->m_kSplitCommand[1] == "save" && 
				kCommand->m_kSplitCommand[2] == "template")
				task = SAVE_TEMPLATE;
	
			if(task != NONE)
				m_pkGui->OpenFileDlg(task);

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
	if(m_pkGui->HaveFocus() || pkConsole->IsActive()) 
	{
		pkInput->SetInputEnabled(false);
	}

	if(pkInput->Pressed(MOUSELEFT)) 
	{
		int x,y;
		pkInput->MouseXY(x,y);

		ZGuiWnd* pkWndUnderCursor = pkGui->GetMainWindowFromPoint(x,y);

		// fulhack för att stänga menyn.
		if(pkWndUnderCursor != m_pkGui->GetMenu())
			m_pkGui->CloseMenu();
	}

	float childmovespeed=2;
	float childrotatespeed=15;
	float speed=40;

	//camera movements
	if(pkInput->Pressed(KEY_X)){
		speed*=0.25;
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
		Vector3 pos = m_pkCurentChild->GetPos();		

		if(pkInput->Pressed(KEY_LEFT)) {
			pos.x-=pkFps->GetFrameTime()*childmovespeed;
		}
		if(pkInput->Pressed(KEY_RIGHT)) {
			pos.x+=pkFps->GetFrameTime()*childmovespeed;
		}
		if(pkInput->Pressed(KEY_UP)) {
			pos.z-=pkFps->GetFrameTime()*childmovespeed;	
		}
		if(pkInput->Pressed(KEY_DOWN)) {
			pos.z+=pkFps->GetFrameTime()*childmovespeed;
		}
		if(pkInput->Pressed(KEY_RSHIFT)) {
			pos.y+=pkFps->GetFrameTime()*childmovespeed;
		}
		if(pkInput->Pressed(KEY_RCTRL)) {
			pos.y-=pkFps->GetFrameTime()*childmovespeed;
		}
		
		m_pkCurentChild->SetPos(pos);	
			
		//child rotation
		Vector3 rot = m_pkCurentChild->GetRot();		
		
		if(pkInput->Pressed(KEY_HOME)) {
			rot.y+=pkFps->GetFrameTime()*childrotatespeed;
		}
		if(pkInput->Pressed(KEY_END)) {
			rot.y-=pkFps->GetFrameTime()*childrotatespeed;
		}
		if(pkInput->Pressed(KEY_INSERT)) {
			rot.x+=pkFps->GetFrameTime()*childrotatespeed;
		}
		if(pkInput->Pressed(KEY_DELETE)) {
			rot.x-=pkFps->GetFrameTime()*childrotatespeed;
		}
		if(pkInput->Pressed(KEY_PAGEUP)) {
			rot.z+=pkFps->GetFrameTime()*childrotatespeed;
		}
		if(pkInput->Pressed(KEY_PAGEDOWN)) {
			rot.z-=pkFps->GetFrameTime()*childrotatespeed;
		}
		
		m_pkCurentChild->SetRot(rot);	
	}
	
	if(pkInput->Pressed(KEY_F8))
		g_ZFObjSys.RunCommand("shot");

	if(pkInput->Pressed(KEY_Q))
		pkFps->GetCam()->GetPos().y+=2*pkFps->GetFrameTime()*speed;			
	if(pkInput->Pressed(KEY_E))
		pkFps->GetCam()->GetPos().y-=2*pkFps->GetFrameTime()*speed;

	// Copy, Paste, Delete & Duplicate
	if(pkInput->Pressed(KEY_C) && m_pkCurentChild)
		m_iCopyNetWorkID = m_pkCurentChild->iNetWorkID;
	if(pkInput->Pressed(KEY_V))
			PasteObject(m_kDrawPos);
	if(pkInput->Pressed(KEY_BACKSPACE) && m_pkCurentChild)
			DeleteSelected();

	//Get mouse x,y		
	int x,z;		
	pkInput->RelMouseXY(x,z);	
    RotateCamera(x,z);

	switch(m_iMode){
		case FLATTEN:
			if(pkInput->Pressed(MOUSELEFT))
			{
				if(pkFps->GetTicks()-m_fTimer < m_fDrawRate)
					break;			
				m_fTimer=pkFps->GetTicks();	
			
				Vector3 pos=m_kDrawPos;
				m_pkMap->GetMapXZ(pos.x,pos.z);
			
				m_pkMap->Flatten(int(pos.x),int(pos.z),m_iPencilSize);
			
			}
			break;
			
		case SMOOTH:
			if(pkInput->Pressed(MOUSELEFT))
			{
				if(pkFps->GetTicks()-m_fTimer < m_fDrawRate)
					break;			
				m_fTimer=pkFps->GetTicks();			
			
				Vector3 pos=m_kDrawPos;
				m_pkMap->GetMapXZ(pos.x,pos.z);
			
				m_pkMap->Smooth(int(pos.x - (m_iPencilSize/2)),int(pos.z - (m_iPencilSize/2)),m_iPencilSize,m_iPencilSize);
			
			}
			break;
			
		case RAISE:
			if(pkInput->Pressed(MOUSELEFT))
			{
				if(pkFps->GetTicks()-m_fTimer < m_fDrawRate)
					break;			
				m_fTimer=pkFps->GetTicks();			
			
				Vector3 pos=m_kDrawPos;
				m_pkMap->GetMapXZ(pos.x,pos.z);
			
				m_pkMap->Raise(int(pos.x),int(pos.z),2,m_iPencilSize,(bool)m_iSmooth);
			
			}
			break;
			
		case LOWER:
			if(pkInput->Pressed(MOUSELEFT))
			{
				if(pkFps->GetTicks()-m_fTimer < m_fDrawRate)
					break;			
				m_fTimer=pkFps->GetTicks();			
			
				Vector3 pos=m_kDrawPos;
				m_pkMap->GetMapXZ(pos.x,pos.z);
			
				m_pkMap->Raise(int(pos.x),int(pos.z),-2,m_iPencilSize,(bool)m_iSmooth);			
			
			}
			break;
			
			
		case TEXTURE:
			if(pkInput->Pressed(MOUSELEFT))
			{
				if(pkFps->GetTicks()-m_fTimer < m_fDrawRate)
					break;			
				m_fTimer=pkFps->GetTicks();	
				
				HeightMapDraw(m_kDrawPos);			
			}
			if(pkInput->Pressed(MOUSERIGHT))
			{
				if(pkFps->GetTicks()-m_fTimer < m_fDrawRate)
					break;			
				m_fTimer=pkFps->GetTicks();	
				
				
				//invert alpha
				m_iMaskColorA*=-1;
				
				HeightMapDraw(m_kDrawPos);			
				
				//invert back
				m_iMaskColorA*=-1;
			}
			break;
			
		case ADDOBJECT:
			if(pkInput->Pressed(KEY_LSHIFT)) {
				// Movment Command
				if(pkInput->Pressed(MOUSELEFT) &&  m_pkCurentChild) 
					m_pkCurentChild->SetPos(m_kDrawPos-Vector3(0,1,0));
					m_pkCurentChild->SetPos(m_kDrawPos-Vector3(0,1,0));					
				// Rotate Command
				if(pkInput->Pressed(MOUSERIGHT) &&  m_pkCurentChild) {
					m_pkCurentChild->GetRot().x += x;
					m_pkCurentChild->GetRot().z += z;
					}
				if(pkInput->Pressed(MOUSEMIDDLE) &&  m_pkCurentChild) {
					m_pkCurentChild->GetRot().y += x;
					}
				}
			else {
                if( pkInput->Pressed(MOUSELEFT) )
				{
					if(pkFps->GetTicks()-m_fTimer < m_fDrawRate)
						break;			
					m_fTimer=pkFps->GetTicks();
				
					//Object *object=pkObjectMan->CreateObject(m_kCurentTemplate.c_str());
					Object *object;
					if(m_iCopyNetWorkID != -1) {
						object = pkObjectMan->CloneObject(m_iCopyNetWorkID);
						m_iCopyNetWorkID = -1;
						}
					else {
						object = pkObjectMan->CreateObjectByArchType(m_kCurentTemplate.c_str());
						}

					if(object==NULL)
						break;
					
					object->SetPos(m_kDrawPos);
					object->SetPos(m_kDrawPos);					
					object->GetVel().Set(0,0,0);
					
					object->SetRot(Vector3(30,0,0));
					object->SetRot(Vector3(30,0,0));					
					
					if(m_iRandom){
						object->GetRot().y=rand()%360;					
						object->GetRot().x=((rand()%20000)-10000)/1000.0;

						object->GetRot().z=((rand()%20000)-10000)/1000.0;					
					}else
					{
						object->GetRot().Set(0,0,0);
					}
					
					object->AttachToClosestZone();

					m_pkCurentChild=object;

					m_pkGui->UpdatePropertybox();
				}
				if( /*(pkInput->Pressed(MOUSERIGHT) && pkInput->Pressed(KEY_LSHIFT)) ||*/
					 pkInput->Pressed(MOUSERIGHT) )
				{
					SelectChild();
				}
			}

			if(pkInput->Pressed(KEY_P))
			{
				if(pkFps->GetTicks()-m_fTimer < m_fDrawRate)
					break;			
				m_fTimer=pkFps->GetTicks();
			
				if(m_pkCurentChild != NULL)
				{			
					VegitationProperty* vp= static_cast<VegitationProperty*>(m_pkCurentChild->GetProperty("VegitationProperty"));
				
					if(vp != NULL)
					{
						cout<<"adding"<<endl;
						Vector3 kPos = m_kDrawPos - m_pkCurentChild->GetPos();
						
						cout<<"pos:"<<kPos.x<<" "<<kPos.y<<" "<<kPos.z<<endl;
						vp->AddPos(kPos);						
					}
					else
						cout<<"no VegitationProperty"<<endl;				
				}				
			}
			
			if(pkInput->Pressed(KEY_L))
			{
				if(pkFps->GetTicks()-m_fTimer < m_fDrawRate)
					break;			
				m_fTimer=pkFps->GetTicks();
			
			
				if(m_pkCurentChild != NULL)
				{			
					VegitationProperty* vp= static_cast<VegitationProperty*>(m_pkCurentChild->GetProperty("VegitationProperty"));
								
					if(vp != NULL)
					{
						Vector3 kPos = m_kDrawPos;						
						
						//randomize position 
						kPos.x += (rand() % m_iPencilSize)-m_iPencilSize/2;
						kPos.z += (rand() % m_iPencilSize)-m_iPencilSize/2;													
						
						//get heightmap heiht att curent possition
						kPos.y = m_fPointerHeight+m_pkMap->Height(kPos.x,kPos.z);					
						
						kPos -= m_pkCurentChild->GetPos();
						vp->AddPos(kPos);						
					}
					else
						cout<<"no VegitationProperty"<<endl;				
				}				
			}
			
			
			if(pkInput->Pressed(KEY_O))
			{
				if(m_pkCurentChild != NULL)
				{			
					VegitationProperty* vp= static_cast<VegitationProperty*>(m_pkCurentChild->GetProperty("VegitationProperty"));
				
					if(vp != NULL)
					{
						vp->Clear();
					}
					else
						cout<<"no VegitationProperty"<<endl;								
				}
			}
			
			if(pkInput->Pressed(KEY_Z))
			{
				if(pkFps->GetTicks()-m_fTimer < m_fDrawRate)
					break;			
				m_fTimer=pkFps->GetTicks();

				//Object *object = new BallObject();
				Object *object = pkObjectMan->CreateObject();
				object->SetPos(m_kDrawPos);	
				object->SetPos(m_kDrawPos);					
				
				object->AttachToClosestZone();
				m_pkCurentChild=object;
				((EditPropertyDlg*)m_pkGui->
					GetDlg("PropertyDlg"))->m_pkCurrentChild = m_pkCurentChild;
				m_pkGui->UpdatePropertybox();
			}

			if(pkInput->Pressed(KEY_U))
			{
				cout<<"updating object"<<endl;
				vector<Property*> kProp;
				m_pkCurentChild->GetAllPropertys(&kProp,PROPERTY_TYPE_NORMAL,PROPERTY_SIDE_ALL);
				
				for(vector<Property*>::iterator it=kProp.begin();it!=kProp.end();it++) {
					(*it)->Update();				
				}				
			}
			if(pkInput->Pressed(KEY_M))
				SelectParent();
					
			break;			
	}

	int iKey = pkInput->GetQueuedKey();

	switch(iKey)
	{
		case KEY_SPACE:
			{
			CmdArgument* pkCmd = new CmdArgument();
			pkCmd->Set("opendlg PropertyDlg");
			RunCommand(FID_OPENDLG, pkCmd);
			}
			break;

		case KEY_F1:
			DlgBox* pkWorkpanel;
			pkWorkpanel = m_pkGui->GetDlg("WorkPanelDlg");
			
			if(pkWorkpanel->IsOpen())
				pkWorkpanel->Close(false);
			else
				pkWorkpanel->Open();
			break;
	}
}

void ZeroEdit::PasteObject(Vector3 kPos)
{
	if(pkFps->GetTicks()-m_fTimer < m_fDrawRate)
		return;			
	m_fTimer=pkFps->GetTicks();

	Object *object;
	if(m_iCopyNetWorkID != -1) {
		object = pkObjectMan->CloneObject(m_iCopyNetWorkID);
		//m_iCopyNetWorkID = -1;
		}
	else {
		object = pkObjectMan->CreateObjectByArchType(m_kCurentTemplate.c_str());
		}

	if(object==NULL)
		return;

	object->SetPos(kPos);
	object->SetPos(kPos);					
	object->GetVel().Set(0,0,0);
	
	object->AttachToClosestZone();
	m_pkCurentChild=object;

	((EditPropertyDlg*)m_pkGui->
		GetDlg("PropertyDlg"))->m_pkCurrentChild = m_pkCurentChild;

	m_pkGui->UpdatePropertybox();

}

void ZeroEdit::DeleteSelected()
{
	if(m_pkCurentChild == NULL)
		return;
	if(m_pkCurentChild->GetName() == "HeightMapObject") 
		return;
	if(m_pkCurentChild->GetName() == "WorldObject") 
		return;

	m_pkGui->ClosePropertybox();
	delete m_pkCurentChild;
	m_pkCurentChild=NULL;

	((EditPropertyDlg*)m_pkGui->
		GetDlg("PropertyDlg"))->m_pkCurrentChild = m_pkCurentChild;
	m_pkGui->UpdatePropertybox();
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
//	glEnable(GL_ALPHA_TEST);
//	glAlphaFunc(GL_GREATER,0.3);
	
//	pkRender->DrawBillboard(pkFps->GetCam()->GetModelViewMatrix(),m_kDrawPos,1,pkTexMan->Load("file:../data/textures/pointer.tga",T_NOMIPMAPPING));	
	pkRender->DrawBillboard(pkFps->GetCam()->GetModelViewMatrix(),m_kDrawPos,1,pkTexMan->Load("../data/textures/pointer.tga",T_NOMIPMAPPING));	
	
	
	if(m_pkCurentParent!=NULL){
		pkRender->DrawBillboard(pkFps->GetCam()->GetModelViewMatrix(),m_pkCurentParent->GetPos(),m_pkCurentParent->GetBoundingRadius()*2,pkTexMan->Load("file:../data/textures/parentmarker.tga",T_NOMIPMAPPING));	
	}


	if(m_pkCurentChild!=NULL){
		float size=1;
		
		MadProperty* mp = static_cast<MadProperty*>(m_pkCurentChild->GetProperty("MadProperty"));
		if( mp != NULL)
		{
			 size = mp->GetRadius();
		}		
		
		if(size < 1)
			size=1;
		
		glDisable(GL_LIGHTING);
		pkRender->Sphere(m_pkCurentChild->GetPos(),size,20,Vector3(1,0,0),false);
		//pkRender->DrawBoundingBox(m_pkCurentChild->GetPos(),
		//	m_pkCurentChild->GetRot(),Vector3(size,size,size));
		glEnable(GL_LIGHTING);
		
		if(m_pkCurentChild->GetParent()){
			pkRender->Line(m_pkCurentChild->GetParent()->GetPos(),m_pkCurentChild->GetPos());
		}
	}	
}


void ZeroEdit::SelectChild()
{
/*		
	list<PhysicProperty*> kPPs;
	kPPs.clear();
	pkPhysEngine->TestLine(&kPPs,pkFps->GetCam()->GetPos(),pkFps->GetCam()->GetRot().AToU());
	
	PhysicProperty* pp=NULL;
	float closest=99999999;

	for(list<PhysicProperty*>::iterator it=kPPs.begin();it!=kPPs.end();it++) 
	{	
		if( (*it)->GetObject()->GetName() == "ZoneObject" ||
			 (*it)->GetObject()->GetName() == "WorldObject" ||
			 (*it)->GetObject()->GetName() == "HeightMapObject")
			continue;
		
		float Distance= (pkFps->GetCam()->GetPos() - (*it)->GetObject()->GetPos()).Length();

		if(Distance < closest)
		{
			pp=(*it);
			closest=Distance;
		}			
	}
	
	
	if(pp==NULL){
		m_pkCurentChild=NULL;
		return;
	}
	
	m_pkCurentChild=pp->GetObject();
	*/
	Object* p = GetClosest(m_kDrawPos);
	
	if(p->GetName() == "ZoneObject" ||
		p->GetName() == "WorldObject" ||
		p->GetName() == "HeightMapObject")
	{
		m_pkCurentChild=NULL;

		((EditPropertyDlg*)m_pkGui->
			GetDlg("PropertyDlg"))->m_pkCurrentChild = m_pkCurentChild;
		m_pkGui->UpdatePropertybox();
		return;
	}
		
		
	m_pkCurentChild=p;

	((EditPropertyDlg*)m_pkGui->
		GetDlg("PropertyDlg"))->m_pkCurrentChild = m_pkCurentChild;
	m_pkGui->UpdatePropertybox();

	// Uppdatera property boxen
	m_pkGui->UpdatePropertybox();


}

void ZeroEdit::SelectParent()
{
	Object* temp=GetClosest(m_kDrawPos);
	
	if(temp!=NULL)
		m_pkCurentParent=temp;	
}

Object* ZeroEdit::GetClosest(Vector3 kPos)
{
	vector<Object*> temp;
	float mindistance=999999999;
	Object* minobject=NULL;
	
	pkObjectMan->GetWorldObject()->GetAllObjects(&temp);
	
	for(vector<Object*>::iterator it=temp.begin();it!=temp.end();it++) {
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
	pkPropertyFactory->Register("P_ServerUnit", Create_P_ServerUnit);
	pkPropertyFactory->Register("P_RenderSelection", Create_P_RenderSelection);	
	pkPropertyFactory->Register("P_ClientUnit", Create_P_ClientUnit);
	pkPropertyFactory->Register("LogProperty", Create_LogProperty);
	pkPropertyFactory->Register("DoorProperty", Create_DoorProperty);
	pkPropertyFactory->Register("ContainerProperty", Create_ContainerProperty);
	pkPropertyFactory->Register("ItemProperty", Create_ItemProperty);
	pkPropertyFactory->Register("TreeIdleProperty", Create_TreeIdleProperty);
	pkPropertyFactory->Register("AdaptorSniper", Create_AdaptorSniper);
	pkPropertyFactory->Register("StatusProperty", Create_StatusProperty);
	pkPropertyFactory->Register("TeleportProperty", Create_TeleportProperty);
	pkPropertyFactory->Register("MassDriverProperty", Create_MassDriverProperty);	
	pkPropertyFactory->Register("MassDriverProjectile", Create_MassDriverProjectile);			
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
	m_pkMap->GetMapXZ(kPencilPos.x,kPencilPos.z);
		
		
	m_pkMap->DrawMask(int(kPencilPos.x),int(kPencilPos.z),m_iEditMask,m_iPencilSize,m_iMaskColorR,m_iMaskColorG,m_iMaskColorB,m_iMaskColorA);
		

}

bool ZeroEdit::RotateCamera(int relMouseX, int relMouseY)
{
	bool bRotate = false;

	switch(m_eCameraMode)
	{
	case FreeFlight:
		bRotate = true;
		break;
	case Precision:
		if(pkInput->Pressed(MOUSELEFT))
			bRotate = true;
		break;
	}

	if(bRotate)
	{
		pkFps->GetCam()->GetRot().x+=relMouseY/5.0;
		pkFps->GetCam()->GetRot().y+=relMouseX/5.0;	
	}

	return false;
}
