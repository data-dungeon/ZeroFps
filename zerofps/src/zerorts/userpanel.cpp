// userpanel.cpp: implementation of the UserPanel class.
//
//////////////////////////////////////////////////////////////////////

#include "userpanel.h"
#include "zerorts.h"
#include "resource_id.h"
#include "minimap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UserPanel::UserPanel(ZeroRTS* pkZeroRts, ZGuiWndProc oMainWndProc) 
	: DlgBox( pkZeroRts->pkGui, pkZeroRts->pkInput, oMainWndProc)
{
	m_pkZeroRts = pkZeroRts;
	m_pkGuiBuilder = pkZeroRts->m_pkGuiBuilder;
	m_pkLastCmd = NULL;
	m_fOriginalHealthbarSize = 123.0f;
}

UserPanel::~UserPanel()
{

}

bool UserPanel::Create(int x, int y, char* szResourceFile, char* szDlgName)
{
	m_pkDlgBox = new ZGuiWnd(Rect(0,m_pkZeroRts->m_iHeight-192,
		m_pkZeroRts->m_iWidth,m_pkZeroRts->m_iHeight), NULL,false,15424);

	ZGuiSkin* pkMainWndSkin = new ZGuiSkin(0,0,0, 0,0,0, 0, 0);
	pkMainWndSkin->m_iBkTexID = 
		m_pkZeroRts->pkTexMan->Load("../data/textures/player_panel_rts.bmp", 0);
	pkMainWndSkin->m_iBkTexAlphaID = 
		m_pkZeroRts->pkTexMan->Load("../data/textures/player_panel_rts_a.bmp", 0);
	m_pkDlgBox->SetSkin(pkMainWndSkin);

	ZGuiWnd* pkMinimap = new ZGuiWnd(Rect(10,9,10+172,10+172),m_pkDlgBox,true,751489);
	pkMinimap->SetSkin(m_pkGuiBuilder->GetSkin("minimap") );
	pkMinimap->SetZValue(121212);
	m_pkGui->RegisterWindow(pkMinimap, "MiniMapWnd");

	Rect rc(m_pkZeroRts->m_iWidth-200,10,0,0);
	rc.Right = rc.Left + CMD_BN_SIZE; rc.Bottom = rc.Top + CMD_BN_SIZE;

	for(int i=0; i<MAX_NUM_CMD_BNS; i++)
	{
		if(i%3==0 && i!=0)
		{
			rc = Rect(m_pkZeroRts->m_iWidth-200,10,0,0);
			rc.Right = rc.Left + CMD_BN_SIZE; rc.Bottom = rc.Top + CMD_BN_SIZE;
			rc = rc.Move(0,(i/3)*(CMD_BN_SIZE+2));
		}

		ZGuiButton* pkButton = new ZGuiButton(rc,m_pkDlgBox,true,ID_CMD_BUTTONS_START+i);
		pkButton->SetMoveArea(pkButton->GetScreenRect());
		pkButton->SetButtonUpSkin(new ZGuiSkin());
		pkButton->SetButtonDownSkin(new ZGuiSkin());
		pkButton->SetButtonHighLightSkin(new ZGuiSkin());
		pkButton->Hide();

		m_akCommandBns.push_back(pkButton);
		rc = rc.Move(CMD_BN_SIZE+2,0);
	}

	// Button tooltip
	ZGuiLabel* pkLabel = m_pkGuiBuilder->CreateLabel(m_pkDlgBox, 0, 
		m_pkZeroRts->m_iWidth-310, 160, 300, 20, "", "CmdButtonTipLB");
	pkLabel->SetZValue(m_akCommandBns.back()->m_iZValue+1);

	// Unit health progress bar
	pkLabel = m_pkGuiBuilder->CreateLabel(m_pkDlgBox, 0, 
		195, 153, 315-195, 178-153, "", "UnitHealthLB");
	pkLabel->SetSkin(new ZGuiSkin(0,0,255, 0,0,0, 0));

	m_pkGui->AddMainWindow(15424+1, m_pkDlgBox, "UserPanelDlg", 
		m_oMainWndProc, false);

	m_pkDlgBox->SortChilds();

	return true;
}

