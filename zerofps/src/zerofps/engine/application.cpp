//#include "../engine/ZeroFps.h"
#include "../engine/engine.pkg"
#include "application.h"

Application* Application::pkApp;

Application::Application(char* pName,int iWidth,int iHeight,int iDepth) 
	: ZFObject("Application") 
{
	Application::pkApp=this;
	
	m_pTitle=pName;
	m_iWidth=iWidth;
	m_iHeight=iHeight;
	m_iDepth=iDepth;
}
	
void Application::SetEnginePointer(ZeroFps* pkZeroFps) 
{
	pkFps=pkZeroFps;	
	pkConsole=pkFps->m_pkConsole;
	pkRender=pkFps->m_pkRender;
	pkInput=pkFps->m_pkInput;
	pkTexMan=pkFps->m_pkTexMan;
	pkFile=pkFps->m_pkFile;
	pkBasicFS=pkFps->m_pkBasicFS;
	pkLight=pkFps->m_pkLight;
	pkObjectMan=pkFps->m_pkObjectMan;
	pkCollisionMan=pkFps->m_pkCollisionMan;
	pkAlSys=pkFps->m_pkOpenAlSystem;
	pkPropertyFactory=pkFps->m_pkPropertyFactory;
	pkGui=pkFps->m_pkGui;
	pkIni=pkFps->m_pkIni;
}
	
	
	
	

