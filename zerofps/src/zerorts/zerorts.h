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

	public:
		ZeroRTS(char* aName,int iWidth,int iHeight,int iDepth);
		~ZeroRTS() {};
	 	void OnInit(void);
		void OnIdle(void);
		void OnHud(void);
		
		void Init();

		void OnServerStart(void);
		void OnClientStart(void);
		

};



#endif