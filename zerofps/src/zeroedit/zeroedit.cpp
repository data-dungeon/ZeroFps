#include "zeroedit.h"

ZeroEdit Editor("ZeroEdit",1024,768,16);

ZeroEdit::ZeroEdit(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) 
{

}

void ZeroEdit::OnInit(void) 
{
	pkConsole->Printf("            ZeroEdit ");
	pkConsole->Printf("--------------------------------");


	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),85,1.333,0.25,400);	

}


void ZeroEdit::OnIdle(void) 
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	



}


void ZeroEdit::OnHud(void) 
{


}


void ZeroEdit::RunCommand(int cmdid, const CmdArgument* kCommand)
{

}

void ZeroEdit::OnServerStart(void)
{	
}

void ZeroEdit::OnClientStart(void)
{	
}



