#ifndef _ENGINE_APPLICATION_H_
#define _ENGINE_APPLICATION_H_

#include "engine_x.h"
#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include "../engine/engine.pkg"
#include "../gui/gui.pkg"

using namespace std;

class ZeroFps;
class Console;
class Render;
class Input;
class FileIo;

class ENGINE_API Application : public ZFObject {	
	protected:
		ZeroFps* pkFps;
		Console* pkConsole;			
		Render* pkRender;
		Input* pkInput;
		TextureManager* pkTexMan;
		FileIo* pkFile;
		ZFBasicFS* pkBasicFS;
		Light* pkLight;
		ObjectManager* pkObjectMan;
		PropertyFactory* pkPropertyFactory;
		CollisionManager* pkCollisionMan;
		OpenAlSystem* pkAlSys;
		ZGui* pkGui;
		ZGuiResourceManager* pkGuiMan;
		ZFIni* pkIni;
		LevelManager* pkLevelMan;
		PhysicsEngine* pkPhysEngine;
		EngineScriptInterface* pkEngineScriptInterface;
		
	public:
		static Application* pkApp;
		
		int m_iWidth,m_iHeight,m_iDepth;
		char* m_pTitle;												//title text
		
		void SetEnginePointer(ZeroFps* pkZeroFps);
		Application(char* pName,int iWidth,int iHeight,int iDepth);

		virtual void OnInit(void)=0;
		virtual void OnIdle(void)=0;
		virtual void OnHud(void)=0;

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



