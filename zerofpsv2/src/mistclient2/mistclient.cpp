/**
	\defgroup MistClient MistClient
	\ingroup MistLand

  MistClient is the Client of the game MistLands.
*/

#ifndef _DONT_MAIN					// <- OBS! Flytta inte pï¿½denna. Mï¿½te ligga i
	#define _MAINAPPLICATION_		// just denna fil och inte pï¿½flera stï¿½len.
	#define _DONT_MAIN
#endif
 
#include "mistclient.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../zerofpsv2/engine/inputhandle.h"
#include "../mcommon/ml_netmessages.h"

#include "../mcommon/p_characterproperty.h"
#include "../mcommon/p_item.h"
#include "../mcommon/p_container.h"
#include "../mcommon/p_enviroment.h"
#include "../mcommon/p_ml.h"
#include "../mcommon/p_shadowblob.h"
#include "actionmenu.h"
#include "gui_inventory.h"
#include "gui_optionsdlg.h"
#include "gui_equipwnd.h"
#include "../mcommon/mainmcommon.h"

MistClient g_kMistClient("MistClient",0,0,0);

bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) ;

void GuiMsgIngameScreen( string strMainWnd, string	strController,	
								 unsigned int msg, int numparms,	void *params )	;

MistClient::MistClient(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	g_ZFObjSys.SetPreLogName("mistclient2");
	g_ZFObjSys.Log_Create("mistclient2");
	
	m_iPickedEntityID = 	-1;
	m_fDelayTime  = 		0;

	m_iCharacterID = 		-1;
	m_bFrontView = 		false;
	m_bShowMenulevel = 	false;
	m_bQuickStart = 		false;
	m_strQuickStartAddress = "127.0.0.1:4242";


	m_strLoginName = "Psykosmurfan";
   m_strLoginPW = "topsecret";

   RegisterVariable("ap_loginname", 		 &m_strLoginName,				CSYS_STRING);
   RegisterVariable("ap_loginpw", 			 &m_strLoginPW,				CSYS_STRING);
	RegisterVariable("ap_showmenulevel", 	 &m_bShowMenulevel,			CSYS_BOOL);
	RegisterVariable("ap_quickstart",		 &m_bQuickStart,				CSYS_BOOL);
	RegisterVariable("ap_quickstartadress", &m_strQuickStartAddress,	CSYS_STRING);

	m_bGuiCapture = false;
} 
 
void MistClient::OnInit() 
{
	//m_pkZFVFileSystem->AddRootPath( string("../datafiles/dm") ,"/data");	
	m_pkZFVFileSystem->AddRootPath( string("../datafiles/mistlands") ,"/data");

	//register commands
	Register_Cmd("say",			FID_SAY);
	Register_Cmd("playerlist",	FID_PLAYERLIST);
	Register_Cmd("killme",		FID_KILLME);
		
	
	//initiate our mainview camera
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.25,250);	
	m_pkCamera->SetName("Main camera");
	m_pkZeroFps->AddRenderCamera(m_pkCamera);

	//register property bï¿½
	RegisterPropertys();

	//register resources
	RegisterResources();
	
	//setup referense sound distance
	m_pkAudioSys->SetReferensDistance(0.25);
	
	//m_pkZeroFps->SetSystemFps(30);
	//m_pkZeroFps->SetNetworkFps(15);
	//m_pkNetwork->SetNetSpeed(4000);	

	//init mistland script intreface
	//MistLandLua::Init(m_pkEntityManager,m_pkScript);

	
	//register emote names
	RegisterEmotes();
	
   // init gui script intreface
	GuiAppLua::Init(&g_kMistClient, m_pkScript);

	//set window title		
   SetTitle("MistClient - Banka och sl");
	
	//set client in server mode to show gui etc
	m_pkZeroFps->StartServer(true,false);

	// create gui for mistlands
	SetupGUI();
	
	//run autoexec script
	if(!m_pkIni->ExecuteCommands("mistclient_autoexec.ini"))
		m_pkConsole->Printf("No game_autoexec.ini.ini found");	
	
	//test
	if(m_bShowMenulevel)
	{
		m_pkEntityManager->SetWorldDir("clienttemp");
		if(m_pkEntityManager->LoadWorld("../datafiles/mistlands/menulevel"))
		{
			m_pkEntityManager->SetTrackerLos(50);
			
			Entity* pkEnt = m_pkEntityManager->CreateEntity();
			pkEnt->SetParent(m_pkEntityManager->GetWorldEntity());
			
			pkEnt->AddProperty("P_Track");
			P_Enviroment* pkEnv = (P_Enviroment*)pkEnt->AddProperty("P_Enviroment");	
			pkEnv->LoadEnviroment("data/enviroments/menu.env");
			pkEnv->SetEnable(true);	
		}
		else
			cout<<"WARNING: could not find menulevel"<<endl;	
	}
	else
	{
		GetWnd("MLStartWnd")->GetSkin()->m_bTransparent = false;
	}

	if(m_bQuickStart)
	{
		g_kMistClient.m_pkZeroFps->StartClient(m_strLoginName, m_strLoginPW, m_strQuickStartAddress);		
	}

 	if(m_pkIni->GetIntValue("ZFAudioSystem", "a_enablesound") == 0 && 
 		m_pkIni->GetIntValue("ZFAudioSystem", "a_enablemusic") == 0)
 		m_pkAudioSys->SetMainVolume(0); // tempgrej för att stänga av all audio, finns inget vettigt sett för tillfället

}

