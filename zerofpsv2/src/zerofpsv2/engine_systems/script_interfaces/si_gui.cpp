// scriptinterfaces.cpp: implementation of the ScriptInterfaces class.
//
//////////////////////////////////////////////////////////////////////

#include "../../script/zfscript.h"
#include "../../gui/zgui.h"
#include "si_gui.h"

void GuiAppLua::Init(ZGuiApp* pkGuiApp, ZFScriptSystem* pkScript)
{
	g_pkGuiApp = pkGuiApp;
	g_pkScript = pkScript;

	pkScript->ExposeFunction("CreateWnd", GuiAppLua::CreateWndLua);
	pkScript->ExposeFunction("AddTabPage", GuiAppLua::AddTabPageLua);
	pkScript->ExposeFunction("AddListItem", GuiAppLua::AddListboxItemLua);
	pkScript->ExposeFunction("ClearListbox", GuiAppLua::ClearListboxLua);
	pkScript->ExposeFunction("GetWnd", GuiAppLua::GetWndLua);
	pkScript->ExposeFunction("ShowWnd", GuiAppLua::ShowWndLua); 
	pkScript->ExposeFunction("ChangeSkin", GuiAppLua::ChangeSkinLua); 
	pkScript->ExposeFunction("GetScreenWidth", GuiAppLua::GetScreenWidthLua); 
	pkScript->ExposeFunction("GetScreenHeight", GuiAppLua::GetScreenHeightLua); 
	pkScript->ExposeFunction("IsWndVisible", GuiAppLua::IsWndVisibleLua); 
	pkScript->ExposeFunction("CloseWnd", GuiAppLua::CloseWndLua); 
	pkScript->ExposeFunction("SetTextInt", GuiAppLua::SetTextInt); 
	pkScript->ExposeFunction("AddTreeItem", GuiAppLua::AddTreeItemLua); 

}

// Name: CreateWndLua
// Parameters:
// (0) int iType, 
// (1) char* szWndName, 
// (2) char* szParentName
// (3) char* szLabel, 
// (4) int x, 
// (5) int y, 
// (6) int w, 
// (7) int h, 
// (8) unsigned long uiFlags
int GuiAppLua::CreateWndLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 9)
	{
		char szWindowName[50];
		g_pkScript->GetArg(pkLua, 1, szWindowName);
		printf("Failed to create window: %s! : not correct number of arguments\n", 
			szWindowName);
		return 0;
	}

	double dType;
	g_pkScript->GetArg(pkLua, 0, &dType);

	char szWindowName[50], szParentName[50], szText[50];
	g_pkScript->GetArg(pkLua, 1, szWindowName);
	g_pkScript->GetArg(pkLua, 2, szParentName);
	g_pkScript->GetArg(pkLua, 3, szText);

	double x, y, w, h, f;
	g_pkScript->GetArg(pkLua, 4, &x);
	g_pkScript->GetArg(pkLua, 5, &y);
	g_pkScript->GetArg(pkLua, 6, &w);
	g_pkScript->GetArg(pkLua, 7, &h);
	g_pkScript->GetArg(pkLua, 8, &f);

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

	g_pkGuiApp->CreateWnd(eType, szWindowName, szParentName, szText,
		(int) x, (int) y, (int) w, (int) h, (unsigned long) f);

	return 1;
}

// Name: AddTabPageLua
// Parameters:
// (0) char* szTabboxResName 
// (1) char* szPageResName
// (1) char* szPageLabel
int GuiAppLua::AddTabPageLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 3)
		return 0;

	char szTabboxResName[100];
	g_pkScript->GetArg(pkLua, 0, szTabboxResName);

	char szPageResName[100];
	g_pkScript->GetArg(pkLua, 1, szPageResName);

	char szPageLabel[100];
	g_pkScript->GetArg(pkLua, 2, szPageLabel);

	g_pkGuiApp->AddTabPage(szTabboxResName, szPageResName, szPageLabel);

	return 1;
}

// Close window
// Parameters:
// (0) char* id to wnd
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
// (0) char* Name of the window
// (1) char* Name of the skin declared in the script file
// (2) char* Name of the type of skin.
// Like: "Window", "Button up", "Button down" (look inside the function GetWndSkinsDesc)
int GuiAppLua::ChangeSkinLua(lua_State* pkLua)
{	
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);
	
	if(iNumArgs != 3)
		return 0;

	char szID[50];
	g_pkScript->GetArg(pkLua, 0, szID);

	char szSkinName[50];
	g_pkScript->GetArg(pkLua, 1, szSkinName);

	char szSkinType[50];
	g_pkScript->GetArg(pkLua, 2, szSkinType);

	g_pkGuiApp->ChangeSkin(g_pkScript, szID, szSkinName, szSkinType);

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

// AddTreeItemLua
// Parameters:
// (0) [szTreeboxName] char* name of the Treebox
// (1) [szParentName] char* name of the Parent
// (2) [szNodeName] char* name of the Node
// (3) [szNodeLabel] char* label of the Node
// (4) [child node] double, if the node is a child that can be selected
int GuiAppLua::AddTreeItemLua(lua_State* pkLua)
{	
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 5)
		return 0;

	char szTreeboxName[100], szParentName[100], 
		  szNodeName[100], szNodeLabel[100];
	double dIsChild;

	g_pkScript->GetArg(pkLua, 0, szTreeboxName);
	g_pkScript->GetArg(pkLua, 1, szParentName);
	g_pkScript->GetArg(pkLua, 2, szNodeName);
	g_pkScript->GetArg(pkLua, 3, szNodeLabel);
	g_pkScript->GetArgNumber(pkLua, 4, &dIsChild);

	if(dIsChild > 0)
		g_pkGuiApp->AddTreeItem(szTreeboxName, szNodeName, szParentName, szNodeLabel, 0, 0);
	else
		g_pkGuiApp->AddTreeItem(szTreeboxName, szNodeName, szParentName, szNodeLabel, 1, 2);

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

int GuiAppLua::ShowWndLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 2)
		return 0;

	char szName[100];
	g_pkScript->GetArg(pkLua, 0, szName);

	double show;
	g_pkScript->GetArg(pkLua, 1, &show);

	ZGuiWnd* pkWnd;
	if((pkWnd = g_pkGuiApp->GetWnd(szName)) != NULL)
	{
		if(show > 0)
			pkWnd->Show();
		else
			pkWnd->Hide();
	}
		
	return 1;
}