#include	"mistclient.h"
#include "gui_optionsdlg.h"
#include "actionmenu.h"
#include "gui_inventory.h"
#include "gui_equipwnd.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../zerofpsv2/basic/zfbasicfs.h"

extern MistClient	g_kMistClient;

bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms,	void *params )	
{
	string strMainWnd;
	string strController;
	
	if(msg == ZGM_COMMAND || msg == ZGM_SCN_SETPOS || msg == ZGM_CBN_SELENDOK || msg == ZGM_EN_CHANGE)
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
	else
	if(msg == ZGM_MOUSEMOVE)
	{
		strMainWnd = win->GetName();

		if(win->IsVisible())
		{
			map<string,msgScreenProg>::iterator itCallback;
			itCallback = g_kMistClient.m_kGuiMsgProcs.find(strMainWnd);
			if(itCallback != g_kMistClient.m_kGuiMsgProcs.end())			
				itCallback->second(strMainWnd, "", ZGM_MOUSEMOVE, numparms, params);
		}
			
		return true;
	}
	
	if(strController.empty())
		return false;

	map<string,msgScreenProg>::iterator itCallback;
	itCallback = g_kMistClient.m_kGuiMsgProcs.find(strMainWnd);
	if(itCallback != g_kMistClient.m_kGuiMsgProcs.end())
		itCallback->second(strMainWnd, strController, msg,	numparms, params);

	return true;
}

void MistClient::InitGUIFonts()
{
	string strOldFont, strNewFont;
	char szFontData[512], szFontTex[512];
	int glyph = 0, w = GetWidth(), h = GetHeight();

	enum ResMode { small, normal, big, very_big, extra_big } eResMode = normal;

	if(w <=  640 && h <=  480) eResMode = small;
	if(w >=  800 && h >=  600) eResMode = normal;
	if(w >= 1024 && h >=  768) eResMode = big;
	if(w >= 1280 && h >= 1024) eResMode = very_big;
	if(w >= 1600 && h >= 1200) eResMode = extra_big;

	// Init default font
	switch(eResMode)
	{
		case small:     strNewFont = "verdana7"; break;
		case normal:    strNewFont = "verdana10"; break;
		case big:       strNewFont = "verdana12"; break;
		case very_big:  strNewFont = "verdana14"; break;
		case extra_big: strNewFont = "verdana16"; break;
	}

	glyph = 0;
	strOldFont = "defguifont";
	sprintf(szFontData, "data/textures/gui/fonts/%s.fnt", strNewFont.c_str());
	sprintf(szFontTex, "fonts/%s.tga", strNewFont.c_str());
	ZGuiFont* pkFont = new ZGuiFont((char*)strNewFont.c_str());
	if(pkFont->Create(szFontData, g_kMistClient.LoadGuiTextureByRes(szFontTex), glyph))
	{
		m_pkGui->GetResMan()->RemoveFont(strOldFont);
		m_pkGui->GetResMan()->Add(strOldFont, pkFont);	
	}

	// Init outlined font
	switch(eResMode)
	{
		case small:     strNewFont = "book_antiqua_outlined8"; break;
		case normal:    strNewFont = "book_antiqua_outlined10"; break;
		case big:       strNewFont = "book_antiqua_outlined12"; break;
		case very_big:  strNewFont = "book_antiqua_outlined14"; break;
		case extra_big: strNewFont = "book_antiqua_outlined16"; break;
	}
	
	glyph = 1;
	strOldFont = "book_antiqua_outlined10";
	sprintf(szFontData, "data/textures/gui/fonts/%s.fnt", strNewFont.c_str());
	sprintf(szFontTex, "fonts/%s.tga", strNewFont.c_str());
	pkFont = new ZGuiFont((char*)strNewFont.c_str());
	if(pkFont->Create(szFontData, g_kMistClient.LoadGuiTextureByRes(szFontTex), glyph))
	{
		m_pkGui->GetResMan()->RemoveFont(strOldFont);
		m_pkGui->GetResMan()->Add(strOldFont, pkFont);	
	}

	// Init small font
	switch(eResMode)
	{
		case small:     strNewFont = "small6"; break;
		case normal:    strNewFont = "small7"; break;
		case big:       strNewFont = "verdana8"; break;
		case very_big:  strNewFont = "verdana10"; break;
		case extra_big: strNewFont = "verdana12"; break;
	}
	
	glyph = 0;
	strOldFont = "small7";
	sprintf(szFontData, "data/textures/gui/fonts/%s.fnt", strNewFont.c_str());
	sprintf(szFontTex, "fonts/%s.tga", strNewFont.c_str());
	pkFont = new ZGuiFont((char*)strNewFont.c_str());
	if(pkFont->Create(szFontData, g_kMistClient.LoadGuiTextureByRes(szFontTex), glyph))
	{
		m_pkGui->GetResMan()->RemoveFont(strOldFont);
		m_pkGui->GetResMan()->Add(strOldFont, pkFont);	
	}
}

