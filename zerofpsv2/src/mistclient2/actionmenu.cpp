#include "actionmenu.h"

#include "mistclient.h"
#include "gui_optionsdlg.h"

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
	m_pkIconSelection->GetSkin()->m_iBkTexID = m_pkTexMan->Load(
		"data/textures/gui/actions/action_selection_border.tga", 0) ;	
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

	if(P_Ml* pkMl = (P_Ml*)pkEnt->GetProperty("P_Ml"))
		pkMl->GetActions(m_kActions);

	if(m_kActions.size() < 2)
		return;

	m_bGuiCaptureBeforOpen = g_kMistClient.m_bGuiCapture; // rembember privius gui capture mode

	printf("Open action menu\n");

	ResetIconSkins();

	float mx = g_kMistClient.GetWidth()/2, my = g_kMistClient.GetHeight()/2;

	if(m_bGuiCaptureBeforOpen == true)
		g_kMistClient.m_pkInputHandle->MouseXY(mx,my);

	m_pkMainWnd->SetPos(mx-m_kWidth/2, my-m_kHeight/2, true, true);
	m_pkMainWnd->Show();

	char szIcon[512];
	float fAngle = 245;
	const int NUM_BUTTONS = m_kActions.size();
	const float ANGLE_MULT = 360.0f/(float)NUM_BUTTONS;

	int cx = m_kWidth / 2 - ICON_WIDTH / 2;
	int cy = m_kHeight / 2 - ICON_HEIGHT / 2;

	const float icon_angle_horz_offset = 100;
	const float icon_angle_vert_offset = 50;

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
			m_pkIconSelection->SetPos(x, y, false, true); 
			m_pkIconSelection->Show();
		}

		ZGuiButton* pkButton;
		pkButton = (ZGuiButton*) g_kMistClient.CreateWnd(Button, 
			szName, "ActionMenuMain", "", 0, 0, ICON_WIDTH, ICON_HEIGHT, 0);
		
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

		sprintf(szIcon, "data/textures/gui/actions/%s.tga", strTemp.c_str());

		int iTexID = m_pkTexMan->Load(szIcon, 0);
		
		pkButton->GetButtonUpSkin()->m_iBkTexID = iTexID ;
		pkButton->GetButtonDownSkin()->m_iBkTexID = iTexID ;
		pkButton->GetButtonHighLightSkin()->m_iBkTexID = iTexID ;
		pkButton->GetSkin()->m_iBkTexID = iTexID ;

		pkButton->GetButtonUpSkin()->m_afBkColor[0] = 0.5f;
		pkButton->GetButtonUpSkin()->m_afBkColor[1] = 0.5f;
		pkButton->GetButtonUpSkin()->m_afBkColor[2] = 0.5f;

		pkButton->GetSkin()->m_afBkColor[0] = 0.5f;
		pkButton->GetSkin()->m_afBkColor[1] = 0.5f;
		pkButton->GetSkin()->m_afBkColor[2] = 0.5f;
		
		pkButton->SetPos(x, y, false, true); 
		pkButton->Show();

		if(pkButton->m_iZValue > iHeighestZValue)
			iHeighestZValue = pkButton->m_iZValue;
	}

	m_pkIconSelection->SetZValue(iHeighestZValue+1);

	ZGuiWnd::m_pkFocusWnd = m_pkMainWnd;

	
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

	list<ZGuiWnd*> kChilds;
	m_pkMainWnd->GetChildrens(kChilds);
	for(list<ZGuiWnd*>::iterator it = kChilds.begin(); it!=kChilds.end(); it++) 
	{
		if((*it) != m_pkIconSelection)
		{
			Rect rc = (*it)->GetScreenRect();
			if(rc.Inside(mx,my))
			{
				pkWndButtonCursor = (*it);
				m_pkIconSelection->SetPos(rc.Left,rc.Top,true,true);
				break;
			}
		}
	}

	if(g_kMistClient.m_pkGui->m_bMouseRightPressed == false) 
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