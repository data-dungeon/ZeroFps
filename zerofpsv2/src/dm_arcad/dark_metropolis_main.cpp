	// dark_metropolis_main.cpp

#ifndef _DONT_MAIN					// <- OBS! Flytta inte på denna. Måste ligga i
	#define _MAINAPPLICATION_		// just denna fil och inte på flera ställen.
	#define _DONT_MAIN
#endif

#include "dark_metropolis.h"
#include "gameplay_dlg.h"
#include "members_dlg.h"
#include "hq_dlg.h"
#include "../mcommon/si_dm.h"
#include "../mcommon/p_dmclickme.h"
#include "../mcommon/p_dmshop.h"
#include "../zerofpsv2/render/glguirender.h"

DarkMetropolis g_kDM("DM_arcad",0,0,0);

DarkMetropolis::DarkMetropolis(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	g_ZFObjSys.Log_Create("DM_arcad");	

	RegisterVariable("r_enablemusic",&m_iEnableMusic,CSYS_INT);

	pkTreadInfo = new THREAD_INFO;
} 

void DarkMetropolis::OnHud() 
{	
	m_pkFps->DevPrintf("common","Active Propertys: %d",
		m_pkObjectMan->GetActivePropertys());	
	m_pkFps->DevPrintf("common", "Fps: %f",m_pkFps->m_fFps);	
	m_pkFps->DevPrintf("common","Avrage Fps: %f",m_pkFps->m_fAvrageFps);			
		
	m_pkFps->m_bGuiMode = false;
	m_pkFps->ToggleGui();
}

void DarkMetropolis::OnInit()
{
	SetTitle("Dark Metropolis");

	m_pkZFVFileSystem->AddRootPath( string("../data/mistlands/") ,"data/");
	m_pkZFVFileSystem->AddRootPath( string("../data/dm/") ,"data/");
	

	//initiate variables
	m_pkCameraProp	= 				NULL;
	m_pkCameraEntity = 			NULL;
	m_pkFps->m_bClientMode =	true;
	m_fMinCamDistance =			0.1f;
	m_fMaxCamDistance =			8;
	m_fDistance =					m_fMaxCamDistance;	
	m_fAngle =						0;
	m_strSaveDirectory =			"savegames/";
	m_pkGameInfoProperty = 		NULL;
	m_pkGameInfoEntity =			NULL;
	m_bSelectSquare = 			false;
	m_fDelayTimer = 				0;
	m_iCurrentFormation =		FORMATION_CIRCLE;
	m_bActionPressed = 			false;
	m_iHQID = 						-1;
	m_iActiveHQ = 					-1;
	m_eGameMode	=					ACTIVE;
	m_fBulletTime =				-1;
	m_pkGamePlayInfoLabel =		NULL;
	m_fCameraMaxDistanceFromAgent = 6;
	m_fMusicVolume = 				0.5f;

	m_pkPlayerEntity =			NULL;
	m_iPlayerEntityID =			-1;
	
	
	//register commands
	Register_Cmd("load",FID_LOAD);			
	Register_Cmd("save",FID_SAVE);	

	//setup system speed
	m_pkFps->SetSystemFps(30);
	m_pkObjectMan->m_fSimTimeScale = 0.5;
	
	//set tracker los
	m_pkObjectMan->SetTrackerLos(5);
	
	//enable light
	m_pkLight->SetLighting(true);

	//set reference distance in audio syustem
	m_pkAudioSys->SetReferensDistance(5);

	//create camera
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.25,250);	
	m_pkFps->SetRenderTarget(m_pkCamera);
	m_pkCamera->SetRender(true);

	LoadResourcesOnStartup();

	//init dm script interface (register script functions for gameplay)
	DMLua::Init(m_pkObjectMan,m_pkScript,m_pkGuiMan);

	//register propertys
	RegisterPropertys();

	//registering resources
	m_pkResourceDB->RegisterResource( string(".env"), Create__EnvSetting	);

	ZGuiRender* pkRenderer = static_cast<ZGuiRender*>(g_ZFObjSys.GetObjectPtr("ZGuiRender"));
	pkRenderer->SetScaleMode(GUIScaleManually);

	// create gui script
	GUI_Init();
	
	if(!m_pkIni->ExecuteCommands("dark_metropolis_autoexec.ini"))
		m_pkConsole->Printf("No dark_metropolis_autoexec.ini found");

	// load marker texture
	m_iMarkerTextureID = this->m_pkFps->m_pkTexMan->Load("data/textures/dm/marker.tga", 0);

}