void MistClient::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid)
	{
		case FID_SAY:
		{
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				m_pkConsole->Printf("say [message]");
				break;				
			}
			
			string strMsg;
			for(int i = 4;i<kCommand->m_strFullCommand.size();i++)
				strMsg.push_back(kCommand->m_strFullCommand[i]);
					
				
			Say(strMsg);	
			break;
		}
		
		case FID_PLAYERLIST:
		{
			SendRequestPlayerList();			
			break;
		}
	
		case FID_KILLME:
		{
			SendRequestKillMe();
			break;
		}	
	}
}

void MistClient::RegisterEmotes()
{
	m_kEmotes.push_back("wave");
	m_kEmotes.push_back("boo");
	m_kEmotes.push_back("yawn");
	m_kEmotes.push_back("donno");
	m_kEmotes.push_back("confused");
	
}


void MistClient::Say(string strMsg)
{
	if(strMsg.size() == 0)
		return;

		
	if(strMsg[0] == '/')
	{
		//handle different chatbox functions		
		if(strMsg.length() > 1)
		{
			//list users
			if(strMsg.substr(1) == "users" || strMsg.substr(1) == "players")
			{
				SendRequestPlayerList();
				return;
			}
		
			//emote with "/emote [emote] [text]"
			if(strMsg.substr(1,5) == "emote")
			{
				if(strMsg.length() >= 7)
				{
					//cout<<"emote:"<<strMsg.substr(7,strMsg.find(" ",7) - 7)<<"|"<<endl;
					SendTaunt(strMsg.substr( 7 , strMsg.find(" ",7) - 7) );
					
					if(strMsg.length() > strMsg.find(" ",7))
						SendMessage(strMsg.substr(strMsg.find(" ",7))  ,MLCM_TALK,""); 	
				}
				
				return;
			}
			
			//emote with "/e[nr] [text]"	
			if(strMsg.substr(1,1) == "e")
			{
				if(strMsg.length() >= 3)
				{
					string strNr = strMsg.substr(2);										
					SendTaunt(atoi(strNr.c_str()));
									
					if(strMsg.length() > strMsg.find(" "))
						SendMessage(strMsg.substr(strMsg.find(" "))  ,MLCM_TALK,""); 	
				
				}
				
				return;
			}
		}
	}
		
		
	//check for inline emotes
	int iEmoteStart = strMsg.find("/");
	
	if(iEmoteStart != -1)
	{
		int iEmoteEnd = strMsg.find(" ",iEmoteStart)-1;		
		//cout<<"line:"<<iEmoteStart<<" - "<<iEmoteEnd<<endl;		
		//cout<<"emote:"<<strMsg.substr(iEmoteStart+1,iEmoteEnd - iEmoteStart)<<"|"<<endl;
		
		SendTaunt(strMsg.substr(iEmoteStart+1,iEmoteEnd - iEmoteStart));
		
		string msg = strMsg.substr(0,iEmoteStart);
		if(strMsg.length() > iEmoteEnd+1)
		{
			msg+=strMsg.substr(iEmoteEnd+1);			
			//cout<<"msg:"<<msg<<"|"<<endl;
			//SendMessage(strMsg,MLCM_TALK,"");
		}
					
		if(!msg.empty())			
			SendMessage(msg,MLCM_TALK,""); 	
		return;
	}
		
	
	
	
	//default to normal talk
	if(strMsg[0] != '/')
		SendMessage(strMsg,MLCM_TALK,""); 	
									
}

void MistClient::SendTaunt(const string& strEmote)
{
	for(int i = 0;i< m_kEmotes.size();i++)
		if(m_kEmotes[i] == strEmote)
			SendTaunt(i+1);							//emotes start at ID 1

}

void MistClient::SendTaunt(int iID)
{
	NetPacket kNp;	
	kNp.Clear();
	kNp.Write((char) MLNM_CS_ANIM);

	kNp.Write(iID);
	
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);	
}

void MistClient::SendMessage(string strMsg,int iChannel,string strToWho)
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_SAY);
	kNp.Write_Str(strMsg);
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);	
}

void MistClient::RegisterResources()
{
	//m_pkResourceDB->RegisterResource( string(".env"), Create__EnvSetting	);
}

void MistClient::RegisterPropertys()
{
	MCommon_RegisterPropertys( m_pkZeroFps, m_pkPropertyFactory );

}

void MistClient::RenderInterface(void)
{
 
}

void MistClient::OnIdle() 
{
	Input();

	// Degree To Compass Angle
	// 0.0		= Norr
	// PI/2		= East
	// PI			= South
	// PI+PI/2	= West
	ZGuiLabel* pkCompass = (ZGuiLabel*) GetWnd("CompassLabel");
	if(pkCompass)
	{
		if(Entity* pkCharacter = m_pkEntityManager->GetEntityByID(m_iCharacterID))
		{
			if(P_Camera* pkCam = (P_Camera*)pkCharacter->GetProperty("P_Camera"))
			{
				float fAngle = pkCam->Get3PYAngle();
				float fMultipel = (PI+PI) / 360.0f;
				pkCompass->GetSkin()->m_fRotDegree = fMultipel * fAngle; 
			}
		}
	}

// 	if(m_iCharacterID == -1) 
// 	{
// 		NetPacket kNp;				
// 		kNp.Clear();
// 		kNp.Write((char) MLNM_CS_REQ_CHARACTERID);
// 		kNp.TargetSetClient(0);
// 		SendAppMessage(&kNp);	
// 	}
}

