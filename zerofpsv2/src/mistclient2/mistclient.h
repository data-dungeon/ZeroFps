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

typedef void (*msgScreenProg)(string, string, unsigned int msg, int numparms, void *params);

/**	\brief	Da MistClient
		\ingroup MistClient
*/
class MistClient :public Application, public ZGuiApp {
	private:
		//console functions
		enum FuncId_e
		{
			FID_SAY,
			FID_PLAYERLIST,
		};

		Camera*		m_pkCamera;				//main camera
		int			m_iCharacterID;		//current active character ID
      		
		bitset<6>	m_kCharacterControls;
		
		bool			m_bSkipLoginScreen;
		string     m_strLoginName, m_strLoginPW;
		
		vector<string>	m_kPlayerList;		//list of players since last playerlist update
		

		vector<pair<string,string> > m_kServerList;
      bool ReadWriteServerList(bool bRead);

		void UpdateCharacter();
		void SendControlInfo();
		void Input();
		
		void Say(string strMsg);
		void RequestPlayerList();

		bool NameIPFromServerList(string& strName, string& strIP);
		void ToogleChatWnd(bool bOpen, bool bSetInputFocus=false);
		void ResizeChatDlg(bool bBigger);      	
		void LoadInGameGui();
		void SetupGUI();
		
				
		void RegisterPropertys();
		void RegisterResources();
	  
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

		void RunCommand(int cmdid, const CmdArgument* kCommand);
				
		void OnClientStart(void);			
		void OnClientConnected();
		void OnNetworkMessage(NetPacket *PkNetMessage);

		
      void AddRemoveServer(const char* szName, const char* szSeverIP, bool bAdd=true);
      void UpdateServerListbox();
		void AddStringToChatBox(string strMsg);
		

      friend bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params );
      friend void GuiMsgStartScreen( string strMainWnd, string strController, unsigned int msg, int numparms, void *params );
		friend void GuiMsgIngameScreen( string strMainWnd, string strController, unsigned int msg, int numparms, void *params );

		map<string, msgScreenProg> m_kGuiMsgProcs;
};



#endif