void DarkMetropolis::OnIdle() 
{
	m_pkFps->SetCamera(m_pkCamera);		
	m_pkFps->GetCam()->ClearViewPort();

	if(m_eGameMode != PAUSED)
	{
		Input();
		
		//update camera position
		CheckCameraPos();	
	}
	

//	m_pkFps->UpdateCamera(); 	
	GUI_OnIdle();
}

void DarkMetropolis::RenderInterface(void)
{
//	m_strGameInfoText = "";

	if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentPickedEntity))
	{
		P_DMItem* pkItem;
		if((pkItem=(P_DMItem*)pkEnt->GetProperty("P_DMItem")))
		{
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GEQUAL, 0.1);
			
			glColor3f (0, 0, 1);

			m_pkRender->Quad (pkEnt->GetWorldPosV(), Vector3 (-90, 0, 0), Vector3(0.5, 0.5, 0.5), m_iMarkerTextureID, Vector3(1,1,0) );

			glDisable(GL_ALPHA_TEST);

			m_strGameInfoText = pkItem->GetName();
		}
		else
			m_strGameInfoText = "";
	
		P_DMClickMe* pkClickMe;
		if((pkClickMe=(P_DMClickMe*)pkEnt->GetProperty("P_DMClickMe")))
		{
			if ( !pkClickMe->m_bIsHouse )
			{
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GEQUAL, 0.1);
				
				glColor3f (0, 0, 1);

				m_pkRender->Quad (pkEnt->GetWorldPosV(), Vector3 (-90, 0, 0), Vector3(0.5, 0.5, 0.5), m_iMarkerTextureID, Vector3(1,1,0) );

				glDisable(GL_ALPHA_TEST);
			}
		}

		if(P_DMCharacter* pkChar = (P_DMCharacter*)pkEnt->GetProperty("P_DMCharacter"))
		{
			if ( pkChar->GetStats()->m_iLife > 0 )
			{
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GEQUAL, 0.1);
				
				Vector3 kColor;

				if ( pkChar->m_iTeam == 0 )
					kColor = Vector3 (0,255,0);
				else if ( pkChar->m_iTeam == 1 )
					kColor = Vector3 (0,0,255);
				else
					kColor = Vector3 (255,255,255);

				m_pkRender->Quad (pkEnt->GetWorldPosV(), Vector3 (-90, 0, 0), Vector3(0.9, 0.9, 0.9), m_iMarkerTextureID, kColor );

				glDisable(GL_ALPHA_TEST);
			}

			if(pkChar->m_iTeam == 0)
				m_strGameInfoText = string("Agent: ") + pkChar->GetStats()->m_strName;
		}
	
	}
	
	//draw markers for selected entitys
	for(unsigned int i = 0;i< m_kSelectedEntitys.size();i++)
	{
		Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_kSelectedEntitys[i]);
		if(pkEnt)
		{
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GEQUAL, 0.1);

			m_pkRender->Quad (pkEnt->GetIWorldPosV(), Vector3 (-90, 0, 0), Vector3(0.9, 0.9, 0.9), m_iMarkerTextureID, Vector3(0,255,0) );

			glDisable(GL_ALPHA_TEST);
		}	
	}
	
	//draw HQ marker
	if(m_iHQID != -1)
	{
		Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_iHQID);
		if(pkEnt)
		{
			float r = 1;//pkEnt->GetBoundingRadius();
			m_pkRender->DrawAABB(pkEnt->GetWorldPosV()-Vector3(r,r,r),pkEnt->GetWorldPosV()+Vector3(r,r,r),Vector3(1,1,0));
		}
	}
	
	if(m_pkPlayerEntity)
	{
		if(P_ArcadeCharacter* pkAC = (P_ArcadeCharacter*)m_pkPlayerEntity->GetProperty("P_ArcadeCharacter"))
		{
			//m_pkRender->Line(m_pkPlayerEntity->GetWorldPosV(),m_pkPlayerEntity->GetWorldPosV() + pkAC->m_kAim);
		
			if(pkAC->m_iTarget != -1)
			{
				if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(pkAC->m_iTarget))
				{
					glEnable(GL_ALPHA_TEST);
					glAlphaFunc(GL_GEQUAL, 0.1);
		
					m_pkRender->Quad (pkEnt->GetWorldPosV(), Vector3 (-90, 0, 0), Vector3(0.9, 0.9, 0.9), m_iMarkerTextureID, Vector3(0,255,0) );
		
					glDisable(GL_ALPHA_TEST);
				}
			}			
		
		//	float r = 1;
		//	m_pkRender->DrawAABB(pkAC->m_kAim-Vector3(r,r,r),pkAC->m_kAim+Vector3(r,r,r),Vector3(1,1,0));		
		}
	}
	
	//draw select square
	if(m_bSelectSquare)
		m_pkRender->DrawAABB(m_kSelectSquareStart,Vector3(m_kSelectSquareStop.x, m_kSelectSquareStart.y+0.2, m_kSelectSquareStop.z),Vector3(0,1,0));
}

