#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include "engine_x.h"
#include <iostream>
#include <vector>

using namespace std;

class ZeroFps;

//max lenght of an entered command
#define COMMAND_LENGHT 37

class ENGINE_API Console : public ZFObject {
	private:
		ZeroFps* m_pkEngine;		
		CmdSystem* m_pkCmd;
		Render* m_pkRender;
		TextureManager* m_pkTexMan;
		
		SDL_Event m_kEvent;
		vector<char*> m_kText;
		char m_aCommand[TEXT_MAX_LENGHT];
		int m_iBufferSize;
		bool m_bShift;

		
		enum FuncId_e
			{
			FID_VERSION,
			FID_HELP
			};

		void RunCommand(int cmdid, const CmdArgument* kCommand);

	public:
		Console();
		
		void Print(const char* aText);
//		void Gemens(char* aTexT);
		
		void Update(void);
//		void Draw(void);
		void Execute(char* aText);
		void Printf(const char *fmt, ...);
};



#endif







