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
#include "../zerofps/engine/application.h"

#include "../common/common.pkg"

class UserPanel;
class MiniMap;
class P_ClientInput;
#include "guibuilder.h"


struct PickInfo
{
	int		iObject;
	HM_vert* pkVert;
	
	Vector3 kHitPos;
	Point	kSquare;
};

const float CAMERA_HEIGHT = 30.0f;

class ZeroRTS :public Application {
	private:
		
		Camera*			m_pkCamera;
		HeightMap*		m_pkMap;
		MiniMap*			m_pkMiniMap;
		P_FogRender*	m_pkFogRender;
		P_ClientInput* m_pkClientInput;

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
		
		
		//console funktions
		enum FuncId_e
		{
			FID_LOAD,
			FID_UNLOAD,
		};

		//list containin network ID of all selected units
		list<int>	m_kSelectedObjects;
		
		//list of commands that is going to be executed on the server
		vector<UnitCommand>	m_kServerCommands;
		
		//client delay
		float			m_fClickTimer;
		float			m_fClickDelay;
		
		//fog update
		float			m_fFogTimer;

		int			m_iSelfObjectID;				// Network ID that i use to send data to server.

	public:

		Vector3 GetPosFromSqr(Point square);
		Point GetSqrFromPos(Vector3 pos);
		
		GuiBuilder* m_pkGuiBuilder;
		UserPanel*	m_pkUserPanel;
		
		ZeroRTS(char* aName,int iWidth,int iHeight,int iDepth);
		~ZeroRTS() {};
	 	
	 	void OnInit(void);
		void OnIdle(void);
		void OnHud(void);
		void OnSystem();
		
		/* Pathfind test */////////////////////////////////////
		PathFind* m_pkTestPath;
		Point m_pkStart, m_pkEnd;
		Object* m_pkMoveObject;
		void BuildPath();
		bool MovePath(Object* pkObject);
		void SetObjDstPos(int sqr_x, int sqr_y, Object* pkObject);
		//////////////////////////////////////////////////////
		
		void RunCommand(int cmdid, const CmdArgument* kCommand);		
		void Init();
		void RegisterActions();		
		void RegisterPropertys();
		void Input();
		void OnServerStart(void);
		void OnClientStart(void);
		
		//picking
		Vector3 Get3DMousePos();		
		Object* PickObject();
		HM_vert* PickMap(Vector3& kHitPos);
		PickInfo Pick();
				
		//camera
		void SetCamPos(Vector3 kPos);
		Vector3 GetCamPos();
		void MoveCam(Vector3 kVel);

		//object selection
		bool AddSelectedObject(int iID);
		bool RemoveSelectedObject(int iID);
		list<int>* GetSelectedObjects(){return &m_kSelectedObjects;};
		bool AlreadySelected(int iID);
		void ClearSelected();		
		
		P_ClientUnit* GetClientUnit(int iID);
		
		//fog stuff
		void Explore();
		

	//Palls (friends) =)
	friend class UserPanel;
	friend class P_ClientInput;
};



#endif











