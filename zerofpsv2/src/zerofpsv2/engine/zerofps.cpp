#include "psystemmanager.h"
#include "zerofps.h"
#include "network.h"
#include "entitymanager.h"
#include "../render/glguirender.h"
#include "../basic/zfsystem.h"
#include "../gui/zguiresourcemanager.h"
#include "application.h"
#include "../gui/zgui.h"
#include "../basic/zfini.h"
#include "inputhandle.h" 
 
#define	ZF_MIN_PLAYERS	1
#define	ZF_DEF_PLAYERS	8
#define	ZF_MAX_PLAYERS	64

int		g_iNumOfFrames;
int		g_iNumOfMadSurfaces;
float		g_fMadLODScale;
int		g_iMadLODLock;
int		g_iLogRenderPropertys;
//char 		g_szIpPort[256];

static char Devformat_text[4096];	//

Property* Create_P_PfPath();
Property* Create_P_PfMesh();
Property* Create_LinkToJoint();


ZeroFps::ZeroFps(void) : I_ZeroFps("ZeroFps") 
{
	// StartUp SDL
	if(SDL_Init(SDL_INIT_VIDEO)<0)
	{
		g_Logf("Error: Failed to StartUp SDL\n");
	}	

   int iScreenWidth=800, iScreenHeight=600;

   // ZGui beh�ver veta dimensionen p� sk�rmen...
	// ZFIni kINI;
   // char szINIFile[256];
   // sprintf(szINIFile, "%s.ini", Application::m_pkApp->m_pTitle);
	// if(kINI.Open(szINIFile, false)) 
   // {
	//	  iScreenWidth = kINI.GetIntValue("Render", "r_width");
   //   iScreenHeight = kINI.GetIntValue("Render", "r_height");
	//	  kINI.Close();
	// }

	// Create Engine SubSystems 
	m_pkBasicFS					= new ZFBasicFS;	
	m_pkAStar					= new AStar;
	m_pkZShaderSystem			= new ZShaderSystem;
	m_pkEntityManager			= new EntityManager;
	m_pkResourceDB				= new ZFResourceDB;
	m_pkIni						= new ZFIni;
	m_pkGui						= new ZGui(iScreenWidth, iScreenHeight);
	m_pkGuiMan					= new ZGuiResourceManager;
	m_pkGuiRenderer			= new GLGuiRender;
	m_pkNetWork					= new NetWork;
	m_pkAudioSystem			= new ZFAudioSystem;
	m_pkConsole					= new Console;
	m_pkRender					= new Render;
	m_pkLight					= new Light;	
	m_pkFrustum					= new Frustum;	
	m_pkPropertyFactory		= new PropertyFactory;
	m_pkInput					= new Input;		
	m_pkTexMan					= new TextureManager;
	m_pkZFVFileSystem			= new ZFVFileSystem;
	m_pkPSystemManager		= new PSystemManager;
	m_pkScript					= new ZFScriptSystem;
	m_pkTcs						= new Tcs;
	m_pkZShadow					= new ZShadow;

	// Set Default values
	m_fFrameTime				= 0;
	m_fLastFrameTime			= 0;
	m_fSystemUpdateFps		= 30;

	m_fSystemUpdateTime		= 0;
	m_bServerMode				= false;
	m_bClientMode				= false;
	m_bGuiMode					= false;
	m_iMadDraw					= 1;
	g_fMadLODScale				= 1.0;
	g_iMadLODLock				= 0;
	m_pkCamera					= NULL;
	m_bRunWorldSim				= true;
	g_iLogRenderPropertys	= 0;
	m_fAvrageFpsTime			= 0;
	m_iAvrageFrameCount		= 0;
	m_bRenderOn					= true;
	m_bMinimized 				= false;
	m_iServerConnection		= -1;
	m_iMaxPlayers				= ZF_DEF_PLAYERS;
	m_bLockFps					= false;
	m_bDrawAxisIcon			= true;
	m_fEngineTime				= 0;
	m_bDebugGraph				= false;
	m_iClientEntityID			= -1;
	m_bAlwaysWork				= true;
	m_bTcsFullframe			= false;
	m_iProfileTotalTime		= 0;
	
	// Register Variables
	RegisterVariable("r_debuggraph",		&m_bDebugGraph,			CSYS_BOOL);
	RegisterVariable("r_maddraw",			&m_iMadDraw,				CSYS_INT);
	RegisterVariable("r_madlod",			&g_fMadLODScale,			CSYS_FLOAT);
	RegisterVariable("r_madlodlock",		&g_iMadLODLock,			CSYS_FLOAT);
	RegisterVariable("e_systemfps",		&m_fSystemUpdateFps,		CSYS_FLOAT);	
	RegisterVariable("e_runsim",			&m_bRunWorldSim,			CSYS_BOOL);	
	RegisterVariable("r_logrp",			&g_iLogRenderPropertys,	CSYS_INT);
	RegisterVariable("r_render",			&m_bRenderOn,				CSYS_BOOL);
	RegisterVariable("n_maxplayers",		&m_iMaxPlayers,			CSYS_INT,		CSYS_FLAG_SRC_CMDLINE|CSYS_FLAG_SRC_INITFILE);	
	RegisterVariable("e_lockfps",			&m_bLockFps,				CSYS_BOOL);	
	RegisterVariable("r_axis",				&m_bDrawAxisIcon,			CSYS_BOOL);	
	RegisterVariable("p_tcsfullframe",	&m_bTcsFullframe,			CSYS_BOOL);	
	RegisterVariable("e_profile",			g_ZFObjSys.GetProfileEnabledPointer(),			CSYS_BOOL);	
	
	
	
	// Register Commands
	Register_Cmd("setdisplay",FID_SETDISPLAY);
	Register_Cmd("quit",FID_QUIT);
	Register_Cmd("slist",FID_SLIST);
	Register_Cmd("connect",FID_CONNECT);
	Register_Cmd("server",FID_SERVER);
	Register_Cmd("printobject",FID_PRINTOBJECT);	
	Register_Cmd("version",FID_VERSION);	
	Register_Cmd("credits",FID_CREDITS);	
	Register_Cmd("echo",FID_ECHO);	
	Register_Cmd("devshow",FID_DEV_SHOWPAGE, CSYS_FLAG_SRC_ALL, "devshow name", 1);	
	Register_Cmd("devhide",FID_DEV_HIDEPAGE, CSYS_FLAG_SRC_ALL, "devhide name", 1);	
	Register_Cmd("devtog",FID_DEV_TOGGLE, CSYS_FLAG_SRC_ALL, "devtog name", 1);	
	Register_Cmd("debug",FID_LISTMAD);	
	Register_Cmd("shot",FID_SCREENSHOOT);	
	Register_Cmd("mass",FID_MASSSPAWN);	
	Register_Cmd("sc",FID_SERVERCOMMAND);	
	Register_Cmd("pos",FID_POS);	
	
}

ZeroFps::~ZeroFps()
{
	delete m_pkDevPageMaterial;

	m_pkNetWork->ServerEnd();
	g_ZFObjSys.ShutDown();
	ConfigFileSave();

	delete m_pkZShadow;
	delete m_pkTcs;
	delete m_pkIni;
	delete m_pkGui;
	delete m_pkGuiMan;
	delete m_pkGuiRenderer;
	delete m_pkNetWork;
	delete m_pkAudioSystem;
	delete m_pkEntityManager;
	delete m_pkConsole;
	delete m_pkRender;
	delete m_pkLight;
	delete m_pkFrustum;
	delete m_pkPropertyFactory;
	delete m_pkInput;
	delete m_pkTexMan;
	delete m_pkZFVFileSystem;
	delete m_pkBasicFS;
	delete m_pkResourceDB;		//d krashar om denna ligger d�r uppe =(, Dvoid
	delete m_pkPSystemManager;
	delete m_pkScript;
	delete m_pkAStar;
}