void DarkMetropolis::OnSystem()
{	
/*

	float t = m_pkFps->m_pkObjectMan->GetGameTime();

	//if no hq has been found, try to find it
	if(m_iActiveHQ == -1)
	{
		m_iActiveHQ = FindActiveHQ();
		if(m_iActiveHQ != -1)
		cout<<"Found Active HQ :"<<m_iActiveHQ<<endl;
	}

	//do line test
	if(Entity* pkEnt = GetTargetObject())
		m_iCurrentPickedEntity = pkEnt->GetEntityID();
	else
		m_iCurrentPickedEntity = -1;
		
		
	//make sure all selected characters are valid
	ValidateSelection();
	ValidateAgentsOnField();

	static float s_fUpdateGUICheckTime;
	if(m_pkGamePlayDlg != NULL && m_pkGamePlayDlg->IsInitialized())
	{
		if(t - s_fUpdateGUICheckTime > 0.25f)
		{
			s_fUpdateGUICheckTime = t;
			((CGamePlayDlg*)m_pkGamePlayDlg)->UpdateAgentList();
		}
	}
*/
}

void DarkMetropolis::OnServerClientJoin(ZFClient* pkClient,int iConID, 
													 char* szLogin, char* szPass)
{
}

void DarkMetropolis::OnServerClientPart(ZFClient* pkClient,int iConID)
{
}

void DarkMetropolis::OnServerStart()
{
	m_pkCameraEntity = m_pkObjectMan->CreateObject();

	if(m_pkCameraEntity)
	{
		m_pkCameraEntity->AddProperty("P_Camera");	
	
		m_pkCameraEntity->SetParent( m_pkObjectMan->GetWorldObject() );
		m_pkCameraEntity->SetWorldPosV(Vector3(0,0,0));
		
		m_pkCameraProp = (P_Camera*)m_pkCameraEntity->GetProperty("P_Camera");		
		if(m_pkCameraProp)
		{
			m_pkCameraProp->SetCamera(m_pkCamera);
			m_pkCameraProp->SetType(CAM_TYPEBIRDSEYE);
			m_pkCameraProp->Set3PPAngle(1.30);
			
			m_pkCameraProp->Set3PYAngle(m_fAngle);
			m_pkCameraProp->Set3PDistance(m_fDistance);							
			
			m_pkCameraEntity->GetSave() = false;
		}
		
		
		//add enviroment to camera
		if(P_Enviroment* pkEnv = (P_Enviroment*)m_pkCameraEntity->AddProperty("P_Enviroment"))
		{
			pkEnv->SetEnable(true);
			pkEnv->SetEnviroment("data/enviroments/dm.env");			
		}

				
		//add tracker to camera
		m_pkCameraEntity->AddProperty("P_Track");
	}

	
	m_iPlayerEntityID =	-1;
	m_pkPlayerEntity =	NULL;
	
//	for(int i = 0 ;i<10;i++)
//		m_pkObjectMan->UpdateZones();		
	
		
	if(!CreatePlayer())
	{
		cout<<"ERROR: PLAYER NOT CREATED"<<endl;
		exit(1);	
	}
	else
	{
		m_pkCameraEntity->SetRelativeOri(true);
		m_pkCameraEntity->SetLocalPosV(Vector3(0,0,0));
		m_pkCameraEntity->SetParent(m_pkPlayerEntity);
		
	}
	
	/*
	m_kSelectedEntitys.clear();
	m_bSelectSquare = 		false;1
	m_iCurrentFormation =	FORMATION_CIRCLE;
	m_bActionPressed =		false;
	m_iHQID =					-1;
	m_iActiveHQ = 				-1;
	*
	
	UpdateAgentsOnField();
	*/
}

void DarkMetropolis::OnClientStart()
{
}

bool DarkMetropolis::StartUp()
{
	return true;
}

