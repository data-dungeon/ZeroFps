#ifndef _MISTSERVER_H_
#define _MISTSERVER_H_

#include <iostream>
#include <cmath>
#include <list>
#include <vector>
#include "../zerofpsv2/engine/application.h"
#include "../zerofpsv2/engine_systems/mad/mad_modell.h"
#include "../zerofpsv2/engine/astar.h"

#include "../zerofpsv2/engine_systems/propertys/p_camera.h"
#include "../zerofpsv2/gui/zgui.h"

#include "../mcommon/si_mistland.h"
#include "../mcommon/si_dm.h"

#include "../mcommon/p_event.h"
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
#include "../mcommon/p_shadowblob.h"
#include "../mcommon/p_dmgun.h"
#include "../mcommon/p_dmitem.h"
#include "../mcommon/p_dmmission.h"
#include "../mcommon/p_dmclickme.h"
#include "../mcommon/p_dmshop.h"
#include "../mcommon/p_car.h"
#include "../zerofpsv2/engine/fh.h"

#include "../mcommon/p_arcadecharacter.h"

#include "ambientsoundareas.h"

//const string CREATED_SCRIPTS_DIR = "data/script/custom_scripts/";

/**	\brief	Da ZeroEd
		\ingroup ZeroEd
*/
class ZeroEd :public Application , public ZGuiApp {
	private:

		//console funktions
		enum FuncId_e
		{
			FID_NEW,
			FID_LOAD,
			FID_SAVE,
			FID_SAVEAS,
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
			FID_CUT,
			FID_COPY,
			FID_PASTE,

			FID_SNAPSAVE,
			FID_SNAPLOAD,
			FID_TEST_JIDDRA,		// Owned by Vim.
		};
			
		enum EditMode_e
		{
			EDIT_ZONES,
			EDIT_OBJECTS,
			EDIT_HMAP,
			EDIT_MAX,
			EDIT_AMBIENTSOUNDS,		
		};
		
		//wierd stuff
		char		szCoolName[256];
		string	strMasterSmiley;

		// zone and object edit
		bool m_bPlaceObjectsOnGround;
		bool m_bDisableFreeZonePlacement;
		vector< pair<Vector3,Vector3> > m_kAddedZonePlacement;
		bool m_bNeedToRebuildZonePosArray;

		// ambient sound areas
		AmbientSoundAreas* m_pkAmbientSoundAreas;
		
		string	m_strWorldDir;						// The current dir for the world. Use for SaveAs and Title.

		//delay
		float	m_fDelayTime;
		
		//edit sun
		bool 			m_bEditSun;
		LightSource	m_kSun;				
		
		//edit stuff
		int		m_iEditMode;

		//zone data
		string	m_strActiveZoneName, m_strPrevZoneName;
		string	m_strActiveObjectName;
		string	m_strActiveEnviroment;
		
		//camera
		Entity*	m_pkCameraObject[4];
		Camera*	m_pkCamera[4];
		Entity*	m_pkActiveCameraObject;
		Camera*	m_pkActiveCamera;
		bool		m_bSoloMode;
		float 	m_CamMoveSpeed;		
		string	m_strActiveViewPort;		//active view port

		//zone data
		Vector3	m_kZoneSize;
		Vector3	m_kZoneMarkerPos;
		
		//entity data
		Vector3	m_kObjectMarkerPos;
		int		m_iCurrentObject;
		int 		m_iCurrentMarkedZone;
		
		//physics stuff
		Vector3	m_kLocalGrabPos;
		Vector3	m_kGrabPos;
		Vector3	m_kGrabCurrentPos;
		int		m_iGrabEntity;
		float		m_fArmLength;
		bool		m_bGrabing;

		//graphs
		DebugGraph m_kTestGraph;
				
		//selected entitys
		set<int>	m_SelectedEntitys;
		
		//heightmap stuff
		Vector3						m_kDrawPos;
		vector<HMSelectVertex>	m_kSelectedHMVertex;
		float 						m_fHMInRadius;
		float 						m_fHMOutRadius;
		int							m_iEditLayer;

		//network
		vector<ZoneData>			m_kNetworkZones;
								
		//consolecommand handle
		void EditRunCommand(FuncId_e eEditCmd);
		
