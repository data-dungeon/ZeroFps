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
#include "../zerofpsv2/engine_systems/propertys/p_camera.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../mcommon/p_arcadecharacter.h"
#include "../mcommon/ml_netmessages.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../zerofpsv2/basic/zguifont.h"

MistClient g_kMistClient("MistClient",0,0,0);

bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) ;

MistClient::MistClient(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	g_ZFObjSys.Log_Create("mistclient2");
	

	m_iCharacterID = -1;

   RegisterVariable("r_jumpstart", &m_bSkipLoginScreen, CSYS_BOOL);
} 
 
void MistClient::OnInit() 
{
	m_pkZFVFileSystem->AddRootPath( string("../datafiles/mistlands") ,"/data");
	m_pkZFVFileSystem->AddRootPath( string("../datafiles/dm") ,"/data");

	//register commands
	Register_Cmd("say",			FID_SAY);	
	
	//initiate our mainview camera
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.25,250);	
	m_pkCamera->SetName("Main camera");
	m_pkZeroFps->AddRenderCamera(m_pkCamera);

	//register property bös
	RegisterPropertys();

	//register resources
	RegisterResources();
		
	//init mistland script intreface
	MistLandLua::Init(m_pkEntityManager,m_pkScript);

   // init gui script intreface
	GuiAppLua::Init(&g_kMistClient, m_pkScript);

	//set window title		
   SetTitle("MistClient - Banka och slå");
	
	//set client in server mode to show gui etc
	m_pkZeroFps->StartServer(true,false);

   // initialize gui system with default skins, font etc
	InitGui(m_pkScript, "morpheus10", "data/script/gui/defskins.lua", NULL, false, true); 

   // load startup screen 
   if(!m_bSkipLoginScreen)
   {
      LoadGuiFromScript("data/script/gui/ml_start.lua");

      char szFontData[512], szFontTex[512];
      sprintf(szFontData, "data/textures/gui/fonts/%s.fnt", "defguifont");
      sprintf(szFontTex, "data/textures/gui/fonts/%s.tga", "defguifont");
      
      ZGuiFont* font = new ZGuiFont("listboxfont");
      font->Create(szFontData, m_pkTexMan->Load(szFontTex, 0));
	   m_pkGui->GetResMan()->Add("listboxfont", font);

      GetWnd("ServerList")->SetFont(font); 
      GetWnd("LoginNameEB")->SetFont(font); 
      GetWnd("LoginPWEb")->SetFont(font); 
      GetWnd("NewServerIPName")->SetFont(font); 
      GetWnd("NewServerNameEB")->SetFont(font); 

      m_pkGui->AddKeyCommand(KEY_RETURN, GetWnd("NewServerNameEB"), GetWnd("AddNewServerOK"));
      m_pkGui->AddKeyCommand(KEY_RETURN, GetWnd("NewServerIPName"), GetWnd("AddNewServerOK"));

      m_pkGui->AddKeyCommand(KEY_RETURN, GetWnd("LoginNameEB"), GetWnd("LoginOK"));
      m_pkGui->AddKeyCommand(KEY_RETURN, GetWnd("LoginPWEb"), GetWnd("LoginOK"));
   }

   // load software cursor
	m_pkGui->SetCursor( 0,0, m_pkTexMan->Load("data/textures/gui/cursor.bmp", 0),
		m_pkTexMan->Load("data/textures/gui/cursor_a.bmp", 0), 32, 32);
   m_pkInput->ShowCursor(false);

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
		}
	}
}

void MistClient::Say(string strMsg)
{
	NetPacket kNp;			
	kNp.Write((char) MLNM_CS_SAY);
	kNp.Write_Str(strMsg);
	kNp.TargetSetClient(0);
	SendAppMessage(&kNp);									
}

void MistClient::RegisterResources()
{
	m_pkResourceDB->RegisterResource( string(".env"), Create__EnvSetting	);
}

void MistClient::RegisterPropertys()
{
	m_pkPropertyFactory->Register("P_ArcadeCharacter",	Create_P_ArcadeCharacter);
	m_pkPropertyFactory->Register("P_Enviroment", Create_P_Enviroment);
//	m_pkPropertyFactory->Register("P_ServerInfo", Create_P_ServerInfo);
	m_pkPropertyFactory->Register("P_Ml", Create_P_Ml);
}


void MistClient::OnIdle() 
{
	Input();
}

void MistClient::Input()
{
	//get mouse
	int x,z;		
	m_pkInputHandle->RelMouseXY(x,z);	
	
	//check buttons
	m_kCharacterControls[eUP] = 	m_pkInputHandle->Pressed(KEY_W);
	m_kCharacterControls[eDOWN] =	m_pkInputHandle->Pressed(KEY_S);			
	m_kCharacterControls[eLEFT] = m_pkInputHandle->Pressed(KEY_A);			
	m_kCharacterControls[eRIGHT]= m_pkInputHandle->Pressed(KEY_D);
	m_kCharacterControls[eJUMP] = m_pkInputHandle->Pressed(MOUSERIGHT);
	
	//update camera
	if(Entity* pkCharacter = m_pkEntityManager->GetEntityByID(m_iCharacterID))
	{
		if(P_Camera* pkCam = (P_Camera*)pkCharacter->GetProperty("P_Camera"))
		{			
			pkCam->Set3PYAngle(pkCam->Get3PYAngle() - (x/5.0));
			pkCam->Set3PPAngle(pkCam->Get3PPAngle() + (z/5.0));			
			pkCam->SetOffset(Vector3(0,1.0,0)); 

			float fDistance = pkCam->Get3PDistance();
			if(m_pkInputHandle->Pressed(MOUSEWUP)) 	fDistance -= 0.5;
			if(m_pkInputHandle->Pressed(MOUSEWDOWN))	fDistance += 0.5;
			
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
				
				if(P_Mad* pkMad = (P_Mad*)pkCharacter->GetProperty("P_Mad"))
					pkMad->SetVisible(true);	
			}			 
				
			pkCam->Set3PDistance(fDistance);
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
		//if theres no camera property, create one and set it up
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
			}
			
			pkEnt->SetInterpolate(true);
		}
		
		//setup enviroment
		if(!pkEnt->GetProperty("P_Enviroment"))
		{
			if(P_Enviroment* pkEnv = (P_Enviroment*)pkEnt->AddProperty("P_Enviroment"))
			{
				pkEnv->SetEnable(true);				
				pkEnv->SetEnviroment("data/enviroments/sun.env");
			}
		}
	}
}


void MistClient::OnHud(void) 
{	

}


void MistClient::OnClientStart(void)
{
	m_pkConsole->Printf("Trying to connect");
	
	m_iCharacterID = -1;
}

void MistClient::OnNetworkMessage(NetPacket *PkNetMessage)
{
	unsigned char ucType;

	// Read Type of Message.
	PkNetMessage->Read(ucType);

	switch(ucType)
	{
		case MLNM_SC_SETVIEW:
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

			break;

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
						
			break;
		}					
		
		default:
			cout << "Error in game packet : " << (int) ucType << endl;
			PkNetMessage->SetError(true);
			return;
	}
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