bool UserPanel::DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
						 int iNumberOfParams,void *pkParams )
{
	int x, y;
	//unsigned int i;
	ZGuiWnd* pkClickWnd = NULL;

	switch(uiMessage)
	{
	case ZGM_LBUTTONDOWN:
	case ZGM_RBUTTONDOWN:

		x = ((int*)pkParams)[0];
		y = ((int*)pkParams)[1];

/*		if(pkWnd == m_pkGuiBuilder->Get("MiniMapWnd"))
		{
			OnClickMinimap(x,y);
		}

		pkClickWnd = m_pkDlgBox->Find(x,y);

		m_pkGuiBuilder->Get("CmdButtonTipLB")->SetText("");

		for(i=0; i<m_akCommandBns.size(); i++)
		{
			if(m_akCommandBns[i] == pkClickWnd)
			{
				map<ZGuiButton*,string>::iterator r;
				r = m_akCmdButtonText.find((ZGuiButton*)pkClickWnd);
				if(r != m_akCmdButtonText.end())
					m_pkGuiBuilder->Get("CmdButtonTipLB")->SetText(
						(char*)r->second.c_str());
				break;
			}
		}*/

		if(pkWnd == m_pkGuiBuilder->Get("MinMapWnd"))
		{
			OnClickMinimap(x,y);
		}
		break;

	case ZGM_MOUSEMOVE:

		x = ((int*)pkParams)[1];
		y = ((int*)pkParams)[2];

		pkClickWnd = m_pkDlgBox->Find(x,y);

		if(((int*)pkParams)[0] == 1) // button down
		{
			if(m_pkGuiBuilder->Get("MiniMapWnd") == pkWnd)
				OnClickMinimap(x,y);
		}

		break;

	case ZGM_COMMAND:
		int iCtrlID; iCtrlID = ((int*)pkParams)[0];
		OnClickCmdButton(iCtrlID);
		break;

	}
	return true;
}

bool UserPanel::OnOpen(int x, int y)
{
	return true;
}

bool UserPanel::OnClose(bool bSave)
{
	return true;
}


bool UserPanel::Click()
{
	int x,y;
	m_pkInput->MouseXY(x,y);

	if(m_pkDlgBox->GetScreenRect().Inside(x,y))
	{
		if( m_pkInput->Pressed(MOUSELEFT) || 
			m_pkInput->Pressed(MOUSERIGHT) )
		{
			return true;
		}
	}

	return false;
}

void UserPanel::OnClickMinimap(int x, int y)
{
	ZGuiWnd* pkWnd = m_pkGuiBuilder->Get("MiniMapWnd");

	Vector3 pos(0,CAMERA_HEIGHT,0);

	Rect rc = pkWnd->GetScreenRect();
	x -= rc.Left;
	y -= rc.Top;
	
	float fMapWidth = rc.Width();
	float fMapHeight = rc.Height();

	float fProcentAvWidth = (1.0f / fMapWidth) * x;
	float fProcentAvHeight = (1.0f / fMapWidth) * y;

	pos.x = -255 + fProcentAvWidth*512;
	pos.z = -255 + fProcentAvHeight*512;

	m_pkZeroRts->SetCamPos(pos);
}

void UserPanel::UpdateCmdButtons()
{
	ObjectManager* pkObjMan = m_pkZeroRts->pkObjectMan;
	Object* pkObject;

	set<int> kSetMap;

	HideAllCmdButtons();

	m_kClickInfoMap.clear();

	for(list<int>::iterator it = m_pkZeroRts->m_kSelectedObjects.begin();
		it != m_pkZeroRts->m_kSelectedObjects.end();it++)		
	{
		pkObject = pkObjMan->GetObjectByNetWorkID((*it));
		P_ClientUnit* pkClientUnit = m_pkZeroRts->GetClientUnit((*it));

		if(pkClientUnit)
		{
			int iIndex = 0;
			for(unsigned int i=0; i<pkClientUnit->m_kUnitCommands.size(); i++)
			{
				int iIconIndex = pkClientUnit->m_kUnitCommands[i].m_iIconIndex;
				set<int>::iterator r = kSetMap.find(iIconIndex);
				if(r == kSetMap.end())
				{
					ZGuiButton* pkButton = SetCmdButtonIcon(iIndex, iIconIndex, true);
					
					m_kClickInfoMap.insert(map<ZGuiButton*, UnitCommandInfo*>::value_type(
						pkButton, &pkClientUnit->m_kUnitCommands[i]));
					
					kSetMap.insert(set<int>::value_type(iIconIndex));
					iIndex++;
				}
			}
		}
	}
}

