#include "main.h"

int ENGINE_API EngineStart(int iNrOfArgs, char* paArgs[])
{
	kEngine =new ZeroFps();
	
	kEngine->Init(iNrOfArgs,paArgs);
	
	return 0;
}








