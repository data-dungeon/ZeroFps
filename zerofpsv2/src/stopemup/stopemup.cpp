#ifndef _DONT_MAIN					// <- OBS! Flytta inte p�denna. M�te ligga i
	#define _MAINAPPLICATION_		// just denna fil och inte p�flera st�len.
	#define _DONT_MAIN
#endif

#include "stopemup.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/engine/inputhandle.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../zerofpsv2/engine_systems/propertys/p_track.h"
#include "../zerofpsv2/engine_systems/tcs/tcs.h"

#include "../mcommon/p_characterproperty.h"

#include "../mcommon/mainmcommon.h"
#include "../mcommon/p_enviroment.h"

#include "p_gun.h"
#include "walker.h"
#include "p_player.h"

StopEmUp g_kStopEmUp("StopEmUp",0,0,0);
bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
	return true;
}

StopEmUp::StopEmUp(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 

	Register_Cmd("newgame",			FID_NEWGAME);
	
	
	m_iPlayerID = 		-1;
	m_bServerMode = 	false;
	m_bFire = 			false;	
	m_iLevel = 			0;
	m_fLevelWait = 	5.0;
	
	m_iEnergy			=100;
	m_iMaxEnergy		=100;
	m_iScore				=0;
	m_iCurrentLevel	=1;	
}

void StopEmUp::OnInit()
{
	//m_pkGui->Activate(false);
	
	//intit gui
	GuiAppLua::Init(NULL, m_pkScript);
	InitGui(m_pkScript, "defguifont", "data/script/gui/defskins.lua", 
				NULL, true, DISABLE_SCALE);
	
	//add vfs root
	m_pkZFVFileSystem->AddRootPath( string("../datafiles/mistlands") ,"/data");
	m_pkZFVFileSystem->AddRootPath( string("../datafiles/stopemup") ,"/data");
	//m_pkZFVFileSystem->AddRootPath( string("../datafiles/dm") ,"/data");

	//register mcommon
	MCommon_RegisterPropertys( m_pkZeroFps, m_pkPropertyFactory );
	m_pkPropertyFactory->Register("P_Gun",	Create_P_Gun);
	m_pkPropertyFactory->Register("P_Walker",	Create_P_Walker);
	m_pkPropertyFactory->Register("P_Player",	Create_P_Player);
	
	//create camera
	m_pkCamera = new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.25,250);	
	
	//set title
	SetTitle("StopEmUp (C) Richard Svensson");

	m_pkZeroFps->SetSystemFps(20);
	
	//setup physics
	m_pkTcs->SetColIt(2);
	m_pkTcs->SetConIt(2);
	
	CreateCamera();
}

void StopEmUp::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid)
	{
		case FID_NEWGAME:
		{
			cout<<"starting new stopemup server"<<endl;			
		
			m_pkZeroFps->StartServer(false,true,4242,"StopEmUp");
			break;
		}			
	
	}

}

void StopEmUp::OnSystem(void)
{
//	cout<<"rand 0:"<<Randomi(0)<<endl;
//	cout<<"rand 2:"<<Randomi(2)<<endl;
//	cout<<"rand 3:"<<Randomi(3)<<endl;
//	cout<<"rand 1:"<<Randomi)<<endl;


	if(m_bServerMode)
	{
		//level stuff
		static bool bWait=false;
		static float fTime;
		
		//level complete?
		if(LevelComplete() && !bWait)
		{
			bWait = true;	
			fTime = m_pkZeroFps->GetTicks();
		}
			
		//waiting for next level
		if(bWait)
			if(m_pkZeroFps->GetTicks() > fTime + m_fLevelWait)
			{
				m_iLevel++;
				SetupLevel(m_iLevel);	
				bWait = false;			
			}
			
		
	}
	else
	{

		SendControlInfo();
	
	
		//find camera
		if(m_iPlayerID != -1)
		{
			if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iPlayerID))
			{
				if(!pkEnt->GetProperty("P_Camera"))
				{
					if(P_Camera* pkCam = (P_Camera*)pkEnt->AddProperty("P_Camera"))
					{
						pkCam->SetCamera(m_pkCamera);
						pkCam->SetType(CAM_TYPE3PERSON);
						pkCam->Set3PPAngle(50);					
						pkCam->Set3PYAngle(0);
						pkCam->Set3PDistance(4);							
						pkCam->SetOffset(Vector3(0,1,0));
					}
				}
				
				//get stats
				if(P_Player* pkPlayer = (P_Player*)pkEnt->GetProperty("P_Player"))
				{
					
					m_iEnergy = 	pkPlayer->m_iEnergy;
					m_iMaxEnergy = pkPlayer->m_iMaxEnergy;
					m_iScore = 		pkPlayer->m_iScore;
				}					
			}	
		}
	}
}

