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

	Vector3 *m_kSpotpos;

	//Core		CoreTest;
	//MadInstans	kBitch;

	vector<Core>		akCoreModells;
	vector<MadInstans>	akModells;

	int		iCoreFrame;
	int		TestUpdateSpeedFulHack;

	public:
		Test(char* aName,int iWidth,int iHeight,int iDepth);
	 	void OnInit(void);
		void OnIdle(void);
		void OnHud(void);

		void input();

		void FH_SkapaObject(void);
		void FH_UpdateraObject(void);
		void FH_RitaObject(void);


};



#endif






