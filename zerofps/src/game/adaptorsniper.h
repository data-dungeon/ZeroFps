#ifndef _ADAPTOR_SNIPER_H_
#define _ADAPTOR_SNIPER_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "statusproperty.h"

using namespace std;

class AdaptorSniper: public Property {
	private:
		ZeroFps *m_pkFps;
		StatusProperty* m_pkStatusProperty;
		ObjectManager *m_pkObjectMan;
		OpenAlSystem* m_pkAlSys;
		
		Vector3 m_kDir;
		float m_fChangeTime;	
		float m_fDirUpdate;
		float m_fRotateSpeed;
		float m_fWalkSpeed;
		bool m_bActive;
		float m_fHitTime;
		
		Object* m_pkPlayer;
		
//		Vector3 GetYawVector(float fAngleDeg);
		
		float BestYawTurnDir(float fStart, float fEnd, float fSpeed);

		Sound *walksound;		

	public:
		AdaptorSniper();
		~AdaptorSniper();
		void Touch(Object* pkObject);
		void Update();
};

Property* Create_AdaptorSniper();


#endif



