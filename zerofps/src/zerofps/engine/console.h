#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include "input.h"
#include "engine_x.h"
#include <iostream>
#include <vector>
#include <deque>

using namespace std;

class ZeroFps;

//max lenght of an entered command
#define COMMAND_LENGHT 37

class ENGINE_API Console : public BasicConsole {
	private:
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
		void Execute(char* aText);
};



#endif