void MistClient::OnHud(void) 
{
	if(m_iCharacterID != -1)
		if(!m_bGuiCapture)
			if(!m_bFrontView)
				DrawCrossHair();
}

void MistClient::DrawCrossHair()
{
	static ZMaterial* pkNormalPointer = NULL;
	if(!pkNormalPointer)
	{
		pkNormalPointer = new ZMaterial;
		pkNormalPointer->GetPass(0)->m_kTUs[0]->SetRes("data/textures/crosshair.tga");	
		pkNormalPointer->GetPass(0)->m_bLighting = 	false;
		pkNormalPointer->GetPass(0)->m_bFog = 			false;	
		pkNormalPointer->GetPass(0)->m_iPolygonModeFront = FILL_POLYGON;		
		//for alpha instead
		pkNormalPointer->GetPass(0)->m_bAlphaTest = true;
				
		//blending is much nicer thou =)
/*		pkNormalPointer->GetPass(0)->m_bDepthMask = false;
		pkNormalPointer->GetPass(0)->m_bBlend = true;
		pkNormalPointer->GetPass(0)->m_iBlendSrc = SRC_ALPHA_BLEND_SRC;
		pkNormalPointer->GetPass(0)->m_iBlendDst = ONE_MINUS_SRC_ALPHA_BLEND_DST;	*/		
	}

	static ZMaterial* pkActivePointer = NULL;
	if(!pkActivePointer)
	{
		pkActivePointer = new ZMaterial;
		pkActivePointer->GetPass(0)->m_kTUs[0]->SetRes("data/textures/crosshair-active.tga");	
		pkActivePointer->GetPass(0)->m_bLighting = 	false;
		pkActivePointer->GetPass(0)->m_bFog = 			false;	
		pkActivePointer->GetPass(0)->m_iPolygonModeFront = FILL_POLYGON;
		pkActivePointer->GetPass(0)->m_bAlphaTest = true;
	}	

	//add geometry
	m_pkZShaderSystem->ClearGeometry();
	m_pkZShaderSystem->AddQuadV(	Vector3(-0.05,0.05,-1),Vector3(-0.05,-0.05,-1),
											Vector3(0.05,-0.05,-1),Vector3(0.05,0.05,-1));
	m_pkZShaderSystem->AddQuadUV(	Vector2(0,0)	,	Vector2(0,1),
											Vector2(1,1)	,	Vector2(1,0));
	
	//active or normal pointer											
	bool bActive = false;																						
	if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iPickedEntityID))
		if(pkEnt->GetProperty("P_Ml"))
			bActive = true;
	
	//set material					
	if(bActive)
		m_pkZShaderSystem->BindMaterial(pkActivePointer);
	else
		m_pkZShaderSystem->BindMaterial(pkNormalPointer);
		
	//draw pointer
	m_pkZShaderSystem->DrawGeometry(QUADS_MODE);
}

bool MistClient::DelayCommand()
{
	if(m_pkZeroFps->GetEngineTime() < m_fDelayTime)
		return true;

	m_fDelayTime = m_pkZeroFps->GetEngineTime() + float(0.3);
	return false;
}


