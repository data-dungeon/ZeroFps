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

MistClient g_kMistClient("MistClient",0,0,0);

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
   string strMainWnd;
   string strController;
   
   if(msg == ZGM_COMMAND)
   {
      strMainWnd = win->GetName();

	   list<ZGuiWnd*> kChilds;
	   win->GetChildrens(kChilds); 

	   list<ZGuiWnd*>::iterator it = kChilds.begin();
	   for( ; it!=kChilds.end(); it++)
	   {
		   if((*it)->GetID() == ((int*)params)[0])
		   {
            strController = (*it)->GetName();
			   break;
		   }
	   }
   }

   if(strController.empty())
      return false;

   if(msg == ZGM_COMMAND)
   {
      if(strMainWnd == "MLStartWnd")
      {
         if(strController == "StarNewGameBn")
         {
            g_kMistClient.ShowWnd("ConnectWnd", true, true, true);
            g_kMistClient.UpdateServerListbox();
         }
      }
      else
      if(strMainWnd == "ConnectWnd")
      {
         if(strController == "CloseServerWndBn")
            g_kMistClient.ShowWnd("ConnectWnd", false);
         else
         if(strController == "AddServerBn")
         {
            g_kMistClient.ShowWnd("AddNewServerWnd", true, true, true);
            g_kMistClient.SetText("NewServerNameEB", "");
            g_kMistClient.SetText("NewServerIPName", "");
         }
         else
         if(strController == "RemoveServerBn")
         {
            char* szSelItem = g_kMistClient.GetSelItem("ServerList");
            if( szSelItem )
            {
               string strText = string(szSelItem);

               int pos = (int) strText.find("-");

               string strName = strText.substr(0, pos-1);
               string strIP = strText.substr(pos+2, strText.length());

               g_kMistClient.AddRemoveServer(strName.c_str(), strIP.c_str(), false);
               g_kMistClient.UpdateServerListbox();
            }
         }
      }
      else
      if(strMainWnd == "AddNewServerWnd")
      {
         if(strController == "AddNewServerCancelBn")
         {
            g_kMistClient.ShowWnd("AddNewServerWnd", false);      
            g_kMistClient.ShowWnd("ConnectWnd", true, true, true);
         }
         else
         if(strController == "AddNewServerOK")
         {
            g_kMistClient.ShowWnd("AddNewServerWnd", false);
            g_kMistClient.ShowWnd("ConnectWnd", true, true, true);

            char* szName = g_kMistClient.GetText("NewServerNameEB");
            char* szIP = g_kMistClient.GetText("NewServerIPName");

            if( (szName != NULL && szIP != NULL) )
            {
               if( strlen(szName) > 0 && strlen(szIP) > 0)
               {
                  g_kMistClient.AddRemoveServer(szName, szIP);
                  g_kMistClient.UpdateServerListbox();
               }
            }
         }
      }
   }

   return true;
}


MistClient::MistClient(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	g_ZFObjSys.Log_Create("mistclient2");
	m_iViewFrom = -1;
} 
 
void MistClient::OnInit() 
{
	m_pkZFVFileSystem->AddRootPath( string("../datafiles/mistlands/") ,"data/");
	m_pkZFVFileSystem->AddRootPath( string("../datafiles/dm/") ,"data/");


	//initiate our mainview camera
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.25,250);	
	m_pkCamera->SetName("Main camera");
	m_pkFps->AddRenderCamera(m_pkCamera);

	//register property bös
	RegisterPropertys();

	//register resources
	RegisterResources();
		
	//init mistland script intreface
	MistLandLua::Init(m_pkObjectMan,m_pkScript);

   // init gui script intreface
	GuiAppLua::Init(&g_kMistClient, m_pkScript);

	//set window title		
   SetTitle("MistClient - Hacka och slå");
	
   // initialize gui system with default skins, font etc
	InitGui(m_pkScript, "defguifont", "data/script/gui/defskins.lua", NULL, false, true); 

   // load startup screen 
   LoadGuiFromScript("data/script/gui/ml_start.lua");

   // load software cursor
	m_pkGui->SetCursor( 0,0, m_pkTexMan->Load("data/textures/gui/cursor.bmp", 0),
		m_pkTexMan->Load("data/textures/gui/cursor_a.bmp", 0), 32, 32);
   m_pkInput->ShowCursor(false);

	//set client in server mode to show gui etc
	m_pkFps->StartServer(true,false);

	//run autoexec script
	if(!m_pkIni->ExecuteCommands("mistclient_autoexec.ini"))
		m_pkConsole->Printf("No game_autoexec.ini.ini found");	
}

	

void MistClient::RegisterResources()
{
	m_pkResourceDB->RegisterResource( string(".env"), Create__EnvSetting	);
}

void MistClient::RegisterPropertys()
{
	m_pkPropertyFactory->Register("P_ArcadeCharacter",	Create_P_ArcadeCharacter);
	m_pkPropertyFactory->Register("P_Enviroment", Create_P_Enviroment);
	m_pkPropertyFactory->Register("P_ServerInfo", Create_P_ServerInfo);
}


void MistClient::OnIdle() 
{

}

void MistClient::OnSystem() 
{
/*	if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_pkFps->GetClientObjectID()))
	{
		if(!pkEnt->GetProperty("P_Camera"))
		{
			if(P_Camera* pkCam = (P_Camera*)pkEnt->AddProperty("P_Camera"))
			{
				pkCam->SetCamera(m_pkCamera);
				cout<<"attached camera to client property"<<endl;
			}
		}
	}*/
}


void MistClient::OnHud(void) 
{	

}


void MistClient::OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass, bool bIsEditor)
{
	cout<<"Client "<<iConID<<" Joined"<<endl;
	
	cout << "Now adding er to client" << endl;
}

void MistClient::OnServerClientPart(ZFClient* pkClient,int iConID)
{
	cout<<"Client "<<iConID<<" Parted"<<endl;	
	
}


void MistClient::OnServerStart(void)
{		
}

void MistClient::OnClientStart(void)
{
	m_pkConsole->Printf("Trying to connect");
}

void MistClient::OnNetworkMessage(NetPacket *PkNetMessage)
{
	unsigned char ucType;

	// Read Type of Message.
	PkNetMessage->Read(ucType);
	//int iJiddra = ucType;
	//m_pkConsole->Printf("AppMessageType: %d", iJiddra );

	switch(ucType)
	{
		case MLNM_SC_SETVIEW:
			int iEntityID;
			PkNetMessage->Read(iEntityID);

			if(Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID( iEntityID ))
			{
				if(P_Camera* pkCam = (P_Camera*)pkEnt->AddProperty("P_Camera"))
				{
					//pkCam->SetType(CAM_TYPEFIRSTPERSON_NON_EA);
					pkCam->SetCamera(m_pkCamera);

					cout<<"attached camera to client property"<<endl;
				}
			}


			break;

		default:
			cout << "Error in game packet : " << (int) ucType << endl;
			PkNetMessage->SetError(true);
			return;
	}
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
            string strIP = strText.substr(pos+2, strText.length());

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