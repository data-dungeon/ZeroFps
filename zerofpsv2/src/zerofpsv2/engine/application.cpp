#include "application.h"
#include "zerofps.h"

Application* Application::pkApp;

Application::Application(char* pName,int iWidth,int iHeight,int iDepth) 
	: ZFObject("Application") 
{
	Application::pkApp = this;
	
	m_pTitle		=	pName;
	m_iWidth		=	iWidth;
	m_iHeight	=	iHeight;
	m_iDepth		=	iDepth;
}
	
void Application::SetEnginePointer(ZeroFps* pkZeroFps) 
{
	pkFps					=	pkZeroFps;	
	pkConsole			=	pkFps->m_pkConsole;
	pkRender				=	pkFps->m_pkRender;
	pkInput				=	pkFps->m_pkInput;
	pkTexMan				=	pkFps->m_pkTexMan;
	pkBasicFS			=	pkFps->m_pkBasicFS;
	pkLight				=	pkFps->m_pkLight;
	pkObjectMan			=	pkFps->m_pkObjectMan;
	pkAlSys				=	pkFps->m_pkOpenAlSystem;
	pkPropertyFactory =	pkFps->m_pkPropertyFactory;
	pkGui					=	pkFps->m_pkGui;
	pkGuiMan				=	pkFps->m_pkGuiMan;
	pkIni					=	pkFps->m_pkIni;
	pkLevelMan			=	pkFps->m_pkLevelMan;
	pkPhysEngine		=	pkFps->m_pkPhysEngine;
	pkScript				=	pkFps->m_pkScript;
	pkPhysics_Engine	=	pkFps->m_pkPhysics_Engine;
	pkZFVFileSystem	=	pkFps->m_pkZFVFileSystem;
}
	
	
	
	