bool ZeroFps::StartUp()	
{
	//std lua lib
	StdLua::Init(m_pkScript, m_pkZFVFileSystem );
		 
	m_pkGuiInputHandle = new InputHandle("Gui");
	m_pkInputHandle = new InputHandle("Zerofps");
	m_pkInput->SetActiveInputHandle("Zerofps");
	m_pkInput->AddActiveInputHandle("Gui");
	m_pkInput->AddActiveInputHandle("Application");	
	 
	RegisterPropertys();
	RegisterResources();

	// Valid Console variables.
	if(m_iMaxPlayers < ZF_MIN_PLAYERS)	m_iMaxPlayers =	ZF_MIN_PLAYERS;
	if(m_iMaxPlayers > ZF_MAX_PLAYERS)	m_iMaxPlayers =	ZF_MAX_PLAYERS;

	m_kClient.resize( m_iMaxPlayers );	// Vim - Hard coded for now. Must be same as Network.SetMaxNodes
	for(int i=0; i<m_iMaxPlayers; i++)
		m_kClient[i].m_pkObject = NULL;
	m_pkNetWork->SetMaxNodes( m_iMaxPlayers );
	

	//m_iClientEntityID = -1;

	m_bDevPagesVisible = true;

	//setup default console camera
	m_pkConsoleCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),84,1.333,0.3,250);	
	m_pkConsoleCamera->SetClearViewPort(false);
	
	//setup gui stuff
	m_pkGui->SetRes(m_pkRender->GetWidth(), m_pkRender->GetHeight());
	m_pkApp->m_iWidth = m_pkRender->GetWidth();
	m_pkApp->m_iHeight = m_pkRender->GetHeight();
	
	if(m_pkGuiRenderer->SetDisplay(m_pkRender->GetWidth(),m_pkRender->GetHeight()) == false)
	{
		printf("Failed to set GUI display!\n");
	}
	
	//create materials
	CreateMaterials();
	
	//set captured input as default
	//m_pkInput->ToggleGrab(true);
	
	return true;
}

bool ZeroFps::ShutDown()	{	return true;	}
bool ZeroFps::IsValid()		{	return true;	}
	
void ZeroFps::SetApp() 
{
	m_pkApp=Application::m_pkApp;
	m_pkApp->SetEnginePointer(this);
}

void ZeroFps::ConfigFileRun()
{
	string CfgName = string("/") + string(m_pkApp->m_pTitle) + ".ini";
	g_ZFObjSys.Config_Load(CfgName);
}

void ZeroFps::ConfigFileSave()
{
	string CfgName = string(m_pkApp->m_pTitle) + ".ini";
	g_ZFObjSys.Config_Save(CfgName);
}

bool ZeroFps::Init(int iNrOfArgs, char** paArgs)
{	
	SetApp();									//	setup class pointers	
	ConfigFileRun();
	g_ZFObjSys.HandleArgs(iNrOfArgs,paArgs);	//	handle arguments
	
	atexit(SDL_Quit);

/*#ifdef _WIN32
	RenderDLL_InitExtGL();
	extgl_Initialize();
#endif*/

	if(!g_ZFObjSys.StartUp())
		return false;

	m_iState=state_normal;									// init gamestate to normal		
	m_pkApp->OnInit();										// call the applications oninit funktion

	m_pkRender->Swap();
	
	m_fFrameTime		=0;
	m_fLastFrameTime	= GetTicks();
	m_fLockFrameTime  = GetTicks();

	return true;
}

void ZeroFps::CreateMaterials()
{
	//devpage material
	m_pkDevPageMaterial = new ZMaterial;
	m_pkDevPageMaterial->GetPass(0)->m_kTUs[0]->SetRes("data/textures/text/devstr.bmp");
	m_pkDevPageMaterial->GetPass(0)->m_iPolygonModeFront = 	FILL_POLYGON;
	m_pkDevPageMaterial->GetPass(0)->m_iCullFace = 				CULL_FACE_BACK;		
	m_pkDevPageMaterial->GetPass(0)->m_bLighting = 				false;		
	m_pkDevPageMaterial->GetPass(0)->m_bColorMaterial = 		false;
	m_pkDevPageMaterial->GetPass(0)->m_bFog = 					false;		
	

}

void ZeroFps::UpdateDevPages()
{
	m_pkNetWork->DevShow_ClientConnections();

	DevPrintf("conn","Num of Clients: %d", m_pkNetWork->GetNumOfClients());
	if(m_bServerMode) 
	{
		for(int i=0; i<4; i++) 
		{
			DevPrintf("conn","Client[%d]: %s", i, m_kClient[i].m_strLogin.c_str());
			// Server gives upp object a time after connection

			if(m_kClient[i].m_pkObject) 
			{
				if(GetEngineTime() > (m_kClient[i].m_fConnectTime + 5))
					m_pkEntityManager->OwnerShip_Give( m_kClient[i].m_pkObject );
			}
		}
	}

	//some devpage stuff
	DevPrintf("common","ENTITYMAN:");	
	DevPrintf("common","  Num Objects     : %d", m_pkEntityManager->GetNumOfEntitys());
	DevPrintf("common","  NextEntityID    : %d", m_pkEntityManager->GetNextEntityID());
	DevPrintf("common","  Active Propertys: %d",m_pkEntityManager->GetActivePropertys());		
	
	DevPrintf("common","RESOURCEMAN:");
	DevPrintf("common","  Res Size    : %d", m_pkResourceDB->GetResSizeInBytes());
		
	DevPrintf("common","ZSHADOW:");
	DevPrintf("common","  shadows       : %d", m_pkZShadow->GetCurrentShadows());
	DevPrintf("common","  active shadows: %d", m_pkZShadow->GetCurrentActiveShadows());
	DevPrintf("common","  shadow verts  : %d", m_pkZShadow->GetCurrentVerts());
	DevPrintf("common","  shadow buffert: %d", m_pkZShadow->GetBuffertSize());

	DevPrintf("common","TCS:");
	DevPrintf("common","  Collissions : %d", m_pkTcs->GetNrOfCollissions());
	DevPrintf("common","  Tests       : %d", m_pkTcs->GetNrOfTests());
	DevPrintf("common","  ActiveBodies: %d", m_pkTcs->GetNrOfActiveBodies());
	
	DevPrintf("common","ZSHADER:");
	DevPrintf("common","  MaterialBinds  : %d", m_pkZShaderSystem->GetMaterialBinds());
	DevPrintf("common","  MaterialReloads: %d", m_pkZShaderSystem->GetMaterialReloads());
	DevPrintf("common","  SavedReloads   : %d", m_pkZShaderSystem->GetSavedReloads());
	DevPrintf("common","  GLUpdates      : %d", m_pkZShaderSystem->GetGLupdates());
	DevPrintf("common","  Total Vertises : %d", m_pkZShaderSystem->GetTotalVertises());
	DevPrintf("common","  Vertises/Sec   : %d", int(m_pkZShaderSystem->GetTotalVertises() * m_fAvrageFps) );
	m_pkZShaderSystem->ResetStatistics();	
	
	DevPrintf("common","FPS:");
	DevPrintf("common","  Fps       : %f",m_fFps);	
	DevPrintf("common","  Avrage Fps: %f",m_fAvrageFps);			
	

	// Describe Active Cam.
	if(GetCam()) 
	{
		string strCamDesc = GetCam()->GetCameraDesc();
		DevPrintf("common",strCamDesc.c_str());
	}

	DevPrintf("common" , "NumMads/NumMadSurfaces: %d / %d", m_iNumOfMadRender , g_iNumOfMadSurfaces);
	DevPrintf("common" , "Zone: %d", this->m_pkEntityManager->m_kZones.size());

	// TIME
	DevPrintf("time","Ticks: %f",							GetTicks());
	DevPrintf("time","FrameTime: %f",					GetFrameTime());
	DevPrintf("time","SimTime: %f",						m_pkEntityManager->GetSimTime());
	DevPrintf("time","SimDelta: %f",						m_pkEntityManager->GetSimDelta());
	DevPrintf("time","LastGameUpdateTime: %f",		GetLastGameUpdateTime());
	DevPrintf("time","EngineTime: %f",					GetEngineTime());
	DevPrintf("time","Run: %i", m_bRunWorldSim);

	m_iNumOfMadRender = 0;
	g_iNumOfMadSurfaces = 0;
		
		

		
	
	if(m_iProfileTotalTime != 0)
	{
		//profiling
		for(int i = 0;i<m_kProfileData.size();i++)
		{
			char temp[3];
			temp[0] = '#';
			temp[1] = int(( float(m_kProfileData[i].m_iTime) / float(m_iProfileTotalTime) ) *100.0);
			temp[2] = '/0';		
			//DevPrintf("profile","%s : %dns / %dms / %d%%",	m_kProfileData[i].m_strName.c_str(), int((m_kProfileData[i].m_iTime / m_fAvrageFps)*1000.0) , m_kProfileData[i].m_iTime, int(( float(m_kProfileData[i].m_iTime) / float(m_iProfileTotalTime) ) *100.0));			
			DevPrintf("profile","%s : %dms / %d%%",	m_kProfileData[i].m_strName.c_str(), m_kProfileData[i].m_iTime, int(( float(m_kProfileData[i].m_iTime) / float(m_iProfileTotalTime) ) *100.0));			
			DevPrintf("profile",temp);
		}	
	} 
	
}

