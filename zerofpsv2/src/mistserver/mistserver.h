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

#include "../zerofpsv2/engine_systems/propertys/cameraproperty.h"
#include "../zerofpsv2/gui/zgui.h"


#include "../mcommon/p_event.h"
#include "../mcommon/si_mistland.h"
#include "../mcommon/p_ml.h"
#include "../mcommon/p_charstats.h"
#include "../mcommon/p_serverinfo.h"
#include "../mcommon/p_clientcontrol.h"

class MistServer :public Application , public ZGuiApp {
	private:
		//console funktions
		enum FuncId_e
		{
			FID_NEW,
			FID_LOAD,
			FID_SAVE,
		};

		string  m_strActiveZoneName, m_strPrevZoneName;
		Object* m_pkCameraObject;
		Camera* m_pkCamera;

		Object* m_pkServerInfo;
		P_ServerInfo* m_pkServerInfoP;

		Vector3	m_kZoneSize;
		Vector3	m_kZoneMarkerPos;

		Vector3	Get3DMousePos(bool m_bMouse);
		Object*	GetTargetObject();		

		void UpdateZoneMarkerPos();
		void DrawZoneMarker(Vector3 kPos);
		void AddZone();

	public:
		void OnClickListbox( ZGuiWnd* pkListBox, int iListboxIndex);
		void OnCommand(int iID, ZGuiWnd* pkMainWnd);
		
		MistServer(char* aName,int iWidth,int iHeight,int iDepth);
	 	
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
		
		//on client join, server runs this
		void OnServerClientJoin(ZFClient* pkClient,int iConID);
		void OnServerClientPart(ZFClient* pkClient,int iConID);

		bool StartUp();
		bool ShutDown();
		bool IsValid();

};



#endif











