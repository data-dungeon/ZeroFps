#include "ZeroFps.h"

ZeroFps::ZeroFps(void) {
	m_pkCmdSystem=new CmdSystem;


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
	
	m_iState=state_normal;								//init gamestate to normal
	
	m_pkApp->OnInit();										//call the applications oninit funktion
	
	MainLoop();														//jump to mainloop
}

void ZeroFps::MainLoop(void) {
	while(m_iState==state_normal) {
		m_pkApp->OnIdle();
	};
}

void ZeroFps::Error(char* aText) {
	cout<<endl;
	cout<<"---== ZeroFps Error ==--"<<endl;
	cout<<aText<<endl;
	exit(1);

}



