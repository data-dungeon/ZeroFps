// inventorydlg.h: interface for the InventoryDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INVENTORYDLG_H__E2EC300C_10CF_4D5C_B948_2FD7FCAB2686__INCLUDED_)
#define AFX_INVENTORYDLG_H__E2EC300C_10CF_4D5C_B948_2FD7FCAB2686__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../zerofpsv2/basic/zfsubsystem.h"
#include "../zerofpsv2/gui/zguilabel.h"

#include <vector>
using namespace std;

class InventoryDlg : public ZFSubSystem  
{
public:
	InventoryDlg();
	~InventoryDlg();

private:
	vector<ZGuiLabel*> m_kSlots;

};

#endif // !defined(AFX_INVENTORYDLG_H__E2EC300C_10CF_4D5C_B948_2FD7FCAB2686__INCLUDED_)
