#include "zerofps.h"
#include "network.h"
#include "objectmanager.h"
#include "../render/glguirender.h"
#include "../basic/zfobjectmanger.h"
#include "../gui/zguiresourcemanager.h"
#include "application.h"
#include "../gui/zgui.h"
#include "levelmanager.h"
#include "../engine_systems/propertys/propertys.pkg"
#include "../basic/zfini.h"
 
int		g_iNumOfFrames;
int		g_iNumOfMadSurfaces;
float		g_fMadLODScale;
int		g_iMadLODLock;
int		g_iLogRenderPropertys;

static char Devformat_text[4096];	//

ZeroFps::ZeroFps(void) : I_ZeroFps("ZeroFps") 
{
	/* Create Engine SubSystems 
		*/

	m_pkZShader					= new ZShader;
	m_pkPhysics_Engine		= new Physics_Engine;
	m_pkResourceDB				= new ZFResourceDB;
	m_pkPhysEngine				= new PhysicsEngine;
	m_pkLevelMan				= new LevelManager;
	m_pkIni						= new ZFIni;
	m_pkGui						= new ZGui(Application::pkApp->m_iWidth, Application::pkApp->m_iHeight);
	m_pkGuiMan					= new ZGuiResourceManager;
	m_pkGuiRenderer			= new GLGuiRender;
	m_pkNetWork					= new NetWork;
	m_pkMusic					= new OggMusic(24,4096);
	m_pkOpenAlSystem			= new OpenAlSystem;
	m_pkSBM						= new SoundBufferManager;	
	m_pkObjectMan				= new ObjectManager;
	m_pkCmd						= new CmdSystem;
	m_pkConsole					= new Console;
	m_pkRender					= new Render;
	m_pkLight					= new Light;	
	m_pkFrustum					= new Frustum;	
	m_pkPropertyFactory		= new PropertyFactory;
	m_pkInput					= new Input;		
	m_pkTexMan					= new TextureManager;
	m_pkZFVFileSystem			= new ZFVFileSystem;
	m_pkBasicFS					= new ZFBasicFS;

	m_pkScript					= new ZFScript;

	// Set Default values
	m_iFullScreen				= 0;
	m_fFrameTime				= 0;
	m_fLastFrameTime			= 0;
	m_fSystemUpdateFps		= 25;
	m_fSystemUpdateTime		= 0;
	m_bServerMode				= false;
	m_bClientMode				= false;
	m_bGuiMode					= false;
	m_iMadDraw					= 1;
	g_fMadLODScale				= 1.0;
	g_iMadLODLock				= 0;
	m_pkCamera					= NULL;
	m_bRunWorldSim				= true;
	m_bCapture					= false;
	g_iLogRenderPropertys	= 0;
	m_fAvrageFpsTime			= 0;
	m_iAvrageFrameCount		= 0;

	// The default graphics mode.
	m_iWidth						= 640;
	m_iHeight					= 480;
	m_iDepth						= 16;

	// Register Commands
	RegisterVariable("r_width",			&m_iWidth,CSYS_INT);
	RegisterVariable("r_height",			&m_iHeight,CSYS_INT);
	RegisterVariable("r_depth",			&m_iDepth,CSYS_INT);
	RegisterVariable("r_fullscreen",		&m_iFullScreen,CSYS_INT);
	RegisterVariable("r_maddraw",			&m_iMadDraw,CSYS_INT);
	RegisterVariable("r_madlod",			&g_fMadLODScale,CSYS_FLOAT);
	RegisterVariable("r_madlodlock",		&g_iMadLODLock,CSYS_FLOAT);
	RegisterVariable("e_systemfps",		&m_fSystemUpdateFps,CSYS_FLOAT);	
	RegisterVariable("e_runsim",			&m_bRunWorldSim,CSYS_BOOL);	
	RegisterVariable("r_logrp",			&g_iLogRenderPropertys,CSYS_INT);	

	// Register Variables
	Register_Cmd("setdisplay",FID_SETDISPLAY);
	Register_Cmd("quit",FID_QUIT);
	Register_Cmd("slist",FID_SLIST);
	Register_Cmd("connect",FID_CONNECT);
	Register_Cmd("server",FID_SERVER);
	Register_Cmd("printobject",FID_PRINTOBJECT);	
	Register_Cmd("version",FID_VERSION);	
	Register_Cmd("credits",FID_CREDITS);	
	Register_Cmd("echo",FID_ECHO);	
	Register_Cmd("gldump",FID_GLDUMP);	
	Register_Cmd("devshow",FID_DEV_SHOWPAGE, CSYS_FLAG_SRC_ALL, "devshow name", 1);	
	Register_Cmd("devhide",FID_DEV_HIDEPAGE, CSYS_FLAG_SRC_ALL, "devhide name", 1);	
	Register_Cmd("debug",FID_LISTMAD);	
	Register_Cmd("shot",FID_SCREENSHOOT);	
}

