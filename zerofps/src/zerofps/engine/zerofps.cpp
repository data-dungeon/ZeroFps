#include "zerofps.h"
#include "network.h"

int		g_iNumOfFrames;
int		g_iNumOfMadSurfaces;
float	g_fMadLODScale;
int		g_iMadLODLock;

static char Devformat_text[4096];	//

ZeroFps::ZeroFps(void) 
 : ZFObject("ZeroFps") {

	m_pkFile						= new FileIo;
	m_pkBasicFS					= new ZFBasicFS;
	m_pkZFVFileSystem			= new ZFVFileSystem;
	m_pkTexMan					= new TextureManager(m_pkFile);
	m_pkInput					= new Input();		
	m_pkPropertyFactory		= new PropertyFactory();
	m_pkFrustum					= new Frustum;	
	m_pkLight					= new Light();	
	m_pkRender					= new Render();
	m_pkConsole					= new Console();
	m_pkCmd						= new CmdSystem;
	m_pkAudioMan				= new AudioManager(this);
	m_pkObjectMan				= new ObjectManager();
	m_pkSBM						= new SoundBufferManager(m_pkFile);	
	m_pkOpenAlSystem			= new OpenAlSystem();
	m_pkNetWork					= new NetWork;
	m_pkGuiRenderer			= new GLGuiRender();
	m_pkGuiMan					= new ZGuiResourceManager();
	m_pkGui						= new ZGui();
	m_pkIni						= new ZFIni();
	m_pkLevelMan				= new LevelManager();
	m_pkPhysEngine				= new PhysicsEngine();
	m_pkResourceDB				= new ZFResourceDB();


	m_iFullScreen=			0;
	m_fFrameTime=			0;
	m_fLastFrameTime=		0;
	m_fSystemUpdateFps=	25;
	m_fSystemUpdateTime= 0;
	m_bServerMode = 		true;
	m_bClientMode = 		true;
	m_bDrawDevList=		true;
	m_bGuiMode=				false;
	m_bGuiTakeControl=	true;
	m_iMadDraw = 			1;
	g_fMadLODScale = 		1.0;
	g_iMadLODLock = 		0;
	m_pkCamera = 			NULL;

	g_ZFObjSys.RegisterVariable("m_Sens", &m_pkInput->m_fMouseSensitivity,CSYS_FLOAT);
	g_ZFObjSys.RegisterVariable("r_LandLod", &m_pkRender->m_iDetail,CSYS_INT);
	g_ZFObjSys.RegisterVariable("r_ViewDistance", &m_pkRender->m_iViewDistance,CSYS_INT);
	g_ZFObjSys.RegisterVariable("r_AutoLod", &m_pkRender->m_iAutoLod,CSYS_INT);
	g_ZFObjSys.RegisterVariable("r_FpsLock", &m_pkRender->m_iFpsLock,CSYS_INT);
	g_ZFObjSys.RegisterVariable("r_MaxLights", &m_pkLight->m_iNrOfLights,CSYS_INT);
	g_ZFObjSys.RegisterVariable("r_Width", &m_iWidth,CSYS_INT);
	g_ZFObjSys.RegisterVariable("r_Height", &m_iHeight,CSYS_INT);
	g_ZFObjSys.RegisterVariable("r_Depth", &m_iDepth,CSYS_INT);
	g_ZFObjSys.RegisterVariable("r_FullScreen", &m_iFullScreen,CSYS_INT);
	g_ZFObjSys.RegisterVariable("r_maddraw", &m_iMadDraw,CSYS_INT);
	g_ZFObjSys.RegisterVariable("r_madlod", &g_fMadLODScale,CSYS_FLOAT);
	g_ZFObjSys.RegisterVariable("r_madlodlock", &g_iMadLODLock,CSYS_FLOAT);

	g_ZFObjSys.Register_Cmd("setdisplay",FID_SETDISPLAY,this);
	g_ZFObjSys.Register_Cmd("quit",FID_QUIT,this);
	g_ZFObjSys.Register_Cmd("slist",FID_SLIST,this);
	g_ZFObjSys.Register_Cmd("connect",FID_CONNECT,this);
	g_ZFObjSys.Register_Cmd("server",FID_SERVER,this);
	g_ZFObjSys.Register_Cmd("printobject",FID_PRINTOBJECT,this);	
	g_ZFObjSys.Register_Cmd("version",FID_VERSION,this);	
	g_ZFObjSys.Register_Cmd("credits",FID_CREDITS,this);	
	g_ZFObjSys.Register_Cmd("echo",FID_ECHO,this);	
	g_ZFObjSys.Register_Cmd("gldump",FID_GLDUMP,this);	
	g_ZFObjSys.Register_Cmd("devshow",FID_DEV_SHOWPAGE,this, "devshow name", 1);	
	g_ZFObjSys.Register_Cmd("devhide",FID_DEV_HIDEPAGE,this, "devhide name", 1);	
	g_ZFObjSys.Register_Cmd("sendmsg",FID_SENDMESSAGE,this,	 "sendmsg name id",2);	
	g_ZFObjSys.Register_Cmd("debug",FID_LISTMAD,this);	

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
}

