/**
	\defgroup MistClient MistClient
	\ingroup MistLand

  MistClient is the Client of the game MistLands.
*/

#ifndef _DONT_MAIN					// <- OBS! Flytta inte p?denna. M?te ligga i
	#define _MAINAPPLICATION_		// just denna fil och inte p?flera st?len.
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
#include "skillbar.h"
#include "../mcommon/mainmcommon.h"
#include "../zerofpsv2/basic/math.h"

MistClient g_kMistClient("MistClient",0,0,0);

bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) ;

void GuiMsgIngameScreen( string strMainWnd, string	strController,	
								 unsigned int msg, int numparms,	void *params )	;

MistClient::MistClient(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	g_ZFObjSys.SetPreLogName("mistclient2");
	
	m_iPickedEntityID 	= 	-1;
	m_fDelayTime  			=	0;

	m_iCharacterID 		=	-1;
	m_iTargetID 			=	-1;
	m_bFrontView 			=	false;
	m_bLoginKeepAlive    =  false;
	m_fPingDelay			=  2;
	m_pkMakerEntity		= NULL;

	m_strMenuMusic			=	"music/menu_music.ogg";

	Register_Cmd("msref", FID_MSREFRESH);
	Register_Cmd("skill", FID_SETSKILLBAR);

	m_kstrLoginName.Register(this, "ap_loginname", "player");
	m_kstrLoginPW.Register(this, "ap_loginpw", "topsecret");
	m_kbShowMenulevel.Register(this, "ap_showmenulevel", "0");
	m_kbQuickStart.Register(this, "ap_quickstart", "0");
	m_kstrQuickStartAddress.Register(this, "ap_quickstartadress", "127.0.0.1:4242");
	m_kbTargetRotate.Register(this, "ap_targetrotate", "0");
	m_kbShowLagMeeter.Register(this, "ap_lagmeeter", "1");
	m_kbBloom.Register(this, "ap_bloom", "0");

	
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
	Register_Cmd("dm",			FID_DMC);
	Register_Cmd("pc",			FID_PCMD);

	//create pointtext manager
	m_pkPointText = new PointText();
			
	
	//initiate our mainview camera
// 	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),85,1.333,0.1,250);	
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),75,1.333,0.1,250);	
	m_pkCamera->SetName("main");
	m_pkZeroFps->AddRenderCamera(m_pkCamera);

	//register property b?
	RegisterPropertys();

	//register resources
	RegisterResources();
	
	//grab input
	m_pkInput->ToggleGrab(true);


	//register emote names
	RegisterEmotes();
	
   // init gui script intreface
	GuiAppLua::Init(&g_kMistClient, m_pkScript);

	//set window title		
   SetTitle("MistClient - Banka och sl�");
	
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

// 	LoadStartScreenGui(true);

 	GetWnd("MLStartWnd")->GetSkin()->m_bTransparent = false;

	//set menu music
	m_pkAudioSys->SetMusic(m_strMenuMusic);
	
	if(m_kbQuickStart.GetBool())
	{
		g_kMistClient.m_pkZeroFps->StartClient(m_kstrLoginName.GetString(), m_kstrLoginPW.GetString(), m_kstrQuickStartAddress.GetString());		
	}
	m_pkNetwork->ClientStart();


}

void MistClient::RunCommand(int cmdid, const ConCommandLine* kCommand)
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

		case FID_PCMD:
		{
			string strMsg;
			for(int i = 3;i<kCommand->m_strFullCommand.size();i++)
				strMsg.push_back(kCommand->m_strFullCommand[i]);

			cout << "Player Command: " << strMsg  << endl;

			NetPacket kNp;			
			kNp.Write((char) MLNM_CS_PCMD);
			kNp.Write_Str(strMsg);
			kNp.TargetSetClient(0);
			SendAppMessage(&kNp);	

			break;
		}	

		case FID_DMC:
		{
			string strMsg;
			for(int i = 3;i<kCommand->m_strFullCommand.size();i++)
				strMsg.push_back(kCommand->m_strFullCommand[i]);

			cout << "Dungeon Master Command: " << strMsg  << endl;

			NetPacket kNp;			
			kNp.Write((char) MLNM_CS_DMC);
			kNp.Write_Str(strMsg);
			kNp.TargetSetClient(0);
			SendAppMessage(&kNp);	

			break;
		}	

		
		case FID_MSREFRESH:
		{
			m_pkNetwork->MS_RequestServers();
			break;
		}	

		case FID_SETSKILLBAR:
		{
			if(kCommand->m_kSplitCommand.size() <= 2)
			{
				m_pkConsole->Printf("say [skill] [pos]");
				break;				
			}

			int iPos = atoi(kCommand->m_kSplitCommand[2].c_str());

			SendAddSkillToSkillbar(kCommand->m_kSplitCommand[1],iPos);
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
// 	int iEmoteStart = strMsg.find("/");
// 	
// 	if(iEmoteStart != -1)
// 	{
// 		int iEmoteEnd = strMsg.find(" ",iEmoteStart)-1;		
// 		//cout<<"line:"<<iEmoteStart<<" - "<<iEmoteEnd<<endl;		
// 		//cout<<"emote:"<<strMsg.substr(iEmoteStart+1,iEmoteEnd - iEmoteStart)<<"|"<<endl;
// 		
// 		SendTaunt(strMsg.substr(iEmoteStart+1,iEmoteEnd - iEmoteStart));
// 		
// 		string msg = strMsg.substr(0,iEmoteStart);
// 		if(strMsg.length() > iEmoteEnd+1)
// 		{
// 			msg+=strMsg.substr(iEmoteEnd+1);			
// 			//cout<<"msg:"<<msg<<"|"<<endl;
// 			//SendMessage(strMsg,MLCM_TALK,"");
// 		}
// 					
// 		if(!msg.empty())			
// 			SendMessage(msg,MLCM_TALK,""); 	
// 		return;
// 	}
// 		
	
	
	
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

void MistClient::GetRenderPackages(vector<RenderPackage*>&	kRenderPackages,const RenderState& kRenderState)
{
	//draw current target marker
	if(m_iTargetID != -1)
		AddTargetMarkerRP(kRenderPackages);
	

}

void MistClient::RenderInterface(void)
{	
	m_pkPointText->Draw();
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
				float fMultipel = Math::TWO_PI / 360.0f;	// (PI+PI)
				pkCompass->GetSkin()->m_fRotDegree = fMultipel * fAngle; 
			}
		}
	}

	UpdateBuffIconList();

	if(m_bLoginKeepAlive)
	{
		m_fPingDelay -= this->m_pkZeroFps->GetFrameTime();
		if(m_fPingDelay < 0.0)
		{
			m_pkNetwork->Ping();
			m_fPingDelay = 2;
		}
	}


 	//SetMouse pointer
	UpdateCursorImage();

	float fModFov = 0;
	if(m_bInWater)
		fModFov = sin(m_pkZeroFps->GetEngineTime()) * 10.0;
	m_pkCamera->SetFov(75 + fModFov);

}

