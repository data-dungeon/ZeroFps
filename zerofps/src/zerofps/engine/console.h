#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include "input.h"
#include "engine_x.h"
#include <iostream>
#include <vector>
#include <deque>
#include <tolua.h>

using namespace std;

class Input;
class ZeroFps;

//max lenght of an entered command
#define COMMAND_LENGHT 128

class ENGINE_API Console : public BasicConsole {
	private:
		void FormatKey(int& r_iKey);
		ZeroFps* m_pkEngine;		
		CmdSystem* m_pkCmd;
		Render* m_pkRender;
		Input* m_pkInput;
		TextureManager* m_pkTexMan;
		
		SDL_Event m_kEvent;
		char m_aCommand[TEXT_MAX_LENGHT];
		int m_iBufferSize;
		bool m_bShift;

		unsigned int m_nStartLine;
		const unsigned int MAX_CMD_HISTRORY_LENGTH;
		unsigned int m_nLastCommand;
		deque<string> m_kCommandHistory;
		
	public:
		Console();
		
		void Update(void);
		bool Execute(char* aText);

		static int LuaSet(lua_State* pkLua) 
		{
			BasicConsole* var=(BasicConsole*) lua_touserdata(pkLua,2);
			BasicConsole* val=(BasicConsole*) lua_touserdata(pkLua,3);
			var=val;
			return 0;
		}

		static int LuaGet(lua_State* pkLua) 
		{
			BasicConsole* var=(BasicConsole*) lua_touserdata(pkLua,2);
			lua_pushusertag(pkLua, var, tolua_tag(pkLua, "BasicConsole"));
			return 1;
		}
};



#endif







