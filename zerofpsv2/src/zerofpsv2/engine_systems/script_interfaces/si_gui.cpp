// scriptinterfaces.cpp: implementation of the ScriptInterfaces class.
//
//////////////////////////////////////////////////////////////////////

#include "../../script/zfscript.h"
#include "../../gui/zgui.h"
#include "si_gui.h"

//ZFScriptSystem*	GuiAppLua::g_pkScript;
//ZGuiApp* 			GuiAppLua::g_pkGuiApp;

void GuiAppLua::Init(ZGuiApp* pkGuiApp, ZFScriptSystem* pkScript)
{
	cout << "Add SI: GuiApp" << endl;

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
	pkScript->ExposeFunction("SetTextString", GuiAppLua::SetTextStringLua); 
	pkScript->ExposeFunction("AddTreeItem", GuiAppLua::AddTreeItemLua);
	pkScript->ExposeFunction("SetMoveArea", GuiAppLua::SetMoveAreaLua);		
	pkScript->ExposeFunction("SetFont", GuiAppLua::SetFontLua);
	pkScript->ExposeFunction("ChangeWndParameter", GuiAppLua::ChangeWndParameterLua);
	pkScript->ExposeFunction("CreateNewRadiobuttonGroup", GuiAppLua::CreateNewRadiobuttonGroupLua);
	
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

/**	\fn CreateWndLua(iType, szWndName, szParentName, szLabel, iX, iY, iW, iH, iFlags)
 	\relates MistLandGuiScript
	\brief Creates a window.

	\param iType Desc
	\param szWndName Desc
	\param szParentName Desc
	\param szLabel Desc
	\param iX Desc
	\param iY Desc
	\param iW Desc
	\param iH Desc
	\param iFlags Desc
	\param iWndAlignent (0-6, se WndAlignent at zguiapp.h)
	\param iWndResizeType (0-3, se WndResizeType at zguiapp.h)

	Creates a window in the GUI.	
*/

int GuiAppLua::CreateWndLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNumArgs < 8)
	{
		char szWindowName[50];
		g_pkScript->GetArg(pkLua, 1, szWindowName);
		printf("Failed to create window: %s! : not correct number of arguments\n", 
			szWindowName);
		return 0;
	}

	double dType;
	g_pkScript->GetArg(pkLua, 0, &dType);

	char szWindowName[50], szParentName[50], szText[512];
	g_pkScript->GetArg(pkLua, 1, szWindowName);
	g_pkScript->GetArg(pkLua, 2, szParentName);
	g_pkScript->GetArgString(pkLua, 3, szText);

	double x, y, w, h, f=0, wndalignent=0, wndresizetype=0;
	g_pkScript->GetArg(pkLua, 4, &x);
	g_pkScript->GetArg(pkLua, 5, &y);
	g_pkScript->GetArg(pkLua, 6, &w);
	g_pkScript->GetArg(pkLua, 7, &h);
	if(iNumArgs > 7) g_pkScript->GetArg(pkLua, 8, &f);
	if(iNumArgs > 8) g_pkScript->GetArg(pkLua, 9, &wndalignent);
	if(iNumArgs > 9) g_pkScript->GetArg(pkLua, 10, &wndresizetype);

	GuiType eType = GuiType_Error;
	WndAlignent eWndAlignent = TopLeft;
	WndResizeType eWndResizeType = eNone;

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
		case 12: eType = Menu;		   break;
		case 13: eType = Progressbar; break;
		default:
		{
			printf("Failed to create window! : wrong type\n");
			return 0;
		}
	}

	switch( (int) wndalignent )
	{
		case 0:  eWndAlignent = TopLeft;		   break;
		case 1:  eWndAlignent = TopRight;	   break;
		case 2:  eWndAlignent = BottomLeft;	   break;
		case 3:  eWndAlignent = BottomRight;	break;
		case 4:  eWndAlignent = CenterHorz;		break;
		case 5:  eWndAlignent = CenterVert;	   break;
		case 6:  eWndAlignent = Center;			break;
		default:
		{
			printf("Failed to create window! : wrong wndalignent\n");
			return 0;
		}
	}

	switch( (int) wndresizetype )
	{
		case 0:  eWndResizeType = eNone;				break;
		case 1:  eWndResizeType = ResizeWidth;	   break;
		case 2:  eWndResizeType = ResizeHeight;	break;
		case 3:  eWndResizeType = Resize;			break;
		default:
		{
			printf("Failed to create window! : wrong wndresizetype\n");
			return 0;
		}
	}

	g_pkGuiApp->CreateWnd(eType, szWindowName, szParentName, szText,
		(int) x, (int) y, (int) w, (int) h, (unsigned long) f,
		eWndAlignent, eWndResizeType);

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

	char szID[150];
	g_pkScript->GetArg(pkLua, 0, szID);

	char szSkinName[150];
	g_pkScript->GetArg(pkLua, 1, szSkinName);

	char szSkinType[150];
	g_pkScript->GetArg(pkLua, 2, szSkinType);

	g_pkGuiApp->ChangeSkin(pkLua, szID, szSkinName, szSkinType);

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