bool DarkMetropolis::ShutDown()
{
	//SaveGame();
	return true;
}

bool DarkMetropolis::IsValid()
{
	return true;
}

void DarkMetropolis::RegisterPropertys()
{
	m_pkPropertyFactory->Register("P_ArcadeCharacter",	Create_P_ArcadeCharacter);
	m_pkPropertyFactory->Register("P_Car",					Create_P_Car);

	m_pkPropertyFactory->Register("P_DMMission",			Create_P_DMMission);
	m_pkPropertyFactory->Register("P_DMItem", 			Create_P_DMItem);
	m_pkPropertyFactory->Register("P_DMGun", 				Create_P_DMGun);
	m_pkPropertyFactory->Register("P_Event",				Create_P_Event);
	m_pkPropertyFactory->Register("P_Enviroment",		Create_P_Enviroment);
	m_pkPropertyFactory->Register("P_DMShop",				Create_P_DMShop);	
	m_pkPropertyFactory->Register("P_DMHQ", 				Create_P_DMHQ);
	m_pkPropertyFactory->Register("P_DMGameInfo",		Create_P_DMGameInfo);	
	m_pkPropertyFactory->Register("P_DMCharacter",		Create_P_DMCharacter);
	m_pkPropertyFactory->Register("P_ShadowBlob",		Create_P_ShadowBlob);	
	m_pkPropertyFactory->Register("P_DMClickMe",			Create_P_DMClickMe);	
	m_pkPropertyFactory->Register("P_Spawn",				Create_P_Spawn);	
}

void DarkMetropolis::MoveCamera(Vector3 pos)
{
	pos.y = m_pkCameraEntity->GetWorldPosV().y ;
	m_pkCameraEntity->SetWorldPosV(pos);
}

void DarkMetropolis::Input()
{
	//get mouse
	int x,z;		
	m_pkInputHandle->RelMouseXY(x,z);	


	//check for camera movment
	float fMx,fMy;
	if(!m_pkInputHandle->VKIsDown("camera"))
		m_pkInputHandle->UnitMouseXY(fMx,fMy);
	else
	{
		fMx = 0;
		fMy = 0;
	}

	if(m_pkCameraEntity)
	{
		Vector3 pos = m_pkCameraEntity->GetWorldPosV();

		float s = m_pkFps->GetFrameTime() * 10;

		if(m_pkInputHandle->VKIsDown("cam_up") || (fMy == -0.5) )
			pos += Vector3(-1 * sin(m_fAngle), 0, -1 * cos(m_fAngle)) * s;
		
		if(m_pkInputHandle->VKIsDown("cam_down") || (fMy ==  0.5) )
			pos += Vector3(1 * sin(m_fAngle), 0, 1 * cos(m_fAngle)) * s;
		
		if(m_pkInputHandle->VKIsDown("cam_left") || (fMx == -0.5) )
			pos += Vector3(-1 * cos((PI*2)-m_fAngle), 0, -1 * sin((PI*2)-m_fAngle)) * s;
		
		if(m_pkInputHandle->VKIsDown("cam_right") || (fMx ==  0.5) )
			pos += Vector3(1 * cos((PI*2)-m_fAngle), 0, 1 * sin((PI*2)-m_fAngle)) * s;
		
	//	m_pkCameraEntity->SetWorldPosV(pos);
	}


	//setup player controls
	if(m_pkInputHandle->VKIsDown("camera"))	//do we want to zoom?
	{
		m_pkInput->ShowCursor(false);

		if(m_pkCameraProp)
		{
			m_fDistance += z/60.f;
			m_fAngle -=x/300.f;
	
			if(m_fDistance < m_fMinCamDistance)
				m_fDistance = m_fMinCamDistance;
		
			if(m_fDistance > m_fMaxCamDistance)
				m_fDistance = m_fMaxCamDistance;				
		
			m_pkCameraProp->Set3PYAngle(m_fAngle);
			//m_pkCameraProp->Set3PDistance(m_fDistance);				
		}
	}
	else
	{
		if(m_pkGui->m_bUseHardwareMouse == true)
			m_pkInput->ShowCursor(true);
	}
		

	if(m_pkPlayerEntity)
	{	
		if(P_ArcadeCharacter* pkChar = (P_ArcadeCharacter*)m_pkPlayerEntity->GetProperty("P_ArcadeCharacter"))
		{
			pkChar->m_kActions.reset();
					
			if(m_pkInputHandle->VKIsDown("cam_up"))
				pkChar->m_kActions[0] = true;;
			if(m_pkInputHandle->VKIsDown("cam_down"))
				pkChar->m_kActions[1] = true;;
			if(m_pkInputHandle->VKIsDown("cam_left"))
				pkChar->m_kActions[2] = true;;
			if(m_pkInputHandle->VKIsDown("cam_right"))
				pkChar->m_kActions[3] = true;;

			if(m_pkInputHandle->VKIsDown("select"))
				pkChar->m_kActions[4] = true;;
				
						
			float x;
			float y;
			m_pkInputHandle->UnitMouseXY(x,y);	
			pkChar->m_kDir.Set(x,0,y);
		}
		
		if(m_pkInputHandle->Pressed(KEY_H))
			m_pkPlayerEntity->SetWorldPosV(Vector3(0,2,0));
				
		if(m_pkInputHandle->VKIsDown("action"))
		{
			if( (m_pkFps->GetTicks() - m_fDelayTimer) > 0.15)
			{
				m_fDelayTimer = m_pkFps->GetTicks();
								
				Entity* pkEnt = m_pkObjectMan->CreateObjectFromScriptInZone("data/script/objects/t_particleball.lua",m_pkPlayerEntity->GetWorldPosV() );											
				if(P_Tcs* pkTcs = (P_Tcs*)pkEnt->GetProperty("P_Tcs"))
				{
					Vector3 kDir = m_pkPlayerEntity->GetWorldRotM().VectorTransform(Vector3(0,0,1));					
					pkTcs->ApplyImpulsForce(kDir*10);				
				} 								
			}
		}
	}
}