ZeroFps::~ZeroFps()
{
	m_pkNetWork->ServerEnd();
	g_ZFObjSys.ShutDown();

	delete m_pkResourceDB;
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
	delete m_pkAudioMan;
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
	delete m_pkFile;
}

void ZeroFps::SetApp() {
	m_pkApp=Application::pkApp;
	m_pkApp->SetEnginePointer(this);
}

void ZeroFps::HandleArgs(int iNrOfArgs, char** paArgs) {
	string strArg;
	for(int i = 0; i < iNrOfArgs; i++) {
		strArg = string(paArgs[i]);
		AppArguments.push_back(strArg);
		}

	cout<<"Nr of arguments: "<<iNrOfArgs<<endl;
	for(int ia = 0; ia < iNrOfArgs; ia++) {
		cout << "Argument[" << ia << "]: "<< AppArguments[ia] << endl;
		}

}

int	ZeroFps::NumberOfArgs(void)
{
	return AppArguments.size();
}

string ZeroFps::GetArg(int iArgIndex)
{
	string strArg("");
	if(iArgIndex < 0 || iArgIndex >= AppArguments.size())
		return strArg;

	strArg = AppArguments[iArgIndex];
	return strArg;
}



bool ZeroFps::Init(int iNrOfArgs, char** paArgs)
{	
	HandleArgs(iNrOfArgs,paArgs);							//	handle arguments
	SetApp();												//	setup class pointers	

	// StartUp SDL
	if(SDL_Init(SDL_OPENGL | SDL_INIT_NOPARACHUTE )<0){
		g_Logf("Error: Failed to StartUp SDL\n");
		return false;
	}	
	
	atexit(SDL_Quit);

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
	m_pkNetWork->Run();
	m_pkObjectMan->PackToClients();
	DevPrintf("common","Num of Clients: %d", m_pkNetWork->GetNumOfClients());
	DevPrintf("common","Num Objects: %d", m_pkObjectMan->GetNumOfObjects());

	// Update Local Input.
	m_pkInput->Update();

	//toggle keyboard/mouse grabing		// SHELL
	if(m_pkInput->Pressed(KEY_F12))
		m_pkInput->ToggleGrab();
			
	//toggle fullscreen on X systems	// SHELL	
	if(m_pkInput->Pressed(KEY_F11))
		ToggleFullScreen();		

	if(m_pkInput->Pressed(KEY_TAB))
	{		
		m_pkConsole->Toggle();
		m_pkInput->Reset();
	}
}

void ZeroFps::Run_Server()
{
	//update zones
	//m_pkLevelMan->UpdateZones();			
	
	Update_System();

	//delete objects
	m_pkObjectMan->UpdateDelete();
	m_pkResourceDB->Refresh();
}

void ZeroFps::Run_Client()
{
	if(m_pkConsole->IsActive())
		m_pkInput->SetInputEnabled(false);
	else 
		m_pkInput->SetInputEnabled(true);						
		
	//run application main loop
	m_pkApp->OnIdle();							
	
	//update zones
	m_pkLevelMan->UpdateZones();			

	//   _---------------------------------- fulhack deluxe 
	UpdateCamera();	
	m_pkObjectMan->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true);

	//update openal sound system			
	Vector3 up=(m_pkCamera->GetRot()-Vector3(0,90,0));//.AToU();
	up.x=90;
	m_pkOpenAlSystem->SetListnerPosition(m_pkCamera->GetPos(),(m_pkCamera->GetRot()+Vector3(0,90,0)).AToU(),up.AToU());//(m_pkCamera->GetRot()-Vector3(-90,90,0)).AToU());
	m_pkOpenAlSystem->Update();

	//run application Head On Display 
	SetCamera(m_pkConsoleCamera);			
	m_pkApp->OnHud();
}

