#include "mistserver.h"
#include "../zerofpsv2/engine_systems/common/heightmap.h"
#include "../zerofpsv2/engine_systems/propertys/madproperty.h"
#include "../zerofpsv2/engine_systems/propertys/primitives3d.h"
#include "../zerofpsv2/engine_systems/propertys/proxyproperty.h"
#include "../zerofpsv2/gui/zgui.h"

MistServer g_kMistServer("MistServer",0,0,0);

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
	switch(msg)
	{
	case ZGM_COMMAND:
		g_kMistServer.OnCommand(((int*)params)[0], win);
		break;
	}
	return true;
}
MistServer::MistServer(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	g_ZFObjSys.Log_Create("mistserver");
} 

void MistServer::OnInit() 
{
	pkConsole->Printf(" MistServer (mistland dedicated server)");
	pkConsole->Printf("--------------------------------");
	pkConsole->Printf(" Hugga?");

	Init();

	//run autoexec script
	if(!pkIni->ExecuteCommands("mistserver_autoexec.ini"))
		pkConsole->Printf("No mistserver_autoexec.ini found");
}

void MistServer::Init()
{	
	//register commmands bös
	Register_Cmd("load",FID_LOAD);		

	//damn "#¤(="%#( lighting fix bös
	glEnable(GL_LIGHTING );
	
	//register property bös
	RegisterPropertys();

	//init mistland script intreface
	MistLandLua::Init(pkObjectMan,pkScript);

	SDL_WM_SetCaption("Mistland, the land of mist", NULL);
}

void MistServer::RegisterPropertys()
{

}

void MistServer::OnIdle() 
{
	Input();
}

void MistServer::OnSystem() 
{

}

void MistServer::Input()
{

};

void MistServer::OnHud(void)
{
	pkFps->DevPrintf("common","Active Propertys: %d",pkObjectMan->GetActivePropertys());	
	pkFps->DevPrintf("common", "Fps: %f",pkFps->m_fFps);	
	pkFps->DevPrintf("common","Avrage Fps: %f",pkFps->m_fAvrageFps);			
		
	pkFps->m_bGuiMode = false;
	pkFps->ToggleGui();
}

void MistServer::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) {
		case FID_LOAD:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				pkConsole->Printf("load [mapname]");
				break;				
			}
			
			cout<<"loading world:"<<kCommand->m_kSplitCommand[1].c_str()<<endl;
			
			if(!pkObjectMan->LoadWorld(kCommand->m_kSplitCommand[1].c_str()))
			{
				cout<<"Error loading world"<<endl;
				break;
			}
						
			cout<<"starting server"<<endl;
			GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);			
			
			break;		
		
	
	}

}

void MistServer::ClientInit()
{
	cout<<"Client Join granted"<<endl;
	
	cout<<"Join Complete"<<endl;
}

void MistServer::OnServerClientJoin(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Joined"<<endl;
	
	
	pkClient->m_pkObject->AddProperty("P_Primitives3D");	
	cout << "Now adding tracker to client" << endl;
	pkClient->m_pkObject->AddProperty("TrackProperty");	
	

}

void MistServer::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Parted"<<endl;	
}


void MistServer::OnServerStart(void)
{		
	
	
	if(pkObjectMan->GetNumOfZones() != 0) {
		pkConsole->Printf("Num of Zones: %d",pkObjectMan->GetNumOfZones());
	}
}

void MistServer::OnClientStart(void)
{
}

bool MistServer::StartUp()	{ return true; }
bool MistServer::ShutDown()	{ return true; }
bool MistServer::IsValid()	{ return true; }

void MistServer::OnCommand(int iID, ZGuiWnd *pkMainWnd)
{
/*	if(iID == 5)
		pkScript->Call(GetGuiScript(), "OnClickBackpack", 0, 0); 
	if(iID == 5)
		pkScript->Call(m_pkScriptResHandle, "OnClickBackpack", 0, 0); 
>>>>>>> 1.8
	if(iID == 6)
		pkScript->Call(m_pkScriptResHandle, "OnClickStats", 0, 0);
	if(iID == 4)
		pkScript->Call(GetGuiScript(), "OnClickMap", 0, 0);
		pkScript->Call(m_pkScriptResHandle, "OnClickMap", 0, 0);
*/
}
