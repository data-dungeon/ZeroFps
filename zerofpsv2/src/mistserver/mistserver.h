#ifndef _MISTSERVER_H_
#define _MISTSERVER_H_

#ifndef _DONT_MAIN
	#define _MAINAPPLICATION_
	#define _DONT_MAIN
#endif

#include <iostream>
#include <cmath>
#include <GL/glut.h>
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
#include "../mcommon/p_spell.h"
#include "../mcommon/p_serverinfo.h"
#include "../mcommon/p_clientcontrol.h"
#include "../mcommon/p_enviroment.h"

class MistServer :public Application , public ZGuiApp {
	private:

		struct MENU_INFO
		{
			ZGuiCombobox* cb;
			int iIndex;
			char* szCommando;
		};

		int m_uiNumMenuItems;
		MENU_INFO* m_pkMenuInfo;

		char* GetSelEnviromentString();

		//console funktions
		enum FuncId_e
		{
			FID_NEW,
			FID_LOAD,
			FID_SAVE,
		};
		enum EditMode_e
		{
			EDIT_ZONES,
			EDIT_OBJECTS,
		
		};

		//server stuff
		Entity* m_pkServerInfo;
		P_ServerInfo* m_pkServerInfoP;

		vector<pair<string,Vector3> >	m_kLocations;

		//edit stuff
		int		m_iEditMode;

		string	m_strActiveZoneName, m_strPrevZoneName;
		string	m_strActiveObjectName;
		string	m_strActiveEnviroment;
		Entity*	m_pkCameraObject;
		Camera*	m_pkCamera;

		Vector3	m_kZoneSize;
		Vector3	m_kZoneMarkerPos;
		
		Vector3	m_kObjectMarkerPos;
		int		m_iCurrentObject;
		
		float		m_fClickDelay;
		int 		m_iCurrentMarkedZone;

		Vector3	Get3DMousePos(bool m_bMouse);
		Entity*	GetTargetObject();		

		void SetZoneEnviroment(const char* szEnviroment);
		string GetZoneEnviroment();
		void UpdateZoneMarkerPos();
		void UpdateObjectMakerPos();
		void DrawZoneMarker(Vector3 kPos);
		void DrawCrossMarker(Vector3 kPos);
		void AddZone();

		void HandleOrders();
		bool CheckValidOrder(ClientOrder* pkOrder);
		void SendTextToMistClientInfoBox(char* szText);

			struct SORT_FILES : public binary_function<string, string, bool> {
				bool operator()(string x, string y) { 
					return (x.find(".") == string::npos);
				};
			} SortFiles;

		bool BuildFileTree(char* szTreeBoxName, char* szRootPath);
		void ToogleLight(bool bEnabled);
		void RotateActiveZoneObject();

		/* Owner by Vim, Master Of Ugly Code, King of the armys of bugs. LOL*/
			AStar*	m_pkAStar;
			vector<Vector3> kPath;
			Vector3 kPathStart;
			Vector3 kPathEnd;

			void PathTest();

		bool CreateMenu(char* szFileName);

	public:
		void OnClickTabPage(ZGuiTabCtrl *pkTabCtrl, int iNewPage, int iPrevPage);
		void OnClickListbox( int iListBoxID/*ZGuiWnd* pkListBox*/, int iListboxIndex, ZGuiWnd* pkMain);
		void OnCommand(int iID, ZGuiWnd* pkMainWnd);
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
		
		int CreatePlayer(const char* csName,const char* csLocation,int iConID);
		void DeletePlayer(int iConID);
		Vector3 GetPlayerStartLocation(const char* csName);
		void UpdateStartLocatons();
		
		
		//on client join, server runs this
		void OnServerClientJoin(ZFClient* pkClient,int iConID);
		void OnServerClientPart(ZFClient* pkClient,int iConID);

		bool StartUp();
		bool ShutDown();
		bool IsValid();

};



#endif











