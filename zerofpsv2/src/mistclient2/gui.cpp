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
      if(win == NULL)
         win = g_kMistClient.GetWnd("GuiMainWnd");

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
         if( strWnd == "SayTextbox" )
         {
            char* text = g_kMistClient.GetText("SayTextbox");
            if(text != NULL && strlen(text) > 0)
            {
               g_kMistClient.Say(text);
               g_kMistClient.SetText("SayTextbox", "");
            }

            ZGuiWnd::m_pkFocusWnd->KillFocus();
            g_kMistClient.m_pkGui->SetFocus(g_kMistClient.GetWnd("GuiMainWnd"));
         }
         else
         {
            g_kMistClient.ToogleChatWnd(true, true);
         }
		}
		else
		if( ((int*)params)[0] == KEY_ESCAPE)
		{
			if( strWnd == "SayTextbox" )
			{
            ((ZGuiTextbox*)win)->KillFocus();
            if(ZGuiWnd::m_pkFocusWnd)
					ZGuiWnd::m_pkFocusWnd->KillFocus();
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
	  if(strMainWnd == "GameGuiToolbar")
	  {
		  if(strController == "OpenChatButton")
		  {
            g_kMistClient.SetText("SayTextbox", "");
				g_kMistClient.ToogleChatWnd(true);
		  }
	  }
	  else
	  if(strMainWnd == "ChatDlgMainWnd")
	  {
		if(strController == "SayButton")
		{
         char* text = g_kMistClient.GetText("SayTextbox");
         if(text != NULL && strlen(text) > 0)
         {
            g_kMistClient.Say(text);
            g_kMistClient.SetText("SayTextbox", "");
            ZGuiWnd::m_pkFocusWnd->KillFocus();
            g_kMistClient.m_pkGui->SetFocus(g_kMistClient.GetWnd("GuiMainWnd"));
         }
		}
      else
      if(strController == "CloseChatButton")
      {
         g_kMistClient.ToogleChatWnd(false);
      }
      else
      if(strController == "ChangeSizeUpChatButton")
      {
         g_kMistClient.ResizeChatDlg(true);
      }
      else
      if(strController == "ChangeSizeDownChatButton")
      {
         g_kMistClient.ResizeChatDlg(false);
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

void MistClient::ToogleChatWnd(bool bOpen, bool bSetInputFocus)
{
	if(bOpen == false)
	{
		g_kMistClient.ShowWnd("ChatDlgMainWnd", false);
      g_kMistClient.ShowWnd("OpenChatButton", true);
	}
	else
	{
		g_kMistClient.ShowWnd("ChatDlgMainWnd", true);
      g_kMistClient.ShowWnd("OpenChatButton", false);
	}

   if(bSetInputFocus)
   {
      g_kMistClient.SetText("SayTextbox", "");
      g_kMistClient.m_pkGui->SetFocus( g_kMistClient.GetWnd("SayTextbox") ); 
   }
}

void MistClient::AddStringToChatBox(string strMsg)
{ 
	string strText = string(g_kMistClient.GetText("ChatTextbox")) + strMsg + string("\n");
	g_kMistClient.SetText("ChatTextbox",(char*) strText.c_str());

   ((ZGuiTextbox*)g_kMistClient.GetWnd("ChatTextbox"))->UpdateText();

   ((ZGuiTextbox*)g_kMistClient.GetWnd("ChatTextbox"))->ScrollRowIntoView(
      ((ZGuiTextbox*)g_kMistClient.GetWnd("ChatTextbox"))->GetRowCount());
}

void MistClient::ResizeChatDlg(bool bBigger)
{
   static unsigned char s_byCurrenSize = 0;

   if(bBigger)
   {
      if(s_byCurrenSize == 2)
         return;

      s_byCurrenSize++;
   }
   else
   {
      if(s_byCurrenSize == 0)
         return;

      s_byCurrenSize--;
   }

   Rect rcChatDlgMainWnd = g_kMistClient.GetWnd("ChatDlgMainWnd")->GetScreenRect();
   Rect rcChatTextbox = g_kMistClient.GetWnd("ChatTextbox")->GetScreenRect();

   const int RESIZE_VALUE = 150;
   int iVerticalOffset = 0;

   if(bBigger)
   {
      rcChatDlgMainWnd.Top -= RESIZE_VALUE;
      rcChatTextbox.Top -= RESIZE_VALUE;
      iVerticalOffset = RESIZE_VALUE;
   }
   else
   {
      rcChatDlgMainWnd.Top += RESIZE_VALUE;
      rcChatTextbox.Top += RESIZE_VALUE;
      iVerticalOffset = -RESIZE_VALUE;
   }

   g_kMistClient.GetWnd("ChatDlgMainWnd")->SetPos(rcChatDlgMainWnd.Left, rcChatDlgMainWnd.Top, true, true);
   g_kMistClient.GetWnd("ChatTextbox")->SetPos(rcChatTextbox.Left, rcChatTextbox.Top, true, true);

   g_kMistClient.GetWnd("ChatDlgMainWnd")->Resize(512, rcChatDlgMainWnd.Height());
   g_kMistClient.GetWnd("ChatTextbox")->Resize(512, rcChatTextbox.Height());

   g_kMistClient.GetWnd("SayTextbox")->Move(0,iVerticalOffset);
   g_kMistClient.GetWnd("SayButton")->Move(0,iVerticalOffset);

	unsigned int min,max,pos;
	ZGuiScrollbar* pkScrollbar = ((ZGuiTextbox*)g_kMistClient.GetWnd("ChatTextbox"))->GetScrollbar();
	pkScrollbar->GetScrollInfo(min,max,pos);
	pkScrollbar->SetScrollPos(max-1);

   ((ZGuiTextbox*)g_kMistClient.GetWnd("ChatTextbox"))->UpdateText();

   ((ZGuiTextbox*)g_kMistClient.GetWnd("ChatTextbox"))->ScrollRowIntoView(
      ((ZGuiTextbox*)g_kMistClient.GetWnd("ChatTextbox"))->GetRowCount());
}

void MistClient::LoadInGameGui()
{
	g_kMistClient.ShowWnd("MLStartWnd", false);
	g_kMistClient.ShowWnd("ConnectWnd", false);
	g_kMistClient.ShowWnd("AddNewServerWnd", false);
	g_kMistClient.ShowWnd("LoginWnd", false);

	g_kMistClient.LoadGuiFromScript("data/script/gui/ml_gamegui2.lua");

	((ZGuiTextbox*) g_kMistClient.GetWnd("ChatTextbox"))->ToggleMultiLine(true);
	((ZGuiTextbox*) g_kMistClient.GetWnd("ChatTextbox"))->SetReadOnly(true); 

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
	g_kMistClient.GetWnd("SayTextbox")->SetTextClr(255,255,255); 
	g_kMistClient.GetWnd("SayTextbox")->SetFont( 
		g_kMistClient.m_pkGui->GetResMan()->Font("listboxfont") );
   g_kMistClient.GetWnd("ChatTextbox")->SetFont( 
					  g_kMistClient.m_pkGui->GetResMan()->Font("listboxfont") );

	g_kMistClient.GetWnd("ChatDlgMainWnd")->m_bUseAlhpaTest = false; // eftersom tex1a är helvit (för att kunna ändra trasparens med färgvärdet)
	g_kMistClient.GetWnd("SayTextbox")->m_bUseAlhpaTest = false;

	g_kMistClient.ShowWnd("OpenChatButton", false);
}