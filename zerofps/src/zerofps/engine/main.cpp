#include "main.h"

int ENGINE_API EngineStart(int iNrOfArgs, char* paArgs[])
{
	kEngine =new ZeroFps();
	
	kEngine->Init(iNrOfArgs,paArgs);
	
	delete kEngine;
	
	// This leave console window on screen.
	exit(1);

	return 0;
}








