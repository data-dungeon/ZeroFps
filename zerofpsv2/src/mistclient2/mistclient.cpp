/**
	\defgroup MistClient MistClient
	\ingroup MistLand

  MistClient is the Client of the game MistLands.
*/

#include "mistclient.h"
#include "../zerofpsv2/engine_systems/propertys/p_camera.h"
 
MistClient g_kMistClient("MistClient",0,0,0);


MistClient::MistClient(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth)
{ 
	g_ZFObjSys.Log_Create("mistclient2");

} 
 
void MistClient::OnInit() 
{
	m_pkZFVFileSystem->AddRootPath( string("../data/mistlands/") ,"data/");
	m_pkZFVFileSystem->AddRootPath( string("../data/dm/") ,"data/");


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

	//set window title		
 	SetTitle("MistClient");
	
	//run autoexec script
	if(!m_pkIni->ExecuteCommands("mistclient_autoexec.ini"))
		m_pkConsole->Printf("No game_autoexec.ini.ini found");
		
	m_pkRender->SetFog(Vector3(0,0,0),10,50,false);			
		
	//set client in server mode to show gui etc
	m_pkFps->StartServer(true,false);
}

	

void MistClient::RegisterResources()
{
	m_pkResourceDB->RegisterResource( string(".env"), Create__EnvSetting	);
}

void MistClient::RegisterPropertys()
{
	m_pkPropertyFactory->Register("P_Enviroment", Create_P_Enviroment);
	m_pkPropertyFactory->Register("P_ServerInfo", Create_P_ServerInfo);
}


void MistClient::OnIdle() 
{

}

void MistClient::OnSystem() 
{
	//m_pkRender->SetClearColor(Vector4(1,0,0,0));

	if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_pkFps->GetClientObjectID()))
	{
		if(!pkEnt->GetProperty("P_Camera"))
		{
			if(P_Camera* pkCam = (P_Camera*)pkEnt->AddProperty("P_Camera"))
			{
				pkCam->SetCamera(m_pkCamera);
				cout<<"attached camera to client property"<<endl;
			}
			/*
				// Create and setup the Env on the server.
			if(P_Enviroment* pe = (P_Enviroment*)pkEnt->AddProperty("P_Enviroment"))
			{
				pe->SetEnable(true);		
				pe->SetEnviroment("data/enviroments/server.env");
			}
			*/
		}
	}
}


void MistClient::OnHud(void) 
{	

}


void MistClient::OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass)
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










