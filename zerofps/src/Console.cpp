#include "Console.h"
#include "ZeroFps.h"

Console::Console(ZeroFps* pkEngine) {
//	m_pkEngine=pkEngine;	
	m_pkCmd=pkEngine->m_pkCmd;
	m_pkPrims=pkEngine->m_pkPrims;
	m_pkTexMan=pkEngine->m_pkTexMan;
	m_pkInput=pkEngine->m_pkInput;
	
	m_iBufferSize=100;
	m_kText.resize(m_iBufferSize);
	m_bShift=false;
	
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
			if(strlen(m_aCommand)<TEXT_MAX_LENGHT) {
				int code=m_kEvent.key.keysym.sym;
				
				//shift?
				if(m_bShift) {
					if(code>96 && code<123){
						code-=32;
						strncat(m_aCommand,&(char)(code),1);
						break;
					}
					if(code=='-'){
						code='_';
						strncat(m_aCommand,&(char)(code),1);
						break;
					}
					if(code=='7'){
						code='/';
						strncat(m_aCommand,&(char)(code),1);
						break;
					}					
				}
				strncat(m_aCommand,&(char)(code),1);
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
	m_pkPrims->SetFont("data/textures/text/console.bmp");

	m_pkPrims->Quad(Vector3(0,0,-.50),Vector3(0,0,0),Vector3(1,1,1),m_pkTexMan->Load("data/textures/background.bmp"));
	
	m_pkPrims->Print(Vector3(-0.45,-0.45,-0.499),Vector3(0,0,0),Vector3(.03,.03,.03),m_aCommand);		
	
	for(int i=0;i<22;i++) {
		if(m_kText[i]!=NULL){
			m_pkPrims->Print(Vector3(-0.45,-0.40+i/(float)25,-0.499),Vector3(0,0,0),Vector3(.03,.03,.03),m_kText[i]);		
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
	if(strlen(aText)==0){
		Print("");
		return;
	}
	

	if(strcmp(aText,"quit")==0){
		exit(1);
		return;
	}
	
	if(strcmp(aText,"help")==0){
		Print("");
		Print("### help ###");
		Print(" quit         -exit program");
		Print(" varlist      -list variables");		
		Print(" set $n $v    -set variable");		
		return;
	}
	
	if(strncmp(aText,"set ",4)==0) {
		char name[256]="";
		char value[20]="";
		int i=4;		

		while(aText[i]!='\0' && aText[i]!=' '){
			strncat(name,&aText[i],1);
			i++;
		}
		if(strlen(name)<1){
			Print("Please Supply a varible name");
			return;
		}

		i++;
		while(aText[i]!='\0' && aText[i]!=' '){
			strncat(value,&aText[i],1);
			i++;
		}

		if(strlen(value)==0) {
			Print("Please Supply a value");
			return;
		}
		
		
		char text[255]="";
		strcpy(text,"Setting ");
		strcat(text,name);
		strcat(text,"=");
		strcat(text,value);
		Print(text);
		
		if(!m_pkCmd->Set(name,atof(value))){
			Print("Variable not found");
			return;
		}
		
		return;
	}

	if(strcmp(aText,"varlist")==0) {
		Print("");
		Print("### variable list ###");
		for(int i=0;i<m_pkCmd->GetList().size();i++){
			char text[255]="";
			char value[20]="";
			strcpy(text,m_pkCmd->GetList()[i]->aName);
			strcat(text," = ");
			
			IntToChar(value,m_pkCmd->GetVar(i));
			strcat(text,value);
//			strcat(text,atoi(m_pkCmd->GetVar(i)))
//			cout<<<<" = "<<m_pkCmd->GetVar(i)<<endl;
			Print(text);			
		}
		return;
	}

	Print("### unknown command ###");
}


