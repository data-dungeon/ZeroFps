#include "Console.h"
#include "ZeroFps.h"

/*Console::Console(CmdSystem* pkCmd,Primitives* pkPrims){
	m_pkCmd=pkCmd;
	m_pkPrims=pkPrims;
}*/

Console::Console(ZeroFps* pkEngine) {
//	m_pkEngine=pkEngine;	
	m_pkCmd=pkEngine->m_pkCmd;
	m_pkPrims=pkEngine->m_pkPrims;
	m_pkTexMan=pkEngine->m_pkTexMan;
	m_pkInput=pkEngine->m_pkInput;
	
	m_iBufferSize=100;
	m_kText.resize(m_iBufferSize);
}

void Console::Update(void) {
	while(SDL_PollEvent(&m_kEvent)) {
		if(m_kEvent.type==SDL_KEYDOWN){

			if(m_kEvent.key.keysym.sym==SDLK_RETURN){
				Execute(m_aCommand);
				strcpy(m_aCommand,"");
				break;
			}
			if(m_kEvent.key.keysym.sym==SDLK_BACKSPACE){
				m_aCommand[strlen(m_aCommand)-1]='\0';
				break;
			}
			
			
			if(strlen(m_aCommand)<TEXT_MAX_LENGHT) {
//				int code=int(m_kEvent.key.keysym.sym);
				
//				if(m_kEvent.key.keysym.mod|KMOD_SHIFT==m_kEvent.key.keysym.mod ||m_kEvent.key.keysym.mod|KMOD_SHIFT==m_kEvent.key.keysym.mod )
//					cout<<"balle "<<(int)(m_kEvent.key.keysym.mod|KMOD_SHIFT)<<endl;
				
/*				bool shift=m_kEvent.key.keysym.mod|KMOD_SHIFT;
					
				if(shift)
					cout<<"ta mig i rva"<<endl;
	*/				
/*				if(code>96 && code <123 ){
					(int)m_kEvent.key.keysym.sym-=32;
					strncat(m_aCommand,&(char)(m_kEvent.key.keysym.sym),1);
				}else{*/
					strncat(m_aCommand,&(char)(m_kEvent.key.keysym.sym),1);
//					cout<<"balle"<<endl;
//				}
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
		Print("-------+ help +-------");
		Print(" quit - exit program  ");
		return;
	}
	
	if(strncmp(aText,"set",4)==0) {
		
	}

	if(strcmp(aText,"varlist")==0) {
		Print("---+ variable list +---");
		for(int i=0;i<m_pkCmd->GetList().size();i++){
			char text[255]="";
			strcpy(text,m_pkCmd->GetList()[i]->aName);
//			strcat(text,"  ");
//			cout<<"old:"<<text<<endl;
//			Gemens(text);
//			cout<<text<<endl;

			strcat(text," = ");
//			strcat(text,atoi(m_pkCmd->GetVar(i)))
//			cout<<<<" = "<<m_pkCmd->GetVar(i)<<endl;
			Print(text);			
		}
		return;
	}

	Print("-+ unknown command +-");
}


void Console::Gemens(char* aText) 
{
	int i=0;
	
	while(aText[i]!='\0'){
		//gems  96>x<123     vers  64>x<91
		int code=int(aText[i]);
		
		if(code>64 && code<91)
			aText[i]=char(code+32);

		i++;
	}
}