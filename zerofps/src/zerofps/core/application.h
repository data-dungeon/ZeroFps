#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "../base/base.pkg"
#include "../engine/engine.pkg"
#include "../render/render.pkg"

class ZeroFps;
class Console;

class Application {	
	protected:
		ZeroFps* pkFps;
		Console* pkConsole;			
	
		
	public:
		static Application* pkApp;
		
		int m_iWidth,m_iHeight,m_iDepth;
		char* m_pTitle;
		
		
		void SetEnginePointer(ZeroFps* pkZeroFps);
		Application(char* pName,int iWidth,int iHeight,int iDepth);
		
		virtual void OnInit(void)=0;
		virtual void OnIdle(void)=0;


};




#endif