ZGuiButton* UserPanel::SetCmdButtonIcon(int iButtonIndex, int iIconIndex, bool bShow)
{
	if(iButtonIndex < 0 || iButtonIndex > MAX_NUM_CMD_BNS)
	{
		printf("SetCmdButtonIcon = ERROR\n");
		return NULL;
	}

	ZGuiButton* pkButton = (ZGuiButton*) m_pkGuiBuilder->GetChild(m_pkDlgBox, 
		ID_CMD_BUTTONS_START+iButtonIndex);

	if(pkButton == NULL)
	{
		printf("SetCmdButtonIcon = ERROR\n");
		return NULL;
	}

	int iNumVisibleButtons = GetNumVisibleCmdButtons();

	int x = m_pkZeroRts->m_iWidth-200, y = 10;
	x += (CMD_BN_SIZE+2) * (iNumVisibleButtons % 3);
	y += (CMD_BN_SIZE+2) * (iNumVisibleButtons / 3);

	Rect rc(x,y,x+CMD_BN_SIZE,y+CMD_BN_SIZE);
	pkButton->SetPos(x,y,false,true);
	pkButton->SetMoveArea(pkButton->GetScreenRect());

	if(bShow)
		pkButton->Show();
	else
		pkButton->Hide();

	char* postfix[] = { "_bnu.bmp", "_bnd.bmp", "_bnf.bmp" };

	ZGuiSkin* pkSkin;

	for(int i=0; i<3; i++)
	{
		switch(i)
		{
		case 0: pkSkin = pkButton->GetButtonUpSkin(); break;
		case 1: pkSkin = pkButton->GetButtonDownSkin(); break;
		case 2: pkSkin = pkButton->GetButtonHighLightSkin(); break;
		}

		string szFileName = "file:../data/textures/cmdbuttons/";
		szFileName += m_kCmdIconNameMap.find(iIconIndex)->second.c_str();
		szFileName += postfix[i];

		pkSkin->m_iBkTexID = m_pkZeroRts->pkTexMan->Load(szFileName.c_str(), 0);
		
		switch(i)
		{
		case 0: pkButton->SetButtonUpSkin(pkSkin); break;
		case 1: pkButton->SetButtonDownSkin(pkSkin); break;
		case 2: pkButton->SetButtonHighLightSkin(pkSkin); break;
		}
	}

	return pkButton;
}

void UserPanel::OnClickCmdButton(int iCtrlID)
{
	if(!(iCtrlID >= ID_CMD_BUTTONS_START && iCtrlID < 
		ID_CMD_BUTTONS_START+MAX_NUM_CMD_BNS))
		return;

	ZGuiButton* pkButton = (ZGuiButton*) 
		m_pkGuiBuilder->GetChild(m_pkDlgBox, iCtrlID);

	if(pkButton == NULL)
		return;

	map<ZGuiButton*,UnitCommandInfo*>::iterator r;
	r = m_kClickInfoMap.find(pkButton);
	if(r != m_kClickInfoMap.end())	
	{
		

		if(r->second->m_bNeedArgument == false)
		{
			UnitCommand cmd;
			for(list<int>::iterator it = m_pkZeroRts->m_kSelectedObjects.begin(); 
				it != m_pkZeroRts->m_kSelectedObjects.end(); it++)
			{
				//strcpy(cmd.m_acCommandName, m_pkLastCmd->m_acCommandName);
				strcpy(cmd.m_acCommandName, r->second->m_acCommandName);
				cmd.m_iXDestinaton = 0;
				cmd.m_iYDestinaton = 0;	
				cmd.m_iTarget = 0;
				cmd.m_iUnitID = (*it);	
				m_pkZeroRts->m_pkClientInput->AddOrder(cmd);
				
			}
			//m_pkLastCmd = NULL;
		}
		else 
			m_pkLastCmd = r->second;
	}
	
}

