#ifndef _MISTSERVER_H_
#define _MISTSERVER_H_

#include <vector>

#include "../zerofpsv2/engine/application.h"
#include "../zerofpsv2/gui/zgui.h"
#include "guiserverinfo.h"
#include "../zerofpsv2/engine/entitymanager.h"
#include "../zerofpsv2/engine_systems/propertys/p_heightmap.h"
#include "../zerofpsv2/basic/concommand.h"

class Camera;
class Entity;


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

			FID_RECREATE,
			
			FID_SETCAM,
			FID_CAMLINK,
			FID_CAMSOLO,
			FID_CAMGRID,
			FID_SELNONE,
			FID_GRIDSIZE,
			FID_GRIDAUTOSNAP,
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
			FID_SNAPSIZE,
			FID_FINDENT,
			FID_TRANSIDENT,
			FID_SCALEIDENT,
		};
			
		enum EditMode_e
		{
			EDIT_ZONES,
			EDIT_OBJECTS,
			EDIT_HMAP,
			EDIT_MAX,
			EDIT_AMBIENTSOUNDS,
			EDIT_ENVIROMENTS,		
			EDIT_CREATEOBJECT,
		};

		enum GUISelectFileWndState_e
		{
			NONE = 0,
			SELECT_SOUND,
			SELECT_MAD,
		};

		enum ZoneCorners
		{ 
			eNone=0, 
			eTopLeft, 
			eTop, 
			eTopRight, 
			eRight, 
			eBottomRight, 
			eBottom, 
			eBottomLeft, 
			eLeft 
		};

		enum HMapEditMode
		{
			HMAP_EDITVERTEX,
			HMAP_DRAWSMFLAT,
			HMAP_DRAWMASK,
			HMAP_DRAWVISIBLE,
			HMAP_STITCH,
			HMAP_SMOOTH, 
			HMAP_MAX,
			HMAP_DRAWBRIGHTNESS,
		};


		// Materials
		ZMaterial*	m_pkLine;

		// zone and object edit
		bool		m_bPlaceObjectsOnGround;
		ConVar	m_kEntitySnaping;
		int		m_iAutoSnapZoneCorner; // -1 = dont use

		// gui stuff
		int		m_iSelectFileState;		
		Entity*	m_pkPreviewEntity;
		vector<GUIServerInfo> m_vkServerList;
		
		string	m_strWorldDir;						// The current dir for the world. Use for SaveAs and Title.

		//
		bool	m_bRemoteEditing;
		
		//delay
		float	m_fDelayTime;

		// snapsize
		Vector3	m_kSnapSize;
		Vector3 m_kSnapOffset;

		// camera->marker distance
		float	m_fZoneMarkerDistance;
		float	m_fObjectMarkerDistance;
		
		//edit sun
		bool 		m_bEditSun;
		ConVar	m_kbRotatingSun;
		ConVar	m_kbBloom;

		//edit stuff
		int		m_iEditMode;
		bool		m_bLockCreate;		// Stay in create until user picks another mode.
		int		m_iHMapEditMode;

		//camera
		Entity*	m_pkCameraObject[4];
		Camera*	m_pkCamera[4];
		Entity*	m_pkActiveCameraObject;
		Camera*	m_pkActiveCamera;
		bool		m_bSoloMode;
		float 	m_CamMoveSpeed;					// Std speed for edit cam, depends on edit mode.				
		float 	m_CamSpeedScale;					// Scales the speed of the edit cam cnd can be changed by the user.
		string	m_strActiveViewPort;				// Name of active view port
		ConVar	m_kbPlaneMovement;

		//zone data
		string	m_strActiveZoneName;		
		string	m_strActiveEnviroment;		
		Vector3	m_kZoneSize;
		Vector3	m_kZoneMarkerPos;
		Vector3	m_kZoneModelRotation;
		Vector3  m_kLastZonePos;
		Vector3  m_kLastZoneSize;
		
		Entity*	m_pkZoneMarkerEntity;
		
		//entity data
		string	m_strActiveObjectName;		
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

		//selection box
		bool		m_bIsSelecting;
		Vector3	m_kSelBoxCornerMin;
		Vector3	m_kSelBoxCornerMax;

		//graphs
		DebugGraph m_kTestGraph;
				
		//selected entitys
		set<int>	m_SelectedEntitys;
		
		//heightmap stuff
		Vector3						m_kDrawPos;
		vector<HMSelectionData>	m_kSelectedHMVertex;
		float 						m_fHMInRadius;
		float 						m_fHMOutRadius;
		float 						m_fHMStrength;
		string						m_strHeightmapMaterial;
		float							m_fHeightmapBrightness;
		
		//network
		vector<ZoneData>			m_kNetworkZones;
								
		ConVar 	m_kstrLoginName;
		ConVar 	m_kstrLoginPW;

		ConFunction g_kFuncJiddra;

		//consolecommand handle
		void EditRunCommand(FuncId_e eEditCmd);
		
		// zone and object edit
		bool PlaceSelectionOnGround();
		
		//gui stuff 
		void FillPropertyValList();
		bool FillPropertyList();
		bool UpdatePropertyList(int iID);		
		void SetupPreviewWnd(const char* szMadFile);
		void UpdatePreviewObject();
		void OpenObjectMenu();
		void InitMainMenu();
		void GUIFillServerList();
		void OnGUIAddServer(bool bAdd);
		void AddLoadFoldersToMenu();
		void ResizeWorkTab(int iSize);

		//camera stuff
		void  CreateEditCameras();
		bool 	SetCamera(int iNum);
		int	GetView(float x, float y);
		void 	CamFollow(bool bFollowMode);
		void  CamCenterOnSelection();


		// Selection of Entitys.
		void	Select_None(	)				{ m_SelectedEntitys.clear(); }
		void	Select_Add( int iId )		{ m_SelectedEntitys.insert(iId); }
		void	Select_Remove( int iId )	{ m_SelectedEntitys.erase(iId); }
		void	DrawSelectedEntity();
		void	Select_Toggle(int iId, bool bMultiSelect);


		//picking
		void AABBSelect(const Vector3& kP1,const Vector3& kP2,bool bAdd);
		Vector3	Get3DMouseDir(bool bMouse);
		Entity*	GetTargetObject();		
		int		GetTargetTCS(Vector3* pkPos);		
				
		int			GetZoneID(const Vector3& kPos);
		ZoneData*	GetZoneData(int iZoneID);
		ZoneData*	GetZoneByEntityID(int iEntityID);
		
		//enviroment
		string 	GetZoneEnviroment();
		char* 	GetSelEnviromentString();
		
		void	UpdateModelMarker(Vector3 kPos,bool bEnabled);
		void	UpdateZoneMarkerPos();
		Vector3 GetZonePosAutoSnap();
		void	UpdateObjectMakerPos();
		void	DrawZoneMarker(Vector3 kPos);
		void	DrawCrossMarker(Vector3 kPos);

		void	SetupGuiEnviroment();
		void	SetupGui_Terrain();
		void	ToogleLight();

		//heightmap
		void	CreateHmapZone(const Vector3& kPos,const Vector3& kSize);
		bool	SetPointer();
		void	DrawHMEditMarker(Vector3 kCenterPos, float fInRadius, float fOutRadius );
		void	HMModifyCommand(float fSize);
		void  HMDrawTexture(const string& strMaterial);
		void	HMDrawBrightness(float fBrightness);	
		void  HMCommand(HMapEditMode eHMCmd, float fSample = 0);


		//input
		void 	Input_Camera(float fMouseX, float fMouseY);
		void 	Input_EditTerrain();
		void 	Input_EditZone();
		void 	Input_EditObject(float fMouseX, float fMouseY);		
		void 	Input_CreateObject(float fMouseX, float fMouseY);		
		void	Input_EditAmbientSounds();
		void  Input_SandBox(float fMouseX, float fMouseY);
		bool	DelayCommand();			//make a delay
		
		//network
		void	SendSetZoneModel(string strModel,int iZoneID);
		void	SendSetZoneEnviroment(string strEnviroment,int iZoneID);
		void	SendRotateZoneModel(int iZoneID);
		void	SendZoneListRequest();
		void	SendDeleteSelected();			// Removes selected entitys.
		void	AddSelected(NetPacket* pkNetPack);	// Add id's of selected entitys to packet.
		void	SendAddZone(Vector3 kPos, Vector3 kSize, Vector3 kModelRot, string strName);		
		void	SendRotateEntity(int iEntityID,const Vector3& kRot);
		void	SendAction(int iEntityID,const string& strAction);
		
		void	SendTranslateSelection(const Vector3& kPosition,bool bRelative,bool bSnap=false);
		void	SendRotateSelection(const Vector3& kRot);
		
		vector<HMSelectVertex> GetAllSelectedHMVertex();

	public:

		void GetRenderPackages(vector<RenderPackage*>&	kRenderPackages,const RenderState& kRenderState);

		bool SetViewPort(const char* szVpName);

		void OnClickTabPage(ZGuiTabCtrl *pkTabCtrl, int iNewPage, int iPrevPage);
		void OnClickListbox( int iListBoxID, int iListboxIndex, ZGuiWnd* pkMain);
		void OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd* pkMainWnd);
		void OnKeyPress(int iID, ZGuiWnd* pkWnd);
		
		void OnClickTreeItem(char *szTreeBox, char *szParentNodeText, 
									char *szClickNodeText, bool bHaveChilds);
		
		ZeroEd(char* aName,int iWidth,int iHeight,int iDepth);
	 	
	 	void OnInit();
		void OnIdle();
		void OnHud();
		void OnSystem();
				
		void RunCommand(int cmdid, const ConCommandLine* kCommand);		
		void Init();
		void RegisterPropertys();
		void RegisterResources();		
		void Input();
		void OnServerStart();
		void OnClientStart();
		void OnClientConnected();

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
		bool StartUp();
		bool ShutDown();
		bool IsValid()		{ return true; }

      friend void GuiMsgTerrain( string strMainWnd, string strController, unsigned int msg, int numparms, void *params );
		friend bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params );

		//void SendAppMessage(NetPacket* pkNet);

};



#endif


