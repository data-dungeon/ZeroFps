#include "mistclient.h"

extern MistClient g_kMistClient;

void GuiMsgStartScreen( string strMainWnd, string strController, 
                        unsigned int msg, int numparms, void *params ) 
{
   static bool s_bChangeServerName = false;

   ////////////////////////////////////////////////////////////////////////////
   // ZGM_COMMAND (hantera meddelanden för Klicka på knappar och annat)
   ////////////////////////////////////////////////////////////////////////////
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
         else
         if(strController == "OptionsBn")
         {
            g_kMistClient.LoadGuiFromScript("data/script/gui/ml_option.lua");
            g_kMistClient.ShowWnd("OptionsWnd", true, true, true);
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
            string strName, strIP;
            if(g_kMistClient.NameIPFromServerList(strName, strIP))
            {
               s_bChangeServerName = true;
               g_kMistClient.ShowWnd("AddNewServerWnd", true, true, true);

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
            string strName, strIP;
            if(g_kMistClient.NameIPFromServerList(strName, strIP))
            {
               string strTitle = string("Connect to ") + strName;
               g_kMistClient.SetText("LoginWndLabel", (char*)strTitle.c_str());

               g_kMistClient.SetText("LoginNameEB", (char*) g_kMistClient.m_strLoginName.c_str());
               g_kMistClient.SetText("LoginPWEb", (char*) g_kMistClient.m_strLoginPW.c_str());
               g_kMistClient.ShowWnd("LoginWnd", true, true, true);
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
                  g_kMistClient.m_pkZeroFps->StartClient(strLogin, strPassword, strServerIP);
                  g_kMistClient.m_strLoginName = strLogin;
                  g_kMistClient.m_strLoginPW = strPassword;
               }
               else
                  printf("Input error, failed to connect.");
            }
         }
         else
         if(strController == "LoginCancel")
         {
             g_kMistClient.ShowWnd("LoginWnd", false);
         }
      }
      else
      if(strMainWnd == "OptionsWnd")
      {
         if(strController == "OptionsWndOKBn")
         {
               g_kMistClient.ShowWnd("OptionsWnd", false);
         }
         else
         if(strController == "OptionsWndCancelBn")
         {
               g_kMistClient.ShowWnd("OptionsWnd", false);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // ZGM_KEYPRESS (hantera meddelanden för tagentbords)
   ////////////////////////////////////////////////////////////////////////////
   if(msg == ZGM_KEYPRESS)
   {
      if( ((int*)params)[0] == KEY_TAB)
      {  
         if( strController == "NewServerNameEB" )
            g_kMistClient.m_pkGui->SetFocus( g_kMistClient.GetWnd("NewServerIPName") ); 
         if( strController == "NewServerIPName" )
            g_kMistClient.m_pkGui->SetFocus( g_kMistClient.GetWnd("NewServerNameEB") ); 

         if( strController == "LoginPWEb" )
            g_kMistClient.m_pkGui->SetFocus( g_kMistClient.GetWnd("LoginNameEB") ); 
         if( strController == "LoginNameEB" )
            g_kMistClient.m_pkGui->SetFocus( g_kMistClient.GetWnd("LoginPWEb") ); 
      }
   }
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