void MistClient::Input()
{
	if(m_pkInputHandle->VKIsDown("togglegui") && !DelayCommand())
	{
		bool bSet = !m_bGuiCapture;
		SetGuiCapture(bSet);
	}

	//get relative mouse
	float x=0;
	float z=0;		
	m_pkInputHandle->RelMouseXY(x,z);	
	
	//get absolute unit mouse
	float fAx=0;
	float fAy=0;
	m_pkInputHandle->UnitMouseXY(fAx,fAy);
	
	// gui stuff
	static bool s_bEscPressed = false;
	if(m_pkInputHandle->Pressed(KEY_ESCAPE) && s_bEscPressed == false){
		s_bEscPressed = true; CloseActiveWindow();
	} else if(!m_pkInputHandle->Pressed(KEY_ESCAPE))
		s_bEscPressed = false;

	if(m_pkInputHandle->Pressed(KEY_I) && !DelayCommand())
	{			
		if(m_pkInventoryDlg->IsVisible())
			m_pkInventoryDlg->Close(); 
		else
			RequestOpenInventory();
	}

	if(m_pkInputHandle->Pressed(KEY_E) && !DelayCommand())
	{			
		if(m_pkEquipmentDlg->IsVisible())
			m_pkEquipmentDlg->Close(); 
		else
		{
			SendRequestOpenEqipment();
		}
	}

	if(m_pkInputHandle->Pressed(KEY_C) && !DelayCommand())
	{			
		bool bOpen = !IsWndVisible("ChatDlgMainWnd");
		SetText("SayTextbox", "");
		ToogleChatWnd(bOpen);
	}
			
	// taunts
	if ( m_pkInputHandle->VKIsDown("taunt1") || m_pkInputHandle->VKIsDown("taunt2")|| 
		m_pkInputHandle->VKIsDown("taunt3") || m_pkInputHandle->VKIsDown("taunt4") || 
		m_pkInputHandle->VKIsDown("taunt5") )
	{					
		if(!DelayCommand())
		{
			int iTauntID = 0;
			if (m_pkInputHandle->VKIsDown("taunt1"))
				iTauntID = 1;
			if (m_pkInputHandle->VKIsDown("taunt2"))
				iTauntID = 2;
			if (m_pkInputHandle->VKIsDown("taunt3"))
				iTauntID = 3;
			if (m_pkInputHandle->VKIsDown("taunt4"))
				iTauntID = 4;
			if (m_pkInputHandle->VKIsDown("taunt5"))
				iTauntID = 5;

			SendTaunt(iTauntID);
		}
	}

	//update picked object	
	if(Entity* pkPickedEnt = GetTargetObject())	
		m_iPickedEntityID = pkPickedEnt->GetEntityID();
	else
		m_iPickedEntityID = -1;
	
		
	//list actions
	if ( m_pkInputHandle->VKIsDown("look") )
	{
		if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iPickedEntityID))
		{
			if(P_Ml* pkMl = (P_Ml*)pkEnt->GetProperty("P_Ml"))
			{						
				m_pkActionDlg->SetEntity(m_iPickedEntityID);							
				m_pkActionDlg->Open();
			}
		}
	}

	//perform the first action in the action list or pickup
	if( m_pkInputHandle->VKIsDown("use") )
	{
		if(!DelayCommand() )
		{			
			if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iPickedEntityID))
			{								
				//if its an item , pick it up
				if(P_Item* pkItem = (P_Item*)pkEnt->GetProperty("P_Item"))
				{
					cout<<"trying pickup"<<endl;
					RequestPickup(m_iPickedEntityID);
				}
				else 
				// if not an item do first action
				if(P_Ml* pkMl = (P_Ml*)pkEnt->GetProperty("P_Ml"))
				{
					cout<<"performing first action"<<endl;
					vector<string>	kActions;
					pkMl->GetActions(kActions);
					
					if(!kActions.empty())
					{
						SendAction(m_iPickedEntityID,kActions[0]);
					}
				}			
			}
		}	
	}	
		
	//check buttons
	m_kCharacterControls[eUP] = 	m_pkInputHandle->VKIsDown("move_forward");
	m_kCharacterControls[eDOWN] =	m_pkInputHandle->VKIsDown("move_back");			
	m_kCharacterControls[eLEFT] = m_pkInputHandle->VKIsDown("move_left");			
	m_kCharacterControls[eRIGHT]= m_pkInputHandle->VKIsDown("move_right");
	m_kCharacterControls[eJUMP] = m_pkInputHandle->VKIsDown("jump");
	m_kCharacterControls[eCRAWL] =m_pkInputHandle->VKIsDown("crawl");	


	//update camera
	if(Entity* pkCharacter = m_pkEntityManager->GetEntityByID(m_iCharacterID))
	{
		if(P_Camera* pkCam = (P_Camera*)pkCharacter->GetProperty("P_Camera"))
		{	
		
			//togle front view
			if(m_pkInputHandle->VKIsDown("frontview") && !DelayCommand())
			{
				static float fOldDistance = 0;
				if(m_bFrontView)
				{
					m_bFrontView = false;
					pkCam->Set3PDistance(fOldDistance);
				}
				else
				{		
					m_bFrontView = true;
					fOldDistance = pkCam->Get3PDistance();
				}
			}				
		
			//capture mouse pointer?
			if(!m_bGuiCapture)
			{
				pkCam->Set3PYAngle(pkCam->Get3PYAngle() - (x/5.0));
				pkCam->Set3PPAngle(pkCam->Get3PPAngle() + (z/5.0));			
			}
			else
			{
				//hot edge camera rotation
				if(fAx >= 0.49)
					pkCam->Set3PYAngle(pkCam->Get3PYAngle() - m_pkZeroFps->GetFrameTime()*100);
				if(fAx <= -0.49)
					pkCam->Set3PYAngle(pkCam->Get3PYAngle() + m_pkZeroFps->GetFrameTime()*100);			

				if(fAy >= 0.49)
					pkCam->Set3PPAngle(pkCam->Get3PPAngle() + m_pkZeroFps->GetFrameTime()*100);
				if(fAy <= -0.49)
					pkCam->Set3PPAngle(pkCam->Get3PPAngle() - m_pkZeroFps->GetFrameTime()*100);
			}
			
			//get current distance
			float fDistance = pkCam->Get3PDistance();
			
			//handle zoom
			if(m_pkInputHandle->VKIsDown("zoomin")) 	fDistance -= 0.5;
			if(m_pkInputHandle->VKIsDown("zoomout"))	fDistance += 0.5;
			
			//set max/min zoom distance
			if(m_bFrontView)
			{
				//make sure camera is nto to far away
				if(fDistance > 3.0)
					fDistance = 3.0;			
				//make sure camera is not to close
				if(fDistance < 0.5)
					fDistance = 0.5;			
			}else
			{
				//make sure camera is nto to far away
				if(fDistance > 8.0)
					fDistance = 8.0;			
				//make sure camera is not to close
				if(fDistance < 0.2)
					fDistance = 0.2;
			}
			
				
			if(m_bFrontView)
			{
				pkCam->SetType(CAM_TYPEMLFRONTVIEW);
				pkCam->SetOffset(Vector3(0,0,0)); 							
				
				//enable player model i 3d person
				if(P_Mad* pkMad = (P_Mad*)pkCharacter->GetProperty("P_Mad"))
					pkMad->SetVisible(true);				
			}
			else
			{				
				// first person
				if(fDistance < 0.3)	
				{	
					pkCam->SetType(CAM_TYPEFIRSTPERSON_NON_EA);
	
					pkCam->SetOffset(Vector3(0,0,0)); 							
									
					//disable player model in first person
					if(P_Mad* pkMad = (P_Mad*)pkCharacter->GetProperty("P_Mad"))
						pkMad->SetVisible(false);					
				}
				
				// 3-person view
				else			
				{
					pkCam->SetType(CAM_TYPE3PERSON);
					pkCam->SetOffset(Vector3(0,0.4,0)); 							
					
					//enable player model i 3d person
					if(P_Mad* pkMad = (P_Mad*)pkCharacter->GetProperty("P_Mad"))
						pkMad->SetVisible(true);				
				}			 
			}
								
			pkCam->Set3PDistance(fDistance);
		
			//rotate character
			Matrix4 kRot;
			kRot.Identity();
			kRot.Rotate(0,pkCam->Get3PYAngle(),0);
			kRot.Transponse();				
			pkCharacter->SetLocalRotM(kRot);			

		}
	}
}

