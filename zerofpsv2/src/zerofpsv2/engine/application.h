#ifndef _ENGINE_APPLICATION_H_
#define _ENGINE_APPLICATION_H_

#include "engine_x.h"
#include "zerofps.h"
#include "../basic/zfvfs.h"

using namespace std;

class ZeroFps;
class Console;
class Render;
class Input;
class FileIo;
class ZFClient;

/// The application object.
class ENGINE_API Application : public ZFSubSystem {	
	protected:
		ZeroFps*					pkFps;
		Console*					pkConsole;			
		Render*					pkRender;
		Input*					pkInput;
		TextureManager*		pkTexMan;
		FileIo*					pkFile;
		ZFBasicFS*				pkBasicFS;
		Light*					pkLight;
		ObjectManager*			pkObjectMan;
		PropertyFactory*		pkPropertyFactory;
		ZFAudioSystem*			pkAudioSys;
		ZGui*						pkGui;
		ZGuiResourceManager*	pkGuiMan;
		ZFIni*					pkIni;
		PhysicsEngine*			pkPhysEngine;
		ZFScript*				pkScript;
		ZFVFileSystem*			pkZFVFileSystem;
		Physics_Engine*		pkPhysics_Engine;
		ZShader*					pkZShader;	
		
		
	public:
		static Application*	pkApp;												///< The one and Only App Instance.
		
		int m_iWidth,m_iHeight,m_iDepth;
		char* m_pTitle;																///< Windows Title.
		
		void SetEnginePointer(ZeroFps* pkZeroFps);							///< Set pointer to ZeroFps Engine.
					
		Application(char* pName,int iWidth,int iHeight,int iDepth);

		virtual void OnInit(void)=0;
		virtual void OnIdle(void)=0;
		virtual void OnHud(void)=0;
		virtual void OnSystem(void)=0;
		virtual void OnServerClientJoin(ZFClient* pkClient,int iConID)=0;
		virtual void OnServerClientPart(ZFClient* pkClient,int iConID)=0;

		virtual void OnServerStart(void)=0;
		virtual void OnClientStart(void)=0;
};


int ENGINE_API EngineStart(int iNrOfArgs, char* paArgs[]);

#ifdef _MAINAPPLICATION_
	#ifndef _MAINFUNKTION_
		#define _MAINFUNKTION_
		int main(int iNrOfArgs, char* paArgs[])
		{
			return EngineStart(iNrOfArgs, paArgs);
		}
	#endif	
#endif


#endif



