#include "console.h"
#include "../engine/zerofps.h"

Console::Console(ZeroFps* pkEngine) {
	m_pkEngine=pkEngine;	
	m_pkCmd=pkEngine->m_pkCmd;
	m_pkRender=pkEngine->m_pkRender;
	m_pkTexMan=pkEngine->m_pkTexMan;
//	m_pkInput=pkEngine->m_pkInput;
	
	m_iBufferSize=100;
	m_kText.resize(m_iBufferSize);
	m_bShift=false;
	strcpy(m_aCommand,"");
	
	Print("ZeroFps engine started");
	Print("ZeroFps (C) Dvoid & Vim");
	Print("-----------------------");
	Print(" Type help for help =)");
	Print("");
}

void Console::Update(void) {
	while(SDL_PollEvent(&m_kEvent)) {
		
		//press keys
		if(m_kEvent.type==SDL_KEYDOWN){

			if(m_kEvent.key.keysym.sym==SDLK_TAB) {
				m_pkEngine->m_iState=state_normal;
				return;
			}

			if(m_kEvent.key.keysym.sym==SDLK_RETURN){
				Execute(m_aCommand);
				for(int i=0;i<TEXT_MAX_LENGHT;i++)					//wipe the command buffer
					m_aCommand[i]=' ';				
				strcpy(m_aCommand,"");
				break;
			}
			if(m_kEvent.key.keysym.sym==SDLK_BACKSPACE){
				m_aCommand[strlen(m_aCommand)-1]='\0';
				break;
			}
						
			if(m_kEvent.key.keysym.sym==SDLK_LSHIFT || m_kEvent.key.keysym.sym==SDLK_RSHIFT){
				m_bShift=true;
				break;
			}

			
			//type text
			if(strlen(m_aCommand)<COMMAND_LENGHT) {
				int code=m_kEvent.key.keysym.sym;
				
				//shift?
				if(m_bShift) {
					if(code>96 && code<123){
						code-=32;
						strncat(m_aCommand,(char*)&(code),1);
						break;
					}
					if(code=='-'){
						code='_';
						strncat(m_aCommand,(char*)&(code),1);
						break;
					}
					if(code=='7'){
						code='/';
						strncat(m_aCommand,(char*)&(code),1);
						break;
					}					
				}
				strncat(m_aCommand,(char*)&(code),1);
			}
		}
		
		//release keys
		if(m_kEvent.type==SDL_KEYUP){									
			if(m_kEvent.key.keysym.sym==SDLK_LSHIFT || m_kEvent.key.keysym.sym==SDLK_RSHIFT){
				m_bShift=false;
				break;
			}		
		}
	}
}

void Console::Draw(void) {
	m_pkRender->SetFont("file:../data/textures/text/console.bmp");

	m_pkRender->Quad(Vector3(0,0,-.50),Vector3(0,0,0),Vector3(1,1,1),m_pkTexMan->Load("file:../data/textures/background.bmp"));
	
	m_pkRender->Print(Vector3(-0.45,-0.45,-0.499),Vector3(0,0,0),Vector3(.03,.03,.03),m_aCommand);		
	
	for(int i=0;i<22;i++) {
		if(m_kText[i]!=NULL){
			m_pkRender->Print(Vector3(-0.45,-0.40+i/(float)25,-0.499),Vector3(0,0,0),Vector3(.03,.03,.03),m_kText[i]);		
		}
	}
}



void Console::Print(char* aText) {
	delete[] m_kText[m_kText.size()-1];
	
	for(int i=m_kText.size()-1;i>0;i--){
		if(m_kText[i-1]!=NULL){
//			cout<<"moving pos:"<<i-1<<" "<<m_kText[i-1]<<" to pos:"<<i<<" "<<m_kText[i]<<endl;
			m_kText[i]=m_kText[i-1];			
		}
	}
	
	m_kText[0]=new char[TEXT_MAX_LENGHT];
	strcpy(m_kText[0],aText);

//	cout<<"added "<<m_kText[0]<<endl;
}

void Console::Execute(char* aText) {
	m_pkEngine->HandleCommands(aText);

}


