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
//	if(m_pkInput->Pressed(SDL_KEY

}

void Console::Draw(void) {
	m_pkPrims->Quad(Vector3(0,0,-.50),Vector3(0,0,0),Vector3(1,1,1),m_pkTexMan->Load("data/textures/background.bmp"));

	
	for(int i=0;i<22;i++) {
		if(m_kText[i]!=NULL){
			m_pkPrims->Print(Vector3(-0.45,-0.40+i/(float)25,-0.499),Vector3(0,0,0),Vector3(.03,.03,.03),m_kText[i]);		
		}
	}
}



void Console::Print(char* aText) {
	for(int i=m_kText.size()-1;i>0;i--){
		if(m_kText[i-1]!=NULL){
//			cout<<"moving pos:"<<i-1<<" "<<m_kText[i-1]<<" to pos:"<<i<<" "<<m_kText[i]<<endl;
			m_kText[i]=m_kText[i-1];			
		}
	}
	
	m_kText[0]=aText;

//	cout<<"added "<<m_kText[0]<<endl;
}



