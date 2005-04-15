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
	
	m_iPickedEntityID 	= 	-1;
	m_fDelayTime  			=	0;

	m_iCharacterID 		=	-1;
	m_iTargetID 			=	-1;
	m_bFrontView 			=	false;
	m_bShowMenulevel 		= 	false;
	m_bQuickStart 			=	false;
	m_bTargetRotate		=	false;
	m_strQuickStartAddress = "127.0.0.1:4242";


	m_strLoginName			= "player";
   m_strLoginPW 			= "topsecret";

   RegisterVariable("ap_loginname", 		 	&m_strLoginName,				CSYS_STRING);
   RegisterVariable("ap_loginpw", 			 	&m_strLoginPW,					CSYS_STRING);
	RegisterVariable("ap_showmenulevel", 	 	&m_bShowMenulevel,			CSYS_BOOL);
	RegisterVariable("ap_quickstart",		 	&m_bQuickStart,				CSYS_BOOL);
	RegisterVariable("ap_quickstartadress",	&m_strQuickStartAddress,	CSYS_STRING);
	RegisterVariable("ap_targetrotate",			&m_bTargetRotate,				CSYS_BOOL);

	m_bGuiCapture = false;
	m_iNumBuffIcons = 0;
	
} 
 
void MistClient::OnInit() 
{
	//m_pkZFVFileSystem->AddRootPath( string("../datafiles/dm") ,"/data");	
	m_pkZFVFileSystem->AddRootPath( string("../datafiles/mistlands") ,"/data");

	//register commands
	Register_Cmd("say",			FID_SAY);
	Register_Cmd("playerlist",	FID_PLAYERLIST);
	Register_Cmd("killme",		FID_KILLME);

	//create pointtext manager
	m_pkPointText = new PointText();
			
	
	//initiate our mainview camera
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),85,1.333,0.25,250);	
	m_pkCamera->SetName("Main camera");
	m_pkZeroFps->AddRenderCamera(m_pkCamera);

	//register property bï¿½
	RegisterPropertys();

	//register resources
	RegisterResources();
	
	//grab input
	m_pkInput->ToggleGrab(true);
	
	//setup referense sound distance
	//m_pkAudioSys->SetReferensDistance(0.25);
	
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
   SetTitle("MistClient - Banka och slå");
	
	//set client in server mode to show gui etc
	m_pkZeroFps->StartServer(true,false);

	// create gui for mistlands
	SetupGUI();
	
	//run autoexec script
	if(!m_pkIni->ExecuteCommands("mistclient_autoexec.ini"))
		m_pkConsole->Printf("No game_autoexec.ini.ini found");	
	
// 	//test
// 	if(m_bShowMenulevel)
// 	{
// 		m_pkEntityManager->SetWorldDir("clienttemp");
// 		if(m_pkEntityManager->LoadWorld("../datafiles/mistlands/menulevel"))
// 		{
// 			m_pkEntityManager->SetTrackerLos(50);
// 			
// 			Entity* pkEnt = m_pkEntityManager->CreateEntity();
// 			pkEnt->SetParent(m_pkEntityManager->GetWorldEntity());
// 			
// 			pkEnt->AddProperty("P_Track");
// 			P_Enviroment* pkEnv = (P_Enviroment*)pkEnt->AddProperty("P_Enviroment");	
// 			pkEnv->LoadEnviroment("data/enviroments/menu.env");
// 			pkEnv->SetEnable(true);	
// 		}
// 		else
// 			cout<<"WARNING: could not find menulevel"<<endl;	
// 	}
// 	else
// 	{
// 		GetWnd("MLStartWnd")->GetSkin()->m_bTransparent = false;
// 	}

	GetWnd("MLStartWnd")->GetSkin()->m_bTransparent = false;
	
	
	if(m_bQuickStart)
	{
		g_kMistClient.m_pkZeroFps->StartClient(m_strLoginName, m_strLoginPW, m_strQuickStartAddress);		
	}