ZeroFps::~ZeroFps()
{
	m_pkNetWork->ServerEnd();
	g_ZFObjSys.ShutDown();
	ConfigFileSave();

	delete m_pkPhysEngine;
	delete m_pkLevelMan;
	delete m_pkIni;
	delete m_pkGui;
	delete m_pkGuiMan;
	delete m_pkGuiRenderer;
	delete m_pkNetWork;
	delete m_pkOpenAlSystem;
	delete m_pkSBM;
	delete m_pkObjectMan;
	delete m_pkCmd;
	delete m_pkConsole;
	delete m_pkRender;
	delete m_pkLight;
	delete m_pkFrustum;
	delete m_pkPropertyFactory;
	delete m_pkInput;
	delete m_pkTexMan;
	delete m_pkZFVFileSystem;
	delete m_pkBasicFS;
	delete m_pkPhysics_Engine;
	delete m_pkResourceDB;		//d krashar om denna ligger där uppe =(, Dvoid
}

bool ZeroFps::StartUp()	
{
	m_kCurentDir = m_pkBasicFS->GetCWD();
	 
	cout << "m_kCurentDir: " << m_kCurentDir.c_str() << endl;
	char szWorkDir[256];
	strcpy(szWorkDir, m_kCurentDir.c_str());

	char* szDiv =  strrchr(szWorkDir, '/');
	if(szDiv)
		szDiv[1] = 0;

	m_pkZFVFileSystem->AddRootPath(szWorkDir);
	m_pkZFVFileSystem->AddRootPath("h:/");

	RegisterPropertys();
	RegisterResources();

	m_kClient.resize( 4 );	// Vim - Hard coded for now. Must be same as Network.SetMaxNodes
	for(int i=0; i<4; i++)
		m_kClient[i].m_pkObject = NULL;

	m_iRTSClientObject = -1;

	m_bDevPagesVisible = true;

	m_kFpsGraph.SetMinMax(0,1000);		
	m_kFpsGraph.SetSize(100,100,100);

//	m_pkObjectMan->Test_CreateZones();
	return true;
}

bool ZeroFps::ShutDown() 
{
	return true; 
}

bool ZeroFps::IsValid()	{ return true; }


void ZeroFps::SetApp() {
	m_pkApp=Application::pkApp;
	m_pkApp->SetEnginePointer(this);
}

void ZeroFps::ConfigFileRun()
{
	string CfgName = string(m_pkApp->m_pTitle) + ".ini";
	g_ZFObjSys.Config_Load(CfgName);
}

void ZeroFps::ConfigFileSave()
{
	string CfgName = string(m_pkApp->m_pTitle) + ".ini";
	g_ZFObjSys.Config_Save(CfgName);
}


bool ZeroFps::Init(int iNrOfArgs, char** paArgs)
{	
	SetApp();												//	setup class pointers	
	ConfigFileRun();
	g_ZFObjSys.HandleArgs(iNrOfArgs,paArgs);		//	handle arguments


	// StartUp SDL
	if(SDL_Init(SDL_OPENGL | SDL_INIT_NOPARACHUTE )<0){
		g_Logf("Error: Failed to StartUp SDL\n");
		return false;
	}	
	
	atexit(SDL_Quit);

/*#ifdef _WIN32
	RenderDLL_InitExtGL();
	extgl_Initialize();
#endif*/

	if(!g_ZFObjSys.StartUp())
		return false;
	
	InitDisplay(m_pkApp->m_iWidth,m_pkApp->m_iHeight,m_pkApp->m_iDepth);

	m_iState=state_normal;									// init gamestate to normal		


	m_pkApp->OnInit();										// call the applications oninit funktion
	
	m_fFrameTime=0;
	m_fLastFrameTime = SDL_GetTicks();

	return true;
}

