#include "zerofps.h"
#include "network.h"

//	extern PFNGLFOGCOORDFEXTPROC glFogCoordfEXT;		//glFogCoordsEXT
int	g_iNumOfFrames;


ZeroFps::ZeroFps(void) 
 : ZFObject("ZeroFps") {

	m_pkFile=new FileIo;
	m_pkBasicFS=new ZFBasicFS;
	m_pkTexMan=new TextureManager(m_pkFile);
	m_pkInput=new Input();		
	m_pkPropertyFactory=new PropertyFactory();
	m_pkFrustum = new Frustum;	
	m_pkLight=new Light();	
	m_pkRender=new Render();
	m_pkConsole=new Console();	
	m_pkCmd=new CmdSystem;
	m_pkAudioMan=new AudioManager(this);
	m_pkObjectMan=new ObjectManager();
	m_pkCollisionMan=new CollisionManager();	
	m_pkSBM=new SoundBufferManager(m_pkFile);	
	m_pkOpenAlSystem= new OpenAlSystem();
	m_pkNetWork = new NetWork;
	m_pkGuiRenderer = new GLGuiRender();
	m_pkGuiMan = new ZGuiResourceManager();
	m_pkGui = new ZGui();
	m_pkIni = new ZFIni();
	m_pkLevelMan = new LevelManager();
	m_pkPhysEngine = new PhysicsEngine();

	m_iFullScreen=0;
	m_fFrameTime=0;
	m_fLastFrameTime=SDL_GetTicks();
	m_bServerMode=false;
	m_bClientMode=true;
	m_bConsoleMode=false;
	m_bDrawDevList=true;
	m_bGuiMode=false;
	m_bGuiTakeControl=true;
	m_iMadDraw = 1;
	
//	akCoreModells.reserve(25);

	//add some nice variables =)
//	m_pkCmd->Add(&m_iState,"G_State",type_int);
	m_pkCmd->Add(&m_pkInput->m_fMouseSensitivity,"m_Sens",type_float);
	m_pkCmd->Add(&m_pkRender->m_iDetail,"r_LandLod",type_int);
	m_pkCmd->Add(&m_pkRender->m_iViewDistance,"r_ViewDistance",type_int);	
	m_pkCmd->Add(&m_pkRender->m_iAutoLod,"r_AutoLod",type_int);		
	m_pkCmd->Add(&m_pkRender->m_iFpsLock,"r_FpsLock",type_int);		
	m_pkCmd->Add(&m_pkLight->m_iNrOfLights,"r_MaxLights",type_int);		

	m_pkCmd->Add(&m_iWidth,"r_Width",type_int);			
	m_pkCmd->Add(&m_iHeight,"r_Height",type_int);		
	m_pkCmd->Add(&m_iDepth,"r_Depth",type_int);		
	m_pkCmd->Add(&m_iFullScreen,"r_FullScreen",type_int);	
	m_pkCmd->Add(&m_iMadDraw,"r_maddraw",type_int);	

	g_ZFObjSys.Register_Cmd("setdisplay",FID_SETDISPLAY,this);
	g_ZFObjSys.Register_Cmd("quit",FID_QUIT,this);
	g_ZFObjSys.Register_Cmd("slist",FID_SLIST,this);
	g_ZFObjSys.Register_Cmd("connect",FID_CONNECT,this);
	g_ZFObjSys.Register_Cmd("server",FID_SERVER,this);
	g_ZFObjSys.Register_Cmd("dir",FID_DIR,this);	
	g_ZFObjSys.Register_Cmd("cd",FID_CD,this);	
	g_ZFObjSys.Register_Cmd("listmad",FID_LISTMAD,this);	
	g_ZFObjSys.Register_Cmd("printobject",FID_PRINTOBJECT,this);	
	g_ZFObjSys.Register_Cmd("version",FID_VERSION,this);	
	g_ZFObjSys.Register_Cmd("credits",FID_CREDITS,this);	


	m_kCurentDir=m_pkBasicFS->GetCWD();

	RegisterPropertys();
}