//  	if(m_pkIni->GetIntValue("ZFAudioSystem", "a_enablesound") == 0 && 
//  		m_pkIni->GetIntValue("ZFAudioSystem", "a_enablemusic") == 0)
//  		m_pkAudioSys->SetMainVolume(0); // tempgrej för att stänga av all audio, finns inget vettigt sett för tillfället

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
	//draw current target marker
	if(m_iTargetID != -1)
		DrawTargetMarker();

	//draw under cursor marker
	if(m_iPickedEntityID != -1)
		if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iPickedEntityID))
			if(pkEnt->GetProperty("P_Ml") || pkEnt->GetProperty("P_CharacterProperty") || pkEnt->GetProperty("P_Item") ) 		
				DrawMouseOverMarker(pkEnt->GetWorldPosV(),pkEnt->GetRadius());
		
	m_pkPointText->Draw();
		
// 	Vector3 kPos = m_pkCamera->GetPos() + Get3DMouseDir(true);
// 	m_pkRender->Line(kPos-Vector3(1,0,0),kPos+Vector3(1,0,0));
// 	m_pkRender->Line(kPos-Vector3(0,1,0),kPos+Vector3(0,1,0));	
// 	m_pkRender->Line(kPos-Vector3(0,0,1),kPos+Vector3(0,0,1));			
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

	UpdateBuffIconList();

// 	if(m_iCharacterID == -1) 
// 	{
// 		NetPacket kNp;				
// 		kNp.Clear();
// 		kNp.Write((char) MLNM_CS_REQ_CHARACTERID);
// 		kNp.TargetSetClient(0);
// 		SendAppMessage(&kNp);	
// 	}

	// Set Color here.


}

void MistClient::OnHud(void) 
{
// 	if(m_iCharacterID != -1)
// 		if(!m_bGuiCapture)
// 			if(!m_bFrontView)
// 				DrawCrossHair();

 	if(m_bDead)
	{
 		DrawHUDEffect(HUD_DEAD);			
		DrawHUDEffect(HUD_FOG);			
	}
}

void MistClient::DrawHUDEffect(int iHUDEffect)
{
	static ZMaterial* pkWater = NULL;
	if(!pkWater)
	{
		pkWater = new ZMaterial;
				
		pkWater->m_bTextureOffset = true;
		pkWater->m_faTextureOffset[0] = 0.25;
		pkWater->m_faTextureOffset[1] = 0;
		
		pkWater->GetPass(0)->m_kTUs[0]->SetRes("data/textures/water.tga");	
		pkWater->GetPass(0)->m_kTUs[1]->SetRes("data/textures/water.tga");	
		pkWater->GetPass(0)->m_bLighting = 		false;
		pkWater->GetPass(0)->m_bFog = 			false;	
		pkWater->GetPass(0)->m_iPolygonModeFront = FILL_POLYGON;		
				
		pkWater->GetPass(0)->m_bDepthMask = false;
		pkWater->GetPass(0)->m_bBlend = true;
		pkWater->GetPass(0)->m_iBlendSrc = SRC_ALPHA_BLEND_SRC;
		pkWater->GetPass(0)->m_iBlendDst = ONE_MINUS_SRC_ALPHA_BLEND_DST;


	}

	static ZMaterial* pkDead = NULL;
	if(!pkDead)
	{
		pkDead = new ZMaterial;

		pkDead->GetPass(0)->m_kTUs[0]->SetRes("data/textures/dead-color.tga");	
		pkDead->GetPass(0)->m_bLighting = 	false;
		pkDead->GetPass(0)->m_bFog = 			false;	
		pkDead->GetPass(0)->m_iPolygonModeFront = FILL_POLYGON;		
				
		pkDead->GetPass(0)->m_bDepthMask = false;
		pkDead->GetPass(0)->m_bBlend = true;
 		pkDead->GetPass(0)->m_iBlendSrc = ONE_MINUS_DST_COLOR_BLEND_SRC;
 		pkDead->GetPass(0)->m_iBlendDst = ONE_MINUS_SRC_COLOR_BLEND_DST;

	}	
	
		static ZMaterial* pkDeadFog = NULL;
	if(!pkDeadFog)
	{
		pkDeadFog = new ZMaterial;

 		pkDeadFog->m_bTextureOffset = true;
 		pkDeadFog->m_faTextureOffset[0] = 0.1;
 		pkDeadFog->m_faTextureOffset[1] = 0;

		pkDeadFog->GetPass(0)->m_kTUs[0]->SetRes("data/textures/dead.tga");	
		pkDeadFog->GetPass(0)->m_kTUs[1]->SetRes("data/textures/dead.tga");	
		pkDeadFog->GetPass(0)->m_bLighting = 	false;
		pkDeadFog->GetPass(0)->m_bFog = 			false;	
		pkDeadFog->GetPass(0)->m_iPolygonModeFront = FILL_POLYGON;		
				
		pkDeadFog->GetPass(0)->m_bDepthMask = false;
		pkDeadFog->GetPass(0)->m_bBlend = true;
 		pkDeadFog->GetPass(0)->m_iBlendSrc = SRC_ALPHA_BLEND_SRC;
 		pkDeadFog->GetPass(0)->m_iBlendDst = ONE_MINUS_SRC_COLOR_BLEND_DST;


	}	
	
	switch(iHUDEffect)
	{
		case HUD_WATER:
			m_pkZShaderSystem->BindMaterial(pkWater);
			break;
		case HUD_DEAD:
			m_pkZShaderSystem->BindMaterial(pkDead);
			break;
	
		case HUD_FOG:
			m_pkZShaderSystem->BindMaterial(pkDeadFog);
			break;
	
	}
	
	static float fV[12] = {	-1.2	,1	,-1,
									-1.2	,-1,-1,
									1.2	,-1,-1,
									1.2	,1	,-1};
	
	static float fUV[8] = {	0,0,
									0,1,
									1,1,
									1,0};
										
	//add geometry
	m_pkZShaderSystem->ResetPointers();
	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,fV);
	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,fUV);
	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER1,fUV);
	
	m_pkZShaderSystem->SetNrOfVertexs(4);