/* Code that need to run on both client/server. */
void ZeroFps::Run_EngineShell()
{
	m_pkInput->SetInputEnabled(true);
/*
	m_pkObjectMan->PackToClients();
	m_pkNetWork->Run();
*/

	DevPrintf("common","Num of Clients: %d", m_pkNetWork->GetNumOfClients());
	if(m_bServerMode) {
		for(int i=0; i<4; i++) {
			DevPrintf("common","Client[%d]: %s", i, m_kClient[i].m_strName.c_str());
			// Server gives upp object a time after connection
			
			if(m_kClient[i].m_pkObject) {
				if(GetEngineTime() > (m_kClient[i].m_fConnectTime + 5))
					m_pkObjectMan->OwnerShip_Give( m_kClient[i].m_pkObject );
				}
			}
		}
	
	DevPrintf("common","Num Objects: %d", m_pkObjectMan->GetNumOfObjects());

	// Update Local Input.
	m_pkInput->Update();

	//toggle keyboard/mouse grabing		// SHELL
	if(m_pkInput->Pressed(KEY_F12))
		m_pkInput->ToggleGrab();
	if(m_pkInput->Pressed(KEY_F10))
		m_pkInput->ToggleGrab();
			
	//toggle fullscreen on X systems	// SHELL	
	if(m_pkInput->Pressed(KEY_F11))
		ToggleFullScreen();		

	if(m_pkInput->Pressed(KEY_TAB))
	{		
		m_pkConsole->Toggle();
		m_pkInput->Reset();
	}

	//update system
	Update_System();

}

void ZeroFps::Run_Server()
{
	//update zones
	//m_pkLevelMan->UpdateZones();			
	

}

void ZeroFps::Run_Client()
{
	if(m_pkConsole->IsActive())
		m_pkInput->SetInputEnabled(false);
	else 
		m_pkInput->SetInputEnabled(true);						
		
	//run application main loop
	m_pkApp->OnIdle();							
	m_pkObjectMan->Update(PROPERTY_TYPE_NORMAL,PROPERTY_SIDE_CLIENT,false);
		

	//update zones
	m_pkObjectMan->UpdateZones();	

	//   _---------------------------------- fulhack deluxe 
	UpdateCamera();
	
	m_pkRender->Draw_AxisIcon(5);

	m_pkObjectMan->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true);
	if(g_iLogRenderPropertys) {
		m_pkObjectMan->DumpActiverPropertysToLog("PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true");
		g_iLogRenderPropertys = 0;
		}
//	m_pkLevelMan->DrawZones();
	m_pkObjectMan->Test_DrawZones();

	//update openal sound system			
	Vector3 up=(m_pkCamera->GetRot()-Vector3(0,90,0));//.AToU();
	up.x=90;
	m_pkOpenAlSystem->SetListnerPosition(m_pkCamera->GetPos(),(m_pkCamera->GetRot()+Vector3(0,90,0)).AToU(),up.AToU());//(m_pkCamera->GetRot()-Vector3(-90,90,0)).AToU());
	m_pkOpenAlSystem->Update();

	//run application Head On Display 
	SetCamera(m_pkConsoleCamera);			
	m_pkApp->OnHud();
	m_pkObjectMan->UpdateDelete();

	m_pkNetWork->DevShow_ClientConnections();

}

void ZeroFps::Update_System()
{
	float fATime = GetTicks() - m_fSystemUpdateTime; 	
	int iLoops = int(m_fSystemUpdateFps * fATime);

	if(iLoops<=0)
		return;

	m_fGameFrameTime = 1/m_fSystemUpdateFps;//(fATime / iLoops);		
	float m_fLU = m_fSystemUpdateTime;
	
	for(int i=0;i<iLoops;i++)
	{
		//calculate current game time
		m_fGameTime = m_fLU + (i * m_fGameFrameTime);
	
		//client & server code
		
		//update network for client & server
		m_pkNetWork->Run();				
		
		//update application systems
		m_pkApp->OnSystem();
		
		
		//server only code
		if(m_bServerMode)
		{
			if(m_bRunWorldSim) {			
				//update all normal propertys
				m_pkObjectMan->Update(PROPERTY_TYPE_NORMAL,PROPERTY_SIDE_SERVER,false);
				m_pkObjectMan->UpdateGameMessages();

				//update physicsengine
				m_pkPhysEngine->Update();	
			
				//update new super duper rigid body physics engine deluxe
				m_pkPhysics_Engine->Update(m_fGameFrameTime);
			}	
		}
		
		//client only code
		if(m_bClientMode)
		{
		
		}

		
		//client & server code
		
		//pack objects to clients
		m_pkObjectMan->PackToClients();		

		//delete objects
		m_pkObjectMan->UpdateDelete();
		
		//update the resource manager
		m_pkResourceDB->Refresh();

		m_fSystemUpdateTime = GetTicks();
	}

}

