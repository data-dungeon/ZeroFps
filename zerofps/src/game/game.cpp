#include "game.h"

Game olle("ZeroFPS game",1024,768,16);

Game::Game(char* aName,int iWidth,int iHeight,int iDepth): Application(aName,iWidth,iHeight,iDepth) { }

void Game::OnInit() 
{
	Init();
	
	pkConsole->Printf(" ZeroFPS project game");
	pkConsole->Printf("--------------------------------");
	pkConsole->Printf(" Use load [map] to load a map");
	
	SetUpMenuScreen();


}

void Game::Init()
{
	//setup some default variables
	RegisterPropertys();

	//register commmands
	g_ZFObjSys.Register_Cmd("load",FID_LOAD,this);		
	g_ZFObjSys.Register_Cmd("unload",FID_UNLOAD,this);			
	
	//damn "#¤(="%#( lighting fix
	glEnable(GL_LIGHTING );
	
	//initiate our camera
	m_pkCamera=new Camera(Vector3(0,10,0),Vector3(0,0,0),85,1.333,0.25,250);	
	
	//disable zones modells
	pkLevelMan->SetVisibleZones(false);
	
	//set gamestate to menu
	m_iGameState=GAME_STATE_MENU;
}

void Game::OnServerStart(void)
{	
}

void Game::OnClientStart(void)
{
}



void Game::OnIdle(void) {
	switch(m_iGameState)
	{
		case GAME_STATE_MENU:
		{
			pkFps->SetCamera(m_pkCamera);		
			pkFps->GetCam()->ClearViewPort();	
			
			pkObjectMan->Update(PROPERTY_TYPE_RENDER, PROPERTY_SIDE_CLIENT, true);
			pkFps->DevPrintf("Active Propertys: %d",pkObjectMan->GetActivePropertys());
		
			
			break;
		}
	
		case GAME_STATE_INGAME:
		{
			pkFps->SetCamera(m_pkCamera);		
			pkFps->GetCam()->ClearViewPort();	
			
			pkObjectMan->Update(PROPERTY_TYPE_RENDER, PROPERTY_SIDE_CLIENT, true);
			pkFps->DevPrintf("Active Propertys: %d",pkObjectMan->GetActivePropertys());			
			
			break;
		}
	}
}

void Game::OnHud(void) 
{	

	glPushAttrib(GL_LIGHTING_BIT);
	
	glDisable(GL_LIGHTING);

	pkRender->SetFont("file:../data/textures/text/console.tga");

	pkFps->DevPrintf("Fps: %d",pkFps->m_iFps);


	glAlphaFunc(GL_GREATER,0.3);
	glEnable(GL_ALPHA_TEST);

//	pkRender->Quad(Vector3(.8,.8,-1),Vector3(0,0,m_pkPlayer->GetRot().y),Vector3(0.2,0.2,0.2),pkTexMan->Load("file:../data/textures/compas.tga",0));
	
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_LIGHTING);	
	
	glPopAttrib();
}


void Game::input() 
{
		
		/*
	//Get mouse x,y		
	int x,z;		
	pkInput->RelMouseXY(x,z);

	//rotate the camera		
	pkFps->GetCam()->GetRot().x+=z/5.0;
	pkFps->GetCam()->GetRot().y+=x/5.0;
	*/
}

void Game::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) {
		case FID_LOAD:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				pkConsole->Printf("load [mapname]");
				break;				
			}
			if(!pkLevelMan->LoadLevel(kCommand->m_kSplitCommand[1].c_str()))	
			{
				pkConsole->Printf("Error loading level");
				break;			
			}		
			
			SetupLevel();
			
			pkConsole->Printf("Level loaded");
			
			break;		
		
		case FID_UNLOAD:
			SetUpMenuScreen();			
			break;
	}
}

/*
void Game::CreateZones()
{
	int radius=250;

//	HeightMap *m_pkMap=test;
//	cout<<"SIZE"<<m_pkMap->m_iHmSize<<endl;

	for(int x=0;x<m_pkMap->m_iHmSize;x+=radius/3){
		for(int z=0;z<m_pkMap->m_iHmSize;z+=radius/3){
			if(m_pkMap->Height(x,z)>-1){
				ZoneObject *object = new ZoneObject();
				object->GetPos()=Vector3(x,m_pkMap->Height(x,z),z);
				object->SetRadius(radius);
				object->SetParent(pkObjectMan->GetWorldObject());
				object->DeleteProperty("MadProperty");
			}
		}
	}
}
*/

