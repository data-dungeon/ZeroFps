#include "console.h"
#include "../engine/zerofps.h"
#include <cstdio>
#include <cstdarg>

using namespace std;

Console::Console()
  : BasicConsole("Console") {
   	
	m_pkEngine	= static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkInput	= static_cast<Input*>(g_ZFObjSys.GetObjectPtr("Input"));	
	m_pkCmd		= m_pkEngine->m_pkCmd;
	m_pkRender	= m_pkEngine->m_pkRender;
	m_pkTexMan  = m_pkEngine->m_pkTexMan;
	
	m_iBufferSize=100;
	m_kText.resize(m_iBufferSize);
	m_bShift=false;
	strcpy(m_aCommand,"");
	
	Print("ZeroFps engine started");
	Print("ZeroFps (C) Dvoid & Vim");
	Print("-----------------------");
	Print(" Type help for help =)");
	Print("");

	g_ZFObjSys.Register_Cmd("version",FID_VERSION,this);
	g_ZFObjSys.Register_Cmd("help",FID_HELP,this);
}

void Console::Update(void) {
	m_pkRender->DrawConsole(m_aCommand,&m_kText);	
	
	int iKey;
	while(m_pkInput->SizeOfQueue() > 0) {
		iKey = m_pkInput->GetQueuedKey();
		
		//press keys
		if(iKey == TAB) {
			glPopAttrib();
			
			m_pkEngine->m_bConsoleMode=false;
			m_pkInput->Reset();
				
			return;
		}


		if(iKey==SDLK_RETURN){
			Execute(m_aCommand);
			for(int i=0;i<TEXT_MAX_LENGHT;i++)					//wipe the command buffer
				m_aCommand[i]=' ';				
			strcpy(m_aCommand,"");
			continue;
		}
		if(iKey==SDLK_BACKSPACE){
			m_aCommand[strlen(m_aCommand)-1]='\0';
			continue;
		}
					
		if(m_pkInput->Pressed(RSHIFT) || m_pkInput->Pressed(LSHIFT)){
			m_bShift=true;
		}else{
			m_bShift=false;
		}
		
//		if(iKey==RSHIFT || iKey==LSHIFT)
//			continue;
		
		
		//type text
		if(strlen(m_aCommand)<COMMAND_LENGHT) {
			int code=iKey;
			
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
				if(code=='.'){
					code=':';
					strncat(m_aCommand,(char*)&(code),1);
					break;
				}					
				if(code=='7'){
					code='/';
					strncat(m_aCommand,(char*)&(code),1);
					break;
				}					
			}else
				strncat(m_aCommand,(char*)&(code),1);
		}
	}		
}
	
	
	
/*	
	while(SDL_PollEvent(&m_kEvent)) {
		
		//press keys
		if(m_kEvent.type==SDL_KEYDOWN){

			if(m_kEvent.key.keysym.sym==SDLK_TAB) {
				glPopAttrib();
//				m_pkEngine->m_iState=state_normal;
//				m_pkEngine->m_bClientMode=false;
				m_pkEngine->m_bConsoleMode=false;

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
					if(code=='.'){
						code=':';
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

*/


void Console::Execute(char* aText) {
	if(strlen(aText)==0){
		Print("");
		return;
	}
	
	g_ZFObjSys.RunCommand(aText);
}