void ZeroFps::UpdateGuiInput()
{
	   // Updata Gui input
	   int mx, my;
	   if(m_pkGui->m_bUseHardwareMouse == true)
		   m_pkGuiInputHandle->SDLMouseXY(mx,my);
	   else
		{
		   float x,y;
			m_pkGuiInputHandle->MouseXY(x,y);
			mx = x;
			my = y;
		}
			
	   m_pkGui->UpdateMouse(mx, my,
         m_pkGuiInputHandle->Pressed(MOUSELEFT),
         m_pkGuiInputHandle->Pressed(MOUSERIGHT),
		   m_pkGuiInputHandle->Pressed(MOUSEMIDDLE), 
         GetTicks());

      vector<ZGui::KEY_INFO> vkKeyInfo;

      while(m_pkGuiInputHandle->SizeOfQueue() > 0)
      {
         QueuedKeyInfo kKey = m_pkGuiInputHandle->GetQueuedKey();

         ZGui::KEY_INFO kKeyInfo;
         kKeyInfo.key = kKey.m_iKey;
         kKeyInfo.pressed = kKey.m_bPressed;
         kKeyInfo.shift = (kKey.m_iModifiers & MODIFIER_SHIFT);
         vkKeyInfo.push_back(kKeyInfo);
      }

      m_pkGui->UpdateKeys(vkKeyInfo, GetTicks());

	   //disablar applicationens input om guit har hanterat den	
	   if(m_pkGui->m_bHandledMouse == true)
			m_pkApp->m_pkInputHandle->SetTempDisable(true);
	   else
		   m_pkApp->m_pkInputHandle->SetTempDisable(false);
}

/* Code that need to run on both client/server. */
void ZeroFps::Run_EngineShell()
{
	//update devpages
	UpdateDevPages();

	//run application main loop
	m_pkApp->OnIdle();	
	
	// Update Local Input.
	m_pkInput->Update();

	// backquote Always handle console.
	if(m_pkInputHandle->Pressed(KEY_BACKQUOTE))
	{		
		m_pkConsole->Toggle();
	}

	//is the console active? , else update gui
	if(m_pkConsole->IsActive()) 
	{		
		m_pkConsole->Update();
	}
   else
   {
		StartProfileTimer("s__input");	
	
			UpdateGuiInput();

		StopProfileTimer("s__input");	
   }

	//update delete
	m_pkEntityManager->UpdateDelete();
	

	//for debuging the input system
	if(m_pkInputHandle->Pressed(KEY_F9))	GetSystem().RunCommand("shot",CSYS_SRC_SUBSYS);	
	if(m_pkInputHandle->Pressed(KEY_F10))	m_pkInput->ToggleGrab();
	if(m_pkInputHandle->Pressed(KEY_F11))	ToggleFullScreen();		

}

void ZeroFps::Run_Server()
{
	//update system
	Update_System();

	if(m_bTcsFullframe)
		m_pkTcs->Update(GetFrameTime());	
	
}

void ZeroFps::Run_Client()
{
	if(!m_bServerMode)
		Update_System();	
	
	//update sound system
	if(m_kRenderCamera.size()  == 1)
		m_pkAudioSystem->SetListnerPosition(m_kRenderCamera[0]->GetPos(),m_kRenderCamera[0]->GetRotM());
	//m_pkAudioSystem->SetListnerPosition(m_pkCamera->GetPos(),m_pkCamera->GetRotM());
	
	StartProfileTimer("s__Sound");
	m_pkAudioSystem->Update();
	StopProfileTimer("s__Sound");
}

void ZeroFps::Update_System()
{
	int iLoops;
	float fRestTime = 0;

	//calculate new system delta time
	m_fSystemUpdateFpsDelta = float(1.0) / m_fSystemUpdateFps;	
		
	//time since last update
	float fATime = GetTicks() - m_fSystemUpdateTime; 	
	
	//how many system loops shuld we make?
	iLoops = int(fATime / m_fSystemUpdateFpsDelta);		
			
	//save rest time to add it att end of function
	fRestTime = fATime - (iLoops * m_fSystemUpdateFpsDelta);				

			
	//if no loops are to be done, just return
	if(iLoops<=0)
		return;
	
	//set maximum number of loops, dont know if this is realy that good...but what the hell
	if(iLoops > 10)
	{
		cout<<"engine runs to slow (try kicking your computer and punching your screen for better performance)"<<endl;
		iLoops = 10;
	}
	

	/*
	static float time = GetTicks();
	static float tStart = GetTicks();
	static int frames = 0;
	time =GetTicks();	
	if(time -tStart >= 1.0)
	{
		cout<<"frames:"<<frames<<endl;
		frames = 0;
		tStart = time;
			
	}
	frames+=iLoops;
	*/
		
	for(int i=0;i<iLoops;i++)
	{	
	
		//update sim time for this systemupdate
		m_pkEntityManager->UpdateSimTime();
		
		//update network for client & server
		m_pkNetWork->Run();				
		
		//update application systems
		m_pkApp->OnSystem();
		
		//server only code
		if(m_bServerMode)
		{
			//update zones
			m_pkEntityManager->UpdateZoneSystem();
		
			if(m_bRunWorldSim)
			{						
				//update all normal propertys
				m_pkEntityManager->Update(PROPERTY_TYPE_NORMAL,PROPERTY_SIDE_SERVER,false);
				
				//update Tiny Collission system
				if(!m_bTcsFullframe)
					m_pkTcs->Update(m_pkEntityManager->GetSimDelta());	
			}	
		}
	

		//client only code
		if(m_bClientMode)
		{			
			//update normal propertys
			m_pkEntityManager->Update(PROPERTY_TYPE_NORMAL,PROPERTY_SIDE_CLIENT,false);					
		}
		
		//client & server code

		//update game message system				
		m_pkEntityManager->UpdateGameMessages();
		
				
		//pack objects to clients
		m_pkEntityManager->PackToClients();		
		
		//delete objects
		m_pkEntityManager->UpdateDelete();
				
		//update the resource manager
		m_pkResourceDB->Refresh();

		//save current update time, 
		m_fSystemUpdateTime = GetTicks();
	}

	//finaly add rest time
	m_fSystemUpdateTime -= fRestTime;
	
}

void ZeroFps::Draw_EngineShell()
{
	StartProfileTimer("r_Render");					

	//render cameras	
	Draw_RenderCameras();
	
	//render gui
	if(!m_bMinimized)	
		m_pkGui->Render((int)m_fAvrageFps);
	
		
	//set console kamera matrisses, and clear depthbuffer
	m_pkConsoleCamera->InitView();
	
	//draw devstrings
	DrawDevStrings();
	
	//on hud drawing
	m_pkApp->OnHud();

	//draw console
	if(!m_bMinimized)
		if(m_pkConsole->IsActive()) 
			m_pkConsole->Draw();

			
	StopProfileTimer("r_Render");				
}