void MistClient::UpdateCursorImage()
{
	if(m_pkGui->m_bHandledMouse)
		return;

	static int iFriendTex = g_kMistClient.LoadGuiTextureByRes("actionicon_equip.tga");
	static int iEnemyTex = g_kMistClient.LoadGuiTextureByRes("cursor_enemy.tga");
	static int iItemTex = g_kMistClient.LoadGuiTextureByRes("actionicon_backpack.tga");
	static int iUseTex = g_kMistClient.LoadGuiTextureByRes("use.tga");
	static int iNormalTex = g_kMistClient.LoadGuiTextureByRes("cursor_sword.tga");

	bool bHaveSet = false;
	string strCursorText = "";

 	if(m_iPickedEntityID != -1 && m_bGuiCapture)
 	{
 		if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iPickedEntityID))
 		{
 			if(P_CharacterProperty* pkChar = (P_CharacterProperty*)pkEnt->GetProperty("P_CharacterProperty"))
 			{
				if(P_CharacterProperty* pkPlayer = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(m_iCharacterID,"P_CharacterProperty"))
				{				
					if(pkPlayer->GetFaction() == pkChar->GetFaction())
						m_pkGui->SetCursorTexture(iFriendTex);
					else
						m_pkGui->SetCursorTexture(iEnemyTex);	
													
					bHaveSet = true;
				}
				
				strCursorText = pkChar->GetName();
			}	
 			else if(P_Item* pkItem = (P_Item*)pkEnt->GetProperty("P_Item"))
 			{
 				if(pkItem->GetEntity()->GetParent()->IsZone())
 				{
					m_pkGui->SetCursorTexture(iItemTex);
					bHaveSet = true;
					strCursorText = pkItem->GetName();
				}
 			}
 			else if(pkEnt->GetProperty("P_Ml"))
 			{
 				m_pkGui->SetCursorTexture(iUseTex);
 				bHaveSet = true;
 				strCursorText = pkEnt->GetName();
 				if(strCursorText == "A Entity")
 					strCursorText = "";
 			}
 		}
	}
	
	//cursor wnd
	static ZGuiWnd* pkWnd = NULL;
	static ZGuiLabel* pkLabel = NULL;
	if(!pkWnd)
	{
		pkWnd = CreateWnd( Wnd,(char*)"cursortextwnd",(char*)"",(char*)"",0,0, 100,20,0);
		pkLabel = (ZGuiLabel*)CreateWnd( Label,(char*)"cursortextlbl",(char*)"bla",pkWnd,0,0, 50,20,0);
		pkWnd->GetSkin()->m_unBorderSize = 1;
	}
	
	
	if(!bHaveSet)
		m_pkGui->SetCursorTexture(iNormalTex);
	
	if(strCursorText.empty())
	{
		pkWnd->Hide();	
	}		
	else
	{				 
		int x,y;
		x = m_pkGui->m_iMouseX;
		y = m_pkGui->m_iMouseY;
		 		 
		pkLabel->SetText((char*)strCursorText.c_str(),true);
		
		Rect kWnd = pkLabel->GetScreenRect();				
		pkWnd->Resize( kWnd.Width(),kWnd.Height());
		x -= pkWnd->GetScreenRect().Width() / 2;
		y -= pkWnd->GetScreenRect().Height() + 25;
		pkWnd->SetPos( x,y,true,true);	
	
		pkWnd->Show();	
	}
}

