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

DarkMetropolis g_kDM("DarkMetropolis",0,0,0);

DarkMetropolis::DarkMetropolis(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	g_ZFObjSys.Log_Create("DarkMetropolis");	

	RegisterVariable("r_enablemusic",&m_iEnableMusic,CSYS_INT);
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
	
	//initiate variables
	m_pkCameraProp	= 			NULL;	
	m_pkCameraEntity = 			NULL;
	m_pkFps->m_bClientMode =	true;
	m_fMinCamDistance =			2;
	m_fMaxCamDistance =			50;
	m_fDistance =					0;	
	m_fAngle =						0;
	m_strSaveDirectory =			"clans/";
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
	
	//register commands
	Register_Cmd("load",FID_LOAD);			
	Register_Cmd("save",FID_SAVE);	

	//setup system speed
	m_pkFps->SetSystemFps(30);
	m_pkObjectMan->m_fSimTimeScale = 0.33;
	
	//set tracker los
	m_pkObjectMan->SetTrackerLos(3);
	
	//enable light
	m_pkLight->SetLighting(true);
	
	//create camera
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.25,250);	
	m_pkFps->SetRenderTarget(m_pkCamera);
	m_pkCamera->SetRender(true);

//	LoadResourcesOnStartup();

	//init dm script interface (register script functions for gameplay)
	DMLua::Init(m_pkObjectMan,m_pkScript);

	//register propertys
	RegisterPropertys();

	//registering resources
	m_pkResourceDB->RegisterResource( string(".env"), Create__EnvSetting	);

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
	}

	m_pkFps->UpdateCamera(); 	

	GUI_OnIdle();

//	if(m_pkMembersDlg)
//		((CMembersDlg*)m_pkMembersDlg)->UpdateCamera();
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
	
		if(P_DMCharacter* pkChar = (P_DMCharacter*)pkEnt->GetProperty("P_DMCharacter"))
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
			float r = pkEnt->GetBoundingRadius();
			m_pkRender->DrawAABB(pkEnt->GetWorldPosV()-Vector3(r,r,r),pkEnt->GetWorldPosV()+Vector3(r,r,r),Vector3(1,1,0));
		}
	}
	
	//draw select square
	if(m_bSelectSquare)
		m_pkRender->DrawAABB(m_kSelectSquareStart,Vector3(m_kSelectSquareStop.x, m_kSelectSquareStart.y+0.2, m_kSelectSquareStop.z),Vector3(0,1,0));
}

void DarkMetropolis::OnSystem() 
{	
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
			m_pkCameraEntity->GetSave() = false;
		}
		
		
		//add enviroment to camera
		if(P_Enviroment* pkEnv = (P_Enviroment*)m_pkCameraEntity->AddProperty("P_Enviroment"))
		{
			pkEnv->SetEnable(true);
			pkEnv->SetEnviroment("data/enviroments/sun.env");
			
		}
	}
			
	//m_kAgentsOnField.clear();			
	m_kSelectedEntitys.clear();
	m_bSelectSquare = 		false;
	m_iCurrentFormation =	FORMATION_CIRCLE;
	m_bActionPressed =		false;
	m_iHQID =					-1;
	m_iActiveHQ = 				-1;
	
	UpdateAgentsOnField();
	
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
}

void DarkMetropolis::MoveCamera(Vector3 pos)
{
	m_pkCameraEntity->SetWorldPosV(pos);
}