void ZeroFps::MainLoop(void) 
{

	while(m_iState!=state_exit) 
	{

		// check if app is iconized
		if(m_bAlwaysWork == false && !(SDL_GetAppState() & SDL_APPACTIVE))
		{
			SDL_WaitEvent(NULL);
		}
		else
		{
			//current time
			m_fEngineTime = GetTicks();
			 			 
			//check if application is minimized
			m_bMinimized = ( !(SDL_GetAppState() & SDL_APPACTIVE) );	

			//handle locked fps delay						
			MakeDelay();			
			
			//update basic engine systems			
			Run_EngineShell();
			
			//update server only systems			
			StartProfileTimer("s_System");			
			if(m_bServerMode)
				Run_Server();		
				
			//update client only systems
			if(m_bClientMode)
				Run_Client();		
			StopProfileTimer("s_System");							
				
			//render stuff			
			Draw_EngineShell();			
			
			//swap buffers n calculate fps
			Swap();			
		}		
	}
}

void ZeroFps::MakeDelay()
{
	//make a delay if locked fps or minimized
	if(m_bLockFps || m_bMinimized  )
	{
		float fDelay = m_pkEntityManager->GetSimDelta() - (GetTicks() - m_fLockFrameTime);
	
		if(fDelay < 0)
			fDelay = 0;

		SDL_Delay((int)(fDelay*1000.0f));	
		
		m_fLockFrameTime = GetTicks();

		//	cout<<"Frametime:"<<fFrameT<<endl;
		//	cout<<"Frametime shuld be:"<<pkFps->GetGameFrameTime()<<endl;
		//	cout<<"Delaying:"<<fDelay<<endl;		
		//end of delay code ---				
	}
}

void ZeroFps::AddRenderCamera(Camera* pkCamera)
{
	for(unsigned int i=0; i<m_kRenderCamera.size(); i++)
		if(m_kRenderCamera[i] == pkCamera)
			return;

	m_kRenderCamera.push_back(pkCamera);
	//cout << "Adding: " << pkCamera->GetName();
	//cout << " to active rendertargets\n";
}

void ZeroFps::ClearRenderCameras()
{
	m_kRenderCamera.clear();
}

void ZeroFps::RemoveRenderCamera(Camera* pkCamera)
{
	for(vector<Camera*>::iterator it=m_kRenderCamera.begin();it!=m_kRenderCamera.end();it++) 
	{
		if(pkCamera == (*it))
			m_kRenderCamera.erase(it);
	}	
}

void ZeroFps::Draw_RenderCameras()
{
	StartProfileTimer("r__RenderCameras");	

	for(unsigned int i=0; i<m_kRenderCamera.size(); i++)
	{		
		m_kRenderCamera[i]->RenderView();
	}
	
	StopProfileTimer("r__RenderCameras");	
}

Camera* ZeroFps::GetRenderCamera(string strName)
{
	for(unsigned int i=0; i<m_kRenderCamera.size(); i++)
	{
		if(m_kRenderCamera[i]->GetName() == strName)
			return m_kRenderCamera[i];
	}

	return NULL;
}




void ZeroFps::Swap(void) {

	m_pkRender->Swap();

	//count FPS
	m_fFrameTime=SDL_GetTicks()-m_fLastFrameTime;
	m_fLastFrameTime=(float)SDL_GetTicks();
	m_fFps=1000.0f/m_fFrameTime;	

	m_iAvrageFrameCount++;
	
	//each 1 seccond
	if( (GetTicks() - m_fAvrageFpsTime) >1)
	{
		//update avrage fps
		m_fAvrageFps = (float) m_iAvrageFrameCount;
		m_iAvrageFrameCount = 0;
		m_fAvrageFpsTime = GetTicks();
				
		//update profile information
		#ifndef NOPROFILE
		if(g_ZFObjSys.GetProfileEnabled())
		{
			m_kProfileData.clear();					
			g_ZFObjSys.GetProfileTimers(&m_kProfileData);
			m_iProfileTotalTime = g_ZFObjSys.GetTotalTime();		
			g_ZFObjSys.ClearProfileTimers();			
		}
		#endif
	} 
	 
		
	
	
#ifdef RUNPROFILE
	g_iNumOfFrames++;
//	if(g_iNumOfFrames >= 1000)
//		m_iState = state_exit;

#endif
}


