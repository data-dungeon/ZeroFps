#include "test.h"


Test olle("MegaGame",800,600,16);


Test::Test(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) {


}


void Test::OnInit(void) {
	pkConsole->Print("MegaUltraSuper Duper Game");
	pkFps->m_pkAudioMan->LoadMusic("file:../data/music/killer_chin.mod");		
	pkFps->m_pkAudioMan->PlayMusic();

}


void Test::OnIdle(void) {


	for(int i=2;i<60;i++)
		pkPrims->Pyra(sin(pkFps->GetTicks()/1000.0*i/2)*2,cos(pkFps->GetTicks()/1000.0*i/2)*2,-i/2);


}

