// selectwnd.cpp: implementation of the SelectWnd class.
//
///////////////////////////////////////////////////////////////////////////////

#include "selectwnd.h"
#include "../zerofps/engine/zgui/zguiwindow.h"

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//
SelectWnd* SelectWnd::m_pkInstance = 0;

SelectWnd::SelectWnd()
{
	m_pkWnd = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Name: GetInstance
// Description: 
//
SelectWnd* SelectWnd::GetInstance()
{
	if(m_pkInstance == 0)
		m_pkInstance = new SelectWnd;
	return m_pkInstance;
}