void ZeroFps::ToggleFullScreen(void)
{
	m_pkRender->ToggleFullScreen();
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

bool ZeroFps::DevPrintPageVisible(const char* szName)
{
	for(unsigned int i=0; i<m_DevStringPage.size(); i++) 
	{
		if(string(szName) == m_DevStringPage[i].m_kName) 
		{
			return m_DevStringPage[i].m_bVisible;
		}
	}

	return false;
}

DevStringPage*	ZeroFps::DevPrint_FindPage(const char* szName)
{
	for(unsigned int i=0; i<m_DevStringPage.size(); i++) 
	{
		if(string(szName) == m_DevStringPage[i].m_kName) 
		{
			return &m_DevStringPage[i];
		}
	}

	return NULL;
}

void ZeroFps::DevPrintf(const char* szName, const char *fmt, ...)
{
	// Find the page to print to.
	DevStringPage* page = DevPrint_FindPage(szName);
	if(!page) 
	{
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
	/*
	if(strlen(Devformat_text) >= 200)
		assert(0);
	*/

	//add null value if string is to long to shorten it
	if(strlen(Devformat_text) >= 200)
		Devformat_text[199] = '\0';	
	
	page->m_akDevString.push_back(string(Devformat_text));
}

void ZeroFps::DrawDevStrings()
{
	static int iGraphTexture = m_pkTexMan->Load("data/textures/graph.bmp");
	static int iMaxTexture = m_pkTexMan->Load("data/textures/notex.bmp");

	unsigned int page;
	if(!m_bDevPagesVisible) 
	{
		//no visible page, just clear the pages
		for(page = 0; page <m_DevStringPage.size(); page++ )
			m_DevStringPage[page].m_akDevString.clear();
		return;
	}
	

	//bind materail
	m_pkZShaderSystem->BindMaterial(m_pkDevPageMaterial);

	//loop all pages and print em
	static float fSize = 0.022;
	string strPageName;
	float fYOffset = 0.75;
	for(page = 0; page <m_DevStringPage.size(); page++ ) 
	{
		if(m_DevStringPage[page].m_bVisible == true) 
		{
			strPageName = "[" + m_DevStringPage[page].m_kName + "]";
			m_pkRender->Print(Vector3(-1.1,fYOffset,-1),strPageName.c_str(),fSize);
			fYOffset -= fSize;

			for(unsigned int i=0; i<m_DevStringPage[page].m_akDevString.size(); i++) 
			{
				//is this a graph?
				if(m_DevStringPage[page].m_akDevString[i][0] == '#')
				{
					if(m_DevStringPage[page].m_akDevString[i].length() >= 2)
					{
						int iVal = int(m_DevStringPage[page].m_akDevString[i][1]);
						if(iVal > 100)
							iVal = 100;
											
						float fPos = -1.1 + (iVal/100.0);
						m_pkRender->Polygon4(Vector3(-1.1,fYOffset,-1),Vector3(fPos,fYOffset,-1),Vector3(fPos,fYOffset+fSize,-1),Vector3(-1.1,fYOffset+fSize,-1),  iGraphTexture);
						m_pkRender->Polygon4(Vector3(-0.1,fYOffset,-1),Vector3(-0.09,fYOffset,-1),Vector3(-0.09,fYOffset+fSize,-1),Vector3(-0.1,fYOffset+fSize,-1),iMaxTexture);
					}
				}
				else
				{			
					m_pkRender->Print(Vector3(-1.1,fYOffset,-1),m_DevStringPage[page].m_akDevString[i].c_str(),fSize);
				}
				//increse position
				fYOffset -= fSize;
			}		
		}
		m_DevStringPage[page].m_akDevString.clear();
	}
}

void ZeroFps::DevPrint_Show(bool bVisible)
{
	m_bDevPagesVisible = bVisible;
}

void ZeroFps::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	unsigned int i;

	//vector<string> kFiles;
	vector<string> kCreditsStrings;
	DevStringPage* page;
	
	GameMessage gm;

	Entity*	pkEnt;
	P_Mad*	pkMad;
	string	strMad("/data/mad/");

	switch(cmdid) {
		case FID_SETDISPLAY:
			m_pkRender->SetDisplay();
			break;

		case FID_QUIT:
			QuitEngine();
			break;

		case FID_SLIST:
			m_bClientMode = false;
//			m_pkConsole->Printf("List of servers");
//			m_pkNetWork->ServerList();
			break;

		case FID_CONNECT:
		{
			if(kCommand->m_kSplitCommand.size() <= 1) 
				return;
		
			string strLogin = "anonymous";
			string strPass  = "nopass";
							
			//login
			if(kCommand->m_kSplitCommand.size() >= 3)
				strLogin = kCommand->m_kSplitCommand[2];

			//password
			if(kCommand->m_kSplitCommand.size() >= 4)
				strPass = kCommand->m_kSplitCommand[3];
			
			
			//m_pkConsole->Printf("Connecting: %s, %s, %s", g_szIpPort,strLogin.c_str(), strPass.c_str());			
			StartClient(strLogin,strPass,kCommand->m_kSplitCommand[1].c_str(),4242);

			break;
		}
			
		case FID_SERVER:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				m_pkConsole->Printf("You need to give a name for your server.");
				return;
			}

			m_pkConsole->Printf("Start a server with name %s", kCommand->m_kSplitCommand[1].c_str());

			//start server
			StartServer(true,true,4242,kCommand->m_kSplitCommand[1].c_str());
			break;
			
		case FID_ECHO:
			m_pkConsole->Printf( kCommand->m_strFullCommand.c_str() + kCommand->m_kSplitCommand[0].size() + 1 );
			break;

		case FID_PRINTOBJECT:
			m_pkEntityManager->DisplayTree();
			break;

		case FID_VERSION:
			m_pkConsole->Printf( ZF_VERSION_NUM );
			break;

		case FID_CREDITS:
			GetEngineCredits(kCreditsStrings);
			for(i=0; i<kCreditsStrings.size(); i++)
				m_pkConsole->Printf(kCreditsStrings[i].c_str());
			break;
		
		case FID_LISTMAD:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				m_pkConsole->Printf("Debug: %%s");
				return;
				}
			
			pkEnt = this->m_pkEntityManager->CreateEntityFromScriptInZone("data/script/objects/t_xw.lua",Vector3(0,0,0));
			pkEnt->AddProperty("P_Event");
			pkEnt->AddProperty("P_Item");
			pkEnt->AddProperty("P_Ml");

			pkMad = dynamic_cast<P_Mad*>(pkEnt->GetProperty("P_Mad"));
			strMad += kCommand->m_kSplitCommand[1];
			pkMad->SetBase(strMad.c_str());
			pkMad->SetScale(1.0);
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

		case FID_DEV_TOGGLE:
			page = DevPrint_FindPage(kCommand->m_kSplitCommand[1].c_str());
			if(page)
				page->m_bVisible = !page->m_bVisible;
			break;

		case FID_SCREENSHOOT:	m_pkRender->ScreenShot();	break;

		case FID_SERVERCOMMAND:
			cout << "Servercommand" << endl;
			PrintToClient(0, "Yooo liksom");
			break;

		case FID_POS:
			Vector3 kCamPos = m_pkCamera->GetPos();
			cout << "Position - x:" << kCamPos.x << 
				" y:" << kCamPos.y <<
				" z:" << kCamPos.z << endl;
			m_pkCamera->SetPos(kCamPos);
			break;
	}	
}


void ZeroFps::StartServer(bool bClient,bool bNetwork,int iPort,string strServerName)
{
	//reset all first
	StopAll();
	
	if(bNetwork)
		m_pkNetWork->ServerStart(iPort);
	
	m_bServerMode = true;		
	if(bClient)
		m_bClientMode = true;

	m_pkApp->OnServerStart();
}

void ZeroFps::StartClient(string strLogin,string strPassword,string strServerIP,int iPort)
{
	//reset all first
	StopAll();

	//add a delay between disconnection and trying to connect again
	SDL_Delay(1000);
	
	//clear world   ---detta kan vara ganska evil ibland =D, 
	m_pkEntityManager->Clear();
	
	// DNS LookUp.
/*	if( !m_pkNetWork->IsValidIPAddress(strServerIP.c_str()) )
	{
		IPaddress kLookUpIP;
		if(m_pkNetWork->DnsLookUp( strServerIP.c_str(),kLookUpIP ))
		{
			kLookUpIP.port = iPort;
			m_pkNetWork->AddressToStr(&kLookUpIP, g_szIpPort);
			cout << "StartClient IP = " << strServerIP << endl;
		}
	}
	else
	{*/
//		cout << "StartClient IP = " << strServerIP << endl;
//		sprintf(g_szIpPort, "%s:%d", strServerIP.c_str(),iPort);
//	}
	
//	cout << "g_szIpPort = " << g_szIpPort << endl;

	
	m_pkNetWork->ClientStart(strServerIP.c_str(), strLogin.c_str(), strPassword.c_str(), m_pkApp->m_bIsEditor);
	m_bClientMode = true;

	m_pkApp->OnClientStart();
}

void ZeroFps::StopAll()
{
	m_pkNetWork->DisconnectAll();
	m_pkNetWork->CloseSocket();
	
	m_bServerMode = false;
	m_bClientMode = false;
}


void ZeroFps::PrintToClient(int iConnectionID, const char* szMsg)
{
	NetPacket kNp;
	kNp.Clear();
	kNp.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
	kNp.Write((char) ZFGP_PRINT);
	kNp.Write_Str(szMsg);
	kNp.Write(ZFGP_ENDOFPACKET);
	kNp.TargetSetClient(iConnectionID);
	m_pkNetWork->Send2(&kNp);
}

void ZeroFps::HandleNetworkPacket(NetPacket* pkNetPacket)
{
	unsigned char ucGamePacketType;
	int	m_iObjectID;
	char	szMsg[1024];

	pkNetPacket->Read(ucGamePacketType);

	while(ucGamePacketType != ZFGP_ENDOFPACKET)
	{
		if(pkNetPacket->IsReadError())
			return;

		switch(ucGamePacketType) 
		{			
			case ZPGP_DELETELIST:
				m_pkEntityManager->HandleDeleteQueue(pkNetPacket);
				break;			
				
			case ZFGP_OBJECTSTATE: 
				//Logf("netpac", "  HandleNetworkPacket(ZFGP_OBJECTSTATE)\n");
				m_pkEntityManager->UpdateState(pkNetPacket);
				break;

			case ZFGP_CLIENTSTATE: 
				//Logf("net", "HandleNetworkPacket(ZFGP_CLIENTSTATE)\n");
				break;
		
			case ZFGP_CLIENTCMD: 
				//Logf("net", "HandleNetworkPacket(ZFGP_CLIENTCMD)\n");
				break;
			
			case ZFGP_PRINT: 
				pkNetPacket->Read_Str(szMsg);
				m_pkConsole->Printf(szMsg);
				//Logf("net", "HandleNetworkPacket(ZFGP_PRINT)\n");
				break;

			case ZFGP_REQOWNOBJECT: 
				pkNetPacket->Read(m_iObjectID);
				m_pkEntityManager->OwnerShip_OnRequest(m_pkEntityManager->GetEntityByID( m_iObjectID ));
				break;

			case ZFGP_GIVEOWNOBJECT: 
				pkNetPacket->Read(m_iObjectID);
				m_pkEntityManager->OwnerShip_OnGrant(m_pkEntityManager->GetEntityByID( m_iObjectID ));
				break;
				
			case ZFGP_EDIT:
				HandleEditCommand(pkNetPacket);
				break;

			case ZPGP_SS_APP:
				m_pkApp->OnNetworkMessage( pkNetPacket );
				break;

			
			/*
			case ZFGP_ZONELIST: 
				//Logf("netpac", "  HandleNetworkPacket(ZFGP_ZONELIST)\n");
				m_pkEntityManager->UpdateZoneList(pkNetPacket);
				break;

			case ZFGP_GETSTATICDATA: 
				m_pkEntityManager->StaticData(pkNetPacket->m_iClientID, pkNetPacket);
				break;
			
			case ZFGP_DELETEOBJECT:
			{
				Logf("net", "HandleNetworkPacket(ZFGP_DELETEOBJECT)\n");
				int iID=-1;
				pkNetPacket->Read(iID);
				m_pkEntityManager->Delete(iID);
				cout<<"Got delete packate from server , deleting entity:"<<iID<<endl;
				//m_pkEntityManager->UpdateDeleteList(pkNetPacket);
				break;
			}
			*/			
			
			default:
				cout << "Error in game packet : " << (int) ucGamePacketType << endl;
				return;
		}

		pkNetPacket->Read(ucGamePacketType);
	}
}