void MistClient::OnSystem() 
{
	UpdateCharacter();
	SendControlInfo();
	CloseActiveContainer();
}

void MistClient::SendControlInfo()
{
	if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iCharacterID))
	{
		if(P_Camera* pkCam = (P_Camera*)pkEnt->GetProperty("P_Camera"))
		{
			//request character entityID
			NetPacket kNp;	
			kNp.Clear();
			kNp.Write((char) MLNM_CS_CONTROLS);
			
			kNp.Write(m_kCharacterControls);
			kNp.Write(pkCam->Get3PYAngle() );
			kNp.Write(pkCam->Get3PPAngle() );
			
			kNp.TargetSetClient(0);
			SendAppMessage(&kNp);		
		}
	}
}

void MistClient::UpdateCharacter()
{
	if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iCharacterID))
	{
		//if theres no camera property, create one and set it up
		if(!pkEnt->GetProperty("P_Camera"))
		{
			if(P_Camera* pkCam = (P_Camera*)pkEnt->AddProperty("P_Camera"))
			{
				pkCam->SetCamera(m_pkCamera);
				pkCam->SetType(CAM_TYPE3PERSON);
				pkCam->Set3PPAngle(.30);					
				pkCam->Set3PYAngle(0);
				pkCam->Set3PDistance(4);			
				
				pkCam->SetAttachToBone(true);
				pkCam->SetBone("headjoint1");
			}			
		}
		
		//disable overhead text for own playercharacter
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)pkEnt->GetProperty("P_CharacterProperty"))
			pkCP->SetOverHeadText(false);					
		
		
		if(P_Enviroment* pkEnv = (P_Enviroment*)pkEnt->GetProperty("P_Enviroment"))
		{
			pkEnv->SetEnable(true);				
		}	
	}
}


