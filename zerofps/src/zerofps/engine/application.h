#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "engine_x.h"
#include "../basic/basic.pkg"
#include "../engine/engine.pkg"
#include "../render/render.pkg"

using namespace std;

class ZeroFps;
class Console;
class Render;

class ENGINE_API Application {	
	protected:
		ZeroFps* pkFps;
		Console* pkConsole;			
		Render* pkRender;
		
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

#ifdef _MAINAPPLICATION_
	#ifndef _MAINFUNKTION_
		#define _MAINFUNKTION_
		int main(int iNrOfArgs, char* paArgs[])
		{
			return EngineStart(iNrOfArgs, paArgs);
		}
	#endif	
#endif


#endif



