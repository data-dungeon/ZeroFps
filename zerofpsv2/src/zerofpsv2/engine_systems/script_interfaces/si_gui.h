// scriptinterfaces.h: interface for the ScriptInterfaces class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SI_GUI_H
#define _SI_GUI_H

class ZGuiApp;
class ZFScriptSystem;

#include "../engine_systems_x.h"

namespace GuiAppLua
{
	void ENGINE_SYSTEMS_API Init(ZGuiApp* pkGuiApp, ZFScriptSystem* pkScript);

	 ZGuiApp* g_pkGuiApp;
	 ZFScriptSystem* g_pkScript;

	int ENGINE_SYSTEMS_API CreateWndLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API AddTabPageLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API CloseWndLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API IsWndVisibleLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API ChangeSkinLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API GetScreenWidthLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API GetScreenHeightLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API AddListboxItemLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API ClearListboxLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API GetWndLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API SetTextInt(lua_State* pkLua);
	int ENGINE_SYSTEMS_API SetTextStringLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API AddTreeItemLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API ShowWndLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API SetMoveAreaLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API SetFontLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API ChangeWndParameterLua(lua_State* pkLua);	
	int ENGINE_SYSTEMS_API CreateNewRadiobuttonGroupLua(lua_State* pkLua);
};

#endif // #ifndef _SI_GUI_H
