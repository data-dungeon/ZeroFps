#include	"mistclient.h"

extern MistClient	g_kMistClient;

void GuiMsgIngameScreen( string strMainWnd, string	strController,	
								 unsigned int msg, int numparms,	void *params )	
{
	if(msg == ZGM_COMMAND)
	{
		if(strMainWnd == "GameGuiToolbar")
		{
			if(strController == "OpenChatButton")
			{
				g_kMistClient.SetText("SayTextbox",	"");
				g_kMistClient.ToogleChatWnd(true);
			}
			else
			if(strController == "IngameBackBn")
			{
				g_kMistClient.LoadStartScreenGui(false);
			}
		}
		else
		if(strMainWnd == "ChatDlgMainWnd")
		{
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

	////////////////////////////////////////////////////////////////////////////
	//	ZGM_KEYPRESS (hantera meddelanden för tagentbordsknappar)
	////////////////////////////////////////////////////////////////////////////
	if(msg == ZGM_KEYPRESS)
	{
		if( ((int*)params)[0] == KEY_RETURN)
		{
			if( strController	==	"SayTextbox" )
			{
				char*	text = g_kMistClient.GetText("SayTextbox");
				if(text != NULL && strlen(text) > 0)
				{
					g_kMistClient.Say(text);
					g_kMistClient.SetText("SayTextbox",	"");
				}

				ZGuiWnd::m_pkFocusWnd->KillFocus();
				g_kMistClient.m_pkGui->SetFocus(g_kMistClient.GetWnd("GuiMainWnd"), false);				
				g_kMistClient.ToggleGuiCapture(0);
				g_kMistClient.m_pkGui->m_bHandledMouse = false;
			}
			else
			{
				g_kMistClient.ToogleChatWnd(true, true);
			}
		}
		else
		if( ((int*)params)[0] == KEY_ESCAPE)
		{
			if( strController	==	"SayTextbox" )
			{
				((ZGuiTextbox*)g_kMistClient.GetWnd(strController))->KillFocus();
				if(ZGuiWnd::m_pkFocusWnd)
					ZGuiWnd::m_pkFocusWnd->KillFocus();
				g_kMistClient.ToggleGuiCapture(0);				
				g_kMistClient.m_pkGui->m_bHandledMouse = false;
			}

			g_kMistClient.m_pkGui->m_bForceGUICapture = false;
		}
	}
}

void MistClient::ToogleChatWnd(bool	bOpen, bool	bSetInputFocus)
{
	if(bOpen	==	false)
	{
		ShowWnd("ChatDlgMainWnd", false);
		ShowWnd("OpenChatButton", true);
	}
	else
	{
		ShowWnd("ChatDlgMainWnd", true);
		ShowWnd("OpenChatButton", false);
	}

	if(bSetInputFocus)
	{
		SetText("SayTextbox",	"");
		m_pkGui->SetFocus( g_kMistClient.GetWnd("SayTextbox"), true); 
		ToggleGuiCapture(1);
	}
}

void MistClient::AddStringToChatBox(string strMsg)
{ 
	const int ROW_BUFFER_SIZE = 50;

	string strText	= string("<clr:255,255,255>") + 
		string(GetText("ChatTextbox")) + strMsg + string("\n");

	if(((ZGuiTextbox*)GetWnd("ChatTextbox"))->GetRowCount() >= ROW_BUFFER_SIZE)
	{
		size_t pos = strText.find("\n");
		if(pos != string::npos)
			strText.erase(0, pos+1);
	}

	SetText("ChatTextbox",(char*) strText.c_str());

	((ZGuiTextbox*)GetWnd("ChatTextbox"))->UpdateText();

	((ZGuiTextbox*)GetWnd("ChatTextbox"))->ScrollRowIntoView(
		((ZGuiTextbox*)GetWnd("ChatTextbox"))->GetRowCount());

	m_pkAudioSys->PlayAudio("data/sound/gui/turn_page.wav", Vector3(), Vector3(), ZFAUDIO_2D);

	
}

void MistClient::ResizeChatDlg(bool	bMakeBigger)
{
	static unsigned char	s_byCurrenSize	= 0;

	if(bMakeBigger)
	{
		if(s_byCurrenSize	==	2)
			return;

		s_byCurrenSize++;
	}
	else
	{
		if(s_byCurrenSize	==	0)
			return;

		s_byCurrenSize--;
	}

	Rect rcChatDlgMainWnd =	GetWnd("ChatDlgMainWnd")->GetScreenRect();
	Rect rcChatTextbox =	GetWnd("ChatTextbox")->GetScreenRect();

	const	int RESIZE_VALUE = 150;
	int iVerticalOffset = 0;

	if(bMakeBigger)
	{
		rcChatDlgMainWnd.Top	-=	RESIZE_VALUE;
		rcChatTextbox.Top	-=	RESIZE_VALUE;
		iVerticalOffset =	RESIZE_VALUE;
	}
	else
	{
		rcChatDlgMainWnd.Top	+=	RESIZE_VALUE;
		rcChatTextbox.Top	+=	RESIZE_VALUE;
		iVerticalOffset =	-RESIZE_VALUE;
	}

	GetWnd("ChatDlgMainWnd")->SetPos(rcChatDlgMainWnd.Left, rcChatDlgMainWnd.Top, true, true);
	GetWnd("ChatTextbox")->SetPos(rcChatTextbox.Left, rcChatTextbox.Top, true, true);

	GetWnd("ChatDlgMainWnd")->Resize(512, rcChatDlgMainWnd.Height());
	GetWnd("ChatTextbox")->Resize(512, rcChatTextbox.Height());

	GetWnd("SayTextbox")->Move(0,iVerticalOffset);

	unsigned	int min,max,pos;
	ZGuiScrollbar*	pkScrollbar	= ((ZGuiTextbox*)GetWnd("ChatTextbox"))->GetScrollbar();
	pkScrollbar->GetScrollInfo(min,max,pos);
	pkScrollbar->SetScrollPos(max-1);

	((ZGuiTextbox*)GetWnd("ChatTextbox"))->UpdateText();

	((ZGuiTextbox*)GetWnd("ChatTextbox"))->ScrollRowIntoView(
		((ZGuiTextbox*)GetWnd("ChatTextbox"))->GetRowCount());
}

void MistClient::LoadInGameGui()
{
	ToggleGuiCapture(0);

	ShowWnd("MLStartWnd",	false);
	ShowWnd("ConnectWnd",	false);
	ShowWnd("AddNewServerWnd", false);
	ShowWnd("LoginWnd", false);
	ShowWnd("RestartMsgBox", false);

	if(!LoadGuiFromScript("data/script/gui/ml_gamegui.lua"))
	{
		printf("Failed to load GUI script!\n");
		return;
	}

	((ZGuiTextbox*) GetWnd("ChatTextbox"))->ToggleMultiLine(true);
	((ZGuiTextbox*) GetWnd("ChatTextbox"))->SetReadOnly(true); 

	((ZGuiTextbox*) GetWnd("ChatTextbox"))->SetScrollbarSkin(
		GetSkin("DefSBrBkSkin"),
		GetSkin("DefSBrNSkin"),
		GetSkin("DefSBrFSkin"),
		GetSkin("DefSBrScrollUpSkin_u"),
		GetSkin("DefSBrScrollUpSkin_d"),
		GetSkin("DefSBrScrollDownSkin_u"),
		GetSkin("DefSBrScrollDownSkin_d"));

	((ZGuiTextbox*) GetWnd("ChatTextbox"))->GetScrollbar()->SetAutoHide(true); 
	((ZGuiTextbox*) GetWnd("ChatTextbox"))->SetLeftAlignedScrollbar(true);

	GetWnd("ChatTextbox")->SetTextColor(255,255,255); 
	GetWnd("SayTextbox")->SetTextColor(255,255,255); 
	GetWnd("SayTextbox")->SetFont(m_pkGui->GetResMan()->Font("defguifont") );
	GetWnd("ChatTextbox")->SetFont(m_pkGui->GetResMan()->Font("chatboxfont") );

	GetWnd("ChatDlgMainWnd")->m_bUseAlhpaTest = false; // eftersom tex1a är helvit	(för att	kunna	ändra	trasparens med	färgvärdet)
	GetWnd("SayTextbox")->m_bUseAlhpaTest =	false;

	ShowWnd("OpenChatButton", false);

	m_pkGui->m_bForceGUICapture = false;

	((ZGuiTextbox*)GetWnd("ChatTextbox"))->UpdateText();
	((ZGuiTextbox*)GetWnd("ChatTextbox"))->ScrollRowIntoView(
		((ZGuiTextbox*)GetWnd("ChatTextbox"))->GetRowCount());

	m_pkGui->SetFocus(GetWnd("GuiMainWnd"), false);
}