void StopEmUp::OnIdle()
{
	Input();

	
	//setup devpage
	if(!m_bServerMode)
	{
		char temp[3];
		temp[0] = '#';
		temp[1] = int(( float(m_iEnergy) / float(m_iMaxEnergy) ) *100.0);
		temp[2] = '/0';		
	
	
		m_pkZeroFps->DevPrintf("StopEmUp-Client", "LEVEL: %d",m_iCurrentLevel);
		m_pkZeroFps->DevPrintf("StopEmUp-Client", "SCORE: %d",m_iScore);
		m_pkZeroFps->DevPrintf("StopEmUp-Client", "ENERGY:  %d /  %d",m_iEnergy,m_iMaxEnergy);
		m_pkZeroFps->DevPrintf("StopEmUp-Client", temp);
		m_pkZeroFps->DevPrintf("StopEmUp-Client", "GUN: YberGun");
		
	}
}

void StopEmUp::Input()
{
	if(!m_bServerMode)
	{
			//update camera
		if(Entity* pkCharacter = m_pkEntityManager->GetEntityByID(m_iPlayerID))
		{
			if(P_Camera* pkCam = (P_Camera*)pkCharacter->GetProperty("P_Camera"))
			{	
				
				m_kCharacterControls[eUP] = 	m_pkInputHandle->Pressed(KEY_UP);
				m_kCharacterControls[eDOWN] =	m_pkInputHandle->Pressed(KEY_DOWN);			
				m_bFire							=	m_pkInputHandle->Pressed(KEY_LCTRL);			
	
				float fYAngle = pkCam->Get3PYAngle();
				
				//reset straf keys
				m_kCharacterControls[eLEFT] = false;
				m_kCharacterControls[eRIGHT] = false;
				
				//want to straf or turn?
				if(m_pkInputHandle->Pressed(KEY_LALT))
				{
					m_kCharacterControls[eLEFT] = m_pkInputHandle->Pressed(KEY_LEFT);				
					m_kCharacterControls[eRIGHT] = m_pkInputHandle->Pressed(KEY_RIGHT);	
				}
				else
				{
					if(m_pkInputHandle->Pressed(KEY_LEFT))
						fYAngle += m_pkZeroFps->GetFrameTime()*100;
					
					if(m_pkInputHandle->Pressed(KEY_RIGHT))
						fYAngle -= m_pkZeroFps->GetFrameTime()*100;
				}
								
				pkCam->Set3PYAngle( fYAngle);
				
			}	
		}
	}
}

void StopEmUp::SendControlInfo()
{
	if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iPlayerID))
	{
		if(P_Camera* pkCam = (P_Camera*)pkEnt->GetProperty("P_Camera"))
		{
			//request character entityID
			NetPacket kNp;	
			kNp.Clear();
			kNp.Write((char) eCS_CONTROLS);
			
			kNp.Write(m_kCharacterControls);
			kNp.Write(pkCam->Get3PYAngle() );
			kNp.Write(pkCam->Get3PPAngle() );
			kNp.Write(m_bFire);
			
			kNp.TargetSetClient(0);
			SendAppMessage(&kNp);		
		}
	}
}