void MistClient::OnHud(void) 
{
// 	if(m_iCharacterID != -1)
// 		if(!m_bGuiCapture)
// 			if(!m_bFrontView)
// 				DrawCrossHair();


	if(m_pkZShaderSystem->SupportGLSLProgram())
	{
		static bool bLast = false;
		if(m_bDead != bLast)
		{
			bLast = m_bDead;			
			if(m_bDead)
			{
				m_pkCamera->SetBloomEnabled(false);
				m_pkCamera->SetFSSEnabled(true);
				m_pkCamera->SetFSSGLSLShader("#dead.frag");
			}
			else
			{
				m_pkCamera->SetFSSEnabled(false);	
			
			 	if(m_kbBloom.GetBool())
 					m_pkCamera->SetBloomEnabled(true); 					
			}
		}
	}
	else if(m_bDead)
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
		
		pkWater->GetPass(0)->m_pkTUs[0]->SetRes("water.tga");	
		pkWater->GetPass(0)->m_pkTUs[1]->SetRes("water.tga");	
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

		pkDead->GetPass(0)->m_pkTUs[0]->SetRes("dead-color.tga");	
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

		pkDeadFog->GetPass(0)->m_pkTUs[0]->SetRes("dead.tga");	
		pkDeadFog->GetPass(0)->m_pkTUs[1]->SetRes("dead.tga");	
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
		pkNormalPointer->GetPass(0)->m_pkTUs[0]->SetRes("crosshair.tga");	
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
		pkActivePointer->GetPass(0)->m_pkTUs[0]->SetRes("crosshair-active.tga");	
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



void MistClient::AddTargetMarkerRP(vector<RenderPackage*>& kRenderPackages)
{
	static ZMaterial* pkEnemyMarker = NULL;
	if(!pkEnemyMarker)
	{
		pkEnemyMarker = new ZMaterial;
		pkEnemyMarker->GetPass(0)->m_pkTUs[0]->SetRes("enemymarker.tga");	
		pkEnemyMarker->GetPass(0)->m_bLighting = 	false;
		pkEnemyMarker->GetPass(0)->m_bFog = 			false;	
		pkEnemyMarker->GetPass(0)->m_iPolygonModeFront = FILL_POLYGON;		
		pkEnemyMarker->GetPass(0)->m_iCullFace = CULL_FACE_NONE;
				
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
		pkFriendMarker->GetPass(0)->m_pkTUs[0]->SetRes("friendmarker.tga");	
		pkFriendMarker->GetPass(0)->m_bLighting = 	false;
		pkFriendMarker->GetPass(0)->m_bFog = 			false;	
		pkFriendMarker->GetPass(0)->m_iPolygonModeFront = FILL_POLYGON;		
		pkFriendMarker->GetPass(0)->m_iCullFace = CULL_FACE_NONE;
				
		//blending is much nicer thou =)
		pkFriendMarker->GetPass(0)->m_bDepthMask = false;
		pkFriendMarker->GetPass(0)->m_bBlend = true;
		pkFriendMarker->GetPass(0)->m_iBlendSrc = SRC_ALPHA_BLEND_SRC;
		pkFriendMarker->GetPass(0)->m_iBlendDst = ONE_MINUS_SRC_ALPHA_BLEND_DST;
	}

	static RenderPackage* pkRenderPackage1 = NULL;
	static RenderPackage* pkRenderPackage2 = NULL;
	
	//setup renderpackage
	if(!pkRenderPackage1)
	{
		pkRenderPackage1 = new RenderPackage;	
		pkRenderPackage1->m_kMeshData.m_kVertises.push_back(Vector3(-0.5,0,-0.5));
		pkRenderPackage1->m_kMeshData.m_kVertises.push_back(Vector3(-0.5,0,0.5));
		pkRenderPackage1->m_kMeshData.m_kVertises.push_back(Vector3(0.5,0,0.5));
		pkRenderPackage1->m_kMeshData.m_kVertises.push_back(Vector3(0.5,0,-0.5));		
		pkRenderPackage1->m_kMeshData.m_kTexture[0].push_back(Vector2(0,0));
		pkRenderPackage1->m_kMeshData.m_kTexture[0].push_back(Vector2(0,1));
		pkRenderPackage1->m_kMeshData.m_kTexture[0].push_back(Vector2(1,1));
		pkRenderPackage1->m_kMeshData.m_kTexture[0].push_back(Vector2(1,0));	
		pkRenderPackage1->m_kMeshData.m_iNrOfDataElements = 4;
		pkRenderPackage1->m_kMeshData.m_ePolygonMode = QUADS_MODE;
		
		pkRenderPackage2 = new RenderPackage;
		*pkRenderPackage2 = *pkRenderPackage1;
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
		
			//set material
			if(bFriend)
			{
				pkRenderPackage1->m_pkMaterial = pkFriendMarker;
				pkRenderPackage2->m_pkMaterial = pkFriendMarker;
			}
			else
			{
				pkRenderPackage1->m_pkMaterial = pkEnemyMarker;
				pkRenderPackage2->m_pkMaterial = pkEnemyMarker;
			}

			//transform
			pkRenderPackage1->m_kModelMatrix.Identity();		
 			pkRenderPackage1->m_kModelMatrix.Rotate(Vector3(0,m_pkZeroFps->GetEngineTime()*10,0));			
 			pkRenderPackage1->m_kModelMatrix.Scale(pkCP->GetMarkerSize() + 0.25*sin(m_pkZeroFps->GetEngineTime()*2.0) );			
			pkRenderPackage1->m_kModelMatrix.Translate(pkEnt->GetIWorldPosV() + Vector3(0,-pkCP->GetLegLength()+0.2,0));
				
			pkRenderPackage2->m_kModelMatrix.Identity();		
 			pkRenderPackage2->m_kModelMatrix.Rotate(Vector3(0,-m_pkZeroFps->GetEngineTime()*10,0));			
 			pkRenderPackage2->m_kModelMatrix.Scale(pkCP->GetMarkerSize() + 0.25*sin(m_pkZeroFps->GetEngineTime()*2.0+Math::PI));			
			pkRenderPackage2->m_kModelMatrix.Translate(pkEnt->GetIWorldPosV() + Vector3(0,-pkCP->GetLegLength()+0.2,0));
				
			kRenderPackages.push_back(pkRenderPackage1);
			kRenderPackages.push_back(pkRenderPackage2);
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
	
	SetGuiCapture(!m_pkInputHandle->VKIsDown("look"));
		
	//toggle combat mode
	if(m_pkInputHandle->VKIsDown("togglecombatmode") && !DelayCommand())
		SendCombatMode(!m_bCombatMode);
			
	//target closest
// 	if(m_pkInputHandle->VKIsDown("target_closest") && !DelayCommand())
// 	{
// 		m_iTargetID = GetClosestEnemy();
// 		SendSetTarget(m_iTargetID);
// 	}
			
	//toggle inventory
	if(m_pkInputHandle->VKIsDown("inventory") && !DelayCommand())
	{			
		if(m_pkInventoryDlg->IsVisible())
			m_pkInventoryDlg->Close(); 
		else
			RequestOpenInventory();
	}

	m_pkInput->PrintInputHandlers();
	m_pkZeroFps->DevPrintf("in", "m_bGuiCapture: %d", m_bGuiCapture);

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
	
		
	if(m_pkInputHandle->VKIsDown("sit"))
		if(!DelayCommand() )
			SendSit();

	//loot dead corps
	if(m_pkInputHandle->VKIsDown( "loot") && !DelayCommand())
	{
		if(m_pkInventoryDlg->IsVisible())
			m_pkInventoryDlg->Close(); 
			
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(m_iPickedEntityID,"P_CharacterProperty"))
		{	
			if(pkCP->IsDead())			
			{
				SendRequestContainer( pkCP->m_iInventory);
			}
		}
	}
			
			
	//perform the first action in the action list or pickup
	static float fUsePressed = -1;
	static bool bAutoAttack = false;
	if( m_pkInputHandle->VKIsDown("use") && m_bGuiCapture && !m_pkActionDlg->IsOpen())
	{	
		//not pressed before
		if(fUsePressed == -1)
		{
			fUsePressed = m_pkZeroFps->GetEngineTime();				
		}
		else if(m_pkZeroFps->GetEngineTime() - fUsePressed > 0.2)
		{
			//bring up action meny if thers any actions
			if(P_Ml* pkMl = (P_Ml*)m_pkEntityManager->GetPropertyFromEntityID(m_iPickedEntityID,"P_Ml"))
			{
				vector<string>	kActions;
				pkMl->GetActions(kActions);			
			
				if(!kActions.empty())
				{
					fUsePressed = -1;
					m_pkActionDlg->SetEntity(m_iPickedEntityID);							
					m_pkActionDlg->Open();
				}
			}
			
			if(!bAutoAttack)
			{
				if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(m_iPickedEntityID,"P_CharacterProperty"))
				{					
					if(!pkCP->IsDead())
					{
						bAutoAttack = true;					
						SendUseSkill( m_pkSkillBar->GetPrimarySkill(),m_iPickedEntityID,true);
					}
				}
			}			
		}		
	}
	else
	{
		if(fUsePressed != -1)
		{
			//stop auto attack
			if(bAutoAttack)
			{				
				bAutoAttack = false;
				SendUseSkill( "",-1,false);
			}		
			else if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iPickedEntityID))
			{					
				//if its an item , pick it up
				if(P_Item* pkItem = (P_Item*)pkEnt->GetProperty("P_Item"))
				{
					if(pkItem->GetEntity()->GetParent()->IsZone())					
						RequestPickup(m_iPickedEntityID);
				}
				else 
				// if not an item do first action
				if(P_Ml* pkMl = (P_Ml*)pkEnt->GetProperty("P_Ml"))
				{
					vector<string>	kActions;
					pkMl->GetActions(kActions);
				
					if(!kActions.empty())
					{
						//cout<<"performing first action"<<endl;						
						SendAction(m_iPickedEntityID,kActions[0]);
					}
				}
				else
				//is it a character?
				if(P_CharacterProperty* pkCP = (P_CharacterProperty*)pkEnt->GetProperty("P_CharacterProperty"))
				{
					if(!pkCP->IsDead())
						SendUseSkill(m_pkSkillBar->GetPrimarySkill(),m_iPickedEntityID,false);							
				}
			}
		}
					
		fUsePressed = -1;
	}

	/*
		seccondary attack button
	
	*/
	static float fSecondaryPressed = -1;
	static bool bSecondaryAutoAttack = false;
	if(m_pkInputHandle->VKIsDown("secondary") && m_bGuiCapture && !m_pkActionDlg->IsOpen())
	{
		//	not pressed before
		if(fSecondaryPressed == -1)
		{
			fSecondaryPressed = m_pkZeroFps->GetEngineTime();				
		}
		else if(m_pkZeroFps->GetEngineTime() - fSecondaryPressed > 0.2)
		{
			if(!bAutoAttack)
			{
				if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(m_iPickedEntityID,"P_CharacterProperty"))
				{	
					if(!pkCP->IsDead())
					{
						bSecondaryAutoAttack = true;					
						SendUseSkill( m_pkSkillBar->GetSecondarySkill(),m_iPickedEntityID,true);								
					}
				}
			}
		}					
	}
	else
	{
		if(fSecondaryPressed != -1)
		{
			//stop auto attack
			if(bSecondaryAutoAttack)
			{				
				bSecondaryAutoAttack = false;
				SendUseSkill( "",-1,false);
			}else if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iPickedEntityID))
			{					
 				//is it a character?
				if(P_CharacterProperty* pkCP = (P_CharacterProperty*)pkEnt->GetProperty("P_CharacterProperty"))
				{
					if(!pkCP->IsDead())
						SendUseSkill(m_pkSkillBar->GetSecondarySkill(),m_iPickedEntityID,false);													 					
				}
			}										
			fSecondaryPressed = -1;
		}	
	}
	

		
	
	//respawn knapp, i brist p?gui
	if(/*m_bDead && */ m_pkInputHandle->Pressed(KEY_BACKSPACE) && !DelayCommand())
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
				/*
				//hot edge camera rotation
				if(fAx >= 0.49)
					pkCam->Set3PYAngle(pkCam->Get3PYAngle() - m_pkZeroFps->GetFrameTime()*100);
				if(fAx <= -0.49)
					pkCam->Set3PYAngle(pkCam->Get3PYAngle() + m_pkZeroFps->GetFrameTime()*100);			

				if(fAy >= 0.49)
					pkCam->Set3PPAngle(pkCam->Get3PPAngle() + m_pkZeroFps->GetFrameTime()*100);
				if(fAy <= -0.49)
					pkCam->Set3PPAngle(pkCam->Get3PPAngle() - m_pkZeroFps->GetFrameTime()*100);
				*/					
			}
			
			//rotate camera by keyboard
			if(m_pkInputHandle->VKIsDown("rotate_left"))		pkCam->Set3PYAngle(pkCam->Get3PYAngle() + m_pkZeroFps->GetFrameTime()*150.0);
			if(m_pkInputHandle->VKIsDown("rotate_right"))	pkCam->Set3PYAngle(pkCam->Get3PYAngle() - m_pkZeroFps->GetFrameTime()*150.0);			
			
			
			//rotate towards target
			if(m_kbTargetRotate.GetBool() && m_iTargetID != -1)
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
						fAngle = -Math::RadToDeg(fAngle);
						
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
				if(fDistance > 5.0)
					fDistance = 5.0;			
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
			if(P_CharacterControl* pkCC = (P_CharacterControl*)m_pkEntityManager->GetPropertyFromEntityID(m_iCharacterID,"P_CharacterControl"))
			{
				if(!pkCC->GetNoClientRotation())			
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
}

