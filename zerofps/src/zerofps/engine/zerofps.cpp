#include "zerofps.h"

ZeroFps::ZeroFps(void) 
 : ZFObject("ZeroFps") {
	m_pkFile=new FileIo;
	m_pkCmd=new CmdSystem;
	m_pkTexMan=new TextureManager(m_pkFile);
	m_pkRender=new Render();
	m_pkConsole=new Console();	
	m_pkInput=new Input();
	m_pkAudioMan=new AudioManager(this);
	m_pkLight=new Light();
	m_pkObjectMan=new ObjectManager();
	m_pkCollisionMan=new CollisionManager();	

	m_iFullScreen=0;
	m_fFrameTime=0;
	m_fLastFrameTime=SDL_GetTicks();


	akCoreModells.reserve(25);


	//add some nice variables =)
	m_pkCmd->Add(&m_iState,"G_State",type_int);
//	m_pkCmd->Add(&m_iFps,"G_Fps",type_int);
//	m_pkCmd->Add(&m_fFrameTime,"m_fFrameTime",type_float);	
	m_pkCmd->Add(&m_pkInput->m_fMouseSensitivity,"m_Sens",type_float);
	m_pkCmd->Add(&m_pkRender->m_iDetail,"r_LandLod",type_int);
	m_pkCmd->Add(&m_pkRender->m_iViewDistance,"r_ViewDistance",type_int);	
	m_pkCmd->Add(&m_pkRender->m_iAutoLod,"r_AutoLod",type_int);		
	m_pkCmd->Add(&m_pkRender->m_iFpsLock,"r_FpsLock",type_int);		
	
	m_pkCmd->Add(&m_iWidth,"r_Width",type_int);			
	m_pkCmd->Add(&m_iHeight,"r_Height",type_int);		
	m_pkCmd->Add(&m_iDepth,"r_Depth",type_int);		
	m_pkCmd->Add(&m_iFullScreen,"r_FullScreen",type_int);	
	
	RegisterPropertys(this);
}

ZeroFps::~ZeroFps()
{
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

/*	m_pkFile=new FileIo;
	m_pkCmd=new CmdSystem;
	m_pkTexMan=new TextureManager(m_pkFile);
	m_pkRender=new Render(m_pkTexMan);
	m_pkConsole=new Console(this);	
	m_pkInput=new Input();
	m_pkAudioMan=new AudioManager(this);
	m_pkLight=new Light();
	m_pkObjectMan=new ObjectManager();
	m_pkCollisionMan=new CollisionManager();*/	


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
		switch(m_iState){
			case state_normal:{

				//run application main loop
				m_pkApp->OnIdle();				
				
				//this changes mode to console
				if(m_pkInput->Pressed(TAB)){
					glPushAttrib(GL_LIGHTING_BIT);
					glDisable(GL_LIGHTING);
					m_iState=state_console;
					m_pkInput->Reset();
//					m_pkTempCamera=m_pkCamera;										
					
					
					SetCamera(m_pkConsoleCamera);					
					break;
				}
				
				//toggle keyboard/mouse grabing
				if(m_pkInput->Pressed(F12))
					m_pkInput->ToggleGrab();
					
				//toggle fullscreen on X systems
				if(m_pkInput->Pressed(F11))
					ToggleFullScreen();				

				//handle input
				m_pkInput->Update();
						
				//update all objects
				m_pkObjectMan->Update();

				//update all collisions
				m_pkCollisionMan->Update();				


				//run application Head On Display 
				SetCamera(m_pkConsoleCamera);
				m_pkApp->OnHud();

				//swap buffers and calculate fps
				Swap();

				break;			
			}
			case state_console: {
				SetCamera(m_pkCamera);			
				m_pkCamera->ClearViewPort();
				
				m_pkConsole->Update();
				Swap();
				
				
				//set the old camera when changing to in-game state
/*				
				if(m_iState==state_normal){
					SetCamera(m_pkTempCamera);
				}					
*/				
				break;
			}
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

	glFogCoordfEXT = NULL;	
	glFogCoordfEXT  =  (PFNGLFOGCOORDFEXTPROC) glXGetProcAddressARB((const GLubyte *)"glFogCoordfEXT");
	
	if(!glFogCoordfEXT){
		cout<<"No glFogCoordEXT support =("<<endl;
		exit(1);	
	}

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
	
	m_pkDefaultCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.25,400);
	m_pkConsoleCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),84,1.333,0.3,400);	
  
	glMatrixMode(GL_MODELVIEW);

	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void ZeroFps::Swap(void) {
	SDL_GL_SwapBuffers();  //guess

	glLoadIdentity();
  
	//count FPS
	m_fFrameTime=SDL_GetTicks()-m_fLastFrameTime;
	m_fLastFrameTime=SDL_GetTicks();
	m_iFps=int(1000.0/m_fFrameTime);	

}


void ZeroFps::ToggleFullScreen(void)
{
	SDL_WM_ToggleFullScreen(m_pkScreen);
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
	m_pkRender->GetFrustum();				
	//Lighting needs camera position for light calculation
	m_pkLight->SetCamera(m_pkCamera->GetPos());				
	//update all lights
	m_pkLight->Update();	
		
}





int ZeroFps::LoadMAD(const char* filename)
{
	int iMadId = GetMADIndex(filename);

	if(iMadId != -1)
		return iMadId;

	Core CoreAdd;
	akCoreModells.push_back(CoreAdd);
	Core* pCoreMdl = &akCoreModells.back();
	pCoreMdl->SetTextureManger(m_pkTexMan);
	pCoreMdl->Load(filename);
	return (akCoreModells.size() - 1);
}

void ZeroFps::ClearMAD(void)
{
	akCoreModells.clear();
}

int ZeroFps::GetMADIndex(const char* filename)
{
	for(int i=0; i < akCoreModells.size(); i++)
	{
		if(strcmp(akCoreModells[i].GetName(),filename) == 0)
			return i;
	}

	return -1;
}

Core* ZeroFps::GetMADPtr(const char* filename)
{
	int iMadId = LoadMAD(filename);

	if(iMadId != -1)
		return &akCoreModells[iMadId];

	return NULL;
}