void DarkMetropolis::RunCommand(int cmdid, const CmdArgument* kCommand)
{

	switch(cmdid) {

		case FID_LOAD:
		{		
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				m_pkConsole->Printf("load [savegame]");
				break;				
			}
			
			if(!LoadGame(kCommand->m_kSplitCommand[1]))
			{
				cout<<"Error loading game"<<endl;
				break;
			}				

			
			break;		
		}

			
		case FID_SAVE:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				m_pkConsole->Printf("save [savegame]");
				break;				
			}
			
			if(!SaveGame(kCommand->m_kSplitCommand[1]))
			{
				m_pkConsole->Printf("Error saving game");
				break;
			}	
			
			break;
	}
}

bool DarkMetropolis::StartNewGame(string strClanName,string strClanColor)
{
	//load world
	if(!m_pkObjectMan->LoadWorld("dmworld"))
	{
		cout<<"ERROR: default world dmworld not found"<<endl;
		return false;
	
	}
	
	//start server
	GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);			

	//create a new gameinfo entity
	m_pkGameInfoEntity = m_pkObjectMan->CreateObject();
	m_pkGameInfoEntity->SetParent(m_pkObjectMan->GetGlobalObject());
	m_pkGameInfoProperty = (P_DMGameInfo*)m_pkGameInfoEntity->AddProperty("P_DMGameInfo");
	m_pkGameInfoEntity->AddProperty("P_DMMission"); // Lägg till ett mission property oxå...

	//setup startup gamesettings
	m_pkGameInfoProperty->m_strClanName = strClanName;
	m_pkGameInfoProperty->m_strClanColor = strClanColor;

	return true;
}


bool DarkMetropolis::LoadGame(string strClanName)
{
	if(!m_pkObjectMan->LoadWorld(m_strSaveDirectory+strClanName))
	{
		cout<<"could not load world from savegame"<<endl;
		return false;	
	}

	//create a new gameinfo entity
	m_pkGameInfoEntity = m_pkObjectMan->CreateObject();
	m_pkGameInfoEntity->SetParent(m_pkObjectMan->GetGlobalObject());
	
	
	//load gameinfo
	ZFVFile blub;
	if(!blub.Open(m_strSaveDirectory+strClanName+"/gameinfo.dat",0,false))
	{
		cout<<"ERROR: could not load gameinfo"<<endl;
		return false;
	}

	m_pkGameInfoEntity->Load( &blub,false,false);
	blub.Close();		
	
	m_pkGameInfoProperty = (P_DMGameInfo*)m_pkGameInfoEntity->GetProperty("P_DMGameInfo");
	
	if(!m_pkGameInfoProperty)
	{
		cout<<"ERROR: could not find any gameinfo"<<endl;
		return false;
	}

	
	cout<<"CLAN NAME:"<<m_pkGameInfoProperty->m_strClanName<<endl;
	
	//start server
	GetSystem().RunCommand("server Default server",CSYS_SRC_SUBSYS);			
	
	//set camera position
	if(m_pkCameraEntity)
	{
		cout<<"seting cam pos"<<endl;
		m_pkCameraEntity->SetWorldPosV(m_pkGameInfoProperty->m_kCameraPos);
	}
		
	return true;
}


