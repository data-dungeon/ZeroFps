#include "ZeroFps.h"

ZeroFps::ZeroFps(void) {
	m_pkCmd=new CmdSystem;
	m_pkTexMan=new TextureManager;
	m_pkPrims=new Primitives(m_pkTexMan);
	m_pkConsole=new Console(this);	
	m_pkInput=new Input();
	
	m_pkCmd->Add(&m_iState,"m_iState",type_int);
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
	
	m_iState=state_console;								//init gamestate to normal		
	
	m_pkApp->OnInit();										//call the applications oninit funktion
	MainLoop();														//jump to mainloop
}

void ZeroFps::MainLoop(void) {
	
	m_pkPrims->Pyra(0,0,-3);
	m_pkPrims->Pyra(1,0,-2);	
	
	
	m_pkConsole->Print("123");	
	m_pkConsole->Print("321");	
	m_pkConsole->Print("111");	
	m_pkConsole->Print("000000000 111111 22222 3");
	m_pkConsole->Print("+-----------------------------+");
	
	while(m_iState!=state_exit) {
		switch(m_iState){
			case state_normal:
				m_pkInput->Update();
				m_pkApp->OnIdle();			
				Swap();								
				break;			
			
			case state_console:
				m_pkInput->Update();			
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

}



