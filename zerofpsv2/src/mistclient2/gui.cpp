#include "mistclient.h"

extern MistClient g_kMistClient;

bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
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
         else
         if(strController == "QuitBn")
         {
            g_kMistClient.m_pkFps->QuitEngine();
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
               string strIP = strText.substr(pos+2, /*strText.length()*/strText.length()-pos);

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
            string strLogin, strPassword, strServerIP;

            char* text;
            if((text = g_kMistClient.GetSelItem("ServerList")))
            {
               strServerIP = string(text);
               int pos = (int) strServerIP.find("-");
               strServerIP = strServerIP.substr(pos+2, strServerIP.length()-pos);
               printf("strServerIP = %s\n", strServerIP.c_str());
            }

            if((text = g_kMistClient.GetText("LoginNameEB")))
            {
               strLogin = text;
               printf("strLogin = %s\n", strLogin.c_str());
            }

            if((text = g_kMistClient.GetText("LoginPWEb")))
            {
               strPassword = text;
               printf("strPassword = %s\n", strPassword.c_str());
            }

            if(!strLogin.empty() && !strPassword.empty() && !strServerIP.empty())
            {
               g_kMistClient.m_pkFps->StartClient(strLogin, strPassword, strServerIP);
            
               g_kMistClient.ShowWnd("MLStartWnd", false);
               g_kMistClient.ShowWnd("ConnectWnd", false);
               g_kMistClient.ShowWnd("AddNewServerWnd", false);
               g_kMistClient.ShowWnd("LoginWnd", false);
            }
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