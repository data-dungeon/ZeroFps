#ifndef _TEST_H_
#define _TEST_H_

#define _MAINAPPLICATION_

#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "../zerofps/engine/application.h"


class Test :public Application {
	HeightMap *test;

	GLfloat light_position[];
	GLfloat white_light[] ;
	GLfloat lmodel_ambient[];


	public:
		Test(char* aName,int iWidth,int iHeight,int iDepth);
	 	void OnInit(void);
		void OnIdle(void);


		void input();
};


#endif






