#ifndef _ZEROEDIT_H_
#define _ZEROEDIT_H_

#define _MAINAPPLICATION_

#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "../zerofps/engine/application.h"
#include <cstdlib>

//#include "normaldrawproperty.h"
//#include "playercontrolproperty.h"
//#include "ballobject.h"
//#include "playerballobject.h"
//#include "playerobject.h"
//#include "bunnyobject.h"

enum EDIT_MODES{
	TEXTURE,
	LOWER,
	RAISE,
	FLATTEN,
	ADDOBJECT
};

class ZeroEdit :public Application {
	private:
		enum FuncId_e
		{
			FID_LOADMAP,
			FID_LOADIMAGEMAP,			
			FID_SAVEMAP,
			FID_NEWMAP
		};	
	
		Camera *m_pkCamera;

		HeightMap *m_pkMap;
		
		int m_iMode;

		Vector3 m_kDrawPos;
		float m_fPointDistance;


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
		
		void CreateNew(int iSize);
};



#endif






