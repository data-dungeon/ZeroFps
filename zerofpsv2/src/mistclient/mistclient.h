#ifndef _MISTCLIENT_H_
#define _MISTCLIENT_H_

#ifndef _DONT_MAIN
	#define _MAINAPPLICATION_
	#define _DONT_MAIN
#endif

#include <iostream>
#include <cmath>
#include <list>
#include "../zerofpsv2/engine/application.h"
#include "../zerofpsv2/engine_systems/mad/mad_modell.h"

#include "../zerofpsv2/engine_systems/propertys/p_camera.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/gui/zgui.h"

#include "../mcommon/p_event.h"
#include "../mcommon/p_ml.h"
#include "../mcommon/p_charstats.h"
#include "../mcommon/p_item.h"
#include "../mcommon/p_container.h"
#include "../mcommon/p_spell.h"
#include "../mcommon/si_mistland.h"
#include "../mcommon/p_serverinfo.h"
#include "../mcommon/p_clientcontrol.h"
#include "../mcommon/p_enviroment.h"

#include "inventorydlg.h"
#include "containerdlg.h"
#include "quickboard.h"
#include "spelldlg.h"
#include "skilldlg.h"
#include "statsdlg.h"
#include "henchman_button.h"

/**	\brief	Da MistClient
		\ingroup MistClient
*/
class MistClient :public Application, public ZGuiApp {
	private:
		//console functions
		enum FuncId_e
		{
			FID_LOAD,
			FID_UNLOAD,
			FID_MASSSPAWN,

			FID_GETCHAR,
			FID_SELECTCHAR,
			FID_DELETECHAR,
			FID_PLAY,

			FID_SERVERORDER
		};

		//selected character
		int					m_iActiveCaracterObjectID;
		Entity* 				m_pkActiveCharacter;

		//clients own little cute object
		int					m_iSelfObjectID;				// Network ID that i use to send data to server.
		Entity*				m_pkClientObject;
		Entity*				m_pkTargetObject, *m_pkTargetObject2;
		P_ClientControl*	m_pkClientControlP;
		P_ServerInfo*		m_pkServerInfo;
		
		Camera*				m_pkCamera;
		Entity*				m_pkTestobj;
		P_Camera*			m_pkCamProp;		
		float					m_fAngle;
		float					m_fPAngle;		
		float					m_fDistance;		
		float 				m_fClickDelay;
		
		float					m_fMaxCamDistance;
		float					m_fMinCamDistance;		
		
		//zone picking 
		Vector3				m_kTargetPos;
		int					m_iTargetZoneObject;
		int					m_iTargetFace;			
		
		//gui
		bool					m_bActionMenuIsOpen;
		int					m_iMouseMode; // if true, move mouse, else move camera	
		const unsigned int MAX_NUM_ACTION_BUTTONS;
		map<ZGuiButton*, string> m_kActionBnTranslator;		
		
		vector<HenchmanButton*> m_vkHenchmanIcons;
		HenchmanButton* m_pkSelHenchmanIcon;
		
		InventoryDlg*	  m_pkInventDlg;
		SpellDlg*		  m_pkSpellDlg;
		QuickBoard*		  m_pkQuickBoard;
		SkillDlg*		  m_pkSkillDlg;
		StatsDlg*		  m_pkStatsDlg;
		ContainerDlg*	  m_pkContainerDlg;
		
		
		//picking funkctions
		Vector3	Get3DMousePos(bool m_bMouse);		
		Entity*	GetTargetObject();	
		bool PickZones();

		void CreateGuiInterface();
		void UpdateObjectList(PlayerInfo* pkPlayerInfo);		
		void UpdateCullObjects();
		void UpdateManaAndHealthBar(CharacterStats* pkCharacterProperty);
		void DrawCrossHair();

		// move camera or mouse
		enum eMOUSE_MODES { eMOUSE_MODE, eCAMERA_MODE, eACTION_MODE };

		// change between mousemode and camera mode
		void ChangeMode ( eMOUSE_MODES eMode );
		
	public:
		void OnKeyPress(int iKey, ZGuiWnd* pkWnd);
		void CloseActionMenu();
		bool OpenActionMenu(int x, int y);
		void OnClientInputSend(char* szText);
		void OnSelectCB(int ListBoxID, int iItemIndex, ZGuiWnd* pkWnd);
		void PrintInfoBox(const char* strText);
		// void PickUp(); // zerom: pickup happens on server
		void OnScroll(int iID, int iPos, ZGuiWnd *pkMain);
		void OnMouseMove(int x, int y, bool bMouseDown, ZGuiWnd* pkWnd);
		void OnClick(int x, int y, bool bMouseDown, bool bLeftButton, ZGuiWnd* pkMain);
		void OnDClick(int x, int y, bool bLeftButton, ZGuiWnd* pkMainWnd);
		void OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd* pkMainWnd);
		
		MistClient(char* aName,int iWidth,int iHeight,int iDepth);
	 	
	 	void OnInit(void);
		void OnIdle(void);
		void OnHud(void);
		void OnSystem();
				
		void RunCommand(int cmdid, const CmdArgument* kCommand);		
		void Init();
		void RegisterPropertys();
		void RegisterResources();
		void Input();
		void OnServerStart(void);
		void OnClientStart(void);
		
//		void SetActiveCaracter(int iCaracter);
		void SetActiveCaracter(bool bEnabled);
		
		void SendOrder(string strOrder);

		//init client
		void ClientInit();
		
		//on client join, server runs this
		bool OnPreConnect(IPaddress kRemoteIp, char* szLogin, char* szPass, bool bIsEditor) { return true; }
		void OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass, bool bIsEditor);
		void OnServerClientPart(ZFClient* pkClient,int iConID);
		void RenderInterface(void) { }
		void OnNetworkMessage(NetPacket *PkNetMessage) {}

		bool StartUp();
		bool ShutDown();
		bool IsValid();
};



#endif











