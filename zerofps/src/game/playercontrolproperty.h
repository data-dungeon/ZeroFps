#ifndef _PLAYERCONTROLPROPERTY_H_
#define _PLAYERCONTROLPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "playerobject.h"
//#include "bunnyobject.h"
#include "statusproperty.h"

using namespace std;

class PlayerControlProperty: public Property {
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

		float walk;
		bool walking;
		
		Sound *walksound;		
		
		CameraProperty*	m_pkCameraProperty;
		float	m_fFov;	// Sniper Zoom

	public:
		bool onGround;
		
		PlayerControlProperty(Input *pkInput,HeightMap *pkMap);
		~PlayerControlProperty();		

		void Update();
		void Touch(Object* pkObject);		
};



#endif



