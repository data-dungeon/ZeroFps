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
		}
		else
		if(strMainWnd == "ChatDlgMainWnd")
		{
			if(strController == "SayButton")
			{
				char*	text = g_kMistClient.GetText("SayTextbox");
				if(text != NULL && strlen(text) > 0)
				{
					g_kMistClient.Say(text);
					g_kMistClient.SetText("SayTextbox",	"");
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
			if( strController	==	"SayTextbox" )
			{
				((ZGuiTextbox*)g_kMistClient.GetWnd(strController))->KillFocus();
				if(ZGuiWnd::m_pkFocusWnd)
					ZGuiWnd::m_pkFocusWnd->KillFocus();
			}
		}
	}
}

void MistClient::ToogleChatWnd(bool	bOpen, bool	bSetInputFocus)
{
	if(bOpen	==	false)
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
		g_kMistClient.SetText("SayTextbox",	"");
		g_kMistClient.m_pkGui->SetFocus(	g_kMistClient.GetWnd("SayTextbox") ); 
	}
}

void MistClient::AddStringToChatBox(string strMsg)
{ 
	const int ROW_BUFFER_SIZE = 50;

	string strText	= string("<clr:255,255,255>") + 
		string(g_kMistClient.GetText("ChatTextbox")) + strMsg + string("\n");

	if(((ZGuiTextbox*)g_kMistClient.GetWnd("ChatTextbox"))->GetRowCount() >= ROW_BUFFER_SIZE)
	{
		size_t pos = strText.find("\n");
		if(pos != string::npos)
			strText.erase(0, pos+1);
	}

	g_kMistClient.SetText("ChatTextbox",(char*) strText.c_str());

	((ZGuiTextbox*)g_kMistClient.GetWnd("ChatTextbox"))->UpdateText();

	((ZGuiTextbox*)g_kMistClient.GetWnd("ChatTextbox"))->ScrollRowIntoView(
		((ZGuiTextbox*)g_kMistClient.GetWnd("ChatTextbox"))->GetRowCount());

	m_pkAudioSys->StartSound("data/sound/gui/turn_page.wav");

	
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

	Rect rcChatDlgMainWnd =	g_kMistClient.GetWnd("ChatDlgMainWnd")->GetScreenRect();
	Rect rcChatTextbox =	g_kMistClient.GetWnd("ChatTextbox")->GetScreenRect();

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

	g_kMistClient.GetWnd("ChatDlgMainWnd")->SetPos(rcChatDlgMainWnd.Left, rcChatDlgMainWnd.Top, true, true);
	g_kMistClient.GetWnd("ChatTextbox")->SetPos(rcChatTextbox.Left, rcChatTextbox.Top, true, true);

	g_kMistClient.GetWnd("ChatDlgMainWnd")->Resize(512, rcChatDlgMainWnd.Height());
	g_kMistClient.GetWnd("ChatTextbox")->Resize(512, rcChatTextbox.Height());

	g_kMistClient.GetWnd("SayTextbox")->Move(0,iVerticalOffset);
	g_kMistClient.GetWnd("SayButton")->Move(0,iVerticalOffset);

	unsigned	int min,max,pos;
	ZGuiScrollbar*	pkScrollbar	= ((ZGuiTextbox*)g_kMistClient.GetWnd("ChatTextbox"))->GetScrollbar();
	pkScrollbar->GetScrollInfo(min,max,pos);
	pkScrollbar->SetScrollPos(max-1);

	((ZGuiTextbox*)g_kMistClient.GetWnd("ChatTextbox"))->UpdateText();

	((ZGuiTextbox*)g_kMistClient.GetWnd("ChatTextbox"))->ScrollRowIntoView(
		((ZGuiTextbox*)g_kMistClient.GetWnd("ChatTextbox"))->GetRowCount());
}

void MistClient::LoadInGameGui()
{
	g_kMistClient.ShowWnd("MLStartWnd",	false);
	g_kMistClient.ShowWnd("ConnectWnd",	false);
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
	((ZGuiTextbox*) g_kMistClient.GetWnd("ChatTextbox"))->SetLeftAlignedScrollbar(true);

	g_kMistClient.GetWnd("ChatTextbox")->SetTextClr(255,255,255); 
	g_kMistClient.GetWnd("SayTextbox")->SetTextClr(255,255,255); 
	g_kMistClient.GetWnd("SayTextbox")->SetFont(g_kMistClient.m_pkGui->GetResMan()->Font("defguifont") );
	g_kMistClient.GetWnd("ChatTextbox")->SetFont(g_kMistClient.m_pkGui->GetResMan()->Font("chatboxfont") );

	g_kMistClient.GetWnd("ChatDlgMainWnd")->m_bUseAlhpaTest = false; // eftersom tex1a är helvit	(för att	kunna	ändra	trasparens med	färgvärdet)
	g_kMistClient.GetWnd("SayTextbox")->m_bUseAlhpaTest =	false;

	g_kMistClient.ShowWnd("OpenChatButton", false);
}
