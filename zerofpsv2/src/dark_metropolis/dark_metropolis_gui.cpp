// dark_metropolis_gui.cpp

#include "dark_metropolis.h"

bool GUIPROC(ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
	switch(msg)
	{
	case ZGM_COMMAND:
		g_kDM.OnCommand(((int*)params)[0], 
			(((int*)params)[1] == 1) ? true : false, win);
		break;
	case ZGM_LBUTTONDOWN:
		g_kDM.OnClick(((int*)params)[0], ((int*)params)[1], true, true, win);
		break;
	case ZGM_LBUTTONUP:
		g_kDM.OnClick(((int*)params)[0], ((int*)params)[1], false, true, win);
		break;
	case ZGM_RBUTTONDOWN:    
		g_kDM.OnClick(((int*)params)[0], ((int*)params)[1], true, false, win);
		break;
	case ZGM_RBUTTONUP:
		g_kDM.OnClick(((int*)params)[0], ((int*)params)[1], false, false, win);
		break;
	case ZGM_LBUTTONDBLCLK:
		g_kDM.OnDoubleClick(((int*)params)[0], ((int*)params)[1], true, win);
		break;
	case ZGM_RBUTTONDBLCLK:
		g_kDM.OnDoubleClick(((int*)params)[0], ((int*)params)[1], false, win);
		break;
	case ZGM_MOUSEMOVE:
		g_kDM.OnMouseMove(((int*)params)[1], ((int*)params)[2], 
			((int*)params)[0] == 1 ? true : false, win);
		break;
	case ZGM_SCROLL:
		g_kDM.OnScroll(((int*)params)[0], ((int*)params)[2], win);
		break;
	case ZGM_CBN_SELENDOK:
		g_kDM.OnSelectCB(((int*)params)[0], ((int*)params)[1], win);
		break;
	case ZGM_KEYPRESS:
		g_kDM.OnKeyPress(((int*)params)[0], win);		
		break;
	}
	return true;
}

void DarkMetropolis::OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd *pkMainWnd)
{
}

void DarkMetropolis::OnClick(int x, int y, bool bMouseDown, bool bLeftButton, 
									  ZGuiWnd *pkMain)
{
}

void DarkMetropolis::OnDoubleClick(int x, int y, bool bLeftButton, 
											  ZGuiWnd *pkMain)
{
}

void DarkMetropolis::OnMouseMove(int x, int y, bool bMouseDown, ZGuiWnd *pkMain)
{
}

void DarkMetropolis::OnScroll(int iID, int iPos, ZGuiWnd *pkMain)
{
}

void DarkMetropolis::OnSelectCB(int ListBoxID, int iItemIndex, ZGuiWnd *pkMain)
{
}

void DarkMetropolis::OnKeyPress(int iKey, ZGuiWnd *pkWnd)
{
}