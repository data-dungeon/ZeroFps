
#include "mistclient.h"
#include "gui_equipwnd.h"
#include "../mcommon/p_characterproperty.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/engine/inputhandle.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"

extern MistClient	g_kMistClient;

void GuiMsgEquipmentDlg( string strMainWnd, string strController, 
								unsigned int msg, int numparms, void *params )
{
	if(msg == ZGM_COMMAND)
	{
		if(strMainWnd == "EquipmentDlg")
			g_kMistClient.m_pkEquipmentDlg->OnCommand(strController);
	}
	else
	if(msg == ZGM_MOUSEMOVE)
	{
		int pressed = ((int*)params)[0], x = ((int*)params)[1], y = ((int*)params)[2];
		g_kMistClient.m_pkEquipmentDlg->OnMouseMove(pressed < 1 ? false : true, x, y);
	}
	else
	if(msg == ZGM_KEYPRESS)
	{
		int iKey = ((int*)params)[0];
		switch(iKey)
		{		
		case KEY_RETURN:
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
				g_kMistClient.ToogleChatWnd(true, true);
			break;
		
		case KEY_ESCAPE:
			if( strController	==	"SayTextbox" )
			{
				((ZGuiTextbox*)g_kMistClient.GetWnd(strController))->KillFocus();
				if(ZGuiWnd::m_pkFocusWnd)
					ZGuiWnd::m_pkFocusWnd->KillFocus();
				g_kMistClient.SetGuiCapture(false);				
				g_kMistClient.m_pkGui->m_bHandledMouse = false;
			}

			g_kMistClient.m_pkGui->m_bForceGUICapture = false;
			break;	
		}
	}
}

EquipmentDlg::EquipmentDlg() 
{
	m_pkEquipmentWnd = NULL;
	m_pkTexMan = g_kMistClient.m_pkTexMan;
}

EquipmentDlg::~EquipmentDlg()
{
	if(m_pkEquipmentWnd)
		g_kMistClient.m_pkGui->UnregisterWindow(m_pkEquipmentWnd);
}

void EquipmentDlg::Open()
{	
	// load inventory
	if(m_pkEquipmentWnd == NULL)
	{
		if(!g_kMistClient.LoadGuiFromScript("data/script/gui/ml_equip.lua"))
		{
			printf("Error loading inventory script!\n");
			return;
		}

		m_pkEquipmentWnd = g_kMistClient.GetWnd("EquipmentDlg");		
	}

	// visa inventory fönstret
	m_pkEquipmentWnd->Show();
	g_kMistClient.m_pkGui->SetFocus(m_pkEquipmentWnd, false);	

	// dölj actionikonen och regruppera dom andra
	g_kMistClient.GetWnd("OpenEquipWndBn")->Hide();
	g_kMistClient.PositionActionButtons();
}

void EquipmentDlg::Close()
{
	// Hide inventory window
	if(m_pkEquipmentWnd)
		m_pkEquipmentWnd->Hide();

	// Must set focus on mainwnd to recive SPACE intput for chatbox...
	g_kMistClient.m_pkGui->SetFocus(g_kMistClient.GetWnd("GuiMainWnd"), false);	

	// Show the button that opens the inventory again.
	g_kMistClient.GetWnd("OpenEquipWndBn")->Show();

	// Reposition action buttons.
	g_kMistClient.PositionActionButtons();
}

void EquipmentDlg::OnCommand(string strController)
{
	if(strController == "EquipmentCloseBn")
		Close();
}

void EquipmentDlg::OnMouseMove(bool bLeftButtonPressed, int mx, int my)
{
	
	
}