#ifndef _MISTCLIENT_H_
#define _MISTCLIENT_H_

#include <iostream>
#include <cmath>
#include <list>
#include "../zerofpsv2/engine/application.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../zerofpsv2/basic/zguifont.h"

#include "../mcommon/si_mistland.h"
#include "../mcommon/p_enviroment.h"
#include "../mcommon/p_charactercontrol.h"

/**	\brief	Da MistClient
		\ingroup MistClient
*/
class MistClient :public Application, public ZGuiApp {
	private:
		//console functions
		enum FuncId_e
		{
			FID_SAY,
		};

		Camera*		m_pkCamera;				//main camera
		int			m_iCharacterID;		//current active character ID
      
		
		bitset<6>	m_kCharacterControls;
		
		bool			m_bSkipLoginScreen;
      string     m_strLoginName, m_strLoginPW;

      vector<pair<string,string> > m_kServerList;
      bool ReadWriteServerList(bool bRead);

		void UpdateCharacter();
		void SendControlInfo();
		void Input();
		
		void Say(string strMsg);

      bool NameIPFromServerList(string& strName, string& strIP);
	  void ToogleChatWnd(bool bSetInputFocus=false);
     void ResizeChatDlg(bool bBigger); 
     	
	  void LoadInGameGui();
	  
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
		void OnClientConnected();
		void OnNetworkMessage(NetPacket *PkNetMessage);
		
		void RegisterPropertys();
		void RegisterResources();

      void AddRemoveServer(const char* szName, const char* szSeverIP, bool bAdd=true);
      void UpdateServerListbox();
	  void AddStringToChatBox(string strMsg);

		void RunCommand(int cmdid, const CmdArgument* kCommand);
		
      friend bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params );
		
};



#endif











