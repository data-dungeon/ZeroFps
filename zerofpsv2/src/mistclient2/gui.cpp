#include	"mistclient.h"

extern MistClient	g_kMistClient;

bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms,	void *params )	
{
	string strMainWnd;
	string strController;
	
	if(msg == ZGM_COMMAND || msg == ZGM_SCN_SETPOS || msg == ZGM_CBN_SELENDOK)
	{
		strMainWnd = win->GetName();

		list<ZGuiWnd*> kChilds;
		win->GetChildrens(kChilds); 

		list<ZGuiWnd*>::iterator it = kChilds.begin();
		for( ; it!=kChilds.end(); it++)
			if((*it)->GetID() == ((int*)params)[0])
			{
				strController = (*it)->GetName();
				break;
			}
	}
	else
	if(msg == ZGM_KEYPRESS)
	{		 
		if(win == NULL)
			win =	g_kMistClient.GetWnd("GuiMainWnd");

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

//	if(g_kMistClient.GetWnd(strController.c_str())->GetParent())
//		strMainWnd = g_kMistClient.GetWnd(strController.c_str())->GetParent()->GetName();

	map<string,msgScreenProg>::iterator itCallback;
	itCallback = g_kMistClient.m_kGuiMsgProcs.find(strMainWnd);
	if(itCallback != g_kMistClient.m_kGuiMsgProcs.end())
		itCallback->second(strMainWnd, strController, msg,	numparms, params);

	return true;
}

void MistClient::SetupGUI()
{
	// initialize gui system with default skins, font etc
	g_kMistClient.InitGui(m_pkScript, "defguifont", "data/script/gui/defskins.lua", NULL, false, true); 

   char szFontData[512], szFontTex[512];
   sprintf(szFontData, "data/textures/gui/fonts/%s.fnt", "book_antiqua_10_bold_outlined");
   sprintf(szFontTex, "data/textures/gui/fonts/%s.tga", "book_antiqua_10_bold_outlined");

   ZGuiFont* font = new ZGuiFont("chatboxfont");
   font->Create(szFontData, m_pkTexMan->Load(szFontTex, 0), 1);
	g_kMistClient.m_pkGui->GetResMan()->Add("chatboxfont", font);

   // load startup screen 
   if(!g_kMistClient.LoadGuiFromScript("data/script/gui/ml_start.lua"))
	{
		printf("Error loading gui script!\n");
		return;
	}

	font = g_kMistClient.m_pkGui->GetResMan()->Font("chatboxfont");

	g_kMistClient.GetWnd("StarNewGameBn")->SetFont(font);
	g_kMistClient.GetWnd("StarNewGameBn")->SetTextColor(164,0,0); 

	font = g_kMistClient.m_pkGui->GetResMan()->Font("defguifont");

   g_kMistClient.GetWnd("ServerList")->SetFont(font); 
   g_kMistClient.GetWnd("LoginNameEB")->SetFont(font); 
   g_kMistClient.GetWnd("LoginPWEb")->SetFont(font); 
   g_kMistClient.GetWnd("NewServerIPName")->SetFont(font); 
   g_kMistClient.GetWnd("NewServerNameEB")->SetFont(font); 

   g_kMistClient.m_pkGui->AddKeyCommand(KEY_RETURN, GetWnd("NewServerNameEB"), GetWnd("AddNewServerOK"));
   g_kMistClient.m_pkGui->AddKeyCommand(KEY_RETURN, GetWnd("NewServerIPName"), GetWnd("AddNewServerOK"));
   g_kMistClient.m_pkGui->AddKeyCommand(KEY_ESCAPE, GetWnd("LoginWnd"), GetWnd("LoginCancel"));
   g_kMistClient.m_pkGui->AddKeyCommand(KEY_RETURN, GetWnd("LoginWnd"), GetWnd("LoginOK"));
   g_kMistClient.m_pkGui->AddKeyCommand(KEY_RETURN, GetWnd("LoginNameEB"), GetWnd("LoginOK"));
   g_kMistClient.m_pkGui->AddKeyCommand(KEY_RETURN, GetWnd("LoginPWEb"), GetWnd("LoginOK"));
   g_kMistClient.m_pkGui->AddKeyCommand(KEY_ESCAPE, GetWnd("ConnectWnd"), GetWnd("CloseServerWndBn"));
   g_kMistClient.m_pkGui->AddKeyCommand(KEY_RETURN, GetWnd("ConnectWnd"), GetWnd("ConnectBn"));
   g_kMistClient.m_pkGui->AddKeyCommand(KEY_ESCAPE, GetWnd("AddNewServerWnd"), GetWnd("AddNewServerCancelBn"));
   g_kMistClient.m_pkGui->AddKeyCommand(KEY_RETURN, GetWnd("AddNewServerWnd"), GetWnd("AddNewServerOK"));

	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("MLStartWnd", GuiMsgStartScreen));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("ConnectWnd", GuiMsgStartScreen));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("AddNewServerWnd", GuiMsgStartScreen));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("LoginWnd", GuiMsgStartScreen));

	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("OptionsWnd", GuiMsgOptionsDlg));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("OptionsPageVideo", GuiMsgOptionsDlg));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("OptionsPageGraphic", GuiMsgOptionsDlg));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("OptionsPageAudio", GuiMsgOptionsDlg));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("OptionsPageController", GuiMsgOptionsDlg));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("RestartMsgBox", GuiMsgOptionsDlg));
	
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("GameGuiToolbar", GuiMsgIngameScreen));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("ChatDlgMainWnd", GuiMsgIngameScreen));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("GuiMainWnd", GuiMsgIngameScreen));

   // load software cursor
	g_kMistClient.m_pkGui->SetCursor( 0,0, m_pkTexMan->Load("data/textures/gui/cursor.bmp", 0),
		m_pkTexMan->Load("data/textures/gui/cursor_a.bmp", 0), 32, 32);
	g_kMistClient.m_pkGui->ShowCursor(false); 
   g_kMistClient.m_pkInput->ShowCursor(false);

	GetWnd("ContinueGameBn")->Hide();
}