void ZeroFps::HandleEditCommand(NetPacket* pkNetPacket)
{
	char 		szCmd[512];
	Vector3 	kPos;
	int 		iEntId;
	Vector3 	kMove;

	pkNetPacket->Read_Str(szCmd);

	if ( szCmd == string("spawn"))
	{
		pkNetPacket->Read_Str(szCmd);
		pkNetPacket->Read(kPos);
		m_pkEntityManager->CreateEntityFromScriptInZone( szCmd, kPos);
	}

	if( szCmd == string("move"))
	{
		pkNetPacket->Read(iEntId);
		pkNetPacket->Read(kMove);
		
		if(Entity* pkObj = m_pkEntityManager->GetEntityByID(iEntId))
			pkObj->SetLocalPosV(pkObj->GetLocalPosV() + kMove);
	}

	if( szCmd == string("use"))
	{
		pkNetPacket->Read(iEntId);
		
		if(Entity* pkObj = m_pkEntityManager->GetEntityByID(iEntId))
			m_pkEntityManager->CallFunction(pkObj, "Useit",NULL);;
	}


	if( szCmd == string("setpos"))
	{
		pkNetPacket->Read(iEntId);
		pkNetPacket->Read(kMove);
		Entity* pkObj = m_pkEntityManager->GetEntityByID(iEntId);								
		if(!pkObj)
			return;	
		pkObj->SetLocalPosV(kMove);
	}

	if( szCmd == string("rot"))
	{
		pkNetPacket->Read(iEntId);
		pkNetPacket->Read(kMove);
		Entity* pkObj = m_pkEntityManager->GetEntityByID(iEntId);								
		if(!pkObj)
			return;	
		pkObj->RotateLocalRotV(kMove);
	}

	if( szCmd == string("del"))
	{
		int iNumOfId;
		pkNetPacket->Read(iNumOfId);

		for(int i=0; i<iNumOfId; i++) 
		{
			pkNetPacket->Read(iEntId);
			
			if(Entity* pkEntity = m_pkEntityManager->GetEntityByID(iEntId))
			{
				cout << " " << pkEntity->GetEntityID() << " - '" << pkEntity->GetType() << "' - '" << pkEntity->GetName() << "'" <<endl;				
				//remove zone entity or normal entity
				if(pkEntity->IsZone())
				{
					int iZoneID = m_pkEntityManager->GetZoneIndex( pkEntity->GetEntityID() );
				
					//send deleted zone to clients
					SendZoneList(true,ZF_NET_ALLCLIENT,iZoneID);
					
					m_pkEntityManager->DeleteZone(iZoneID);
					cout << "Delete zone " << iZoneID << endl;
					
				}
				else
				{					
					m_pkEntityManager->Delete(iEntId);	
				}
			}
		}
	}
	
	if( szCmd == string("create_zone") )
	{
		string 	strZoneName;
		Vector3	kZoneMarkerPos;
		Vector3	kZoneSize;
		Vector3	kModelRot;
		
		pkNetPacket->Read_Str(strZoneName);
		pkNetPacket->Read(kZoneMarkerPos);		
		pkNetPacket->Read(kZoneSize);
		pkNetPacket->Read(kModelRot);
	
		//cout<<"creating zone:"<<strZoneName<< " size:"<<kZoneSize.x<<" "<<kZoneSize.y<<" "<<kZoneSize.z<< "  position:"<<kZoneMarkerPos.x<<" "<<kZoneMarkerPos.y<<" "<<kZoneMarkerPos.z<<endl;
	
		//check if colliding with any other zone
		if(m_pkEntityManager->IsInsideZone(kZoneMarkerPos, kZoneSize))
			return;		
	
		//create zone
		int id = m_pkEntityManager->CreateZone(kZoneMarkerPos, kZoneSize);
		
		//force loading of this zone
		m_pkEntityManager->LoadZone(id);
		
		if(id != -1)
		{
			//send added zone to client
			SendZoneList(false,ZF_NET_ALLCLIENT,id);
		
			if(strZoneName.length() != 0)
				m_pkEntityManager->SetZoneModel(strZoneName.c_str(),id);
			
			
			RotateZoneModel(id,kModelRot);
		}						
	}
	
	if( szCmd == string("setvariable") )	
	{
		int iCurrentObject;
		string strItem;
		string strValue;
		string strRes;
	
		pkNetPacket->Read(iCurrentObject);
		pkNetPacket->Read_Str(strItem);
		pkNetPacket->Read_Str(strValue);
		pkNetPacket->Read_Str(strRes);		
		
		if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(iCurrentObject))
		{
			if(string("Variables") == string(strItem))
			{
				pkEnt->SetVarString(strValue, strRes);				
			}			
			else
			{
				if(Property* pkProp = pkEnt->GetProperty(strItem.c_str()))
				{
					pkProp->SetValue(strValue, strRes);
					pkProp->ResetAllNetUpdateFlags();
				}							
			}			
		}
	}
	
	if( szCmd == string("addproperty") )	
	{	
		int 		iEntityID;
		string 	strPropertyName;
		
		pkNetPacket->Read(iEntityID);
		pkNetPacket->Read_Str(strPropertyName);
			
		if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(iEntityID))
		{
			if(!pkEnt->GetProperty(strPropertyName.c_str()))
				pkEnt->AddProperty(strPropertyName.c_str());			
		}
	}
	
	if( szCmd == string("removeproperty") )	
	{	
		int 		iEntityID;
		string 	strPropertyName;
		
		pkNetPacket->Read(iEntityID);
		pkNetPacket->Read_Str(strPropertyName);
			
		if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(iEntityID))
		{
			pkEnt->DeleteProperty(strPropertyName.c_str());							
		}
	}
	
	if( szCmd == string("request_zones"))
	{
		SendZoneList(false,pkNetPacket->m_iClientID,-1);
		//cout<<"sending zones to client editor"<<endl;	
	}
	
	if( szCmd == string("setzonemodel") )	
	{	
		int 		iZoneID;
		string	strModel;
		
		pkNetPacket->Read(iZoneID);
		pkNetPacket->Read_Str(strModel);	
		
		m_pkEntityManager->SetZoneModel(strModel.c_str(),iZoneID);
	}	
	
	if( szCmd == string("rotatezonemodel") )	
	{	
		int 		iZoneID;
		
		pkNetPacket->Read(iZoneID);
		RotateZoneModel(iZoneID,Vector3(0,90,0));
	}		
	
	if( szCmd == string("setzoneenviroment") )	
	{	
		int 		iZoneID;
		string	strEnviroment;
		
		pkNetPacket->Read(iZoneID);
		pkNetPacket->Read_Str(strEnviroment);	
		
		if(ZoneData* z = m_pkEntityManager->GetZoneData(iZoneID))
		{
			z->m_strEnviroment = strEnviroment;
			cout<<"Setting enviroment:"<<strEnviroment<<" on zone "<<iZoneID<<endl;
		}			
	}		
	
}


