#ifndef _TEST_H_
#define _TEST_H_

#define _MAINAPPLICATION_

#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "../zerofps/engine/application.h"
#include "../zerofps/render/pmddraw.h"

class Test :public Application {
	HeightMap *test;
	HeightMap *test2;

	Vector4 light_position;
	Vector4 white_light;
	Vector4 lmodel_ambient;

	int fpsupdate;
	char fps[20];
	float speed;
	int m_iGrass;
	int m_iGrassVolyme;

	PmdDraw testa;

	public:
		Test(char* aName,int iWidth,int iHeight,int iDepth);
	 	void OnInit(void);
		void OnIdle(void);
		void OnHud(void);


		void input();
};



#endif