void ZeroFps::Draw_EngineShell()
{
	// Describe Active Cam.
	if(GetCam()) {
		string strCamDesc = GetCam()->GetCameraDesc();
		DevPrintf("common",strCamDesc.c_str());
		}
	DevPrintf("common" , "NumMads/NumMadSurfaces: %d / %d", m_iNumOfMadRender , g_iNumOfMadSurfaces);

	m_iNumOfMadRender = 0;
	g_iNumOfMadSurfaces = 0;
	
	if(m_pkConsole->IsActive()) {		
		SetCamera(m_pkConsoleCamera);			
		
		m_pkInput->SetInputEnabled(true);			
		m_pkConsole->Update();
	}		
	else 
	{
		// Update GUI
		m_pkInput->SetInputEnabled(true);

		int mx, my;
		m_pkInput->MouseXY(mx,my);

		map<int,int>::iterator itKeyPressed;
		int iInputKey = m_pkInput->GetQueuedKey();
		int iPrevKey = iInputKey;
		m_pkInput->FormatKey(iInputKey);

		bool bGlobalFormat = (iPrevKey == iInputKey) ? true : false;

		if( (iInputKey >= 'a' && iInputKey <= 'z') ||
			(iInputKey >= 'A' && iInputKey <= 'Z') ||
			(iInputKey >= '0' && iInputKey <= '9') ||
			iInputKey == '+' || iInputKey == '?'||
			iInputKey == ';' || iInputKey == ','||
			iInputKey == ':' || iInputKey == '.' ||
			iInputKey == ' ')
			bGlobalFormat = false;

		if(bGlobalFormat)
		{
			itKeyPressed = m_pkInput->m_kGlobalKeyTranslator.find(iInputKey);
			if(itKeyPressed != m_pkInput->m_kGlobalKeyTranslator.end())
				iInputKey = itKeyPressed->second;
		}
	
		m_pkGui->Update(GetGameTime(),iInputKey,false,
			(m_pkInput->Pressed(KEY_RSHIFT) || m_pkInput->Pressed(KEY_LSHIFT)),
			mx,my,m_pkInput->Pressed(MOUSELEFT),m_pkInput->Pressed(MOUSERIGHT));
	}


}

void ZeroFps::MainLoop(void) {
	while(m_iState!=state_exit) {
		m_fEngineTime = GetTicks();

		Swap();											//swap buffers n calculate fps
		 
		Run_EngineShell();

		if(m_bServerMode)	{
			Run_Server();
			}
		if(m_bClientMode)	{
			Run_Client();
			}

		Draw_EngineShell();

//		m_kFpsGraph.DrawGraph(0,200);
//		m_pkNetWork->DrawConnectionGraphs();

	}
}

void ZeroFps::SetRenderTarget(Camera* pkCamera)
{
	for(unsigned int i=0; i<m_kRenderTarget.size(); i++)
		if(m_kRenderTarget[i] == pkCamera)
			return;

	m_kRenderTarget.push_back(pkCamera);
	cout << "Adding: " << pkCamera->GetName();
	cout << " to active rendertargets\n";
}

void ZeroFps::RemoveRenderTarget(Camera* pkCamera)
{
	
}


void ZeroFps::InitDisplay(int iWidth,int iHeight,int iDepth) 
{
	// Anything sent from app overrides default and ini files.
	if(iWidth || iHeight || iDepth) {
		m_iWidth	= iWidth;
		m_iHeight= iHeight;
		m_iDepth	= iDepth;
		}

	// Must call set res again or else GUI doesnt work..
	m_pkGui->SetRes(m_iWidth, m_iHeight);

	SetDisplay();

	// Set w and h for appliction
	m_pkApp->m_iWidth = m_iWidth;
	m_pkApp->m_iHeight = m_iHeight;

#ifdef _WIN32
	RenderDLL_InitExtGL();
	extgl_Initialize();
#endif

	//setup some opengl stuff =)
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);	
	glEnable(GL_CULL_FACE);
	glEnable(GL_SCISSOR_TEST);

	glShadeModel(GL_SMOOTH);
	glClearColor(0, 0, 0, 0);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glMatrixMode(GL_MODELVIEW);
  
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	
//	m_pkDefaultCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.25,250);
	m_pkConsoleCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),84,1.333,0.3,250);	
  
	glMatrixMode(GL_MODELVIEW);

	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
}

void ZeroFps::Swap(void) {
	DrawDevStrings();

	SDL_GL_SwapBuffers();  //guess

	if(m_bCapture) {
		m_bCapture = false;
		m_pkRender->CaptureScreenShoot(m_iWidth, m_iHeight);
		}

	glLoadIdentity();
  
	//count FPS
	m_fFrameTime=SDL_GetTicks()-m_fLastFrameTime;
	m_fLastFrameTime=SDL_GetTicks();
	m_fFps=1000.0/m_fFrameTime;	

//	m_kFpsGraph.PushValue( m_fFps);
	m_kFpsGraph.PushValue( 500 );
	
	m_iAvrageFrameCount++;
	
	if( (GetTicks() - m_fAvrageFpsTime) >1)
	{
		m_fAvrageFps = m_iAvrageFrameCount;
		m_iAvrageFrameCount = 0;
		m_fAvrageFpsTime = GetTicks();
	}
	
#ifdef RUNPROFILE
	g_iNumOfFrames++;
//	if(g_iNumOfFrames >= 1000)
//		m_iState = state_exit;

#endif
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	

}