// 	m_pkZShaderSystem->ClearGeometry();
// 	m_pkZShaderSystem->AddQuadV(	Vector3(-1.2,1,-1),Vector3(-1.2,-1,-1),
// 											Vector3(1.2,-1,-1),Vector3(1.2,1,-1));
// 	m_pkZShaderSystem->AddQuadUV(	Vector2(0,0)	,	Vector2(0,1),
// 											Vector2(1,1)	,	Vector2(1,0));
	
	//draw pointer
	m_pkZShaderSystem->DrawArray(QUADS_MODE);
	
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
		//pkNormalPointer->GetPass(0)->m_bAlphaTest = true;
				
		//blending is much nicer thou =)
		pkNormalPointer->GetPass(0)->m_bDepthMask = false;
		pkNormalPointer->GetPass(0)->m_bBlend = true;
		pkNormalPointer->GetPass(0)->m_iBlendSrc = SRC_ALPHA_BLEND_SRC;
		pkNormalPointer->GetPass(0)->m_iBlendDst = ONE_MINUS_SRC_ALPHA_BLEND_DST;
	}

	static ZMaterial* pkActivePointer = NULL;
	if(!pkActivePointer)
	{
		pkActivePointer = new ZMaterial;
		pkActivePointer->GetPass(0)->m_kTUs[0]->SetRes("data/textures/crosshair-active.tga");	
		pkActivePointer->GetPass(0)->m_bLighting = 	false;
		pkActivePointer->GetPass(0)->m_bFog = 			false;	
		pkActivePointer->GetPass(0)->m_iPolygonModeFront = FILL_POLYGON;
		//pkActivePointer->GetPass(0)->m_bAlphaTest = true;
		
		//blending is much nicer thou =)
		pkActivePointer->GetPass(0)->m_bDepthMask = false;
		pkActivePointer->GetPass(0)->m_bBlend = true;
		pkActivePointer->GetPass(0)->m_iBlendSrc = SRC_ALPHA_BLEND_SRC;
		pkActivePointer->GetPass(0)->m_iBlendDst = ONE_MINUS_SRC_ALPHA_BLEND_DST;		
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
		if(pkEnt->GetProperty("P_Ml") || pkEnt->GetProperty("P_CharacterProperty"))
			bActive = true;
			
	//set material					
	if(bActive)
		m_pkZShaderSystem->BindMaterial(pkActivePointer);
	else
		m_pkZShaderSystem->BindMaterial(pkNormalPointer);
		
	//draw pointer
	m_pkZShaderSystem->DrawGeometry(QUADS_MODE);
}

