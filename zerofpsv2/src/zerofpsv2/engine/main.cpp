#include "main.h"



/**
	\todo Build a smile engine =).
*/
 
/**
	Start point for ZeroFps. 
	
	* Creates ZeroFps Engine object and all major Engine sub systems.

	* Call ZeroFps->Init that starts up engine.

	* Run ZeroFps->MainLoop that runs the engine until it shuts down.

	* delete the Engine that deletes all sub systems and shutdown it all.
*/

int ENGINE_API EngineStart(int iNrOfArgs, char* paArgs[])
{
	kEngine = new ZeroFps();
	
	if( kEngine->Init(iNrOfArgs,paArgs) ) 
	{
		kEngine->MainLoop();
	}

	delete kEngine;
	
	//MemMan::m_pkInstance->Print();

	// This leave console window on screen.
	exit(1);

//	return 0;
}