void ZeroFps::ToggleFullScreen(void)
{
	SDL_WM_ToggleFullScreen(m_pkScreen);
}

void ZeroFps::ToggleGui(void)
{
	m_bGuiMode = !m_bGuiMode;
	
	if(m_bGuiMode == true)
	{
		m_pkGui->Activate(m_bGuiMode);
	}
	else
	{
		m_pkGui->Activate(m_bGuiMode);
	}
}


void ZeroFps::SetDisplay(int iWidth,int iHeight,int iDepth)
{
	m_iWidth=iWidth;
	m_iHeight=iHeight;
	m_iDepth=iDepth;

	SetDisplay();
}


void ZeroFps::SetDisplay()
{
	m_pkTexMan->ClearAll();

	//turn of opengl 
	SDL_QuitSubSystem(SDL_OPENGL);

	
	//reinit opengl with the new configuration
	SDL_InitSubSystem(SDL_OPENGL);

	if(m_iFullScreen > 0)
		m_pkScreen= SDL_SetVideoMode(m_iWidth,m_iHeight,m_iDepth,SDL_OPENGL|SDL_FULLSCREEN);	
	else
		m_pkScreen= SDL_SetVideoMode(m_iWidth,m_iHeight,m_iDepth,SDL_OPENGL);

	glViewport(0, 0,m_iWidth,m_iHeight);
	
	if(m_pkGuiRenderer->SetDisplay(m_iWidth,m_iHeight) == false)
	{
		printf("Failed to set GUI display!\n");
	}
}


void ZeroFps::SetCamera(Camera* pkCamera)
{
	//set camera pointer
	m_pkCamera=pkCamera;		
	
	//call updateall so that the camera updates viewport and realoads projectionmatrix
	m_pkCamera->UpdateAll(m_iWidth,m_iHeight);
	
	UpdateCamera();
}

void ZeroFps::UpdateCamera()
{
	//update camera
	m_pkCamera->Update(m_iWidth,m_iHeight);
	
	//get the frustrum for frustum culling
	//m_pkFrustum->GetFrustum();				
}

DevStringPage*	ZeroFps::DevPrint_FindPage(const char* szName)
{
	for(unsigned int i=0; i<m_DevStringPage.size(); i++) {
		if(string(szName) == m_DevStringPage[i].m_kName) {
			return &m_DevStringPage[i];
			}
		}

	return NULL;
}

void ZeroFps::DevPrintf(const char* szName, const char *fmt, ...)
{
	// Find the page to print to.
	DevStringPage* page = DevPrint_FindPage(szName);
	if(!page) {
		DevStringPage kNewPage;
		kNewPage.m_kName = szName;
		kNewPage.m_bVisible = false;
		m_DevStringPage.push_back(kNewPage);
		page = DevPrint_FindPage(szName);
		if(!page)
			return;
		}

	if(page->m_bVisible == false)
		return;
	
	va_list		ap;								// Pointer To List Of Arguments

	// Make sure we got something to work with.
	if (fmt == NULL)	return;					

	va_start(ap, fmt);							// Parses The String For Variables
		vsprintf(Devformat_text, fmt, ap);			// And Convert Symbols
	va_end(ap);									// 

	// Now call our print function.
	page->m_akDevString.push_back(string(Devformat_text));
}

void ZeroFps::DrawDevStrings()
{
	unsigned int page;

	if(!m_bDevPagesVisible) {
		for(page = 0; page <m_DevStringPage.size(); page++ )
			m_DevStringPage[page].m_akDevString.clear();
		return;
		}

	string strPageName;
		
	glPushAttrib(GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_ALPHA_TEST);

	m_pkRender->SetFont("data/textures/text/devstr.bmp");

	float fYOffset = 0.75;

	for(page = 0; page <m_DevStringPage.size(); page++ ) {
		if(m_DevStringPage[page].m_bVisible == true) {
			strPageName = "[" + m_DevStringPage[page].m_kName + "]";

			m_pkRender->Print(Vector3(-1.1,fYOffset,-1),Vector3(0,0,0),Vector3(0.02,0.02,0.02), 
				const_cast<char*>(strPageName.c_str()));	
			fYOffset -= 0.02;

			for(unsigned int i=0; i<m_DevStringPage[page].m_akDevString.size(); i++) 
			{
				m_pkRender->Print(Vector3(-1.1,fYOffset,-1),Vector3(0,0,0),Vector3(0.02,0.02,0.02), 
					const_cast<char*>(m_DevStringPage[page].m_akDevString[i].c_str()));	
				fYOffset -= 0.02;
			}
		}

		m_DevStringPage[page].m_akDevString.clear();
	}

	glPopAttrib();
}