void MistClient::DrawMouseOverMarker(const Vector3& kPos,float fSize)
{
	static ZMaterial* pkMarker = NULL;
	if(!pkMarker)
	{
		pkMarker = new ZMaterial;
		pkMarker->GetPass(0)->m_kTUs[0]->SetRes("data/textures/enemymarker.tga");	
		pkMarker->GetPass(0)->m_bLighting = 	false;
		pkMarker->GetPass(0)->m_bFog = 			false;	
		pkMarker->GetPass(0)->m_iPolygonModeFront = FILL_POLYGON;		
				
		//blending is much nicer thou =)
		pkMarker->GetPass(0)->m_bDepthMask = false;
		pkMarker->GetPass(0)->m_bBlend = true;
		pkMarker->GetPass(0)->m_iBlendSrc = SRC_ALPHA_BLEND_SRC;
		pkMarker->GetPass(0)->m_iBlendDst = ONE_BLEND_DST;//ONE_MINUS_SRC_ALPHA_BLEND_DST;
	}

	m_pkRender->DrawBillboardQuad(m_pkZeroFps->GetCam()->GetRotM(),kPos,fSize*2.0,pkMarker);

}

void MistClient::DrawTargetMarker()
{
	static ZMaterial* pkEnemyMarker = NULL;
	if(!pkEnemyMarker)
	{
		pkEnemyMarker = new ZMaterial;
		pkEnemyMarker->GetPass(0)->m_kTUs[0]->SetRes("data/textures/enemymarker.tga");	
		pkEnemyMarker->GetPass(0)->m_bLighting = 	false;
		pkEnemyMarker->GetPass(0)->m_bFog = 			false;	
		pkEnemyMarker->GetPass(0)->m_iPolygonModeFront = FILL_POLYGON;		
				
		//blending is much nicer thou =)
		pkEnemyMarker->GetPass(0)->m_bDepthMask = false;
		pkEnemyMarker->GetPass(0)->m_bBlend = true;
		pkEnemyMarker->GetPass(0)->m_iBlendSrc = SRC_ALPHA_BLEND_SRC;
		pkEnemyMarker->GetPass(0)->m_iBlendDst = ONE_BLEND_DST;//ONE_MINUS_SRC_ALPHA_BLEND_DST;
	}

	static ZMaterial* pkFriendMarker = NULL;
	if(!pkFriendMarker)
	{
		pkFriendMarker = new ZMaterial;
		pkFriendMarker->GetPass(0)->m_kTUs[0]->SetRes("data/textures/friendmarker.tga");	
		pkFriendMarker->GetPass(0)->m_bLighting = 	false;
		pkFriendMarker->GetPass(0)->m_bFog = 			false;	
		pkFriendMarker->GetPass(0)->m_iPolygonModeFront = FILL_POLYGON;		
				
		//blending is much nicer thou =)
		pkFriendMarker->GetPass(0)->m_bDepthMask = false;
		pkFriendMarker->GetPass(0)->m_bBlend = true;
		pkFriendMarker->GetPass(0)->m_iBlendSrc = SRC_ALPHA_BLEND_SRC;
		pkFriendMarker->GetPass(0)->m_iBlendDst = ONE_MINUS_SRC_ALPHA_BLEND_DST;
	}
	
	
	
	if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iTargetID))
	{
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)pkEnt->GetProperty("P_CharacterProperty"))
		{
			bool bFriend = false;
			
			//check if enemy
			if(P_CharacterProperty* pkPlayer = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(m_iCharacterID,"P_CharacterProperty"))
			{
				if(pkPlayer->GetFaction() == pkCP->GetFaction())
					bFriend = true;	
			}	
		
		
			Vector3 kPos = pkEnt->GetIWorldPosV();
		

			if(bFriend)
				m_pkZShaderSystem->BindMaterial(pkFriendMarker);
			else
				m_pkZShaderSystem->BindMaterial(pkEnemyMarker);
			
					
			
			m_pkZShaderSystem->ClearGeometry();
			m_pkZShaderSystem->AddQuadV(	Vector3(-0.5,0,-0.5),Vector3(-0.5,0,0.5),
													Vector3(0.5,0,0.5),	Vector3(0.5,0,-0.5));
			m_pkZShaderSystem->AddQuadUV(	Vector2(0,0)	,	Vector2(0,1),
													Vector2(1,1)	,	Vector2(1,0));	
		
				
			m_pkZShaderSystem->MatrixPush();
			
			m_pkZShaderSystem->MatrixTranslate(kPos + Vector3(0,-pkCP->GetLegLength()+0.2,0));
			m_pkZShaderSystem->MatrixScale(pkCP->GetMarkerSize());
			m_pkZShaderSystem->MatrixRotate(Vector3(0,m_pkZeroFps->GetEngineTime()*10,0));
			
			//draw pointer			
			m_pkZShaderSystem->DrawGeometry(QUADS_MODE);											
			
 			m_pkZShaderSystem->MatrixRotate(Vector3(0,-m_pkZeroFps->GetEngineTime()*10 * 2,0));
 			m_pkZShaderSystem->DrawGeometry(QUADS_MODE);											
			
			m_pkZShaderSystem->MatrixPop();
		}
	}

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

	//gui capture toggle
	if(m_pkInputHandle->VKIsDown("togglegui") && !DelayCommand())
		SetGuiCapture(!m_bGuiCapture);
	
	SetGuiCapture(!m_pkInputHandle->Pressed(MOUSERIGHT));
		
	//toggle combat mode
	if(m_pkInputHandle->VKIsDown("togglecombatmode") && !DelayCommand())
		SendCombatMode(!m_bCombatMode);
			
	//toggle inventory
	if(m_pkInputHandle->VKIsDown("inventory") && !DelayCommand())
	{			
		if(m_pkInventoryDlg->IsVisible())
			m_pkInventoryDlg->Close(); 
		else
			RequestOpenInventory();
	}

	//toggle eqipment
	if(m_pkInputHandle->VKIsDown("eqipment") && !DelayCommand())
	{			
		if(m_pkEquipmentDlg->IsVisible())
			m_pkEquipmentDlg->Close(); 
		else
		{
			SendRequestOpenEquipment();
		}
	}

	//toggle chat
	if(m_pkInputHandle->VKIsDown("chat") && !DelayCommand())
	{			
		bool bOpen = !IsWndVisible("ChatDlgMainWnd");
		SetText("SayTextbox", "");
		ToogleChatWnd(bOpen);
	}
			

	//update picked object	
	if(Entity* pkPickedEnt = GetTargetObject())	
		m_iPickedEntityID = pkPickedEnt->GetEntityID();
	else
		m_iPickedEntityID = -1;
	
		


	//fireball test
	if(m_pkInputHandle->Pressed(KEY_1))
		if(!DelayCommand() )
			SendAddSkillToCombatQueue("skill-fireball.lua",m_iTargetID);
