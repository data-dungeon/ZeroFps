#ifndef _ZERORTS_H_
#define _ZERORTS_H_

#ifndef _DONT_MAIN
	#define _MAINAPPLICATION_
	#define _DONT_MAIN
#endif

#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <list>
#include "../zerofpsv2/engine/application.h"
//#include "../zerofpsv2/engine/levelmanager.h"
#include "../zerofpsv2/engine_systems/common/heightmap2.h"

#include "../zerofpsv2/engine_systems/mad/mad_modell.h"

#include "../zerofpsv2/engine_systems/propertys/cameraproperty.h"

#include "../zerofpsv2/gui/zgui.h"

class ZeroTank :public Application, public ZGuiApp {
	private:
		Camera*			m_pkCamera;
		Object*			m_pkCameraObject;
		
		//HeightMap*		m_pkMap;
		// actions
/*		int 			m_iActionCamLeft;
		int 			m_iActionCamRight;
		int 			m_iActionCamUp;
		int 			m_iActionCamDown;
		int 			m_iActionSelect;
		int 			m_iActionScroll;		
		int 			m_iActionSelectManyModifier;		
		int			m_iActionExploreAll;
		int			m_iActionUnExploreAll;		
//		int			m_iActionPrintServerInfo;				
		int			m_iActionDoOrder;*/
		
		//this is true when client has got heightmap from server
		bool			m_HaveFoundHMapObject;
		
		//console funktions
		enum FuncId_e
		{
			FID_LOAD,
			FID_UNLOAD,
			FID_MASSSPAWN,
		};

		//clients own little qute object
		int					m_iSelfObjectID;				// Network ID that i use to send data to server.

		//list of possible spawn points
//		vector<Vector3>	m_kSpawnPoints;
		//wich gametype for server to use
//		int					m_iGameType;
//		Heightmap2*			m_pkMap2;
		//GuiBuilder*			m_pkGuiBuilder;
		//float m_fConePosY;

		Mad_Modell			m_pkTestMod;
		
		Object*				m_pkME;
		Object*				m_pkTestObject;

		Object*				m_pkZeroTankClientObject;
		Object*				m_pkZeroTankHull;
		Object*				m_pkZeroTankTower;
		Object*				m_pkZeroTankGun;
		Object*				m_pkZeroTankTrack;			// Owned by Vim

		Object*				m_pkZeroTank_Modify;


	public:
		void OnCommand(int iID, ZGuiWnd* pkMainWnd);
		
		ZeroTank(char* aName,int iWidth,int iHeight,int iDepth);
	 	
	 	void OnInit(void);
		void OnIdle(void);
		void OnHud(void);
		void OnSystem();
				
		void RunCommand(int cmdid, const CmdArgument* kCommand);		
		void Init();
		void RegisterPropertys();
		void Input();
		void OnServerStart(void);
		void OnClientStart(void);
		
		
		//init client
		void ClientInit();
		
		//order handler
		void HandleOrders();


		//on client join, server runs this
		void OnServerClientJoin(ZFClient* pkClient,int iConID);
		void OnServerClientPart(ZFClient* pkClient,int iConID);

		bool StartUp();
		bool ShutDown();
		bool IsValid();

		ZFScript* GetScript() { return pkScript; }

		//void RegisterActions();		
		//Setup spawn points
		//void SetupSpawnPoints();
};



#endif











