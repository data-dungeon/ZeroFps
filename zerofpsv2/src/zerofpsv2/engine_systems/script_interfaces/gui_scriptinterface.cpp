// scriptinterfaces.cpp: implementation of the ScriptInterfaces class.
//
//////////////////////////////////////////////////////////////////////

#include "../../script/zfscript.h"
#include "../../gui/zgui.h"
#include "gui_scriptinterface.h"

void GuiAppLua::Init(ZGuiApp* pkGuiApp, ZFScript* pkScript)
{
	g_pkGuiApp = pkGuiApp;
	g_pkScript = pkScript;

	pkScript->ExposeFunction("CreateWnd", GuiAppLua::CreateWndLua);
	pkScript->ExposeFunction("AddTabPage", GuiAppLua::AddTabPageLua);
	pkScript->ExposeFunction("AddListItem", GuiAppLua::AddListboxItemLua);
	pkScript->ExposeFunction("ClearListbox", GuiAppLua::ClearListboxLua);
	pkScript->ExposeFunction("GetWnd", GuiAppLua::GetWndLua);
	pkScript->ExposeFunction("CloseWnd", GuiAppLua::CloseWndLua); 
	pkScript->ExposeFunction("ChangeSkin", GuiAppLua::ChangeSkinLua); 
	pkScript->ExposeFunction("GetScreenWidth", GuiAppLua::GetScreenWidthLua); 
	pkScript->ExposeFunction("GetScreenHeight", GuiAppLua::GetScreenHeightLua); 
	pkScript->ExposeFunction("IsWndVisible", GuiAppLua::IsWndVisibleLua); 
	pkScript->ExposeFunction("SetTextInt", GuiAppLua::SetTextInt); 
}

// Name: CreateWndLua
// Parameters:
// (0) int iType, (1) char* szResourceName, (2) char* szText, (3) int iID, 
// (4) int parentID, (5) int x, (6) int y, (7) int w, (8) int h, 
// (9) unsigned long uiFlags
int GuiAppLua::CreateWndLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 10)
	{
		printf("Failed to create window! : to few arguments\n");
		return 0;
	}

	double dType;
	g_pkScript->GetArg(pkLua, 0, &dType);

	char szResName[50], szText[50];
	g_pkScript->GetArg(pkLua, 1, szResName);
	g_pkScript->GetArg(pkLua, 2, szText);

	double dID, dParentID;
	g_pkScript->GetArg(pkLua, 3, &dID);
	g_pkScript->GetArg(pkLua, 4, &dParentID);

	double x, y, w, h, f;
	g_pkScript->GetArg(pkLua, 5, &x);
	g_pkScript->GetArg(pkLua, 6, &y);
	g_pkScript->GetArg(pkLua, 7, &w);
	g_pkScript->GetArg(pkLua, 8, &h);
	g_pkScript->GetArg(pkLua, 9, &f);

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

	g_pkGuiApp->CreateWnd(eType, szResName, szText, (int) dID, (int) dParentID, 
		(int) x, (int) y, (int) w, (int) h, (unsigned long) f);

	return 1;
}

// Name: AddTabPageLua
// Parameters:
// (0) int iParentID, (1) int iID
int GuiAppLua::AddTabPageLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 2)
		return 0;

	double dType;
	g_pkScript->GetArg(pkLua, 0, &dType);

	char szResName[100];
	g_pkScript->GetArg(pkLua, 1, szResName);

	g_pkGuiApp->AddTabPage((int)dType, szResName);

	return 1;
}

// Close window
// Parameters:
// (0) int iID to wnd
int GuiAppLua::CloseWndLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 1)
		return 0;

	char szWnd[100];
	g_pkScript->GetArg(pkLua, 0, szWnd);

	g_pkGuiApp->CloseWindow(szWnd); 

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
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);
	
	if(iNumArgs != 3)
		return 0;

	double wndID;
	g_pkScript->GetArg(pkLua, 0, &wndID);

	char szSkinName[50];
	g_pkScript->GetArg(pkLua, 1, szSkinName);

	char szSkinType[50];
	g_pkScript->GetArg(pkLua, 2, szSkinType);

	g_pkGuiApp->ChangeSkin(g_pkScript, (int) wndID, szSkinName, szSkinType);

	return 1;
}

int GuiAppLua::GetScreenWidthLua(lua_State* pkLua)
{
	g_pkScript->AddReturnValue(pkLua, (double) g_pkGuiApp->GetWidth());
	return 1;
}

int GuiAppLua::GetScreenHeightLua(lua_State* pkLua)
{
	g_pkScript->AddReturnValue(pkLua, g_pkGuiApp->GetHeight());
	return 1;
}

int GuiAppLua::IsWndVisibleLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 1)
		return 0;

	char szName[100];
	g_pkScript->GetArg(pkLua, 0, szName);

	int ret = 0;

	if(g_pkGuiApp->IsWndVisible(szName)) 
		ret = 1;

	g_pkScript->AddReturnValue(pkLua, ret);

	return 1;
}

// Close window
// Parameters:
// (0) char* resName of the Listbox
// (1) char* Item name
int GuiAppLua::AddListboxItemLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 2)
		return 0;

	char szWndName[100];
	g_pkScript->GetArg(pkLua, 0, szWndName);

	char szItemName[100];
	g_pkScript->GetArg(pkLua, 1, szItemName);

	g_pkGuiApp->AddListItem(szWndName, szItemName);
	
	return 1;
}

// ClearListbox
// Parameters:
// (0) char* resName of the Listbox
int GuiAppLua::ClearListboxLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 1)
		return 0;

	char szWndName[100];
	g_pkScript->GetArg(pkLua, 0, szWndName);

	g_pkGuiApp->ClearListbox(szWndName); 


	return 1;
}

// GetWndLua
// Description: Check to if a Window exist. Returns -1 if the window does not exist 
// or a possitive value that is the ID number of the window if it exist.
// Parameters:
// (0) char* resName of the Listbox
int GuiAppLua::GetWndLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 1)
		return 0;

	char szWndName[100];
	g_pkScript->GetArg(pkLua, 0, szWndName);

	int ret = g_pkGuiApp->GetWndID(szWndName);

	g_pkScript->AddReturnValue(pkLua, ret);

	return 1;
}

// SetText
// Parameters:
// (0) char* resName of the Listbox
// (1) 
int GuiAppLua::SetTextInt(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 2)
		return 0;

	char szWndName[100];
	g_pkScript->GetArg(pkLua, 0, szWndName);

	double dValue;
	g_pkScript->GetArg(pkLua, 1, &dValue);

	g_pkGuiApp->SetTextInt(szWndName, (int) dValue);

	return 1;
}
