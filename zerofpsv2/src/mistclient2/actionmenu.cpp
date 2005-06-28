#include "actionmenu.h"

#include "mistclient.h"
#include "gui_optionsdlg.h"
#include "../zerofpsv2/engine/entitymanager.h"
#include "../zerofpsv2/engine/inputhandle.h"
#include "../mcommon/p_ml.h"

extern MistClient	g_kMistClient;

void GuiMsgActionDlg( string strMainWnd, string strController, unsigned int msg, int numparms, void *params )
{
	if(msg == ZGM_COMMAND)
	{
		if(strMainWnd == "ActionMenuMain")
		{
			g_kMistClient.m_pkActionDlg->OnCommand(strController);
		}
	}

	if(msg == ZGM_MOUSEMOVE)
	{
		int pressed = ((int*)params)[0];
		int x = ((int*)params)[1];
		int y = ((int*)params)[2];
		g_kMistClient.m_pkActionDlg->OnMouseMove(pressed < 1 ? false : true, x, y);
	}
}

ActionMenu::ActionMenu() : ICON_WIDTH(64), ICON_HEIGHT(64)
{
	m_iEntityID = -1;

	m_kWidth = 500;
	m_kHeight = 500;

	m_pkTexMan = g_kMistClient.m_pkTexMan;
	m_pkMainWnd = g_kMistClient.CreateWnd(Wnd, "ActionMenuMain", "", "", 0, 0, m_kWidth, m_kHeight, 0);
	m_pkMainWnd->Hide();
	m_pkMainWnd->SetSkin(new ZGuiSkin());
	m_pkMainWnd->GetSkin()->m_bTransparent = true; 
	
	for(int i=0; i<75; i++)
	{
		m_vkIconSkins.push_back( pair<ZGuiSkin*,bool>(new ZGuiSkin(), true) );
	}

	m_pkIconSelection = (ZGuiLabel*) g_kMistClient.CreateWnd(Label, 
		"ActionMenuSelLabel", "ActionMenuMain", "", 0, 0, ICON_WIDTH, ICON_HEIGHT, 0);
	m_pkIconSelection->Hide();


	m_pkIconSelection->SetSkin(new ZGuiSkin());
	m_pkIconSelection->GetSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes(
		"actions/action_selection_border.tga") ;	
}

ActionMenu::~ActionMenu()
{
	g_kMistClient.m_pkGui->UnregisterWindow(m_pkMainWnd);
}

Vector2 ActionMenu::RotateXY(float angle, Vector2 size)
{ 
	float xold,yold;
	Vector2 newpoint(0,0);

	xold = size.x;
	yold = size.y;
	newpoint.x = xold*cosf(angle)-yold*sinf(angle);
	newpoint.y = xold*sinf(angle)+yold*cosf(angle);
	return newpoint;
}

bool ActionMenu::IsOpen() 
{ 
	if(m_pkMainWnd)
		if(m_pkMainWnd->IsVisible())
			return true;

	return false;
}