void MistClient::OnSystem() 
{
	ShowLag();

	UpdateCharacter();
	SendControlInfo();
// 	CloseActiveContainer();

	if(m_iCharacterID != -1)
 		m_pkSkillBar->Update(m_pkZeroFps->GetSystemUpdateFpsDelta());

	
}

void MistClient::ShowLag()
{
	if(m_kbShowLagMeeter.GetBool())
	{
		if(ZGuiWnd* pkLagWnd = (ZGuiLabel*)GetWnd("LagWnd"))
			pkLagWnd->Show();
	
		ZGuiLabel* pkLagLable = (ZGuiLabel*)GetWnd("LagLabel");
		ZGuiLabel* pkLossLable = (ZGuiLabel*)GetWnd("LossLabel");
	
		if(pkLagLable && pkLossLable)
		{
			pkLagLable->Show();
			pkLossLable->Show();
			
			static float fDelay = 0;
 			static float fTotal = 0;
 			static float fTotalLoss = 0;
			static int   iSamples = 0;
			
 			fTotal += m_pkNetwork->GetPing(0);
 			fTotalLoss += m_pkNetwork->GetBadPackages();
 			iSamples++;
			
			if(m_pkZeroFps->GetEngineTime() - fDelay > 2.0)
			{
				pkLagLable->SetText((char*)IntToString(int(fTotal/iSamples)).c_str());
				pkLossLable->SetText((char*)IntToString(int(fTotalLoss/iSamples)).c_str());
					
				fTotal = 0;
				fTotalLoss = 0;
				iSamples = 0;
				fDelay = m_pkZeroFps->GetEngineTime();
			}
		}
	}
	else
	{
		if(ZGuiWnd* pkLagWnd = (ZGuiLabel*)GetWnd("LagWnd"))
			pkLagWnd->Hide();
	}
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
		//pkEnt->SetNetIgnoreFlag(NETUPDATEFLAG_ROT,true);
	
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
				
 				if(m_kbBloom.GetBool())
 					m_pkCamera->SetBloomEnabled(true);
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
		
		//when player character is hit, this is called
		case MLNM_SC_PLAYERHIT:
		{
			int iDamage;
			pkNetMessage->Read( iDamage);
		
			float fShakeamount = Min(iDamage*2,20);
					
			if(m_pkCamera)
				m_pkCamera->SetShakeAmount(fShakeamount);
				
			break;
		}
		
		case MLNM_SC_SKILLINFO:
		{
			string	strScreenName;
			string	strIcon;
			string	strInfoText;
			int		iLevel;
			int		iSkillType;
			float		fRange;
			float		fStaminaUsage;
			float		fManaUsage;
			float		fReloadTime;
			float		fCastTime;
			int		iSkillPoints;
			
			pkNetMessage->Read_Str(strScreenName);
			pkNetMessage->Read_Str(strIcon);
			pkNetMessage->Read_Str(strInfoText);
			pkNetMessage->Read(iLevel);
			pkNetMessage->Read(iSkillType);
			pkNetMessage->Read(fRange);
			pkNetMessage->Read(fStaminaUsage);
			pkNetMessage->Read(fManaUsage);
			pkNetMessage->Read(fReloadTime);
			pkNetMessage->Read(fCastTime);
			pkNetMessage->Read(iSkillPoints);
			
			
			cout<<"-= gott skillinfo =-"<<endl;
			cout<<"screen name:"<<strScreenName<<endl;
			cout<<"icon:"<<strIcon<<endl;
			cout<<"info text:"<<strInfoText<<endl;
			cout<<"level:"<<iLevel<<endl;
			cout<<"skill type:"<<iSkillType<<endl;
			cout<<"range:"<<fRange<<endl;
			cout<<"stamina usage:"<<fStaminaUsage<<endl;
			cout<<"mana usage:"<<fManaUsage<<endl;
			cout<<"reload time:"<<fReloadTime<<endl;
			cout<<"cast time:"<<fCastTime<<endl;
			cout<<"available skillpoints:"<<iSkillPoints<<endl;
			
			if(ZGuiButton* pkSkill = (ZGuiButton*)g_kMistClient.GetWnd("SkillIcon"))
			{
				char szLevel[16];
				sprintf(szLevel, "%i", iLevel);
				pkSkill->SetText(szLevel);
			}

			if(ZGuiTextbox* pkTextbox = (ZGuiTextbox*)g_kMistClient.GetWnd("SkillInfoText"))
			{
				pkTextbox->SetMultiLine(true);
				pkTextbox->SetReadOnly(true);
				pkTextbox->SetText((char*)strInfoText.c_str());
			}
			
			break;
		}
		
		case MLNM_SC_SKILLTREE:
		{
			int iSize;
			pkNetMessage->Read(iSize);
			
/*			
			string strName;
			string strParent;
			string strScreenName;
			string strIcon;
			int	 iLevel;
			*/
			ZGuiTreebox* pkTreeBox = (ZGuiTreebox*)g_kMistClient.GetWnd("SkillTree");	

			if(!pkTreeBox)
			{
				cout<<"ERROR: skill tree box not found"<<endl;
				return;
			}

			pkTreeBox->Clear();
			pkTreeBox->ChangeIconSize(32);			

			vector<SKILL_TREE_INFO> vkSkillInfo;
			
			for(int i =0;i<iSize;i++)
			{
				SKILL_TREE_INFO info;
				pkNetMessage->Read_Str(info.strName);
				pkNetMessage->Read_Str(info.strParent);
				pkNetMessage->Read_Str(info.strScreenName);
				pkNetMessage->Read_Str(info.strIcon);
				pkNetMessage->Read(info.iLevel);				

				vkSkillInfo.push_back(info);
			}

			map<string,int> icon_map;

			SortSkillTreeData(vkSkillInfo);
			AddIconsToSkillTree(pkTreeBox, vkSkillInfo, icon_map);

			for(unsigned int i =0;i<vkSkillInfo.size();i++)
			{
				//cout << "SKILL[" << i << "]: " << vkSkillInfo[i].strName<<" "<< vkSkillInfo[i]. strParent<<" "<<vkSkillInfo[i].strIcon<<endl;

				SKILL_TREE_INFO info = vkSkillInfo[i];
				int idex = icon_map[info.strIcon];
			
				//text
				string strText = info.strScreenName + string("[")+IntToString(info.iLevel)+string("]"); 
			
				if(info.strParent.empty())
				{
					pkTreeBox->AddItem(pkTreeBox->Root(),(char*)strText.c_str(),idex,idex,(char*)info.strName.c_str());					
				}
				else
				{
					if(!pkTreeBox->AddItem(info.strParent,(char*)strText.c_str(),idex,idex,(char*)info.strName.c_str()))
					{
						cout<<"Error adding treeitem "<<info.strName<< " parent "<<info.strParent<<endl;
						pkTreeBox->AddItem(pkTreeBox->Root(),(char*)strText.c_str(),idex,idex,(char*)info.strName.c_str());					
					}
				}

				cout<<"Have added: " << info.strName << "," << info.strScreenName << endl;
			}				
			
			break;
		}
		
		case MLNM_SC_CLOSECONTAINER:
		{
			int iID;
			pkNetMessage->Read(iID);

			if(m_pkInventoryDlg == NULL)
				return;

			if(!m_pkInventoryDlg->IsVisible())
				return;
	
			if(m_pkInventoryDlg->m_iActiveContainerID != iID)
				return;
				
			m_pkInventoryDlg->CloseContainerWnd();
		
			break;
		}
					
		case MLNM_SC_SKILLBAR:
		{
			vector<SkillNetInfo>	kSkillList;
			SkillNetInfo temp;

			while(true)
			{
				pkNetMessage->Read(temp.m_cPos);
				
				//break if reaching pos -1
				if(temp.m_cPos == -1)
					break;
				
				pkNetMessage->Read_Str(temp.m_strSkillName);								
				if(!temp.m_strSkillName.empty())
				{
					pkNetMessage->Read_Str(temp.m_strSkillScreenName);
					pkNetMessage->Read_Str(temp.m_strSkillIcon);
					pkNetMessage->Read(temp.m_fReloadTimeLeft);				
					pkNetMessage->Read(temp.m_fReloadTimeTotal);				
					pkNetMessage->Read(temp.m_cSkillType);	
					pkNetMessage->Read(temp.m_cTargetType);																	
				}
				
				kSkillList.push_back(temp);
			}
		
			m_pkSkillBar->UpdateList(kSkillList);
			
			break;			
		}
	
		case MLNM_SC_CHARACTERID:
		{
			cout<<"got character entityID from server"<<endl;
			pkNetMessage->Read(m_iCharacterID);
		
			//temporary hack 
			/*SendAddSkillToSkillbar("skill-fireball.lua",0);
			SendAddSkillToSkillbar("skill-speed.lua",1);
			SendAddSkillToSkillbar("skill-heal.lua",2);
			SendAddSkillToSkillbar("skill-basic_attack.lua",3);
			SendAddSkillToSkillbar("skill-bow.lua",4);
			SendAddSkillToSkillbar("skill-resurrect.lua",5);
			SendAddSkillToSkillbar("skill-bolt.lua",6);
			SendAddSkillToSkillbar("skill-light.lua",7);
			SendAddSkillToSkillbar("skill-stun.lua",8);
			SendAddSkillToSkillbar("skill-freeze.lua",9);
			SendAddSkillToSkillbar("skill-camp.lua",10);
			SendAddSkillToSkillbar("skill-create_food.lua",11);
			SendAddSkillToSkillbar("skill-push.lua",12);
			SendAddSkillToSkillbar("skill-poison_cloud.lua",13);
			SendAddSkillToSkillbar("skill-charm.lua",14);*/
			//--------------------------
		
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
			
			pkNetMessage->Read_Str(kInfo.strName);
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
			cout<<"NAME:"<<kInfo.strName<<endl;	
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
			float	fOxygen;
			float fOxygenMax;
			float	fHealth;
			float fHealthMax;
			float	fMana;
			float fManaMax;
			string strName;
			float fLevel;
			float fXP;
			float fPrevLevel;
			float fNextLevel;
			
			float fMoney;
			float fStatPoints;
			
			float fStrength;
			float fDexterity;
			float fVitality;
			float fIntelligence;
			float fWisdom;
			float fCharisma;
			
			float fAttack;
			float fDefense;
			
			float fMinDamage;
			float fMaxDamage;
			
			float fAbsorbTotal;
	
			pkNetMessage->Read(fStamina);
			pkNetMessage->Read(fStaminaMax);

			pkNetMessage->Read(fOxygen);
			pkNetMessage->Read(fOxygenMax);

			pkNetMessage->Read(fHealth);
			pkNetMessage->Read(fHealthMax);
			pkNetMessage->Read(fMana);
			pkNetMessage->Read(fManaMax);									
			pkNetMessage->Read_Str(strName);			
			pkNetMessage->Read(fLevel);
			pkNetMessage->Read(fXP);
			pkNetMessage->Read(fPrevLevel);
 			pkNetMessage->Read(fNextLevel);
 			pkNetMessage->Read(fMoney);
 			pkNetMessage->Read(fStatPoints);
			
 			pkNetMessage->Read(fStrength);
 			pkNetMessage->Read(fDexterity);
 			pkNetMessage->Read(fVitality);
 			pkNetMessage->Read(fIntelligence);
 			pkNetMessage->Read(fWisdom);
 			pkNetMessage->Read(fCharisma);
 			pkNetMessage->Read(fAttack);
 			pkNetMessage->Read(fDefense);
 			
			pkNetMessage->Read(fMinDamage);
 			pkNetMessage->Read(fMaxDamage);

 			pkNetMessage->Read(fAbsorbTotal);
			
			//basic stats
			if(ZGuiLabel* pkP = (ZGuiLabel*)GetWnd("StatStrength"))
				pkP->SetText((char*)IntToString(int(fStrength)).c_str());
			if(ZGuiLabel* pkP = (ZGuiLabel*)GetWnd("StatDexterity"))
				pkP->SetText((char*)IntToString(int(fDexterity)).c_str());
			if(ZGuiLabel* pkP = (ZGuiLabel*)GetWnd("StatVitality"))
				pkP->SetText((char*)IntToString(int(fVitality)).c_str());
			if(ZGuiLabel* pkP = (ZGuiLabel*)GetWnd("StatIntelligence"))
				pkP->SetText((char*)IntToString(int(fIntelligence)).c_str());
			if(ZGuiLabel* pkP = (ZGuiLabel*)GetWnd("StatWisdom"))
				pkP->SetText((char*)IntToString(int(fWisdom)).c_str());
			if(ZGuiLabel* pkP = (ZGuiLabel*)GetWnd("StatCharisma"))
				pkP->SetText((char*)IntToString(int(fCharisma)).c_str());
			
			if(ZGuiLabel* pkP = (ZGuiLabel*)GetWnd("StatAttack"))
				pkP->SetText((char*)IntToString(int(fAttack)).c_str());
			if(ZGuiLabel* pkP = (ZGuiLabel*)GetWnd("StatDefense"))
				pkP->SetText((char*)IntToString(int(fDefense)).c_str());
			if(ZGuiLabel* pkP = (ZGuiLabel*)GetWnd("StatDamage"))
				pkP->SetText( (char*) (IntToString(int(fMinDamage))+string(" - ")+IntToString(int(fMaxDamage)) ).c_str()  );
							
			if(ZGuiLabel* pkP = (ZGuiLabel*)GetWnd("StatAbsorbTotal"))
				pkP->SetText((char*)IntToString(int(fAbsorbTotal)).c_str());
							
						
			//level and xp
			if(ZGuiLabel* pkP = (ZGuiLabel*)GetWnd("StatLevel"))
				pkP->SetText((char*)IntToString(int(fLevel)).c_str());
			if(ZGuiLabel* pkP = (ZGuiLabel*)GetWnd("StatXP"))
				pkP->SetText(((char*)IntToString(int(fXP)).c_str()));
			if(ZGuiLabel* pkP = (ZGuiLabel*)GetWnd("StatNextLevel"))
				pkP->SetText((char*)IntToString(int(fNextLevel)).c_str());
			if(ZGuiLabel* pkP = (ZGuiLabel*)GetWnd("StatStatPoints"))
				pkP->SetText((char*)IntToString(int(fStatPoints)).c_str());
				
			//money
			if(ZGuiLabel* pkP = (ZGuiLabel*)GetWnd("StatMoney"))
				pkP->SetText((char*)IntToString(int(fMoney)).c_str());			
				
			//XP BAR
			if(ZGuiProgressbar* pkPB = (ZGuiProgressbar*)GetWnd("StatXPBar"))
			{
				pkPB->SetRange(0,int(fNextLevel - fPrevLevel));
				pkPB->SetPos(int(fXP - fPrevLevel));				
			}
							
			//name
			if(ZGuiLabel* pkP = (ZGuiLabel*)GetWnd("StatName"))
				pkP->SetText((char*)strName.c_str());

			//life
			if(ZGuiProgressbar* pkPB = (ZGuiProgressbar*)GetWnd("LifeProgressbar"))
			{
				pkPB->SetRange(0,int(fHealthMax));
				pkPB->SetPos(int(fHealth));				
			}
					
			//mana
			if(ZGuiProgressbar* pkPB = (ZGuiProgressbar*)GetWnd("ManaProgressbar"))
			{
				pkPB->SetRange(0,int(fManaMax));
				pkPB->SetPos(int(fMana));				
			}			
			
			// Flag if we are in/out of water.
			if(fOxygenMax == 100)
				m_bInWater = true;
			else
				m_bInWater = false;

			//stamina / Oxygen
			if(ZGuiProgressbar* pkPB = (ZGuiProgressbar*)GetWnd("StaminaProgressbar"))
			{
				if(!m_bInWater)
				{
					pkPB->SetRange(0,int(fStaminaMax));
					pkPB->SetPos(int(fStamina));				
				}
				else
				{
					pkPB->SetRange(0,int(fOxygenMax));
					pkPB->SetPos(int(fOxygen));				
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
			AddStringToChatBox("* Playerlist *");
			
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
	m_pkMakerEntity	=	NULL;
}

void MistClient::OnClientConnected() 
{
	m_pkConsole->Printf("Successfully connected to server");


/*	ShowWnd("ConnectWnd", true, true, true);
	m_pkGui->PlaceWndFrontBack(g_kMistClient.GetWnd("ConnectWnd"), true);
	m_pkGui->SetCaptureToWnd(g_kMistClient.GetWnd("ConnectWnd"));	*/
	
	ShowWnd("CharGen_SelectCharWnd", true, true, true);
	m_pkGui->PlaceWndFrontBack(g_kMistClient.GetWnd("CharGen_SelectCharWnd"), true);	
	m_pkGui->SetCaptureToWnd(g_kMistClient.GetWnd("CharGen_SelectCharWnd"));	
	m_bLoginKeepAlive = true;

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
	m_bLoginKeepAlive = false;
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
	g_ZFObjSys.Log_Create("mistclient2");

   ReadWriteServerList(true);
   return true;
}

int MistClient::GetClosestEnemy()
{
	P_CharacterProperty* pkCharacter = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID( m_iCharacterID,"P_CharacterProperty");
	if(!pkCharacter)
		return -1;

// 	int iMyFaction = pkCharacter->GetFaction();
	Vector3 kRefPos = pkCharacter->GetEntity()->GetWorldPosV();
	int iClosestEntity = -1;
	float fClosestDistance = MAX_TARGET_DISTANCE;


	vector<Entity*> kEntitys;
	m_pkEntityManager->GetZoneEntity()->GetAllEntitys( &kEntitys);
	
	int iEntitys = kEntitys.size();
	for(int i = 0;i<iEntitys;i++)
	{
		int iID = kEntitys[i]->GetEntityID();
		if(pkCharacter->IsEnemy(iID))
		{	
			if(P_CharacterProperty* pkCP = (P_CharacterProperty*)kEntitys[i]->GetProperty("P_CharacterProperty"))
			{		
				if(!pkCP->IsDead())
				{		
					float d = kEntitys[i]->GetWorldPosV().DistanceTo(kRefPos);
			
					if(d < fClosestDistance)
					{
						iClosestEntity = iID;
						fClosestDistance = d;
					}
				}
			}
		}
	}
	
	return iClosestEntity;
}

Entity* MistClient::GetTargetObject()
{
	Vector3 start = m_pkCamera->GetPos();
	Vector3 dir = Get3DMouseDir(m_bGuiCapture);

	vector<Entity*> kObjects;
	kObjects.clear();
	
	m_pkEntityManager->GetZoneEntity()->GetAllEntitys(&kObjects);
		
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
		if(kObjects[i]->GetType() == "t_serverinfo.lua")
			continue;					
		if(kObjects[i]->GetEntityID() == m_iCharacterID)		//dont pick self
			continue;
		if(Entity* pkParent = kObjects[i]->GetParent())			//object is not a free object
			if(!pkParent->IsZone())
				continue;
		
		
		
		//-------------
		
		if(kObjects[i]->GetWorldPosV().DistanceTo(m_pkCamera->GetPos()) > MAX_TARGET_DISTANCE)
			continue;
		
 		if(kObjects[i]->GetProperty("P_Ml") || 
 			kObjects[i]->GetProperty("P_Item") || 
 			kObjects[i]->GetProperty("P_CharacterProperty"))
 		{		
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
	}
	
// 	if(pkClosest)
// 		cout<<"target:"<<pkClosest->GetType()<<endl;
	
	return pkClosest;
}

/*	Return 3D postion of mouse in world. */
Vector3 MistClient::Get3DMouseDir(bool bMouse)
{
	float x;
	float y;
	m_pkInputHandle->MouseXY(x,y);		
	
	return m_pkCamera->Get3DCursorDir(x,y,bMouse);

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

void MistClient::SendAddStatPoint(int iStat)
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_ADDSTATPOINT);
	
	kNp.Write(iStat);
	
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);		
}

void MistClient::SendAddSkillToSkillbar(const string& strSkill,int iPos)
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_ADDSKILLTOSKILLBAR);
	
	kNp.Write_Str(strSkill);
	kNp.Write(iPos);
	
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);		
}