void ZeroFps::Update_System()
{

	float fATime = GetTicks() - m_fSystemUpdateTime; 	
	int iLoops = int(m_fSystemUpdateFps * fATime);

	if(iLoops<=0)
		return;

	m_fGameFrameTime = (fATime / iLoops);	
	
	for(int i=0;i<iLoops;i++)
	{
		m_fGameTime = m_fSystemUpdateTime + (i * m_fGameFrameTime);
		
		//update all normal propertys
		m_pkObjectMan->Update(PROPERTY_TYPE_NORMAL,PROPERTY_SIDE_ALL,false);
		m_pkObjectMan->UpdateGameMessages();

		//update physicsengine
		m_pkPhysEngine->Update();	

		m_fSystemUpdateTime = GetTicks();
	}
	

}

void ZeroFps::Draw_EngineShell()
{
	// Describe Active Cam.
	string strCamDesc = GetCam()->GetCameraDesc();
	DevPrintf("common",strCamDesc.c_str());
	DevPrintf("common" , "NumMads/NumMadSurfaces: %d / %d", m_iNumOfMadRender , g_iNumOfMadSurfaces);

	m_iNumOfMadRender = 0;
	g_iNumOfMadSurfaces = 0;
	
	if(m_pkConsole->IsActive()) {		
		SetCamera(m_pkConsoleCamera);			
		
		m_pkInput->SetInputEnabled(true);			
		m_pkConsole->Update();
	}		
	else {
		// Update GUI
		m_pkGui->Update();
	}


}

void ZeroFps::MainLoop(void) {

	while(m_iState!=state_exit) {

		Swap();											//swap buffers n calculate fps
		 
		Run_EngineShell();

		if(m_bServerMode)	Run_Server();
		if(m_bClientMode)	Run_Client();

		Draw_EngineShell();
	}
}

void ZeroFps::SetRenderTarget(Camera* pkCamera)
{
	for(int i=0; i<m_kRenderTarget.size(); i++)
		if(m_kRenderTarget[i] == pkCamera)
			return;

	m_kRenderTarget.push_back(pkCamera);
	cout << "Adding: " << pkCamera->GetName();
	cout << " to active rendertargets\n";
}

void ZeroFps::RemoveRenderTarget(Camera* pkCamera)
{
	
}


void ZeroFps::InitDisplay(int iWidth,int iHeight,int iDepth) {
	m_iWidth=iWidth;
	m_iHeight=iHeight;
	m_iDepth=iDepth;

	SetDisplay();

#ifdef _WIN32
	RenderDLL_InitExtGL();
	extgl_Initialize();
#endif

	cout << "Gl Version: "		<< glGetString(GL_VERSION) << endl;
	cout << "Gl Vendor: "		<< glGetString(GL_VENDOR) << endl;
	cout << "Gl Render: "		<< glGetString(GL_RENDERER) << endl;
	cout << "Gl Extensions: "	<< glGetString(GL_EXTENSIONS) << endl;


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

	glLoadIdentity();
  
	//count FPS
	m_fFrameTime=SDL_GetTicks()-m_fLastFrameTime;
	m_fLastFrameTime=SDL_GetTicks();
	m_iFps=int(1000.0/m_fFrameTime);	
	
#ifdef RUNPROFILE
	g_iNumOfFrames++;
//	if(g_iNumOfFrames >= 1000)
//		m_iState = state_exit;

#endif
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
		if(m_bGuiTakeControl)
		{
			//SDL_ShowCursor(SDL_DISABLE);
			m_bDrawDevList = !m_bGuiMode;
		}
		m_pkGui->Activate(m_bGuiMode);
	}
	else
	{
		if(m_bGuiTakeControl)
		{
			//SDL_ShowCursor(SDL_ENABLE);
			m_bDrawDevList = !m_bGuiMode;
		}
		m_pkGui->Activate(m_bGuiMode);
	}

	bool bShowCursor;
	
	if(m_bGuiMode == true)
		bShowCursor = true;
	else
		bShowCursor = false;

	if(m_bGuiTakeControl == false)
		bShowCursor = false;

	//m_pkGui->ShowCursor(bShowCursor);
}


void ZeroFps::SetDisplay(int iWidth,int iHeight,int iDepth)
{
//	m_pkGui->Resize(m_iWidth, m_iHeight, iWidth, iHeight);

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
	m_pkFrustum->GetFrustum();				
}