/**	\fn ClearListbox( resName )
 	\relates MistLandGuiScript
	\brief Clears a Listbox.
	\param resName resName of the Listbox.
*/
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


// SetText
// Parameters:
// (0) char* resName of the Listbox
// (1) en sträng, max 16 tusen tecken stor.
int GuiAppLua::SetTextStringLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 2)
		return 0;

	char szWndName[100];
	g_pkScript->GetArg(pkLua, 0, szWndName);

	char szText[16000];
	g_pkScript->GetArg(pkLua, 1, szText);
	string temp = g_pkScript->FormatMultiLineTextFromLua(szText);
	strcpy(szText, temp.c_str());

	g_pkGuiApp->SetText(szWndName, szText);

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

	if(iNumArgs != 3)
		return 0;

	char szName[100];
	g_pkScript->GetArg(pkLua, 0, szName);

	double show;
	g_pkScript->GetArg(pkLua, 1, &show);

	double set_focus;
	g_pkScript->GetArg(pkLua, 2, &set_focus);

	ZGuiWnd* pkWnd;
	if((pkWnd = g_pkGuiApp->GetWnd(szName)) != NULL)
	{
		if(show > 0)
			pkWnd->Show();
		else
			pkWnd->Hide();

		if(set_focus > 0)
			pkWnd->SetFocus();
	}
		
	return 1;
}

int GuiAppLua:: SetMoveAreaLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 5)
		return 0;

	char szWnd[100];
	g_pkScript->GetArg(pkLua, 0, szWnd);

	double l, t, r, b;
	g_pkScript->GetArg(pkLua, 1, &l);
	g_pkScript->GetArg(pkLua, 2, &t);
	g_pkScript->GetArg(pkLua, 3, &r);
	g_pkScript->GetArg(pkLua, 4, &b);

	ZGuiWnd* pkWnd = g_pkGuiApp->GetWnd(szWnd);

	if(pkWnd)
	{
		pkWnd->SetMoveArea( Rect((int)l,(int)t,(int)r,(int)b), true);
	}

	return 1;
}

// 1:st arg: String name of window
// 2:nd arg: String name of font
// 3:rd arg: (optinal) color red
// 4:rt arg: (optinal) color green
// 5:th arg: (optinal) color blue
// 6:th arg: (optinal) glyph
int GuiAppLua::SetFontLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	char szWindow[250], szFont[250];
	g_pkScript->GetArg(pkLua, 0, szWindow);
	g_pkScript->GetArg(pkLua, 1, szFont);

	double dRed=0, dGreen=0, dBlue=0, dGlyph=0;
	if(iNumArgs > 2) g_pkScript->GetArgNumber(pkLua, 2, &dRed);
	if(iNumArgs > 3) g_pkScript->GetArgNumber(pkLua, 3, &dGreen);
	if(iNumArgs > 4) g_pkScript->GetArgNumber(pkLua, 4, &dBlue);
	if(iNumArgs > 5) g_pkScript->GetArgNumber(pkLua, 5, &dGlyph);

	ZGuiWnd* pkWnd = g_pkGuiApp->GetWnd(szWindow);
	if(pkWnd)
	{
		g_pkGuiApp->SetFont(szWindow, szFont, dRed, dGreen, dBlue, dGlyph);
	}
		
	return 1;
}