void MistClient::SendRemoveItemFromSkillbar(int iPos)
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_REMOVEITEMFROMSKILLBAR);
	
	kNp.Write(iPos);
	
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);
}

void MistClient::SendSit()
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_SIT);
	
	
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);		

}

void MistClient::SendUseSkill(const string& strSkill,int iTargetID,bool bAutomatic)
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_ADDSKILLTOQUEUE);
	
	kNp.Write_Str(strSkill);
	kNp.Write(iTargetID);
	kNp.Write(bAutomatic);
	
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);		
}

void MistClient::SendCombatMode(bool bCombatMode)
{
	if(bCombatMode)
	{
		AddStringToChatBox("Entering combat mode");
	
		//toggle button texture
		if(ZGuiButton* pkBtn = (ZGuiButton*)g_kMistClient.GetWnd("ToggleCombatBn"))
		{
			pkBtn->GetButtonUpSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("combaton_up.bmp");
			pkBtn->GetButtonHighLightSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("combaton_up.bmp");
			pkBtn->GetButtonDownSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("combaton_down.bmp");
		}		
	}
	else
	{
		AddStringToChatBox("Leaving combat mode");
		
		if(ZGuiButton* pkBtn = (ZGuiButton*)g_kMistClient.GetWnd("ToggleCombatBn"))
		{
			pkBtn->GetButtonUpSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("combatoff_up.bmp");
			pkBtn->GetButtonHighLightSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("combatoff_up.bmp");
			pkBtn->GetButtonDownSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("combatoff_down.bmp");
		}			
	}
	
	//save new mode in client, 
	m_bCombatMode = bCombatMode;
	
	//send toggle to server
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_COMBATMODE);
	
	kNp.Write(bCombatMode);
	
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);	
}