void MistClient::OnNetworkMessage(NetPacket *pkNetMessage)
{
	unsigned char ucType;

	// Read Type of Message.
	pkNetMessage->Read(ucType);

	switch(ucType)
	{
		
	
		case MLNM_SC_CHARACTERID:
		{
			cout<<"got character entityID from server"<<endl;
			pkNetMessage->Read(m_iCharacterID);
		
			break;
		}

			
		case MLNM_SC_SAY:
		{
			string strMsg;
			string strSource;
			int iCharacterID;
			pkNetMessage->Read(iCharacterID);
			pkNetMessage->Read_Str(strSource);
			pkNetMessage->Read_Str(strMsg);

						
			AddStringToChatBox(strSource+string(": ")+strMsg);
			
			if(iCharacterID != -1)
			{
				if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(iCharacterID,"P_CharacterProperty"))
				{
					pkCP->AddChatMsg(strMsg);
				}
			}
			
						
			break;
		}			

		case MLNM_SC_BUFFLIST:
		{
			cout<<"got buff list from server"<<endl;
		
			string strName;
			string strIcon;
			float  fTimeout;
			char   cType;
			
			pkNetMessage->Read_Str(strName);
			while(!strName.empty())
			{
				pkNetMessage->Read_Str(strIcon);
				pkNetMessage->Read(fTimeout);
				pkNetMessage->Read(cType);							
			
				pkNetMessage->Read_Str(strName);			
			}
			
			
			break;
		}
		
		case MLNM_SC_CHARLIST:
		{
			int iNumOfChar;
			string strChar;
			vector<string> kCharList;
			
			pkNetMessage->Read(iNumOfChar);
			
			m_pkConsole->Printf("Listing Players : %d",iNumOfChar);
			
			int i;
			for(i = 0;i<iNumOfChar;i++)
			{
				pkNetMessage->Read_Str(strChar);
				kCharList.push_back(strChar);					
			}

			g_kMistClient.ClearListbox("CharGen_CharList");
			for(i=0; i<kCharList.size(); i++ )
			{
				char szArghhh[256];
				strcpy(szArghhh, kCharList[i].c_str());
				g_kMistClient.AddListItem("CharGen_CharList",szArghhh , true);	
			}

			ZGuiCombobox* pkCharbox =(ZGuiCombobox*)GetWnd("CharGen_CharList");
			if(pkCharbox)
			{
				int iItem = pkCharbox->GetListbox()->GetItemCount();				
				pkCharbox->SetNumVisibleRows(iItem > 20 ? 20 : iItem);
			}


			break;
		}

		case MLNM_SC_PLAYERLIST:
		{
			int iPlayers;
			string strName;
			
			m_kPlayerList.clear();
			
			pkNetMessage->Read(iPlayers);
			
			//m_pkConsole->Printf("Listing Players : %d",iPlayers);
			AddStringToChatBox("Requesting playerlist");
			
			for(int i = 0;i<iPlayers;i++)
			{
				pkNetMessage->Read_Str(strName);
				//m_pkConsole->Printf("%d %s",i,strName.c_str());
				AddStringToChatBox(strName);
				m_kPlayerList.push_back(strName);					
			}
			
			char nr[5];
			IntToChar(nr,iPlayers);
			AddStringToChatBox(string(nr) + string(" online"));
			
			
			break;
		}
		
		case MLNM_SC_CONTAINER:
		{		
			int iContainerID;
			int iContainerType;
			char cSizeX;
			char cSizeY;
			int  iItems;
			bool bOpen;
			
			vector<MLContainerInfo> kItemList;			
			
			pkNetMessage->Read(bOpen);
			pkNetMessage->Read(iContainerID);
			pkNetMessage->Read(iContainerType);
			pkNetMessage->Read(cSizeX);
			pkNetMessage->Read(cSizeY);

			pkNetMessage->Read(iItems);			
			
			
			MLContainerInfo kTemp;
			for(int i = 0;i<iItems;i++)
			{
				pkNetMessage->Read_Str(kTemp.m_strName);
				pkNetMessage->Read_Str(kTemp.m_strIcon);
				
				pkNetMessage->Read(kTemp.m_iItemID);
				pkNetMessage->Read(kTemp.m_cItemX);
				pkNetMessage->Read(kTemp.m_cItemY);
				pkNetMessage->Read(kTemp.m_cItemW);
				pkNetMessage->Read(kTemp.m_cItemH);		
				pkNetMessage->Read(kTemp.m_cItemType);			
				pkNetMessage->Read(kTemp.m_iStackSize);				
				pkNetMessage->Read(kTemp.m_bIsContainer);				
				
				kItemList.push_back(kTemp);
			}
			
			/*
			cout<<"type:"<<iContainerType<<endl;
			cout<<"size:"<<int(cSizeX)<<" x "<<int(cSizeY)<<endl;
			cout<<"number of items:"<<iItems<<endl;						
		
			for(int i = 0;i<kItemList.size();i++)
			{
				cout<<i<<" id:"<<kItemList[i].m_iItemID<<" name:"<<kItemList[i].m_strName<<" icon:"<<kItemList[i].m_strIcon<<" pos:"<<int(kItemList[i].m_cItemX)<<" x "<<int(kItemList[i].m_cItemY)<<" stack:"<<kItemList[i].m_iStackSize<<endl;			
			}
			*/
			
			//is this and inventory?
			if(iContainerType == eInventory)
			{
				printf("bOpen = %i\n", bOpen);

				//do we want to open the container window?
				if(bOpen)
					if(!m_pkInventoryDlg->IsVisible())
						m_pkInventoryDlg->Open(); 
					
				printf("Updating inventory\n");
				m_pkInventoryDlg->UpdateInventory(kItemList);
			}
			else
			if(iContainerType == eNormal)
			{
				if(bOpen)
					if(!m_pkInventoryDlg->IsVisible())
					{
						m_pkInventoryDlg->Open();
						RequestOpenInventory();
					}

				if(bOpen)					
					m_pkInventoryDlg->OpenContainerWnd(iContainerID, cSizeX, cSizeY);

				m_pkInventoryDlg->UpdateContainer(kItemList);
			}
			else
			//if(iContainerType == eHead)  //head 
			{
				if(bOpen)
				{
					// this is the first container in the eqipment screen, lets open eqipment
					m_pkEquipmentDlg->Open(); 			
				}

				m_pkEquipmentDlg->Update(iContainerID, iContainerType, kItemList );
			}
				
			break;
		}		
		
		
		default:
			cout << "Error in game packet : " << (int) ucType << endl;
			pkNetMessage->SetError(true);
			return;
	}
} 

void MistClient::OnClientStart(void)
{
	m_pkConsole->Printf("Trying to connect");	
	m_iCharacterID		= -1;
	m_bFrontView		= false;
}