void MistClient::SetupGUI()
{
	// initialize gui system with default skins, font etc
	g_kMistClient.InitGui(m_pkScript, "defguifont", "data/script/gui/defskins.lua", 
		NULL, false, AUTO_SCALE); 

	// Create new font based on resolution.
	InitGUIFonts();

	// Search the gui script folder and find the correct script for this app
	// based on the resolution suffix of the file, like "800x600".
	FindGUIScriptsByResSuffix();

   // load startup screen 
   if(!g_kMistClient.LoadGuiFromScript(m_kGuiScrips[GSF_START].c_str()))
	{
		printf("Error loading gui script!\n");
		return;
	}

	m_pkOptionsDlg = new OptionsDlg(this);
	m_pkActionDlg = new ActionMenu();
	m_pkInventoryDlg = new InventoryDlg();
	m_pkEquipmentDlg = new EquipmentDlg();

	ZGuiFont* font = g_kMistClient.m_pkGui->GetResMan()->Font("book_antiqua_outlined10");

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
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("CharGen_SelectCharWnd", GuiMsgStartScreen));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("CharGen_CreateCharWnd", GuiMsgStartScreen));
	
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("OptionsWnd", GuiMsgOptionsDlg));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("OptionsPageVideo", GuiMsgOptionsDlg));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("OptionsPageGraphic", GuiMsgOptionsDlg));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("OptionsPageAudio", GuiMsgOptionsDlg));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("OptionsPageController", GuiMsgOptionsDlg));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("OptionsPageGame", GuiMsgOptionsDlg));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("RestartMsgBox", GuiMsgOptionsDlg));
	
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("ActionBar", GuiMsgIngameScreen));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("ChatDlgMainWnd", GuiMsgIngameScreen));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("GuiMainWnd", GuiMsgIngameScreen));

	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("ActionMenuMain", GuiMsgActionDlg));

	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("InventoryWnd", GuiMsgInventoryDlg));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("ContainerWnd", GuiMsgInventoryDlg));
	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("SplitStockWnd", GuiMsgInventoryDlg));

	g_kMistClient.m_kGuiMsgProcs.insert( map<string, msgScreenProg>::value_type("EquipWnd", GuiMsgEquipmentDlg));

	GetWnd("ContinueGameBn")->Hide();

   // load software cursor
	float w = g_kMistClient.GetScaleX()*64.0f, h = g_kMistClient.GetScaleY()*64.0f ;
	g_kMistClient.m_pkGui->SetCursor( 0,0, g_kMistClient.LoadGuiTextureByRes("cursor_sword.tga"), -1, w, h);
	g_kMistClient.m_pkGui->ShowCursor(false); 
   g_kMistClient.m_pkInput->ShowCursor(false);
	SetGuiCapture(true);
}