void ZeroFps::DevPrint_Show(bool bVisible)
{
	m_bDevPagesVisible = bVisible;
}

char g_szIpPort[256];

void ZeroFps::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	unsigned int i;

	vector<string> kFiles;
	vector<string> kCreditsStrings;
	DevStringPage* page;
	
	GameMessage gm;


	switch(cmdid) {
		case FID_SETDISPLAY:
			SetDisplay();
			break;

		case FID_QUIT:
			QuitEngine();
			break;

		case FID_SLIST:
			m_pkConsole->Printf("List of servers");
//			m_pkNetWork->ServerList();
			break;

		case FID_CONNECT:
			if(kCommand->m_kSplitCommand.size() <= 1)
				return;

			m_pkConsole->Printf("Connect to %s", kCommand->m_kSplitCommand[1].c_str());
			//m_pkNetWork->ClientStart(kCommand->m_kSplitCommand[1].c_str());
			
			
			if(strcmp(kCommand->m_kSplitCommand[1].c_str(), "vim") == 0) {
				strcpy(g_szIpPort, "192.168.0.101:4242");
				}
			else if(strcmp(kCommand->m_kSplitCommand[1].c_str(), "hugo") == 0) {
				strcpy(g_szIpPort, "192.168.0.145:4242");
				}
			else if(strcmp(kCommand->m_kSplitCommand[1].c_str(), "dvoid") == 0) {
				strcpy(g_szIpPort, "192.168.0.126:4242");
				}
			else
				sprintf(g_szIpPort, "%s:4242", kCommand->m_kSplitCommand[1].c_str());


			m_pkConsole->Printf("Connect to: %s", g_szIpPort);
			m_pkNetWork->ClientStart(g_szIpPort);


			//m_pkNetWork->ClientStart("192.168.0.7:4242"); // hugo
			//m_pkNetWork->ClientStart("192.168.0.145:4242"); // hugo
			//m_pkNetWork->ClientStart("192.168.0.111:4242");	// me
			m_pkConsole->Printf("FID_CONNECT");
			m_pkApp->OnClientStart();
			m_bClientMode = true;
			break;

		case FID_SERVER:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				m_pkConsole->Printf("You need to give a name for your server.");
				return;
				}

			m_pkConsole->Printf("Start a server with name %s", kCommand->m_kSplitCommand[1].c_str());
			m_pkNetWork->ServerStart();

			m_pkApp->OnServerStart();
			m_bServerMode = true;
				m_bClientMode = true;
			break;
	
		case FID_DIR:
			{
				vector<string> kFiles;

				m_pkBasicFS->ListDir(&kFiles,m_kCurentDir.c_str(),false);
				
				m_pkConsole->Printf("DIRECTORY %s",m_kCurentDir.c_str());
				for(i=0;i<kFiles.size();i++)
				{
					m_pkConsole->Printf(kFiles[i].c_str());
				}
				m_pkConsole->Printf("%i files",kFiles.size());		
				m_pkBasicFS->ListDir(&kFiles,m_kCurentDir.c_str());

				kFiles.clear();
				m_pkConsole->Printf("Listing %s",m_kCurentDir.c_str());
				for(unsigned int i=0;i<kFiles.size();i++)
				{
					m_pkConsole->Printf(kFiles[i].c_str());
				}
			}
			break;
			
		case FID_CD:	
			if(kCommand->m_kSplitCommand.size() <= 1) {
				m_pkConsole->Printf(m_kCurentDir.c_str());
				return;
			}
			
			// special case when entering .. directory
			if(kCommand->m_kSplitCommand[1]=="..")
			{
				for(int i=m_kCurentDir.length()-1;i>0;i--)
				{
					if(m_kCurentDir[i]=='/'){
						m_kCurentDir[i]='\0';						
						m_kCurentDir=m_kCurentDir.c_str();
						break;
					}
				}
				m_pkConsole->Printf(m_kCurentDir.c_str());				
				break;
			}					
				
			//enter a normal directory
			if(m_pkBasicFS->ListDir(&kFiles,(m_kCurentDir+"/"+kCommand->m_kSplitCommand[1]).c_str()))
			{
				m_kCurentDir += "/";
				m_kCurentDir += kCommand->m_kSplitCommand[1];
				m_pkConsole->Printf(m_kCurentDir.c_str());
			} else
			{
				m_pkConsole->Printf("Cant change directory");
				break;
			}
					
			kFiles.clear();
					
			break;

		case FID_ECHO:
			m_pkConsole->Print( kCommand->m_strFullCommand.c_str() + kCommand->m_kSplitCommand[0].size() + 1 );
			break;

		case FID_PRINTOBJECT:
			m_pkObjectMan->DisplayTree();
			break;

		case FID_VERSION:
			m_pkConsole->Printf( ZF_VERSION_NUM );
			break;

		case FID_CREDITS:
			GetEngineCredits(kCreditsStrings);
			for(i=0; i<kCreditsStrings.size(); i++)
				m_pkConsole->Printf(kCreditsStrings[i].c_str());
			break;
		
		case FID_GLDUMP:
			m_pkRender->DumpGLState();
			break;

		case FID_LISTMAD:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				m_pkConsole->Printf("Debug: %%s");
				return;
				}

			m_pkConsole->Printf("Para1 %s", kCommand->m_kSplitCommand[1].c_str());
			break;


		case FID_DEV_SHOWPAGE:
			DevPrintf(kCommand->m_kSplitCommand[1].c_str(), "=)");	// Force creation of page.
			page = DevPrint_FindPage(kCommand->m_kSplitCommand[1].c_str());
			if(page)
				page->m_bVisible = true;
			break;

		case FID_DEV_HIDEPAGE:
			page = DevPrint_FindPage(kCommand->m_kSplitCommand[1].c_str());
			if(page)
				page->m_bVisible = false;
			break;

		case FID_SCREENSHOOT:	m_bCapture = true;	break;

