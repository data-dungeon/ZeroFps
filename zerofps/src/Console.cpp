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
	m_pkPrims->PrintChar('1');
	m_pkPrims->Print(Vector3(0,0,-0.499),Vector3(0,0,0),Vector3(0.05,0.05,0.05),"Hej Julle");
	
//	m_pkPrims->Quad(Vector3(0,0,-.499),Vector3(0,0,0),Vector3(.05,.05,.05),m_pkTexMan->Load("data/textures/text/1.bmp"));
}