void MistClient::OnClientConnected() 
{
	m_pkConsole->Printf("Successfully connected to server");

	//request character entityID
// 	NetPacket kNp;				
// 	kNp.Clear();
// 	kNp.Write((char) MLNM_CS_REQ_CHARACTERID);
// 	kNp.TargetSetClient(0);
// 	SendAppMessage(&kNp);	

	//load ingame gui	
	//LoadInGameGui();
}

void MistClient::OnDisconnect(int iConnectionID)
{
	// Clear Entity Manger.
	m_pkEntityManager->Clear();
   
	// Load start screen.
	LoadStartScreenGui(true);

	m_iCharacterID = -1;

	cout << "NOOOOOOOOOOOO im disconnected" << endl;
}


void MistClient::AddRemoveServer(const char* szName, const char* szSeverIP, bool bAdd)
{
   if(bAdd)
   {
      for(int i=0; i<m_kServerList.size(); i++)
      {
         if(m_kServerList[i].first == string(szName) &&
            m_kServerList[i].second == string(szSeverIP))
            return;
      }

      m_kServerList.push_back( pair<string, string>(string(szName), string(szSeverIP)) );
   }
   else
   {
      vector<pair<string,string> >::iterator it = m_kServerList.begin();
      for( ; it != m_kServerList.end(); it++)
      {
         if((it)->first == string(szName) && (it)->second == string(szSeverIP))
         {
            m_kServerList.erase(it); 
            return;
         }
      }
   }
}

void MistClient::UpdateServerListbox()
{
   g_kMistClient.ClearListbox("ServerList");

   char szText[150];
   for(int i=0; i<m_kServerList.size(); i++)
   {
      sprintf(szText, "%s - %s", m_kServerList[i].first.c_str(), m_kServerList[i].second.c_str());
      g_kMistClient.AddListItem("ServerList", szText, i==0);
   }
}

bool MistClient::ReadWriteServerList(bool bRead)
{
   if(bRead)
   {
      m_kServerList.clear();

      FILE* pkFile = fopen("serverlist_mistclient.txt", "rt");
      if(pkFile == NULL)
         return false;

      char strLine[512];
	   while (!feof(pkFile))
	   {
		   fgets(strLine, 512, pkFile);
         string strText = string(strLine);

         int pos = (int) strText.find("-");

         if(pos != string::npos)
         {
            string strName = strText.substr(0, pos-1);
            string strIP = strText.substr(pos+2, /*strText.length()*/strText.length()-pos-3);

            if(strName.length() > 0 && strIP.length() > 0) 
               AddRemoveServer(strName.c_str(), strIP.c_str());
         }
	   }

      fclose(pkFile);
   }
   else
   {
      FILE* pkFile = fopen("serverlist_mistclient.txt", "wt");
      if(pkFile == NULL)
         return false;

      for(int i=0; i<m_kServerList.size(); i++)
      {
         if(m_kServerList[i].first.length() > 0 && m_kServerList[i].second.length() > 0) 
            fprintf(pkFile, "%s - %s\n", m_kServerList[i].first.c_str(), m_kServerList[i].second.c_str());
      }

      fclose(pkFile);
   }

   return true;
}

void MistClient::SendRequestKillMe()
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_REQ_KILLME);
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);		
}

void MistClient::SendRequestPlayerList()
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_REQ_PLAYERLIST);
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);		
}

bool MistClient::ShutDown()
{
   ReadWriteServerList(false);
	delete m_pkInventoryDlg;
	delete m_pkEquipmentDlg;
	delete m_pkActionDlg;
	delete m_pkOptionsDlg;
   return true;
}

bool MistClient::StartUp()
{
   ReadWriteServerList(true);
   return true;
}

Entity* MistClient::GetTargetObject()
{
	Vector3 start = m_pkCamera->GetPos();
	Vector3 dir;
	dir = Get3DMouseDir(m_bGuiCapture);

	vector<Entity*> kObjects;
	kObjects.clear();
	
	m_pkEntityManager->GetZoneEntity()->GetAllEntitys(&kObjects);
	
//	cout<<"nr of targets: "<<kObjects.size()<<endl;
	
	float closest = 999999999;
	Entity* pkClosest = NULL;	
	for(unsigned int i=0;i<kObjects.size();i++)
	{
		//objects that should not be clicked on (special cases)
		if(kObjects[i]->GetEntityID() <100000)
			continue;
		
		if(kObjects[i]->GetName() == "A t_serverinfo.lua")
			continue;		
		if(kObjects[i]->GetType() == "hosplayer.lua")
			continue;		
		if(kObjects[i]->GetType() == "Entity")
			continue;		
		
		//-------------
		
		//get mad property and do a linetest
		P_Mad* mp = (P_Mad*)kObjects[i]->GetProperty("P_Mad");
		if(mp)
		{
			if(mp->TestLine(start,dir))
			{	
				float d = (start - kObjects[i]->GetWorldPosV()).Length();
	
				if(d < closest)
				{
					closest = d;
					pkClosest = kObjects[i];
				}				
			}
		}		
		
	}
	
	return pkClosest;
}

