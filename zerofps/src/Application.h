#ifndef _APPLICATION_H_
#define _APPLICATION_H_



class ZeroFps;

class Application {	
	protected:
		ZeroFps* pkFps;
		

		
	public:
		static Application* pkApp;
		
		inline void SetEnginePointer(ZeroFps* pkZeroFps) {pkFps=pkZeroFps;};
		Application(char* aName);
		
		virtual void OnInit(void)=0;
		virtual void OnIdle(void)=0;


};




#endif



