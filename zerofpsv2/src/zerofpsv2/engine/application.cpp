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
	m_bIsEditor =  false;
}
	
/**
	Sets up all SubSystem ptr's so app can access subsystems.
*/
void Application::SetEnginePointer(ZeroFps* pkZeroFps) 
{
	m_pkZeroFps				=	pkZeroFps;	
	m_pkConsole				=	m_pkZeroFps->m_pkConsole;
	m_pkRender				=	m_pkZeroFps->m_pkRender;
	m_pkInput				=	m_pkZeroFps->m_pkInput;
	m_pkTexMan				=	m_pkZeroFps->m_pkTexMan;
	m_pkBasicFS				=	m_pkZeroFps->m_pkBasicFS;
	m_pkLight				=	m_pkZeroFps->m_pkLight;
	m_pkEntityManager		=	m_pkZeroFps->m_pkEntityManager;
	m_pkAudioSys			=	m_pkZeroFps->m_pkAudioSystem;
	m_pkPropertyFactory	=	m_pkZeroFps->m_pkPropertyFactory;
	m_pkGui					=	m_pkZeroFps->m_pkGui;
	m_pkGuiMan				=	m_pkZeroFps->m_pkGuiMan;
	m_pkIni					=	m_pkZeroFps->m_pkIni;
	m_pkScript				=	m_pkZeroFps->m_pkScript;
	m_pkZFVFileSystem		=	m_pkZeroFps->m_pkZFVFileSystem;
	m_pkResourceDB			=	m_pkZeroFps->m_pkResourceDB;
	m_pkZShadow				=	m_pkZeroFps->m_pkZShadow;
	m_pkZShaderSystem		=	m_pkZeroFps->m_pkZShaderSystem;
	m_pkTcs					=	m_pkZeroFps->m_pkTcs;

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
	kNp.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;	// ZF_NETTYPE_UNREL	ZF_NETTYPE_REL
	kNp.Write((char) ZPGP_SS_APP);
	
	kNp.WriteNp(pkNet);

	kNp.Write(ZFGP_ENDOFPACKET);
	kNp.m_iTargetClients = pkNet->m_iTargetClients;
	m_pkZeroFps->m_pkNetWork->Send2(&kNp);
}



