
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

EquipmentDlg::EquipmentDlg() :
		m_rcHelm(		  97,  11,  97 + 64,  11 +  64 ),
		m_rcGloves(		   7,  47,   7 + 64,  47 +  64 ),
		m_rcAmulett(	 113,  84, 113 + 32,  84 +  32 ),
		m_rcCloak(		 185,  50, 185 + 64,  50 +  64 ),
		m_rcWeaponHand(   7, 124,   7 + 64, 124 + 128 ),
		m_rcBody(		  80, 124,  80 + 96, 124 + 128 ),
		m_rcShieldHand( 185, 125, 185 + 64, 125 + 128 ),
		m_rcBracer1(	   7, 265,   7 + 32, 265 +  64 ),
		m_rcBracer2(	  40, 265,  40 + 32, 265 +  64 ),
		m_rcBelt(		  94, 265,  94 + 64, 265 +  32 ),
		m_rcRing1(		 184, 267, 184 + 32, 267 +  32 ),
		m_rcRing2(		 218, 267, 218 + 32, 267 +  32 ),
		m_rcRing3(		 184, 301, 184 + 32, 301 +  32 ),
		m_rcRing4(		 217, 301, 217 + 32, 301 +  32 ),
		m_rcBoots(		  94, 309,  94 + 64, 309 +  64 )
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

void EquipmentDlg::OnDrop(int mx, int my)
{

}

Rect EquipmentDlg::GetRect(EquipmentSlot eSlot, bool bScreenPos)
{
	if(m_pkEquipmentWnd == NULL)
		return Rect(0,0,0,0);

	Rect rc(0,0,0,0);

	if(bScreenPos)
		rc += m_pkEquipmentWnd->GetScreenRect();

	switch(eSlot)
	{
	case EqS_Helm:			rc += m_rcHelm;
	case EqS_Gloves:		rc += m_rcGloves;
	case EqS_Amulett:		rc += m_rcAmulett;
	case EqS_Cloak:		rc += m_rcCloak;
	case EqS_WeaponHand:	rc += m_rcWeaponHand;
	case EqS_Body:			rc += m_rcBody;
	case EqS_ShieldHand:	rc += m_rcShieldHand;
	case EqS_Bracer1:		rc += m_rcBracer1;
	case EqS_Bracer2:		rc += m_rcBracer2;
	case EqS_Belt:			rc += m_rcBelt;
	case EqS_Ring1:		rc += m_rcRing1;
	case EqS_Ring2:		rc += m_rcRing2;
	case EqS_Ring3:		rc += m_rcRing3;
	case EqS_Ring4:		rc += m_rcRing4;
	case EqS_Boots:		rc += m_rcBoots;
		default: return Rect(0,0,0,0);
	}

	return rc;
}

EquipmentSlot EquipmentDlg::GetSlot(int mx, int my, bool bScreenPos)
{
	if(m_pkEquipmentWnd == NULL)
		return EqS_None;

	Rect rc(0,0,0,0);
	
	if(bScreenPos)
	{
		mx -= m_pkEquipmentWnd->GetScreenRect().Left;
		my -= m_pkEquipmentWnd->GetScreenRect().Top;
	}
	
	if(m_rcHelm.Inside(mx,my)) return EqS_Helm;
	if(m_rcGloves.Inside(mx,my)) return EqS_Gloves;
	if(m_rcAmulett.Inside(mx,my)) return EqS_Amulett;
	if(m_rcCloak.Inside(mx,my)) return EqS_Cloak;
	if(m_rcWeaponHand.Inside(mx,my)) return EqS_WeaponHand;
	if(m_rcBody.Inside(mx,my)) return EqS_Body;
	if(m_rcShieldHand.Inside(mx,my)) return EqS_ShieldHand;
	if(m_rcBracer1.Inside(mx,my)) return EqS_Bracer1;
	if(m_rcBracer2.Inside(mx,my)) return EqS_Bracer2;
	if(m_rcBelt.Inside(mx,my)) return EqS_Belt;
	if(m_rcRing1.Inside(mx,my)) return EqS_Ring1;
	if(m_rcRing2.Inside(mx,my)) return EqS_Ring2;
	if(m_rcRing3.Inside(mx,my)) return EqS_Ring3;
	if(m_rcRing4.Inside(mx,my)) return EqS_Ring4;
	if(m_rcBoots.Inside(mx,my)) return EqS_Boots;

	return EqS_None;	
}

void EquipmentDlg::Update(vector<MLContainerInfo>& vkItemList)
{
	char szItemName[128];
	char text[64] = "";

	// Remove all slots.
	for(int i=0; i<m_vkEquipmentItemList.size(); i++)
	{
		ZGuiWnd* pkWnd = m_vkEquipmentItemList[i].pkWnd;
		
		if(pkWnd->GetSkin())
			delete pkWnd->GetSkin();
		
		g_kMistClient.m_pkGui->GetToolTip()->RemoveToolTip(pkWnd);
		g_kMistClient.m_pkGui->UnregisterWindow( pkWnd );
	}

	m_vkEquipmentItemList.clear();

	Rect rc = GetRect(EqS_Helm, false);

	sprintf(szItemName, "EquipmentItemLabel%i", 0);
	ZGuiWnd* pkLabel = g_kMistClient.CreateWnd(Label, szItemName, text, 
		m_pkEquipmentWnd, rc.Left, rc.Top, rc.Width(), rc.Height(), 0);
	pkLabel->Show();

	EQUIPMENT_SLOT kNewSlot;
	kNewSlot.eSlotType = EqS_Helm;
	kNewSlot.pkWnd = pkLabel;
	m_vkEquipmentItemList.push_back(kNewSlot);

}