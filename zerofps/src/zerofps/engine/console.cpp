#include "console.h"
#include "../engine/zerofps.h"
#include <cstdio>
#include <cstdarg>

using namespace std;

Console::Console()
  : ZFObject("Console") {
   	
   m_pkEngine = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
//	m_pkEngine=pkEngine;	
	m_pkCmd=m_pkEngine->m_pkCmd;
	m_pkRender=m_pkEngine->m_pkRender;
	m_pkTexMan=m_pkEngine->m_pkTexMan;
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
	m_pkRender->DrawConsole(m_aCommand,&m_kText);	
	
	while(SDL_PollEvent(&m_kEvent)) {
		
		//press keys
		if(m_kEvent.type==SDL_KEYDOWN){

			if(m_kEvent.key.keysym.sym==SDLK_TAB) {
				glPopAttrib();
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
	string arg[20];		//list of arguments
	int args;					//number of arguments
	
	if(strlen(aText)==0){
		Print("");
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
		Print("ZeroFps Beta 1.0");
		return;
	}
	
	if(arg[0]=="help"){
		Print("");
		Print("### help ###");
		Print(" quit        -exit program");
		Print(" varlist     -list variables");		
		Print(" set $n $v   -set variable");		
		Print(" music 1/0   -music on/off");
		Print(" togglegrab  -toggle input grab");
		Print(" setdisplay  -reinit display");
		return;
	}
	
	if(arg[0]=="set") {
		char name[256]="";
		char value[20]="";
		int i=4;		

		if(arg[1].size()==0){
			Print("Please Supply a varible name");
			return;
		}

		if(arg[2].size()==0) {
			Print("Please Supply a value");
			return;
		}
		
		
		char text[255]="";
		strcpy(text,"Setting ");
		strcat(text,arg[1].c_str());
		strcat(text,"=");
		strcat(text,arg[2].c_str());
		Print(text);
		
		strcat(name,arg[1].c_str());
		
		if(!m_pkCmd->Set(name,atof(arg[2].c_str()))){
			Print("Variable not found");
			return;
		}
		
		return;
	}

	if(arg[0]=="varlist") {
		Print("");
		Print("### variable list ###");
		for(int i=0;i<m_pkCmd->GetList().size();i++){
			char text[255]="";
			char value[20]="";
			strcpy(text,m_pkCmd->GetList()[i]->aName);
			strcat(text," = ");
			
			IntToChar(value,(int)m_pkCmd->GetVar(i));
			strcat(text,value);
//			strcat(text,atoi(m_pkCmd->GetVar(i)))
//			cout<<<<" = "<<m_pkCmd->GetVar(i)<<endl;
			Print(text);			
		}
		return;
	}

	if(arg[0]=="music") {
		if(args==1) {
			Print("Syntax: music 1/0");		
			return;
		}
		if(arg[1]=="1")
			m_pkEngine->m_pkAudioMan->PlayMusic();
		if(arg[1]=="0")
			m_pkEngine->m_pkAudioMan->StopMusic();
			
		return;
	}

	if(arg[0]=="togglegrab") {
		m_pkEngine->m_pkInput->ToggleGrab();
		return;
	}

	if(arg[0]=="setdisplay") {
		m_pkEngine->SetDisplay();
		return;
	}


	Print("### unknown command ###");
}


static char format_text[4096];	//

void Console::Printf(const char *fmt, ...)
{
	va_list		ap;							// Pointer To List Of Arguments

	// Make sure we got something to work with.
	if (fmt == NULL)	return;					

	va_start(ap, fmt);						// Parses The String For Variables
		vsprintf(format_text, fmt, ap);		// And Convert Symbols
	va_end(ap);								// 

	// Now call our print function.
	Print(format_text);
}


