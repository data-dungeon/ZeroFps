#ifndef _ZEROEDIT_H_
#define _ZEROEDIT_H_

#define _MAINAPPLICATION_

#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "../zerofps/engine/application.h"

//#include "normaldrawproperty.h"
//#include "playercontrolproperty.h"
//#include "ballobject.h"
//#include "playerballobject.h"
//#include "playerobject.h"
//#include "bunnyobject.h"

class ZeroEdit :public Application {
	private:
		Camera *m_pkCamera;

		HeightMap *m_pkMap;

	public:
		ZeroEdit(char* aName,int iWidth,int iHeight,int iDepth);
		~ZeroEdit() {};
	 	void OnInit(void);
		void OnIdle(void);
		void OnHud(void);

		void OnServerStart(void);
		void OnClientStart(void);
		
		void Input();
		
		void RunCommand(int cmdid, const CmdArgument* kCommand);
};



#endif






