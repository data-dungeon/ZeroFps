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
		g_kMistClient.m_pkActionDlg->OnMouseMove();
	}
}

ActionMenu::ActionMenu() : ICON_WIDTH(64), ICON_HEIGHT(64)
{
	m_kWidth = 500;
	m_kHeight = 500;

	m_pkTexMan = g_kMistClient.m_pkTexMan;
	m_pkMainWnd = g_kMistClient.CreateWnd(Wnd, "ActionMenuMain", "", "", 0, 0, m_kWidth, m_kHeight, 0);
	m_pkMainWnd->Hide();
	m_pkMainWnd->SetSkin(new ZGuiSkin());
	m_pkMainWnd->GetSkin()->m_bTransparent = true; 

	m_pkEntity = NULL;

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

	m_iDefTexID = m_pkTexMan->Load("data/textures/gui/actions/action_default.tga", 0) ;
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
	if(m_pkEntity == NULL)
		return;

	m_kActions.clear();

	if(P_Ml* pkMl = (P_Ml*)m_pkEntity->GetProperty("P_Ml"))
		pkMl->GetActions(m_kActions);

	if(m_kActions.empty())
		return;

	char szIcon[512];

	g_kMistClient.ToggleGuiCapture(true);

	ResetIconSkins();

	float mx=g_kMistClient.GetWidth()/2, my=g_kMistClient.GetHeight()/2;
	//g_kMistClient.m_pkInputHandle->MouseXY(mx, my);

	m_pkMainWnd->SetPos(mx-m_kWidth/2, my-m_kHeight/2, true, true);
	m_pkMainWnd->Show();

	float fAngle = 245;
	const int NUM_BUTTONS = m_kActions.size();
	const float ANGLE_MULT = 360.0f/(float)NUM_BUTTONS;

	int cx = m_kWidth / 2 - ICON_WIDTH / 2;
	int cy = m_kHeight / 2 - ICON_HEIGHT / 2;

	const float icon_angle_horz_offset = 100;
	const float icon_angle_vert_offset = 50;

	int iHeighestZValue = 0;

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

	m_pkEntity = NULL;
	m_pkMainWnd->Hide();
	g_kMistClient.ToggleGuiCapture();
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
	ZGuiWnd* pkClickWnd = g_kMistClient.GetWnd(strController);

	if(pkClickWnd)
	{
		m_pkIconSelection->SetZValue(pkClickWnd->m_iZValue+1);
		m_pkMainWnd->SortChilds(); 

		GuiType eType = g_kMistClient.GetWndType(pkClickWnd);

		if(eType == Button)
		{			
			if(m_pkEntity != NULL && !m_kActions.empty())
			{
				int p = strController.find("_");
				if(p != string::npos)
				{
					strController.erase(0,p+1);
					int iAction = atoi(strController.c_str());
					g_kMistClient.SendAction(m_pkEntity->GetEntityID(),m_kActions[iAction]);
				}
			}

			Close();
		}
	}
}

void ActionMenu::OnMouseMove()
{
	float mx, my;
	g_kMistClient.m_pkInputHandle->MouseXY(mx, my);

	list<ZGuiWnd*> kChilds;
	m_pkMainWnd->GetChildrens(kChilds);
	for(list<ZGuiWnd*>::iterator it = kChilds.begin(); it!=kChilds.end(); it++) 
	{
		Rect rc = (*it)->GetScreenRect();
		if(rc.Inside(mx,my))
		{
			m_pkIconSelection->SetPos(rc.Left,rc.Top,true,true);
		}
	}

	if(g_kMistClient.m_pkInputHandle->Pressed(MOUSELEFT)) 
	{
		Close();
	}
}


void ActionMenu::SetEntity(Entity* pkEnt)
{
	m_pkEntity = pkEnt;
}