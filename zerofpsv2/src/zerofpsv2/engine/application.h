#ifndef _ENGINE_APPLICATION_H_
#define _ENGINE_APPLICATION_H_

#include "engine_x.h"

#include "../basic/zfsubsystem.h"
#include "network.h"

#include "../gui/zgui.h"			//evil gui needs this


using namespace std;

class ZeroFps;
class Console;
class Render;
class Input;
class FileIo;
class ZFClient; 
class ZShadow;
class Tcs;
class ZGuiResourceManager;
class ZGui;
class ZFAudioSystem;
class ZFBasicFS;
class ZFVFileSystem;
class TextureManager;
class PropertyFactory;
class Console;
class ZGui;
class FileIo;
class Light;
class EntityManager;
class Render;
class ZFIni;
class ZFResourceDB;
class ZShadow;
class ZFScriptSystem;
class ZShaderSystem;
class InputHandle;

//class IPaddress;
class NetPacket;

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
		EntityManager*			m_pkEntityManager;
		ZFAudioSystem*			m_pkAudioSys;
		ZGuiResourceManager*	m_pkGuiMan;
		ZGui*						m_pkGui;
		ZFIni*					m_pkIni;
		ZFResourceDB*			m_pkResourceDB;
		ZFScriptSystem*		m_pkScript;
		FileIo*					m_pkFile;
		ZShadow*					m_pkZShadow;
		ZeroFps*					m_pkZeroFps;
		ZShaderSystem*			m_pkZShaderSystem;
		Tcs*						m_pkTcs;
		
	public:
		static Application*	m_pkApp;												///< The one and Only App Instance.
		
		int				m_iWidth,m_iHeight,m_iDepth;
		char*				m_pTitle;																///< Windows Title.		
		InputHandle*	m_pkInputHandle;
		bool				m_bIsEditor;

		void SetEnginePointer(ZeroFps* pkZeroFps);							///< Set pointer to ZeroFps Engine.
		void SetTitle(string strTitle);		
		
		Application(char* pName,int iWidth,int iHeight,int iDepth);

		virtual void OnInit(void)		{};
		virtual void OnIdle(void)		{};
		virtual void OnHud(void)		{};
		virtual void OnSystem(void)	{};
		virtual void RenderInterface(void) {};		
		
		virtual bool OnPreConnect(IPaddress kRemoteIp, char* szLogin, char* szPass, bool bIsEditor, string& strWhy) {return true;};
		virtual void OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass, bool bIsEditor)	{};
		virtual void OnServerClientPart(ZFClient* pkClient,int iConID) {};
		virtual void OnClientConnected() {};
		virtual void OnDisconnect(int iConnectionID) {};
		virtual void OnServerStart(void) {};
		virtual void OnClientStart(void)	{};
		virtual void OnNetworkMessage(NetPacket *PkNetMessage) {};

		void SendAppMessage(NetPacket* pkNet);
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



