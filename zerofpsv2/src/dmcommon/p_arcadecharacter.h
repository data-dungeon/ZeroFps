#ifndef _P_ARCADECHARATER_H_
#define _P_ARCADECHARATER_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "mcommon_x.h"

#include "../zerofpsv2/engine/inputhandle.h"
#include "../zerofpsv2/engine_systems/propertys/p_tcs.h"

using namespace std;

class MCOMMON_API P_ArcadeCharacter : public Property
{
	private:
	
	public:
		float 		m_fSpeed;		
		bitset<6>	m_kActions;
		Vector3		m_kDir;
		Vector3		m_kAim;
		int			m_iTarget;
		Matrix4		m_kCameraRotation;
		
		P_ArcadeCharacter();
		~P_ArcadeCharacter();


		void Update();
		void Init();
		
		void Fire();
		
		void AutoAim();

		//void Save(ZFIoInterface* pkPackage);
		//void Load(ZFIoInterface* pkPackage);

};

MCOMMON_API Property* Create_P_ArcadeCharacter();

#endif
