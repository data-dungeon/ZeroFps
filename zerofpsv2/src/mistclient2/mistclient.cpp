/**
	\defgroup MistClient MistClient
	\ingroup MistLand

  MistClient is the Client of the game MistLands.
*/

#ifndef _DONT_MAIN					// <- OBS! Flytta inte på denna. Måste ligga i
	#define _MAINAPPLICATION_		// just denna fil och inte på flera ställen.
	#define _DONT_MAIN
#endif
 
#include "mistclient.h"
//#include "../zerofpsv2/engine_systems/propertys/p_camera.h"
//#include "../zerofpsv2/engine_systems/propertys/p_sound.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../mcommon/p_arcadecharacter.h"
#include "../mcommon/ml_netmessages.h"

#include "../mcommon/p_characterproperty.h"
#include "../mcommon/p_item.h"
#include "../mcommon/p_container.h"

#include "gui_optionsdlg.h"

MistClient g_kMistClient("MistClient",0,0,0);

bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) ;

void GuiMsgIngameScreen( string strMainWnd, string	strController,	unsigned int msg, int numparms,	void *params )	;

MistClient::MistClient(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	g_ZFObjSys.SetPreLogName("mistclient2");
	g_ZFObjSys.Log_Create("mistclient2");
	
	m_pkHighlight = NULL;
	m_fDelayTime  = 0;

	m_iCharacterID = -1;

	m_strLoginName = "Psykosmurfan";
   m_strLoginPW = "topsecret";

   RegisterVariable("r_loginname", 	&m_strLoginName, CSYS_STRING);
   RegisterVariable("r_loginpw", 	&m_strLoginPW, CSYS_STRING);

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

	//register property bös
	RegisterPropertys();

	//register resources
	RegisterResources();
	
	//setup referense sound distance
	m_pkAudioSys->SetReferensDistance(0.25);

	//init mistland script intreface
	MistLandLua::Init(m_pkEntityManager,m_pkScript);

   // init gui script intreface
	GuiAppLua::Init(&g_kMistClient, m_pkScript);

	//set window title		
   SetTitle("MistClient - Banka och slå");
	
	//set client in server mode to show gui etc
	m_pkZeroFps->StartServer(true,false);

	// create gui for mistlands
	m_pkOptionsDlg = new OptionsDlg(this);
	SetupGUI();

	ToggleGuiCapture(1);

	//run autoexec script
	if(!m_pkIni->ExecuteCommands("mistclient_autoexec.ini"))
		m_pkConsole->Printf("No game_autoexec.ini.ini found");	
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
			RequestPlayerList();			
			break;
		}
	
		case FID_KILLME:
		{
			RequestKillMe();
			break;
		}	
	}
}

void MistClient::Say(string strMsg)
{
	if(strMsg.size() == 0)
		return;

	if(strMsg[0] == '/')
	{
		//handle different chatbox funktions		
		if(strMsg.length() > 1)
		{
			if(strMsg.substr(1) == "users")
			{
				RequestPlayerList();
			}
		
		}
	}
	else
	{
		//default to normal talk
		SendMessage(strMsg,MLCM_TALK,""); 	
	}									
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
	m_pkPropertyFactory->Register("P_CharacterProperty",	Create_P_CharacterProperty);	
	m_pkPropertyFactory->Register("P_CharacterControl",  Create_P_CharacterControl);	
	m_pkPropertyFactory->Register("P_Enviroment", 			Create_P_Enviroment);
	m_pkPropertyFactory->Register("P_Ml", 						Create_P_Ml);
	m_pkPropertyFactory->Register("P_Item", 					Create_P_Item);
	m_pkPropertyFactory->Register("P_Container", 			Create_P_Container);
}

void MistClient::RenderInterface(void)
{
	if(m_pkHighlight) 
	{
		float fRadius = m_pkHighlight->GetRadius();
		if(fRadius < 0.1)
			fRadius = 0.1;
				
		Vector3 kMin = m_pkHighlight->GetWorldPosV() - fRadius;
		Vector3 kMax = m_pkHighlight->GetWorldPosV() + fRadius;

		m_pkRender->DrawAABB( kMin,kMax, m_pkRender->GetEditColor("active/firstentity") );
	}
}

