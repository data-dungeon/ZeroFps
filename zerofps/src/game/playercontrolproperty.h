#ifndef _PLAYERCONTROLPROPERTY_H_
#define _PLAYERCONTROLPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "playerobject.h"
#include "bunnyobject.h"

using namespace std;

class PlayerControlProperty: public Property {
	private:
		int FORWARD_ACTION;
		Input *m_pkInput;
		ZeroFps *m_pkFps;
		HeightMap *m_pkMap;
		
		ObjectManager *m_pkObjectMan;
		CollisionManager *m_pkCollisionMan;
		int m_iActionForward;
		int m_iActionStrafeRight;
		int m_iActionStrafeLeft;
		int m_iActionBack;

		float walk;
		bool walking;
		
	public:
		bool onGround;
		
		PlayerControlProperty(Input *pkInput,HeightMap *pkMap);

		void Update();
		void Touch(Object* pkObject);		
};



#endif