// 1:st Arg: String name of Wnd
// 2:rd Arg: String for parameter to set
int GuiAppLua::ChangeWndParameterLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	char szWindow[250], szParameter[250];
	g_pkScript->GetArg(pkLua, 0, szWindow);
	g_pkScript->GetArg(pkLua, 1, szParameter);

	ZGuiWnd* pkWnd = g_pkGuiApp->GetWnd(szWindow);
	if(pkWnd)
	{
		GuiType eWndType = g_pkGuiApp->GetWndType(pkWnd);

		if(eWndType == Progressbar)
		{
			// ProgressbarDir
			if(!strcmp("PBDIR_LEFT_TO_RIGHT", szParameter))
				((ZGuiProgressbar*)pkWnd)->SetDir(PBDIR_LEFT_TO_RIGHT); 
			else 
			if(!strcmp("PBDIR_RIGHT_TO_LEFT", szParameter))
				((ZGuiProgressbar*)pkWnd)->SetDir(PBDIR_RIGHT_TO_LEFT); 
			else 
			if(!strcmp("PBDIR_TOP_TO_BOTTOM", szParameter))
				((ZGuiProgressbar*)pkWnd)->SetDir(PBDIR_TOP_TO_BOTTOM); 
			else 
			if(!strcmp("PBDIR_BOTTOM_TO_TOP", szParameter))
				((ZGuiProgressbar*)pkWnd)->SetDir(PBDIR_BOTTOM_TO_TOP); 

			// ProgressbarTextOrientation
			if(!strcmp("PBTEXTORIENT_CENTER", szParameter))
				((ZGuiProgressbar*)pkWnd)->SetTextOrientation(PBTEXTORIENT_CENTER); 
			else
			if(!strcmp("PBTEXTORIENT_LEFT", szParameter))
				((ZGuiProgressbar*)pkWnd)->SetTextOrientation(PBTEXTORIENT_LEFT); 	
			else
			if(!strcmp("PBTEXTORIENT_OVER", szParameter))
				((ZGuiProgressbar*)pkWnd)->SetTextOrientation(PBTEXTORIENT_OVER); 	
			else
			if(!strcmp("PBTEXTORIENT_RIGHT", szParameter))
				((ZGuiProgressbar*)pkWnd)->SetTextOrientation(PBTEXTORIENT_RIGHT); 
			else
			if(!strcmp("PBTEXTORIENT_UNDER", szParameter))
				((ZGuiProgressbar*)pkWnd)->SetTextOrientation(PBTEXTORIENT_UNDER); 

			//Hide text
			if(!strcmp("PB_HIDETEXT", szParameter))
				((ZGuiProgressbar*)pkWnd)->m_bShowText = false; 
			
		}
		else
		if(eWndType == Textbox)
		{
			if(!strcmp("TOGGLE_MULTILINE", szParameter))
			{
				((ZGuiTextbox*)pkWnd)->ToggleMultiLine(true);

				((ZGuiTextbox*)pkWnd)->SetScrollbarSkin(
					g_pkGuiApp->GetSkin("DefSBrBkSkin"),
					g_pkGuiApp->GetSkin("DefSBrNSkin"),
					g_pkGuiApp->GetSkin("DefSBrFSkin"),
					g_pkGuiApp->GetSkin("DefSBrScrollUpSkin_u"),
					g_pkGuiApp->GetSkin("DefSBrScrollUpSkin_d"),
					g_pkGuiApp->GetSkin("DefSBrScrollDownSkin_u"),
					g_pkGuiApp->GetSkin("DefSBrScrollDownSkin_d"));
			}
		}
		else
		if(eWndType == Slider)
		{
			if(!strcmp("SCF_VERT", szParameter))
			{
				((ZGuiSlider*)pkWnd)->SetCtrlStyle(SCF_VERT);			
				
				Rect rc = pkWnd->GetScreenRect();
				pkWnd->Resize(rc.Width(),rc.Height(),true); 
			}
		}
		
	}

	return 1;
}

int GuiAppLua::CreateNewRadiobuttonGroupLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNumArgs < 1)
		return false;

	char szName[100];
	g_pkScript->GetArgString(pkLua, 0, szName);
	
	g_pkGuiApp->CreateNewRadiobuttonGroup(szName);

	return 1;
}