void MistClient::OnIdle() 
{
	Input();
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
	if(m_pkInputHandle->Pressed(KEY_SPACE) && !DelayCommand())
		ToggleGuiCapture();


	//get mouse
	float x,z;		
	m_pkInputHandle->RelMouseXY(x,z);	
	
	//check buttons
	if(!m_bGuiCapture)
	{
		m_kCharacterControls[eUP] = 	m_pkInputHandle->VKIsDown("move_forward");
		m_kCharacterControls[eDOWN] =	m_pkInputHandle->VKIsDown("move_back");			
		m_kCharacterControls[eLEFT] = m_pkInputHandle->VKIsDown("move_left");			
		m_kCharacterControls[eRIGHT]= m_pkInputHandle->VKIsDown("move_right");
		m_kCharacterControls[eJUMP] = m_pkInputHandle->VKIsDown("jump");
		m_kCharacterControls[eCRAWL] =m_pkInputHandle->VKIsDown("crawl");
	}

	// taunts
	if ( !m_bGuiCapture )
	{
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

				NetPacket kNp;	
				kNp.Clear();
				kNp.Write((char) MLNM_CS_ANIM);

				kNp.Write(iTauntID);
				
				kNp.TargetSetClient(0);
				SendAppMessage(&kNp);
			}
		}
	}

	m_pkHighlight = GetTargetObject();
	
	if ( m_pkInputHandle->VKIsDown("use") )
	{
		if(!DelayCommand())
		{
			if(m_pkHighlight)
			{
				NetPacket kNp;			
				kNp.Write((char) MLNM_CS_USE);
				kNp.Write(m_pkHighlight->GetEntityID());
				kNp.TargetSetClient(0);
				SendAppMessage(&kNp);		
			}
		}
	}

	if ( m_pkInputHandle->VKIsDown("look") )
	{
		if(!DelayCommand())
		{
			if(m_pkHighlight)
			{
				NetPacket kNp;			
				kNp.Write((char) MLNM_CS_LOOK);
				kNp.Write(m_pkHighlight->GetEntityID());
				kNp.TargetSetClient(0);
				SendAppMessage(&kNp);		
			}
		}
	}

	

	if(m_pkInputHandle->Pressed(KEY_F1) && !DelayCommand())
	{
		if(IsWndVisible("ChatDlgMainWnd"))
			LoadStartScreenGui(false);
	}

	if(m_pkInputHandle->Pressed(KEY_ESCAPE) && !DelayCommand())
	{
		if(IsWndVisible("OptionsWnd"))
			ShowWnd("OptionsWnd", 0,0,0);
		else
		if(IsWndVisible("MLStartWnd"))
			LoadInGameGui();
	}

	if(!m_bGuiCapture)
	{
		//update camera
		if(Entity* pkCharacter = m_pkEntityManager->GetEntityByID(m_iCharacterID))
		{
			if(P_Camera* pkCam = (P_Camera*)pkCharacter->GetProperty("P_Camera"))
			{			
				pkCam->Set3PYAngle(pkCam->Get3PYAngle() - (x/5.0));
				pkCam->Set3PPAngle(pkCam->Get3PPAngle() + (z/5.0));			
				pkCam->SetOffset(Vector3(0,0,0)); 

				float fDistance = pkCam->Get3PDistance();
				if(m_pkInputHandle->VKIsDown("zoomin")) 	fDistance -= 0.5;
				if(m_pkInputHandle->VKIsDown("zoomout"))	fDistance += 0.5;
				
				//make sure camera is nto to far away
				if(fDistance > 8.0)
					fDistance = 8.0;
				
				//make sure camera is not to close
				if(fDistance < 0.2)
					fDistance = 0.2;

					
				//select first or 3d view camera
				if(fDistance < 0.3)	
				{	
					pkCam->SetType(CAM_TYPEFIRSTPERSON_NON_EA);
					
					//disable player model in first person
					if(P_Mad* pkMad = (P_Mad*)pkCharacter->GetProperty("P_Mad"))
						pkMad->SetVisible(false);					
				}
				else			
				{
					pkCam->SetType(CAM_TYPE3PERSON);
					
					//enable player model i 3d person
					if(P_Mad* pkMad = (P_Mad*)pkCharacter->GetProperty("P_Mad"))
						pkMad->SetVisible(true);				
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
}

void MistClient::OnSystem() 
{
	UpdateCharacter();
	SendControlInfo();
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
		
		//setup enviroment
/*		if(!pkEnt->GetProperty("P_Enviroment"))
		{
			if(P_Enviroment* pkEnv = (P_Enviroment*)pkEnt->AddProperty("P_Enviroment"))
			{
				pkEnv->SetEnable(true);				
				pkEnv->SetEnviroment("data/enviroments/sun.env");
			}
		}*/
		
		if(P_Enviroment* pkEnv = (P_Enviroment*)pkEnt->GetProperty("P_Enviroment"))
		{
			pkEnv->SetEnable(true);				
		}		
	}
}


void MistClient::OnHud(void) 
{	

}




void MistClient::OnNetworkMessage(NetPacket *PkNetMessage)
{
	unsigned char ucType;

	// Read Type of Message.
	PkNetMessage->Read(ucType);

	switch(ucType)
	{
/*		case MLNM_SC_SETVIEW:
			int iEntityID;
			PkNetMessage->Read(iEntityID);

			if(Entity* pkEnt = m_pkEntityManager->GetEntityByID( iEntityID ))
			{
				if(P_Camera* pkCam = (P_Camera*)pkEnt->AddProperty("P_Camera"))
				{
					//pkCam->SetType(CAM_TYPEFIRSTPERSON_NON_EA);
					pkCam->SetCamera(m_pkCamera);
					cout<<"attached camera to client property"<<endl;
				}
			}

			break;*/

		case MLNM_SC_CHARACTERID:
		{
			cout<<"got character entityID from server"<<endl;
			PkNetMessage->Read(m_iCharacterID);
		
			break;
		}

			
		case MLNM_SC_SAY:
		{
			string strMsg;
			PkNetMessage->Read_Str(strMsg);
			m_pkConsole->Printf("Msg> %s",strMsg.c_str());
			AddStringToChatBox(strMsg);
						
			break;
		}			

		case MLNM_SC_PLAYERLIST:
		{
			int iPlayers;
			string strName;
			
			m_kPlayerList.clear();
			
			PkNetMessage->Read(iPlayers);
			
			//m_pkConsole->Printf("Listing Players : %d",iPlayers);
			AddStringToChatBox("Requesting playerlist");
			
			for(int i = 0;i<iPlayers;i++)
			{
				PkNetMessage->Read_Str(strName);
				//m_pkConsole->Printf("%d %s",i,strName.c_str());
				AddStringToChatBox(strName);
				m_kPlayerList.push_back(strName);					
			}
			
			char nr[5];
			IntToChar(nr,iPlayers);
			AddStringToChatBox(string(nr) + string(" online"));
			
			
			break;
		}
		
		default:
			cout << "Error in game packet : " << (int) ucType << endl;
			PkNetMessage->SetError(true);
			return;
	}
} 

void MistClient::OnClientStart(void)
{
	m_pkConsole->Printf("Trying to connect");	
	m_iCharacterID = -1;
}

void MistClient::OnClientConnected() 
{
	m_pkConsole->Printf("Successfully connected to server");

	//request character entityID
	NetPacket kNp;				
	kNp.Clear();
	kNp.Write((char) MLNM_CS_REQ_CHARACTERID);
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);	

	//load ingame gui	
	LoadInGameGui();
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
      g_kMistClient.AddListItem("ServerList", szText);
   }
}