void StopEmUp::OnServerStart()
{
	cout<<"StopEmUp server started"<<endl;

	m_pkEntityManager->LoadWorld("../datafiles/stopemup/map");	
	CreateCamera();
	
	m_bServerMode =	true;
	m_iLevel = 			0;
	
	SetTitle("StopEmUp - Server");
}

void StopEmUp::OnClientStart()
{
	m_iPlayerID = -1;
	m_bFire = false;

	cout<<"joing server"<<endl;
	CreateCamera();
	

	//show pevpage	
	m_pkZeroFps->SetDevPageVisible("StopEmUp-Client",true);


	//title
	SetTitle("StopEmUp - Client");
	
	m_bServerMode = false;
}

void StopEmUp::OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass, bool bIsEditor)
{
	int iEntityID = CreatePlayerEntity(iConID);

	if(iEntityID == -1)
	{
		cout<<"ERROR: could not create player entity"<<endl;
		return;
	}
		
	AddPlayer(iConID,iEntityID);	
	
	//send id to client
	NetPacket kNp;		
	kNp.Clear();	
	kNp.Write((char)eSC_PLAYERID);
	kNp.Write(iEntityID);
	kNp.TargetSetClient(iConID);
	m_pkApp->SendAppMessage(&kNp);	
	
	
	//send level data
	SendLevelData();
}

void StopEmUp::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	int iEntityID = GetPlayerEntity(iConID);	
	m_pkEntityManager->Delete(iEntityID);
	
	RemovePlayer(iConID);
	
	cout<<"deleted player entity:"<<iEntityID<<endl;
}

int StopEmUp::CreatePlayerEntity(int iConnID)
{
	Entity* pkPlayer = m_pkEntityManager->CreateEntityFromScriptInZone("data/script/objects/stopemupplayer.lua",Vector3(0,0,0) );
// 	Entity* pkPlayer = m_pkEntityManager->CreateEntityFromScript("data/script/objects/stopemupplayer.lua");
		
	if(!pkPlayer)
	{
		cout<<"ERROR COULD NOT CREATE PLAYER"<<endl;
		return -1;
	}

// 	pkPlayer->SetUseZones(true);
// 	pkPlayer->SetWorldPosV(Vector3(0,0,0));
	
	if(P_Track* pkTrack = (P_Track*)pkPlayer->GetProperty("P_Track"))
	{
		pkTrack->SetClient(iConnID);
	}
	
	return pkPlayer->GetEntityID();
}

void StopEmUp::CreateCamera()
{
	m_pkCameraEntity = m_pkEntityManager->CreateEntityFromScript("data/script/objects/cameraedit.lua");	
	if(m_pkCameraEntity) 
	{
		m_pkCameraEntity->SetWorldPosV(Vector3(0,-0.5,-20));
	
		m_pkCameraEntity->SetParent( m_pkEntityManager->GetWorldEntity() );
		m_pkCameraEntity->SetSave(false);
		
		P_Camera* m_pkCamProp = (P_Camera*)m_pkCameraEntity->GetProperty("P_Camera");
		m_pkCamProp->SetCamera(m_pkCamera);
	}

	m_pkCamera->SetSelected(true);

	m_pkZeroFps->AddRenderCamera(m_pkCamera);
	m_pkCamera->m_bForceFullScreen = true;
	
	
	//create enviroment
	if(P_Enviroment* pkEnv = (P_Enviroment*)m_pkCameraEntity->AddProperty("P_Enviroment"))
	{
		pkEnv->LoadEnviroment("data/enviroments/stopemup.env");
		pkEnv->SetEnable(true);
	}
	
}

void StopEmUp::AddPlayer(int iID,int iEntity)
{
	if(GetPlayerEntity(iID) == -1)
	{
		m_kPlayers.push_back( pair<int,int>(iID,iEntity) );
	}
}

void StopEmUp::RemovePlayer(int iID)
{
	for(vector<pair<int,int> >::iterator it = m_kPlayers.begin();it != m_kPlayers.end();it++)
	{
		if((*it).first == iID)
		{
			m_kPlayers.erase(it);
			return;
		}
	}
}


