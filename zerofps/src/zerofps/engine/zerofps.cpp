#include "zerofps.h"

ZeroFps::ZeroFps(void) {
	m_pkFile=new FileIo;
	m_pkCmd=new CmdSystem;
	m_pkTexMan=new TextureManager(m_pkFile);
	m_pkPrims=new Primitives(m_pkTexMan);
	m_pkConsole=new Console(this);	
	m_pkInput=new Input();
	m_pkModMan=new ModellManager(m_pkFile);
	
	m_pkCmd->Add(&m_iState,"m_iState",type_int);
	m_pkCmd->Add(&m_iFps,"m_iFps",type_int);
	m_pkCmd->Add(&m_fFrameTime,"m_fFrameTime",type_float);	
}


void ZeroFps::SetApp() {
	m_pkApp=Application::pkApp;
	m_pkApp->SetEnginePointer(this);
	
	
}


void ZeroFps::HandleArgs(int iNrOfArgs, char** paArgs) {
	cout<<"Nr of arguments: "<<iNrOfArgs<<endl;

}

void ZeroFps::Init(int iNrOfArgs, char** paArgs){	
	HandleArgs(iNrOfArgs,paArgs);					//handle arguments
	SetApp();															//setup class pointers	
	InitDisplay(m_pkApp->m_iWidth,m_pkApp->m_iHeight,m_pkApp->m_iDepth);
	
	m_iState=state_normal;								//init gamestate to normal		
	
	m_pkApp->OnInit();										//call the applications oninit funktion
	MainLoop();														//jump to mainloop
}

void ZeroFps::MainLoop(void) {
//	m_pkModMan->Load("horan/fuck/you");
	m_pkModMan->Load("horan/fuck/ass.pMd");

	while(m_iState!=state_exit) {		
		switch(m_iState){
			case state_normal:
				m_pkInput->Update();
				
				//this changes mode to console
				if(m_pkInput->Pressed(TAB)){
					m_iState=state_console;
					m_pkInput->Reset();
				}
				m_pkApp->OnIdle();			
				Swap();								
				break;			
			
			case state_console:
				m_pkConsole->Update();
				m_pkConsole->Draw();
				Swap();
				break;
		}
	};
}



void ZeroFps::InitDisplay(int iWidth,int iHeight,int iDepth) {

	//initiera sdl med opengl
	if(SDL_Init(SDL_OPENGL)<0){
		cout<<"Sdl_Graphic didt want to work right now =("<<endl;
		exit(1);
	}	
	
	//create sdl surface
	m_pkScreen= SDL_SetVideoMode(iWidth,iHeight,iDepth,SDL_OPENGL);
	glViewport(0, 0,iWidth,iHeight);	


		//setup some opengl stuff =)
	glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glEnable(GL_COLOR_MATERIAL);	

  glShadeModel(GL_SMOOTH);
  glClearColor(0, 0, 0, 0);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glMatrixMode(GL_MODELVIEW);

  
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

}

void ZeroFps::Swap(void) {
	SDL_GL_SwapBuffers();
	
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glFrustum(-0.25,0.25,-0.25,0.25,0.25,50.0);

	//count FPS
	m_fFrameTime=SDL_GetTicks()-m_fLastFrameTime;
	m_fLastFrameTime=SDL_GetTicks();
	m_iFps=int(1000/m_fFrameTime);
}