void ActionMenu::Open()
{
	if(m_pkMainWnd->IsVisible())
	{
		ZGuiWnd::m_pkFocusWnd = m_pkMainWnd;
		return;
	}

	if(m_iEntityID == -1)
		return;

	Entity* pkEnt = g_kMistClient.m_pkEntityManager->GetEntityByID(m_iEntityID);
	if(pkEnt == NULL)
		return;

	
	//get actions from p_ml
	if(P_Ml* pkMl = (P_Ml*)pkEnt->GetProperty("P_Ml"))
		pkMl->GetActions(m_kActions);

	//if theres an p_item also add pickup action		
	if(pkEnt->GetProperty("P_Item"))
		m_kActions.push_back("Pickup");

	if(m_kActions.empty())
		return;

	m_bGuiCaptureBeforOpen = g_kMistClient.m_bGuiCapture; // rembember privius gui capture mode

	printf("Open action menu\n");

	ResetIconSkins();

	float mx = g_kMistClient.GetWidth()/2, my = g_kMistClient.GetHeight()/2;

	if(m_bGuiCaptureBeforOpen == true)
		g_kMistClient.m_pkInputHandle->MouseXY(mx,my);

	int mainwnd_pos_x = int(mx-m_kWidth/2), mainwnd_pos_y = int(my-m_kHeight/2);

	const float icon_angle_horz_offset = 75;
	const float icon_angle_vert_offset = 75;

	// Check so mainwindow are visible on screen...
	if(mainwnd_pos_y < -icon_angle_vert_offset) mainwnd_pos_y = (int)-icon_angle_vert_offset;
	if(mainwnd_pos_x < -icon_angle_horz_offset) mainwnd_pos_x = (int)-icon_angle_horz_offset;
	if(mainwnd_pos_y + m_kHeight > g_kMistClient.GetHeight()+icon_angle_vert_offset) 
		mainwnd_pos_y = int(g_kMistClient.GetHeight()-m_kHeight+icon_angle_vert_offset);
	if(mainwnd_pos_x + m_kWidth > g_kMistClient.GetWidth()+icon_angle_horz_offset ) 
		mainwnd_pos_x = int(g_kMistClient.GetWidth()-m_kWidth+icon_angle_horz_offset);

	m_pkMainWnd->SetPos(mainwnd_pos_x, mainwnd_pos_y, true, true);
	m_pkMainWnd->Show();

	char szIcon[512];
	float fAngle = 225;
	const int NUM_BUTTONS = m_kActions.size();
	const float ANGLE_MULT = 360.0f/(float)NUM_BUTTONS;

	int cx = m_kWidth / 2 - ICON_WIDTH / 2;
	int cy = m_kHeight / 2 - ICON_HEIGHT / 2;

	int iHeighestZValue = 0;

	g_kMistClient.SetGuiCapture(true); // change to gui capture (mouse cursor)

	if(m_bGuiCaptureBeforOpen == true)
	{
		g_kMistClient.m_pkInputHandle->SetCursorInputPos(mx-m_kWidth/2 + cx + 32, my-m_kHeight/2 + cy + 32);
	}

	for(int i=0; i<NUM_BUTTONS; i++)
	{
		float x = cx, y = cy; // centrum position

		float fMultipel = (PI+PI) / 360.0f;
		float rot = fMultipel * fAngle; 

		Vector2 p = RotateXY(rot, Vector2(icon_angle_horz_offset,icon_angle_vert_offset));

		fAngle += ANGLE_MULT;
		x += p.x;
		y += p.y;

		if(m_kActions.size() == 1)
		{
			x = cx;
			y = cy;
		}

		char szName[50];
		sprintf(szName, "ActionMenuButton_%i", i);

		if(i==0)
		{				
			m_pkIconSelection->SetPos((int)x, (int)y, false, true); 
 			//m_pkIconSelection->Show();
 			m_pkIconSelection->Hide();	//visa inte mark?en i b?jan
		}

		ZGuiButton* pkButton;
		pkButton = (ZGuiButton*) g_kMistClient.CreateWnd(Button, 
			szName, "", m_pkMainWnd, 0, 0, ICON_WIDTH, ICON_HEIGHT, 0);
		
		ZGuiSkin* pkUpSkin = GetFreeIconSkin();
		ZGuiSkin* pkDownFocusSkin = GetFreeIconSkin();
		if(pkDownFocusSkin)
		{
			pkButton->SetButtonDownSkin(pkDownFocusSkin);
			pkButton->SetButtonHighLightSkin(pkDownFocusSkin);
		}
		if(pkUpSkin)
			pkButton->SetButtonUpSkin(pkUpSkin);

		
		string strTemp = m_kActions[i];
		for(int u=0; u<strTemp.size(); u++)
		{
			char ch = strTemp[u];
			if(ch >= 65 && ch <= 90)
				strTemp[u] = (char) (ch + 32);
		}

		sprintf(szIcon, "actions/%s.tga", strTemp.c_str());

		int iTexID = g_kMistClient.LoadGuiTextureByRes(szIcon);
		
		pkButton->GetButtonUpSkin()->m_iBkTexID = iTexID ;
		pkButton->GetButtonDownSkin()->m_iBkTexID = iTexID ;
		pkButton->GetButtonHighLightSkin()->m_iBkTexID = iTexID ;
		pkButton->GetSkin()->m_iBkTexID = iTexID ;

// 		pkButton->GetButtonUpSkin()->m_afBkColor[0] = 0.5f;
// 		pkButton->GetButtonUpSkin()->m_afBkColor[1] = 0.5f;
// 		pkButton->GetButtonUpSkin()->m_afBkColor[2] = 0.5f;
// 		pkButton->GetSkin()->m_afBkColor[0] = 0.5f;
// 		pkButton->GetSkin()->m_afBkColor[1] = 0.5f;
// 		pkButton->GetSkin()->m_afBkColor[2] = 0.5f;
		pkButton->GetButtonUpSkin()->m_afBkColor[0] = 1.0f;
		pkButton->GetButtonUpSkin()->m_afBkColor[1] = 1.0f;
		pkButton->GetButtonUpSkin()->m_afBkColor[2] = 1.0f;
		pkButton->GetSkin()->m_afBkColor[0] = 1.0f;
		pkButton->GetSkin()->m_afBkColor[1] = 1.0f;
		pkButton->GetSkin()->m_afBkColor[2] = 1.0f;		
		
		
		pkButton->SetPos((int)x, (int)y, false, true); 
		pkButton->Show();

		if(pkButton->m_iZValue > iHeighestZValue)
			iHeighestZValue = pkButton->m_iZValue;
	}

	m_pkIconSelection->SetZValue(iHeighestZValue+1);

	g_kMistClient.m_pkGui->SetFocus(m_pkMainWnd, true);
	g_kMistClient.m_pkGui->PlaceWndFrontBack(m_pkMainWnd, true);
	
}

