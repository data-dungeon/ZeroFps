// scriptinterfaces.h: interface for the ScriptInterfaces class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SCRIPTINTERFACES_H
#define _SCRIPTINTERFACES_H

#include "../zerofpsv2/script/zfscript.h"

namespace GuiAppLua
{
	int CreateWndLua(lua_State* pkLua);
	int AddTabPageLua(lua_State* pkLua);
	int CloseWndLua(lua_State* pkLua);
	int IsWndVisibleLua(lua_State* pkLua);
	int ChangeSkinLua(lua_State* pkLua);
	int GetScreenWidthLua(lua_State* pkLua);
	int GetScreenHeightLua(lua_State* pkLua);
};

#endif // #ifndef _SCRIPTINTERFACES_H
