#ifndef _ENGINE_APPLICATION_H_
#define _ENGINE_APPLICATION_H_

#include "engine_x.h"
#include "zerofps.h"
#include "../basic/zfvfs.h"
#include "inputhandle.h"

using namespace std;

class ZeroFps;
class Console;
class Render;
class Input;
class FileIo;
class ZFClient;

/** \brief	The application object.
	 \ingroup Engine
*/
class ENGINE_API Application : public ZFSubSystem {
	protected:
		ZFBasicFS*				m_pkBasicFS;
		ZFVFileSystem*			m_pkZFVFileSystem;
		TextureManager*		m_pkTexMan;
		Input*					m_pkInput;
		PropertyFactory*		m_pkPropertyFactory;
		Light*					m_pkLight;
		Render*					m_pkRender;
		Console*					m_pkConsole;
		EntityManager*			m_pkObjectMan;
		ZFAudioSystem*			m_pkAudioSys;
		ZGuiResourceManager*	m_pkGuiMan;
		ZGui*						m_pkGui;
		ZFIni*					m_pkIni;
		ZFResourceDB*			m_pkResourceDB;
		ZFScriptSystem*		m_pkScript;
		FileIo*					m_pkFile;
		ZShadow*					m_pkZShadow;
		ZeroFps*					m_pkFps;
		ZShaderSystem*			m_pkZShaderSystem;
		
	public:
		static Application*	m_pkApp;												///< The one and Only App Instance.
		
		int m_iWidth,m_iHeight,m_iDepth;
		char* m_pTitle;																///< Windows Title.
		
		InputHandle*	m_pkInputHandle;
		
		void SetEnginePointer(ZeroFps* pkZeroFps);							///< Set pointer to ZeroFps Engine.
		void SetTitle(string strTitle);		
		
		Application(char* pName,int iWidth,int iHeight,int iDepth);

		virtual void OnInit(void)=0;
		virtual void OnIdle(void)=0;
		virtual void OnHud(void)=0;
		virtual void OnSystem(void)=0;
		virtual bool OnPreConnect(IPaddress kRemoteIp, char* szLogin, char* szPass) = 0;
		virtual void OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass)=0;
		virtual void OnServerClientPart(ZFClient* pkClient,int iConID)=0;

		virtual void RenderInterface(void)=0;


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



