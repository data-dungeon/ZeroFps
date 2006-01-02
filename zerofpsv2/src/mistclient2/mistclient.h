#ifndef _MISTCLIENT_H_
#define _MISTCLIENT_H_

#include "../zerofpsv2/engine/application.h"
#include "../zerofpsv2/gui/zgui.h"
#include "pointtext.h"

class OptionsDlg;
class ActionMenu;
class InventoryDlg;
class EquipmentDlg;
class SkillBar;
class Camera;
class Entity;

typedef void (*msgScreenProg)(string, string, unsigned int msg, int numparms, void *params);

struct SKILL_TREE_INFO
{
	string strName;
	string strParent;
	string strScreenName;
	string strIcon;
};

enum GUI_SCRIPT
{
	GSF_GAMEGUI,
	GSF_OPTION,
	GSF_CHARGEN,
	GSF_START,
	GSF_ITEMINFO,
};

enum HUD_EFFECTS
{
	HUD_WATER,
	HUD_DEAD,
	HUD_FOG,

};


const int MAX_NUM_BUFF_ICONS = 32;


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
			FID_MSREFRESH,
			FID_SETSKILLBAR,
			FID_DMC,
			FID_PCMD,
		};

		//music
		string		m_strMenuMusic;
		
		ConVar		m_kbBloom;
		
		//point text handler
		PointText*	m_pkPointText;
		
		//delay
		float			m_fDelayTime;

		//character and camera
		Camera*		m_pkCamera;						//main camera
		bool			m_bFrontView;				
		int			m_iCharacterID;				//current active character ID      
		int			m_iPickedEntityID;			// Selected entity for use.								
		bitset<6>	m_kCharacterControls;
		bool			m_bDead;
		bool			m_bCombatMode;
		bool			m_bInWater;

		//game
		vector<string>	m_kEmotes;					//emote list
		int				m_iTargetID;
		
		Entity*		m_pkMakerEntity;
		
		//states
		ConVar		m_kbShowLagMeeter;
		ConVar		m_kbTargetRotate;
		ConVar		m_kbShowMenulevel;
		ConVar		m_kbQuickStart; 
		ConVar		m_kstrQuickStartAddress;	
		bool			m_bGuiCapture;
		ConVar		m_kstrLoginName;
		ConVar		m_kstrLoginPW;
		bool			m_bLoginKeepAlive;			// If we are in login menu we send to keep connection open.
		float			m_fPingDelay;

		vector<string>	m_kPlayerList;		//list of players since last playerlist update

		//gui
		map<GUI_SCRIPT, string> m_kGuiScrips;			
		vector<pair<string,string> > 	m_kServerList;
		map<string, msgScreenProg> 	m_kGuiMsgProcs;

		OptionsDlg* 	m_pkOptionsDlg;
		ActionMenu* 	m_pkActionDlg;
		InventoryDlg* 	m_pkInventoryDlg;
		EquipmentDlg* 	m_pkEquipmentDlg;
		SkillBar*		m_pkSkillBar;


		struct BUFF_ICON_INFO
		{
			string m_strName;
			string m_strIcon;
			float  m_fTimeout;
			float  m_fTimeLeft;
			float  m_fStartTime;
			char   m_cType;	
			ZGuiLabel* m_pkWnd;
			ZGuiProgressbar* m_pkProgressBar;
		};

		void InitBuffWnd();
		void RebuildBuffIconList(vector<BUFF_ICON_INFO>* kList);
		void UpdateBuffIconList();
		
		int m_iNumBuffIcons;
		BUFF_ICON_INFO m_kBuffIcons[MAX_NUM_BUFF_ICONS];

		bool ReadWriteServerList(bool bRead);

		void UpdateCharacter();
		void SendControlInfo();
		void Input();
		bool DelayCommand();
		
		
		//chat system
		void Say(string strMsg);
		void RegisterEmotes();
		
		//network help fucntions
		void RequestOpenInventory();
		void RequestPickup(int iEntityID,int iPosX = -1,int iPosY = -1);		
		
		//Network sends		
		void SendSkillInc(const string& strSkill);
		void SendRequestSkillInfo(const string& strSkil);
		void SendRequestSkillTree();
		void SendUseItem(int iItemID);
		void SendSetDefaultAttack(const string& strSkill);		
		void SendAddSkillToQueue(const string& strSkill,int iTargetID);		
		void SendAddStatPoint(int iStat);
		void SendAddSkillToSkillbar(const string& strSkill,int iPos);
		void SendRemoveItemFromSkillbar(int iPos);
		void SendSit();
		void SendSetTarget(int iTargetID);
		void SendCombatMode(bool bCombatMode);
		void SendAction(int iEntityID,const string& strAction);
		void SendRequestContainer(int iContainerID);		
		void SendMoveItem(int iItemID,int iTarget,int iPosX,int iPosY,int iCount = -1);	
		void SendRequestPlayerList();
		void SendMessage(string strMsg,int iChannel,string strToWho);
		void SendRequestKillMe();
		void SendRequestOpenEquipment();			
		void SendTaunt(int iID);
		void SendTaunt(const string& strEmote);
		void SendRespawnRequest();
		void SendRequestItemInfo(int iItemID);
		void SendUseSkill(const string& strSkill,int iTargetID,const Vector3& kPos,const Vector3& kDir);		
			/*SendMoveItem hj?p.
				iItemID : alltid entity id't f? itemet i fr?a
				iTraget : target container entity ID , om -1 s?antas i spelarens inventory
				iPosX   : position i containern som f?em?et skall flyttas till, om -1 s?s?ts f?em?et p?en ledig plats
			*/
		
		
			
		// gui stuff
		void AddIconsToSkillTree(ZGuiTreebox* pkTreebox,vector<SKILL_TREE_INFO> vSkills, map<string,int>& rmIconMap);
		bool AlreadyInTreeList(vector<SKILL_TREE_INFO>& kList, SKILL_TREE_INFO kTest);
		void SortSkillTreeData(vector<SKILL_TREE_INFO>& rvSkills);
		void ShowLag();
		void SetupGUI();
		void InitGUIFonts();
		void FindGUIScriptsByResSuffix();
		bool NameIPFromServerList(string& strName, string& strIP);
		void ToogleChatWnd(bool bOpen, bool bSetInputFocus=false);
		void ToggleEmoteList(bool bOpen);
		//void ResizeChatDlg(bool bBigger);      	
		void LoadInGameGui();
		void LoadStartScreenGui(bool bShowSplashImage);
		void SetGuiCapture(bool bSet, bool bMoveCursorToCenter=true);
		void PositionActionButtons();
		void CloseActiveContainer();
		void CloseActiveWindow(); // on ESC press
		string GetSkillTreeSkill();	// Returns selected skill in skilltree.
						
		//register
		void RegisterPropertys();
		void RegisterResources();
	  
		//draw
		void AddTargetMarkerRP(vector<RenderPackage*>& kRenderPackages);
		
		void DrawCrossHair();
		void DrawTargetMarker();
		void DrawMouseOverMarker(const Vector3& kPos,float fSize);
		void DrawHUDEffect(int iHUDEffect);
		void UpdateCursorImage();
		
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
		void GetRenderPackages(vector<RenderPackage*>&	kRenderPackages,const RenderState& kRenderState);

		void RunCommand(int cmdid, const ConCommandLine* kCommand);
				
		void OnClientStart(void);			
		void OnClientConnected();
		void OnNetworkMessage(NetPacket *PkNetMessage);
		void OnDisconnect(int iConnectionID);

		void OnSystemMessage(const string& strType,int iNrOfParam,const void** pkParams);

      void AddRemoveServer(const char* szName, const char* szSeverIP, bool bAdd=true);
      void UpdateServerListbox();
		void AddStringToChatBox(string strMsg);

		void RegBeginPlay(string strChar);
		void DeleteChar(string strChar);
		void AddChar(string strChar, string strMod);

		
		int	GetCurrentTargetID()														{	return m_iTargetID;		}
		bool	GetCombatMode()															{	return m_bCombatMode;	}
		
		
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
		friend class SkillBar;
};



#endif