ZeroFps::~ZeroFps()
{
	m_pkNetWork->ServerEnd();

	delete m_pkFile;
	delete m_pkCmd;
	delete m_pkTexMan;
	delete m_pkRender;
	delete m_pkConsole;
	delete m_pkInput;
	delete m_pkAudioMan;
	delete m_pkLight;
	delete m_pkObjectMan;
	delete m_pkCollisionMan;
	delete m_pkNetWork;
	delete m_pkIni;
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



void ZeroFps::Init(int iNrOfArgs, char** paArgs)
{	

	HandleArgs(iNrOfArgs,paArgs);					//handle arguments
	SetApp();										//setup class pointers	

	InitDisplay(m_pkApp->m_iWidth,m_pkApp->m_iHeight,m_pkApp->m_iDepth);


	m_iState=state_normal;								//init gamestate to normal		
//	m_pkGameCamera=m_pkDefaultCamera;		
//	m_pkCamera=m_pkGameCamera;

//	SetCamera(m_pkDefaultCamera);	

	m_pkApp->OnInit();										//call the applications oninit funktion
	m_fFrameTime=0;
	m_fLastFrameTime = SDL_GetTicks();
	MainLoop();														//jump to mainloop
}

void ZeroFps::MainLoop(void) {

	while(m_iState!=state_exit) {
		
		Swap();				//swap buffers n calculate fps
		m_pkNetWork->Run();
		m_pkObjectMan->PackToClients();
		DevPrintf("Num of Clients: %d", m_pkNetWork->GetNumOfClients());
		DevPrintf("Num Objects: %d", m_pkObjectMan->GetNumOfObjects());

		//handle input
		m_pkInput->Update();
		 
		if(m_bServerMode)
		{
		
		}		
		
		if(m_bClientMode)
		{
			if(m_bConsoleMode)
				m_pkInput->SetInputEnabled(false);
			else 
			{
				m_pkInput->SetInputEnabled(true);						
				
				if(m_pkInput->Pressed(KEY_TAB))
				{			
					m_bConsoleMode=true;		
					m_pkInput->Reset();
				}
				
				if(m_bGuiTakeControl)
				{
					if(m_bGuiMode)
						m_pkInput->SetInputEnabled(false);
					else
						m_pkInput->SetInputEnabled(true);
				}
			}
			
				
			//toggle keyboard/mouse grabing
			if(m_pkInput->Pressed(KEY_F12))
				m_pkInput->ToggleGrab();
					
			//toggle fullscreen on X systems
			if(m_pkInput->Pressed(KEY_F11))
				ToggleFullScreen();		
				
			//run application main loop
			m_pkApp->OnIdle();							
			
			//update zones
			m_pkLevelMan->UpdateZones();			
			
			//update all normal propertys
			m_pkObjectMan->Update(PROPERTY_TYPE_NORMAL,PROPERTY_SIDE_ALL,false);

			//update physicsengine
			m_pkPhysEngine->Update();

			//update all collisions
//			m_pkCollisionMan->Update();				

			//update openal sound system
			
			Vector3 up=(m_pkCamera->GetRot()-Vector3(0,90,0));//.AToU();
			up.x=90;
			m_pkOpenAlSystem->SetListnerPosition(m_pkCamera->GetPos(),(m_pkCamera->GetRot()+Vector3(0,90,0)).AToU(),up.AToU());//(m_pkCamera->GetRot()-Vector3(-90,90,0)).AToU());
			m_pkOpenAlSystem->Update();

			// Describe Active Cam.
			string strCamDesc = GetCam()->GetCameraDesc();
			DevPrintf(strCamDesc.c_str());

			//run application Head On Display 
			SetCamera(m_pkConsoleCamera);
			m_pkApp->OnHud();

			// Update GUI
			if(!m_bConsoleMode)
				m_pkGui->Update();

			//delete objects
			m_pkObjectMan->UpdateDelete();

			DevPrintf("Mad's: %d", m_iNumOfMadRender);
			m_iNumOfMadRender = 0;
			
		}
		
		if(m_bConsoleMode)
		{		
//			SetCamera(m_pkCamera);			
			SetCamera(m_pkConsoleCamera);			
//			m_pkCamera->ClearViewPort();
			
			m_pkInput->SetInputEnabled(true);			
			m_pkConsole->Update();
		}
	}
}



void ZeroFps::InitDisplay(int iWidth,int iHeight,int iDepth) {
	m_iWidth=iWidth;
	m_iHeight=iHeight;
	m_iDepth=iDepth;


	//initiera sdl med opengl
	if(SDL_Init(SDL_OPENGL | SDL_INIT_NOPARACHUTE )<0){
		cout<<"Sdl_Graphic didt want to work right now =("<<endl;
		exit(1);
	}	
	
	atexit(SDL_Quit);


	SetDisplay();

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

	m_pkGui->ShowCursor(bShowCursor);
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
	
	//if we have changed active camera, update all
	if(m_pkCamera!=pkCamera){
		m_pkCamera=pkCamera;		
		m_pkCamera->UpdateAll(m_iWidth,m_iHeight);					
		
	//else do a normal update
	} else {
		m_pkCamera->Update(m_iWidth,m_iHeight);
	}
	
	//get the frustrum for frustum culling
	m_pkFrustum->GetFrustum();				
	
	
}
static char Devformat_text[4096];	//

void ZeroFps::DevPrintf(const char *fmt, ...)
{
	va_list		ap;								// Pointer To List Of Arguments

	// Make sure we got something to work with.
	if (fmt == NULL)	return;					

	va_start(ap, fmt);							// Parses The String For Variables
		vsprintf(Devformat_text, fmt, ap);			// And Convert Symbols
	va_end(ap);									// 

	// Now call our print function.
	akDevString.push_back(string(Devformat_text));
}

void ZeroFps::DrawDevStrings()
{
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_ALPHA_TEST);

	m_pkRender->SetFont("file:../data/textures/text/devstr.bmp");

	if(m_bDrawDevList == true)
	{
		float fYOffset = 0.85;
		for(int i=0; i<akDevString.size(); i++) 
		{
			m_pkRender->Print(Vector3(-1.1,fYOffset,-1),Vector3(0,0,0),Vector3(0.02,0.02,0.02), 
				const_cast<char*>(akDevString[i].c_str()));	
			fYOffset -= 0.02;
		}
	}

	akDevString.clear();
	glPopAttrib();
}

