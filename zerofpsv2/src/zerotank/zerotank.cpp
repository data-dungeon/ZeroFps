#include "zerotank.h"
#include "../onefps/engine_systems/common/heightmap.h"

ZeroTank g_kZeroTank("ZeroTank",800,600,16);

ZeroTank::ZeroTank(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth) 
{
	m_iSelfObjectID			 = -1;
	m_HaveFoundHMapObject	 = false;
	m_iGameType				 = 1;
	
	g_ZFObjSys.Log_Create("zerorts");

}

void ZeroTank::OnInit() 
{
	pkConsole->Printf(" ZeroTank");
	pkConsole->Printf("--------------------------------");
	pkConsole->Printf(" Can we shoot yet?");

	Init();

	//run autoexec script
	if(!pkIni->ExecuteCommands("zerotank_autoexec.ini"))
		pkConsole->Printf("No game_autoexec.ini.ini found");
		
}

void ZeroTank::Init()
{	

	//get heightmap pointer bös
	m_pkMap = pkLevelMan->GetHeightMap();	

	//register variables
	g_ZFObjSys.RegisterVariable("m_iGameType", &m_iGameType,CSYS_INT);
	
	//register commmands bös
	g_ZFObjSys.Register_Cmd("load",FID_LOAD,this);		
	g_ZFObjSys.Register_Cmd("unload",FID_UNLOAD,this);			
	g_ZFObjSys.Register_Cmd("massspawn",FID_MASSSPAWN,this);			

	//damn "#¤(="%#( lighting fix bös
	glEnable(GL_LIGHTING );
	
	//initiate our camera bös
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(70,0,0),90,1.6,0.25,250);	
	m_pkCamera->SetViewPort(0,0.24,1,0.76);
	
	//disable zones modells bös
	pkLevelMan->SetVisibleZones(false);

	//register actions bös
	RegisterActions();

	//register property bös
	RegisterPropertys();
	
	srand( (int) (pkFps->GetGameTime()*1000) );

	char* szRandom[] =
	{
		"The Ultimate Fulhack!",
		"Does anything work yet?",
		"Apa..",
		"Made by gubb, vim, zeb, dvoid",
	};

	char szTitle[150];
	sprintf(szTitle, "zero rts - %s",szRandom[rand()%(sizeof(szRandom)/sizeof(szRandom[1]))]);

	SDL_WM_SetCaption(szTitle, NULL);
}

void ZeroTank::RegisterActions()
{
	m_iActionDoOrder=pkInput->RegisterAction("DoOrder");
	m_iActionPrintServerInfo=pkInput->RegisterAction("PrintServerInfo");
	m_iActionUnExploreAll=pkInput->RegisterAction("UnExploreAll");
	m_iActionExploreAll=pkInput->RegisterAction("ExploreAll");
	m_iActionCamLeft=pkInput->RegisterAction("CamLeft");
	m_iActionCamRight=pkInput->RegisterAction("CamRight");
	m_iActionCamUp=pkInput->RegisterAction("CamUp");
	m_iActionCamDown=pkInput->RegisterAction("CamDown");
	m_iActionSelect=pkInput->RegisterAction("Select");
	m_iActionScroll=pkInput->RegisterAction("Scroll");	
	m_iActionSelectManyModifier=pkInput->RegisterAction("SelectManyModifier");	
}

void ZeroTank::RegisterPropertys()
{

}


void ZeroTank::OnIdle() 
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	
			
	Input();
	

	
	//update player possition
	Object* pkObj = pkObjectMan->GetObjectByNetWorkID( m_iSelfObjectID );
	if(pkObj) {
		pkObjectMan->OwnerShip_Take( pkObj );
		pkObj->SetPos(pkFps->GetCam()->GetPos() + Vector3(0,-10,0));
		pkObj->SetPos(pkFps->GetCam()->GetPos() + Vector3(0,-10,0));
	}


}

