#include "application.h"
#include "zerofps.h"

Application* Application::pkApp;
 
Application::Application(char* pName,int iWidth,int iHeight,int iDepth) 
	: ZFSubSystem("Application") 
{
	Application::pkApp = this;
	
	m_pTitle		=	pName;
	m_iWidth		=	iWidth;
	m_iHeight	=	iHeight;
	m_iDepth		=	iDepth;
}
	
/**
	Sets up all SubSystem ptr's so app can access subsystems.
*/
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
	pkAudioSys			=	pkFps->m_pkAudioSystem;
	pkAudioSys2			=	pkFps->m_pkAudioSystem2;
	pkPropertyFactory =	pkFps->m_pkPropertyFactory;
	pkGui					=	pkFps->m_pkGui;
	pkGuiMan				=	pkFps->m_pkGuiMan;
	pkIni					=	pkFps->m_pkIni;
//	pkLevelMan			=	pkFps->m_pkLevelMan;
	pkPhysEngine		=	pkFps->m_pkPhysEngine;
	pkScript				=	pkFps->m_pkScript;
	pkPhysics_Engine	=	pkFps->m_pkPhysics_Engine;
	pkZFVFileSystem	=	pkFps->m_pkZFVFileSystem;
	pkZShader			=	pkFps->m_pkZShader;
}
	
	
	
	