bool DarkMetropolis::SaveGame(string strsavegame)
{
	m_pkBasicFS->CreateDir(m_strSaveDirectory.c_str());

	//first save gameinfo
	
	
	
	if(m_pkGameInfoEntity)
	{
		if(m_pkCameraEntity)
			m_pkGameInfoProperty->m_kCameraPos  = m_pkCameraEntity->GetWorldPosV();
	
	
		ZFVFile blub;
		if(!blub.Open(m_strSaveDirectory+strsavegame+"/gameinfo.dat",0,true))
		{
			cout<<"ERROR: could not save gameinfo"<<endl;
			return false;
		}
	
		m_pkGameInfoEntity->Save( &blub);
		blub.Close();	
	}
	
	if(!m_pkObjectMan->SaveWorld(m_strSaveDirectory+strsavegame,true))
	{
		cout<<"ERROR: could not save world"<<endl;
		return false;
	}

	return true;

}

Vector3 DarkMetropolis::Get3DMousePos(bool m_bMouse=true)
{
	Vector3 dir;
	
	//screen propotions
	float xp=4;
	float yp=3;
	float fovshit=-2.15;
	
	if(m_bMouse)
	{
		float x;
		float y;
		
		m_pkInputHandle->UnitMouseXY(x,y);
		
		dir.Set(x*xp,-y*yp,fovshit);
		dir.Normalize();
	}
	else
	{
		dir.Set(0,0,fovshit);
		dir.Normalize();
	}	
	
	Matrix4 rm = m_pkCamera->GetRotM();
	rm.Transponse();
	dir = rm.VectorTransform(dir);
	return dir;
}

Entity* DarkMetropolis::GetTargetObject(bool bZonesOnly)
{
	Vector3 start = m_pkCamera->GetPos();
	Vector3 dir;

	dir = Get3DMousePos(true);		

	vector<Entity*> kObjects;
	kObjects.clear();
	
	m_pkObjectMan->GetZoneObject()->GetAllEntitys(&kObjects);
	
	float closest = 999999999;
	float d;
	Vector3 cp;
	Entity* pkClosest = NULL;	
	for(unsigned int i=0;i<kObjects.size();i++)
	{
		
		//objects that should not be clicked on (special cases)
		if(kObjects[i]->GetEntityID() <100000)
			continue;
		
		if(bZonesOnly)  //zones only, skip if its not a zone
			if(!kObjects[i]->IsZone())
				continue;
		
		//-------------
		
		//get mad property and do a linetest
		P_Mad* mp = (P_Mad*)kObjects[i]->GetProperty("P_Mad");
		if(mp)
		{
			if(mp->TestLine(start,dir))
			{	
				cp = mp->GetLastColPos();
				d = start.DistanceTo(cp);
	
				if(d < closest)
				{
					closest = d;
					pkClosest = kObjects[i];
					m_kPickPos = cp;
				}				
			}
		}				
	}
	
	return pkClosest;
}

void DarkMetropolis::StartSong(char* szName)
{
	static string prev_song = "";

	if(m_iEnableMusic == 1 && prev_song != szName)
	{
		OggMusic* pkMusic = static_cast<OggMusic*>(
			g_ZFObjSys.GetObjectPtr("OggMusic")); 

		pkMusic->SetVolume(m_fMusicVolume); 
		pkMusic->LoadFile(szName);
		pkMusic->Play();

		prev_song = szName;
	}
}


Vector3 DarkMetropolis::GetFormationPos(int iType,int iTotal,int iPos)
{
	if(iType == FORMATION_CIRCLE)
	{
		float g = float((360 / iTotal) * iPos);
	
		return Vector3(cos(DegToRad(g)),0,sin(DegToRad(g)))*1;	
	}
	
	if(iType == FORMATION_SQUARE)
	{
		int iSide = int(round2(sqrt(float(iTotal))));
	
		return Vector3(float(iPos%iSide),0,float(int(iPos/iSide)));
	
	}

	return Vector3(0,0,0);
}

