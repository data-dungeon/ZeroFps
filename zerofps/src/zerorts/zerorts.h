#ifndef _ZERORTS_H_
#define _ZERORTS_H_

#ifndef _DONT_MAIN
	#define _MAINAPPLICATION_
	#define _DONT_MAIN
#endif

#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "../zerofps/engine/application.h"

#include "../common/common.pkg"

class UserPanel;

struct PickInfo
{
	Object* pkObject;
	HM_vert* pkVert;
	
	Vector3 kHitPos;
};


class ZeroRTS :public Application {
	private:
		Camera*		m_pkCamera;
		HeightMap*	m_pkMap;

		// actions
		int m_iActionCamLeft;
		int m_iActionCamRight;
		int m_iActionCamUp;
		int m_iActionCamDown;
		int m_iActionSelect;
		int m_iActionScroll;		

		

		//console funktions
		enum FuncId_e
		{
			FID_LOAD,
			FID_UNLOAD,
		};

		UserPanel* m_pkUserPanel;
		
		friend class UserPanel;
		
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
		
		Vector3 Get3DMousePos();		
		Object* PickObject();
		HM_vert* PickMap(Vector3& kHitPos);
		PickInfo Pick();
		
		void RunCommand(int cmdid, const CmdArgument* kCommand);
		
		void SetCamPos(Vector3 kPos);
		Vector3 GetCamPos();
		void MoveCam(Vector3 kVel);
};



#endif











