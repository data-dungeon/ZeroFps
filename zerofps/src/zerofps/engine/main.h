#ifndef _MAIN_H_
#define _MAIN_H_

#include <iostream>
#include "../engine/zerofps.h"

	
	
ZeroFps* kEngine;
	
#ifndef _MAINFUNKTION_
#define _MAINFUNKTION_
int main(int iNrOfArgs, char* paArgs[])
{
	return EngineStart(iNrOfArgs, paArgs);
}
#endif
	

#endif









