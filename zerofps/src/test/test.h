#ifndef _TEST_H_
#define _TEST_H_

#define _MAINAPPLICATION_

#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "../zerofps/engine/application.h"
//#include "../zerofps/render/glguirender.h"

#include "normaldrawproperty.h"
#include "playercontrolproperty.h"
#include "ballobject.h"
#include "playerballobject.h"
#include "playerobject.h"
#include "bunnyobject.h"

class Test :public Application {
	private:
		bool InitUI();
		HeightMap *m_pkMap;
		HeightMapObject *m_pkHeightMapObject;

		Vector4 light_position;
		Vector4 white_light;
		Vector4 lmodel_ambient;

		float speed;
		int m_iGrass;
		int m_iGrassVolyme;

		Vector3 *m_kSpotpos;
	
		Camera *cam1;
		Camera *cam2;
	
		PlayerObject *m_pkPlayer;

		string m_kMapBaseDir;

		//ZGui* m_pkGui;
//		GLGuiRender* m_pkGuiRender; // gui render*/
		void RegisterPropertys();	
		void CreateZones();
		void input();		
		
		bool LoadLevel(const char* acFile);				
		void Clear();		
		void CreateNew(int iSize);
		
	public:
//		Core	m_kTestCore;

		enum FuncId_e
		{
			FID_LOADMAP,
			FID_SAVEMAP,
			FID_LOAD,
		};
	
		
	
		Test(char* aName,int iWidth,int iHeight,int iDepth);
		~Test() {};
	 	void OnInit(void);
		void OnIdle(void);
		void OnHud(void);


		void OnServerStart(void);
		void OnClientStart(void);
		


		void RunCommand(int cmdid, const CmdArgument* kCommand);
};



#endif






