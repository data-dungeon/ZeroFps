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
}

void Console::Update(void) {


}

void Console::Draw(void) {
	m_pkPrims->Pyra(0,0,-3);
	m_pkPrims->Pyra(1,0,-2);	
	m_pkPrims->Quad(Vector3(0,0,-.50),Vector3(0,0,0),Vector3(1,1,1),m_pkTexMan->Load("data/textures/background.bmp"));
//	m_pkPrims->Print(Vector3(0,0,-0.499),Vector3(0,0,0),Vector3(.04,.04,.04),"123321");
	
	for(int i=kText.size()-1;i>kText.size()-10 && i>=0;i--) {
		cout<<kText[i]<<endl;
	}
	

}



void Console::Print(char* aText) {
	



}