/*			
		case FID_SENDMESSAGE:
			gm.m_FromObject = -1;
			gm.m_ToObject	= atoi(kCommand->m_kSplitCommand[2].c_str());
			gm.m_Name		= kCommand->m_kSplitCommand[1].c_str();
			m_pkConsole->Printf("Sending Msg '%s' to %d from %d", gm.m_Name.c_str(), gm.m_ToObject, gm.m_FromObject);
			m_pkObjectMan->RouteMessage(gm);
			break;*/
	}	
}


void ZeroFps::HandleNetworkPacket(NetPacket* pkNetPacket)
{
	unsigned char ucGamePacketType;
	int	m_iObjectID;

	pkNetPacket->Read(ucGamePacketType);

	while(ucGamePacketType != ZFGP_ENDOFPACKET) {
		if(pkNetPacket->IsReadError())
			return;

		switch(ucGamePacketType) {
			case ZFGP_DELETEOBJECT:
				Logf("net", "HandleNetworkPacket(ZFGP_DELETEOBJECT)\n");
				m_pkObjectMan->UpdateDeleteList(pkNetPacket);
				break;

			case ZFGP_OBJECTSTATE: 
				Logf("net", "HandleNetworkPacket(ZFGP_OBJECTSTATE)\n");
				m_pkObjectMan->UpdateState(pkNetPacket);
				break;

			case ZFGP_CLIENTSTATE: 
				Logf("net", "HandleNetworkPacket(ZFGP_CLIENTSTATE)\n");
				break;
		
			case ZFGP_CLIENTCMD: 
				Logf("net", "HandleNetworkPacket(ZFGP_CLIENTCMD)\n");
				break;
			
			case ZFGP_PRINT: 
				Logf("net", "HandleNetworkPacket(ZFGP_PRINT)\n");
				break;

			case ZFGP_REQOWNOBJECT: 
				pkNetPacket->Read(m_iObjectID);
				m_pkObjectMan->OwnerShip_OnRequest(m_pkObjectMan->GetObjectByNetWorkID( m_iObjectID ));
				break;

			case ZFGP_GIVEOWNOBJECT: 
				pkNetPacket->Read(m_iObjectID);
				m_pkObjectMan->OwnerShip_OnGrant(m_pkObjectMan->GetObjectByNetWorkID( m_iObjectID ));
				break;

			default:
				cout << "Error in game packet : " << (int) ucGamePacketType << endl;
				return;
		}

		pkNetPacket->Read(ucGamePacketType);
		}


}

void ZeroFps::RegisterResources()
{
	m_pkResourceDB->RegisterResource( string(".mad"), Create__Mad_Core		);
	m_pkResourceDB->RegisterResource( string(".tga"), Create__ResTexture	);
	m_pkResourceDB->RegisterResource( string(".bmp"), Create__ResTexture	);
	m_pkResourceDB->RegisterResource( string(".zmt"), Create__Material	);	
}

