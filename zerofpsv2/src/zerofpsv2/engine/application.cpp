#include "application.h"
#include "zerofps.h"
#include "inputhandle.h"
 
Application* Application::m_pkApp;
 
Application::Application(char* pName,int iWidth,int iHeight,int iDepth) 
	: ZFSubSystem("Application") 
{
	Application::m_pkApp = this;
	
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
	m_pkFps					=	pkZeroFps;	
	m_pkConsole				=	m_pkFps->m_pkConsole;
	m_pkRender				=	m_pkFps->m_pkRender;
	m_pkInput				=	m_pkFps->m_pkInput;
	m_pkTexMan				=	m_pkFps->m_pkTexMan;
	m_pkBasicFS				=	m_pkFps->m_pkBasicFS;
	m_pkLight				=	m_pkFps->m_pkLight;
	m_pkObjectMan			=	m_pkFps->m_pkObjectMan;
	m_pkAudioSys			=	m_pkFps->m_pkAudioSystem;
	m_pkPropertyFactory	=	m_pkFps->m_pkPropertyFactory;
	m_pkGui					=	m_pkFps->m_pkGui;
	m_pkGuiMan				=	m_pkFps->m_pkGuiMan;
	m_pkIni					=	m_pkFps->m_pkIni;
	m_pkScript				=	m_pkFps->m_pkScript;
	m_pkZFVFileSystem		=	m_pkFps->m_pkZFVFileSystem;
	m_pkResourceDB			=	m_pkFps->m_pkResourceDB;
	m_pkZShadow				=	m_pkFps->m_pkZShadow;
	m_pkZShaderSystem		=	m_pkFps->m_pkZShaderSystem;


	m_pkInputHandle = new InputHandle("Application");
}

void Application::SetTitle(string strTitle)
{
	SDL_WM_SetCaption(strTitle.c_str(), NULL);
}

void Application::SendAppMessage(NetPacket* pkNet)
{
	NetPacket kNp;
	kNp.Clear();
	kNp.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
	kNp.Write((char) ZPGP_SS_APP);
	
	kNp.WriteNp(pkNet);

	kNp.Write(ZFGP_ENDOFPACKET);
	kNp.m_iTargetClients = pkNet->m_iTargetClients;
	m_pkFps->m_pkNetWork->Send2(&kNp);
}