void DarkMetropolis::PauseGame(bool bPause)
{
	if(bPause)
	{
//		GetSystem().RunCommand("set e_runsim 0",CSYS_SRC_SUBSYS);
//		GetSystem().RunCommand("set e_simspeed 0.0",CSYS_SRC_SUBSYS);
		m_eGameMode = PAUSED;
	}
	else
	{
//		GetSystem().RunCommand("set e_runsim 1",CSYS_SRC_SUBSYS);
//		GetSystem().RunCommand("set e_simspeed 1.0",CSYS_SRC_SUBSYS);
		m_eGameMode = ACTIVE;
	}
}

int DarkMetropolis::FindActiveHQ()
{
	vector<Entity*> kObjects;	
	m_pkObjectMan->GetZoneObject()->GetAllEntitys(&kObjects,false);

	for(unsigned int i = 0;i<kObjects.size();i++)
	{
		if(P_DMHQ* pkHQ = (P_DMHQ*)kObjects[i]->GetProperty("P_DMHQ"))
		{
			if(pkHQ->GetActive())
				return kObjects[i]->GetEntityID();		
		}
	}
	
	return -1;
}



//
// Har markerat en agent.
//
void DarkMetropolis::SelectAgent(int id, bool bToggleSelect, bool bResetFirst, 
											bool bMoveCamera) 
{ 
	if(bResetFirst)
		m_kSelectedEntitys.clear(); 
 
	if(id == -1)
		return;

	if(bToggleSelect=false)
		m_kSelectedEntitys.push_back(id); 
	else
	{
		bool bFound=false;
		vector<int>::iterator it = m_kSelectedEntitys.begin();
		for( ; it!=m_kSelectedEntitys.end(); it++)
		{
			if((*it) == id)
			{
				m_kSelectedEntitys.erase(it); 
				bFound = true;
				break;
			}
		}

		if(bFound == false)
		{
			m_kSelectedEntitys.push_back(id); 
		}
	}

	Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(id);

	if(pkEnt)
	{
		P_DMCharacter* pkCharProp = 
			(P_DMCharacter*)pkEnt->GetProperty("P_DMCharacter");
		
		if(pkCharProp && pkCharProp->GetStats()->m_iLife > 0)
		{
			int iNumSelSounds = pkCharProp->m_vkSelectSounds.size();
			if(iNumSelSounds > 0)
			{
				m_pkAudioSys->StartSound(
					pkCharProp->m_vkSelectSounds[rand()%iNumSelSounds], 
					pkEnt->GetIWorldPosV());
			}
		}
	}

	

	//
	// TODO: Flytta kameran till samma plats.
	//

	if(bMoveCamera)
	{

	}
}

void DarkMetropolis::ValidateSelection()
{
	bool update_list = false;

	for(int i = 0;i < m_kSelectedEntitys.size();i++)
	{
		if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_kSelectedEntitys[i]))
		{
			if(!pkEnt->GetParent()->IsZone())		
			{
				cout<<"avmarkerad"<<endl;
				SelectAgent(m_kSelectedEntitys[i], true, false,false);

				// Uppdatera GUI:t och berätta att ingen agent är i fokus.
				if(m_pkGamePlayDlg)
					((CGamePlayDlg*)m_pkGamePlayDlg)->SelectAgentGUI(-1, false);

				i = 0;

				update_list = true;
			}
		}
		else
		{
			SelectAgent(m_kSelectedEntitys[i], true, false,false);
		}
	}

	if(m_pkGamePlayDlg && update_list == true)
		((CGamePlayDlg*)m_pkGamePlayDlg)->UpdateAgentList();
}

void DarkMetropolis::ValidateAgentsOnField()
{
	for( vector<int>::iterator it = m_kAgentsOnField.begin(); it!=m_kAgentsOnField.end(); it++)
	{
		if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(*it))
		{
			//character was found but not in a zone, remove it
			if(!pkEnt->GetParent()->IsZone())
			{		
				m_kAgentsOnField.erase(it); 
				it = m_kAgentsOnField.begin();
			}
			else
			{
				if(P_DMCharacter* pkCh = (P_DMCharacter*)pkEnt->GetProperty("P_DMCharacter"))
				{	
					//agent is in a zone but has wrong team
					if(pkCh->m_iTeam != 0)
					{			
						m_kAgentsOnField.erase(it); 
						it = m_kAgentsOnField.begin();						
					}
				}
			}
		}
		//character was not found att all, remove it
		else		
		{			
			if(m_kAgentsOnField.empty())
				break;

			m_kAgentsOnField.erase(it); 

			if(m_kAgentsOnField.empty())
				break;

			it = m_kAgentsOnField.begin();
		}
	}
}

