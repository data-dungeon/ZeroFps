#ifndef _TEST_H_
#define _TEST_H_

#define _MAINAPPLICATION_

#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "../zerofps/engine/application.h"

#include "normaldrawproperty.h"
#include "playercontrolproperty.h"
#include "ballobject.h"
#include "playerballobject.h"
#include "playerobject.h"
#include "bunnyobject.h"

class Test :public Application {
	HeightMap *test;
	HeightMap *test2;

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
	


	public:
		Test(char* aName,int iWidth,int iHeight,int iDepth);
		~Test() {};
	 	void OnInit(void);
		void OnIdle(void);
		void OnHud(void);

		void input();
};



#endif






