#ifndef _MISTCLIENT_H_
#define _MISTCLIENT_H_

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

#include "../zerofpsv2/engine_systems/propertys/cameraproperty.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/engine_systems/common/heightmap2.h"
#include "../zerofpsv2/gui/zgui.h"

#include "../mcommon/p_event.h"
#include "../mcommon/p_ml.h"
#include "../mcommon/p_charstats.h"
#include "../mcommon/si_mistland.h"
#include "../mcommon/p_serverinfo.h"
#include "../mcommon/p_clientcontrol.h"

class MistClient :public Application, public ZGuiApp {
	private:
		
		// actions
		int 			m_iActionCamLeft;
		int 			m_iActionCamRight;
		int 			m_iActionCamUp;
		int 			m_iActionCamDown;
		int 			m_iActionSelect;
		int 			m_iActionScroll;		
		int 			m_iActionSelectManyModifier;		
		int			m_iActionExploreAll;
		int			m_iActionUnExploreAll;		
		int			m_iActionPrintServerInfo;				
		int			m_iActionDoOrder;
		
		//console funktions
		enum FuncId_e
		{
			FID_LOAD,
			FID_UNLOAD,
			FID_MASSSPAWN,
		};

		//clients own little qute object
		int					m_iSelfObjectID;				// Network ID that i use to send data to server.

		Heightmap2*			m_pkMap2;
		
		Camera*				m_pkCamera;
		Object*				m_pkTestobj;
		CameraProperty*	m_pkCamProp;		
		float					m_fAngle;
		float					m_fDistance;
		
		float 				m_fClickDelay;
		
		Vector3	Get3DMousePos();
		Object*	GetTargetObject();		

	public:
		void OnCommand(int iID, ZGuiWnd* pkMainWnd);
		
		MistClient(char* aName,int iWidth,int iHeight,int iDepth);
	 	
	 	void OnInit(void);
		void OnIdle(void);
		void OnHud(void);
		void OnSystem();
				
		void RunCommand(int cmdid, const CmdArgument* kCommand);		
		void Init();
		void RegisterActions();		
		void RegisterPropertys();
		void Input();
		void OnServerStart(void);
		void OnClientStart(void);
		
		
		//init client
		void ClientInit();
		
		//on client join, server runs this
		void OnServerClientJoin(ZFClient* pkClient,int iConID);
		void OnServerClientPart(ZFClient* pkClient,int iConID);

		bool StartUp();
		bool ShutDown();
		bool IsValid();

		ZFScriptSystem* GetScript() { return pkScript; }

};



#endif