void ZeroFps::RegisterPropertys()
{
	m_pkPropertyFactory->Register("P_Heightmap2", Create_P_Heightmap2);
	m_pkPropertyFactory->Register("BodyProperty", Create_BodyProperty);
	m_pkPropertyFactory->Register("VegitationProperty", Create_VegitationProperty);
	m_pkPropertyFactory->Register("BoxRenderProperty", Create_BoxRenderProperty);
	m_pkPropertyFactory->Register("BillBoardRenderProperty", Create_BillBoardRenderProperty);
	m_pkPropertyFactory->Register("CrossRenderProperty", Create_CrossRenderProperty);
	m_pkPropertyFactory->Register("MadProperty", Create_MadProperty);
	m_pkPropertyFactory->Register("P_Primitives3D", Create_Prim3DProperty);
	m_pkPropertyFactory->Register("LightProperty", Create_LightProperty);
	m_pkPropertyFactory->Register("WaterRenderProperty",Create_WaterRenderProperty);	
	m_pkPropertyFactory->Register("SkyBoxRenderProperty",Create_SkyBoxRenderProperty);		
	m_pkPropertyFactory->Register("HeightMapRenderProperty",Create_HeightMapRenderProperty);		
	m_pkPropertyFactory->Register("HMRP2",Create_HMRP2);			
	m_pkPropertyFactory->Register("CameraProperty",Create_CameraProperty);			
	m_pkPropertyFactory->Register("ProxyProperty",Create_ProxyProperty);				
	m_pkPropertyFactory->Register("TrackProperty",Create_TrackProperty);				
	m_pkPropertyFactory->Register("LightUpdateProperty",Create_LightUpdateProperty);					
	m_pkPropertyFactory->Register("WorldInfoProperty",Create_WorldInfoProperty);						
	m_pkPropertyFactory->Register("AutoParentProperty",Create_AutoParentProperty);							
	m_pkPropertyFactory->Register("PhysicProperty",Create_PhysicProperty);								
}

void ZeroFps::QuitEngine()
{
	m_iState = state_exit;
}

void ZeroFps::GetEngineCredits(vector<string>& kCreditsStrings)
{
	kCreditsStrings.clear();

	kCreditsStrings.push_back( string("		  ZeroFps Engine		") );
	kCreditsStrings.push_back( string("		       by				") );
	kCreditsStrings.push_back( string("								") );
	kCreditsStrings.push_back( string("   Jimmy 'Vim' Magnusson		") );
	kCreditsStrings.push_back( string("   Martin 'Diz' Kopparhed	") );
	kCreditsStrings.push_back( string("   Richard 'Dvoid' Svensson	") );
	kCreditsStrings.push_back( string("   Erik 'Zeb' Glans			") );
	kCreditsStrings.push_back( string("   Patrik 'Gubb' Sellin		") );
	kCreditsStrings.push_back( string("   Nina 'Nanna3d' Rydqvist	") );
}

bool ZeroFps::PreConnect(IPaddress kRemoteIp, char* szWhy256)
{
	m_pkConsole->Printf("ZeroFps::PreConnect()");
	return true; 
}

void ZeroFps::Connect(int iConnectionID) 
{
	if(!m_bServerMode)
		return;

	m_pkConsole->Printf("ZeroFps::Connect(%d)", iConnectionID);

	char szName[64];
	sprintf(szName, "Player%d", iConnectionID);
	m_kClient[iConnectionID].m_strName = szName;

	m_kClient[iConnectionID].m_pkObject = m_pkObjectMan->CreateObject();//m_pkObjectMan->CreateObjectByArchType("ZeroRTSPlayer");
	assert(m_kClient[iConnectionID].m_pkObject);	
	m_kClient[iConnectionID].m_pkObject->SetWorldPosV(Vector3(0,0,2));

	// Connect all client objects to top level object,
	m_kClient[iConnectionID].m_pkObject->SetParent(m_pkObjectMan->GetWorldObject());
	//m_kClient[iConnectionID].m_pkObject->AttachToClosestZone();
	
	m_kClient[iConnectionID].m_fConnectTime = GetEngineTime();

	m_pkConsole->Printf("Player Object %d", m_kClient[iConnectionID].m_pkObject->iNetWorkID);

	m_pkApp->OnServerClientJoin(&m_kClient[iConnectionID],iConnectionID);

	m_pkObjectMan->m_fEndTimeForceNet = GetEngineTime() + 5.0;
}

void ZeroFps::Disconnect(int iConnectionID)
{
	if(!m_bServerMode)
		return;
	
	m_pkConsole->Printf("ZeroFps::Disconnect(%d)", iConnectionID);
	assert( m_kClient[iConnectionID].m_pkObject );
	
	m_pkApp->OnServerClientPart(&m_kClient[iConnectionID],iConnectionID);	
	
	m_pkObjectMan->Delete( m_kClient[iConnectionID].m_pkObject );
}

int ZeroFps::GetClientObjectID()
{
	if(m_iRTSClientObject == -1) {
		m_pkNetWork->RTS_RequestClientObjectID();
		}

	return m_iRTSClientObject;

}
