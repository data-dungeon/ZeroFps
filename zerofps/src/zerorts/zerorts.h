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
#include "guibuilder.h"


struct PickInfo
{
	int		iObject;
	HM_vert* pkVert;
	
	Vector3 	kHitPos;
};



class ZeroRTS :public Application {
	private:
		
		Camera*		m_pkCamera;
		HeightMap*	m_pkMap;
		MiniMap*	m_pkMiniMap;

		// actions
		int 			m_iActionCamLeft;
		int 			m_iActionCamRight;
		int 			m_iActionCamUp;
		int 			m_iActionCamDown;
		int 			m_iActionSelect;
		int 			m_iActionScroll;		
		int 			m_iActionSelectManyModifier;		
		
		//console funktions
		enum FuncId_e
		{
			FID_LOAD,
			FID_UNLOAD,
		};

		list<int>	m_kSelectedObjects;

		//client delay
		float			m_fClickTimer;
		float			m_fClickDelay;

	public:
		//void DrawMinimap();
		
		GuiBuilder* m_pkGuiBuilder;
		UserPanel*	m_pkUserPanel;
		

		ZeroRTS(char* aName,int iWidth,int iHeight,int iDepth);
		~ZeroRTS() {};
	 	
	 	void OnInit(void);
		void OnIdle(void);
		void OnHud(void);
		
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

	//Palls (friends) =)
	friend class UserPanel;
};



#endif











