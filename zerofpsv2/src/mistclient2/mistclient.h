#ifndef _MISTCLIENT_H_
#define _MISTCLIENT_H_

#include "../zerofpsv2/engine/application.h"
#include "../zerofpsv2/gui/zgui.h"

class OptionsDlg;
class ActionMenu;
class InventoryDlg;
class EquipmentDlg;
class Camera;
class Entity;

typedef void (*msgScreenProg)(string, string, unsigned int msg, int numparms, void *params);

enum GUI_SCRIPT
{
	GSF_GAMEGUI,
	GSF_OPTION,
	GSF_CHARGEN,
	GSF_START,
};


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
			FID_KILLME,
		};


		//delay
		float			m_fDelayTime;

		//character and camera
		Camera*		m_pkCamera;						//main camera
		bool			m_bFrontView;				
		int			m_iCharacterID;				//current active character ID      
		int			m_iPickedEntityID;			// Selected entity for use.								
		bitset<6>	m_kCharacterControls;
		
		
		vector<string>	m_kEmotes;					//emote list
		
		
		//states
		bool			m_bShowMenulevel;
		bool			m_bQuickStart; 
		string		m_strQuickStartAddress;	
		bool			m_bGuiCapture;
		string		m_strLoginName, m_strLoginPW;
		
		vector<string>	m_kPlayerList;		//list of players since last playerlist update

		//gui
		map<GUI_SCRIPT, string> m_kGuiScrips;			
		vector<pair<string,string> > 	m_kServerList;
		map<string, msgScreenProg> 	m_kGuiMsgProcs;

		OptionsDlg* 	m_pkOptionsDlg;
		ActionMenu* 	m_pkActionDlg;
		InventoryDlg* 	m_pkInventoryDlg;
		EquipmentDlg* 	m_pkEquipmentDlg;

		bool ReadWriteServerList(bool bRead);

		void UpdateCharacter();
		void SendControlInfo();
		void Input();
		bool DelayCommand();
		
		
		//chat system
		void Say(string strMsg);
		void RegisterEmotes();
		
		//specifik network requests
		void RequestOpenInventory();
		void RequestPickup(int iEntityID,int iPosX = -1,int iPosY = -1);
		
		//general network sends
		void SendAction(int iEntityID,const string& strAction);
		void SendRequestContainer(int iContainerID);		
		void SendMoveItem(int iItemID,int iTarget,int iPosX,int iPosY,int iCount = -1);	
		void SendRequestPlayerList();
		void SendMessage(string strMsg,int iChannel,string strToWho);
		void SendRequestKillMe();
		void SendRequestOpenEqipment();			
		void SendTaunt(int iID);
		void SendTaunt(const string& strEmote);
		
			/*SendMoveItem hj�p.
				iItemID : alltid entity id't f� itemet i fr�a
				iTraget : target container entity ID , om -1 s�antas i spelarens inventory
				iPosX   : position i containern som f�em�et skall flyttas till, om -1 s�s�ts f�em�et p�en ledig plats
			*/
		
		
			
		// gui stuff
		void SetupGUI();
		void InitGUIFonts();
		void FindGUIScriptsByResSuffix();
		bool NameIPFromServerList(string& strName, string& strIP);
		void ToogleChatWnd(bool bOpen, bool bSetInputFocus=false);
		//void ResizeChatDlg(bool bBigger);      	
		void LoadInGameGui();
		void LoadStartScreenGui(bool bShowSplashImage);
		void SetGuiCapture(bool bSet, bool bMoveCursorToCenter=true);
		void PositionActionButtons();
		void CloseActiveContainer();
		void CloseActiveWindow(); // on ESC press
						
		//register
		void RegisterPropertys();
		void RegisterResources();
	  
		//draw
		void DrawCrossHair();
		
		//picking
		Entity* GetTargetObject();
		Vector3 Get3DMouseDir(bool bMouse);

				
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
		void RenderInterface(void);

		void RunCommand(int cmdid, const CmdArgument* kCommand);
				
		void OnClientStart(void);			
		void OnClientConnected();
		void OnNetworkMessage(NetPacket *PkNetMessage);
		void OnDisconnect(int iConnectionID);

		
      void AddRemoveServer(const char* szName, const char* szSeverIP, bool bAdd=true);
      void UpdateServerListbox();
		void AddStringToChatBox(string strMsg);
		

      friend bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params );
      friend void GuiMsgStartScreen( string strMainWnd, string strController, unsigned int msg, int numparms, void *params );
		friend void GuiMsgIngameScreen( string strMainWnd, string strController, unsigned int msg, int numparms, void *params );
		friend void GuiMsgOptionsDlg( string strMainWnd, string strController, unsigned int msg, int numparms, void *params );
		friend void GuiMsgActionDlg( string strMainWnd, string strController, unsigned int msg, int numparms, void *params );
		friend void GuiMsgInventoryDlg( string strMainWnd, string strController, unsigned int msg, int numparms, void *params );
		friend void GuiMsgEquipmentDlg( string strMainWnd, string strController, unsigned int msg, int numparms, void *params );
		
		friend class OptionsDlg;
		friend class ActionMenu;
		friend class InventoryDlg;
		friend class EquipmentDlg;
};



#endif