char g_szIpPort[256];

void ZeroFps::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	int i;

	vector<string> kFiles;
	vector<string> kCreditsStrings;
	
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

		
		case FID_LISTMAD:
			int iSize = akCoreModells.size();
			m_pkConsole->Printf("Loaded Mads: ");
			for(i=0; i < akCoreModells.size(); i++) {
				m_pkConsole->Printf(" %d: %s", i, akCoreModells[i]->Name);
				}
			break;
	}	
}

void ZeroFps::ClearLevelData()
{
	
}

int ZeroFps::LoadMAD(const char* filename)
{
	int iMadId = GetMADIndex(filename);

	if(iMadId != -1)
		return iMadId;

	Mad_Core* CoreAdd = new Mad_Core;
	if(CoreAdd->LoadMad(filename)) {
		akCoreModells.push_back(CoreAdd);
		return (akCoreModells.size() - 1);
		}
	else {
		delete CoreAdd;
		CoreAdd = new Mad_Core;
		CoreAdd->LoadMad("../data/mad/zoneobject.mad");
		akCoreModells.push_back(CoreAdd);
		return (akCoreModells.size() - 1);
		}

	return -1;
}

void ZeroFps::ClearMAD(void)
{
	for(int i=0; i< akCoreModells.size(); i++)
		delete akCoreModells[i];

	akCoreModells.clear();
}

int ZeroFps::GetMADIndex(const char* filename)
{
	for(unsigned int i=0; i < akCoreModells.size(); i++)
	{
		if(strcmp(akCoreModells[i]->GetName(),filename) == 0)
			return i;
	}

	return -1;
}

Mad_Core* ZeroFps::GetMADPtr(const char* filename)
{
	int iMadId = LoadMAD(filename);

	if(iMadId != -1)
		return akCoreModells[iMadId];

	return NULL;
}


void ZeroFps::HandleNetworkPacket(NetPacket* pkNetPacket)
{
	unsigned char ucGamePacketType;
	
	pkNetPacket->Read(ucGamePacketType);
	while(ucGamePacketType != ZFGP_ENDOFPACKET) {
		switch(ucGamePacketType) {
			case ZFGP_OBJECTSTATE: 
				cout << "Recv: ZFGP_OBJECTSTATE" << endl;
				m_pkObjectMan->UpdateState(pkNetPacket);
				break;

			case ZFGP_CLIENTSTATE: 
				cout << "Recv: ZFGP_CLIENTSTATE" << endl;
				break;
		
			case ZFGP_CLIENTCMD: 
				cout << "Recv: ZFGP_CLIENTCMD" << endl;
				break;
			
			case ZFGP_PRINT: 
				cout << "Recv: ZFGP_PRINT" << endl;
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