DevStringPage*	ZeroFps::DevPrint_FindPage(const char* szName)
{
	for(int i=0; i<m_DevStringPage.size(); i++) {
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
	string strPageName;
		
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_ALPHA_TEST);

	m_pkRender->SetFont("file:../data/textures/text/devstr.bmp");

	float fYOffset = 0.75;

	for(int page = 0; page <m_DevStringPage.size(); page++ ) {
		if(m_DevStringPage[page].m_bVisible == true) {
			strPageName = "[" + m_DevStringPage[page].m_kName + "]";

			m_pkRender->Print(Vector3(-1.1,fYOffset,-1),Vector3(0,0,0),Vector3(0.02,0.02,0.02), 
				const_cast<char*>(strPageName.c_str()));	
			fYOffset -= 0.02;

			for(int i=0; i<m_DevStringPage[page].m_akDevString.size(); i++) 
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

char g_szIpPort[256];

void ZeroFps::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	int i;
	int iArghhh;

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
			m_pkNetWork->ServerList();
			break;

		case FID_CONNECT:
			if(kCommand->m_kSplitCommand.size() <= 1)
				return;

			m_pkConsole->Printf("Connect to %s", kCommand->m_kSplitCommand[1].c_str());
			//m_pkNetWork->ClientStart(kCommand->m_kSplitCommand[1].c_str());
			sprintf(g_szIpPort, "%s:4242", kCommand->m_kSplitCommand[1].c_str());
			m_pkConsole->Printf("Connect to: %s", g_szIpPort);
			m_pkNetWork->ClientStart(g_szIpPort);
			
			//m_pkNetWork->ClientStart("192.168.0.7:4242"); // hugo
			//m_pkNetWork->ClientStart("192.168.0.145:4242"); // hugo
			//m_pkNetWork->ClientStart("192.168.0.111:4242");	// me
			m_pkConsole->Printf("FID_CONNECT");
			m_pkApp->OnClientStart();
			break;

		case FID_SERVER:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				m_pkConsole->Printf("You need to give a name for your server.");
				return;
				}

			m_pkConsole->Printf("Start a server with name %s", kCommand->m_kSplitCommand[1].c_str());
			m_pkNetWork->ServerStart();

			m_pkApp->OnServerStart();
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
				for(int i=0;i<kFiles.size();i++)
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
			iArghhh = 12;
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

			
		case FID_SENDMESSAGE:
			gm.m_FromObject = -1;
			gm.m_ToObject	= atoi(kCommand->m_kSplitCommand[2].c_str());
			gm.m_Name		= kCommand->m_kSplitCommand[1].c_str();
			m_pkConsole->Printf("Sending Msg '%s' to %d from %d", gm.m_Name.c_str(), gm.m_ToObject, gm.m_FromObject);
			m_pkObjectMan->RouteMessage(gm);
			break;
	}	
}


void ZeroFps::HandleNetworkPacket(NetPacket* pkNetPacket)
{
	unsigned char ucGamePacketType;
	
	pkNetPacket->Read(ucGamePacketType);

	while(ucGamePacketType != ZFGP_ENDOFPACKET) {
		switch(ucGamePacketType) {
			case ZFGP_OBJECTSTATE: 
				g_ZFObjSys.Logf("net", "HandleNetworkPacket(ZFGP_OBJECTSTATE)\n");
				m_pkObjectMan->UpdateState(pkNetPacket);
				break;

			case ZFGP_CLIENTSTATE: 
				g_ZFObjSys.Logf("net", "HandleNetworkPacket(ZFGP_CLIENTSTATE)\n");
				break;
		
			case ZFGP_CLIENTCMD: 
				g_ZFObjSys.Logf("net", "HandleNetworkPacket(ZFGP_CLIENTCMD)\n");
				break;
			
			case ZFGP_PRINT: 
				g_ZFObjSys.Logf("net", "HandleNetworkPacket(ZFGP_PRINT)\n");
				break;

			default:
				cout << "Error in game packet : " << (int) ucGamePacketType << endl;
				return;
		}

		pkNetPacket->Read(ucGamePacketType);
		}


}

void ZeroFps::RegisterPropertys()
{
	m_pkPropertyFactory->Register("BoxRenderProperty", Create_BoxRenderProperty);
	m_pkPropertyFactory->Register("BillBoardRenderProperty", Create_BillBoardRenderProperty);
	m_pkPropertyFactory->Register("CrossRenderProperty", Create_CrossRenderProperty);
	m_pkPropertyFactory->Register("MadProperty", Create_MadProperty);
	m_pkPropertyFactory->Register("ModelProperty", Create_ModelProperty);
	m_pkPropertyFactory->Register("LightProperty", Create_LightProperty);
	m_pkPropertyFactory->Register("WaterRenderProperty",Create_WaterRenderProperty);	
	m_pkPropertyFactory->Register("SkyBoxRenderProperty",Create_SkyBoxRenderProperty);		
	m_pkPropertyFactory->Register("HeightMapRenderProperty",Create_HeightMapRenderProperty);		
	m_pkPropertyFactory->Register("HMRP2",Create_HMRP2);			
	m_pkPropertyFactory->Register("CameraProperty",Create_CameraProperty);			
	m_pkPropertyFactory->Register("ProxyProperty",Create_ProxyProperty);				
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