void DarkMetropolis::Input()
{
	//get mouse
	int x,z;		
	m_pkInputHandle->RelMouseXY(x,z);	

	// Byt tillbaks till bakgrundsmusik om det har gått 10 sek efter det 
	// att ingen kula har avlossats.
	if(m_pkFps->m_pkObjectMan->GetGameTime() - m_fBulletTime > 10 &&
		m_fBulletTime != -1)
	{
		m_fBulletTime = -1;
		StartSong("data/music/dm ingame.ogg");	
	}
		
		
	//some debuging buttons
	if(m_pkInputHandle->Pressed(KEY_P))
		if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_iHQID))
			if(P_DMHQ* pkHQ = (P_DMHQ*)pkEnt->GetProperty("P_DMHQ"))
			{
				vector<int>	kChars;
				pkHQ->GetCharacters(&kChars);
				
				cout<<"There is "<<kChars.size()<< " characters in this hq"<<endl;
			}
	
	
	if(m_pkInputHandle->Pressed(KEY_N))
		if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_iHQID))
			if(P_DMHQ* pkHQ = (P_DMHQ*)pkEnt->GetProperty("P_DMHQ"))
			{
				if(m_pkFps->GetTicks()-m_fDelayTimer > 1)
				{
					m_fDelayTimer = m_pkFps->GetTicks();								
					pkHQ->SpawnNewCharacter(0);
					
					cout<<"spawning a new character"<<endl;
				}
			}
	

	if(m_pkInputHandle->Pressed(KEY_O))
	{
		if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_iHQID))
			if(P_DMHQ* pkHQ = (P_DMHQ*)pkEnt->GetProperty("P_DMHQ"))
			{
				pkHQ->EjectAllCharacters();
			}
	}
	
	if(m_pkInputHandle->Pressed(KEY_P))
		if(!m_kSelectedEntitys.empty())
			if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_kSelectedEntitys[0]))
				if(P_DMCharacter* pkHQ = (P_DMCharacter*)pkEnt->GetProperty("P_DMCharacter"))
				{
					pkHQ->GetStats()->Print();
					pkHQ->m_pkBackPack->Print();
				}

	if(m_pkInputHandle->Pressed(KEY_O))
		if(!m_kSelectedEntitys.empty())
			if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_kSelectedEntitys[0]))
				if(P_DMCharacter* pkHQ = (P_DMCharacter*)pkEnt->GetProperty("P_DMCharacter"))
					pkHQ->m_pkBackPack->DropAll();				
				

	//if(m_pkInputHandle->Pressed(KEY_H))
	//	m_pkHQDlg->OpenDlg(); // Open the HQ dialog

	if(m_pkInputHandle->Pressed(KEY_F))
	{
		cout<<"Agents on field"<<endl;
		for(int i = 0;i<m_kAgentsOnField.size();i++)
		{
			cout<<"agent:"<<m_kAgentsOnField[i]<<endl;
		}
	}

	//check for camera movment	
	if(m_pkCameraEntity)
	{
		Vector3 pos = m_pkCameraEntity->GetWorldPosV();

		float s = m_pkFps->GetFrameTime() * 10;
		
		if(m_pkInputHandle->VKIsDown("cam_up"))
			pos += Vector3(-1 * sin(m_fAngle), 0, -1 * cos(m_fAngle)) * s;
		
		if(m_pkInputHandle->VKIsDown("cam_down"))
			pos += Vector3(1 * sin(m_fAngle), 0, 1 * cos(m_fAngle)) * s;
		
		if(m_pkInputHandle->VKIsDown("cam_left"))
			pos += Vector3(-1 * cos((PI*2)-m_fAngle), 0, -1 * sin((PI*2)-m_fAngle)) * s;
		
		if(m_pkInputHandle->VKIsDown("cam_right"))
			pos += Vector3(1 * cos((PI*2)-m_fAngle), 0, 1 * sin((PI*2)-m_fAngle)) * s;
		
		m_pkCameraEntity->SetWorldPosV(pos);
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
			m_pkCameraProp->Set3PDistance(m_fDistance);				
		}
	}
	else
	{
		if(pkGui->m_bUseHardwareMouse == true)
			m_pkInput->ShowCursor(true);
	}
		
		
		
	//check for selection
	if(m_pkInputHandle->VKIsDown("select"))
	{
		if(!m_bSelectSquare)
		{
			Entity* pkEnt = GetTargetObject(true);
		
			if(pkEnt)
			{
				m_bSelectSquare = true;
				m_kSelectSquareStart = m_kPickPos;
				m_kSelectSquareStop = m_kPickPos;
			}	
		}
		else
		{
			Entity* pkEnt = GetTargetObject(true);
			
			if(pkEnt)
			{
				m_kSelectSquareStop  =  m_kPickPos;
			}
		}
	}
	else
	{
		if(m_bSelectSquare)
		{
			int iAgentInFocus = -1;

			m_bSelectSquare = false;
			if(!m_pkInputHandle->VKIsDown("multiselect"))			
				SelectAgent(-1,false,true,false);
			
			//reset hq selection
			m_iHQID = -1;	
			
			//is there a box? , else do a quick check
			if( (m_kSelectSquareStart-m_kSelectSquareStop).Length() < 0.1)
			{
				Entity* pkEnt = GetTargetObject();
				if(pkEnt)
					if(pkEnt->GetProperty("P_DMCharacter"))   //selected a character
					{
						// check if character is on your team
						if ( ((P_DMCharacter*)pkEnt->GetProperty("P_DMCharacter"))->m_iTeam == 0 )
						{
							SelectAgent(pkEnt->GetEntityID(), false,false, false);
							iAgentInFocus = pkEnt->GetEntityID();
						}
					}
					else if(pkEnt->GetProperty("P_DMHQ"))		//selected a HQ , 
					{
						m_kSelectedEntitys.clear();		//clear all selected entitys if a hq is selected
						m_iHQID = pkEnt->GetEntityID();
						
						// Test for double click and in that case open HQ dlg.
						////////////////////////////////////////////////////////
						float fGameTime = m_pkFps->m_pkObjectMan->GetGameTime();
						static bool s_bClickedOnes = false;
						static float s_fClickTime = fGameTime;

						if(s_bClickedOnes == true && fGameTime-s_fClickTime<0.25f)
						{
							m_pkHQDlg->OpenDlg(); // Open the HQ dialog and Pause game
							s_bClickedOnes = false;
						}

						if(s_bClickedOnes == false)
							s_bClickedOnes = true;

						s_fClickTime = fGameTime; 
						////////////////////////////////////////////////////////

					}
			}
			else  //else check for entitys inside the selection square
			{
				Vector3 tl( (m_kSelectSquareStart.x < m_kSelectSquareStop.x) ? m_kSelectSquareStart.x : m_kSelectSquareStop.x,
								m_kSelectSquareStart.y ,
								(m_kSelectSquareStart.z < m_kSelectSquareStop.z) ? m_kSelectSquareStart.z : m_kSelectSquareStop.z);
							
				Vector3 br( (m_kSelectSquareStart.x > m_kSelectSquareStop.x) ? m_kSelectSquareStart.x : m_kSelectSquareStop.x,
								m_kSelectSquareStart.y ,
								(m_kSelectSquareStart.z > m_kSelectSquareStop.z) ? m_kSelectSquareStart.z : m_kSelectSquareStop.z);		
				
				vector<Entity*> kObjects;	
				m_pkObjectMan->GetZoneObject()->GetAllEntitys(&kObjects,false);
	
				for(unsigned int i=0;i<kObjects.size();i++)
				{		
					//objects that should not be clicked on (special cases)
					if(kObjects[i]->GetEntityID() <100000)
						continue;
							
					Vector3 pos = kObjects[i]->GetWorldPosV();
			
					if(pos.x > tl.x && pos.x < br.x)
						if(pos.z > tl.z && pos.z < br.z)
							if(kObjects[i]->GetProperty("P_DMCharacter"))
							{
								// only select characters on your team
								if ( ((P_DMCharacter*)kObjects[i]->GetProperty("P_DMCharacter"))->m_iTeam == 0 )
								{
									iAgentInFocus = kObjects[i]->GetEntityID();								
									SelectAgent(kObjects[i]->GetEntityID(), false,false, false);
								}
							}
				}			
			}

			// Uppdatera GUI:t med den nya agenten i fokus.
			((CGamePlayDlg*)m_pkGamePlayDlg)->SelectAgentGUI(iAgentInFocus, false); 
		}

	}

	//check if we want do do any action
	if(m_pkInputHandle->VKIsDown("action"))
	{
		//FIRE GUN
		if(m_pkInputHandle->VKIsDown("multiselect"))			
		{
			if(m_pkFps->GetTicks()-m_fDelayTimer > 0.1)
			{
				m_fDelayTimer = m_pkFps->GetTicks();
		
				if(Entity* pkPickEnt = GetTargetObject())
				{	
					for(unsigned int i = 0;i < m_kSelectedEntitys.size();i++)
					{
						if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_kSelectedEntitys[i]))
						{
							if(P_DMCharacter* pkCharacter = (P_DMCharacter*)pkEnt->GetProperty("P_DMCharacter"))
							{
								m_fBulletTime = m_pkFps->m_pkObjectMan->GetGameTime();

								int* pGunID; // Start action music only if a bullet have been shoot.
								if(*(pGunID = pkCharacter->m_pkHand->GetItem(0,0)) != -1)
								{
									Entity* gun = m_pkObjectMan->GetObjectByNetWorkID(*pGunID);
									P_DMGun* pkGun = (P_DMGun*) gun->GetProperty("P_DMGun"); 
									if(pkGun && pkGun->HasAmmo() )
										StartSong("data/music/dm action.ogg");	
								}

								//pkCharacter->Shoot (m_kPickPos);
								
								DMOrder kOrder;
								kOrder.m_iOrderType = eAttack;
								kOrder.m_kPosition = m_kPickPos;							 
								
								pkCharacter->ClearOrders();
								pkCharacter->AddOrder(kOrder);									
								
							}
						}
					}
				}
			}	
		}
		else
		{
			m_bActionPressed = true;
		}
	}
	else if(m_bActionPressed)
	{
		m_bActionPressed = false;
	
		if(Entity* pkPickEnt = GetTargetObject())
		{
		
			//walk
			if(pkPickEnt->GetName() == "ZoneObject")	//we clicked on a zone , lets tae a walk
			{
				for(unsigned int i = 0;i < m_kSelectedEntitys.size();i++)
				{
					Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_kSelectedEntitys[i]);
					if(pkEnt)
					{
						if(P_DMCharacter* pkCharProp = (P_DMCharacter*)pkEnt->GetProperty("P_DMCharacter"))
						{					
							if(i==0 && pkCharProp->GetStats()->m_iLife > 0)
							{
								int iNumMoveSounds = pkCharProp->m_vkMoveSounds.size();
								if(iNumMoveSounds > 0)
								{
									m_pkAudioSys->StartSound(
										pkCharProp->m_vkMoveSounds[rand()%iNumMoveSounds], 
										pkEnt->GetIWorldPosV());
								}
							}

							DMOrder kWalkOrder;
							kWalkOrder.m_iOrderType = eWalk;							 
							
							if(m_kSelectedEntitys.size() > 1)
								kWalkOrder.m_kPosition = m_kPickPos + GetFormationPos(m_iCurrentFormation,m_kSelectedEntitys.size(),i);								
							else
								kWalkOrder.m_kPosition = m_kPickPos;
							
							pkCharProp->ClearOrders();
							pkCharProp->AddOrder(kWalkOrder);
							
						}
					}	
				}
				return;
			}
			
			//enter HQ
			if(P_DMHQ* pkHQ = (P_DMHQ*)pkPickEnt->GetProperty("P_DMHQ"))
			{				
				
				for(unsigned int i = 0;i < m_kSelectedEntitys.size();i++)
				{					
					if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_kSelectedEntitys[i]))				
					{
						
						if( (pkPickEnt->GetWorldPosV() - pkEnt->GetWorldPosV()).Length() < 4) 
						{
							cout<<"entering hq"<<endl;
						
							SelectAgent(m_kSelectedEntitys[i], false, false,false); // remove selection
							pkHQ->InsertCharacter(m_kSelectedEntitys[i]);
							
							// Remove selected agent from list
							((CGamePlayDlg*)m_pkGamePlayDlg)->UpdateAgentList();

							// Uppdatera GUI:t och berätta att ingen agent är i fokus.
							((CGamePlayDlg*)m_pkGamePlayDlg)->SelectAgentGUI(-1, false);
							
						}
						/*else
						{
							P_PfPath* pkPF = (P_PfPath*)pkEnt->GetProperty("P_PfPath");
							if(pkPF)//we have selected an entity whit a pathfind property, lets take a walk =)
							{					
						
								//randomize position a bit if theres many characters selected
								if(m_kSelectedEntitys.size() > 1)
									pkPF->MakePathFind(m_kPickPos + GetFormationPos(m_iCurrentFormation,m_kSelectedEntitys.size(),i));								
								else
									pkPF->MakePathFind(m_kPickPos);
								}
*/
						if(P_DMCharacter* pkCh = (P_DMCharacter*)pkEnt->GetProperty("P_DMCharacter"))
						{														
							if( (pkPickEnt->GetWorldPosV() - pkEnt->GetWorldPosV()).Length() < 1) 
							{
								cout<<"entering hq"<<endl;
								//SelectAgent(m_kSelectedEntitys[i], true, false,false); // remove selection

								DMOrder kOrder;
								kOrder.m_iOrderType = eEnterHQ;
								kOrder.m_iEntityID = pkPickEnt->GetEntityID();							 
								
								pkCh->ClearOrders();
								pkCh->AddOrder(kOrder);	
							}
							else
							{
								//SelectAgent(m_kSelectedEntitys[i], true, false,false); // remove selection
								
								pkCh->ClearOrders();
								
								DMOrder kOrder;
								
								//first walk to the item
								kOrder.m_iOrderType = eWalk;
								kOrder.m_kPosition = pkPickEnt->GetWorldPosV();								
								pkCh->AddOrder(kOrder);
							
								//then pick it up
								kOrder.m_iOrderType = eEnterHQ;
								kOrder.m_iEntityID = pkPickEnt->GetEntityID();								
								pkCh->AddOrder(kOrder);							
								
							}
						}
					}
				}
				return;
			}

			// Clicked a ClickMe object :)
			if(P_DMClickMe* pkClick = (P_DMClickMe*)pkPickEnt->GetProperty("P_DMClickMe"))
			{
				// loop through all selected characters.. hmm :/
				for ( int i = 0; i < m_kSelectedEntitys.size(); i++ )
				{
					if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_kSelectedEntitys[i]))				
					{
						// is the character close enough?
						if( (pkPickEnt->GetWorldPosV() - pkEnt->GetWorldPosV()).Length() < 4) 
						{
							pkClick->Click( m_kSelectedEntitys[i] );

							// if is house, deselect
							if ( pkClick->m_bIsHouse )
								SelectAgent (m_kSelectedEntitys[i], false, false, false);
						}
						else
						if(P_PfPath* pkPF = (P_PfPath*)pkEnt->GetProperty("P_PfPath"))
						{				
							//randomize position a bit if theres many characters selected
							if(m_kSelectedEntitys.size() > 1)
								pkPF->MakePathFind(m_kPickPos + GetFormationPos(m_iCurrentFormation,m_kSelectedEntitys.size(),i));								
							else
								pkPF->MakePathFind(m_kPickPos);
						}
					}
				}
				
				//m_kSelectedEntitys.clear();
			}
			
			//pick item
			if(P_DMItem* pkItem = (P_DMItem*)pkPickEnt->GetProperty("P_DMItem"))
			{				
				for(unsigned int i = 0;i < m_kSelectedEntitys.size();i++)
				{
					if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_kSelectedEntitys[i]))				
					{
						if(P_DMCharacter* pkCh = (P_DMCharacter*)pkEnt->GetProperty("P_DMCharacter"))
						{																
							if( (pkPickEnt->GetWorldPosV() - pkEnt->GetWorldPosV()).Length() < 1) 
							{
							
								DMOrder kOrder;
								//pickup item
								kOrder.m_iOrderType = ePickup;
								kOrder.m_iEntityID = pkPickEnt->GetEntityID();							 
								
								pkCh->ClearOrders();
								pkCh->AddOrder(kOrder);							
							}
							else
							{
								pkCh->ClearOrders();
								
								DMOrder kOrder;
								
								//first walk to the item
								kOrder.m_iOrderType = eWalk;
								kOrder.m_kPosition = pkPickEnt->GetWorldPosV();								
								pkCh->AddOrder(kOrder);
							
								//then pick it up
								kOrder.m_iOrderType = ePickup;
								kOrder.m_iEntityID = pkPickEnt->GetEntityID();								
								pkCh->AddOrder(kOrder);
								
							}
						}
					}
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
	
	//GetSystem().RunCommand("load dmworld",CSYS_SRC_SUBSYS);
	
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
//	((P_DMMission*)m_pkGameInfoEntity->GetProperty("P_DMMission"))->Init();

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
				d = (start - cp).Length();
	
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
		GetSystem().RunCommand("set e_runsim 0",CSYS_SRC_SUBSYS);
		GetSystem().RunCommand("set e_simspeed 0.0",CSYS_SRC_SUBSYS);
		m_eGameMode = PAUSED;
	}
	else
	{
		GetSystem().RunCommand("set e_runsim 1",CSYS_SRC_SUBSYS);
		GetSystem().RunCommand("set e_simspeed 0.33",CSYS_SRC_SUBSYS);
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
				SelectAgent(m_kSelectedEntitys[i], true, false,false);

				// Uppdatera GUI:t och berätta att ingen agent är i fokus.
				if(m_pkGamePlayDlg)
					((CGamePlayDlg*)m_pkGamePlayDlg)->SelectAgentGUI(-1, false);

				i = 0;

				update_list = true;
			}		
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