void ActionMenu::Close()
{
	list<ZGuiWnd*> kChilds;
	m_pkMainWnd->GetChildrens(kChilds);
	for(list<ZGuiWnd*>::iterator it = kChilds.begin(); it!=kChilds.end(); it++) 
		(*it)->Hide();

	ResetIconSkins();
	m_iEntityID = -1;
	m_pkMainWnd->Hide();

	if(m_bGuiCaptureBeforOpen == false)
		g_kMistClient.SetGuiCapture(m_bGuiCaptureBeforOpen);

	printf("Close action menu\n");

	// Must set focus on mainwnd to recive SPACE intput for chatbox...
	g_kMistClient.m_pkGui->SetFocus(g_kMistClient.GetWnd("GuiMainWnd"), false);	

}

ZGuiSkin* ActionMenu::GetFreeIconSkin()
{
	for(int i=0; i<m_vkIconSkins.size(); i++)
	{
		if(m_vkIconSkins[i].second)
		{
			m_vkIconSkins[i].second = false;
			return m_vkIconSkins[i].first;
		}
	}

	return NULL;
}

void ActionMenu::ResetIconSkins()
{
	for(int i=0; i<m_vkIconSkins.size(); i++)
	{
		m_vkIconSkins[i].second = true;
	}
}

void ActionMenu::OnCommand(string strController)
{

}

void ActionMenu::OnMouseMove(bool bLeftButtonPressed, int mx, int my)
{
	
	ZGuiWnd* pkWndButtonCursor = NULL;

	// Kolla vilken knapp som ? vald.
	list<ZGuiWnd*> kChilds;
	m_pkMainWnd->GetChildrens(kChilds);
	m_pkIconSelection->Hide();				//dvoid hax, dölj markören om ingen ikon ? markerad
	for(list<ZGuiWnd*>::iterator it = kChilds.begin(); it!=kChilds.end(); it++) 
	{
		ZGuiWnd* pkChild = (*it);

		if(pkChild != m_pkIconSelection && pkChild->IsVisible() )
		{
			Rect rc = pkChild->GetScreenRect();
			if(rc.Inside(mx,my))
			{
				pkWndButtonCursor = pkChild;
				m_pkIconSelection->SetPos(rc.Left,rc.Top,true,true);
				m_pkIconSelection->Show();
				break;
			}
		}
	}


	
	// Om h?er musknapp har sl?ps skall vi s?da valt action
// 	if(g_kMistClient.m_pkGui->m_bMouseRightPressed == false) 
	if(g_kMistClient.m_pkGui->m_bMouseLeftPressed == false) 
	{
		if(pkWndButtonCursor != NULL)
		{
			if(m_iEntityID != -1 && !m_kActions.empty())
			{
				string strController = pkWndButtonCursor->GetName();
				int p = strController.find("_");
				if(p != string::npos)
				{
					strController.erase(0,p+1);
					int iAction = atoi(strController.c_str());
					
					if(m_kActions[iAction] == "Pickup")
						g_kMistClient.RequestPickup(m_iEntityID, -1, -1);
					else
						g_kMistClient.SendAction(m_iEntityID, m_kActions[iAction]);
				}
			}
		}
		Close();
	}


}

void ActionMenu::SetEntity(int iEntityID)
{
	m_iEntityID = iEntityID;
}