int StopEmUp::GetPlayerEntity(int iID)
{
	for(int i = 0;i < m_kPlayers.size();i++)
	{	
		if(m_kPlayers[i].first == iID)
			return m_kPlayers[i].second;
	}
	
	return -1;
}


void StopEmUp::OnNetworkMessage(NetPacket *pkNetMessage)
{
	char cType;
	
	pkNetMessage->Read(cType);

	switch(cType)
	{
		case eSC_PLAYERID:
		{
			pkNetMessage->Read(m_iPlayerID);					
			cout<<"Get my player entity id from the server"<<endl;
			
			
			break;
		}				
		
		case eSC_LEVELDATA:
		{
			pkNetMessage->Read(m_iCurrentLevel);					
			
			break;
		}				
		
		case eCS_CONTROLS:
		{
			int iEntityID = GetPlayerEntity(pkNetMessage->m_iClientID);
		
			bitset<6> kControls;
			float	fYAngle;
			float fPAngle;
			bool bFire;
							
			pkNetMessage->Read(kControls);
			pkNetMessage->Read(fYAngle);
			pkNetMessage->Read(fPAngle);
			pkNetMessage->Read(bFire);
				
			if(Entity* pkCharacter = m_pkEntityManager->GetEntityByID(iEntityID))
			{
				if(P_CharacterControl* pkCC = (P_CharacterControl*)pkCharacter->GetProperty("P_CharacterControl"))
				{
					pkCC->SetKeys(&kControls);
					pkCC->SetRotation(fYAngle,fPAngle);
				}
				else
					cout<<"WARNING:player missing p_charactercontrol"<<endl;
					
				if(P_Gun* pkGun = (P_Gun*)pkCharacter->GetProperty("P_Gun"))
					pkGun->m_bFire = bFire;
				else
					cout<<"WARNING:player missing P_GUN"<<endl;
					
			}

		
			break;
		}
	}	
}

void StopEmUp::SetupLevel(int iLevel)
{
	cout<<"Seting up lvl: "<<iLevel<<endl;
	
	m_kSpawners.clear();
	
	
	int iSpawners = int(iLevel / 2.0);
	iSpawners += 1;
	
	cout<<"spawners:"<<iSpawners<<endl;
	
	for(int i = 0;i<iSpawners;i++)
	{
		//Vector3 kRandomPos = Vector3(9999999,999999,999999);
	
		float r = Randomf(360);
		r = DegToRad(r);
		Vector3 kRandomPos;
		kRandomPos.x = sin(r)*8;
		kRandomPos.z = cos(r)*8;
		kRandomPos.y = 0;
		
		//randomize 
		//while(m_pkEntityManager->GetZoneIndex(kRandomPos,-1,false) == -1)
		//	kRandomPos = Vector3(Randomf(32)-16.0,0,Randomf(32)-16.0);		
					
		Entity* pkEnt = m_pkEntityManager->CreateEntityFromScriptInZone("data/script/objects/spawner.lua",kRandomPos)		;
		
		m_kSpawners.push_back(pkEnt->GetEntityID());
	}
	
	//update level to client
	SendLevelData();
}

bool StopEmUp::LevelComplete()
{
	if(m_pkEntityManager->GetEntityByType("walker.lua"))
		return false;

	if(m_kSpawners.empty())
		return true;
		
	//check if theres any spawners left
	for(int i = 0;i<m_kSpawners.size();i++)
	{
		if(m_pkEntityManager->GetEntityByID(m_kSpawners[i]))
			return false;
		
	}
	
	return true;
}

void StopEmUp::SendLevelData()
{
	NetPacket kNp;		
	kNp.Clear();	
	kNp.Write((char)eSC_LEVELDATA);
	kNp.Write(m_iLevel);
	kNp.TargetSetClient(-2);
	m_pkApp->SendAppMessage(&kNp);			
}