void DarkMetropolis::UpdateAgentsOnField()
{
	m_kAgentsOnField.clear();

	vector<Entity*> m_kEntitys;
	m_pkObjectMan->GetZoneObject()->GetAllEntitys(&m_kEntitys);
	
	for(int i = 0;i<m_kEntitys.size();i++)
	{
		if(P_DMCharacter* pkCh = (P_DMCharacter*)m_kEntitys[i]->GetProperty("P_DMCharacter"))
		{	
			if(pkCh->m_iTeam == 0)
			{
				if(pkCh->GetStats()->m_iLife > 0)
					m_kAgentsOnField.push_back(m_kEntitys[i]->GetEntityID());
			}
		}
	}
}


void DarkMetropolis::CheckCameraPos()
{
	if(!m_pkCameraEntity)
		return;

		
	if(m_pkPlayerEntity)
	{
		Vector3 kCamPos = m_pkCameraEntity->GetIWorldPosV();
		Vector3 kPlayerPos = m_pkPlayerEntity->GetIWorldPosV();
		
		Vector3 kNewPos = (kCamPos + kPlayerPos) / 2;
		
	//	m_pkCameraEntity->SetWorldPosV(kNewPos);
	}
		
	//first check wich object is closest to the camera
/*

	float dist = 999999999;
	Vector3 kCamPos = m_pkCameraEntity->GetWorldPosV();
	kCamPos.y = 0;
	Entity* pkClosestEnt = NULL;
	
	Vector3 kPos;
	float d = 0;

	if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_iActiveHQ))
	{
		kPos = pkEnt->GetWorldPosV();
		kPos.y = 0;
		dist = kCamPos.DistanceTo(kPos);	
		pkClosestEnt = pkEnt;
	}
	
	for(int i = 0;i < m_kAgentsOnField.size(); i++)
	{
		if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_kAgentsOnField[i]))
		{
			kPos = pkEnt->GetWorldPosV();
			kPos.y = 0;			
			d = kCamPos.DistanceTo(kPos);
			if( d < dist )
			{
				dist = d;
				pkClosestEnt = pkEnt;				
			}
		}
	}


	if(!pkClosestEnt)
		return;
		
	
	if(dist > m_fCameraMaxDistanceFromAgent)
	{
		//cout<<"camera to far away"<<endl;		
		Vector3 kTemp = pkClosestEnt->GetIWorldPosV();
		kTemp.y = 0;
		Vector3 kDir = (kCamPos - kTemp).Unit() * m_fCameraMaxDistanceFromAgent;
		Vector3 kNewPos = kTemp + kDir;
		kNewPos.y = m_pkCameraEntity->GetWorldPosV().y;
		
//		kNewCamPos += kDir * m_pkObjectMan->GetSimDelta() * (dist * 3) ;
//		kNewCamPos += kDir * m_pkFps->GetFrameTime() * dist  ;
		//kNewCamPos += kDir;
		m_pkCameraEntity->SetWorldPosV(kNewPos);
	}
	*/
}

bool DarkMetropolis::CreatePlayer()
{
	vector<Entity*> m_kEntitys;
	m_pkObjectMan->GetZoneObject()->GetAllEntitys(&m_kEntitys);

	for(int i = 0;i<m_kEntitys.size();i++)
	{
		if(m_kEntitys[i]->GetType() == "t_playerstart.lua")
		{
/*			ZoneData* dat = m_pkObjectMan->GetZone(m_kEntitys[i]->GetParent());
			if(dat)
			{
				cout<<"ID:"<<dat->m_iZoneID<<endl;
			
				if(!dat->m_bActive)
					cout<<"CPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP"<<endl;
					
				if(m_kEntitys[i]->GetParent()->GetUpdateStatus() == UPDATE_ALL)
					cout<<"CSSSSSSSSSSNNNNNNNNNZZZZZZZZZZZZZZ!#¤"<<endl;
			}
*/		
		
			Vector3 kStartPos = m_kEntitys[i]->GetWorldPosV();		
			m_pkObjectMan->Delete(m_kEntitys[i]);
		
			m_pkPlayerEntity = m_pkObjectMan->CreateObjectFromScriptInZone("data/script/objects/t_arcadeplayer.lua",kStartPos);			
			m_iPlayerEntityID = m_pkPlayerEntity->GetEntityID();

			return true;			
		}
	}
	
	return false;
}