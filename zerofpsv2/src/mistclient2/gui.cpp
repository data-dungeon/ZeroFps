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
   else
   if(msg == ZGM_KEYPRESS)
   {      
      if(win == NULL)
         win = g_kMistClient.GetWnd("GuiMainWnd");

      GuiType type = g_kMistClient.GetWndType(win);

      if(type == Wnd)
      {
         strMainWnd = strController = win->GetName();
      }
      else
      {
         strController = win->GetName();

         if(win->GetParent())
            strMainWnd = win->GetParent()->GetName();
         else
            strMainWnd = g_kMistClient.GetWnd("GuiMainWnd")->GetName();
      }
   }

   if(strController.empty())
      return false;

   if(strMainWnd == "MLStartWnd" || 
      strMainWnd == "ConnectWnd" || 
      strMainWnd == "AddNewServerWnd" ||
      strMainWnd == "LoginWnd" ||
      strMainWnd == "OptionsWnd")
   GuiMsgStartScreen(strMainWnd, strController, msg, numparms, params);

   if(strMainWnd == "GameGuiToolbar" || 
      strMainWnd == "ChatDlgMainWnd" ||
      strMainWnd == "GuiMainWnd")
   GuiMsgIngameScreen(strMainWnd, strController, msg, numparms, params);

   return true;
}