// 			SendUseSkill("skill-fireball.lua",m_iTargetID,Vector3(1,2,3),Vector3(10,20,30));		
	// speed	
	if(m_pkInputHandle->Pressed(KEY_2))
		if(!DelayCommand() )
// 			SendAddSkillToCombatQueue("skill-fireball.lua",m_iTargetID);
 			SendUseSkill("skill-speed.lua",m_iTargetID,Vector3(1,2,3),Vector3(10,20,30));		

	if(m_pkInputHandle->Pressed(KEY_3))
		if(!DelayCommand() )
// 			SendAddSkillToCombatQueue("skill-fireball.lua",m_iTargetID);
			SendUseSkill("skill-heal.lua",m_iTargetID,Vector3(1,2,3),Vector3(10,20,30));		

	if(m_pkInputHandle->Pressed(KEY_4))
		if(!DelayCommand() )
// 			SendUseSkill("skill-basic_attack.lua",m_iTargetID,Vector3(1,2,3),Vector3(10,20,30));		
			SendAddSkillToCombatQueue("skill-basic_attack.lua",m_iTargetID);		
				
	if(m_pkInputHandle->Pressed(KEY_5))
		if(!DelayCommand() )
// 			SendUseSkill("skill-bow.lua",m_iTargetID,Vector3(1,2,3),Vector3(10,20,30));		
			SendAddSkillToCombatQueue("skill-bow.lua",m_iTargetID);		

	if(m_pkInputHandle->Pressed(KEY_6))
		if(!DelayCommand() )
			SendUseSkill("skill-resurrect.lua",m_iTargetID,Vector3(1,2,3),Vector3(10,20,30));		

