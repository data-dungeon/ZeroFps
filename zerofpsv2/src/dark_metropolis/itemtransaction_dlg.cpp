#include "dark_metropolis.h"
#include "itemtransaction_dlg.h"

CItemTransactionDlg::CItemTransactionDlg() : CGameDlg(
	"ItemTransactionWnd", &g_kDM)
{
}

CItemTransactionDlg::~CItemTransactionDlg() 
{
}

void CItemTransactionDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	if(strClickName == "ItemTransactionCloseBn")
	{
	//	m_pkGui->KillWndCapture(); 
		ShowWnd("ItemTransactionWnd", false/*, true*/);
		ShowWnd("ItemRemoveWnd", false/*, true*/);
		ShowWnd("ItemAddWnd", false/*, true*/);

		if(GetWnd("HQWnd")->IsVisible())
		{
			m_pkGui->KillWndCapture(); 
			m_pkGui->SetCaptureToWnd(GetWnd("HQWnd"));
		}
	}
}