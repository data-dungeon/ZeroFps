#ifndef _ADAPTOR_SNIPER_H_
#define _ADAPTOR_SNIPER_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "statusproperty.h"
#include "common_x.h"

using namespace std;

class COMMON_API AdaptorSniper: public Property {
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
		Sound *walksound;		
		Sound *firesound;				
				
		vector<PropertyValues> GetPropertyValues();
		float BestYawTurnDir(float fStart, float fEnd, float fSpeed);

		void Fire(Vector3 kAim);

	public:
		AdaptorSniper();
		~AdaptorSniper();
		void Touch(Collision* pkCol);
		void Update();
		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);

		
};

COMMON_API Property* Create_AdaptorSniper();


#endif



