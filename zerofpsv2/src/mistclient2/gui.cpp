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
	  else
		if( ((int*)params)[0] == KEY_RETURN)
		{
			if( g_kMistClient.IsWndVisible("ToggleChatWnd") )
			{
				if( strWnd != "SayTextbox" )
				{
					g_kMistClient.ToogleChatWnd(!g_kMistClient.IsWndVisible("SayTextbox"));

					if(g_kMistClient.IsWndVisible("SayTextbox"))
					{
						g_kMistClient.m_pkGui->SetFocus( g_kMistClient.GetWnd("SayTextbox") ); 
					}
				}
				else
				{
					if( g_kMistClient.GetText("SayTextbox") != NULL && 
							strlen(g_kMistClient.GetText("SayTextbox")) > 0 )
					{
						string strMsg = string(g_kMistClient.GetText("SayTextbox"));	
						g_kMistClient.AddStringToChatBox(strMsg);
						g_kMistClient.Say(strMsg);
						g_kMistClient.SetText("SayTextbox", "");
						g_kMistClient.m_pkGui->SetFocus( g_kMistClient.GetWnd("SayTextbox") ); 
					}
				}
			}
		}
		else
		if( ((int*)params)[0] == KEY_ESCAPE)
		{
			if(strWnd == "ChatDlgMainWnd")
			{
				if(g_kMistClient.IsWndVisible("SayTextbox"))
				{
					g_kMistClient.ToogleChatWnd(false);
					if(ZGuiWnd::m_pkFocusWnd)
						ZGuiWnd::m_pkFocusWnd->KillFocus();
				}
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
            g_kMistClient.ShowWnd("LoginWnd", true, true, true);
            g_kMistClient.SetText("LoginNameEB", (char*) g_kMistClient.m_strLoginName.c_str());
            g_kMistClient.SetText("LoginPWEb", (char*) g_kMistClient.m_strLoginPW.c_str());
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
               
                  g_kMistClient.ShowWnd("MLStartWnd", false);
                  g_kMistClient.ShowWnd("ConnectWnd", false);
                  g_kMistClient.ShowWnd("AddNewServerWnd", false);
                  g_kMistClient.ShowWnd("LoginWnd", false);

				  g_kMistClient.LoadGuiFromScript("data/script/gui/ml_gamegui2.lua");

				  char text[] =
				  {
					"<clr:255,255,255>" \
					"0000: Starting ZeroFps Object System\n" \
					"0001: Register 'CmdSystem'ok\n" \
					"<clr:255,0,0>" \
					"0002: Register 'Application'ok\n" \
					"0003: Register 'ZeroFps'ok\n" \
					"0004: Register 'AStar'ok\n" \
					"0005: Register 'ZShader'ok\n" \
					"<clr:0,62,255>" \
					"<fnt:listboxfont>" \
					"0006: Register 'ZShaderSystem'ok\n" \
					"0007: Register 'EntityManager'ok\n" \
					"0008: Register 'ZFResourceDB'ok\n" \
					"0009: Register 'Gui'ok\n" \
					"0010: Register 'ZGuiResourceManager'ok\n" \
					"0011: Register 'ZGuiRender'ok\n" \
					"0012: Register 'NetWork'ok\n" \
					"0013: Register 'OggMusic'ok\n" \
					"0014: Register 'ZFAudioSystem'ok\n" \
					"0015: Register 'Console'ok\n" \
					"0016: Register 'Render'ok\n" \
					"0017: Register 'Light'ok\n" \
					"0018: Register 'PropertyFactory'ok\n" \
					"0019: Register 'Input'ok\n" \
					"0020: Register 'TextureManager'ok\n" \
					"0021: Register 'ZFVFileSystem'ok\n" \
					"0022: Register 'ZFBasicFS'ok\n" \
					"0023: Register 'ZFScriptSystem'ok\n"
				  };

				  ((ZGuiTextbox*) g_kMistClient.GetWnd("ChatTextbox"))->ToggleMultiLine(true);
				  ((ZGuiTextbox*) g_kMistClient.GetWnd("ChatTextbox"))->SetReadOnly(true); 
			//	  g_kMistClient.SetText("ChatTextbox", text);

				  ((ZGuiTextbox*) g_kMistClient.GetWnd("ChatTextbox"))->SetScrollbarSkin(
					  g_kMistClient.GetSkin("DefSBrBkSkin"),
					  g_kMistClient.GetSkin("DefSBrNSkin"), 
					  g_kMistClient.GetSkin("DefSBrFSkin"),
					  g_kMistClient.GetSkin("DefSBrScrollUpSkin_u"), 
					  g_kMistClient.GetSkin("DefSBrScrollUpSkin_d"),
					  g_kMistClient.GetSkin("DefSBrScrollDownSkin_u"), 
					  g_kMistClient.GetSkin("DefSBrScrollDownSkin_d"));

				  ((ZGuiTextbox*) g_kMistClient.GetWnd("ChatTextbox"))->GetScrollbar()->SetAutoHide(true); 

				  g_kMistClient.GetWnd("ChatTextbox")->SetTextClr(255,255,255); 
				  g_kMistClient.GetWnd("SayTextbox")->SetFont( 
					  g_kMistClient.m_pkGui->GetResMan()->Font("listboxfont") );

				  printf("%s connecting to %s.", strLogin.c_str(), strServerIP.c_str());

                  g_kMistClient.m_strLoginName = strLogin;
                  g_kMistClient.m_strLoginPW = strPassword;

				  g_kMistClient.GetWnd("ChatDlgMainWnd")->m_bUseAlhpaTest = false; // eftersom tex1a är helvit (för att kunna ändra trasparens med färgvärdet)

				  g_kMistClient.ShowWnd("SayTextbox", false);
				  g_kMistClient.ShowWnd("SayButton", false);

		/*		  g_kMistClient.m_pkGui->AddKeyCommand(KEY_RETURN, 
					  g_kMistClient.GetWnd("SayTextbox"), 
					  g_kMistClient.GetWnd("SayButton"));*/

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
	  if(strMainWnd == "GameGuiToolbar")
	  {
		  if(strController == "ToggleChatWnd")
		  {
			//if(g_kMistClient.IsWndVisible("SayTextbox") == true) // stänga ner Saybox?
			{
				g_kMistClient.ToogleChatWnd(!g_kMistClient.IsWndVisible("SayTextbox"));
			}
	/*		else
			{
				ToogleChatWnd(true);
			}*/
		  }
	  }
	  else
	  if(strMainWnd == "ChatDlgMainWnd")
	  {
		if(strController == "SayButton")
		{
			string szMsg = string(g_kMistClient.GetText("SayTextbox"));
			g_kMistClient.AddStringToChatBox(szMsg);
			g_kMistClient.Say(szMsg);
			g_kMistClient.SetText("SayTextbox", "");
			g_kMistClient.m_pkGui->SetFocus( g_kMistClient.GetWnd("SayTextbox") ); 
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

void MistClient::ToogleChatWnd(bool bOpen)
{
	if(bOpen == false)
	{
		Rect rc = g_kMistClient.GetWnd("ChatDlgMainWnd")->GetScreenRect();
		rc = rc.Move(0,24); // flytta ner fönstret
		g_kMistClient.GetWnd("ChatDlgMainWnd")->SetPos(rc.Left, rc.Top, true, true);

		g_kMistClient.ShowWnd("SayTextbox", false);
		g_kMistClient.ShowWnd("SayButton", false);
	}
	else
	{
		Rect rc = g_kMistClient.GetWnd("ChatDlgMainWnd")->GetScreenRect();
		rc = rc.Move(0,-24); // flytta upp fönstret
		g_kMistClient.GetWnd("ChatDlgMainWnd")->SetPos(rc.Left, rc.Top, true, true);

		g_kMistClient.ShowWnd("SayTextbox", true);
		g_kMistClient.ShowWnd("SayButton", true);
	}
}

void MistClient::AddStringToChatBox(string strMsg)
{
	string strText = string(g_kMistClient.GetText("ChatTextbox")) + strMsg + string("\n");
	g_kMistClient.SetText("ChatTextbox",(char*) strText.c_str());
 
	unsigned int min,max,pos;
	ZGuiScrollbar* pkScrollbar = ((ZGuiTextbox*)g_kMistClient.GetWnd("ChatTextbox"))->GetScrollbar();
	pkScrollbar->GetScrollInfo(min,max,pos);
	pkScrollbar->SetScrollPos(max-1);

	Rect rc = pkScrollbar->GetArrowButton(false)->GetScreenRect();
	pkScrollbar->GetButton()->SetPos(rc.Left,rc.Top-20,true,true);
}