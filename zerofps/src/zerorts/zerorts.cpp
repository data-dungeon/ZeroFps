#include "zerorts.h"

ZeroRTS g_kZeroRTS("ZeroRTS",1024,768,16);

ZeroRTS::ZeroRTS(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) { }

void ZeroRTS::OnInit() 
{
	pkConsole->Printf(" ZeroRTS");
	pkConsole->Printf("--------------------------------");
	pkConsole->Printf(" Does anything work yet?");
	
	//run autoexec script
	if(!pkIni->ExecuteCommands("zerorts_autoexec.ini"))
		pkConsole->Printf("No game_autoexec.ini.ini found");


	Init();
}

void ZeroRTS::Init()
{
	//damn "#¤(="%#( lighting fix
	glEnable(GL_LIGHTING );
	
	//initiate our camera
	m_pkCamera=new Camera(Vector3(0,10,0),Vector3(0,0,0),90,1.333,0.25,250);	
	
	//disable zones modells
	pkLevelMan->SetVisibleZones(false);


}


void ZeroRTS::OnIdle(void) 
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	
	pkFps->DevPrintf("common","Active Propertys: %d",pkObjectMan->GetActivePropertys());
			

}

void ZeroRTS::OnHud(void) 
{	

}

void ZeroRTS::OnServerStart(void)
{	
}

void ZeroRTS::OnClientStart(void)
{
}













