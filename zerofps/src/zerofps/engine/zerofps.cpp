#include "zerofps.h"

ZeroFps::ZeroFps(void) {
	m_pkFile=new FileIo;
	m_pkCmd=new CmdSystem;
	m_pkTexMan=new TextureManager(m_pkFile);
	m_pkRender=new Render(m_pkTexMan);
	m_pkConsole=new Console(this);	
	m_pkInput=new Input();
	m_pkModMan=new ModellManager(m_pkFile);
	m_pkAudioMan=new AudioManager(this);
	
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

void ZeroFps::Init(int iNrOfArgs, char** paArgs)
{	

	HandleArgs(iNrOfArgs,paArgs);					//handle arguments
	SetApp();										//setup class pointers	
	InitDisplay(m_pkApp->m_iWidth,m_pkApp->m_iHeight,m_pkApp->m_iDepth);
	
	m_iState=state_normal;								//init gamestate to normal		
	
	m_pkApp->OnInit();										//call the applications oninit funktion
	MainLoop();														//jump to mainloop
}

void ZeroFps::MainLoop(void) {
	while(m_iState!=state_exit) {		
		switch(m_iState){
			case state_normal:{
				m_pkInput->Update();				
				//this changes mode to console
				if(m_pkInput->Pressed(TAB)){
					m_iState=state_console;
					m_pkInput->Reset();
				}
				m_pkApp->OnIdle();			
				Swap();

				break;			
			}
			case state_console: {
				m_pkConsole->Update();
				m_pkConsole->Draw();
				Swap();
				break;
			}
		}
	}
}



void ZeroFps::InitDisplay(int iWidth,int iHeight,int iDepth) {

	//initiera sdl med opengl
	if(SDL_Init(SDL_OPENGL | SDL_INIT_NOPARACHUTE )<0){
		cout<<"Sdl_Graphic didt want to work right now =("<<endl;
		exit(1);
	}	
	
	atexit(SDL_Quit);

	//create sdl surface
	m_pkScreen= SDL_SetVideoMode(iWidth,iHeight,iDepth,SDL_OPENGL);
	glViewport(0, 0,iWidth,iHeight);	


	//setup some opengl stuff =)
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_COLOR_MATERIAL);	
	glEnable(GL_CULL_FACE);

  glShadeModel(GL_SMOOTH);
  glClearColor(0, 0, 0, 0);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glMatrixMode(GL_MODELVIEW);

  
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	
	//set camera mode
	m_iCamMode=cam_look;
	m_kCamPos=new Vector3(0,0.5,0);
	m_kCamRot=new Vector3(0,0,0);

}

void ZeroFps::Swap(void) {
	SDL_GL_SwapBuffers();  //guess
	
	glLoadIdentity();													//
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);		// Reset view and clea
	glFrustum(-0.25,0.25,-0.25,0.25,0.25,50.0);				//
	
	//count FPS
	m_fFrameTime=SDL_GetTicks()-m_fLastFrameTime;
	m_fLastFrameTime=SDL_GetTicks();
	m_iFps=int(1000/m_fFrameTime);	

	if(m_iState==state_normal) {
		// Update Camera
		switch(m_iCamMode)
		{
		case cam_look:
			//rotate the camera
			glRotatef(m_kCamRot->x,1,0,0);	
			glRotatef(m_kCamRot->y,0,1,0);	
			glRotatef(m_kCamRot->z,0,0,1);	
		
			//translate the camera
			glTranslatef(-m_kCamPos->x,-m_kCamPos->y-0.2,-m_kCamPos->z);
			break;
		case cam_target:
			gluLookAt(m_kCamPos->x,m_kCamPos->y,m_kCamPos->z,
				m_kCamRot->x,m_kCamRot->y,m_kCamRot->z,0,1,0);
			break;
		}
	}
}

void ZeroFps::HandleCommands(char* aText){
		string arg[20];		//list of arguments
	int args;					//number of arguments
	
	if(strlen(aText)==0){
		m_pkConsole->Print("");
		return;
	}
	
	
	//read input parameters 
	args=0;
	for(int i=0;i<strlen(aText);i++) {
		while(int(aText[i])!=32 && i<strlen(aText)){	//loop until space
			arg[args].append(1,aText[i]);			//add to argument nr args
			i++;
		}
		if(arg[args].size()!=0)//if nothing was added to the argument use it in the next loop
			args++;
	}
	
	if(arg[0]=="quit"){
		exit(1);
		return;
	}
	
	if(arg[0]=="version") {
		m_pkConsole->Print("ZeroFps Beta 1.0");
		return;
	}
	
	if(arg[0]=="help"){
		m_pkConsole->Print("");
		m_pkConsole->Print("### help ###");
		m_pkConsole->Print(" quit         -exit program");
		m_pkConsole->Print(" varlist      -list variables");		
		m_pkConsole->Print(" set $n $v    -set variable");		
		return;
	}
	
	if(arg[0]=="set") {
		char name[256]="";
		char value[20]="";
		int i=4;		

		if(arg[1].size()==0){
			m_pkConsole->Print("Please Supply a varible name");
			return;
		}

		if(arg[2].size()==0) {
			m_pkConsole->Print("Please Supply a value");
			return;
		}
		
		
		char text[255]="";
		strcpy(text,"Setting ");
		strcat(text,arg[1].c_str());
		strcat(text,"=");
		strcat(text,arg[2].c_str());
		m_pkConsole->Print(text);
		
		strcat(name,arg[1].c_str());
		
		if(!m_pkCmd->Set(name,atof(arg[2].c_str()))){
			m_pkConsole->Print("Variable not found");
			return;
		}
		
		return;
	}

	if(arg[0]=="varlist") {
		m_pkConsole->Print("");
		m_pkConsole->Print("### variable list ###");
		for(int i=0;i<m_pkCmd->GetList().size();i++){
			char text[255]="";
			char value[20]="";
			strcpy(text,m_pkCmd->GetList()[i]->aName);
			strcat(text," = ");
			
			IntToChar(value,(int)m_pkCmd->GetVar(i));
			strcat(text,value);
//			strcat(text,atoi(m_pkCmd->GetVar(i)))
//			cout<<<<" = "<<m_pkCmd->GetVar(i)<<endl;
			m_pkConsole->Print(text);			
		}
		return;
	}

	if(arg[0]=="music") {
		if(args==1) {
			m_pkConsole->Print("Syntax: music 1/0");		
			return;
		}
		if(arg[1]=="1")
			m_pkAudioMan->PlayMusic();
		if(arg[1]=="0")
			m_pkAudioMan->StopMusic();
			
		return;
	}

	m_pkConsole->Print("### unknown command ###");
}



