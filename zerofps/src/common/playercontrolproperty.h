#ifndef _PLAYERCONTROLPROPERTY_H_
#define _PLAYERCONTROLPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "playerobject.h"
//#include "bunnyobject.h"
#include "statusproperty.h"

#include "inventoryproperty.h"
#include "common_x.h"

using namespace std;

class COMMON_API PlayerControlProperty: public Property {
	private:
		int FORWARD_ACTION;
		Input *m_pkInput;
		ZeroFps *m_pkFps;
		HeightMap *m_pkMap;
		OpenAlSystem* m_pkAlSys;


		ObjectManager *m_pkObjectMan;
		CollisionManager *m_pkCollisionMan;
		
		StatusProperty* m_pkStatusProperty;
		
		int m_iActionForward;
		int m_iActionStrafeRight;
		int m_iActionStrafeLeft;
		int m_iActionBack;
		int m_iActionJump;		
		int m_iActionUseItem;
		int m_iActionNextItem;
		int m_iActionZoomIn;
		int m_iActionZoomOut;		

		float m_fNextTime;
	
		float m_fSpeed;
		bool m_bAlive;
	
		float walk;
		bool walking;
		bool onGround;		
		Vector3 GroundNormal;
		
		float m_fGroundAngle;
		//Vector3 OldWalkVel;
		
		Sound *walksound;		
		
		list<InventoryProperty*>::iterator m_kCurentInv;
		list<InventoryProperty*> m_kInventory;
		
		CameraProperty*	m_pkCameraProperty;
		float	m_fFov;	// Sniper Zoom
		float	m_fCamSwitchTimer;

	public:

		
		PlayerControlProperty(Input *pkInput,HeightMap *pkMap);
		~PlayerControlProperty();		

		void Update();
		void Touch(Collision* pkCol);		
		
		void AddObject(InventoryProperty* pkProperty);
		void RemoveObject(InventoryProperty* pkProperty);
		void NextInvItem();		
		bool UseInvItem();
};



#endif



