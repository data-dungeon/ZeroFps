#ifndef _MISTSERVER_H_
#define _MISTSERVER_H_

#ifndef _DONT_MAIN
	#define _MAINAPPLICATION_
	#define _DONT_MAIN
#endif

#include <iostream>
#include <cmath>
//#include <GL/glut.h>
#include <list>
#include "../zerofpsv2/engine/application.h"
#include "../zerofpsv2/engine_systems/mad/mad_modell.h"
#include "../zerofpsv2/engine/astar.h"

#include "../zerofpsv2/engine_systems/propertys/p_camera.h"
#include "../zerofpsv2/gui/zgui.h"

#include <vector>


#include "../mcommon/p_event.h"
#include "../mcommon/si_mistland.h"
#include "../mcommon/p_ml.h"
#include "../mcommon/p_charstats.h"
#include "../mcommon/p_item.h"
#include "../mcommon/p_container.h"
#include "../mcommon/p_spell.h"
#include "../mcommon/p_serverinfo.h"
#include "../mcommon/p_clientcontrol.h"
#include "../mcommon/p_enviroment.h"
#include "../mcommon/p_ai.h"
#include "../mcommon/p_spawn.h"
#include "../mcommon/p_dmhq.h"
#include "../mcommon/p_dmcharacter.h"

#include "playerdatabase.h"



/**	\brief	Da MistServer
		\ingroup MistServer
*/
class MistServer :public Application , public ZGuiApp {
	private:

		char* GetSelEnviromentString();

		//console funktions
		enum FuncId_e
		{
			FID_NEW,
			FID_LOAD,
			FID_SAVE,
			FID_USERS,
			FID_LOCALORDER,
			FID_LIGHTMODE,
			FID_EDITSUN,

			FID_SETCAM,
			FID_CAMLINK,
			FID_CAMSOLO,
			FID_CAMGRID,
			FID_SELNONE,
			FID_GRIDSIZE,
			FID_GRIDSNAP,
			FID_CAMFOLLOW,
			FID_CAMNOFOLLOW,
			FID_DELETE,
			FID_CLONE,

		};

		enum EditMode_e
		{
			EDIT_ZONES,
			EDIT_OBJECTS,
			EDIT_HMAP,
			EDIT_MAX,
		
		};

		bool	m_bEditSun;

		//server stuff
		Entity* m_pkServerInfo;
		P_ServerInfo* m_pkServerInfoP;

		vector<pair<string,Vector3> >	m_kLocations;
		
		PlayerDatabase*	m_pkPlayerDB;
		bool					m_AcceptNewLogins;

		//edit stuff
		int		m_iEditMode;

		string	m_strActiveZoneName, m_strPrevZoneName;
		string	m_strActiveObjectName;
		string	m_strActiveEnviroment;
		
		Entity*	m_pkCameraObject[4];
		Camera*	m_pkCamera[4];
		Entity*	m_pkActiveCameraObject;
		Camera*	m_pkActiveCamera;
		bool		m_bSoloMode;


		Vector3	m_kZoneSize;
		Vector3	m_kZoneMarkerPos;
		
		Vector3	m_kObjectMarkerPos;
		int		m_iCurrentObject;
		
		int 		m_iCurrentMarkedZone;

		Vector3	Get3DMousePos(bool m_bMouse);
		Vector3 Get3DMouseDir(bool bMouse);

		Entity*	GetTargetObject();		

		bool	SetCamera(int iNum);
		int	GetView(float x, float y);
		void CamFollow(bool bFollowMode);

		// Selection of Entitys.
		set<int>	m_SelectedEntitys;
		void Select_None(	)				{ m_SelectedEntitys.clear(); }
		void Select_Add( int iId )		{ m_SelectedEntitys.insert(iId); }
		void Select_Remove( int iId )	{ m_SelectedEntitys.erase(iId); }
		void DrawSelectedEntity();
		void Select_Toggle(int iId, bool bMultiSelect);

		void DeleteSelected();			// Removes selected entitys.


		void SetZoneEnviroment(const char* szEnviroment);
		string GetZoneEnviroment();
		void UpdateZoneMarkerPos();
		void UpdateObjectMakerPos();
		void DrawZoneMarker(Vector3 kPos);
		void DrawCrossMarker(Vector3 kPos);
		void AddZone(Vector3 kPos, Vector3 kSize, string strName, bool bEmpty=false);

		void HandleOrders();
		void HSO_Edit(ClientOrder* pkOrder);
		void HSO_Character(ClientOrder* pkOrder);

		bool CheckValidOrder(ClientOrder* pkOrder);
		void SendTextToMistClientInfoBox(char* szText);

		void SetupGuiEnviroment();
		void ToogleLight(bool bEnabled);
		void RotateActiveZoneObject();

		/* Owner by Vim, Master Of Ugly Code, King of the armys of bugs. LOL*/
			AStar*	m_pkAStar;
			vector<Vector3> kPath;
			Vector3 kPathStart;
			Vector3 kPathEnd;

			void PathTest();

		Vector3	m_kDrawPos;
		HeightMap* SetPointer();

		void DrawHMEditMarker(HeightMap* pkHmap, Vector3 kCenterPos, float fInRadius, float fOutRadius );

		LightSource	m_kSun;


		vector<HMSelectVertex>	m_kSelectedHMVertex;
		float m_fHMInRadius;
		float m_fHMOutRadius;
		int	m_iEditLayer;

		void HMModifyCommand(float fSize);

		float m_CamMoveSpeed;
		void Input_Camera(float fMouseX, float fMouseY);
		void Input_EditTerrain();
		void Input_EditZone();
		void Input_EditObject(float fMouseX, float fMouseY);

	public:
		void OnClickTabPage(ZGuiTabCtrl *pkTabCtrl, int iNewPage, int iPrevPage);
		void OnClickListbox( int iListBoxID/*ZGuiWnd* pkListBox*/, int iListboxIndex, ZGuiWnd* pkMain);
		void OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd* pkMainWnd);
		void OnClickTreeItem(char *szTreeBox, char *szParentNodeText, 
			char *szClickNodeText, bool bHaveChilds);
		
		MistServer(char* aName,int iWidth,int iHeight,int iDepth);
	 	
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
		
		//init client
		void ClientInit();
		
		int CreatePlayer(const char* csPlayer,const char* csCharacter,const char* csLocation,int iConID);
		void DeletePlayer(int iConID);
		void SpawnPlayer(int iConID);

		Vector3 GetPlayerStartLocation(const char* csName);
		void UpdateStartLocatons();

		//on client join, server runs this
		bool OnPreConnect(IPaddress kRemoteIp, char* szLogin, char* szPass);
		void OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass);
		void OnServerClientPart(ZFClient* pkClient,int iConID);
		void RenderInterface(void);

		void AutoSetZoneSize(string strName);
		void SoloToggleView();

		float	m_fDelayTime;
		bool	DelayCommand();

		bool StartUp();
		bool ShutDown();
		bool IsValid();

};



#endif











