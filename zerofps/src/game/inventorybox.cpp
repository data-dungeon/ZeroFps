// inventorybox.cpp: implementation of the InventoryBox class.
//
//////////////////////////////////////////////////////////////////////

#include "inventorybox.h"
#include "gui_resource_files/inventary_id.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

InventoryBox::InventoryBox(ZGui* pkGui, ZGuiWndProc oMainWndProc) 
	: DlgBox(pkGui, oMainWndProc)
{
	Create(0,0,0,0,oMainWndProc);
}

InventoryBox::~InventoryBox()
{

}

bool InventoryBox::DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
						   int iNumberOfParams,void *pkParams ) 
{
	switch(uiMessage)
	{
	// Command Messages
	case ZGM_COMMAND:
		switch(((int*)pkParams)[0]) // control id
		{
		case InventCloseBN:
			OnClose(false);
			break;
		}
		break;
	}

	return true;
}

bool InventoryBox::Create(int x, int y, int w, int h, ZGuiWndProc pkWndProc)
{
	if(m_pkGui->LoadDialog("../src/game/gui_resource_files/inventary_rc.txt", 
		"InventoryWnd", pkWndProc) == false)
		return false;

	m_pkDlgBox = m_pkGuiMan->Wnd("InventoryWnd");

	if(m_pkDlgBox == NULL)
		return false;

	m_pkGui->ShowMainWindow(m_pkDlgBox, false);
	return true;
}

bool InventoryBox::OnOpen(int x, int y)
{
	m_pkDlgBox->SetPos(x,y,true,true);
	m_pkGui->ShowMainWindow(m_pkDlgBox, true);
	return true;
}

bool InventoryBox::OnClose(bool bSave)
{
	m_pkGui->ShowMainWindow(m_pkDlgBox, false);
	return true;
}
