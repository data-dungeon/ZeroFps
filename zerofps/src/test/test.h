#ifndef _TEST_H_
#define _TEST_H_

#define _MAINAPPLICATION_

#include <iostream>
#include <cmath>
#include "../zerofps/engine/application.h"


class Test :public Application {
	


	public:
		Test(char* aName,int iWidth,int iHeight,int iDepth);
	 	void OnInit(void);
		void OnIdle(void);


};


#endif






