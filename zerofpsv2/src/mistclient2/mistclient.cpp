/**
	\defgroup MistClient MistClient
	\ingroup MistLand

  MistClient is the Client of the game MistLands.
*/

#ifndef _DONT_MAIN					// <- OBS! Flytta inte på denna. Måste ligga i
	#define _MAINAPPLICATION_		// just denna fil och inte på flera ställen.
	#define _DONT_MAIN
#endif

#include "mistclient.h"
#include "../zerofpsv2/engine_systems/propertys/p_camera.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../mcommon/p_arcadecharacter.h"
 
MistClient g_kMistClient("MistClient",0,0,0);

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
   return true;
}


MistClient::MistClient(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	g_ZFObjSys.Log_Create("mistclient2");

} 
 
void MistClient::OnInit() 
{
	m_pkZFVFileSystem->AddRootPath( string("../datafiles/mistlands/") ,"data/");
	m_pkZFVFileSystem->AddRootPath( string("../datafiles/dm/") ,"data/");


	//initiate our mainview camera
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.25,250);	
	m_pkCamera->SetName("Main camera");
	m_pkFps->AddRenderCamera(m_pkCamera);

	//register property bös
	RegisterPropertys();

	//register resources
	RegisterResources();
		
	//init mistland script intreface
	MistLandLua::Init(m_pkObjectMan,m_pkScript);

   // init gui script intreface
	GuiAppLua::Init(&g_kMistClient, m_pkScript);

	//set window title		
   SetTitle("MistClient - Hacka och slå");
	
   // initialize gui system with default skins, font etc
	InitGui(m_pkScript, "defguifont", "data/script/gui/defskins.lua", NULL, false, true); 

   // load startup screen 
   //LoadGuiFromScript("data/script/gui/ml_start.lua");

   // load software cursor
	m_pkGui->SetCursor( 0,0, m_pkTexMan->Load("data/textures/gui/cursor.bmp", 0),
		m_pkTexMan->Load("data/textures/gui/cursor_a.bmp", 0), 32, 32);
   m_pkInput->ShowCursor(false);

	//set client in server mode to show gui etc
	m_pkFps->StartServer(true,false);

	//run autoexec script
	if(!m_pkIni->ExecuteCommands("mistclient_autoexec.ini"))
		m_pkConsole->Printf("No game_autoexec.ini.ini found");	
}

	

void MistClient::RegisterResources()
{
	m_pkResourceDB->RegisterResource( string(".env"), Create__EnvSetting	);
}

void MistClient::RegisterPropertys()
{
	m_pkPropertyFactory->Register("P_ArcadeCharacter",	Create_P_ArcadeCharacter);
	m_pkPropertyFactory->Register("P_Enviroment", Create_P_Enviroment);
	m_pkPropertyFactory->Register("P_ServerInfo", Create_P_ServerInfo);
}


void MistClient::OnIdle() 
{

}

void MistClient::OnSystem() 
{
	if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_pkFps->GetClientObjectID()))
	{
		if(!pkEnt->GetProperty("P_Camera"))
		{
			if(P_Camera* pkCam = (P_Camera*)pkEnt->AddProperty("P_Camera"))
			{
				pkCam->SetCamera(m_pkCamera);
				cout<<"attached camera to client property"<<endl;
			}
		}
	}
}


void MistClient::OnHud(void) 
{	

}


void MistClient::OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass, bool bIsEditor)
{
	cout<<"Client "<<iConID<<" Joined"<<endl;
	
	cout << "Now adding er to client" << endl;
}

void MistClient::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Parted"<<endl;	
	
}


void MistClient::OnServerStart(void)
{		
}

void MistClient::OnClientStart(void)
{
	m_pkConsole->Printf("Trying to connect");
}