void MistClient::CloseActiveWindow()
{
	if(GetWnd("ChatDlgMainWnd")==NULL) // we have not loaded Ingame gui yet.
		return;

	printf("PRESSED ESC!\n");

	if(IsWndVisible("OptionsWnd"))
	{
		ShowWnd("OptionsWnd", 0,0,0);
	}
	else
	{
		LoadStartScreenGui(false);
		return;
	}

	if(IsWndVisible("MLStartWnd"))
		LoadInGameGui();
	
	if(IsWndVisible("SplitStockWnd"))
		m_pkInventoryDlg->CloseSplitStockWnd(); 
}


void MistClient::FindGUIScriptsByResSuffix()
{
	vector<string> vkFolders;
	m_pkZFVFileSystem->ListDir(&vkFolders, "data/script/gui/");

	int w = GetWidth();
	int h = GetHeight();

	int iNumOfPixelsInCurrentResolution = w*h;
	
	struct SEARCH_INFO
	{
		char strFileName[50];
		GUI_SCRIPT eScriptType;
	};

	SEARCH_INFO akInfo[] =
	{
		{ "ml_gamegui_", GSF_GAMEGUI },
		{ "ml_option_", GSF_OPTION },
		{ "ml_chargen_", GSF_CHARGEN },
		{ "ml_start_", GSF_START },
	};

	const int NUM_SCRIPS = sizeof(akInfo)/sizeof(akInfo[0]);
	
	for(int si=0; si<NUM_SCRIPS; si++)
	{
		int min_diff = 999999999;

		for(int i=0; i<vkFolders.size(); i++)
		{
			string strFile = vkFolders[i];

			// Find file exten.
			char *ext = strrchr( strFile.c_str(), '.');
			if(ext == NULL)		
				continue;

			if(strcmp(ext,".lua") != 0)
				continue;

			const char* szSearchString = akInfo[si].strFileName;

			int pos = strFile.find(szSearchString);

			for(int a=0; a<strlen(szSearchString); a++)
				if(strFile[a] != szSearchString[a]) {
					pos = -1; 
					break;
				}

			if(pos != string::npos && pos != -1 )
			{
				int p = strFile.find_last_of("x");
				if(p != string::npos)
				{			
					char szWidth[10];
					char szHeight[10];

					for(int j=0; j<10; j++)
					{
						int index = p - 1 - j;
						if( index >= 0 )
						{
							char ch = strFile[index];
							if(ch > 47 && ch < 58)
								szWidth[j] = ch;
							else
							{
								szWidth[j] = '\0';
								break;
							}
						}
					}

					for(int j=0; j<10; j++)
					{
						int index = p + 1 + j;
						if( index < strFile.size() )
						{
							char ch = strFile[index];
							if(ch > 47 && ch < 58)
								szHeight[j] = ch;
							else
							{
								szHeight[j] = '\0';
								break;
							}
						}
					}

					//char* rev = strrev(szWidth);
					//strcpy(szWidth, rev);
					StringReverse(szWidth);
					
					int sw = atoi(szWidth), sh = atoi(szHeight);
					int iNumOfPixels = sw*sh;
					int diff = abs(iNumOfPixelsInCurrentResolution-iNumOfPixels);

					if(diff < min_diff)
					{
						min_diff = diff;
						m_kGuiScrips[akInfo[si].eScriptType] = string("data/script/gui/") + strFile;
					}
				}
			}
		}
	}

	printf("m_kGuiScrips[GSF_GAMEGUI] = %s\n", m_kGuiScrips[GSF_GAMEGUI].c_str());
	printf("m_kGuiScrips[GSF_OPTION] = %s\n", m_kGuiScrips[GSF_OPTION].c_str());
	printf("m_kGuiScrips[GSF_CHARGEN] = %s\n", m_kGuiScrips[GSF_CHARGEN].c_str());
	printf("m_kGuiScrips[GSF_START] = %s\n", m_kGuiScrips[GSF_START].c_str());

}