#ifndef _ZERORTS_H_
#define _ZERORTS_H_

#define _MAINAPPLICATION_

#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "../zerofps/engine/application.h"

#include "../common/common.pkg"



class ZeroRTS :public Application {
	private:
		Camera *m_pkCamera;;


		// actions
		int m_iActionCamLeft;
		int m_iActionCamRight;
		int m_iActionCamUp;
		int m_iActionCamDown;

		//console funktions
		enum FuncId_e
		{
			FID_LOAD,
			FID_UNLOAD,
		};
		
		
	public:
		ZeroRTS(char* aName,int iWidth,int iHeight,int iDepth);
		~ZeroRTS() {};
	 	void OnInit(void);
		void OnIdle(void);
		void OnHud(void);
		
		void Init();
		void RegisterActions();
		
		void Input();

		void OnServerStart(void);
		void OnClientStart(void);
		
		void RunCommand(int cmdid, const CmdArgument* kCommand);
};



#endif











