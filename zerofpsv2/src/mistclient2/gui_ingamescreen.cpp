#include	"mistclient.h"
#include "gui_inventory.h"
#include "gui_equipwnd.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"

extern MistClient	g_kMistClient;



void GuiMsgIngameScreen( string strMainWnd, string	strController,	
								 unsigned int msg, int numparms,	void *params )	
{
	if(msg == ZGM_COMMAND)
	{
		if(strMainWnd == "ActionBar")
		{
			if(strController == "IngameBackBn")
			{
				g_kMistClient.LoadStartScreenGui(false);
			}
			else
			if(strController == "ToggleChatBn")
			{
				bool bOpen = !g_kMistClient.IsWndVisible("ChatDlgMainWnd");
				g_kMistClient.SetText("SayTextbox",	"");
				g_kMistClient.ToogleChatWnd(bOpen);
			}
			else
			if(strController == "ToggleInventoryBn")
			{
				bool bOpen = !g_kMistClient.IsWndVisible("InventoryWnd");
				if(bOpen)
					g_kMistClient.RequestOpenInventory(); 
				else
					g_kMistClient.m_pkInventoryDlg->Close(); 

			}
			else
			if(strController == "ToggleEquipBn")
			{
				bool bOpen = !g_kMistClient.IsWndVisible("EquipWnd");
				if(bOpen)
					g_kMistClient.SendRequestOpenEqipment();
				else
					g_kMistClient.m_pkEquipmentDlg->Close();
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
			if(strController == "OpenEmoteListBn")
			{
				g_kMistClient.ToggleEmoteList(!g_kMistClient.GetWnd("EmoteWnd")->IsVisible());
			}
			//else
			//if(strController == "ChangeSizeUpChatButton")
			//{
			//	g_kMistClient.ResizeChatDlg(true);
			//}
			//else
			//if(strController == "ChangeSizeDownChatButton")
			//{
			//	g_kMistClient.ResizeChatDlg(false);
			//}
		}
	}

	if(msg == ZGM_SELECTLISTITEM)
	{
		if(strMainWnd == "EmoteWnd");
		{
			if(strController == "EmoteList")
			{
				string strSelItem = g_kMistClient.GetSelItem("EmoteList");
				g_kMistClient.SendTaunt(strSelItem);
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
				g_kMistClient.SetGuiCapture(false);
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
				g_kMistClient.SetGuiCapture(false);				
				g_kMistClient.m_pkGui->m_bHandledMouse = false;
			}

			g_kMistClient.m_pkGui->m_bForceGUICapture = false;
		}
		//else
		//if( ((int*)params)[0] == KEY_P)
		//{
		//	if( strController	!=	"SayTextbox" )
		//	{
		//		if(!g_kMistClient.m_pkInventoryDlg->IsVisible())
		//			g_kMistClient.m_pkInventoryDlg->Open(); 
		//	}
		//}
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
		SetGuiCapture(true);
	}
}

void MistClient::ToggleEmoteList(bool bOpen)
{
	if(bOpen)
	{
		m_pkGui->PlaceWndFrontBack(GetWnd("EmoteWnd"), true);
		GetWnd("EmoteWnd")->Show();
	}
	else
	{
		m_pkGui->PlaceWndFrontBack(GetWnd("EmoteWnd"), false);
		GetWnd("EmoteWnd")->Hide();
	}

	((ZGuiListbox*)GetWnd("EmoteList"))->m_bCanHaveKeyboarFocus = false;

	ClearListbox("EmoteList");

	for(int i=0; i<m_kEmotes.size(); i++)
		AddListItem("EmoteList", (char*)m_kEmotes[i].c_str(), false);
	
}

void MistClient::AddStringToChatBox(string strMsg)
{ 
	ZGuiTextbox* pkChatEb = (ZGuiTextbox*)GetWnd("ChatTextbox");
	if(pkChatEb == NULL)
		return;

	const int ROW_BUFFER_SIZE = 50;
	const int MAX_NUM_VISIBLE_ROWS = pkChatEb->GetScreenRect().Height() / pkChatEb->GetFont()->m_iRowHeight;

	string strText	= string("<clr:255,255,255>") + 
		string(GetText("ChatTextbox")) + strMsg + string("\n");

	int rows = pkChatEb->GetRowCount();

	if(rows >= ROW_BUFFER_SIZE)
	{
		size_t pos = strText.find("\n");
		if(pos != string::npos)
			strText.erase(0, pos+1);
	}

	//if(rows < MAX_NUM_VISIBLE_ROWS)
	//{
	//	int saybox_h = GetWnd("SayTextbox")->GetScreenRect().Height();
	//	int h = (rows+1) * pkChatEb->GetFont()->m_iRowHeight;
	//	pkChatEb->SetPos(0, pkChatEb->GetParent()->GetScreenRect().Height()-h-saybox_h, false, true);
	//}
	//else
	//{
	//	pkChatEb->SetPos(0, 0, false, true);
	//}

	SetText("ChatTextbox",(char*) strText.c_str());

	pkChatEb->UpdateText();

	pkChatEb->ScrollRowIntoView(pkChatEb->GetRowCount());

	m_pkAudioSys->PlayAudio("data/sound/gui/turn_page.wav", Vector3(), Vector3(), ZFAUDIO_2D);	
}

/*void MistClient::ResizeChatDlg(bool	bMakeBigger)
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
}*/

void MistClient::LoadInGameGui()
{
	SetGuiCapture(false);

	ShowWnd("MLStartWnd",	false);
	ShowWnd("ConnectWnd",	false);
	ShowWnd("AddNewServerWnd", false);
	ShowWnd("LoginWnd", false);
	ShowWnd("RestartMsgBox", false);
	ShowWnd("CharGen_SelectCharWnd", false);

	if(!LoadGuiFromScript(m_kGuiScrips[GSF_GAMEGUI].c_str()))
	{
		printf("Failed to load GUI script!\n");
		return;
	}

	InitBuffWnd();

	((ZGuiTextbox*) GetWnd("ChatTextbox"))->ToggleMultiLine(true);
	((ZGuiTextbox*) GetWnd("ChatTextbox"))->GetScrollbar()->Resize((int)(16.0f*GetScaleX()), -1);  
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

	//GetWnd("ChatDlgMainWnd")->m_bUseAlhpaTest = false; // eftersom tex1a är helvit	(för att	kunna	ändra	trasparens med	färgvärdet)
	GetWnd("SayTextbox")->m_bUseAlhpaTest =	false;

	ShowWnd("OpenChatButton", false);

	m_pkGui->m_bForceGUICapture = false;

	((ZGuiTextbox*)GetWnd("ChatTextbox"))->UpdateText();
	((ZGuiTextbox*)GetWnd("ChatTextbox"))->ScrollRowIntoView(
		((ZGuiTextbox*)GetWnd("ChatTextbox"))->GetRowCount());

	m_pkGui->SetFocus(GetWnd("GuiMainWnd"), false);
}

void MistClient::InitBuffWnd()
{
	static bool s_bInitialized = false;

	if(s_bInitialized == true)
		return;

	char szName[25];
	int w=32, h=32, pbh = 8, mrg = 2, x = mrg, y = 0;
	ZGuiWnd* pkBuffWnd = GetWnd("BuffWnd");

	for(int i=0; i<MAX_NUM_BUFF_ICONS; i++)
	{
		sprintf(szName, "BuffIcon%i", i);

		m_kBuffIcons[i].m_pkWnd = (ZGuiLabel*) CreateWnd(Label, szName, "", pkBuffWnd, x, y, w, h, 0);
		m_kBuffIcons[i].m_pkWnd->Hide();
		m_kBuffIcons[i].m_fStartTime = 0;
		m_kBuffIcons[i].m_fTimeout = 0;

		ZGuiSkin* pkSkin = new ZGuiSkin();
		m_kBuffIcons[i].m_pkWnd->SetSkin(pkSkin);

		sprintf(szName, "BuffProgressbar%i", i);

		y += h;

		m_kBuffIcons[i].m_pkProgressBar = (ZGuiProgressbar*) 
			CreateWnd(Progressbar, szName, "", pkBuffWnd, x, y, w, pbh, 0);
		m_kBuffIcons[i].m_pkProgressBar->Hide();
		m_kBuffIcons[i].m_pkProgressBar->SetTextType(PBTEXTTYPE_ONLYPOS); 
		m_kBuffIcons[i].m_pkProgressBar->SetDir(PBDIR_RIGHT_TO_LEFT); 
		g_kMistClient.SetFont(szName, "small7", 255, 255, 255, 0);

		y += (pbh + mrg);

		if((y + h) > (int)(float)pkBuffWnd->GetScreenRect().Height() / g_kMistClient.GetScaleY())
		{
			x += (w + mrg);
			y = 0;
		}
	}

	s_bInitialized = true;
}

void MistClient::RebuildBuffIconList(vector<BUFF_ICON_INFO>* kList)
{
	ZGuiWnd* pkBuffWnd = GetWnd("BuffWnd");

	if(pkBuffWnd == NULL)
	{
		printf("!!!Tried to update buff icon list when parentwnd dont exist!!!\n");
		return;
	}

	string strImageFile;

	for(int i=0; i<MAX_NUM_BUFF_ICONS; i++)
	{
		if(i<kList->size())
		{
			m_kBuffIcons[i].m_pkWnd->Show();
			m_kBuffIcons[i].m_pkProgressBar->Show();
			m_kBuffIcons[i].m_strIcon = string("buffs/") + (*kList)[i].m_strIcon;			
			m_kBuffIcons[i].m_strName = (*kList)[i].m_strName;
			m_kBuffIcons[i].m_cType = (*kList)[i].m_cType;
			m_kBuffIcons[i].m_fTimeout = (*kList)[i].m_fTimeout;			
			m_kBuffIcons[i].m_fStartTime = m_pkZeroFps->GetTicks();
			m_kBuffIcons[i].m_pkProgressBar->SetRange(0, m_kBuffIcons[i].m_fTimeout);

			m_kBuffIcons[i].m_fTimeLeft = (*kList)[i].m_fTimeLeft;
			if(m_kBuffIcons[i].m_fTimeLeft == -1) 
				m_kBuffIcons[i].m_fTimeLeft = m_kBuffIcons[i].m_fTimeout;				//ful fix
			
			m_kBuffIcons[i].m_pkWnd->GetSkin()->m_iBkTexID = 
				LoadGuiTextureByRes(m_kBuffIcons[i].m_strIcon);	
				
			// visa inte progressbaren om timeout är -1
			if(m_kBuffIcons[i].m_fTimeout == -1)
				m_kBuffIcons[i].m_pkProgressBar->Hide();
		}
		else
		{
			m_kBuffIcons[i].m_pkWnd->Hide();
			m_kBuffIcons[i].m_pkProgressBar->Hide();
		}
	}

	m_iNumBuffIcons = kList->size();
}

void MistClient::UpdateBuffIconList()
{
	for(int i=0; i<m_iNumBuffIcons; i++)
	{
		if(m_kBuffIcons[i].m_fTimeout == -1)
			continue;
		
		float fDiff = (m_pkZeroFps->GetTicks() - m_kBuffIcons[i].m_fStartTime) + 
			(m_kBuffIcons[i].m_fTimeout - m_kBuffIcons[i].m_fTimeLeft)  ;		
		m_kBuffIcons[i].m_pkProgressBar->SetPos((int)fDiff);		
	}
}