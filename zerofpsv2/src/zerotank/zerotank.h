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
#include "../zerofpsv2/engine_systems/common/heightmap2.h"

#include "../zerofpsv2/engine_systems/mad/mad_modell.h"

#include "../zerofpsv2/engine_systems/propertys/cameraproperty.h"

#include "../zerofpsv2/gui/zgui.h"

/// Class for ZeroTank application.
class ZeroTank :public Application, public ZGuiApp {
	private:
		Camera*			m_pkCamera;
		Object*			m_pkCameraObject;
		
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

		Mad_Modell			m_pkTestMod;
		
		Object*				m_pkME;
		Object*				m_pkTestObject;

		Object*				m_pkZeroTankClientObject;
		Object*				m_pkZeroTankHull;
		Object*				m_pkZeroTankTower;
		Object*				m_pkZeroTankGun;
		Object*				m_pkZeroTankTrack;			// Owned by Vim

		Object*				m_pkZeroTank_Modify;

		// Test for rotations - Vim
		Object*				m_pkGoblinLord;
		Object*				m_pkGoblinSlave;
		Object*				m_pkGoblinSword;


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

		ZFScriptSystem* GetScript() { return pkScript; }
};



#endif