/*	Return 3D postion of mouse in world. */
Vector3 MistClient::Get3DMouseDir(bool bMouse)
{
	Vector3 dir;
	float x,y;		
	
	//screen propotions
	float xp=4;
	float yp=3;

	Vector3 kViewSize, kViewCorner;
	kViewSize = m_pkCamera->GetViewPortSize();
	kViewCorner = m_pkCamera->GetViewPortCorner();
	
	if(bMouse)
	{
		// Zeb was here! Nu kï¿½ vi med operativsystemets egna snabba musmarkï¿½
		// alltsï¿½mï¿½te vi anvï¿½da den position vi fï¿½ dï¿½ifrï¿½.
		m_pkInputHandle->UnitMouseXY(x,y);
		//x = -0.5f + (float) m_pkInputHandle->m_iSDLMouseX / (float) m_pkApp->m_iWidth;
		//y = -0.5f + (float) m_pkInputHandle->m_iSDLMouseY / (float) m_pkApp->m_iHeight;
		
		
		/*
		int mx;		
		int my;
		
		//m_pkInputHandle->SDLMouseXY(mx,my);
		
		x = -0.5f + (float) (mx - kViewCorner.x) / (float) kViewSize.x;
		y = -0.5f + (float) ((m_pkApp->m_iHeight - my) - kViewCorner.y) / (float) kViewSize.y;
		*/
		
		if(m_pkCamera->GetViewMode() == Camera::CAMMODE_PERSP)
		{
			dir.Set(x*xp,-y*yp,-2.15);
			dir.Normalize();
		}
		else
		{
			dir.Set(0,0,-1);
			//dir.Normalize();
			//return dir;
		}
	}
	else
	{
		dir.Set(0,0,-1.5);
		dir.Normalize();	
	}
	
	Matrix4 rm = m_pkCamera->GetRotM();
	rm.Transponse();
	dir = rm.VectorTransform(dir);
	
	return dir;
}


void MistClient::SetGuiCapture(bool bCapture, bool bMoveCursorToCenter)
{
	m_bGuiCapture = bCapture;
	
	if(m_bGuiCapture == true)
	{
		//g_kMistClient.m_pkGui->m_bHandledMouse = false;
		if(bMoveCursorToCenter)
			m_pkInputHandle->SetCursorInputPos(m_pkRender->GetWidth()/2,m_pkRender->GetHeight()/2);
		m_pkGui->ShowCursor(true);
	}
	else
	{
		m_pkGui->ShowCursor(false);
	}
}

void MistClient::SendAction(int iEntityID,const string& strAction)
{

	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_ACTION);
	
	kNp.Write(iEntityID);
	kNp.Write_Str(strAction);
	
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);			
}

void MistClient::SendRequestOpenEqipment()
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_REQ_EQIPMENT);
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);				
}

void MistClient::RequestOpenInventory()
{
	if(Entity* pkCharacter = m_pkEntityManager->GetEntityByID(m_iCharacterID))
	{
		if(P_CharacterProperty* pkCharProp = (P_CharacterProperty*)pkCharacter->GetProperty("P_CharacterProperty"))
		{
			SendRequestContainer(pkCharProp->m_iInventory);
		}
	}
}

void MistClient::RequestPickup(int iEntityID,int iPosX,int iPosY)
{
	if(Entity* pkCharacter = m_pkEntityManager->GetEntityByID(m_iCharacterID))
	{
		if(P_CharacterProperty* pkCharProp = (P_CharacterProperty*)pkCharacter->GetProperty("P_CharacterProperty"))
		{
			SendMoveItem(iEntityID,pkCharProp->m_iInventory,iPosX,iPosY);
		}
	}
}


void MistClient::SendRequestContainer(int iContainerID)
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_REQ_CONTAINER);
	kNp.Write(iContainerID);
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);			
}

void MistClient::SendMoveItem(int iItemID,int iTarget,int iPosX,int iPosY,int iCount)
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_MOVE_ITEM);
	
	kNp.Write(iItemID);
	kNp.Write(iTarget);
	kNp.Write(iPosX);
	kNp.Write(iPosY);
	kNp.Write(iCount);
	
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);			
}

void MistClient::CloseActiveContainer()
{
	if(m_pkInventoryDlg == NULL)
		return;

	if(!m_pkInventoryDlg->IsVisible())
		return;
	
	if(Entity* pkContainer = m_pkEntityManager->GetEntityByID(m_pkInventoryDlg->m_iActiveContainerID))
		if(Entity* pkCharacter = m_pkEntityManager->GetEntityByID(m_iCharacterID))
		{
			Vector3 dist = pkCharacter->GetWorldPosV() - pkContainer->GetWorldPosV();
			if(dist.Length() > 2)
				m_pkInventoryDlg->CloseContainerWnd(); 
		}
}

void MistClient::RegBeginPlay(string strChar)
{
	NetPacket kNp;
	kNp.Clear();
	kNp.Write((char) MLNM_CS_REQPLAY);
	kNp.Write_Str( strChar );
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);
}

void MistClient::DeleteChar(string strChar)
{
	NetPacket kNp;
	kNp.Clear();
	kNp.Write((char) MLNM_CS_CHARDEL);
	kNp.Write_Str(strChar);
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);
}

void MistClient::AddChar(string strChar, string strMod)
{
	NetPacket kNp;
	kNp.Clear();
	kNp.Write((char) MLNM_CS_CHARADD);
	kNp.Write_Str(strChar);
	kNp.Write_Str(strMod);
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);
}

