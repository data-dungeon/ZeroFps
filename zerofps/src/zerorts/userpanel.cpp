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
	m_szLastCmd = NULL;
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

	Rect rc(m_pkZeroRts->m_iWidth-300-10,10,0,0);
	rc.Right = rc.Left + CMD_BN_SIZE; rc.Bottom = rc.Top + CMD_BN_SIZE;

	for(int i=0; i<MAX_NUM_CMD_BNS; i++)
	{
		if(i%6==0 && i!=0)
		{
			rc = Rect(m_pkZeroRts->m_iWidth-300-10,10,0,0);
			rc.Right = rc.Left + CMD_BN_SIZE; rc.Bottom = rc.Top + CMD_BN_SIZE;
			rc = rc.Move(0,(i/6)*(CMD_BN_SIZE+2));
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

	ZGuiLabel* pkLabel = m_pkGuiBuilder->CreateLabel(m_pkDlgBox, 123456, 
		m_pkZeroRts->m_iWidth-310, 160, 300, 20, "", "CmdButtonTipLB");
	pkLabel->SetZValue(m_akCommandBns.back()->m_iZValue+1);

	m_pkGui->AddMainWindow(15424+1, m_pkDlgBox, "UserPanelDlg", 
		m_oMainWndProc, false);

	m_pkDlgBox->SortChilds();


	return true;
}

bool UserPanel::DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
						 int iNumberOfParams,void *pkParams )
{
	int x, y;
	unsigned int i;
	ZGuiWnd* pkClickWnd = NULL;

	switch(uiMessage)
	{
	case ZGM_LBUTTONDOWN:
	case ZGM_RBUTTONDOWN:

		x = ((int*)pkParams)[0];
		y = ((int*)pkParams)[1];

		if(pkWnd == m_pkGuiBuilder->Get("MiniMapWnd"))
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
		}

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
					SetCmdButtonIcon(iIndex, iIconIndex, true);
					kSetMap.insert(set<int>::value_type(iIconIndex));
					iIndex++;
				}
			}
		}
	}
}

void UserPanel::SetCmdButtonIcon(int iButtonIndex, int iIconIndex, bool bShow)
{
	if(iButtonIndex < 0 || iButtonIndex > MAX_NUM_CMD_BNS)
	{
		printf("SetCmdButtonIcon = ERROR\n");
		return;
	}

	ZGuiButton* pkButton = (ZGuiButton*) m_pkGuiBuilder->GetChild(m_pkDlgBox, 
		ID_CMD_BUTTONS_START+iButtonIndex);

	if(pkButton == NULL)
	{
		printf("SetCmdButtonIcon = ERROR\n");
		return;
	}

	int iNumVisibleButtons = GetNumVisibleCmdButtons();

	int x = m_pkZeroRts->m_iWidth-300-10, y = 10;
	x += (CMD_BN_SIZE+2) * (iNumVisibleButtons % 6);
	y += (CMD_BN_SIZE+2) * (iNumVisibleButtons / 6);

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
}

void UserPanel::OnClickCmdButton(int iCtrlID)
{
	if(iCtrlID >= ID_CMD_BUTTONS_START && iCtrlID < 
		ID_CMD_BUTTONS_START+MAX_NUM_CMD_BNS)
		return;

	ZGuiButton* pkButton = (ZGuiButton*) 
		m_pkGuiBuilder->GetChild(m_pkDlgBox, iCtrlID);

	if(pkButton == NULL)
		return;

	if(m_szLastCmd)
		delete[] m_szLastCmd;

	m_szLastCmd = new char[25];
	strcpy(m_szLastCmd, "Move");
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
	if(m_szLastCmd != NULL)
	{
		strcpy(szCommand, m_szLastCmd);
		delete[] m_szLastCmd;
		m_szLastCmd = NULL;
		return true;
	}

	return false;
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


