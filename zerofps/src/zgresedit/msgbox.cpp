// msgbox.cpp: implementation of the MsgBox class.
//
//////////////////////////////////////////////////////////////////////

#include "msgbox.h"
#include "../zerofps/engine/zgui/zgui.h"
#include "../zerofps/engine/zgui/zguilabel.h"
#include "../zerofps/engine/zgui/zguibutton.h"
#include "../zerofps/basic/zguiskin.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static bool MSGROC( ZGuiWnd* pkWindow, unsigned int uiMessage, 
					int iNumberOfParams, void *pkParams ) 
{
	return true;
}

MsgBox::MsgBox(ZGui* pkGui, char* szText) : ZGuiWnd(Rect(200,200,400,300))
{
/*	m_iID = 1212389;

	m_pkGui = pkGui;

	m_pkLabel = new ZGuiLabel(Rect(10,10,100,30), this, true, 999);
	
	if(szText)
		m_pkLabel->SetText(szText);

	m_pkOKBn = new ZGuiButton(Rect(10,70,100,90),this,true,996);
	m_pkCancelBn = new ZGuiButton(Rect(110,70,190,90),this,true,997);

	m_pkWndSkin = new ZGuiSkin(-1, -1, -1, -1, 255, 255, 255, 0, 0, 0, 1);
	m_pkButtonSkinUp = new ZGuiSkin(-1, -1, -1, -1, 255, 0, 255, 0, 0, 0, 1);
	m_pkButtonSkinDown = new ZGuiSkin(-1, -1, -1, -1, 128, 128, 0, 0, 0, 0, 1);

	m_pkOKBn->SetButtonHighLightSkin(m_pkButtonSkinDown);
	m_pkOKBn->SetButtonDownSkin(m_pkButtonSkinDown);
	m_pkOKBn->SetButtonUpSkin(m_pkButtonSkinUp);
	m_pkOKBn->SetText("OK");

	m_pkCancelBn->SetButtonHighLightSkin(m_pkButtonSkinDown);
	m_pkCancelBn->SetButtonDownSkin(m_pkButtonSkinDown);
	m_pkCancelBn->SetButtonUpSkin(m_pkButtonSkinUp);
	m_pkCancelBn->SetText("Cancel");

	SetSkin(m_pkWndSkin);

	m_pkLabel->Show();
	Show();*/

//	m_pkGui->AddMainWindow(998, this, MSGROC, true);
}

MsgBox::~MsgBox()
{
//	m_pkGui->UnregisterWindow(this);

//	delete m_pkLabel;
//	delete m_pkSkin;
}