void ZeroFps::RotateZoneModel(int iZoneID,Vector3 kRot)
{
	if(ZoneData* pkData = m_pkEntityManager->GetZoneData(iZoneID)) 
	{
		if(pkData->m_pkZone)
		{
			pkData->m_pkZone->RotateLocalRotV( kRot ); 
	
			// Update PFind Mesh				
			if(P_PfMesh* pkMesh = (P_PfMesh*)pkData->m_pkZone->GetProperty("P_PfMesh"))
				pkMesh->CalcNaviMesh();
		}		
	}		
}

void ZeroFps::SendZoneList(bool bRemove,int iClientID,int iZoneID)
{
	NetPacket kNp;		
	kNp.Clear();
	
	if(iZoneID == -1)
	{		
		kNp.Write((unsigned char ) ZPGP_ZED_ZONELIST);		
		kNp.Write(bool(true));
		kNp.Write(bool(bRemove));
			
		for(int i = 0;i<m_pkEntityManager->m_kZones.size();i++)
		{
			//if packet gets to big
			if(kNp.m_iLength >= 800)
			{
				//send package
				kNp.Write(int(-1));				
				kNp.TargetSetClient(iClientID);
				m_pkApp->SendAppMessage(&kNp);
				
				// and setup a new one
				kNp.Clear();
				kNp.Write((unsigned char ) ZPGP_ZED_ZONELIST);		
				kNp.Write(bool(false));				
				kNp.Write(bool(bRemove));
			}
			
			//write zone info
			if(m_pkEntityManager->m_kZones[i].m_iStatus != EZS_UNUSED)
			{
				kNp.Write(m_pkEntityManager->m_kZones[i].m_iStatus);
				kNp.Write(m_pkEntityManager->m_kZones[i].m_iZoneID);
				kNp.Write(m_pkEntityManager->m_kZones[i].m_iZoneObjectID);				
				kNp.Write(m_pkEntityManager->m_kZones[i].m_kPos);
				kNp.Write(m_pkEntityManager->m_kZones[i].m_kSize);			
			}			
		}
	
		//send package
		kNp.Write(int(-1));
		kNp.TargetSetClient(iClientID);
		m_pkApp->SendAppMessage(&kNp);					
	}
	else
	{
		kNp.Write((unsigned char ) ZPGP_ZED_ZONELIST);		
		kNp.Write(bool(false));
		kNp.Write(bool(bRemove));		
		
		if(ZoneData* pkZone = m_pkEntityManager->GetZoneData(iZoneID))
		{
			if( (pkZone->m_iStatus != EZS_UNUSED) )
			{							
				kNp.Write(pkZone->m_iStatus);
				kNp.Write(pkZone->m_iZoneID);
				kNp.Write(pkZone->m_iZoneObjectID);				
				kNp.Write(pkZone->m_kPos);
				kNp.Write(pkZone->m_kSize);			
			}							
		
			//send package
			kNp.Write(int(-1));
			kNp.TargetSetClient(iClientID);
			m_pkApp->SendAppMessage(&kNp);					
						
		}
	}					
}


/*
	This function checks if the command for the editor should run localy (we are the server) or be sent over the
	network (we are connected to a server).
*/
void ZeroFps::RouteEditCommand(NetPacket* pkNetPacket)
{
	pkNetPacket->m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
	pkNetPacket->Write((char)ZFGP_ENDOFPACKET);
	pkNetPacket->m_iPos = 0;

	char cEcmd;

	if(m_bServerMode)
	{
		pkNetPacket->Read(cEcmd);
		HandleEditCommand(pkNetPacket);
	}
	else
	{
		pkNetPacket->TargetSetClient(ZF_NET_ALLCLIENT);
		m_pkNetWork->Send2(pkNetPacket);
	}
}

void ZeroFps::RegisterResources()
{
	m_pkResourceDB->RegisterResource( string(".mad"), Create__Mad_Core	);
	m_pkResourceDB->RegisterResource( string(".tga"), Create__ResTexture	);
	m_pkResourceDB->RegisterResource( string(".bmp"), Create__ResTexture	);
	m_pkResourceDB->RegisterResource( string(".zmt"), Create__Material	);
	m_pkResourceDB->RegisterResource( string(".wav"), Create__WavSound	);
	m_pkResourceDB->RegisterResource( string(".zvp"), Create__ZVProgram	);
	m_pkResourceDB->RegisterResource( string(".zfp"), Create__ZFProgram	);	
	m_pkResourceDB->RegisterResource( string(".lua"), Create__ZFScript  	);
}

void ZeroFps::RegisterPropertys()
{
	//render propertys
	m_pkPropertyFactory->Register("P_Primitives3D",		Create_Prim3DProperty);			
	m_pkPropertyFactory->Register("P_LightUpdate",		Create_LightUpdateProperty);							
	m_pkPropertyFactory->Register("P_Light",				Create_LightProperty);					
	m_pkPropertyFactory->Register("P_LinkToJoint",		Create_LinkToJoint);										
	m_pkPropertyFactory->Register("P_Camera",				Create_CameraProperty);			
	m_pkPropertyFactory->Register("P_Vegitation",		Create_VegitationProperty);
	m_pkPropertyFactory->Register("P_WaterRender",		Create_WaterRenderProperty);	
	m_pkPropertyFactory->Register("P_BillBoardRender", Create_BillBoardRenderProperty);
	m_pkPropertyFactory->Register("P_CrossRender",		Create_CrossRenderProperty);
	m_pkPropertyFactory->Register("P_SkyBoxRender",		Create_SkyBoxRenderProperty);		
	m_pkPropertyFactory->Register("P_HMRP2",				Create_HMRP2);			
	//m_pkPropertyFactory->Register("P_PSystem",			Create_PSystemProperty);											
	//m_pkPropertyFactory->Register("P_Mad",					Create_MadProperty);				
	//m_pkPropertyFactory->Register("P_Heightmap2",		Create_P_Heightmap2);
	//m_pkPropertyFactory->Register("P_HeightMapRender",	Create_HeightMapRenderProperty);		

	//normal propertys
	m_pkPropertyFactory->Register("P_Track",				Create_TrackProperty);							
	m_pkPropertyFactory->Register("P_AmbientSound",		Create_AmbientSound);
	m_pkPropertyFactory->Register("P_Sound",				Create_SoundProperty);		
	m_pkPropertyFactory->Register("P_PfPath",				Create_P_PfPath);											
	m_pkPropertyFactory->Register("P_PfMesh",				Create_P_PfMesh);											
	m_pkPropertyFactory->Register("P_WorldInfo",			Create_WorldInfoProperty);						
	//m_pkPropertyFactory->Register("P_Tcs",					Create_P_Tcs);					
	//m_pkPropertyFactory->Register("P_ScriptInterface",	Create_P_ScriptInterface);
	//m_pkPropertyFactory->Register("P_Controller",		Create_P_Controller);

	Register_MadProperty(this);
	Register_PTcs(this);
	Register_PController(this);
	Register_PScriptInterface(this);
	Register_PPSystem(this);

	Register_SIEntityProperty(this);
}


void ZeroFps::QuitEngine()
{
	vector<string> kPropertyNames;
	
	if(m_pkEntityManager->GetWorldEntity())
	{
		m_pkEntityManager->GetWorldEntity()->GetAllVarNames(kPropertyNames);

		Logf("net", "WorldObject Dump %f\n", GetEngineTime());
		for(unsigned int i=0; i<kPropertyNames.size(); i++) 
		{
			Logf("net", " %s %f\n",kPropertyNames[i].c_str(),  m_pkEntityManager->GetWorldEntity()->GetVarDouble(kPropertyNames[i]));
		}
	}

	printf("ZeroFps::QuitEngine\n");
	m_iState = state_exit;
}