/*	//list actions
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
	}		*/	
			
	//perform the first action in the action list or pickup
	if( m_pkInputHandle->VKIsDown("use") )
	{
		if(!DelayCommand() && m_bGuiCapture )
		{			
			if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iPickedEntityID))
			{					
				//remove current target
				int iOldTarget = m_iTargetID;
				m_iTargetID = -1;

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
					vector<string>	kActions;
					pkMl->GetActions(kActions);
				
					if(kActions.size() == 1)
					{
						cout<<"performing first action"<<endl;						
						SendAction(m_iPickedEntityID,kActions[0]);
					}
					else if(kActions.size() > 1)
					{
						if(!m_pkActionDlg->IsOpen())
						{
							cout<<"opening action list"<<endl;			
							m_pkActionDlg->SetEntity(m_iPickedEntityID);							
							m_pkActionDlg->Open();
						}
					}

				}
				else
				//is it a character?
				if(P_CharacterProperty* pkCP = (P_CharacterProperty*)pkEnt->GetProperty("P_CharacterProperty"))
				{
					if(m_iPickedEntityID != m_iTargetID)
					{
						m_iTargetID = m_iPickedEntityID;
						SendSetTarget(	m_iTargetID );
					}
				}
				
				
				//send new target if it has changed
				if(m_iTargetID != iOldTarget)
					SendSetTarget(	m_iTargetID );
			}
			else
			{
				//remove current traget if nothing was picked
				m_iTargetID = -1;			
			}
		}	
	}	
		
	
	//respawn knapp, i brist på gui
	if(m_bDead && m_pkInputHandle->Pressed(KEY_BACKSPACE) && !DelayCommand())
		SendRespawnRequest();
	
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
		
			//CAMERA ROTATION
			
			//captured mouse pointer?
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
			
			//rotate camera by keyboard
			if(m_pkInputHandle->VKIsDown("rotate_left"))		pkCam->Set3PYAngle(pkCam->Get3PYAngle() + m_pkZeroFps->GetFrameTime()*150);
			if(m_pkInputHandle->VKIsDown("rotate_right"))	pkCam->Set3PYAngle(pkCam->Get3PYAngle() - m_pkZeroFps->GetFrameTime()*150);			
			
			
			//rotate towards target
			if(m_bTargetRotate && m_iTargetID != -1)
			{
				if(Entity* pkTarget = m_pkEntityManager->GetEntityByID(m_iTargetID))
				{
					if(Entity* pkPlayer = m_pkEntityManager->GetEntityByID(m_iCharacterID))
					{
					
						static Vector3 kDir;
						static float fAngle;
					
						kDir = pkTarget->GetIWorldPosV() - pkPlayer->GetIWorldPosV();
						kDir.Normalize();
						kDir.y = 0;
						
						fAngle = Vector3(0,0,1).Angle(kDir);
						fAngle = -RadToDeg(fAngle);
						
						if(kDir.x > 0)
							fAngle = fabs(fAngle);	
					
						pkCam->Set3PYAngle(fAngle);					
					}
				}
			}
			
			// ///////////////////
			
			
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
			if(!m_bDead)
			{
				Matrix4 kRot;
				kRot.Identity();
				kRot.Rotate(0,pkCam->Get3PYAngle(),0);
				kRot.Transponse();				
				pkCharacter->SetLocalRotM(kRot);			
			}
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
		//disable rotation from server
		pkEnt->SetNetIgnoreFlag(NETUPDATEFLAG_ROT,true);
	
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
		
		//enable enviroment 
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

		case MLNM_SC_POINTTEXT:
		{
			string strText;
			Vector3 kPos;
			Vector3 kVel;
			float fTTL;
			int iType;
			
			pkNetMessage->Read_Str(strText);	
			pkNetMessage->Read(kPos);
			pkNetMessage->Read(kVel);
			pkNetMessage->Read(fTTL);
			pkNetMessage->Read(iType);		
		
			m_pkPointText->AddText(strText,kPos,kVel,fTTL,iType);
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

		case MLNM_SC_DEAD:
		{
			cout<<"YOU ARE DEAD"<<endl;
		
			m_bDead = true;
			
			break;
		}		

		case MLNM_SC_ALIVE:
		{
			cout<<"YOU ARE ALIVE AGAIN"<<endl;
		
			m_bDead = false;
			
			break;
		}		
		
				
		case MLNM_SC_ITEMINFO:
		{
			ITEM_INFO kInfo;
			
			pkNetMessage->Read_Str(kInfo.strInfo);
			pkNetMessage->Read_Str(kInfo.strImage);
			pkNetMessage->Read(kInfo.m_fWeight);
			pkNetMessage->Read(kInfo.m_iValue);
		
			//item stats
			vector<Stat>	m_kStats;			
			int iNr;
			Stat kTemp("",0,0);
			
			kInfo.strInfo += string("\n");
			
			pkNetMessage->Read(iNr);
			for(int i = 0;i<iNr;i++)
			{
				pkNetMessage->Read_Str(kTemp.m_strName);
				pkNetMessage->Read(kTemp.m_fValue);
				pkNetMessage->Read(kTemp.m_fMod);
				
				m_kStats.push_back(kTemp);
				
				kInfo.strInfo += string("\n");
				kInfo.strInfo += kTemp.m_strName + string(" ") + IntToString(int(kTemp.m_fValue+kTemp.m_fMod));
			}					
			
			cout<<"-- Got item info --"<<endl;
			cout<<"IMAGE:"<<kInfo.strImage<<endl;			
			cout<<"TEXT:"<<kInfo.strInfo<<endl;		
			cout<<"STATS:"<<endl;
			for(int i =0;i<m_kStats.size();i++)
				cout<<m_kStats[i].m_strName<<"  "<<m_kStats[i].m_fValue<<" "<<m_kStats[i].m_fMod<<endl;
			
			
			m_pkInventoryDlg->OpenItemInfoWnd(true, kInfo);
			
			break;
		}
				
		case MLNM_SC_CHARACTERSTATS:
		{
			float	fStamina;
			float fStaminaMax;
			float	fHealth;
			float fHealthMax;
			float	fMana;
			float fManaMax;
			
			pkNetMessage->Read(fStamina);
			pkNetMessage->Read(fStaminaMax);

			pkNetMessage->Read(fHealth);
			pkNetMessage->Read(fHealthMax);

			pkNetMessage->Read(fMana);
			pkNetMessage->Read(fManaMax);
									
			//cout<<"stamina:"<<fStamina<<endl;
			
			//update stamina bar
			if(ZGuiProgressbar* pkPB = (ZGuiProgressbar*)GetWnd("ManaProgressbar"))
			{
				pkPB->SetRange(0,int(fStaminaMax));
				pkPB->SetPos(int(fStamina));				
			}

			if(ZGuiProgressbar* pkPB = (ZGuiProgressbar*)GetWnd("LifeProgressbar"))
			{
				pkPB->SetRange(0,int(fHealthMax));
				pkPB->SetPos(int(fHealth));				
			}
						
			break;		
		}
				
		case MLNM_SC_BUFFLIST:
		{
			cout<<"got buff list from server"<<endl;
		
			string strName;
			string strIcon;
			float  fTimeout;
			float  fTimeLeft;
			char   cType;

			BUFF_ICON_INFO kInfo;
			vector<BUFF_ICON_INFO> kInfoVector;
			
			pkNetMessage->Read_Str(strName);
			while(!strName.empty())
			{
				pkNetMessage->Read_Str(strIcon);
				pkNetMessage->Read(fTimeout);
				pkNetMessage->Read(fTimeLeft);
				pkNetMessage->Read(cType);							
			
				kInfo.m_strName = strName;
				kInfo.m_strIcon = strIcon;
				kInfo.m_fTimeout = fTimeout;
				kInfo.m_fTimeLeft = fTimeLeft;
				kInfo.m_cType = cType;
				kInfoVector.push_back(kInfo);

				pkNetMessage->Read_Str(strName);							
			}

			RebuildBuffIconList(&kInfoVector);
						
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
	m_iCharacterID		=	-1;
	m_iTargetID 		=	-1;
	m_bFrontView		=	false;
	m_bDead				=	false;
	m_bCombatMode		=	false;
}

void MistClient::OnClientConnected() 
{
	m_pkConsole->Printf("Successfully connected to server");

	ShowWnd("CharGen_SelectCharWnd", true, true, true);

	m_pkGui->PlaceWndFrontBack(g_kMistClient.GetWnd("CharGen_SelectCharWnd"), true);
	m_pkGui->SetCaptureToWnd(g_kMistClient.GetWnd("CharGen_SelectCharWnd"));	

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
   
	// Load start screen if where ingame, 
	if(m_iCharacterID != -1)
		LoadStartScreenGui(true);
	else
		ShowWnd("CharGen_SelectCharWnd", false);

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

void MistClient::SendRespawnRequest()
{
	cout<<"respaawn me"<<endl;
	
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_RESPAWN_IN_TOWN);
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);		
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
		
		if(kObjects[i]->IsZone())
			continue;
		if(kObjects[i]->GetName() == "ZoneObject")
			continue;
			
						
		if(kObjects[i]->GetName() == "A t_serverinfo.lua")
			continue;		
	
// 		if(kObjects[i]->GetType() == "Entity")
// 			continue;		
		
		if(kObjects[i]->GetEntityID() == m_iCharacterID)		//dont pick self
			continue;
		
		//-------------
		
		
		
		//get mad property and do a linetest		
		if(P_Mad* mp = (P_Mad*)kObjects[i]->GetProperty("P_Mad"))
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
 			//dir.Set(x*xp,-y*yp,-2.15);			//for 75 deg fov
 			dir.Set(x*xp,-y*yp,-1.64);				//for 85 deg fov
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
	if(m_bGuiCapture == bCapture)
		return;

	static float fMouseX,fMouseY;
	
	if(bCapture == true)
	{
		m_pkInputHandle->SetCursorInputPos(fMouseX,fMouseY);
		m_pkGui->ShowCursor(true);
	}
	else
	{
		m_pkInputHandle->MouseXY(fMouseX,fMouseY);
		m_pkGui->ShowCursor(false);
	}	
	
	m_bGuiCapture = bCapture;
	
	
// 	m_bGuiCapture = bCapture;
// 	
// 	if(m_bGuiCapture == true)
// 	{
// 		//g_kMistClient.m_pkGui->m_bHandledMouse = false;
//  		if(bMoveCursorToCenter)
//  			m_pkInputHandle->SetCursorInputPos(m_pkRender->GetWidth()/2,m_pkRender->GetHeight()/2);				
// 
// 		m_pkGui->ShowCursor(true);
// 	}
// 	else
// 	{
// 		m_pkGui->ShowCursor(false);
// 	}
}

void MistClient::SendAddSkillToCombatQueue(const string& strSkill,int iTargetID)
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_ADDSKILLTOCOMBATQUEUE);
	
	kNp.Write_Str(strSkill);
	kNp.Write(iTargetID);
	
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);		
}

void MistClient::SendCombatMode(bool bCombatMode)
{
	if(bCombatMode)
		AddStringToChatBox("Entering combat mode");
	else
		AddStringToChatBox("Leaving combat mode");

	m_bCombatMode = bCombatMode;
	

	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_COMBATMODE);
	
	kNp.Write(bCombatMode);
	
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);	
}

void MistClient::SendSetTarget(int iTargetID)
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_SET_TARGET);
	
	kNp.Write(iTargetID);
	
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);	
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

void MistClient::SendRequestOpenEquipment()
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

void MistClient::SendUseSkill(const string& strSkill,int iTargetID,const Vector3& kPos,const Vector3& kDir)
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_USESKILL);
		
	kNp.Write_Str(strSkill);
	kNp.Write(iTargetID);
	kNp.Write(kPos);
	kNp.Write(kDir);
	
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);	

}

void MistClient::RequestItemInfo(int iItemID)
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_REQ_ITEMINFO);
	kNp.Write(iItemID);
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);			
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

