#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "engine_x.h"
#include "../basic/basic.pkg"
#include "../engine/engine.pkg"
#include "../render/render.pkg"

using namespace std;

class ZeroFps;
class Console;
class Primitives;

class ENGINE_API Application {	
	protected:
		ZeroFps* pkFps;
		Console* pkConsole;			
		Primitives* pkPrims;
		
	public:
		static Application* pkApp;
		
		int m_iWidth,m_iHeight,m_iDepth;
		char* m_pTitle;												//title text
		
		
		void SetEnginePointer(ZeroFps* pkZeroFps);
		Application(char* pName,int iWidth,int iHeight,int iDepth);
		
		virtual void OnInit(void)=0;
		virtual void OnIdle(void)=0;


};


int ENGINE_API EngineStart(int iNrOfArgs, char* paArgs[]);



#endif