void ZeroFps::GetEngineCredits(vector<string>& kCreditsStrings)
{
	kCreditsStrings.clear();

	kCreditsStrings.push_back( string("		  ZeroFps Engine		") );
	kCreditsStrings.push_back( string("		       by				") );
	kCreditsStrings.push_back( string("								") );
	kCreditsStrings.push_back( string("      Jimmy Magnusson		") );
	kCreditsStrings.push_back( string("      Richard Svensson		") );
	kCreditsStrings.push_back( string("         Erik Glans			") );
	kCreditsStrings.push_back( string("       Magnus J�nsson 		") );
}

/**	\brief	Called before someone would like to connect.

	PreConnect is called when a connection is about to be made. It's after a Connect message
	for a server Node that accepts connections. Never called on a client node. Return false
	to deny connection. Put reason if any into szWhy256.
*/
bool	ZeroFps::PreConnect(IPaddress kRemoteIp, char* szLogin, char* szPass, bool bIsEditor, string& strWhy)
{
	//m_pkConsole->Printf("ZeroFps::PreConnect()");
	return m_pkApp->OnPreConnect(kRemoteIp, szLogin, szPass,bIsEditor, strWhy);
}

/**	\brief	Called when someone have been connected.

	Connect is called when a connection have been made. It is called after PreConnect on server if
	PreConnect returns true. It is called on Clients when they recive connect_yes from server.
	Return value is the NetID off the client object on the server. It don't matter on the client.
*/
int ZeroFps::Connect(int iConnectionID, char* szLogin, char* szPass, bool bIsEditor)
{
	if(m_bServerMode)
	{			
	
		//m_pkConsole->Printf("ZeroFps::Connect(%d)", iConnectionID);
		m_kClient[iConnectionID].m_pkObject = m_pkEntityManager->CreateEntity();//m_pkEntityManager->CreateObjectByArchType("ZeroRTSPlayer");
		m_kClient[iConnectionID].m_pkObject->SetName("A Client Obj");
		m_kClient[iConnectionID].m_pkObject->SetWorldPosV(Vector3(0,0,2));
	
		// Connect all client objects to top level object,
		m_kClient[iConnectionID].m_pkObject->SetParent(m_pkEntityManager->m_pkClientEntity);
		
		m_kClient[iConnectionID].m_fConnectTime = GetEngineTime();
		m_pkApp->OnServerClientJoin(&m_kClient[iConnectionID],iConnectionID, szLogin, szPass,bIsEditor);
		m_pkEntityManager->m_fEndTimeForceNet = GetEngineTime() + 5.0f;
	
		
	//	m_pkConsole->Printf("Player Object %d", m_kClient[iConnectionID].m_pkObject->GetEntityID());	
		return m_kClient[iConnectionID].m_pkObject->GetEntityID();
	}
	else
	{
		m_pkApp->OnClientConnected();
				
		return -1;
	}
}

/**	\brief	Called when a connection is closed down.
*/
void ZeroFps::Disconnect(int iConnectionID)
{
	if(!m_kClient[iConnectionID].m_pkObject)
	{
		cout << "ZeroFps::Disconnect - Client is already disconnected." <<endl;
		return;
	}

	//reset all netupdate flags in the world
	m_pkEntityManager->ResetNetUpdateFlags(iConnectionID);	
	
	if(m_bClientMode && !m_bServerMode)
	{
		m_pkApp->OnDisconnect( iConnectionID );
	}

	if(!m_bServerMode)
		return;
		
	m_pkConsole->Printf("ZeroFps::Disconnect(%d)", iConnectionID);
	
	//assert( m_kClient[iConnectionID].m_pkObject );
	
	m_pkApp->OnServerClientPart(&m_kClient[iConnectionID],iConnectionID);	
	
	m_pkEntityManager->Delete( m_kClient[iConnectionID].m_pkObject );
	m_kClient[iConnectionID].m_pkObject = NULL;
	m_kClient[iConnectionID].m_strLogin = "";
}


/**	\brief	Returns ID of of object Client use to send data to server.	

	Returns ID of of object Client use to send data to server. Returns -1 if object is unknown at the moment.
	Keep asking :)
*/
int ZeroFps::GetClientObjectID()
{
/*	if(m_iRTSClientObject == -1) {
		m_pkNetWork->RTS_RequestClientObjectID();
		}*/

	return m_iClientEntityID;
}


//vad g�r denna h�r igentligen?
void ZeroFps::AddHMProperty(Entity* pkEntity, int iNetWorkId, Vector3 kZoneSize)
{
	// Get Entity, Check For Valid and Check if its already have a hmap.
	//Entity* pkEntity = this->m_pkEntityManager->GetObjectByNetWorkID(iNetWorkId);
	if(!pkEntity)										return;
	if(pkEntity->GetProperty("P_HMRP2"))		return;

	// Create a new Hmap and add it.
	HeightMap* pkMap = new HeightMap;
	pkMap->SetID(iNetWorkId);
	pkMap->Create( int(kZoneSize.x) );
	//pkMap->Random();

	// Create a new Hmrp and set hmap and add it to Entity.
	P_HMRP2* pkhmrp2 = new P_HMRP2(pkMap, "Spya");
	pkEntity->AddProperty(pkhmrp2);
	P_Tcs* pp = (P_Tcs*)pkEntity->GetProperty("P_Tcs");
	if(!pp)
	{
		pp = (P_Tcs*)pkEntity->AddProperty("P_Tcs");	
		pp->SetPolygonTest(true);	
		pp->SetStatic(true);			
	}

	//	pp->SetupMeshData();

	// Setup Collision Data
	/*
	vector<Mad_Face>	kFace;
	vector<Vector3>	kVertex;
	vector<Vector3>	kNormal;

	pkMap->GetCollData(&kFace,&kVertex,&kNormal);

	// Transform all vertex from Local to World.
	for(unsigned int i=0; i<kVertex.size(); i++) 
		kVertex[i] += pkEntity->GetWorldPosV() + pkMap->m_kCornerPos;

	// Create Tcs Property
	P_Tcs* pp = (P_Tcs*)pkEntity->GetProperty("P_Tcs");
	if(!pp)
		pp = (P_Tcs*)pkEntity->AddProperty("P_Tcs");	

	// Set Data.
	if(pp)
	{
		pp->SetPolygonTest(true);	
		pp->SetStatic(true);			
		//pp->SetRefetchPolygonData();
		pp->SetData(kFace,kVertex,kNormal,	50);	 //(int)kZoneSize.x * 2
		pp->SetHmap(pkMap);
		pp->SetGroup(0);
	}
	*/

/*		Vector3 kMin = pkZd->m_kPos - pkZd->m_kSize/2;
		Vector3 kMax = pkZd->m_kPos + pkZd->m_kSize/2;
		Vector3 ANormal(0,1,0);
		kNormal.push_back( ANormal );
		kNormal.push_back( ANormal );
		kVertex.push_back(Vector3(kMin.x,kMin.y + 3,kMin.z));
		kVertex.push_back(Vector3(kMax.x,kMin.y + 3,kMin.z));
		kVertex.push_back(Vector3(kMax.x,kMin.y + 3,kMax.z));
		kVertex.push_back(Vector3(kMin.x,kMin.y + 3,kMax.z));
		Mad_Face aFace;
		aFace.iIndex[0] = 0;	aFace.iIndex[1] = 2;	aFace.iIndex[2] = 1;
		kFace.push_back(aFace);
		aFace.iIndex[0] = 0;	aFace.iIndex[1] = 3;	aFace.iIndex[2] = 2;
		kFace.push_back(aFace);*/
}


/*
void ZeroFps::SetCamera(Camera* pkCamera)
{
	//set camera pointer 
	m_pkCamera=pkCamera;		
	
	//call updateall so that the camera updates viewport and realoads projectionmatrix
	//m_pkCamera->UpdateAll(m_pkRender->GetWidth(),m_pkRender->GetHeight());
	
	UpdateCamera();
}

void ZeroFps::UpdateCamera()
{
	//update camera
	m_pkCamera->Update();//m_pkRender->GetWidth(),m_pkRender->GetHeight());
	
}
*/