bool MistClient::ReadWriteServerList(bool bRead)
{
   if(bRead)
   {
      m_kServerList.clear();

      FILE* pkFile = fopen("serverlist.txt", "rt");
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
      FILE* pkFile = fopen("serverlist.txt", "wt");
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

void MistClient::RequestKillMe()
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_REQ_KILLME);
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);		
}

void MistClient::RequestPlayerList()
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_REQ_PLAYERLIST);
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);		
}

bool MistClient::ShutDown()
{
   ReadWriteServerList(false);
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
	dir = Get3DMousePos(true);

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
		// Zeb was here! Nu kör vi med operativsystemets egna snabba musmarkör
		// alltså måste vi använda den position vi får därifrån.
		//	m_pkInputHandle->UnitMouseXY(x,y);
		//x = -0.5f + (float) m_pkInputHandle->m_iSDLMouseX / (float) m_pkApp->m_iWidth;
		//y = -0.5f + (float) m_pkInputHandle->m_iSDLMouseY / (float) m_pkApp->m_iHeight;
		int mx;		
		int my;
		
		m_pkInputHandle->SDLMouseXY(mx,my);
		
		x = -0.5f + (float) (mx - kViewCorner.x) / (float) kViewSize.x;
		y = -0.5f + (float) ((m_pkApp->m_iHeight - my) - kViewCorner.y) / (float) kViewSize.y;

		if(m_pkCamera->GetViewMode() == Camera::CAMMODE_PERSP)
		{
			dir.Set(x*xp,y*yp,-1.5);
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

/*	Returns 3D dir of mouse click in world. */
Vector3 MistClient::Get3DMousePos(bool m_bMouse)
{
	Vector3 dir;
//	float x,y;		
	
	//screen propotions
	float xp=4;
	float yp=3;
	float fovshit=-2.15;
	
	if(m_bMouse)
	{
		// Zeb was here! Nu kör vi med operativsystemets egna snabba musmarkör
		// alltså måste vi använda det inputsystemet.
		//	m_pkInputHandle->UnitMouseXY(x,y); 
		// Dvoid was here to . måste o måste, vill man ha lite kontroll över saker o ting så =D
		int x;
		int y;
		
		m_pkInputHandle->SDLMouseXY(x,y);
		
		x = int( -0.5f + (float) x / (float) m_pkApp->m_iWidth );
		y = int( -0.5f + (float) y / (float) m_pkApp->m_iHeight );
		
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

void MistClient::ToggleGuiCapture(int iForce)
{
	if(iForce == -1)
		m_bGuiCapture = !m_bGuiCapture;
	else
		m_bGuiCapture = (bool) iForce;

	if(m_bGuiCapture == true)
	{
		m_pkGui->ShowCursor(true);
	}
	else
	{
		m_pkGui->ShowCursor(false);
	}
}