		//gui stuff...or something 
		void RebuildZonePosArray();
		bool ZoneHaveNeighbour(const Vector3& kPos, const Vector3& kSize);
		void FillPropertyValList();
		bool FillPropertyList();
		bool UpdatePropertyList(int iID);
		bool PlaceObjectOnGround(int iObjectID, int iZoneID);
		bool PlaceObjectOnGround(int iObjectID);
		

		//camera stuff
		void  CreateEditCameras();
		bool 	SetCamera(int iNum);
		int	GetView(float x, float y);
		void 	CamFollow(bool bFollowMode);


		// Selection of Entitys.
		void Select_None(	)				{ m_SelectedEntitys.clear(); }
		void Select_Add( int iId )		{ m_SelectedEntitys.insert(iId); }
		void Select_Remove( int iId )	{ m_SelectedEntitys.erase(iId); }
		void DrawSelectedEntity();
		void Select_Toggle(int iId, bool bMultiSelect);

		void 		DeleteSelected();			// Removes selected entitys.

		//picking
		Vector3	Get3DMousePos(bool m_bMouse);
		Vector3	Get3DMouseDir(bool bMouse);
		Entity*	GetTargetObject();		
		int		GetTargetTCS(Vector3* pkPos);		
		
		
		//enviroment
		void 		SetZoneEnviroment(const char* szEnviroment);
		string 	GetZoneEnviroment();
		char* 	GetSelEnviromentString();
		
		void UpdateZoneMarkerPos();
		void UpdateObjectMakerPos();
		void DrawZoneMarker(Vector3 kPos);
		void DrawCrossMarker(Vector3 kPos);
		void AddZone(Vector3 kPos, Vector3 kSize, string strName, bool bEmpty=false);

		void SetupGuiEnviroment();
		void ToogleLight(bool bEnabled);
		void RotateActive();

		HeightMap* SetPointer();

		void DrawHMEditMarker(HeightMap* pkHmap, Vector3 kCenterPos, float fInRadius, float fOutRadius );

		void HMModifyCommand(float fSize);

		//input
		void 	Input_Camera(float fMouseX, float fMouseY);
		void 	Input_EditTerrain();
		void 	Input_EditZone();
		void 	Input_EditObject(float fMouseX, float fMouseY);		
		void Input_EditAmbientSounds();
		bool	DelayCommand();			//make a delay
		
		//network
		void	SendSetZoneModel(string strModel,int iZoneID);
		void	SendZoneListRequest();
		void	DrawZoneList();
		
	public:
		bool SetViewPort(const char* szVpName);

		void OnClickTabPage(ZGuiTabCtrl *pkTabCtrl, int iNewPage, int iPrevPage);
		void OnClickListbox( int iListBoxID/*ZGuiWnd* pkListBox*/, int iListboxIndex, ZGuiWnd* pkMain);
		void OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd* pkMainWnd);
		void OnClickTreeItem(char *szTreeBox, char *szParentNodeText, 
									char *szClickNodeText, bool bHaveChilds);
		
		ZeroEd(char* aName,int iWidth,int iHeight,int iDepth);
	 	
	 	void OnInit();
		void OnIdle();
		void OnHud();
		void OnSystem();
				
		void RunCommand(int cmdid, const CmdArgument* kCommand);		
		void Init();
		void RegisterPropertys();
		void RegisterResources();		
		void Input();
		void OnServerStart();
		void OnClientStart();

		void AddPropertyVal();
		void AddProperty(int iEntityID,const string& strProperty);
		void RemoveSelProperty();
      bool SaveCurrentToScript();
	
		//on client join, server runs this
		bool OnPreConnect(IPaddress, char*, char*, bool bIsEditor, string& strWhy){return true;}
		void OnServerClientJoin(ZFClient*,int, char*, char*, bool bIsEditor){};
		void OnServerClientPart(ZFClient* pkClient,int iConID){};
		void RenderInterface(void);
		void OnNetworkMessage(NetPacket *PkNetMessage);
		void OnDisconnect(int iConnectionID);

		void AutoSetZoneSize(string strName);
		void SoloToggleView();


		bool StartUp()		{ return true; }
		bool ShutDown()	{ return true; }
		bool IsValid()		{ return true; }

		//void SendAppMessage(NetPacket* pkNet);

};



#endif