// void MistClient::SendSetTarget(int iTargetID)
// {
// 	NetPacket kNp;			
// 	kNp.Write((char) MLNM_CS_SET_TARGET);
// 	
// 	kNp.Write(iTargetID);
// 	
// 	kNp.TargetSetClient(0);
// 	SendAppMessage(&kNp);	
// }

void MistClient::SendUseItem(int iItemID)
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_USEITEM);
	
	kNp.Write(iItemID);
	
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

void MistClient::SendRequestSkillInfo(const string& strSkill)
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_REQ_SKILLINFO);
	
	kNp.Write_Str(strSkill);
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);	
}

void MistClient::SendRequestSkillTree()
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_REQ_SKILLTREE);
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

void MistClient::SendSkillInc(const string& strSkill)
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_SKILLINC);
		
	kNp.Write_Str(strSkill);
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);	
}

void MistClient::SendRequestItemInfo(int iItemID)
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
	
// 	if(Entity* pkContainer = m_pkEntityManager->GetEntityByID(m_pkInventoryDlg->m_iActiveContainerID))
// 		if(Entity* pkCharacter = m_pkEntityManager->GetEntityByID(m_iCharacterID))
// 		{
// 			Vector3 dist = pkCharacter->GetWorldPosV() - pkContainer->GetWorldPosV();
// 			if(dist.Length() > 2)
// 				m_pkInventoryDlg->CloseContainerWnd(); 
// 		}
}