void Game::RegisterPropertys()
{
	pkPropertyFactory->Register("FloatProperty", Create_FloatProperty);
	pkPropertyFactory->Register("GravityProperty", Create_GravityProperty);	

}


/*
bool Game::LoadLevel(const char* acFile)
{
	for(int i=0;acFile[i]!='\0';i++)
	{
		if(acFile[i]=='\\' || acFile[i]=='/')
		{
			pkConsole->Print("Bad filename");
			return false;
		}
	}
		
	string kHmfile;
	string kZolfile;
	string kInifile;

	kHmfile=m_kMapBaseDir;
	kHmfile+="/";
	kHmfile+=acFile;
	kHmfile+="/";	
	kHmfile+="heightmap.hm";

	kZolfile=m_kMapBaseDir;
	kZolfile+="/";
	kZolfile+=acFile;
	kZolfile+="/";	
	kZolfile+="objects.zol";

	kInifile=m_kMapBaseDir;
	kInifile+="/";
	kInifile+=acFile;
	kInifile+="/";	
	kInifile+="config.ini";

	Clear();
	
	if(!m_pkMap->Load(kHmfile.c_str())){
		pkConsole->Printf("Error loading heightmap");
		return false;
	};
	cout<<"heightmap loaded"<<endl;
	
	//create zoneobjects
	CreateZones();		
	
	if(!pkObjectMan->LoadAllObjects(kZolfile .c_str())){
		pkConsole->Printf("Error loading objects");
		return false;
	}		
	cout<<"objects loaded"<<endl;

	if(!pkIni->ExecuteCommands(kInifile.c_str())){
		cout<<"No ini-file found"<<endl;
	}

	//setup player aso
	SetupLevel();

	return true;
}

void Game::Clear() 
{
	CreateNew(100);
}

void Game::CreateNew(int iSize) 
{
	delete m_pkMap;
	m_pkMap=new HeightMap();	
	
	pkObjectMan->Clear();
//	m_pkCurentChild=NULL;

	m_pkHeightMapObject=new HeightMapObject(m_pkMap);		
	m_pkHeightMapObject->SetParent(pkObjectMan->GetWorldObject());
	
//	pkObjectMan->Add(m_pkHeightMapObject);	
	pkCollisionMan->Add(m_pkHeightMapObject);

//	m_pkCurentParent=m_pkHeightMapObject;
	
	m_pkMap->Create(iSize);
	m_pkMap->GenerateNormals(); 
	m_pkMap->GenerateTextures();

	m_pkHeightMapObject->GetPos().Set(0,-4,0);				
	m_pkMap->SetPosition(Vector3(0,-4,0));
	

	//skybox
	SkyBoxObject *sky=new SkyBoxObject("file:../data/textures/skybox-hor.bmp","file:../data/textures/skybox-topbotom.bmp");
	sky->SetParent(pkObjectMan->GetWorldObject());	
	sky->SetRotate(Vector3(.5,0,0));

	//water
	WaterObject *water=new WaterObject(1200,50,"file:../data/textures/water2.bmp");
	water->GetPos().Set(512,0,512);
	water->SetParent(pkObjectMan->GetWorldObject());

	cout<<"new map"<<endl;
}
*/

void Game::SetUpMenuScreen()
{
	pkLevelMan->LoadLevel("menu");

	m_pkCamera->GetPos().Set(94.5,23,98.1);	
	m_pkCamera->GetRot().Set(13,138,0);	

	m_iGameState=GAME_STATE_MENU;
}


void Game::SetupLevel()
{
	list<Object*> kObjects;		
	pkObjectMan->GetAllObjects(&kObjects);
	
	Object* po=NULL;
	
	for(list<Object*>::iterator it=kObjects.begin();it!=kObjects.end();it++) 
	{
		if((*it)->GetName() == "PLAYER_SPAWN_POINT")
		{
			//player
			PlayerObject* m_pkPlayer=new PlayerObject(pkLevelMan->GetHeightMap(),pkInput);
			m_pkPlayer->GetPos() = (*it)->GetPos();
			m_pkPlayer->GetRot() = (*it)->GetRot();			
			m_pkPlayer->AddProperty(new CameraProperty(m_pkCamera));
			m_pkPlayer->AttachToClosestZone();
			pkCollisionMan->Add(m_pkPlayer);			
			
			po=(*it);
		}		
	}
	
	if(po==NULL)
	{
		pkConsole->Printf("ERROR: Player spawn point is missing");
		return;
	}
	
	delete po;
	
	m_iGameState=GAME_STATE_INGAME;
}