void ZeroTank::OnSystem() 
{
	//setup client
	if(pkFps->m_bClientMode && !pkFps->m_bServerMode)
	{
		g_ZFObjSys.Logf("net","??? m_iSelfObjectID %d\n", m_iSelfObjectID);

/*		if(m_pkClientInput == NULL)		
		{
			g_ZFObjSys.Logf("net","??? m_pkClientInput == NULL\n");
			if(m_iSelfObjectID != -1)
			{
				g_ZFObjSys.Logf("net","??? m_iSelfObjectID != -1\n");
				Object* pkClientObj = pkObjectMan->GetObjectByNetWorkID(m_iSelfObjectID);
				if(pkClientObj) {
					g_ZFObjSys.Logf("net","??? pkClientObj found\n");
					m_pkClientInput = (P_ClientInput*)pkClientObj->GetProperty("P_ClientInput");

					if(m_pkClientInput != NULL)
					{
						g_ZFObjSys.Logf("net","??? Found client input property %d\n", m_pkClientInput->m_iPlayerID);
						cout<<"Found client input property"<<endl;					
						cout << "My Num: " << m_pkClientInput->m_iPlayerID;
					}
				}
			}
		}*/



		//try to get self id
		if(m_iSelfObjectID == -1)
			m_iSelfObjectID = pkFps->GetClientObjectID();

	};
	


}

void ZeroTank::Input()
{
	int mx,my;
	pkInput->MouseXY(mx,my);

	static int s_iCursorTex=-1; 
	static int s_iCursorTex_a=-1;
	int iNewCursorTex=0;
	int iNewCursorTex_a=0;


	if(pkInput->Action(m_iActionPrintServerInfo))
	{
	}

	if(pkInput->Pressed(KEY_W))
		pkLevelMan->ChangeLandscapeFillMode(LINE);
	if(pkInput->Pressed(KEY_F))
		pkLevelMan->ChangeLandscapeFillMode(FILL);

}

void ZeroTank::OnHud(void) 
{	
	pkFps->DevPrintf("common","Active Propertys: %d",pkObjectMan->GetActivePropertys());	
	pkFps->DevPrintf("common", "Fps: %f",pkFps->m_fFps);	
	pkFps->DevPrintf("common","Avrage Fps: %f",pkFps->m_fAvrageFps);			
	pkFps->DevPrintf("common","SelfID: %d", m_iSelfObjectID);	

	pkFps->m_bGuiMode = false;
	pkFps->ToggleGui();

}

void ZeroTank::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	Object* pkmad;

	switch(cmdid) {
		case FID_LOAD:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				pkConsole->Printf("load [mapname]");
				break;				
			}
			
			if(!pkLevelMan->LoadLevel(kCommand->m_kSplitCommand[1].c_str()))	
			{
				pkConsole->Printf("Error loading level");
				break;			
			}

			
			pkConsole->Printf("Level loaded");

			SetupSpawnPoints();
			
						
			pkConsole->Printf("Everything is loaded ,Starting server");
			g_ZFObjSys.RunCommand("server Default server");	
			break;		
		
		case FID_UNLOAD:
			break;
	
	}
}




void ZeroTank::HandleOrders()
{

}

void ZeroTank::ClientInit()
{
	cout<<"Client Join granted"<<endl;

	int mapwidth = pkLevelMan->GetHeightMap()->GetSize();

	cout<<"Mapsize is :"<<mapwidth<<endl;
	
	m_pkMap = pkLevelMan->GetHeightMap();	

	
	cout<<"Join Complete"<<endl;
}

void ZeroTank::OnServerClientJoin(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Joined"<<endl;
	
	pkClient->m_pkObject->AddProperty("ModelProperty");	
	
	//setup client input
	pkClient->m_pkObject->AddProperty("P_ClientInput");


}

void ZeroTank::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Parted"<<endl;	
	
}

void ZeroTank::SetupSpawnPoints()
{
	cout<<"Searching for spawn points"<<endl;	
	
	vector<Object*>	kObjects;	
	kObjects.clear();
	m_kSpawnPoints.clear();
	
	pkObjectMan->GetAllObjects(&kObjects);
	
	for(int i=0;i<kObjects.size();i++)
	{
		if(kObjects[i]->GetName() == "A ZeroRTSSpawnPoint")
		{
			m_kSpawnPoints.push_back(kObjects[i]->GetPos());
			pkObjectMan->Delete(kObjects[i]);
		}
	}

	cout<<"found "<<m_kSpawnPoints.size()<< " spawn points"<<endl;	
}

void ZeroTank::OnServerStart(void)
{	
	
	//add server info property
	if(!pkObjectMan->GetObject("A ServerInfoObject"))
	{
		Object* pkObj = pkObjectMan->CreateObjectByArchType("ServerInfoObject");
		if(!pkObj)
			cout<<"Faild to create serverinfoobject"<<endl;
		else
			pkObjectMan->GetWorldObject()->AddChild(pkObj);
	}
	
}

void ZeroTank::OnClientStart(void)
{
}

