#ifndef _APPLICATION_H_
#define _APPLICATION_H_



class ZeroFps;

class Application {	
	protected:
		ZeroFps* pkFps;
		

		
	public:
		static Application* pkApp;
		
		int m_iWidth,m_iHeight,m_iDepth;
		char* m_pTitle;
		
		
		inline void SetEnginePointer(ZeroFps* pkZeroFps) {pkFps=pkZeroFps;};
		Application(char* pName,int iWidth,int iHeight,int iDepth);
		
		virtual void OnInit(void)=0;
		virtual void OnIdle(void)=0;


};




#endif