void MistClient::RegBeginPlay(string strChar)
{
	NetPacket kNp;
	kNp.Clear();
	kNp.Write((char) MLNM_CS_REQPLAY);
	kNp.Write_Str( strChar );
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);
	m_bLoginKeepAlive = false;
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

void MistClient::OnSystemMessage(const string& strType,int iNrOfParam,const void** pkParams)
{
	char szTitle[256];

	if(strType == "serverlist")
	{
		m_kServerList.clear();
		vector<ServerInfo> pkServerIp = m_pkNetwork->GetServers();
		char szIp[128];
		
		for(int i=0; i<pkServerIp.size(); i++)
		{
			m_pkNetwork->AddressToStr(&pkServerIp[i].m_kServerIp,szIp);
			
			if(!pkServerIp[i].m_bUpdated)
				sprintf(szTitle, "No respons",pkServerIp[i].m_acServerName);
			else						
				sprintf(szTitle, "%s [%d.%d.%d] [Ping %d][Players %d/%d]",pkServerIp[i].m_acServerName,
					pkServerIp[i].m_kVersion.m_iMajor, pkServerIp[i].m_kVersion.m_iMinor, pkServerIp[i].m_kVersion.m_iBuild,
					(int)(pkServerIp[i].m_fRequestTime*1000), 
					pkServerIp[i].m_iNumOfPlayers,	pkServerIp[i].m_iMaxPlayers);
						
			AddRemoveServer(szTitle, szIp,true);

		}
		
		//fulhaxos
		AddRemoveServer("localhost","localhost:4242",true);
		
		UpdateServerListbox();
	} 
	
	if(strType == "serverlist-requested")
	{
		m_kServerList.clear();
		
		AddRemoveServer("WAITING FOR MASTERSERVER RESPONS","",true);		
		AddRemoveServer("localhost","localhost:4242",true);		
		
		UpdateServerListbox();
	}
}

// void MistClient::SendSetDefaultAttack(const string& strSkill)
// {
// 	NetPacket kNp;	
// 	kNp.Clear();
// 	kNp.Write((char) MLNM_CS_SETDEFAULTATTACK);
// 
// 	kNp.Write_Str(strSkill);
// 	
// 	kNp.TargetSetClient(0);
// 	SendAppMessage(&kNp);	
// 
// }

string MistClient::GetSkillTreeSkill()
{
	string strSkill;

	ZGuiWnd* pkWndSkill = g_kMistClient.GetWnd("SkillWnd");
	if(pkWndSkill && pkWndSkill->IsVisible())
	{
		ZGuiTreebox* pkBox = dynamic_cast<ZGuiTreebox*>( g_kMistClient.GetWnd("SkillTree"));
		ZFAssert(pkBox, "No Skilltree found");
		ZGuiTreeboxNode* pkTreeNode = pkBox->GetSelNode();
		ZFAssert(pkTreeNode, "No selected skilltree node");
		strSkill = pkTreeNode->strNodeID;
	}

	return strSkill;
}

