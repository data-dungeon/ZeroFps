#include "mistclient.h"

extern MistClient g_kMistClient;

bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
   string strMainWnd;
   string strController;

   static bool s_bChangeServerName = false;
   
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
   else
   if(msg == ZGM_KEYPRESS)
   {
      string strWnd = win->GetName();

      if( ((int*)params)[0] == KEY_TAB)
      {  
         if( strWnd == "NewServerNameEB" )
            g_kMistClient.m_pkGui->SetFocus( g_kMistClient.GetWnd("NewServerIPName") ); 
         if( strWnd == "NewServerIPName" )
            g_kMistClient.m_pkGui->SetFocus( g_kMistClient.GetWnd("NewServerNameEB") ); 

         if( strWnd == "LoginPWEb" )
            g_kMistClient.m_pkGui->SetFocus( g_kMistClient.GetWnd("LoginNameEB") ); 
         if( strWnd == "LoginNameEB" )
            g_kMistClient.m_pkGui->SetFocus( g_kMistClient.GetWnd("LoginPWEb") ); 
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
         else
         if(strController == "QuitBn")
         {
            g_kMistClient.m_pkZeroFps->QuitEngine();
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
            s_bChangeServerName = false;
            g_kMistClient.ShowWnd("AddNewServerWnd", true, true, true);
            g_kMistClient.SetText("NewServerNameEB", "");
            g_kMistClient.SetText("NewServerIPName", "");
         }
         else
         if(strController == "EditServerBn")
         {
            s_bChangeServerName = true;
            g_kMistClient.ShowWnd("AddNewServerWnd", true, true, true);

            string strName, strIP;
            if(g_kMistClient.NameIPFromServerList(strName, strIP))
            {
               g_kMistClient.SetText("NewServerNameEB", (char*)strName.c_str());
               g_kMistClient.SetText("NewServerIPName", (char*)strIP.c_str());
            }
         }
         else
         if(strController == "RemoveServerBn")
         {
            string strName, strIP;
            if(g_kMistClient.NameIPFromServerList(strName, strIP))
            {
               g_kMistClient.AddRemoveServer(strName.c_str(), strIP.c_str(), false);
               g_kMistClient.UpdateServerListbox();
            }
         }
         else
         if(strController == "ConnectBn")
         {
            g_kMistClient.ShowWnd("LoginWnd", true, true, true);
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
                  if(s_bChangeServerName)
                  {
                     string szOLDName, szOLDIP;
                     if(g_kMistClient.NameIPFromServerList(szOLDName, szOLDIP))
                        g_kMistClient.AddRemoveServer(szOLDName.c_str(), szOLDIP.c_str(), false);
                  }

                  g_kMistClient.AddRemoveServer(szName, szIP);
                  g_kMistClient.UpdateServerListbox();
               }
            }
         }
      }
      else
      if(strMainWnd == "LoginWnd")
      {
         if(strController == "LoginOK")
         {
            string strLogin, strPassword, strServerName, strServerIP;

            char* text;

            if(g_kMistClient.NameIPFromServerList(strServerName, strServerIP))
            {
               if((text = g_kMistClient.GetText("LoginNameEB")))
                  strLogin = text;

               if((text = g_kMistClient.GetText("LoginPWEb")))
                  strPassword = text;

               if(!strLogin.empty() && !strPassword.empty() && !strServerIP.empty())
               {
                  g_kMistClient.m_pkFps->StartClient(strLogin, strPassword, strServerIP);
               
                  g_kMistClient.ShowWnd("MLStartWnd", false);
                  g_kMistClient.ShowWnd("ConnectWnd", false);
                  g_kMistClient.ShowWnd("AddNewServerWnd", false);
                  g_kMistClient.ShowWnd("LoginWnd", false);

                  printf("%s connecting to %s.", strLogin.c_str(), strServerIP.c_str());
               }
               else
                  printf("Input error, failed to connect.");
         }
         else
         if(strController == "LoginCancel")
         {
             g_kMistClient.ShowWnd("LoginWnd", false);
         }
      }
   }

   return true;
}

bool MistClient::NameIPFromServerList(string& strName, string& strIP)
{
   char* szSelItem = GetSelItem("ServerList");
   if( szSelItem )
   {
      string strText = string(szSelItem);

      int pos = (int) strText.find("-");

      strName = strText.substr(0, pos-1);
      strIP = strText.substr(pos+2, strText.length()-pos);
      return true;
   }

   return false;
}