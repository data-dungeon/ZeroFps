#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <iostream>
#include <vector>
#include "CmdSystem.h"
#include "Primitives.h"
#include "TextureManager.h"
#include "Input.h"

class ZeroFps;

class Console {
	private:
//		ZeroFps* m_pkEngine;		
		CmdSystem* m_pkCmd;
		Primitives* m_pkPrims;
		TextureManager* m_pkTexMan;
		Input* m_pkInput;
		
		SDL_Event m_kEvent;
		vector<char*> m_kText;
		char m_aCommand[TEXT_MAX_LENGHT];
		int m_iBufferSize;

	public:
		Console(ZeroFps* pkEngine);
		
		void Print(char* aText);
		
		void Update(void);
		void Draw(void);
};



#endif





