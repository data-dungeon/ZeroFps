// scriptinterfaces.cpp: implementation of the ScriptInterfaces class.
//
//////////////////////////////////////////////////////////////////////

#include "scriptinterfaces.h"

#define _DONT_MAIN
#include "zerotank.h"

extern ZeroTank g_kZeroTank;

// Name: CreateWndLua
// Parameters:
// (0) int iType, (1) char* szResourceName, (2) char* szText, (3) int iID, 
// (4) int parentID, (5) int x, (6) int y, (7) int w, (8) int h, 
// (9) unsigned long uiFlags
int GuiAppLua::CreateWndLua(lua_State* pkLua)
{
	ZFScript* pkScript = g_kZeroTank.GetScript();

	int iNumArgs = pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 10)
	{
		printf("Failed to create window! : to few arguments\n");
		return 0;
	}

	double dType;
	pkScript->GetArg(pkLua, 0, &dType);

	char szResName[50], szText[50];
	pkScript->GetArg(pkLua, 1, szResName);
	pkScript->GetArg(pkLua, 2, szText);

	double dID, dParentID;
	pkScript->GetArg(pkLua, 3, &dID);
	pkScript->GetArg(pkLua, 4, &dParentID);

	double x, y, w, h, f;
	pkScript->GetArg(pkLua, 5, &x);
	pkScript->GetArg(pkLua, 6, &y);
	pkScript->GetArg(pkLua, 7, &w);
	pkScript->GetArg(pkLua, 8, &h);
	pkScript->GetArg(pkLua, 9, &f);

	GuiType eType = GuiType_Error;

	switch( (int) dType )
	{
		case 0:  eType = Wnd;		   break;
		case 1:  eType = Button;	   break;
		case 2:  eType = Checkbox;	   break;
		case 3:  eType = Combobox;	   break;
		case 4:  eType = Label;		   break;
		case 5:  eType = Listbox;	   break;
		case 6:  eType = Radiobutton; break;
		case 7:  eType = Scrollbar;   break;
		case 8:  eType = Slider;	   break;
		case 9:  eType = TabControl;  break;
		case 10: eType = Textbox;	   break;
		case 11: eType = Treebox;	   break;
		default:
		{
			printf("Failed to create window! : wrong type\n");
			return 0;
		}
	}

	g_kZeroTank.CreateWnd(eType, szResName, szText, (int) dID, dParentID, 
		(int) x, (int) y, (int) w, (int) h, (unsigned long) f);

	return 1;
}

// Name: AddTabPageLua
// Parameters:
// (0) int iParentID, (1) int iID
int GuiAppLua::AddTabPageLua(lua_State* pkLua)
{
	ZFScript* pkScript = g_kZeroTank.GetScript();

	int iNumArgs = pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 2)
		return 0;

	double dType;
	pkScript->GetArg(pkLua, 0, &dType);

	char szResName[100];
	pkScript->GetArg(pkLua, 1, szResName);

	g_kZeroTank.AddTabPage((int)dType, szResName);

	return 1;
}

// Close window
// Parameters:
// (0) int iID to wnd
int GuiAppLua::CloseWndLua(lua_State* pkLua)
{
	ZFScript* pkScript = g_kZeroTank.GetScript();

	int iNumArgs = pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 1)
		return 0;

	char szWnd[100];
	pkScript->GetArg(pkLua, 0, szWnd);

	g_kZeroTank.CloseWindow(szWnd);

	return 1;
}

// Close window
// Parameters:
// (0) int iID to wnd
// (1) char* Name of the skin declared in the script file
// (2) char* Name of the type of skin.
// Like: "Window", "Button up", "Button down" (look inside the function GetWndSkinsDesc)
int GuiAppLua::ChangeSkinLua(lua_State* pkLua)
{	
	ZFScript* pkScript = g_kZeroTank.GetScript();

	int iNumArgs = pkScript->GetNumArgs(pkLua);
	
	if(iNumArgs != 3)
		return 0;

	double wndID;
	pkScript->GetArg(pkLua, 0, &wndID);

	char szSkinName[50];
	pkScript->GetArg(pkLua, 1, szSkinName);

	char szSkinType[50];
	pkScript->GetArg(pkLua, 2, szSkinType);

	g_kZeroTank.ChangeSkin(pkScript, (int) wndID, szSkinName, szSkinType);

	return 1;
}

int GuiAppLua::GetScreenWidthLua(lua_State* pkLua)
{
	ZFScript* pkScript = g_kZeroTank.GetScript();
	pkScript->AddReturnValue(pkLua, (double) g_kZeroTank.m_iWidth);
	return 1;
}

int GuiAppLua::GetScreenHeightLua(lua_State* pkLua)
{
	ZFScript* pkScript = g_kZeroTank.GetScript();
	pkScript->AddReturnValue(pkLua, g_kZeroTank.m_iHeight);
	return 1;
}

int GuiAppLua::IsWndVisibleLua(lua_State* pkLua)
{
	ZFScript* pkScript = g_kZeroTank.GetScript();

	int iNumArgs = pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 1)
		return 0;

	char szName[100];
	pkScript->GetArg(pkLua, 0, szName);

	int ret = 0;
	
	if(g_kZeroTank.IsWndVisible(szName))
		ret = 1;

	pkScript->AddReturnValue(pkLua, ret);

	return 1;
}

// Close window
// Parameters:
// (0) char* resName of the Listbox
// (1) char* Item name
int GuiAppLua::AddListboxItemLua(lua_State* pkLua)
{
	ZFScript* pkScript = g_kZeroTank.GetScript();

	int iNumArgs = pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 2)
		return 0;

	char szWndName[100];
	pkScript->GetArg(pkLua, 0, szWndName);

	char szItemName[100];
	pkScript->GetArg(pkLua, 1, szItemName);

	g_kZeroTank.AddListItem(szWndName, szItemName);
	
	return 1;
}

// ClearListbox
// Parameters:
// (0) char* resName of the Listbox
int GuiAppLua::ClearListboxLua(lua_State* pkLua)
{
	ZFScript* pkScript = g_kZeroTank.GetScript();

	int iNumArgs = pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 1)
		return 0;

	char szWndName[100];
	pkScript->GetArg(pkLua, 0, szWndName);

	g_kZeroTank.ClearListbox(szWndName);

	return 1;
}

// GetWndLua
// Description: Check to if a Window exist. Returns -1 if the window does not exist 
// or a possitive value that is the ID number of the window if it exist.
// Parameters:
// (0) char* resName of the Listbox
int GuiAppLua::GetWndLua(lua_State* pkLua)
{
	ZFScript* pkScript = g_kZeroTank.GetScript();

	int iNumArgs = pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 1)
		return 0;

	char szWndName[100];
	pkScript->GetArg(pkLua, 0, szWndName);

	int ret = g_kZeroTank.GetWndID(szWndName);

	pkScript->AddReturnValue(pkLua, ret);

	return 1;
}

// SetText
// Parameters:
// (0) char* resName of the Listbox
// (1) 
int GuiAppLua::SetTextInt(lua_State* pkLua)
{
	ZFScript* pkScript = g_kZeroTank.GetScript();

	int iNumArgs = pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 2)
		return 0;

	char szWndName[100];
	pkScript->GetArg(pkLua, 0, szWndName);

	double dValue;
	pkScript->GetArg(pkLua, 1, &dValue);

	g_kZeroTank.SetTextInt(szWndName, (int) dValue);

	return 1;

}
