#include "zerotank.h"
#include "../zerofpsv2/engine_systems/common/heightmap.h"

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
	m_pkCamera=new Camera(Vector3(0,10,0),Vector3(0,0,0),85,1.333,0.25,250);	
//	m_pkCamera->SetViewPort(0,0.24,1,0.76);
	
	//disable zones modells bös
	pkLevelMan->SetVisibleZones(false);

	m_pkMap2 = new Heightmap2;
	m_pkMap2->CreateHMFromImage("test.tga");

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
	
 	pkFps->UpdateCamera(); 	
	
	m_pkMap2->SetPos(Vector3(0,-5,-10));
	pkRender->DrawHM2(m_pkMap2,pkFps->GetCam()->GetPos());

	
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

	float speed = 20;

	//camera movements
	if(pkInput->Pressed(KEY_X)){
		speed*=0.25;
	}
	if(pkInput->Pressed(KEY_D)){
		pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y)/degtorad) *pkFps->GetFrameTime()*speed;			
		pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y)/degtorad) *pkFps->GetFrameTime()*speed;				
	}
	if(pkInput->Pressed(KEY_A)){
		pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y+180)/degtorad)*pkFps->GetFrameTime()*speed;			
		pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y+180)/degtorad)*pkFps->GetFrameTime()*speed;				
	}	
	if(pkInput->Pressed(KEY_W))	{
		pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
		pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
	}					
	if(pkInput->Pressed(KEY_S))	{
		pkFps->GetCam()->GetPos().x+=cos((pkFps->GetCam()->GetRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;			
		pkFps->GetCam()->GetPos().z+=sin((pkFps->GetCam()->GetRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;
	}		
	
	if(pkInput->Pressed(KEY_Q))
		pkFps->GetCam()->GetPos().y+=2*pkFps->GetFrameTime()*speed;			
	if(pkInput->Pressed(KEY_E))
		pkFps->GetCam()->GetPos().y-=2*pkFps->GetFrameTime()*speed;

	int x,z;		
	pkInput->RelMouseXY(x,z);	
	pkFps->GetCam()->GetRot().x+=z/5.0;
	pkFps->GetCam()->GetRot().y+=x/5.0;	
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