int UserPanel::GetNumVisibleCmdButtons()
{
	int iCounter = 0;

	ZGuiButton* pkButton;
	for(int i=0; i<MAX_NUM_CMD_BNS; i++)
	{
		pkButton = (ZGuiButton*) m_pkGuiBuilder->GetChild(m_pkDlgBox, 
			ID_CMD_BUTTONS_START+i);

		if(pkButton->IsVisible())
			iCounter++;
	}

	return iCounter;
}

void UserPanel::HideAllCmdButtons()
{
	ZGuiButton* pkButton;
	for(int i=0; i<MAX_NUM_CMD_BNS; i++)
	{
		pkButton = (ZGuiButton*) m_pkGuiBuilder->GetChild(m_pkDlgBox, 
			ID_CMD_BUTTONS_START+i);
		pkButton->Hide();
	}
}

bool UserPanel::PopLastButtonCommand(char* szCommand)
{
	if(m_pkLastCmd != NULL)
	{
		strcpy(szCommand, m_pkLastCmd->m_acCommandName);
		m_pkLastCmd = NULL;
		return true;
	}

	return false;
}

void UserPanel::OnSelectObjects(Object* pkObjectInFocus)
{
	UpdateCmdButtons();

	P_ClientUnit* pkClientUnit = (P_ClientUnit*) pkObjectInFocus->GetProperty("P_ClientUnit");

	if(pkClientUnit)
	{
		int w, h;
		Rect rc = m_pkGuiBuilder->Get("UnitHealthLB")->GetScreenRect();
		float fHelth = (float) ((int) pkClientUnit->m_kInfo.m_Info2.m_cHealth) / 255.0f;

		h = rc.Height();
		w = fHelth * m_fOriginalHealthbarSize;

		m_pkGuiBuilder->Get("UnitHealthLB")->Resize(w,h);
		printf("%s\n", (int) pkClientUnit->m_kInfo.m_cName);
	}
}

/* COMMENT OUT BY ZEB
void ZeroRTS::SetObjDstPos(int sqr_x, int sqr_y, Object* pkObject)
{	
	if(pkObject == NULL)
		return;

	Vector3 newp = GetPosFromSqr(Point(sqr_x, sqr_y));

	pkObject->SetPos(newp);
	pkObject->SetPos(newp);
}

void ZeroRTS::BuildPath()
{
	static bool bDone = false;
	if(bDone == false)
		bDone = true;
	else
		return;

	int aiCost[5];
	aiCost[0] = 15; // gräs (grön nyans)
	aiCost[1] = 1; // väg (röd nyans)
	aiCost[2] = 7; // sten (blå nyans)
	aiCost[3] = 10; // öken (röd nyans)
	aiCost[4] = 999; // vatten

	PathBuilder kPathBuilder(m_pkMap, &m_pkTestPath);
	kPathBuilder.Build(aiCost);
}

bool ZeroRTS::MovePath(Object* pkObject)
{
	static float prev_time = 0;

	float time = pkFps->GetGameTime();

	if(time - prev_time > 0.125f)
	{
		int x=-1, y=-1;
		if(!m_pkTestPath->GetNextStep(x,y))
		{
			return true; // do nothing
		}
		
		if(!(x==-1&&y==-1))
			SetObjDstPos(x, y, pkObject);

		prev_time = time;
	}

	return true;
}
*/

void UserPanel::UpdateGraphic()
{	
	if(!m_pkZeroRts->m_kSelectedObjects.empty())
	{
		P_ClientUnit* pkClientUnit = m_pkZeroRts->GetClientUnit(
			m_pkZeroRts->m_kSelectedObjects.front());

		if(pkClientUnit)
		{
			int h=178-153;
			int w = (float) pkClientUnit->m_kInfo.m_Info2.m_cHealth / 255.0f * m_fOriginalHealthbarSize;

			if(w >= 0)
				m_pkGuiBuilder->Get("UnitHealthLB")->Resize(w,h);
		}
	}
}
