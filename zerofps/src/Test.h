#ifndef _TEST_H_
#define _TEST_H_


#include <iostream>
#include "Application.h"
#include "ZeroFps.h"

class Test :public Application {
	


	public:
		Test(char* aName);
	 	void OnInit(void);
		void OnIdle(void);


};


#endif






