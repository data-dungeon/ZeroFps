#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <iostream>
#include <vector>
#include "CmdSystem.h"
#include "Primitives.h"
#include "TextureManager.h"

class ZeroFps;

class Console {
	private:
//		ZeroFps* m_pkEngine;		
		CmdSystem* m_pkCmd;
		Primitives* m_pkPrims;
		TextureManager* m_pkTexMan;

		vector<char[TEXT_MAX_LENGHT]> kText;

	public:
//		Console(CmdSystem* pkCmd,Primitives* pkPrims);
		Console(ZeroFps* pkEngine);
		
		
		void Update(void);
		void Draw(void);
};



#endif





