#ifndef _MISTCLIENT_H_
#define _MISTCLIENT_H_

#include <iostream>
#include <cmath>
#include <list>
#include "../zerofpsv2/engine/application.h"
#include "../zerofpsv2/gui/zgui.h"

#include "../mcommon/si_mistland.h"
#include "../mcommon/p_enviroment.h"


/**	\brief	Da MistClient
		\ingroup MistClient
*/
class MistClient :public Application, public ZGuiApp {
	private:
		//console functions
		enum FuncId_e
		{

		};

		Camera*				m_pkCamera;
		int					m_iCharacterID;
      
		bool              m_bSkipLoginScreen;

      vector<pair<string,string> > m_kServerList;
      bool ReadWriteServerList(bool bRead);

	public:
		//application virtuals
		MistClient(char* aName,int iWidth,int iHeight,int iDepth);
		
		bool StartUp();
		bool ShutDown();
		bool IsValid()			{return true;}
		void OnInit(void);
		void OnIdle(void);
		void OnHud(void);
		void OnSystem();

				
		void OnClientStart(void);
				
		//on client join, server runs this
		//bool OnPreConnect(IPaddress kRemoteIp, char* szLogin, char* szPass, bool bIsEditor) { return true; }
		//void OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass, bool bIsEditor);
		//void OnServerClientPart(ZFClient* pkClient,int iConID);
		void OnClientConnected();
		void OnNetworkMessage(NetPacket *PkNetMessage);
		
		void RegisterPropertys();
		void RegisterResources();

      void AddRemoveServer(const char* szName, const char* szSeverIP, bool bAdd=true);
      void UpdateServerListbox();

      friend bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params );
		
};



#endif











